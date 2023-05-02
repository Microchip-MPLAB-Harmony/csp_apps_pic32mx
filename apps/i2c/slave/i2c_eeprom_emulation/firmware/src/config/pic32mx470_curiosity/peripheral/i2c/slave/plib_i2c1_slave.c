/*******************************************************************************
  Inter-Integrated Circuit (I2C) Library
  Source File

  Company:
    Microchip Technology Inc.

  File Name:
    plib_i2c1_slave.c

  Summary:
    I2C PLIB Slave Implementation file

  Description:
    This file defines the interface to the I2C peripheral library.
    This library provides access to and control of the associated peripheral
    instance.

*******************************************************************************/
// DOM-IGNORE-BEGIN
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
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************
#include "device.h"
#include "plib_i2c1_slave.h"
#include "interrupts.h"

// *****************************************************************************
// *****************************************************************************
// Section: Global Data
// *****************************************************************************
// *****************************************************************************
#define I2C1_SLAVE_DATA_SETUP_TIME_CORE_TIMER_CNTS          4
#define I2C1_SLAVE_RISE_TIME_CORE_TIMER_CNTS                14

volatile static I2C_SLAVE_OBJ i2c1Obj;

void I2C1_Initialize(void)
{
    /* Turn off the I2C module */
    I2C1CONCLR = _I2C1CON_ON_MASK;

    I2C1CON = (_I2C1CON_STREN_MASK );

    I2C1ADD = 0x54;

    I2C1MSK = 0x00;

    /* Clear slave interrupt flag */
    IFS1CLR = _IFS1_I2C1SIF_MASK;

    /* Clear fault interrupt flag */
    IFS1CLR = _IFS1_I2C1BIF_MASK;

    /* Enable the I2C Slave interrupt */
    IEC1SET = _IEC1_I2C1SIE_MASK;

    /* Enable the I2C Bus collision interrupt */
    IEC1SET = _IEC1_I2C1BIE_MASK;

    i2c1Obj.callback = NULL;

    /* Turn on the I2C module */
    I2C1CONSET = _I2C1CON_ON_MASK;
}

static void I2C1_RiseAndSetupTime(uint8_t sdaState)
{
    uint32_t startCount, endCount;

    if (sdaState == 0U)
    {
        endCount = I2C1_SLAVE_DATA_SETUP_TIME_CORE_TIMER_CNTS;
    }
    else
    {
        endCount = I2C1_SLAVE_DATA_SETUP_TIME_CORE_TIMER_CNTS + I2C1_SLAVE_RISE_TIME_CORE_TIMER_CNTS;
    }

    startCount =_CP0_GET_COUNT();

    while((_CP0_GET_COUNT()- startCount) < endCount)
    {
           /* Wait for timeout */
    }
}

/* I2C slave state machine */
static void I2C1_TransferSM(void)
{
    uint32_t i2c_addr;
    uint8_t sdaValue = 0U;
    uintptr_t context = i2c1Obj.context;

    /* ACK the slave interrupt */
    IFS1CLR = _IFS1_I2C1SIF_MASK;

    if ((I2C1STAT & _I2C1STAT_D_A_MASK) == 0U)
    {
        if ((I2C1STAT & _I2C1STAT_RBF_MASK) != 0U)
        {
            /* Received I2C address must be read out */
            i2c_addr = I2C1RCV;
            (void)i2c_addr;

            if (i2c1Obj.callback != NULL)
            {
                /* Notify that a address match event has occurred */
               (void)i2c1Obj.callback(I2C_SLAVE_TRANSFER_EVENT_ADDR_MATCH, context);

                if ((I2C1STAT & _I2C1STAT_R_W_MASK) != 0U)
                {
                    /* I2C master wants to read */
                    if ((I2C1STAT & _I2C1STAT_TBF_MASK) == 0U)
                    {
                        /* In the callback, slave must write to transmit register by calling I2Cx_WriteByte() */
                        (void)i2c1Obj.callback(I2C_SLAVE_TRANSFER_EVENT_TX_READY, context);

                        sdaValue = (i2c1Obj.lastByteWritten & 0x80U);
                        I2C1_RiseAndSetupTime(sdaValue);
                    }
                }
            }
        /* Data written by the application; release the clock stretch */
        I2C1CONSET = _I2C1CON_SCLREL_MASK;
        }
    }
    else
    {
        /* Master reads from slave, slave transmits */
        if ((I2C1STAT & _I2C1STAT_R_W_MASK) != 0U)
        {
            if (((I2C1STAT & (_I2C1STAT_TBF_MASK | _I2C1STAT_ACKSTAT_MASK))  == 0U))
            {
                if (i2c1Obj.callback != NULL)
                {
                    /* I2C master wants to read. In the callback, slave must write to transmit register */
                    (void)i2c1Obj.callback(I2C_SLAVE_TRANSFER_EVENT_TX_READY, context);

                    sdaValue = (i2c1Obj.lastByteWritten & 0x80U);
                }

                I2C1_RiseAndSetupTime(sdaValue);

                /* Data written by the application; release the clock stretch */
                I2C1CONSET = _I2C1CON_SCLREL_MASK;
            }
        }
        /* Master writes to slave, slave receives */
        else
        {
            if ((I2C1STAT & _I2C1STAT_RBF_MASK) != 0U)
            {
                if (i2c1Obj.callback != NULL)
                {
                    /* I2C master wants to write. In the callback, slave must read data by calling I2Cx_ReadByte()  */
                    (void)i2c1Obj.callback(I2C_SLAVE_TRANSFER_EVENT_RX_READY, context);
                }
                /* Data read by the application; release the clock stretch */
                I2C1CONSET = _I2C1CON_SCLREL_MASK;
            }
        }
    }
}

