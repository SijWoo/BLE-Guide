/***************************************************************************//**
 * @file
 * @brief Silabs Network Co-Processor (NCP) library USART driver
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
#include "ble-configuration.h"
#include "board_features.h"

#include "em_assert.h"
#include "em_device.h"
#include "uartdrv.h"
#include "em_core.h"
#include "ncp_usart.h"
#include "gpiointerrupt.h"
#include "sleep.h"

// temporary buffer for receiving data from UART
static uint8_t rxbuf[NCP_CMD_SIZE];
static uint32_t* cmd_header = (uint32_t*)rxbuf;
#if defined(NCP_DEEP_SLEEP_ENABLED)
static void ncp_enable_deep_sleep();
static volatile bool sleep_requested = false;
#endif
#if defined(NCP_HOST_WAKEUP_ENABLED)
static void ncp_enable_host_wakeup();
#endif
static volatile uint32_t cmd_len = 0;
static uint32_t timeout_reset = 0;
static volatile uint32_t timeout = 0;

static uint32_t ncp_usart_transmit(uint8_t* data, uint8_t len);
static void ncp_usart_receive_next();
static void uart_rx_callback(UARTDRV_Handle_t handle, Ecode_t transferStatus, uint8_t *data,
                             UARTDRV_Count_t transferCount);
static void uart_tx_callback(UARTDRV_Handle_t handle, Ecode_t transferStatus, uint8_t *data,
                             UARTDRV_Count_t transferCount);
void NCP_USART_IRQ_NAME(void);
void NCP_USART_TX_IRQ_NAME(void);

// UART driver instance handle
UARTDRV_HandleData_t handleData;
UARTDRV_Handle_t handle = &handleData;

// Define receive/transmit operation queues
DEFINE_BUF_QUEUE(EMDRV_UARTDRV_MAX_CONCURRENT_RX_BUFS, rxBufferQueueI0);
DEFINE_BUF_QUEUE(EMDRV_UARTDRV_MAX_CONCURRENT_TX_BUFS, txBufferQueueI0);

void ncp_usart_init()
{
  UARTDRV_InitUart_t initData;
  Ecode_t retVal;

  initData.port = NCP_USART_UART;
  initData.portLocationTx = BSP_UARTNCP_TX_LOC; // Location number for UART Tx pin.
  initData.portLocationRx = BSP_UARTNCP_RX_LOC; // Location number for UART Rx pin.
  initData.baudRate = HAL_UARTNCP_BAUD_RATE; // UART baud rate

  initData.stopBits = NCP_USART_STOPBITS; // Number of stop bits
  initData.parity = NCP_USART_PARITY; // Parity configuration
  initData.oversampling = NCP_USART_OVS; // Oversampling mode
  initData.mvdis = NCP_USART_MVDIS; // Majority Vote Disable for 16x, 8x

  initData.fcType = NCP_USART_FLOW_CONTROL_TYPE; // Flow control mode
  // Flow control pins should be only set when board-specific definitions exist
#if (HAL_UARTNCP_FLOW_CONTROL == HAL_USART_FLOW_CONTROL_HWUART)
  initData.ctsPort = BSP_UARTNCP_CTS_PORT; // CTS pin port number
  initData.ctsPin = BSP_UARTNCP_CTS_PIN;                     // CTS pin number
  initData.rtsPort = BSP_UARTNCP_RTS_PORT; // RTS pin port number
  initData.rtsPin = BSP_UARTNCP_RTS_PIN;                      // RTS pin number
#endif
  initData.rxQueue = (UARTDRV_Buffer_FifoQueue_t*)&rxBufferQueueI0;    // Receive operation queue
  initData.txQueue = (UARTDRV_Buffer_FifoQueue_t*)&txBufferQueueI0;    // Transmit operation queue
  
  initData.portLocationCts = BSP_UARTNCP_CTS_LOC;  // Location number for UART CTS pin.
  initData.portLocationRts = BSP_UARTNCP_RTS_LOC;  // Location number for UART RTS pin.

  retVal = UARTDRV_InitUart(handle, &initData);
  EFM_ASSERT(retVal == ECODE_EMDRV_UARTDRV_OK);
  timeout_reset = initData.baudRate / 64;
  ncp_set_transmit_callback(ncp_usart_transmit);
  ncp_usart_status_update();

#if defined(NCP_DEEP_SLEEP_ENABLED)
#if defined(NCP_USART_FLOW_CONTROL_ENABLED)
  // make sure RTS pin is in high state during sleep
  GPIO_PinModeSet(BSP_UARTNCP_RTS_PORT, BSP_UARTNCP_RTS_PIN, gpioModePushPull, 1);
#endif  // NCP_USART_FLOW_CONTROL_ENABLED
  ncp_enable_deep_sleep();
  //By default block sleep in EM2 when usart is enabled
  SLEEP_SleepBlockBegin(sleepEM2);
#endif  // NCP_DEEP_SLEEP_ENABLED
#if defined(NCP_HOST_WAKEUP_ENABLED)
  ncp_enable_host_wakeup();
#endif  // NCP_HOST_WAKEUP_ENABLED
  CORE_SetNvicRamTableHandler(NCP_USART_IRQn, (void *)NCP_USART_IRQ_NAME);
  CORE_SetNvicRamTableHandler(NCP_USART_TX_IRQn, (void *)NCP_USART_TX_IRQ_NAME);
  NVIC_ClearPendingIRQ(NCP_USART_IRQn);           // Clear pending RX interrupt flag in NVIC
  NVIC_ClearPendingIRQ(NCP_USART_TX_IRQn);        // Clear pending TX interrupt flag in NVIC
  NVIC_EnableIRQ(NCP_USART_IRQn);
  NVIC_EnableIRQ(NCP_USART_TX_IRQn);

  //Setup RX timeout
  handle->peripheral.uart->TIMECMP1 = USART_TIMECMP1_RESTARTEN
                                      | USART_TIMECMP1_TSTOP_RXACT
                                      | USART_TIMECMP1_TSTART_RXEOF
                                      | (0x30 << _USART_TIMECMP1_TCMPVAL_SHIFT);
  //IRQ
  USART_IntClear(handle->peripheral.uart, _USART_IF_MASK);       // Clear any USART interrupt flags
  USART_IntEnable(handle->peripheral.uart, USART_IF_TXIDLE | USART_IF_TCMP1);
  /* RX the next command header*/
  UARTDRV_Receive(handle, rxbuf, NCP_CMD_SIZE, uart_rx_callback);
  timeout = timeout_reset;
}

