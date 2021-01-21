/*******************************************************************************
  Power PLIB

  Company
    Microchip Technology Inc.

  File Name
    plib_power.h

  Summary
    Power PLIB Header File.

  Description
    This file defines the interface to the DSCTRL peripheral library.
    This library provides access to and control of the associated Resets.

  Remarks:
    None.

*******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
* Copyright (C) 2019 Microchip Technology Inc. and its subsidiaries.
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

#ifndef PLIB_POWER_H      // Guards against multiple inclusion
#define PLIB_POWER_H

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <stdbool.h>
#include <stddef.h>
#include "device.h"

// DOM-IGNORE-BEGIN
#ifdef __cplusplus // Provide C++ Compatibility

    extern "C" {

#endif
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Data Types
// *****************************************************************************
// *****************************************************************************

typedef enum {

    LOW_POWER_IDLE_MODE,
    LOW_POWER_SLEEP_MODE,
    LOW_POWER_DREAM_MODE,
    LOW_POWER_DEEP_SLEEP_MODE,
} POWER_LOW_POWER_MODE;

typedef enum
{
    POWER_DS_WAKEUP_SOURCE_DSMCLR = _DSWAKE_DSMCLR_MASK,

    POWER_DS_WAKEUP_SOURCE_DSRTC = _DSWAKE_DSRTC_MASK,

    POWER_DS_WAKEUP_SOURCE_DSWDT = _DSWAKE_DSWDT_MASK,

    POWER_DS_WAKEUP_SOURCE_DSFLT = _DSWAKE_DSFLT_MASK,

    POWER_DS_WAKEUP_SOURCE_DSINT0 = _DSWAKE_DSINT0_MASK,

} POWER_DS_WAKEUP_SOURCE;

typedef enum
{
    POWER_DS_GPR_1,
    POWER_DS_GPR_2,
    POWER_DS_GPR_3,
    POWER_DS_GPR_4,
    POWER_DS_GPR_5,
    POWER_DS_GPR_6,
    POWER_DS_GPR_7,
    POWER_DS_GPR_8,
    POWER_DS_GPR_9,
    POWER_DS_GPR_10,
    POWER_DS_GPR_11,
    POWER_DS_GPR_12,
    POWER_DS_GPR_13,
    POWER_DS_GPR_14,
    POWER_DS_GPR_15,
    POWER_DS_GPR_16,
    POWER_DS_GPR_17,
    POWER_DS_GPR_18,
    POWER_DS_GPR_19,
    POWER_DS_GPR_20,
    POWER_DS_GPR_21,
    POWER_DS_GPR_22,
    POWER_DS_GPR_23,
    POWER_DS_GPR_24,
    POWER_DS_GPR_25,
    POWER_DS_GPR_26,
    POWER_DS_GPR_27,
    POWER_DS_GPR_28,
    POWER_DS_GPR_29,
    POWER_DS_GPR_30,
    POWER_DS_GPR_31,
    POWER_DS_GPR_32,
} POWER_DS_GPR;
// *****************************************************************************
// *****************************************************************************
// Section: Interface
// *****************************************************************************
// *****************************************************************************

void POWER_LowPowerModeEnter( POWER_LOW_POWER_MODE mode );
void POWER_Initialize( void );
POWER_DS_WAKEUP_SOURCE POWER_DS_WakeupSourceGet( void );
void POWER_DS_SoftwareRestore(void);
void POWER_DS_WakeupSourceClear( POWER_DS_WAKEUP_SOURCE wakeupSource );
void POWER_DS_GPR0Write(uint32_t sema1Value);
uint32_t POWER_DS_GPR0Read(void);
void POWER_DS_GPRnWrite(POWER_DS_GPR xsema, uint32_t xsemaValue);
uint32_t POWER_DS_GPRnRead(POWER_DS_GPR xsema);
// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    }

#endif
// DOM-IGNORE-END

#endif /* PLIB_POWER_H */
