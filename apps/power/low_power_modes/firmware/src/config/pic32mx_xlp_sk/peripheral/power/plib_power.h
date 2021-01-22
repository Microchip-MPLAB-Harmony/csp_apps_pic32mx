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
    POWER_WAKEUP_SOURCE_DSMCLR = _DSWAKE_DSMCLR_MASK,

    POWER_WAKEUP_SOURCE_DSRTC = _DSWAKE_DSRTC_MASK,

    POWER_WAKEUP_SOURCE_DSWDT = _DSWAKE_DSWDT_MASK,

    POWER_WAKEUP_SOURCE_DSFLT = _DSWAKE_DSFLT_MASK,

    POWER_WAKEUP_SOURCE_DSINT0 = _DSWAKE_DSINT0_MASK,

} POWER_WAKEUP_SOURCE;

typedef enum
{
    POWER_DSGPR0,
    POWER_DSGPR1,
    POWER_DSGPR2,
    POWER_DSGPR3,
    POWER_DSGPR4,
    POWER_DSGPR5,
    POWER_DSGPR6,
    POWER_DSGPR7,
    POWER_DSGPR8,
    POWER_DSGPR9,
    POWER_DSGPR10,
    POWER_DSGPR11,
    POWER_DSGPR12,
    POWER_DSGPR13,
    POWER_DSGPR14,
    POWER_DSGPR15,
    POWER_DSGPR16,
    POWER_DSGPR17,
    POWER_DSGPR18,
    POWER_DSGPR19,
    POWER_DSGPR20,
    POWER_DSGPR21,
    POWER_DSGPR22,
    POWER_DSGPR23,
    POWER_DSGPR24,
    POWER_DSGPR25,
    POWER_DSGPR26,
    POWER_DSGPR27,
    POWER_DSGPR28,
    POWER_DSGPR29,
    POWER_DSGPR30,
    POWER_DSGPR31,
    POWER_DSGPR32,
} POWER_DSGPR;
// *****************************************************************************
// *****************************************************************************
// Section: Interface
// *****************************************************************************
// *****************************************************************************

void POWER_LowPowerModeEnter( POWER_LOW_POWER_MODE mode );
void POWER_Initialize( void );
POWER_WAKEUP_SOURCE POWER_WakeupSourceGet( void );
void POWER_ReleaseGPIO(void);
void POWER_WakeupSourceClear( POWER_WAKEUP_SOURCE wakeupSource );
void POWER_DSGPR_Write(POWER_DSGPR gprNumb, uint32_t gprValue);
uint32_t POWER_DSGPR_Read(POWER_DSGPR gprNumb);

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    }

#endif
// DOM-IGNORE-END

#endif /* PLIB_POWER_H */
