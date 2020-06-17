/*******************************************************************************
  Serial Communication Interface Inter-Integrated Circuit (I2C) Library
  Instance Header File

  Company:
    Microchip Technology Inc.

  File Name:
    plib_i2c1_i2c_slave.h

  Summary:
    I2C PLIB Slave Header file

  Description:
    This file defines the interface to the I2C peripheral library. This
    library provides access to and control of the associated peripheral
    instance.
*******************************************************************************/
// DOM-IGNORE-BEGIN
/*******************************************************************************
* Copyright (C) 2019-2020 Microchip Technology Inc. and its subsidiaries.
*
* Subject to your compliance with these terms, you may use Microchip software
* and any derivatives exclusively with Microchip products. It is your
* responsibility to comply with third party license terms applicable to your
* use of third party software (including open source software) that may
* accompany Microchip software.
*
* THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
* EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
* WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
* PARTICULAR PURPOSE.
*
* IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
* INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
* WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
* BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
* FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
* ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
* THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
*******************************************************************************/
// DOM-IGNORE-END

#ifndef PLIB_I2C1_SLAVE_H
#define PLIB_I2C1_SLAVE_H

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************
/* This section lists the other files that are included in this file.
*/

#include "plib_i2c_slave_common.h"

// DOM-IGNORE-BEGIN
#ifdef __cplusplus // Provide C++ Compatibility

    extern "C" {

#endif
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Interface Routines
// *****************************************************************************
// *****************************************************************************

/*
 * The following functions make up the methods (set of possible operations) of
 * this interface.
 */

void I2C1_Initialize(void);
void I2C1_CallbackRegister(I2C_SLAVE_CALLBACK callback, uintptr_t contextHandle);
bool I2C1_IsBusy(void);
uint8_t I2C1_ReadByte(void);
void I2C1_WriteByte(uint8_t wrByte);
I2C_SLAVE_TRANSFER_DIR I2C1_TransferDirGet(void);
I2C_SLAVE_ACK_STATUS I2C1_LastByteAckStatusGet(void);
I2C_SLAVE_ERROR I2C1_ErrorGet(void);


// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility
}
#endif
// DOM-IGNORE-END

#endif /* PLIB_I2C1_SLAVE_H */