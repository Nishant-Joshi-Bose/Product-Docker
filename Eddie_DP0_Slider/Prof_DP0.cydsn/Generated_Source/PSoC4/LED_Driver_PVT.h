/***************************************************************************//**
* \file .h
* \version 3.20
*
* \brief
*  This private file provides constants and parameter values for the
*  SCB Component.
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

#if !defined(CY_SCB_PVT_LED_Driver_H)
#define CY_SCB_PVT_LED_Driver_H

#include "LED_Driver.h"


/***************************************
*     Private Function Prototypes
***************************************/

/* APIs to service INTR_I2C_EC register */
#define LED_Driver_SetI2CExtClkInterruptMode(interruptMask) LED_Driver_WRITE_INTR_I2C_EC_MASK(interruptMask)
#define LED_Driver_ClearI2CExtClkInterruptSource(interruptMask) LED_Driver_CLEAR_INTR_I2C_EC(interruptMask)
#define LED_Driver_GetI2CExtClkInterruptSource()                (LED_Driver_INTR_I2C_EC_REG)
#define LED_Driver_GetI2CExtClkInterruptMode()                  (LED_Driver_INTR_I2C_EC_MASK_REG)
#define LED_Driver_GetI2CExtClkInterruptSourceMasked()          (LED_Driver_INTR_I2C_EC_MASKED_REG)

#if (!LED_Driver_CY_SCBIP_V1)
    /* APIs to service INTR_SPI_EC register */
    #define LED_Driver_SetSpiExtClkInterruptMode(interruptMask) \
                                                                LED_Driver_WRITE_INTR_SPI_EC_MASK(interruptMask)
    #define LED_Driver_ClearSpiExtClkInterruptSource(interruptMask) \
                                                                LED_Driver_CLEAR_INTR_SPI_EC(interruptMask)
    #define LED_Driver_GetExtSpiClkInterruptSource()                 (LED_Driver_INTR_SPI_EC_REG)
    #define LED_Driver_GetExtSpiClkInterruptMode()                   (LED_Driver_INTR_SPI_EC_MASK_REG)
    #define LED_Driver_GetExtSpiClkInterruptSourceMasked()           (LED_Driver_INTR_SPI_EC_MASKED_REG)
#endif /* (!LED_Driver_CY_SCBIP_V1) */

#if(LED_Driver_SCB_MODE_UNCONFIG_CONST_CFG)
    extern void LED_Driver_SetPins(uint32 mode, uint32 subMode, uint32 uartEnableMask);
#endif /* (LED_Driver_SCB_MODE_UNCONFIG_CONST_CFG) */


/***************************************
*     Vars with External Linkage
***************************************/

#if (LED_Driver_SCB_IRQ_INTERNAL)
#if !defined (CY_REMOVE_LED_Driver_CUSTOM_INTR_HANDLER)
    extern cyisraddress LED_Driver_customIntrHandler;
#endif /* !defined (CY_REMOVE_LED_Driver_CUSTOM_INTR_HANDLER) */
#endif /* (LED_Driver_SCB_IRQ_INTERNAL) */

extern LED_Driver_BACKUP_STRUCT LED_Driver_backup;

#if(LED_Driver_SCB_MODE_UNCONFIG_CONST_CFG)
    /* Common configuration variables */
    extern uint8 LED_Driver_scbMode;
    extern uint8 LED_Driver_scbEnableWake;
    extern uint8 LED_Driver_scbEnableIntr;

    /* I2C configuration variables */
    extern uint8 LED_Driver_mode;
    extern uint8 LED_Driver_acceptAddr;

    /* SPI/UART configuration variables */
    extern volatile uint8 * LED_Driver_rxBuffer;
    extern uint8   LED_Driver_rxDataBits;
    extern uint32  LED_Driver_rxBufferSize;

    extern volatile uint8 * LED_Driver_txBuffer;
    extern uint8   LED_Driver_txDataBits;
    extern uint32  LED_Driver_txBufferSize;

    /* EZI2C configuration variables */
    extern uint8 LED_Driver_numberOfAddr;
    extern uint8 LED_Driver_subAddrSize;
#endif /* (LED_Driver_SCB_MODE_UNCONFIG_CONST_CFG) */

#if (! (LED_Driver_SCB_MODE_I2C_CONST_CFG || \
        LED_Driver_SCB_MODE_EZI2C_CONST_CFG))
    extern uint16 LED_Driver_IntrTxMask;
#endif /* (! (LED_Driver_SCB_MODE_I2C_CONST_CFG || \
              LED_Driver_SCB_MODE_EZI2C_CONST_CFG)) */


/***************************************
*        Conditional Macro
****************************************/

#if(LED_Driver_SCB_MODE_UNCONFIG_CONST_CFG)
    /* Defines run time operation mode */
    #define LED_Driver_SCB_MODE_I2C_RUNTM_CFG     (LED_Driver_SCB_MODE_I2C      == LED_Driver_scbMode)
    #define LED_Driver_SCB_MODE_SPI_RUNTM_CFG     (LED_Driver_SCB_MODE_SPI      == LED_Driver_scbMode)
    #define LED_Driver_SCB_MODE_UART_RUNTM_CFG    (LED_Driver_SCB_MODE_UART     == LED_Driver_scbMode)
    #define LED_Driver_SCB_MODE_EZI2C_RUNTM_CFG   (LED_Driver_SCB_MODE_EZI2C    == LED_Driver_scbMode)
    #define LED_Driver_SCB_MODE_UNCONFIG_RUNTM_CFG \
                                                        (LED_Driver_SCB_MODE_UNCONFIG == LED_Driver_scbMode)

    /* Defines wakeup enable */
    #define LED_Driver_SCB_WAKE_ENABLE_CHECK       (0u != LED_Driver_scbEnableWake)
#endif /* (LED_Driver_SCB_MODE_UNCONFIG_CONST_CFG) */

/* Defines maximum number of SCB pins */
#if (!LED_Driver_CY_SCBIP_V1)
    #define LED_Driver_SCB_PINS_NUMBER    (7u)
#else
    #define LED_Driver_SCB_PINS_NUMBER    (2u)
#endif /* (!LED_Driver_CY_SCBIP_V1) */

#endif /* (CY_SCB_PVT_LED_Driver_H) */


/* [] END OF FILE */
