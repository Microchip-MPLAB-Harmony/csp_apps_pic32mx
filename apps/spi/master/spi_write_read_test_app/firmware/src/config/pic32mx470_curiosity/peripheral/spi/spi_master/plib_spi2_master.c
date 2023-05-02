/*******************************************************************************
  SPI PLIB

  Company:
    Microchip Technology Inc.

  File Name:
    plib_spi2_master.c

  Summary:
    SPI2 Master Source File

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

#include "plib_spi2_master.h"
#include "interrupts.h"

// *****************************************************************************
// *****************************************************************************
// Section: SPI2 Implementation
// *****************************************************************************
// *****************************************************************************

/* Global object to save SPI Exchange related data */
volatile static SPI_OBJECT spi2Obj;

#define SPI2_CON_MSTEN                      (1UL << _SPI2CON_MSTEN_POSITION)
#define SPI2_CON_CKP                        (0UL << _SPI2CON_CKP_POSITION)
#define SPI2_CON_CKE                        (1UL << _SPI2CON_CKE_POSITION)
#define SPI2_CON_MODE_32_MODE_16            (0UL << _SPI2CON_MODE16_POSITION)
#define SPI2_CON_ENHBUF                     (1UL << _SPI2CON_ENHBUF_POSITION)
#define SPI2_CON_MCLKSEL                    (0UL << _SPI2CON_MCLKSEL_POSITION)
#define SPI2_CON_MSSEN                      (0UL << _SPI2CON_MSSEN_POSITION)
#define SPI2_CON_SMP                        (0UL << _SPI2CON_SMP_POSITION)

void SPI2_Initialize ( void )
{
    uint32_t rdata = 0U;

    /* Disable SPI2 Interrupts */
    IEC1CLR = 0x200000;
    IEC1CLR = 0x400000;
    IEC1CLR = 0x800000;

    /* STOP and Reset the SPI */
    SPI2CON = 0;

    /* Clear the Receiver buffer */
    rdata = SPI2BUF;
    rdata = rdata;

    /* Clear SPI2 Interrupt flags */
    IFS1CLR = 0x200000;
    IFS1CLR = 0x400000;
    IFS1CLR = 0x800000;

    /* BAUD Rate register Setup */
    SPI2BRG = 23;

    /* CLear the Overflow */
    SPI2STATCLR = _SPI2STAT_SPIROV_MASK;

    /*
    MSTEN = 1
    CKP = 0
    CKE = 1
    MODE<32,16> = 0
    ENHBUF = 1
    MSSEN = 0
    MCLKSEL = 0
    */
    SPI2CONSET = (SPI2_CON_MSSEN | SPI2_CON_MCLKSEL | SPI2_CON_ENHBUF | SPI2_CON_MODE_32_MODE_16 | SPI2_CON_CKE | SPI2_CON_CKP | SPI2_CON_MSTEN | SPI2_CON_SMP);

    /* Enable transmit interrupt when transmit buffer is completely empty (STXISEL = '01') */
    /* Enable receive interrupt when the receive buffer is not empty (SRXISEL = '01') */
    SPI2CONSET = 0x00000005;

    /* Initialize global variables */
    spi2Obj.transferIsBusy = false;
    spi2Obj.callback = NULL;

    /* Enable SPI2 */
    SPI2CONSET = _SPI2CON_ON_MASK;
}

bool SPI2_TransferSetup (SPI_TRANSFER_SETUP* setup, uint32_t spiSourceClock )
{
    uint32_t t_brg;
    uint32_t baudHigh;
    uint32_t baudLow;
    uint32_t errorHigh;
    uint32_t errorLow;

    if ((setup == NULL) || (setup->clockFrequency == 0U))
    {
        return false;
    }

    if(spiSourceClock == 0U)
    {
        // Use Master Clock Frequency set in GUI
        spiSourceClock = 48000000;
    }

    t_brg = (((spiSourceClock / (setup->clockFrequency)) / 2u) - 1u);
    baudHigh = spiSourceClock / (2u * (t_brg + 1u));
    baudLow = spiSourceClock / (2u * (t_brg + 2u));
    errorHigh = baudHigh - setup->clockFrequency;
    errorLow = setup->clockFrequency - baudLow;

    if (errorHigh > errorLow)
    {
        t_brg++;
    }

    if(t_brg > 511U)
    {
        return false;
    }

    SPI2BRG = t_brg;
    SPI2CON = (SPI2CON & (~(_SPI2CON_MODE16_MASK | _SPI2CON_MODE32_MASK | _SPI2CON_CKP_MASK | _SPI2CON_CKE_MASK))) |
                            ((uint32_t)setup->clockPolarity | (uint32_t)setup->clockPhase | (uint32_t)setup->dataBits);

    return true;
}

