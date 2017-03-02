/***************************************************************************//**
* \file LED_Driver_PM.c
* \version 3.20
*
* \brief
*  This file provides the source code to the Power Management support for
*  the SCB Component.
*
* Note:
*
********************************************************************************
* \copyright
* Copyright 2013-2016, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "LED_Driver.h"
#include "LED_Driver_PVT.h"

#if(LED_Driver_SCB_MODE_I2C_INC)
    #include "LED_Driver_I2C_PVT.h"
#endif /* (LED_Driver_SCB_MODE_I2C_INC) */

#if(LED_Driver_SCB_MODE_EZI2C_INC)
    #include "LED_Driver_EZI2C_PVT.h"
#endif /* (LED_Driver_SCB_MODE_EZI2C_INC) */

#if(LED_Driver_SCB_MODE_SPI_INC || LED_Driver_SCB_MODE_UART_INC)
    #include "LED_Driver_SPI_UART_PVT.h"
#endif /* (LED_Driver_SCB_MODE_SPI_INC || LED_Driver_SCB_MODE_UART_INC) */


/***************************************
*   Backup Structure declaration
***************************************/

#if(LED_Driver_SCB_MODE_UNCONFIG_CONST_CFG || \
   (LED_Driver_SCB_MODE_I2C_CONST_CFG   && (!LED_Driver_I2C_WAKE_ENABLE_CONST))   || \
   (LED_Driver_SCB_MODE_EZI2C_CONST_CFG && (!LED_Driver_EZI2C_WAKE_ENABLE_CONST)) || \
   (LED_Driver_SCB_MODE_SPI_CONST_CFG   && (!LED_Driver_SPI_WAKE_ENABLE_CONST))   || \
   (LED_Driver_SCB_MODE_UART_CONST_CFG  && (!LED_Driver_UART_WAKE_ENABLE_CONST)))

    LED_Driver_BACKUP_STRUCT LED_Driver_backup =
    {
        0u, /* enableState */
    };
#endif


/*******************************************************************************
* Function Name: LED_Driver_Sleep
****************************************************************************//**
*
*  Prepares the LED_Driver component to enter Deep Sleep.
*  The “Enable wakeup from Deep Sleep Mode” selection has an influence on this 
*  function implementation:
*  - Checked: configures the component to be wakeup source from Deep Sleep.
*  - Unchecked: stores the current component state (enabled or disabled) and 
*    disables the component. See SCB_Stop() function for details about component 
*    disabling.
*
*  Call the LED_Driver_Sleep() function before calling the 
*  CyPmSysDeepSleep() function. 
*  Refer to the PSoC Creator System Reference Guide for more information about 
*  power management functions and Low power section of this document for the 
*  selected mode.
*
*  This function should not be called before entering Sleep.
*
*******************************************************************************/
void LED_Driver_Sleep(void)
{
#if(LED_Driver_SCB_MODE_UNCONFIG_CONST_CFG)

    if(LED_Driver_SCB_WAKE_ENABLE_CHECK)
    {
        if(LED_Driver_SCB_MODE_I2C_RUNTM_CFG)
        {
            LED_Driver_I2CSaveConfig();
        }
        else if(LED_Driver_SCB_MODE_EZI2C_RUNTM_CFG)
        {
            LED_Driver_EzI2CSaveConfig();
        }
    #if(!LED_Driver_CY_SCBIP_V1)
        else if(LED_Driver_SCB_MODE_SPI_RUNTM_CFG)
        {
            LED_Driver_SpiSaveConfig();
        }
        else if(LED_Driver_SCB_MODE_UART_RUNTM_CFG)
        {
            LED_Driver_UartSaveConfig();
        }
    #endif /* (!LED_Driver_CY_SCBIP_V1) */
        else
        {
            /* Unknown mode */
        }
    }
    else
    {
        LED_Driver_backup.enableState = (uint8) LED_Driver_GET_CTRL_ENABLED;

        if(0u != LED_Driver_backup.enableState)
        {
            LED_Driver_Stop();
        }
    }

#else

    #if (LED_Driver_SCB_MODE_I2C_CONST_CFG && LED_Driver_I2C_WAKE_ENABLE_CONST)
        LED_Driver_I2CSaveConfig();

    #elif (LED_Driver_SCB_MODE_EZI2C_CONST_CFG && LED_Driver_EZI2C_WAKE_ENABLE_CONST)
        LED_Driver_EzI2CSaveConfig();

    #elif (LED_Driver_SCB_MODE_SPI_CONST_CFG && LED_Driver_SPI_WAKE_ENABLE_CONST)
        LED_Driver_SpiSaveConfig();

    #elif (LED_Driver_SCB_MODE_UART_CONST_CFG && LED_Driver_UART_WAKE_ENABLE_CONST)
        LED_Driver_UartSaveConfig();

    #else

        LED_Driver_backup.enableState = (uint8) LED_Driver_GET_CTRL_ENABLED;

        if(0u != LED_Driver_backup.enableState)
        {
            LED_Driver_Stop();
        }

    #endif /* defined (LED_Driver_SCB_MODE_I2C_CONST_CFG) && (LED_Driver_I2C_WAKE_ENABLE_CONST) */

#endif /* (LED_Driver_SCB_MODE_UNCONFIG_CONST_CFG) */
}


