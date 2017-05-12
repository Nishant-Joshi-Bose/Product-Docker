/***************************************************************************//**
* \file LED_Driver_SPI_UART_PVT.h
* \version 3.20
*
* \brief
*  This private file provides constants and parameter values for the
*  SCB Component in SPI and UART modes.
*  Please do not use this file or its content in your project.
*
* Note:
*
********************************************************************************
* \copyright
* Copyright 2013-2016, Cypress Semiconductor Corporation. All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_SCB_SPI_UART_PVT_LED_Driver_H)
#define CY_SCB_SPI_UART_PVT_LED_Driver_H

#include "LED_Driver_SPI_UART.h"


/***************************************
*     Internal Global Vars
***************************************/

#if (LED_Driver_INTERNAL_RX_SW_BUFFER_CONST)
    extern volatile uint32  LED_Driver_rxBufferHead;
    extern volatile uint32  LED_Driver_rxBufferTail;
    
    /**
    * \addtogroup group_globals
    * @{
    */
    
    /** Sets when internal software receive buffer overflow
     *  was occurred.
    */  
    extern volatile uint8   LED_Driver_rxBufferOverflow;
    /** @} globals */
#endif /* (LED_Driver_INTERNAL_RX_SW_BUFFER_CONST) */

#if (LED_Driver_INTERNAL_TX_SW_BUFFER_CONST)
    extern volatile uint32  LED_Driver_txBufferHead;
    extern volatile uint32  LED_Driver_txBufferTail;
#endif /* (LED_Driver_INTERNAL_TX_SW_BUFFER_CONST) */

#if (LED_Driver_INTERNAL_RX_SW_BUFFER)
    extern volatile uint16 LED_Driver_rxBufferInternal[LED_Driver_INTERNAL_RX_BUFFER_SIZE];
#endif /* (LED_Driver_INTERNAL_RX_SW_BUFFER) */

#if (LED_Driver_INTERNAL_TX_SW_BUFFER)
    extern volatile uint16 LED_Driver_txBufferInternal[LED_Driver_TX_BUFFER_SIZE];
#endif /* (LED_Driver_INTERNAL_TX_SW_BUFFER) */


/***************************************
*     Private Function Prototypes
***************************************/

void LED_Driver_SpiPostEnable(void);
void LED_Driver_SpiStop(void);

#if (LED_Driver_SCB_MODE_SPI_CONST_CFG)
    void LED_Driver_SpiInit(void);
#endif /* (LED_Driver_SCB_MODE_SPI_CONST_CFG) */

#if (LED_Driver_SPI_WAKE_ENABLE_CONST)
    void LED_Driver_SpiSaveConfig(void);
    void LED_Driver_SpiRestoreConfig(void);
#endif /* (LED_Driver_SPI_WAKE_ENABLE_CONST) */

void LED_Driver_UartPostEnable(void);
void LED_Driver_UartStop(void);

#if (LED_Driver_SCB_MODE_UART_CONST_CFG)
    void LED_Driver_UartInit(void);
#endif /* (LED_Driver_SCB_MODE_UART_CONST_CFG) */

#if (LED_Driver_UART_WAKE_ENABLE_CONST)
    void LED_Driver_UartSaveConfig(void);
    void LED_Driver_UartRestoreConfig(void);
#endif /* (LED_Driver_UART_WAKE_ENABLE_CONST) */


/***************************************
*         UART API Constants
***************************************/

/* UART RX and TX position to be used in LED_Driver_SetPins() */
#define LED_Driver_UART_RX_PIN_ENABLE    (LED_Driver_UART_RX)
#define LED_Driver_UART_TX_PIN_ENABLE    (LED_Driver_UART_TX)

/* UART RTS and CTS position to be used in  LED_Driver_SetPins() */
#define LED_Driver_UART_RTS_PIN_ENABLE    (0x10u)
#define LED_Driver_UART_CTS_PIN_ENABLE    (0x20u)


/***************************************
* The following code is DEPRECATED and
* must not be used.
***************************************/

/* Interrupt processing */
#define LED_Driver_SpiUartEnableIntRx(intSourceMask)  LED_Driver_SetRxInterruptMode(intSourceMask)
#define LED_Driver_SpiUartEnableIntTx(intSourceMask)  LED_Driver_SetTxInterruptMode(intSourceMask)
uint32  LED_Driver_SpiUartDisableIntRx(void);
uint32  LED_Driver_SpiUartDisableIntTx(void);


#endif /* (CY_SCB_SPI_UART_PVT_LED_Driver_H) */


/* [] END OF FILE */
