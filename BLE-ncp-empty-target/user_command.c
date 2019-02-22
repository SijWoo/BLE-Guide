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

#include "ncp_gecko.h"

/**
 * Default implementation to user command handling.
 *
 * @param payload the data payload
 */
void handle_user_command(const uint8_t* data)
{
  gecko_send_rsp_user_message_to_target(bg_err_not_implemented, 0, NULL);
}