void I2C1_CallbackRegister(I2C_SLAVE_CALLBACK callback, uintptr_t contextHandle)
{
    if (callback != NULL)
    {
        i2c1Obj.callback = callback;
        i2c1Obj.context = contextHandle;
    }
}

bool I2C1_IsBusy(void)
{
    return ((I2C1STAT & _I2C1STAT_S_MASK) != 0U);
}

uint8_t I2C1_ReadByte(void)
{
    return (uint8_t)I2C1RCV;
}

void I2C1_WriteByte(uint8_t wrByte)
{
    if ((I2C1STAT & _I2C1STAT_TBF_MASK) == 0U)
    {
        I2C1TRN = wrByte;
        i2c1Obj.lastByteWritten = wrByte;
    }
}

I2C_SLAVE_TRANSFER_DIR I2C1_TransferDirGet(void)
{
    return ((I2C1STAT & _I2C1STAT_R_W_MASK) != 0U) ? I2C_SLAVE_TRANSFER_DIR_READ : I2C_SLAVE_TRANSFER_DIR_WRITE;
}

I2C_SLAVE_ACK_STATUS I2C1_LastByteAckStatusGet(void)
{
    return ((I2C1STAT & _I2C1STAT_ACKSTAT_MASK) != 0U) ? I2C_SLAVE_ACK_STATUS_RECEIVED_NAK : I2C_SLAVE_ACK_STATUS_RECEIVED_ACK;
}

I2C_SLAVE_ERROR I2C1_ErrorGet(void)
{
    I2C_SLAVE_ERROR error;

    error = i2c1Obj.error;
    i2c1Obj.error = I2C_SLAVE_ERROR_NONE;

    return error;
}

static void I2C1_BUS_InterruptHandler(void)
{
    /* Clear the bus collision error status bit */
    I2C1STATCLR = _I2C1STAT_BCL_MASK;

    /* ACK the bus interrupt */
    IFS1CLR = _IFS1_I2C1BIF_MASK;

    i2c1Obj.error = I2C_SLAVE_ERROR_BUS_COLLISION;

    if (i2c1Obj.callback != NULL)
    {
        uintptr_t context = i2c1Obj.context;

        (void) i2c1Obj.callback(I2C_SLAVE_TRANSFER_EVENT_ERROR, context);
    }
}

static void __attribute__((used)) I2C1_SLAVE_InterruptHandler(void)
{
    I2C1_TransferSM();
}

void __attribute__((used)) I2C_1_InterruptHandler(void)
{
    uint32_t iec_bus_reg_read = IEC1;
    uint32_t iec_slave_reg_read = IEC1;
    if (((IFS1 & _IFS1_I2C1BIF_MASK) != 0U) &&
        ((iec_bus_reg_read & _IEC1_I2C1BIE_MASK) != 0U))
    {
        I2C1_BUS_InterruptHandler();
    }
    else if (((IFS1 & _IFS1_I2C1SIF_MASK) != 0U) &&
             ((iec_slave_reg_read & _IEC1_I2C1SIE_MASK) != 0U))
    {
        I2C1_SLAVE_InterruptHandler();
    }
    else
    {
        /* Do Nothing */
    }
}