/*******************************************************************************
  SPI PLIB

  Company:
    Microchip Technology Inc.

  File Name:
    plib_spi1_slave.c

  Summary:
    SPI1 Slave Source File

  Description:
    This file has implementation of all the interfaces provided for particular
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
#include "plib_spi1_slave.h"
#include "peripheral/gpio/plib_gpio.h"
#include <string.h>
// *****************************************************************************
// *****************************************************************************
// Section: SPI1 Slave Implementation
// *****************************************************************************
// *****************************************************************************
#define SPI1_BUSY_PIN					 GPIO_PIN_RB4
#define SPI1_CS_PIN						 GPIO_PIN_RD4


#define SPI1_READ_BUFFER_SIZE            256
#define SPI1_WRITE_BUFFER_SIZE           256

static uint8_t SPI1_ReadBuffer[SPI1_READ_BUFFER_SIZE];
static uint8_t SPI1_WriteBuffer[SPI1_WRITE_BUFFER_SIZE];


/* Global object to save SPI Exchange related data */
SPI_SLAVE_OBJECT spi1Obj;

#define SPI1_CON_CKP                        (0 << _SPI1CON_CKP_POSITION)
#define SPI1_CON_CKE                        (1 << _SPI1CON_CKE_POSITION)
#define SPI1_CON_MODE_32_MODE_16            (0 << _SPI1CON_MODE16_POSITION)
#define SPI1_CON_ENHBUF                     (1 << _SPI1CON_ENHBUF_POSITION)
#define SPI1_CON_STXISEL                    (3 << _SPI1CON_STXISEL_POSITION)
#define SPI1_CON_SRXISEL                    (1 << _SPI1CON_SRXISEL_POSITION)
                             
#define SPI1_ENABLE_RX_INT() 				IEC1SET = 0x10
#define SPI1_CLEAR_RX_INT_FLAG()			IFS1CLR = 0x10

#define SPI1_DISABLE_TX_INT()				IEC1CLR = 0x20                
#define SPI1_ENABLE_TX_INT() 				IEC1SET = 0x20
#define SPI1_CLEAR_TX_INT_FLAG()			IFS1CLR = 0x20

#define SPI1_ENABLE_ERR_INT() 				IEC1SET = 0x8
#define SPI1_CLEAR_ERR_INT_FLAG()			IEC1CLR = 0x8

/* Forward declarations */
static void SPI1_CS_Handler(GPIO_PIN pin, uintptr_t context);

void SPI1_Initialize ( void )
{
    /* Disable SPI1 Interrupts */
    IEC1CLR = 0x8;
    IEC1CLR = 0x10;
    IEC1CLR = 0x20;

    /* STOP and Reset the SPI */
    SPI1CON = 0;    

    /* Clear SPI1 Interrupt flags */
    IFS1CLR = 0x8;
    IFS1CLR = 0x10;
    IFS1CLR = 0x20;
    
    /* CLear the receiver overflow error flag */
    SPI1STATCLR = _SPI1STAT_SPIROV_MASK;

    /*	
	SRXISEL = 1 (Receive buffer is not empty)
	STXISEL = 3 (Transmit buffer is not full)
    MSTEN = 0
    CKP = 0
    CKE = 1
    MODE<32,16> = 0
    ENHBUF = 1    
    */
	
    SPI1CONSET = (SPI1_CON_ENHBUF | SPI1_CON_MODE_32_MODE_16 | SPI1_CON_CKE | SPI1_CON_CKP | SPI1_CON_STXISEL | SPI1_CON_SRXISEL);   
	
	/* Enable generation of interrupt on receiver overflow */
	SPI1CON2SET = _SPI1CON2_SPIROVEN_MASK;

	spi1Obj.rdInIndex = 0;
    spi1Obj.wrOutIndex = 0;
    spi1Obj.nWrBytes = 0;
    spi1Obj.errorStatus = SPI_SLAVE_ERROR_NONE;
    spi1Obj.callback = NULL ;
    spi1Obj.transferIsBusy = false ;	
	spi1Obj.csInterruptPending = false;
	spi1Obj.rxInterruptActive = false;
	
    /* Set the Busy Pin to ready state */
    GPIO_PinWrite((GPIO_PIN)SPI1_BUSY_PIN, 0);
	
	/* Register callback and enable notifications on Chip Select logic level change */
	GPIO_PinInterruptCallbackRegister(SPI1_CS_PIN, SPI1_CS_Handler, (uintptr_t)NULL);
    GPIO_PinInterruptEnable(SPI1_CS_PIN);
	
	/* Enable SPI1 RX and Error Interrupts. TX interrupt will be enabled when a SPI write is submitted. */
    SPI1_ENABLE_RX_INT();
	SPI1_ENABLE_ERR_INT();
    
    /* Enable SPI1 */
    SPI1CONSET = _SPI1CON_ON_MASK;
}