bool SPI2_Write(void* pTransmitData, size_t txSize)
{
    return(SPI2_WriteRead(pTransmitData, txSize, NULL, 0));
}

bool SPI2_Read(void* pReceiveData, size_t rxSize)
{
    return(SPI2_WriteRead(NULL, 0, pReceiveData, rxSize));
}

bool SPI2_IsTransmitterBusy (void)
{
    return ((SPI2STAT & _SPI2STAT_SRMT_MASK) == 0U)? true : false;
}

bool SPI2_WriteRead (void* pTransmitData, size_t txSize, void* pReceiveData, size_t rxSize)
{
    bool isRequestAccepted = false;

    /* Verify the request */
    if((spi2Obj.transferIsBusy == false) && (((txSize > 0U) && (pTransmitData != NULL)) || ((rxSize > 0U) && (pReceiveData != NULL))))
    {
        isRequestAccepted = true;
        spi2Obj.txBuffer = pTransmitData;
        spi2Obj.rxBuffer = pReceiveData;
        spi2Obj.rxCount = 0;
        spi2Obj.txCount = 0;
        spi2Obj.dummySize = 0;

        if (pTransmitData != NULL)
        {
            spi2Obj.txSize = txSize;
        }
        else
        {
            spi2Obj.txSize = 0;
        }

        if (pReceiveData != NULL)
        {
            spi2Obj.rxSize = rxSize;
        }
        else
        {
            spi2Obj.rxSize = 0;
        }

        spi2Obj.transferIsBusy = true;

        size_t txSz = spi2Obj.txSize;
        if (spi2Obj.rxSize > txSz)
        {
            spi2Obj.dummySize = spi2Obj.rxSize - txSz;
        }

        /* Clear the receive overflow error if any */
        SPI2STATCLR = _SPI2STAT_SPIROV_MASK;

        /* Make sure there is no data pending in the RX FIFO */
        /* Depending on 8/16/32 bit mode, there may be 16/8/4 bytes in the FIFO */
        while ((SPI2STAT & _SPI2STAT_SPIRBE_MASK) == 0U)
        {
            (void)SPI2BUF;
        }

        /* Configure SPI to generate receive interrupt when receive buffer is empty (SRXISEL = '01') */
        SPI2CONCLR = _SPI2CON_SRXISEL_MASK;
        SPI2CONSET = 0x00000001;

        /* Configure SPI to generate transmit interrupt when the transmit shift register is empty (STXISEL = '00')*/
        SPI2CONCLR = _SPI2CON_STXISEL_MASK;

        /* Disable the receive interrupt */
        IEC1CLR = 0x400000;

        /* Disable the transmit interrupt */
        IEC1CLR = 0x800000;

        /* Clear the receive interrupt flag */
        IFS1CLR = 0x400000;

        /* Clear the transmit interrupt flag */
        IFS1CLR = 0x800000;

        /* Start the first write here itself, rest will happen in ISR context */
        if((_SPI2CON_MODE32_MASK) == (SPI2CON & (_SPI2CON_MODE32_MASK)))
        {
            spi2Obj.txSize >>= 2;
            spi2Obj.dummySize >>= 2;
            spi2Obj.rxSize >>= 2;

            txSz = spi2Obj.txSize;
            if(spi2Obj.txCount < txSz)
            {
                SPI2BUF = *((uint32_t*)spi2Obj.txBuffer);
                spi2Obj.txCount++;
            }
            else if (spi2Obj.dummySize > 0U)
            {
                SPI2BUF = (uint32_t)(0xffU);
                spi2Obj.dummySize--;
            }
            else
            {
                /* Nothing to process */
            }
        }
        else if((_SPI2CON_MODE16_MASK) == (SPI2CON & (_SPI2CON_MODE16_MASK)))
        {
            spi2Obj.txSize >>= 1;
            spi2Obj.dummySize >>= 1;
            spi2Obj.rxSize >>= 1;

            txSz = spi2Obj.txSize;
            if (spi2Obj.txCount < txSz)
            {
                SPI2BUF = *((uint16_t*)spi2Obj.txBuffer);
                spi2Obj.txCount++;
            }
            else if (spi2Obj.dummySize > 0U)
            {
                SPI2BUF = (uint16_t)(0xffU);
                spi2Obj.dummySize--;
            }
            else
            {
                /* Nothing to process */
            }
        }
        else
        {
            if (spi2Obj.txCount < txSz)
            {
                SPI2BUF = *((uint8_t*)spi2Obj.txBuffer);
                spi2Obj.txCount++;
            }
            else if (spi2Obj.dummySize > 0U)
            {
                SPI2BUF = (uint8_t)(0xffU);
                spi2Obj.dummySize--;
            }
            else
            {
                /* Nothing to process */
            }
        }

        if (rxSize > 0U)
        {
            /* Enable receive interrupt to complete the transfer in ISR context.
             * Keep the transmit interrupt disabled. Transmit interrupt will be
             * enabled later if txCount < txSize, when rxCount = rxSize.
             */
            IEC1SET = 0x400000;
        }
        else
        {
            if (spi2Obj.txCount != txSz)
            {
                /* Configure SPI to generate transmit buffer empty interrupt only if more than
                 * data is pending (STXISEL = '01')  */
                SPI2CONSET = 0x00000004;
            }
            /* Enable transmit interrupt to complete the transfer in ISR context */
            IEC1SET = 0x800000;
        }
    }

    return isRequestAccepted;
}

