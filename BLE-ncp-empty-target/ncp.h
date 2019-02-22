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

#ifndef NCP_H_
#define NCP_H_

#include <stdint.h>
#include <stdbool.h>
#include "ncp_gecko.h"

// This is defining the maximum length of the buffer for holding
// NCP commands, responses and events include header
#ifndef NCP_CMD_SIZE
#define NCP_CMD_SIZE                 360
#endif

// Length of TX queue
#ifndef NCP_TX_QUEUE_LEN
#define NCP_TX_QUEUE_LEN             42
#endif
// Length of the area in the TX queue reserved
// for responses only (no events buffered here)
#define NCP_TX_QUEUE_RESERVED_LEN    12
// Size of each NCP buffer
#ifndef NCP_BUF_SIZE
#define NCP_BUF_SIZE                 30
#endif

/***************************************************************************//**
 * @brief
 *   Set callback function for transmit.
 *
 * @details
 *   This function should be called by actual NCP connection drivers to set
 *   function pointer to their transmit function. The callback function set
 *   by this API will be used to transmit NCP responses and events.
 *
 * @param[in] transmit_callback
 *   Callback function pointer.
 *
 ******************************************************************************/
void ncp_set_transmit_callback(uint32_t (*transmit_callback)(uint8_t* data, uint8_t len));

/***************************************************************************//**
 * @brief
 *   Get the number of buffers currently queued in NCP transmit queue.
 *
 * @details
 *   This function will return the number of buffers currently queued in NCP
 *   transmit queue.
 *
 * @return
 *   Number of buffers in TX queue.
 *
 ******************************************************************************/
uint32_t ncp_transmit_queue_len();

/***************************************************************************//**
 * @brief
 *   This function should be called in application's main loop.
 *
 * @details
 *   This function will check NCP receive queue and if a command is received it
 *   will forward it to stack to handle.
 *
 ******************************************************************************/
void ncp_handle_command();

/***************************************************************************//**
 * @brief
 *   Write NCP command data to handle.
 *
 * @details
 *   This function will write a complete NCP command, caller should ensure
 *   full NCP command is received.
 *
 * @param[in] data
 *   Pointer to byte stream data received.
 *
 * @param[in] len
 *   Length of the byte stream data received.
 *
 ******************************************************************************/
void ncp_receive_command(uint8_t* data, uint32_t len);

/***************************************************************************//**
 * @brief
 * Called when a user command (Message ID: gecko_cmd_user_message_to_target_id)
 * is received.
 *
 * @details
 * Implement this function if the BGAPI protocol is extended at application layer
 * between the host and target for data exchange. At the end of command handling,
 * a response to this command must be sent to the host. Use
 * gecko_send_rsp_user_message_to_target(uint16_t, uint8_t, uint8_t*) to send the
 * response.
 *
 * The target can also initiate the communication by sending event messages
 * (Message ID: gecko_evt_user_message_to_host_id) to the host by using API
 * gecko_send_evt_user_message_to_host(uint8_t, uint8_t*).
 *
 * Notice that events should not be sent in the context of user command handling.
 *
 * @param[in] data
 *   Pointer to the command message.
 *
 ******************************************************************************/
void handle_user_command(uint8_t* data);

/***************************************************************************//**
 * @brief
 *   Write NCP data stream to handle.
 *
 * @details
 *   This function will write NCP data stream.
 *
 * @param[in] data
 *   Pointer to byte stream data received.
 *
 * @param[in] len
 *   Length of the byte stream data received.
 *
 * @return
 *   The number of bytes accepted. If the data length exceeds single command
 *   length, this will return the length of command and remaining data should be
 *   given to this API after receiving the response of current command which means
 *   in the next loop
 *
 ******************************************************************************/
uint32_t ncp_receive(uint8_t* data, uint32_t len);

/***************************************************************************//**
 * @brief
 *   Get the amount of bytes current NCP command expecting.
 *
 * @details
 *   This function will calculate the amount of remaining bytes NCP data stream
 *   should receive for current command.
 *
 * @return
 *   Remaining number of bytes expected. If commander header is not received yet
 *   this will return the length of command header
 *
 ******************************************************************************/
uint32_t ncp_calc_expecting();

/***************************************************************************//**
 * @brief
 *   Get the status of current NCP command.
 *
 * @details
 *   This function will return true if a full NCP command is received.
 *
 * @return
 *   True if full command is received, otherwise False
 *
 ******************************************************************************/
bool ncp_command_received();

/***************************************************************************//**
 * @brief
 *   This function appends response or event packets to TX queue.
 *
 * @details
 *   In case TX queue is full, this function will simply drop the event.
 *
 * @param[in] evt
 *   Pointer to the event received from the Bluetooth stack.
 *
 * @return
 *   True if the event or response is queued, otherwise False
 *
 ******************************************************************************/
bool ncp_transmit_enqueue(struct gecko_cmd_packet *evt);

/***************************************************************************//**
 * @brief
 *   This function removes already sent packets from TX queue.
 *
 * @details
 *   This function should be called after transportation layer received the
 *   confirmation of data is actually sent out.
 *
 * @param[in] data
 *   Pointer to the buffer already transferred.
 *
 * @param[in] len
 *   length of the buffer already transferred.
 *
 ******************************************************************************/
void ncp_transmit_dequeue(uint8_t* data, uint32_t len);

/***************************************************************************//**
 * @brief
 *   This function should be called in application's main loop after all the
 *   received events are handled or enqueued.
 *
 * @details
 *   This function starts transmitting of enqueued events in TX queue.
 *
 ******************************************************************************/
void ncp_transmit();

#endif /* NCP_H_ */