/* For 16-bit/32-bit mode, the "size" must be specified in terms of 16-bit/32-bit words */
size_t SPI1_Read(void* pRdBuffer, size_t size)
{
    size_t rdSize = size;
	uint32_t rdInIndex = spi1Obj.rdInIndex;    

    if (rdSize > rdInIndex)
    {
        rdSize = rdInIndex;
    }

    memcpy(pRdBuffer, SPI1_ReadBuffer, rdSize);    

    return rdSize;
}

/* For 16-bit/32-bit mode, the "size" must be specified in terms of 16-bit/32-bit words */
size_t SPI1_Write(void* pWrBuffer, size_t size )
{    
    size_t wrSize = size;

    SPI1_DISABLE_TX_INT();

    if (wrSize > SPI1_WRITE_BUFFER_SIZE)
    {
        wrSize = SPI1_WRITE_BUFFER_SIZE;
    }

    memcpy(SPI1_WriteBuffer, pWrBuffer, wrSize);

    spi1Obj.nWrBytes = wrSize;
    spi1Obj.wrOutIndex = 0;	

	/* Fill up the FIFO as long as there are empty elements */
    while ((!(SPI1STAT & _SPI1STAT_SPITBF_MASK)) && (spi1Obj.wrOutIndex < spi1Obj.nWrBytes))
    {
        SPI1BUF = SPI1_WriteBuffer[spi1Obj.wrOutIndex++];
    }

    /* Enable TX interrupt */
    SPI1_ENABLE_TX_INT();

    return wrSize;
}

/* For 16-bit/32-bit mode, the return value is in terms of 16-bit/32-bit words */
size_t SPI1_ReadCountGet(void)
{
    return spi1Obj.rdInIndex;
}

/* For 16-bit/32-bit mode, the return value is in terms of 16-bit/32-bit words */
size_t SPI1_ReadBufferSizeGet(void)
{
    return SPI1_READ_BUFFER_SIZE;
}

/* For 16-bit/32-bit mode, the return value is in terms of 16-bit/32-bit words */
size_t SPI1_WriteBufferSizeGet(void)
{
    return SPI1_WRITE_BUFFER_SIZE;
}

void SPI1_CallbackRegister(SPI_SLAVE_CALLBACK callBack, uintptr_t context )
{
    spi1Obj.callback = callBack;

    spi1Obj.context = context;
}

/* The status is returned as busy when CS is asserted */
bool SPI1_IsBusy(void)
{
    return spi1Obj.transferIsBusy;
}

/* Drive the GPIO pin to indicate to SPI Master that the slave is ready now */
void SPI1_Ready(void)
{
    GPIO_PinWrite((GPIO_PIN)SPI1_BUSY_PIN, 0);
}

SPI_SLAVE_ERROR SPI1_ErrorGet(void)
{
    SPI_SLAVE_ERROR errorStatus = spi1Obj.errorStatus;

    spi1Obj.errorStatus = SPI_SLAVE_ERROR_NONE;

    return errorStatus;
}

