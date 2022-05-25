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
    LOW_POWER_EXTREME_DEEP_SLEEP_MODE,
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
    POWER_DS_GPR0,
    POWER_DS_GPR1,
    POWER_DS_GPR2,
    POWER_DS_GPR3,
    POWER_DS_GPR4,
    POWER_DS_GPR5,
    POWER_DS_GPR6,
    POWER_DS_GPR7,
    POWER_DS_GPR8,
    POWER_DS_GPR9,
    POWER_DS_GPR10,
    POWER_DS_GPR11,
    POWER_DS_GPR12,
    POWER_DS_GPR13,
    POWER_DS_GPR14,
    POWER_DS_GPR15,
    POWER_DS_GPR16,
    POWER_DS_GPR17,
    POWER_DS_GPR18,
    POWER_DS_GPR19,
    POWER_DS_GPR20,
    POWER_DS_GPR21,
    POWER_DS_GPR22,
    POWER_DS_GPR23,
    POWER_DS_GPR24,
    POWER_DS_GPR25,
    POWER_DS_GPR26,
    POWER_DS_GPR27,
    POWER_DS_GPR28,
    POWER_DS_GPR29,
    POWER_DS_GPR30,
    POWER_DS_GPR31,
    POWER_DS_GPR32,
} POWER_DS_GPR;
// *****************************************************************************
// *****************************************************************************
// Section: Interface
// *****************************************************************************
// *****************************************************************************

void POWER_LowPowerModeEnter( POWER_LOW_POWER_MODE mode );
void POWER_Initialize( void );
POWER_DS_WAKEUP_SOURCE POWER_DS_WakeupSourceGet( void );
void POWER_DS_ReleaseGPIO(void);
void POWER_DS_WakeupSourceClear( POWER_DS_WAKEUP_SOURCE wakeupSource );
void POWER_DS_GPR_Enable(void);
void POWER_DS_GPR_Disable(void);
void POWER_DS_RTCC_Enable(void);
void POWER_DS_RTCC_Disable(void);
void POWER_DS_RTCC_WakeupEnable(void);
void POWER_DS_RTCC_WakeupDisable(void);
void POWER_DS_GPR_Write(POWER_DS_GPR gprNumb, uint32_t gprValue);
uint32_t POWER_DS_GPR_Read(POWER_DS_GPR gprNumb);

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    }

#endif
// DOM-IGNORE-END

#endif /* PLIB_POWER_H */
