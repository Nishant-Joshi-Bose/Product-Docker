/***************************************************************************//**
* \file LED_Driver_BOOT.h
* \version 3.20
*
* \brief
*  This file provides constants and parameter values of the bootloader
*  communication APIs for the SCB Component.
*
* Note:
*
********************************************************************************
* \copyright
* Copyright 2014-2016, Cypress Semiconductor Corporation. All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_SCB_BOOT_LED_Driver_H)
#define CY_SCB_BOOT_LED_Driver_H

#include "LED_Driver_PVT.h"

#if (LED_Driver_SCB_MODE_I2C_INC)
    #include "LED_Driver_I2C.h"
#endif /* (LED_Driver_SCB_MODE_I2C_INC) */

#if (LED_Driver_SCB_MODE_EZI2C_INC)
    #include "LED_Driver_EZI2C.h"
#endif /* (LED_Driver_SCB_MODE_EZI2C_INC) */

#if (LED_Driver_SCB_MODE_SPI_INC || LED_Driver_SCB_MODE_UART_INC)
    #include "LED_Driver_SPI_UART.h"
#endif /* (LED_Driver_SCB_MODE_SPI_INC || LED_Driver_SCB_MODE_UART_INC) */


/***************************************
*  Conditional Compilation Parameters
****************************************/

/* Bootloader communication interface enable */
#define LED_Driver_BTLDR_COMM_ENABLED ((CYDEV_BOOTLOADER_IO_COMP == CyBtldr_LED_Driver) || \
                                             (CYDEV_BOOTLOADER_IO_COMP == CyBtldr_Custom_Interface))

/* Enable I2C bootloader communication */
#if (LED_Driver_SCB_MODE_I2C_INC)
    #define LED_Driver_I2C_BTLDR_COMM_ENABLED     (LED_Driver_BTLDR_COMM_ENABLED && \
                                                            (LED_Driver_SCB_MODE_UNCONFIG_CONST_CFG || \
                                                             LED_Driver_I2C_SLAVE_CONST))
#else
     #define LED_Driver_I2C_BTLDR_COMM_ENABLED    (0u)
#endif /* (LED_Driver_SCB_MODE_I2C_INC) */

/* EZI2C does not support bootloader communication. Provide empty APIs */
#if (LED_Driver_SCB_MODE_EZI2C_INC)
    #define LED_Driver_EZI2C_BTLDR_COMM_ENABLED   (LED_Driver_BTLDR_COMM_ENABLED && \
                                                         LED_Driver_SCB_MODE_UNCONFIG_CONST_CFG)
#else
    #define LED_Driver_EZI2C_BTLDR_COMM_ENABLED   (0u)
#endif /* (LED_Driver_EZI2C_BTLDR_COMM_ENABLED) */

/* Enable SPI bootloader communication */
#if (LED_Driver_SCB_MODE_SPI_INC)
    #define LED_Driver_SPI_BTLDR_COMM_ENABLED     (LED_Driver_BTLDR_COMM_ENABLED && \
                                                            (LED_Driver_SCB_MODE_UNCONFIG_CONST_CFG || \
                                                             LED_Driver_SPI_SLAVE_CONST))
#else
        #define LED_Driver_SPI_BTLDR_COMM_ENABLED (0u)
#endif /* (LED_Driver_SPI_BTLDR_COMM_ENABLED) */

/* Enable UART bootloader communication */
#if (LED_Driver_SCB_MODE_UART_INC)
       #define LED_Driver_UART_BTLDR_COMM_ENABLED    (LED_Driver_BTLDR_COMM_ENABLED && \
                                                            (LED_Driver_SCB_MODE_UNCONFIG_CONST_CFG || \
                                                             (LED_Driver_UART_RX_DIRECTION && \
                                                              LED_Driver_UART_TX_DIRECTION)))
#else
     #define LED_Driver_UART_BTLDR_COMM_ENABLED   (0u)
#endif /* (LED_Driver_UART_BTLDR_COMM_ENABLED) */

/* Enable bootloader communication */
#define LED_Driver_BTLDR_COMM_MODE_ENABLED    (LED_Driver_I2C_BTLDR_COMM_ENABLED   || \
                                                     LED_Driver_SPI_BTLDR_COMM_ENABLED   || \
                                                     LED_Driver_EZI2C_BTLDR_COMM_ENABLED || \
                                                     LED_Driver_UART_BTLDR_COMM_ENABLED)


/***************************************
*        Function Prototypes
***************************************/

