/*******************************************************************************
  SPI PLIB

  Company:
    Microchip Technology Inc.

  File Name:
    plib_spi2_slave.c

  Summary:
    SPI2 Slave Source File

  Description:
    This file has implementation of all the interfaces provided for particular
    SPI peripheral.

*******************************************************************************/

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
#include "plib_spi2_slave.h"
#include "peripheral/gpio/plib_gpio.h"
#include <string.h>
#include "interrupts.h"
// *****************************************************************************
// *****************************************************************************
// Section: SPI2 Slave Implementation
// *****************************************************************************
// *****************************************************************************
#define SPI2_BUSY_PIN                    GPIO_PIN_RB11
#define SPI2_CS_PIN                      GPIO_PIN_RG9


#define SPI2_READ_BUFFER_SIZE            256
#define SPI2_WRITE_BUFFER_SIZE           256

volatile static uint8_t SPI2_ReadBuffer[SPI2_READ_BUFFER_SIZE];
volatile static uint8_t SPI2_WriteBuffer[SPI2_WRITE_BUFFER_SIZE];


/* Global object to save SPI Exchange related data */
volatile static SPI_SLAVE_OBJECT spi2Obj;

#define SPI2_CON_CKP                        (0UL << _SPI2CON_CKP_POSITION)
#define SPI2_CON_CKE                        (1UL << _SPI2CON_CKE_POSITION)
#define SPI2_CON_MODE_32_MODE_16            (0UL << _SPI2CON_MODE16_POSITION)
#define SPI2_CON_ENHBUF                     (1UL << _SPI2CON_ENHBUF_POSITION)
#define SPI2_CON_STXISEL                    (3UL << _SPI2CON_STXISEL_POSITION)
#define SPI2_CON_SRXISEL                    (1UL << _SPI2CON_SRXISEL_POSITION)
#define SPI2_CON_SSEN                       (1UL << _SPI2CON_SSEN_POSITION)

#define SPI2_ENABLE_RX_INT()                IEC1SET = 0x400000
#define SPI2_CLEAR_RX_INT_FLAG()            IFS1CLR = 0x400000

#define SPI2_DISABLE_TX_INT()               IEC1CLR = 0x800000
#define SPI2_ENABLE_TX_INT()                IEC1SET = 0x800000
#define SPI2_CLEAR_TX_INT_FLAG()            IFS1CLR = 0x800000

#define SPI2_ENABLE_ERR_INT()               IEC1SET = 0x200000
#define SPI2_CLEAR_ERR_INT_FLAG()           IFS1CLR = 0x200000

/* Forward declarations */
static void SPI2_CS_Handler(GPIO_PIN pin, uintptr_t context);

static void mem_copy(volatile void* pDst, volatile void* pSrc, uint32_t nBytes)
{
    volatile uint8_t* pSource = (volatile uint8_t*)pSrc;
    volatile uint8_t* pDest = (volatile uint8_t*)pDst;

    for (uint32_t i = 0U; i < nBytes; i++)
    {
        pDest[i] = pSource[i];
    }
}

void SPI2_Initialize ( void )
{
    /* Disable SPI2 Interrupts */
    IEC1CLR = 0x200000;
    IEC1CLR = 0x400000;
    IEC1CLR = 0x800000;

    /* STOP and Reset the SPI */
    SPI2CON = 0;

    /* Clear SPI2 Interrupt flags */
    IFS1CLR = 0x200000;
    IFS1CLR = 0x400000;
    IFS1CLR = 0x800000;

    /* CLear the receiver overflow error flag */
    SPI2STATCLR = _SPI2STAT_SPIROV_MASK;

    /*
    SRXISEL = 1 (Receive buffer is not empty)
    STXISEL = 3 (Transmit buffer is not full)
    MSTEN = 0
    CKP = 0
    CKE = 1
    MODE< 32,16 > = 0
    ENHBUF = 1
    */

    SPI2CONSET = (SPI2_CON_ENHBUF | SPI2_CON_MODE_32_MODE_16 | SPI2_CON_CKE | SPI2_CON_CKP | SPI2_CON_SSEN | SPI2_CON_STXISEL | SPI2_CON_SRXISEL);

    /* Enable generation of interrupt on receiver overflow */
    SPI2CON2SET = _SPI2CON2_SPIROVEN_MASK;

    spi2Obj.rdInIndex = 0;
    spi2Obj.wrOutIndex = 0;
    spi2Obj.nWrBytes = 0;
    spi2Obj.errorStatus = SPI_SLAVE_ERROR_NONE;
    spi2Obj.callback = NULL ;
    spi2Obj.transferIsBusy = false ;
    spi2Obj.csInterruptPending = false;
    spi2Obj.rxInterruptActive = false;

    /* Set the Busy Pin to ready state */
    GPIO_PinWrite((GPIO_PIN)SPI2_BUSY_PIN, 0);

    /* Register callback and enable notifications on Chip Select logic level change */
    (void)GPIO_PinInterruptCallbackRegister(SPI2_CS_PIN, SPI2_CS_Handler, 0U);
    GPIO_PinInterruptEnable(SPI2_CS_PIN);

    /* Enable SPI2 RX and Error Interrupts. TX interrupt will be enabled when a SPI write is submitted. */
    SPI2_ENABLE_RX_INT();
    SPI2_ENABLE_ERR_INT();

    /* Enable SPI2 */
    SPI2CONSET = _SPI2CON_ON_MASK;
}