static void SPI1_CS_Handler(GPIO_PIN pin, uintptr_t context)
{
	bool activeState = 0;
	
    if (GPIO_PinRead((GPIO_PIN)SPI1_CS_PIN) == activeState)
	{
		/* CS is asserted */
		spi1Obj.transferIsBusy = true;
		
		/* Drive busy line to active state */
        GPIO_PinWrite((GPIO_PIN)SPI1_BUSY_PIN, 1);
	}
	else
	{
		/* Give application callback only if RX interrupt is not preempted and RX interrupt is not pending to be serviced */
		
		if ((spi1Obj.rxInterruptActive == false) && ((IFS1 & _IFS1_SPI1RXIF_MASK) == 0))
		{
			/* CS is de-asserted */
			spi1Obj.transferIsBusy = false;

			spi1Obj.wrOutIndex = 0;
			spi1Obj.nWrBytes = 0;

			if(spi1Obj.callback != NULL)
			{
				spi1Obj.callback(spi1Obj.context);
			}
			
			/* Clear the read index. Application must read out the data by calling SPI1_Read API in the callback */
			spi1Obj.rdInIndex = 0;
		}
		else
		{
			/* If CS interrupt is serviced by either preempting the RX interrupt or RX interrupt is pending to be serviced, 
			 * then delegate the responsibility of giving application callback to the RX interrupt handler */
			 
			spi1Obj.csInterruptPending = true;
		}
	}
}

static void SPI1_ERR_InterruptHandler (void)
{
	spi1Obj.errorStatus = (SPI1STAT & _SPI1STAT_SPIROV_MASK);
	
	/* Clear the receive overflow flag */
	SPI1STATCLR = _SPI1STAT_SPIROV_MASK;
		
	SPI1_CLEAR_ERR_INT_FLAG();
}

static void SPI1_TX_InterruptHandler (void)
{
	/* Fill up the FIFO as long as there are empty elements */
	while ((!(SPI1STAT & _SPI1STAT_SPITBF_MASK)) && (spi1Obj.wrOutIndex < spi1Obj.nWrBytes))
	{
		SPI1BUF = SPI1_WriteBuffer[spi1Obj.wrOutIndex++];
	}
	
	/* Clear the transmit interrupt flag */
    SPI1_CLEAR_TX_INT_FLAG();
		
	if (spi1Obj.wrOutIndex == spi1Obj.nWrBytes)
	{
		/* Nothing to transmit. Disable transmit interrupt. The last byte sent by the master will be shifted out automatically*/
		SPI1_DISABLE_TX_INT();
	}			       
}

static void SPI1_RX_InterruptHandler (void)
{
	uint32_t receivedData = 0;
	
	spi1Obj.rxInterruptActive = true;
				
	while (!(SPI1STAT & _SPI1STAT_SPIRBE_MASK))
    {        
        /* Receive buffer is not empty. Read the received data. */
        receivedData = SPI1BUF;

        if (spi1Obj.rdInIndex < SPI1_READ_BUFFER_SIZE)
        {
            SPI1_ReadBuffer[spi1Obj.rdInIndex++] = receivedData;
        }
    }
	    
    /* Clear the receive interrupt flag */
    SPI1_CLEAR_RX_INT_FLAG();
	
	spi1Obj.rxInterruptActive = false;
	
	/* Check if CS interrupt occured before the RX interrupt and that CS interrupt delegated the responsibility to give 
	 * application callback to the RX interrupt */
	 
	if (spi1Obj.csInterruptPending == true)
	{	
		spi1Obj.csInterruptPending = false;		
		spi1Obj.transferIsBusy = false;

        spi1Obj.wrOutIndex = 0;
        spi1Obj.nWrBytes = 0;

        if(spi1Obj.callback != NULL)
        {
            spi1Obj.callback(spi1Obj.context);
        }
		
		/* Clear the read index. Application must read out the data by calling SPI1_Read API in the callback */
		spi1Obj.rdInIndex = 0;
	}
}

void SPI_1_InterruptHandler (void)
{
	if ((IFS1 & _IFS1_SPI1EIF_MASK) && (IEC1 & _IEC1_SPI1EIE_MASK))
    {
        /* Call error interrupt handler */
        SPI1_ERR_InterruptHandler();
    }
    if ((IFS1 & _IFS1_SPI1RXIF_MASK) && (IEC1 & _IEC1_SPI1RXIE_MASK))
    {
        /* RX interrupt is enabled and RX buffer is not empty */
        SPI1_RX_InterruptHandler();
    }
    if ((IFS1 & _IFS1_SPI1TXIF_MASK) && (IEC1 & _IEC1_SPI1TXIE_MASK))
    {
        /* TX interrupt is enabled and TX buffer is empty */
        SPI1_TX_InterruptHandler();
    }
}
