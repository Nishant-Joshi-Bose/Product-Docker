/***************************************************************************//**
* \file LED_Driver_SPI_UART.c
* \version 3.20
*
* \brief
*  This file provides the source code to the API for the SCB Component in
*  SPI and UART modes.
*
* Note:
*
*******************************************************************************
* \copyright
* Copyright 2013-2016, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "LED_Driver_PVT.h"
#include "LED_Driver_SPI_UART_PVT.h"

/***************************************
*        SPI/UART Private Vars
***************************************/

#if(LED_Driver_INTERNAL_RX_SW_BUFFER_CONST)
    /* Start index to put data into the software receive buffer.*/
    volatile uint32 LED_Driver_rxBufferHead;
    /* Start index to get data from the software receive buffer.*/
    volatile uint32 LED_Driver_rxBufferTail;
    /**
    * \addtogroup group_globals
    * \{
    */
    /** Sets when internal software receive buffer overflow
    *  was occurred.
    */
    volatile uint8  LED_Driver_rxBufferOverflow;
    /** \} globals */
#endif /* (LED_Driver_INTERNAL_RX_SW_BUFFER_CONST) */

#if(LED_Driver_INTERNAL_TX_SW_BUFFER_CONST)
    /* Start index to put data into the software transmit buffer.*/
    volatile uint32 LED_Driver_txBufferHead;
    /* Start index to get data from the software transmit buffer.*/
    volatile uint32 LED_Driver_txBufferTail;
#endif /* (LED_Driver_INTERNAL_TX_SW_BUFFER_CONST) */

#if(LED_Driver_INTERNAL_RX_SW_BUFFER)
    /* Add one element to the buffer to receive full packet. One byte in receive buffer is always empty */
    volatile uint16 LED_Driver_rxBufferInternal[LED_Driver_INTERNAL_RX_BUFFER_SIZE];
#endif /* (LED_Driver_INTERNAL_RX_SW_BUFFER) */

#if(LED_Driver_INTERNAL_TX_SW_BUFFER)
    volatile uint16 LED_Driver_txBufferInternal[LED_Driver_TX_BUFFER_SIZE];
#endif /* (LED_Driver_INTERNAL_TX_SW_BUFFER) */