static void ncp_usart_receive_next()
{
  gecko_external_signal(NCP_USART_UPDATE_SIGNAL);
  //stop the timer
  handle->peripheral.uart->TIMECMP1 &= ~_USART_TIMECMP1_TSTART_MASK;
  handle->peripheral.uart->TIMECMP1 |= USART_TIMECMP1_TSTART_RXEOF;
  USART_IntClear(handle->peripheral.uart, USART_IF_TCMP1);
  //abort receive operation
  UARTDRV_Abort(handle, uartdrvAbortReceive);
  //enqueue next receive buffer
  UARTDRV_Receive(handle, rxbuf, NCP_CMD_SIZE, uart_rx_callback);
  timeout = timeout_reset;
}

void NCP_USART_IRQ_NAME()
{
  if (handle->peripheral.uart->IF & USART_IF_TCMP1) {
    /* RX timeout, stop transfer and handle what we got in buffer */
    USART_IntClear(handle->peripheral.uart, USART_IF_TCMP1);
    if (timeout > 0) {
      timeout--;
      uint8_t* buffer = NULL;
      uint32_t received = 0;
      uint32_t remaining = 0;
      UARTDRV_GetReceiveStatus(handle, &buffer, &received, &remaining);
      if (buffer && received >= BGLIB_MSG_HEADER_LEN) {
        cmd_len = BGLIB_MSG_LEN(*cmd_header) + BGLIB_MSG_HEADER_LEN;
        if (received >= cmd_len) {
          EFM_ASSERT(received <= NCP_CMD_SIZE);
          //current command is received
          ncp_receive_command(buffer, cmd_len);
          ncp_usart_receive_next();
        }
      }
    } else {
      gecko_external_signal(NCP_USART_TIMEOUT_SIGNAL);
      ncp_usart_receive_next();
    }
  }
}

void NCP_USART_TX_IRQ_NAME()
{
  if (handle->peripheral.uart->IF & USART_IF_TXIDLE) {
    gecko_external_signal(NCP_USART_UPDATE_SIGNAL);
    USART_IntClear(handle->peripheral.uart, USART_IF_TXIDLE);
  }
}

static void uart_rx_callback(UARTDRV_Handle_t handle, Ecode_t transferStatus, uint8_t *data, UARTDRV_Count_t transferCount)
{
  //let RX timeout handle it
}

static void uart_tx_callback(UARTDRV_Handle_t handle, Ecode_t transferStatus, uint8_t *data, UARTDRV_Count_t transferCount)
{
  ncp_transmit_dequeue(data, transferCount);
}

static uint32_t ncp_usart_transmit(uint8_t* data, uint8_t len)
{
  ncp_usart_status_update();
  return UARTDRV_Transmit(handle, data, len, uart_tx_callback);
}

#if defined(NCP_DEEP_SLEEP_ENABLED)
static void ncp_manage_wakeup(uint8_t pin)
{
  if (GPIO_PinInGet(NCP_WAKEUP_PORT, NCP_WAKEUP_PIN) == NCP_WAKEUP_POLARITY) {
    sleep_requested = false;
    gecko_external_signal(NCP_USART_WAKEUP_SIGNAL);
  } else {
    sleep_requested = true;
  }
  gecko_external_signal(NCP_USART_UPDATE_SIGNAL);
}

