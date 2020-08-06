/*******************************************************************************
  SPI PLIB

  Company:
    Microchip Technology Inc.

  File Name:
    plib_spi1_slave.h

  Summary:
    SPI1 Slave PLIB Header File

  Description:
    This file has prototype of all the interfaces provided for particular
    SPI peripheral.

*******************************************************************************/

/*******************************************************************************
* Copyright (C) 2018-2019 Microchip Technology Inc. and its subsidiaries.
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

#ifndef PLIB_SPI1_SLAVE_H
#define PLIB_SPI1_SLAVE_H

#include "device.h"
#include "plib_spi_slave_common.h"

/* Provide C++ Compatibility */
#ifdef __cplusplus

    extern "C" {

#endif


/****************************** SPI1 Interface *********************************/

void SPI1_Initialize (void);
size_t SPI1_Read(void* pRdBuffer, size_t size);
size_t SPI1_Write(void* pWrBuffer, size_t size );
size_t SPI1_ReadCountGet(void);
size_t SPI1_ReadBufferSizeGet(void);
size_t SPI1_WriteBufferSizeGet(void);
void SPI1_CallbackRegister(SPI_SLAVE_CALLBACK callBack, uintptr_t context );
SPI_SLAVE_ERROR SPI1_ErrorGet(void);
bool SPI1_IsBusy(void);
void SPI1_Ready(void);

/* Provide C++ Compatibility */
#ifdef __cplusplus

    }

#endif

#endif // PLIB_SPI1_SLAVE_H