#if(LED_Driver_RX_DIRECTION)
    /*******************************************************************************
    * Function Name: LED_Driver_SpiUartReadRxData
    ****************************************************************************//**
    *
    *  Retrieves the next data element from the receive buffer.
    *   - RX software buffer is disabled: Returns data element retrieved from
    *     RX FIFO. Undefined data will be returned if the RX FIFO is empty.
    *   - RX software buffer is enabled: Returns data element from the software
    *     receive buffer. Zero value is returned if the software receive buffer
    *     is empty.
    *
    * \return
    *  Next data element from the receive buffer. 
    *  The amount of data bits to be received depends on RX data bits selection 
    *  (the data bit counting starts from LSB of return value).
    *
    * \globalvars
    *  LED_Driver_rxBufferHead - the start index to put data into the 
    *  software receive buffer.
    *  LED_Driver_rxBufferTail - the start index to get data from the 
    *  software receive buffer.
    *
    *******************************************************************************/
    uint32 LED_Driver_SpiUartReadRxData(void)
    {
        uint32 rxData = 0u;

    #if (LED_Driver_INTERNAL_RX_SW_BUFFER_CONST)
        uint32 locTail;
    #endif /* (LED_Driver_INTERNAL_RX_SW_BUFFER_CONST) */

        #if (LED_Driver_CHECK_RX_SW_BUFFER)
        {
            if (LED_Driver_rxBufferHead != LED_Driver_rxBufferTail)
            {
                /* There is data in RX software buffer */

                /* Calculate index to read from */
                locTail = (LED_Driver_rxBufferTail + 1u);

                if (LED_Driver_INTERNAL_RX_BUFFER_SIZE == locTail)
                {
                    locTail = 0u;
                }

                /* Get data from RX software buffer */
                rxData = LED_Driver_GetWordFromRxBuffer(locTail);

                /* Change index in the buffer */
                LED_Driver_rxBufferTail = locTail;

                #if (LED_Driver_CHECK_UART_RTS_CONTROL_FLOW)
                {
                    /* Check if RX Not Empty is disabled in the interrupt */
                    if (0u == (LED_Driver_INTR_RX_MASK_REG & LED_Driver_INTR_RX_NOT_EMPTY))
                    {
                        /* Enable RX Not Empty interrupt source to continue
                        * receiving data into software buffer.
                        */
                        LED_Driver_INTR_RX_MASK_REG |= LED_Driver_INTR_RX_NOT_EMPTY;
                    }
                }
                #endif

            }
        }
        #else
        {
            /* Read data from RX FIFO */
            rxData = LED_Driver_RX_FIFO_RD_REG;
        }
        #endif

        return (rxData);
    }


    /*******************************************************************************
    * Function Name: LED_Driver_SpiUartGetRxBufferSize
    ****************************************************************************//**
    *
    *  Returns the number of received data elements in the receive buffer.
    *   - RX software buffer disabled: returns the number of used entries in
    *     RX FIFO.
    *   - RX software buffer enabled: returns the number of elements which were
    *     placed in the receive buffer. This does not include the hardware RX FIFO.
    *
    * \return
    *  Number of received data elements.
    *
    * \globalvars
    *  LED_Driver_rxBufferHead - the start index to put data into the 
    *  software receive buffer.
    *  LED_Driver_rxBufferTail - the start index to get data from the 
    *  software receive buffer.
    *
    *******************************************************************************/
    uint32 LED_Driver_SpiUartGetRxBufferSize(void)
    {
        uint32 size;
    #if (LED_Driver_INTERNAL_RX_SW_BUFFER_CONST)
        uint32 locHead;
    #endif /* (LED_Driver_INTERNAL_RX_SW_BUFFER_CONST) */

        #if (LED_Driver_CHECK_RX_SW_BUFFER)
        {
            locHead = LED_Driver_rxBufferHead;

            if(locHead >= LED_Driver_rxBufferTail)
            {
                size = (locHead - LED_Driver_rxBufferTail);
            }
            else
            {
                size = (locHead + (LED_Driver_INTERNAL_RX_BUFFER_SIZE - LED_Driver_rxBufferTail));
            }
        }
        #else
        {
            size = LED_Driver_GET_RX_FIFO_ENTRIES;
        }
        #endif

        return (size);
    }


    /*******************************************************************************
    * Function Name: LED_Driver_SpiUartClearRxBuffer
    ****************************************************************************//**
    *
    *  Clears the receive buffer and RX FIFO.
    *
    * \globalvars
    *  LED_Driver_rxBufferHead - the start index to put data into the 
    *  software receive buffer.
    *  LED_Driver_rxBufferTail - the start index to get data from the 
    *  software receive buffer.
    *
    *******************************************************************************/
    void LED_Driver_SpiUartClearRxBuffer(void)
    {
        #if (LED_Driver_CHECK_RX_SW_BUFFER)
        {
            /* Lock from component interruption */
            LED_Driver_DisableInt();

            /* Flush RX software buffer */
            LED_Driver_rxBufferHead = LED_Driver_rxBufferTail;
            LED_Driver_rxBufferOverflow = 0u;

            LED_Driver_CLEAR_RX_FIFO;
            LED_Driver_ClearRxInterruptSource(LED_Driver_INTR_RX_ALL);

            #if (LED_Driver_CHECK_UART_RTS_CONTROL_FLOW)
            {
                /* Enable RX Not Empty interrupt source to continue receiving
                * data into software buffer.
                */
                LED_Driver_INTR_RX_MASK_REG |= LED_Driver_INTR_RX_NOT_EMPTY;
            }
            #endif
            
            /* Release lock */
            LED_Driver_EnableInt();
        }
        #else
        {
            LED_Driver_CLEAR_RX_FIFO;
        }
        #endif
    }

#endif /* (LED_Driver_RX_DIRECTION) */