#if defined(CYDEV_BOOTLOADER_IO_COMP) && (LED_Driver_I2C_BTLDR_COMM_ENABLED)
    /* I2C Bootloader physical layer functions */
    void LED_Driver_I2CCyBtldrCommStart(void);
    void LED_Driver_I2CCyBtldrCommStop (void);
    void LED_Driver_I2CCyBtldrCommReset(void);
    cystatus LED_Driver_I2CCyBtldrCommRead       (uint8 pData[], uint16 size, uint16 * count, uint8 timeOut);
    cystatus LED_Driver_I2CCyBtldrCommWrite(const uint8 pData[], uint16 size, uint16 * count, uint8 timeOut);

    /* Map I2C specific bootloader communication APIs to SCB specific APIs */
    #if (LED_Driver_SCB_MODE_I2C_CONST_CFG)
        #define LED_Driver_CyBtldrCommStart   LED_Driver_I2CCyBtldrCommStart
        #define LED_Driver_CyBtldrCommStop    LED_Driver_I2CCyBtldrCommStop
        #define LED_Driver_CyBtldrCommReset   LED_Driver_I2CCyBtldrCommReset
        #define LED_Driver_CyBtldrCommRead    LED_Driver_I2CCyBtldrCommRead
        #define LED_Driver_CyBtldrCommWrite   LED_Driver_I2CCyBtldrCommWrite
    #endif /* (LED_Driver_SCB_MODE_I2C_CONST_CFG) */

#endif /* defined(CYDEV_BOOTLOADER_IO_COMP) && (LED_Driver_I2C_BTLDR_COMM_ENABLED) */


#if defined(CYDEV_BOOTLOADER_IO_COMP) && (LED_Driver_EZI2C_BTLDR_COMM_ENABLED)
    /* Bootloader physical layer functions */
    void LED_Driver_EzI2CCyBtldrCommStart(void);
    void LED_Driver_EzI2CCyBtldrCommStop (void);
    void LED_Driver_EzI2CCyBtldrCommReset(void);
    cystatus LED_Driver_EzI2CCyBtldrCommRead       (uint8 pData[], uint16 size, uint16 * count, uint8 timeOut);
    cystatus LED_Driver_EzI2CCyBtldrCommWrite(const uint8 pData[], uint16 size, uint16 * count, uint8 timeOut);

    /* Map EZI2C specific bootloader communication APIs to SCB specific APIs */
    #if (LED_Driver_SCB_MODE_EZI2C_CONST_CFG)
        #define LED_Driver_CyBtldrCommStart   LED_Driver_EzI2CCyBtldrCommStart
        #define LED_Driver_CyBtldrCommStop    LED_Driver_EzI2CCyBtldrCommStop
        #define LED_Driver_CyBtldrCommReset   LED_Driver_EzI2CCyBtldrCommReset
        #define LED_Driver_CyBtldrCommRead    LED_Driver_EzI2CCyBtldrCommRead
        #define LED_Driver_CyBtldrCommWrite   LED_Driver_EzI2CCyBtldrCommWrite
    #endif /* (LED_Driver_SCB_MODE_EZI2C_CONST_CFG) */

#endif /* defined(CYDEV_BOOTLOADER_IO_COMP) && (LED_Driver_EZI2C_BTLDR_COMM_ENABLED) */

#if defined(CYDEV_BOOTLOADER_IO_COMP) && (LED_Driver_SPI_BTLDR_COMM_ENABLED)
    /* SPI Bootloader physical layer functions */
    void LED_Driver_SpiCyBtldrCommStart(void);
    void LED_Driver_SpiCyBtldrCommStop (void);
    void LED_Driver_SpiCyBtldrCommReset(void);
    cystatus LED_Driver_SpiCyBtldrCommRead       (uint8 pData[], uint16 size, uint16 * count, uint8 timeOut);
    cystatus LED_Driver_SpiCyBtldrCommWrite(const uint8 pData[], uint16 size, uint16 * count, uint8 timeOut);

    /* Map SPI specific bootloader communication APIs to SCB specific APIs */
    #if (LED_Driver_SCB_MODE_SPI_CONST_CFG)
        #define LED_Driver_CyBtldrCommStart   LED_Driver_SpiCyBtldrCommStart
        #define LED_Driver_CyBtldrCommStop    LED_Driver_SpiCyBtldrCommStop
        #define LED_Driver_CyBtldrCommReset   LED_Driver_SpiCyBtldrCommReset
        #define LED_Driver_CyBtldrCommRead    LED_Driver_SpiCyBtldrCommRead
        #define LED_Driver_CyBtldrCommWrite   LED_Driver_SpiCyBtldrCommWrite
    #endif /* (LED_Driver_SCB_MODE_SPI_CONST_CFG) */

#endif /* defined(CYDEV_BOOTLOADER_IO_COMP) && (LED_Driver_SPI_BTLDR_COMM_ENABLED) */