static void ncp_enable_deep_sleep()
{
  GPIO_PinModeSet(NCP_WAKEUP_PORT, NCP_WAKEUP_PIN, gpioModeInput, 0);
  sleep_requested = (GPIO_PinInGet(NCP_WAKEUP_PORT, NCP_WAKEUP_PIN) == NCP_WAKEUP_POLARITY) ? false : true;
  GPIOINT_Init();
  GPIOINT_CallbackRegister(NCP_WAKEUP_PIN, ncp_manage_wakeup);
  GPIO_ExtIntConfig(NCP_WAKEUP_PORT, NCP_WAKEUP_PIN, NCP_WAKEUP_PIN, true, true, true);
}
#endif

#if defined(NCP_HOST_WAKEUP_ENABLED)
static void ncp_enable_host_wakeup()
{
  GPIO_PinModeSet(NCP_HOST_WAKEUP_PORT, NCP_HOST_WAKEUP_PIN, gpioModePushPull, NCP_HOST_WAKEUP_POLARITY ? 0 : 1);
  if (NCP_HOST_WAKEUP_POLARITY) {
    GPIO_PinOutSet(NCP_HOST_WAKEUP_PORT, NCP_HOST_WAKEUP_PIN);
  } else {
    GPIO_PinOutClear(NCP_HOST_WAKEUP_PORT, NCP_HOST_WAKEUP_PIN);
  }
}
#endif

void ncp_usart_status_update()
{
#if defined(NCP_HOST_WAKEUP_ENABLED)
  if (ncp_transmit_queue_len() > 0
      || (UARTDRV_GetTransmitDepth(handle) != 0)) {
    NCP_HOST_WAKEUP_POLARITY ? GPIO_PinOutSet(NCP_HOST_WAKEUP_PORT, NCP_HOST_WAKEUP_PIN)
    : GPIO_PinOutClear(NCP_HOST_WAKEUP_PORT, NCP_HOST_WAKEUP_PIN);
  } else {
    NCP_HOST_WAKEUP_POLARITY ? GPIO_PinOutClear(NCP_HOST_WAKEUP_PORT, NCP_HOST_WAKEUP_PIN)
    : GPIO_PinOutSet(NCP_HOST_WAKEUP_PORT, NCP_HOST_WAKEUP_PIN);
  }
#endif

#if defined(NCP_DEEP_SLEEP_ENABLED)

  static volatile bool sleepStatus = false;
  uint32_t status = UARTDRV_GetPeripheralStatus(handle);
  if (sleep_requested
      && (UARTDRV_GetTransmitDepth(handle) == 0)
      && (status & UARTDRV_STATUS_TXIDLE)
      && ((status & UARTDRV_STATUS_RXDATAV) == 0)
      && ((status & _USART_STATUS_TXBUFCNT_MASK) == 0)
      && ncp_transmit_queue_len() == 0) {
    if (sleepStatus != true) {
      SLEEP_SleepBlockEnd(sleepEM2);
#if defined(NCP_USART_FLOW_CONTROL_ENABLED)
#if defined(USART_ROUTEPEN_RTSPEN)
      handle->peripheral.uart->ROUTEPEN &= ~USART_ROUTEPEN_RTSPEN;
#elif defined(GPIO_USART_ROUTEEN_RTSPEN)
      GPIO->USARTROUTE_CLR[handle->uartNum].ROUTEEN = GPIO_USART_ROUTEEN_RTSPEN;
#endif
#endif
    }
    sleepStatus = true;
  } else {
    if (sleepStatus != false) {
      SLEEP_SleepBlockBegin(sleepEM2);
#if defined(NCP_USART_FLOW_CONTROL_ENABLED)
#if defined(USART_ROUTEPEN_RTSPEN)
      handle->peripheral.uart->ROUTEPEN |= USART_ROUTEPEN_RTSPEN;
#elif defined(GPIO_USART_ROUTEEN_RTSPEN)
      GPIO->USARTROUTE_SET[handle->uartNum].ROUTEEN = GPIO_USART_ROUTEEN_RTSPEN;
#endif
#endif
    }
    sleepStatus = false;
  }
#endif
}

bool ncp_handle_event(struct gecko_cmd_packet *evt)
{
  bool evt_handled = false;
  switch (BGLIB_MSG_ID(evt->header)) {
    case gecko_evt_system_external_signal_id:
      if (evt->data.evt_system_external_signal.extsignals & NCP_USART_WAKEUP_SIGNAL) {
        gecko_send_system_awake();
        evt_handled = true;
      }
      if (evt->data.evt_system_external_signal.extsignals & NCP_USART_TIMEOUT_SIGNAL) {
        // NCP command receive timeout, sending system error
        gecko_send_system_error(bg_err_command_incomplete, 0, NULL);
        evt_handled = true;
      }
      if (evt->data.evt_system_external_signal.extsignals & NCP_USART_UPDATE_SIGNAL) {
        ncp_usart_status_update();
        evt_handled = true;
      }
      break;
    default:
      break;
  }
  return evt_handled;
}