#if(LED_Driver_TX_DIRECTION)
    /*******************************************************************************
    * Function Name: LED_Driver_SpiUartWriteTxData
    ****************************************************************************//**
    *
    *  Places a data entry into the transmit buffer to be sent at the next available
    *  bus time.
    *  This function is blocking and waits until there is space available to put the
    *  requested data in the transmit buffer.
    *
    *  \param txDataByte: the data to be transmitted.
    *   The amount of data bits to be transmitted depends on TX data bits selection 
    *   (the data bit counting starts from LSB of txDataByte).
    *
    * \globalvars
    *  LED_Driver_txBufferHead - the start index to put data into the 
    *  software transmit buffer.
    *  LED_Driver_txBufferTail - start index to get data from the software
    *  transmit buffer.
    *
    *******************************************************************************/
    void LED_Driver_SpiUartWriteTxData(uint32 txData)
    {
    #if (LED_Driver_INTERNAL_TX_SW_BUFFER_CONST)
        uint32 locHead;
    #endif /* (LED_Driver_INTERNAL_TX_SW_BUFFER_CONST) */

        #if (LED_Driver_CHECK_TX_SW_BUFFER)
        {
            /* Put data directly into the TX FIFO */
            if ((LED_Driver_txBufferHead == LED_Driver_txBufferTail) &&
                (LED_Driver_SPI_UART_FIFO_SIZE != LED_Driver_GET_TX_FIFO_ENTRIES))
            {
                /* TX software buffer is empty: put data directly in TX FIFO */
                LED_Driver_TX_FIFO_WR_REG = txData;
            }
            /* Put data into TX software buffer */
            else
            {
                /* Head index to put data */
                locHead = (LED_Driver_txBufferHead + 1u);

                /* Adjust TX software buffer index */
                if (LED_Driver_TX_BUFFER_SIZE == locHead)
                {
                    locHead = 0u;
                }

                /* Wait for space in TX software buffer */
                while (locHead == LED_Driver_txBufferTail)
                {
                }

                /* TX software buffer has at least one room */

                /* Clear old status of INTR_TX_NOT_FULL. It sets at the end of transfer when TX FIFO is empty. */
                LED_Driver_ClearTxInterruptSource(LED_Driver_INTR_TX_NOT_FULL);

                LED_Driver_PutWordInTxBuffer(locHead, txData);

                LED_Driver_txBufferHead = locHead;

                /* Check if TX Not Full is disabled in interrupt */
                if (0u == (LED_Driver_INTR_TX_MASK_REG & LED_Driver_INTR_TX_NOT_FULL))
                {
                    /* Enable TX Not Full interrupt source to transmit from software buffer */
                    LED_Driver_INTR_TX_MASK_REG |= (uint32) LED_Driver_INTR_TX_NOT_FULL;
                }
            }
        }
        #else
        {
            /* Wait until TX FIFO has space to put data element */
            while (LED_Driver_SPI_UART_FIFO_SIZE == LED_Driver_GET_TX_FIFO_ENTRIES)
            {
            }

            LED_Driver_TX_FIFO_WR_REG = txData;
        }
        #endif
    }


    /*******************************************************************************
    * Function Name: LED_Driver_SpiUartPutArray
    ****************************************************************************//**
    *
    *  Places an array of data into the transmit buffer to be sent.
    *  This function is blocking and waits until there is a space available to put
    *  all the requested data in the transmit buffer. The array size can be greater
    *  than transmit buffer size.
    *
    * \param wrBuf: pointer to an array of data to be placed in transmit buffer. 
    *  The width of the data to be transmitted depends on TX data width selection 
    *  (the data bit counting starts from LSB for each array element).
    * \param count: number of data elements to be placed in the transmit buffer.
    *
    * \globalvars
    *  LED_Driver_txBufferHead - the start index to put data into the 
    *  software transmit buffer.
    *  LED_Driver_txBufferTail - start index to get data from the software
    *  transmit buffer.
    *
    *******************************************************************************/
    void LED_Driver_SpiUartPutArray(const uint16 wrBuf[], uint32 count)
    {
        uint32 i;

        for (i=0u; i < count; i++)
        {
            LED_Driver_SpiUartWriteTxData((uint32) wrBuf[i]);
        }
    }


    /*******************************************************************************
    * Function Name: LED_Driver_SpiUartGetTxBufferSize
    ****************************************************************************//**
    *
    *  Returns the number of elements currently in the transmit buffer.
    *   - TX software buffer is disabled: returns the number of used entries in
    *     TX FIFO.
    *   - TX software buffer is enabled: returns the number of elements currently
    *     used in the transmit buffer. This number does not include used entries in
    *     the TX FIFO. The transmit buffer size is zero until the TX FIFO is
    *     not full.
    *
    * \return
    *  Number of data elements ready to transmit.
    *
    * \globalvars
    *  LED_Driver_txBufferHead - the start index to put data into the 
    *  software transmit buffer.
    *  LED_Driver_txBufferTail - start index to get data from the software
    *  transmit buffer.
    *
    *******************************************************************************/
    uint32 LED_Driver_SpiUartGetTxBufferSize(void)
    {
        uint32 size;
    #if (LED_Driver_INTERNAL_TX_SW_BUFFER_CONST)
        uint32 locTail;
    #endif /* (LED_Driver_INTERNAL_TX_SW_BUFFER_CONST) */

        #if (LED_Driver_CHECK_TX_SW_BUFFER)
        {
            /* Get current Tail index */
            locTail = LED_Driver_txBufferTail;

            if (LED_Driver_txBufferHead >= locTail)
            {
                size = (LED_Driver_txBufferHead - locTail);
            }
            else
            {
                size = (LED_Driver_txBufferHead + (LED_Driver_TX_BUFFER_SIZE - locTail));
            }
        }
        #else
        {
            size = LED_Driver_GET_TX_FIFO_ENTRIES;
        }
        #endif

        return (size);
    }


    /*******************************************************************************
    * Function Name: LED_Driver_SpiUartClearTxBuffer
    ****************************************************************************//**
    *
    *  Clears the transmit buffer and TX FIFO.
    *
    * \globalvars
    *  LED_Driver_txBufferHead - the start index to put data into the 
    *  software transmit buffer.
    *  LED_Driver_txBufferTail - start index to get data from the software
    *  transmit buffer.
    *
    *******************************************************************************/
    void LED_Driver_SpiUartClearTxBuffer(void)
    {
        #if (LED_Driver_CHECK_TX_SW_BUFFER)
        {
            /* Lock from component interruption */
            LED_Driver_DisableInt();

            /* Flush TX software buffer */
            LED_Driver_txBufferHead = LED_Driver_txBufferTail;

            LED_Driver_INTR_TX_MASK_REG &= (uint32) ~LED_Driver_INTR_TX_NOT_FULL;
            LED_Driver_CLEAR_TX_FIFO;
            LED_Driver_ClearTxInterruptSource(LED_Driver_INTR_TX_ALL);

            /* Release lock */
            LED_Driver_EnableInt();
        }
        #else
        {
            LED_Driver_CLEAR_TX_FIFO;
        }
        #endif
    }