bool SPI2_IsBusy (void)
{
    uint32_t StatRead = SPI2STAT;
    return (((spi2Obj.transferIsBusy) != false) || (( StatRead & _SPI2STAT_SRMT_MASK) == 0U));
}

void SPI2_CallbackRegister (SPI_CALLBACK callback, uintptr_t context)
{
    spi2Obj.callback = callback;

    spi2Obj.context = context;
}

static void __attribute__((used)) SPI2_RX_InterruptHandler (void)
{
    uint32_t receivedData = 0;

    /* Check if the receive buffer is empty or not */
    if ((SPI2STAT & _SPI2STAT_SPIRBE_MASK) == 0U)
    {
        /* Receive buffer is not empty. Read the received data. */
        receivedData = SPI2BUF;

        size_t rxCount = spi2Obj.rxCount;
        size_t txCount = spi2Obj.txCount;
        if (rxCount < spi2Obj.rxSize)
        {
            /* Copy the received data to the user buffer */
            if((_SPI2CON_MODE32_MASK) == (SPI2CON & (_SPI2CON_MODE32_MASK)))
            {
                ((uint32_t*)spi2Obj.rxBuffer)[rxCount] = receivedData;
                rxCount++;
            }
            else if((_SPI2CON_MODE16_MASK) == (SPI2CON & (_SPI2CON_MODE16_MASK)))
            {
                ((uint16_t*)spi2Obj.rxBuffer)[rxCount] = (uint16_t)receivedData;
                rxCount++;
            }
            else
            {
                ((uint8_t*)spi2Obj.rxBuffer)[rxCount] = (uint8_t)receivedData;
                rxCount++;
            }

            spi2Obj.rxCount = rxCount;

            if (rxCount == spi2Obj.rxSize)
            {
                if (txCount < spi2Obj.txSize)
                {
                    /* Reception of all bytes is complete. However, there are few more
                     * bytes to be transmitted as txCount != txSize. Finish the
                     * transmission of the remaining bytes from the transmit interrupt. */

                    /* Disable the receive interrupt */
                    IEC1CLR = 0x400000;

                    /* Generate TX interrupt when buffer is completely empty (STXISEL = '00') */
                    SPI2CONCLR = _SPI2CON_STXISEL_MASK;
                    SPI2CONSET = 0x00000004;

                    /* Enable the transmit interrupt. Callback will be given from the
                     * transmit interrupt, when all bytes are shifted out. */
                    IEC1SET = 0x800000;
                }
            }
        }
        if (rxCount < spi2Obj.rxSize)
        {
            /* More bytes pending to be received .. */
            if((_SPI2CON_MODE32_MASK) == (SPI2CON & (_SPI2CON_MODE32_MASK)))
            {
                if (txCount < spi2Obj.txSize)
                {
                    SPI2BUF = ((uint32_t*)spi2Obj.txBuffer)[txCount];
                    txCount++;
                }
                else if (spi2Obj.dummySize > 0U)
                {
                    SPI2BUF = (uint32_t)(0xffU);
                    spi2Obj.dummySize--;
                }
                else
                {
                    /* Do Nothing */
                }
            }
            else if((_SPI2CON_MODE16_MASK) == (SPI2CON & (_SPI2CON_MODE16_MASK)))
            {
                if (txCount < spi2Obj.txSize)
                {
                    SPI2BUF = ((uint16_t*)spi2Obj.txBuffer)[txCount];
                    txCount++;
                }
                else if (spi2Obj.dummySize > 0U)
                {
                    SPI2BUF = (uint16_t)(0xffU);
                    spi2Obj.dummySize--;
                }
                else
                {
                    /* Do Nothing */
                }
            }
            else
            {
                if (txCount < spi2Obj.txSize)
                {
                    SPI2BUF = ((uint8_t*)spi2Obj.txBuffer)[txCount];
                    txCount++;
                }
                else if (spi2Obj.dummySize > 0U)
                {
                    SPI2BUF = (uint8_t)(0xffU);
                    spi2Obj.dummySize--;
                }
                else
                {
                    /* Do Nothing */
                }
            }
            spi2Obj.txCount = txCount;
        }
        else
        {
            if(rxCount == spi2Obj.rxSize)
            {
                if (txCount == spi2Obj.txSize)
                {
                    /* Clear receiver overflow error if any */
                    SPI2STATCLR = _SPI2STAT_SPIROV_MASK;

                    /* Disable receive interrupt */
                    IEC1CLR = 0x400000;

                    /* Transfer complete. Give a callback */
                    spi2Obj.transferIsBusy = false;

                    if(spi2Obj.callback != NULL)
                    {
                        uintptr_t context = spi2Obj.context;
                        spi2Obj.callback(context);
                    }
                }
            }
        }
    }

    /* Clear SPI2 RX Interrupt flag */
    /* This flag should cleared only after reading buffer */
    IFS1CLR = 0x400000;
}