/*******************************************************************************
* Function Name: LED_Driver_Wakeup
****************************************************************************//**
*
*  Prepares the LED_Driver component for Active mode operation after 
*  Deep Sleep.
*  The “Enable wakeup from Deep Sleep Mode” selection has influence on this 
*  function implementation:
*  - Checked: restores the component Active mode configuration.
*  - Unchecked: enables the component if it was enabled before enter Deep Sleep.
*
*  This function should not be called after exiting Sleep.
*
*  \sideeffect
*   Calling the LED_Driver_Wakeup() function without first calling the 
*   LED_Driver_Sleep() function may produce unexpected behavior.
*
*******************************************************************************/
void LED_Driver_Wakeup(void)
{
#if(LED_Driver_SCB_MODE_UNCONFIG_CONST_CFG)

    if(LED_Driver_SCB_WAKE_ENABLE_CHECK)
    {
        if(LED_Driver_SCB_MODE_I2C_RUNTM_CFG)
        {
            LED_Driver_I2CRestoreConfig();
        }
        else if(LED_Driver_SCB_MODE_EZI2C_RUNTM_CFG)
        {
            LED_Driver_EzI2CRestoreConfig();
        }
    #if(!LED_Driver_CY_SCBIP_V1)
        else if(LED_Driver_SCB_MODE_SPI_RUNTM_CFG)
        {
            LED_Driver_SpiRestoreConfig();
        }
        else if(LED_Driver_SCB_MODE_UART_RUNTM_CFG)
        {
            LED_Driver_UartRestoreConfig();
        }
    #endif /* (!LED_Driver_CY_SCBIP_V1) */
        else
        {
            /* Unknown mode */
        }
    }
    else
    {
        if(0u != LED_Driver_backup.enableState)
        {
            LED_Driver_Enable();
        }
    }

#else

    #if (LED_Driver_SCB_MODE_I2C_CONST_CFG  && LED_Driver_I2C_WAKE_ENABLE_CONST)
        LED_Driver_I2CRestoreConfig();

    #elif (LED_Driver_SCB_MODE_EZI2C_CONST_CFG && LED_Driver_EZI2C_WAKE_ENABLE_CONST)
        LED_Driver_EzI2CRestoreConfig();

    #elif (LED_Driver_SCB_MODE_SPI_CONST_CFG && LED_Driver_SPI_WAKE_ENABLE_CONST)
        LED_Driver_SpiRestoreConfig();

    #elif (LED_Driver_SCB_MODE_UART_CONST_CFG && LED_Driver_UART_WAKE_ENABLE_CONST)
        LED_Driver_UartRestoreConfig();

    #else

        if(0u != LED_Driver_backup.enableState)
        {
            LED_Driver_Enable();
        }

    #endif /* (LED_Driver_I2C_WAKE_ENABLE_CONST) */

#endif /* (LED_Driver_SCB_MODE_UNCONFIG_CONST_CFG) */
}


/* [] END OF FILE */