#endif /* (LED_Driver_TX_DIRECTION) */


/*******************************************************************************
* Function Name: LED_Driver_SpiUartDisableIntRx
****************************************************************************//**
*
*  Disables the RX interrupt sources.
*
*  \return
*   Returns the RX interrupt sources enabled before the function call.
*
*******************************************************************************/
uint32 LED_Driver_SpiUartDisableIntRx(void)
{
    uint32 intSource;

    intSource = LED_Driver_GetRxInterruptMode();

    LED_Driver_SetRxInterruptMode(LED_Driver_NO_INTR_SOURCES);

    return (intSource);
}


/*******************************************************************************
* Function Name: LED_Driver_SpiUartDisableIntTx
****************************************************************************//**
*
*  Disables TX interrupt sources.
*
*  \return
*   Returns TX interrupt sources enabled before function call.
*
*******************************************************************************/
uint32 LED_Driver_SpiUartDisableIntTx(void)
{
    uint32 intSourceMask;

    intSourceMask = LED_Driver_GetTxInterruptMode();

    LED_Driver_SetTxInterruptMode(LED_Driver_NO_INTR_SOURCES);

    return (intSourceMask);
}


#if(LED_Driver_SCB_MODE_UNCONFIG_CONST_CFG)
    /*******************************************************************************
    * Function Name: LED_Driver_PutWordInRxBuffer
    ****************************************************************************//**
    *
    *  Stores a byte/word into the RX buffer.
    *  Only available in the Unconfigured operation mode.
    *
    *  \param index:      index to store data byte/word in the RX buffer.
    *  \param rxDataByte: byte/word to store.
    *
    *******************************************************************************/
    void LED_Driver_PutWordInRxBuffer(uint32 idx, uint32 rxDataByte)
    {
        /* Put data in buffer */
        if (LED_Driver_ONE_BYTE_WIDTH == LED_Driver_rxDataBits)
        {
            LED_Driver_rxBuffer[idx] = ((uint8) rxDataByte);
        }
        else
        {
            LED_Driver_rxBuffer[(uint32)(idx << 1u)]      = LO8(LO16(rxDataByte));
            LED_Driver_rxBuffer[(uint32)(idx << 1u) + 1u] = HI8(LO16(rxDataByte));
        }
    }


    /*******************************************************************************
    * Function Name: LED_Driver_GetWordFromRxBuffer
    ****************************************************************************//**
    *
    *  Reads byte/word from RX buffer.
    *  Only available in the Unconfigured operation mode.
    *
    *  \return
    *   Returns byte/word read from RX buffer.
    *
    *******************************************************************************/
    uint32 LED_Driver_GetWordFromRxBuffer(uint32 idx)
    {
        uint32 value;

        if (LED_Driver_ONE_BYTE_WIDTH == LED_Driver_rxDataBits)
        {
            value = LED_Driver_rxBuffer[idx];
        }
        else
        {
            value  = (uint32) LED_Driver_rxBuffer[(uint32)(idx << 1u)];
            value |= (uint32) ((uint32)LED_Driver_rxBuffer[(uint32)(idx << 1u) + 1u] << 8u);
        }

        return (value);
    }


    /*******************************************************************************
    * Function Name: LED_Driver_PutWordInTxBuffer
    ****************************************************************************//**
    *
    *  Stores byte/word into the TX buffer.
    *  Only available in the Unconfigured operation mode.
    *
    *  \param idx:        index to store data byte/word in the TX buffer.
    *  \param txDataByte: byte/word to store.
    *
    *******************************************************************************/
    void LED_Driver_PutWordInTxBuffer(uint32 idx, uint32 txDataByte)
    {
        /* Put data in buffer */
        if (LED_Driver_ONE_BYTE_WIDTH == LED_Driver_txDataBits)
        {
            LED_Driver_txBuffer[idx] = ((uint8) txDataByte);
        }
        else
        {
            LED_Driver_txBuffer[(uint32)(idx << 1u)]      = LO8(LO16(txDataByte));
            LED_Driver_txBuffer[(uint32)(idx << 1u) + 1u] = HI8(LO16(txDataByte));
        }
    }


    /*******************************************************************************
    * Function Name: LED_Driver_GetWordFromTxBuffer
    ****************************************************************************//**
    *
    *  Reads byte/word from the TX buffer.
    *  Only available in the Unconfigured operation mode.
    *
    *  \param idx: index to get data byte/word from the TX buffer.
    *
    *  \return
    *   Returns byte/word read from the TX buffer.
    *
    *******************************************************************************/
    uint32 LED_Driver_GetWordFromTxBuffer(uint32 idx)
    {
        uint32 value;

        if (LED_Driver_ONE_BYTE_WIDTH == LED_Driver_txDataBits)
        {
            value = (uint32) LED_Driver_txBuffer[idx];
        }
        else
        {
            value  = (uint32) LED_Driver_txBuffer[(uint32)(idx << 1u)];
            value |= (uint32) ((uint32) LED_Driver_txBuffer[(uint32)(idx << 1u) + 1u] << 8u);
        }

        return (value);
    }

#endif /* (LED_Driver_SCB_MODE_UNCONFIG_CONST_CFG) */


/* [] END OF FILE */