/* For 16-bit/32-bit mode, the "size" must be specified in terms of 16-bit/32-bit words */
size_t SPI2_Read(void* pRdBuffer, size_t size)
{
    size_t rdSize = size;
    uint32_t rdInIndex = spi2Obj.rdInIndex;

    if (rdSize > rdInIndex)
    {
        rdSize = rdInIndex;
    }

   (void) mem_copy(pRdBuffer, SPI2_ReadBuffer, rdSize);

    return rdSize;
}

/* For 16-bit/32-bit mode, the "size" must be specified in terms of 16-bit/32-bit words */
size_t SPI2_Write(void* pWrBuffer, size_t size )
{
    size_t wrSize = size;
    size_t wrOutIndex = 0;

    SPI2_DISABLE_TX_INT();

    if (wrSize > SPI2_WRITE_BUFFER_SIZE)
    {
        wrSize = SPI2_WRITE_BUFFER_SIZE;
    }

    (void) mem_copy(SPI2_WriteBuffer, pWrBuffer, wrSize);

    spi2Obj.nWrBytes = wrSize;

    /* Fill up the FIFO as long as there are empty elements */
    while ((!(SPI2STAT & _SPI2STAT_SPITBF_MASK)) && (wrOutIndex < wrSize))
    {
        SPI2BUF = SPI2_WriteBuffer[wrOutIndex];
        wrOutIndex++;
    }

    spi2Obj.wrOutIndex = wrOutIndex;

    /* Enable TX interrupt */
    SPI2_ENABLE_TX_INT();

    return wrSize;
}

/* For 16-bit/32-bit mode, the return value is in terms of 16-bit/32-bit words */
size_t SPI2_ReadCountGet(void)
{
    return spi2Obj.rdInIndex;
}

/* For 16-bit/32-bit mode, the return value is in terms of 16-bit/32-bit words */
size_t SPI2_ReadBufferSizeGet(void)
{
    return SPI2_READ_BUFFER_SIZE;
}

/* For 16-bit/32-bit mode, the return value is in terms of 16-bit/32-bit words */
size_t SPI2_WriteBufferSizeGet(void)
{
    return SPI2_WRITE_BUFFER_SIZE;
}

void SPI2_CallbackRegister(SPI_SLAVE_CALLBACK callBack, uintptr_t context )
{
    spi2Obj.callback = callBack;

    spi2Obj.context = context;
}

/* The status is returned as busy when CS is asserted */
bool SPI2_IsBusy(void)
{
    return spi2Obj.transferIsBusy;
}

/* Drive the GPIO pin to indicate to SPI Master that the slave is ready now */
void SPI2_Ready(void)
{
    GPIO_PinWrite((GPIO_PIN)SPI2_BUSY_PIN, 0);
}

SPI_SLAVE_ERROR SPI2_ErrorGet(void)
{
    SPI_SLAVE_ERROR errorStatus = spi2Obj.errorStatus;

    spi2Obj.errorStatus = SPI_SLAVE_ERROR_NONE;

    return errorStatus;
}

static void __attribute__((used)) SPI2_CS_Handler(GPIO_PIN pin, uintptr_t context)
{
    bool activeState = false;

    if (GPIO_PinRead((GPIO_PIN)SPI2_CS_PIN) == activeState)
    {
        /* CS is asserted */
        spi2Obj.transferIsBusy = true;

        /* Drive busy line to active state */
        GPIO_PinWrite((GPIO_PIN)SPI2_BUSY_PIN, 1);
    }
    else
    {
        /* Give application callback only if RX interrupt is not preempted and RX interrupt is not pending to be serviced */

        bool rxInterruptActive = spi2Obj.rxInterruptActive;

        if (((IFS1 & _IFS1_SPI2RXIF_MASK) == 0) && (rxInterruptActive == false))
        {
            /* CS is de-asserted */
            spi2Obj.transferIsBusy = false;

            spi2Obj.wrOutIndex = 0;
            spi2Obj.nWrBytes = 0;

            if(spi2Obj.callback != NULL)
            {
                uintptr_t context_val = spi2Obj.context;

                spi2Obj.callback(context_val);
            }

            /* Clear the read index. Application must read out the data by calling SPI2_Read API in the callback */
            spi2Obj.rdInIndex = 0;
        }
        else
        {
            /* If CS interrupt is serviced by either preempting the RX interrupt or RX interrupt is pending to be serviced,
             * then delegate the responsibility of giving application callback to the RX interrupt handler */

            spi2Obj.csInterruptPending = true;
        }
    }
}

