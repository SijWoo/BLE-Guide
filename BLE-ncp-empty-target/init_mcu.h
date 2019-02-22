/***************************************************************************//**
 * @file
 * @brief init_mcu.h
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

#ifndef INIT_MCU_H
#define INIT_MCU_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 * If Studio is allowed to use the CTUNE value from EEPROM on board
 * it will store it in the User page memory on the MFG_CTUNE_ADDR address.
 * If Studio is not allowed to store this value on that address
 * for example user already storing a value there, then the MFG_CTUNE_EN
 * must be set to 0, to avoid wrong value read out.
 */

//Enable this to use CTUNE value form User page (0 DISABLE , 1 ENABLE)
#define MFG_CTUNE_EN   1
//Address for CTUNE in User page
#define MFG_CTUNE_ADDR 0x0FE00100UL
//Value of the CTUNE in User page
#define MFG_CTUNE_VAL  (*((uint16_t *) (MFG_CTUNE_ADDR)))

void initMcu(void);

#ifdef __cplusplus
}
#endif

#endif // INIT_MCU_H