static void __attribute__((used)) SPI2_TX_InterruptHandler (void)
{
    /* If there are more words to be transmitted, then transmit them here and keep track of the count */
    if((SPI2STAT & _SPI2STAT_SPITBE_MASK) == _SPI2STAT_SPITBE_MASK)
    {
        size_t txCount = spi2Obj.txCount;
        if (txCount < spi2Obj.txSize)
        {
            if((_SPI2CON_MODE32_MASK) == (SPI2CON & (_SPI2CON_MODE32_MASK)))
            {
                SPI2BUF = ((uint32_t*)spi2Obj.txBuffer)[txCount];
                txCount++;
            }
            else if((_SPI2CON_MODE16_MASK) == (SPI2CON & (_SPI2CON_MODE16_MASK)))
            {
                SPI2BUF = ((uint16_t*)spi2Obj.txBuffer)[txCount];
                txCount++;
            }
            else
            {
                SPI2BUF = ((uint8_t*)spi2Obj.txBuffer)[txCount];
                txCount++;
            }

            spi2Obj.txCount = txCount;

            if (txCount == spi2Obj.txSize)
            {
                /* All bytes are submitted to the SPI module. Now, enable transmit
                 * interrupt when the shift register is empty (STXISEL = '00')*/
                SPI2CONCLR = _SPI2CON_STXISEL_MASK;
            }
        }
        else if (txCount == spi2Obj.txSize)
        {
            if ((SPI2STAT & _SPI2STAT_SRMT_MASK) != 0U)
            {
                /* This part of code is executed when the shift register is empty. */

                /* Clear receiver overflow error if any */
                SPI2STATCLR = _SPI2STAT_SPIROV_MASK;

                /* Disable transmit interrupt */
                IEC1CLR = 0x800000;

                /* Transfer complete. Give a callback */
                spi2Obj.transferIsBusy = false;

                if(spi2Obj.callback != NULL)
                {
                    uintptr_t context = spi2Obj.context;
                    spi2Obj.callback(context);
                }
            }
        }
        else
        {
            /* Do Nothing */
        }
    }
    /* Clear the transmit interrupt flag */
    IFS1CLR = 0x800000;
}

void __attribute__((used)) SPI_2_InterruptHandler (void)
{
    uint32_t iec_reg_read = IEC1;
    if (((IFS1 & _IFS1_SPI2RXIF_MASK) != 0U) && (( iec_reg_read & _IEC1_SPI2RXIE_MASK) != 0U))
    {
        /* RX interrupt is enabled and RX buffer is not empty */
        SPI2_RX_InterruptHandler();
    }
    iec_reg_read = IEC1;
    if (((IFS1 & _IFS1_SPI2TXIF_MASK) != 0U) && (( iec_reg_read & _IEC1_SPI2TXIE_MASK) != 0U))
    {
        /* TX interrupt is enabled and TX buffer is empty */
        SPI2_TX_InterruptHandler();
    }
}