static void __attribute__((used)) SPI2_FAULT_InterruptHandler (void)
{
    spi2Obj.errorStatus =(SPI2STAT & _SPI2STAT_SPIROV_MASK);

    /* Clear the receive overflow flag */
    SPI2STATCLR = _SPI2STAT_SPIROV_MASK;

    SPI2_CLEAR_ERR_INT_FLAG();
}

static void __attribute__((used)) SPI2_TX_InterruptHandler (void)
{
    size_t wrOutIndex = spi2Obj.wrOutIndex;
    size_t nWrBytes = spi2Obj.nWrBytes;

    /* Fill up the FIFO as long as there are empty elements */
    while ((!(SPI2STAT & _SPI2STAT_SPITBF_MASK)) && (wrOutIndex < nWrBytes))
    {
        SPI2BUF = SPI2_WriteBuffer[wrOutIndex];
        wrOutIndex++;
    }

    spi2Obj.wrOutIndex = wrOutIndex;

    /* Clear the transmit interrupt flag */
    SPI2_CLEAR_TX_INT_FLAG();

    if (spi2Obj.wrOutIndex == nWrBytes)
    {
        /* Nothing to transmit. Disable transmit interrupt. The last byte sent by the master will be shifted out automatically*/
        SPI2_DISABLE_TX_INT();
    }
}

static void __attribute__((used)) SPI2_RX_InterruptHandler (void)
{
    uint32_t receivedData = 0;

    spi2Obj.rxInterruptActive = true;

    size_t rdInIndex = spi2Obj.rdInIndex;

    while (!(SPI2STAT & _SPI2STAT_SPIRBE_MASK))
    {
        /* Receive buffer is not empty. Read the received data. */
        receivedData = SPI2BUF;

        if (rdInIndex < SPI2_READ_BUFFER_SIZE)
        {
            SPI2_ReadBuffer[rdInIndex] = (uint8_t)receivedData;
            rdInIndex++;
        }
    }

    spi2Obj.rdInIndex = rdInIndex;

    /* Clear the receive interrupt flag */
    SPI2_CLEAR_RX_INT_FLAG();

    spi2Obj.rxInterruptActive = false;

    /* Check if CS interrupt occured before the RX interrupt and that CS interrupt delegated the responsibility to give
     * application callback to the RX interrupt */

    if (spi2Obj.csInterruptPending == true)
    {
        spi2Obj.csInterruptPending = false;
        spi2Obj.transferIsBusy = false;

        spi2Obj.wrOutIndex = 0;
        spi2Obj.nWrBytes = 0;

        if(spi2Obj.callback != NULL)
        {
            uintptr_t context = spi2Obj.context;

            spi2Obj.callback(context);
        }

        /* Clear the read index. Application must read out the data by calling SPI2_Read API in the callback */
        spi2Obj.rdInIndex = 0;
    }
}

void __attribute__((used)) SPI_2_InterruptHandler(void)
{
    bool tmp;
    tmp = ((IEC1 & _IEC1_SPI2EIE_MASK) != 0U);
    if(((IFS1 & _IFS1_SPI2EIF_MASK) != 0U) && tmp)
    {
        /* Call error interrupt handler */
        SPI2_FAULT_InterruptHandler();
    }
    tmp = ((IEC1 & _IEC1_SPI2RXIE_MASK) != 0U);
    if(((IFS1 & _IFS1_SPI2RXIF_MASK) != 0U) && tmp)
    {
        /* RX interrupt is enabled and RX buffer is not empty */
        SPI2_RX_InterruptHandler();
    }
    tmp = ((IEC1 & _IEC1_SPI2TXIE_MASK) != 0U);
    if(((IFS1 & _IFS1_SPI2TXIF_MASK) != 0U) && tmp)
    {
        /* TX interrupt is enabled and TX buffer is empty */
        SPI2_TX_InterruptHandler();
    }
}
