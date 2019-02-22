/***************************************************************************//**
 * @file
 * @brief Silabs Network Co-Processor (NCP) library
 * This library allows customers create applications work in NCP mode.
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/
#include <string.h>
#include "em_assert.h"
#include "em_core.h"
#include "ncp.h"

typedef struct {
  uint32_t len;
  uint8_t data[NCP_BUF_SIZE];
} ncp_buf;

typedef struct {
  volatile uint16_t write;
  volatile uint16_t read;
  volatile uint16_t end;
  volatile uint16_t queued;
  volatile uint16_t used;
  const uint16_t size;
  ncp_buf  fifo[];
} ncp_queue;

#define DEFINE_NCP_QUEUE(qSize, qName) \
  typedef struct {                     \
    volatile uint16_t write;           \
    volatile uint16_t read;            \
    volatile uint16_t end;             \
    volatile uint16_t queued;          \
    volatile uint16_t used;            \
    const uint16_t size;               \
    ncp_buf fifo[qSize];               \
  } _##qName;                          \
  static volatile _##qName qName =     \
  {                                    \
    .write = 0,                        \
    .read = 0,                         \
    .end = 0,                          \
    .queued = 0,                       \
    .used = 0,                         \
    .size = qSize,                     \
  }

typedef struct {
  volatile uint16_t remaining;
  volatile uint16_t len;
  volatile bool receiving_header;
  // RX queue is only one buffer because NCP protocol only
  // allows a single command at a time
  uint8_t data[NCP_CMD_SIZE];
} rx_queue_t;

static volatile rx_queue_t rx_queue =
{
  .len = 0,
  .receiving_header = true,
  .remaining = BGLIB_MSG_HEADER_LEN
};

DEFINE_NCP_QUEUE(NCP_TX_QUEUE_LEN, tx_queue_t);
static ncp_queue* tx_queue = (ncp_queue*)&tx_queue_t;

static uint32_t (*transmit_callback)(uint8_t* data, uint8_t len) = 0;

//Enqueue accepted data into RX queue
static void rx_enqueue(uint8_t* data, uint32_t len);
//Reset the RX queue
static void rx_queue_reset();
//Reset the TX queue
static void tx_queue_reset();

static bool ncp_enqueue(ncp_queue* queue, uint8_t* buf, uint32_t len, uint8_t rsp_not_evt);
//Get the first item from queue, the item will stay in the queue
static ncp_buf* ncp_queue_read(ncp_queue* queue);
//Confirm the last read action from queue, the item will stay in the queue
static void ncp_queue_confirm_read(ncp_queue* queue);
//Remove the first item from queue
static void ncp_dequeue(ncp_queue* queue);

static void rx_queue_reset()
{
  rx_queue.receiving_header = true;
  rx_queue.remaining = BGLIB_MSG_HEADER_LEN;
  rx_queue.len = 0;
}

static void tx_queue_reset()
{
  tx_queue->write = 0;
  tx_queue->read = 0;
  tx_queue->end = 0;
  tx_queue->queued = 0;
  tx_queue->used = 0;
  memset((void*)tx_queue->fifo, 0, sizeof(ncp_buf) * NCP_TX_QUEUE_LEN);
}

void ncp_set_transmit_callback(uint32_t (*_transmit_callback)(uint8_t* data, uint8_t len))
{
  EFM_ASSERT(_transmit_callback != NULL);
  transmit_callback = _transmit_callback;
  rx_queue_reset();
  tx_queue_reset();
}

void ncp_handle_command()
{
  struct gecko_cmd_packet * rsp;

  if (ncp_command_received()) {
    uint32_t *cmd_header = (uint32_t *)rx_queue.data;
    if (BGLIB_MSG_ID(*cmd_header) == gecko_cmd_user_message_to_target_id) {
      // call user command handler:
      handle_user_command((uint8_t*)&rx_queue.data[BGLIB_MSG_HEADER_LEN]);
    } else {
      gecko_handle_command(*cmd_header, (void*)&rx_queue.data[BGLIB_MSG_HEADER_LEN]);
    }
    rsp = (struct gecko_cmd_packet *)gecko_rsp_msg_buf;
    if (!ncp_enqueue(tx_queue, (uint8_t*)rsp, BGLIB_MSG_LEN(rsp->header) + BGLIB_MSG_HEADER_LEN, 1)) {
      EFM_ASSERT(false);
      // TX queue is full, should never reach here
    }
    //reset the buffer after the command is handled
    rx_queue_reset();
  }
  ncp_transmit();
}

void ncp_receive_command(uint8_t* data, uint32_t len)
{
  memcpy((void*)rx_queue.data, data, len);
  rx_queue.len = len;
  rx_queue.receiving_header = false;
  rx_queue.remaining = 0;
}

uint32_t ncp_receive(uint8_t* data, uint32_t len)
{
  uint32_t received = 0;

  if (data == NULL || len == 0) {
    return received;
  }

  if (ncp_command_received()) {
    return received; //wait until current command is handled
  }
  if (len <= rx_queue.remaining) {
    rx_enqueue(data, len);
    return len;
  } else {
    if (rx_queue.receiving_header) {
      received += rx_queue.remaining;
      rx_enqueue(data, rx_queue.remaining);
    }
    // after receiving header and there is still more data
    // try to consume them as command parameters
    rx_enqueue(data + received, rx_queue.remaining);
    received += rx_queue.remaining;
    return received;
  }
}

uint32_t ncp_calc_expecting()
{
  return (rx_queue.remaining > 0) ? rx_queue.remaining : BGLIB_MSG_HEADER_LEN;
}

bool ncp_transmit_enqueue(struct gecko_cmd_packet *evt)
{
  if (evt == NULL) {
    return false;
  }
  return ncp_enqueue(tx_queue, (uint8_t*)evt, BGLIB_MSG_LEN(evt->header) + BGLIB_MSG_HEADER_LEN, 0);
}

void ncp_transmit_dequeue(uint8_t* data, uint32_t len)
{
  ncp_dequeue(tx_queue);
}

uint32_t ncp_transmit_queue_len()
{
  return tx_queue->queued;
}

void ncp_transmit()
{
  while (tx_queue->queued > 0) {
    ncp_buf* buf = ncp_queue_read(tx_queue);
    if (transmit_callback(buf->data, buf->len)) {
      break;
    }
    ncp_queue_confirm_read(tx_queue);
  }
}

static void rx_enqueue(uint8_t* data, uint32_t len)
{
  memcpy((void*)&rx_queue.data[rx_queue.len], data, len);
  rx_queue.len += len;
  rx_queue.receiving_header = rx_queue.len < BGLIB_MSG_HEADER_LEN ? true : false;
  if (rx_queue.receiving_header) {
    rx_queue.remaining = BGLIB_MSG_HEADER_LEN - rx_queue.len;
  } else {
    uint32_t *cmd_header = (uint32_t*)rx_queue.data;
    uint32_t cmd_len = BGLIB_MSG_LEN(*cmd_header) + BGLIB_MSG_HEADER_LEN;
    rx_queue.remaining = cmd_len - rx_queue.len;
  }
}

bool ncp_command_received()
{
  if (rx_queue.receiving_header == false && rx_queue.remaining == 0) {
    return true;
  } else {
    return false;
  }
}

static bool ncp_enqueue(ncp_queue* queue, uint8_t* buf, uint32_t len, uint8_t rsp_not_evt)
{
  uint16_t available_size = queue->size - (rsp_not_evt ? 0 : NCP_TX_QUEUE_RESERVED_LEN);

  CORE_DECLARE_IRQ_STATE;
  CORE_ENTER_ATOMIC();
  if (queue->used >= available_size) {
    CORE_EXIT_ATOMIC();
    return false;
  }
  if (len <= NCP_BUF_SIZE) {
    //data fit into single TX buffer
    memcpy((void*)queue->fifo[queue->write].data,
           (const void*)buf, len);
    queue->fifo[queue->write].len = len;
    queue->write = (queue->write + 1) % queue->size;
    queue->queued++;
    queue->used++;
    CORE_EXIT_ATOMIC();
    return true;
  } else {
    uint32_t space = (available_size - queue->used) * NCP_BUF_SIZE;
    if (len > space) {
      //not enough space in the queue
      CORE_EXIT_ATOMIC();
      return false;
    } else {
      uint32_t left = len;
      uint8_t* src = buf;
      while (left > 0) {
        uint32_t count = (left > NCP_BUF_SIZE) ? NCP_BUF_SIZE : left;
        memcpy((void*)queue->fifo[queue->write].data,
               (const void*)src, count);
        queue->fifo[queue->write].len = count;
        queue->write = (queue->write + 1) % queue->size;
        queue->queued++;
        queue->used++;
        src += count;
        left -= count;
      }
      CORE_EXIT_ATOMIC();
      return true;
    }
  }
}

static ncp_buf* ncp_queue_read(ncp_queue* queue)
{
  CORE_DECLARE_IRQ_STATE;
  CORE_ENTER_ATOMIC();
  if (queue->queued == 0) {
    CORE_EXIT_ATOMIC();
    return NULL;
  }
  ncp_buf* buffer = &queue->fifo[queue->read];
  CORE_EXIT_ATOMIC();
  return buffer;
}

static void ncp_queue_confirm_read(ncp_queue* queue)
{
  CORE_DECLARE_IRQ_STATE;
  CORE_ENTER_ATOMIC();
  queue->read = (queue->read + 1) % queue->size;
  queue->queued--;
  CORE_EXIT_ATOMIC();
}

static void ncp_dequeue(ncp_queue* queue)
{
  CORE_DECLARE_IRQ_STATE;
  CORE_ENTER_ATOMIC();
  queue->end = (queue->end + 1) % queue->size;
  queue->used--;
  CORE_EXIT_ATOMIC();
}