#if defined(CYDEV_BOOTLOADER_IO_COMP) && (LED_Driver_UART_BTLDR_COMM_ENABLED)
    /* UART Bootloader physical layer functions */
    void LED_Driver_UartCyBtldrCommStart(void);
    void LED_Driver_UartCyBtldrCommStop (void);
    void LED_Driver_UartCyBtldrCommReset(void);
    cystatus LED_Driver_UartCyBtldrCommRead       (uint8 pData[], uint16 size, uint16 * count, uint8 timeOut);
    cystatus LED_Driver_UartCyBtldrCommWrite(const uint8 pData[], uint16 size, uint16 * count, uint8 timeOut);

    /* Map UART specific bootloader communication APIs to SCB specific APIs */
    #if (LED_Driver_SCB_MODE_UART_CONST_CFG)
        #define LED_Driver_CyBtldrCommStart   LED_Driver_UartCyBtldrCommStart
        #define LED_Driver_CyBtldrCommStop    LED_Driver_UartCyBtldrCommStop
        #define LED_Driver_CyBtldrCommReset   LED_Driver_UartCyBtldrCommReset
        #define LED_Driver_CyBtldrCommRead    LED_Driver_UartCyBtldrCommRead
        #define LED_Driver_CyBtldrCommWrite   LED_Driver_UartCyBtldrCommWrite
    #endif /* (LED_Driver_SCB_MODE_UART_CONST_CFG) */

#endif /* defined(CYDEV_BOOTLOADER_IO_COMP) && (LED_Driver_UART_BTLDR_COMM_ENABLED) */

/**
* \addtogroup group_bootloader
* @{
*/

#if defined(CYDEV_BOOTLOADER_IO_COMP) && (LED_Driver_BTLDR_COMM_ENABLED)
    #if (LED_Driver_SCB_MODE_UNCONFIG_CONST_CFG)
        /* Bootloader physical layer functions */
        void LED_Driver_CyBtldrCommStart(void);
        void LED_Driver_CyBtldrCommStop (void);
        void LED_Driver_CyBtldrCommReset(void);
        cystatus LED_Driver_CyBtldrCommRead       (uint8 pData[], uint16 size, uint16 * count, uint8 timeOut);
        cystatus LED_Driver_CyBtldrCommWrite(const uint8 pData[], uint16 size, uint16 * count, uint8 timeOut);
    #endif /* (LED_Driver_SCB_MODE_UNCONFIG_CONST_CFG) */

    /* Map SCB specific bootloader communication APIs to common APIs */
    #if (CYDEV_BOOTLOADER_IO_COMP == CyBtldr_LED_Driver)
        #define CyBtldrCommStart    LED_Driver_CyBtldrCommStart
        #define CyBtldrCommStop     LED_Driver_CyBtldrCommStop
        #define CyBtldrCommReset    LED_Driver_CyBtldrCommReset
        #define CyBtldrCommWrite    LED_Driver_CyBtldrCommWrite
        #define CyBtldrCommRead     LED_Driver_CyBtldrCommRead
    #endif /* (CYDEV_BOOTLOADER_IO_COMP == CyBtldr_LED_Driver) */

#endif /* defined(CYDEV_BOOTLOADER_IO_COMP) && (LED_Driver_BTLDR_COMM_ENABLED) */

/** @} group_bootloader */

/***************************************
*           API Constants
***************************************/

/* Timeout unit in milliseconds */
#define LED_Driver_WAIT_1_MS  (1u)

/* Return number of bytes to copy into bootloader buffer */
#define LED_Driver_BYTES_TO_COPY(actBufSize, bufSize) \
                            ( ((uint32)(actBufSize) < (uint32)(bufSize)) ? \
                                ((uint32) (actBufSize)) : ((uint32) (bufSize)) )

/* Size of Read/Write buffers for I2C bootloader  */
#define LED_Driver_I2C_BTLDR_SIZEOF_READ_BUFFER   (64u)
#define LED_Driver_I2C_BTLDR_SIZEOF_WRITE_BUFFER  (64u)

/* Byte to byte time interval: calculated basing on current component
* data rate configuration, can be defined in project if required.
*/
#ifndef LED_Driver_SPI_BYTE_TO_BYTE
    #define LED_Driver_SPI_BYTE_TO_BYTE   (16u)
#endif

/* Byte to byte time interval: calculated basing on current component
* baud rate configuration, can be defined in the project if required.
*/
#ifndef LED_Driver_UART_BYTE_TO_BYTE
    #define LED_Driver_UART_BYTE_TO_BYTE  (2500u)
#endif /* LED_Driver_UART_BYTE_TO_BYTE */

#endif /* (CY_SCB_BOOT_LED_Driver_H) */


/* [] END OF FILE */
