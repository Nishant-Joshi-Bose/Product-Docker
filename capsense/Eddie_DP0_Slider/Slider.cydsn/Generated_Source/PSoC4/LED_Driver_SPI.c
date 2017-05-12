/***************************************************************************//**
* \file LED_Driver_SPI.c
* \version 3.20
*
* \brief
*  This file provides the source code to the API for the SCB Component in
*  SPI mode.
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

#if(LED_Driver_SCB_MODE_UNCONFIG_CONST_CFG)

    /***************************************
    *  Configuration Structure Initialization
    ***************************************/

    const LED_Driver_SPI_INIT_STRUCT LED_Driver_configSpi =
    {
        LED_Driver_SPI_MODE,
        LED_Driver_SPI_SUB_MODE,
        LED_Driver_SPI_CLOCK_MODE,
        LED_Driver_SPI_OVS_FACTOR,
        LED_Driver_SPI_MEDIAN_FILTER_ENABLE,
        LED_Driver_SPI_LATE_MISO_SAMPLE_ENABLE,
        LED_Driver_SPI_WAKE_ENABLE,
        LED_Driver_SPI_RX_DATA_BITS_NUM,
        LED_Driver_SPI_TX_DATA_BITS_NUM,
        LED_Driver_SPI_BITS_ORDER,
        LED_Driver_SPI_TRANSFER_SEPARATION,
        0u,
        NULL,
        0u,
        NULL,
        (uint32) LED_Driver_SCB_IRQ_INTERNAL,
        LED_Driver_SPI_INTR_RX_MASK,
        LED_Driver_SPI_RX_TRIGGER_LEVEL,
        LED_Driver_SPI_INTR_TX_MASK,
        LED_Driver_SPI_TX_TRIGGER_LEVEL,
        (uint8) LED_Driver_SPI_BYTE_MODE_ENABLE,
        (uint8) LED_Driver_SPI_FREE_RUN_SCLK_ENABLE,
        (uint8) LED_Driver_SPI_SS_POLARITY
    };


    /*******************************************************************************
    * Function Name: LED_Driver_SpiInit
    ****************************************************************************//**
    *
    *  Configures the LED_Driver for SPI operation.
    *
    *  This function is intended specifically to be used when the LED_Driver 
    *  configuration is set to “Unconfigured LED_Driver” in the customizer. 
    *  After initializing the LED_Driver in SPI mode using this function, 
    *  the component can be enabled using the LED_Driver_Start() or 
    * LED_Driver_Enable() function.
    *  This function uses a pointer to a structure that provides the configuration 
    *  settings. This structure contains the same information that would otherwise 
    *  be provided by the customizer settings.
    *
    *  \param config: pointer to a structure that contains the following list of 
    *   fields. These fields match the selections available in the customizer. 
    *   Refer to the customizer for further description of the settings.
    *
    *******************************************************************************/
    void LED_Driver_SpiInit(const LED_Driver_SPI_INIT_STRUCT *config)
    {
        if(NULL == config)
        {
            CYASSERT(0u != 0u); /* Halt execution due to bad function parameter */
        }
        else
        {
            /* Configure pins */
            LED_Driver_SetPins(LED_Driver_SCB_MODE_SPI, config->mode, LED_Driver_DUMMY_PARAM);

            /* Store internal configuration */
            LED_Driver_scbMode       = (uint8) LED_Driver_SCB_MODE_SPI;
            LED_Driver_scbEnableWake = (uint8) config->enableWake;
            LED_Driver_scbEnableIntr = (uint8) config->enableInterrupt;

            /* Set RX direction internal variables */
            LED_Driver_rxBuffer      =         config->rxBuffer;
            LED_Driver_rxDataBits    = (uint8) config->rxDataBits;
            LED_Driver_rxBufferSize  = (uint8) config->rxBufferSize;

            /* Set TX direction internal variables */
            LED_Driver_txBuffer      =         config->txBuffer;
            LED_Driver_txDataBits    = (uint8) config->txDataBits;
            LED_Driver_txBufferSize  = (uint8) config->txBufferSize;

            /* Configure SPI interface */
            LED_Driver_CTRL_REG     = LED_Driver_GET_CTRL_OVS(config->oversample)           |
                                            LED_Driver_GET_CTRL_BYTE_MODE(config->enableByteMode) |
                                            LED_Driver_GET_CTRL_EC_AM_MODE(config->enableWake)    |
                                            LED_Driver_CTRL_SPI;

            LED_Driver_SPI_CTRL_REG = LED_Driver_GET_SPI_CTRL_CONTINUOUS    (config->transferSeperation)  |
                                            LED_Driver_GET_SPI_CTRL_SELECT_PRECEDE(config->submode &
                                                                          LED_Driver_SPI_MODE_TI_PRECEDES_MASK) |
                                            LED_Driver_GET_SPI_CTRL_SCLK_MODE     (config->sclkMode)            |
                                            LED_Driver_GET_SPI_CTRL_LATE_MISO_SAMPLE(config->enableLateSampling)|
                                            LED_Driver_GET_SPI_CTRL_SCLK_CONTINUOUS(config->enableFreeRunSclk)  |
                                            LED_Driver_GET_SPI_CTRL_SSEL_POLARITY (config->polaritySs)          |
                                            LED_Driver_GET_SPI_CTRL_SUB_MODE      (config->submode)             |
                                            LED_Driver_GET_SPI_CTRL_MASTER_MODE   (config->mode);

            /* Configure RX direction */
            LED_Driver_RX_CTRL_REG     =  LED_Driver_GET_RX_CTRL_DATA_WIDTH(config->rxDataBits)         |
                                                LED_Driver_GET_RX_CTRL_BIT_ORDER (config->bitOrder)           |
                                                LED_Driver_GET_RX_CTRL_MEDIAN    (config->enableMedianFilter) |
                                                LED_Driver_SPI_RX_CTRL;

            LED_Driver_RX_FIFO_CTRL_REG = LED_Driver_GET_RX_FIFO_CTRL_TRIGGER_LEVEL(config->rxTriggerLevel);

            /* Configure TX direction */
            LED_Driver_TX_CTRL_REG      = LED_Driver_GET_TX_CTRL_DATA_WIDTH(config->txDataBits) |
                                                LED_Driver_GET_TX_CTRL_BIT_ORDER (config->bitOrder)   |
                                                LED_Driver_SPI_TX_CTRL;

            LED_Driver_TX_FIFO_CTRL_REG = LED_Driver_GET_TX_FIFO_CTRL_TRIGGER_LEVEL(config->txTriggerLevel);

            /* Configure interrupt with SPI handler but do not enable it */
            CyIntDisable    (LED_Driver_ISR_NUMBER);
            CyIntSetPriority(LED_Driver_ISR_NUMBER, LED_Driver_ISR_PRIORITY);
            (void) CyIntSetVector(LED_Driver_ISR_NUMBER, &LED_Driver_SPI_UART_ISR);

            /* Configure interrupt sources */
            LED_Driver_INTR_I2C_EC_MASK_REG = LED_Driver_NO_INTR_SOURCES;
            LED_Driver_INTR_SPI_EC_MASK_REG = LED_Driver_NO_INTR_SOURCES;
            LED_Driver_INTR_SLAVE_MASK_REG  = LED_Driver_GET_SPI_INTR_SLAVE_MASK(config->rxInterruptMask);
            LED_Driver_INTR_MASTER_MASK_REG = LED_Driver_GET_SPI_INTR_MASTER_MASK(config->txInterruptMask);
            LED_Driver_INTR_RX_MASK_REG     = LED_Driver_GET_SPI_INTR_RX_MASK(config->rxInterruptMask);
            LED_Driver_INTR_TX_MASK_REG     = LED_Driver_GET_SPI_INTR_TX_MASK(config->txInterruptMask);
            
            /* Configure TX interrupt sources to restore. */
            LED_Driver_IntrTxMask = LO16(LED_Driver_INTR_TX_MASK_REG);

            /* Set active SS0 */
            LED_Driver_SpiSetActiveSlaveSelect(LED_Driver_SPI_SLAVE_SELECT0);

            /* Clear RX buffer indexes */
            LED_Driver_rxBufferHead     = 0u;
            LED_Driver_rxBufferTail     = 0u;
            LED_Driver_rxBufferOverflow = 0u;

            /* Clear TX buffer indexes */
            LED_Driver_txBufferHead = 0u;
            LED_Driver_txBufferTail = 0u;
        }
    }

#else

    /*******************************************************************************
    * Function Name: LED_Driver_SpiInit
    ****************************************************************************//**
    *
    *  Configures the SCB for the SPI operation.
    *
    *******************************************************************************/
    void LED_Driver_SpiInit(void)
    {
        /* Configure SPI interface */
        LED_Driver_CTRL_REG     = LED_Driver_SPI_DEFAULT_CTRL;
        LED_Driver_SPI_CTRL_REG = LED_Driver_SPI_DEFAULT_SPI_CTRL;

        /* Configure TX and RX direction */
        LED_Driver_RX_CTRL_REG      = LED_Driver_SPI_DEFAULT_RX_CTRL;
        LED_Driver_RX_FIFO_CTRL_REG = LED_Driver_SPI_DEFAULT_RX_FIFO_CTRL;

        /* Configure TX and RX direction */
        LED_Driver_TX_CTRL_REG      = LED_Driver_SPI_DEFAULT_TX_CTRL;
        LED_Driver_TX_FIFO_CTRL_REG = LED_Driver_SPI_DEFAULT_TX_FIFO_CTRL;

        /* Configure interrupt with SPI handler but do not enable it */
    #if(LED_Driver_SCB_IRQ_INTERNAL)
            CyIntDisable    (LED_Driver_ISR_NUMBER);
            CyIntSetPriority(LED_Driver_ISR_NUMBER, LED_Driver_ISR_PRIORITY);
            (void) CyIntSetVector(LED_Driver_ISR_NUMBER, &LED_Driver_SPI_UART_ISR);
    #endif /* (LED_Driver_SCB_IRQ_INTERNAL) */

        /* Configure interrupt sources */
        LED_Driver_INTR_I2C_EC_MASK_REG = LED_Driver_SPI_DEFAULT_INTR_I2C_EC_MASK;
        LED_Driver_INTR_SPI_EC_MASK_REG = LED_Driver_SPI_DEFAULT_INTR_SPI_EC_MASK;
        LED_Driver_INTR_SLAVE_MASK_REG  = LED_Driver_SPI_DEFAULT_INTR_SLAVE_MASK;
        LED_Driver_INTR_MASTER_MASK_REG = LED_Driver_SPI_DEFAULT_INTR_MASTER_MASK;
        LED_Driver_INTR_RX_MASK_REG     = LED_Driver_SPI_DEFAULT_INTR_RX_MASK;
        LED_Driver_INTR_TX_MASK_REG     = LED_Driver_SPI_DEFAULT_INTR_TX_MASK;

        /* Configure TX interrupt sources to restore. */
        LED_Driver_IntrTxMask = LO16(LED_Driver_INTR_TX_MASK_REG);
            
        /* Set active SS0 for master */
    #if (LED_Driver_SPI_MASTER_CONST)
        LED_Driver_SpiSetActiveSlaveSelect(LED_Driver_SPI_SLAVE_SELECT0);
    #endif /* (LED_Driver_SPI_MASTER_CONST) */

    #if(LED_Driver_INTERNAL_RX_SW_BUFFER_CONST)
        LED_Driver_rxBufferHead     = 0u;
        LED_Driver_rxBufferTail     = 0u;
        LED_Driver_rxBufferOverflow = 0u;
    #endif /* (LED_Driver_INTERNAL_RX_SW_BUFFER_CONST) */

    #if(LED_Driver_INTERNAL_TX_SW_BUFFER_CONST)
        LED_Driver_txBufferHead = 0u;
        LED_Driver_txBufferTail = 0u;
    #endif /* (LED_Driver_INTERNAL_TX_SW_BUFFER_CONST) */
    }
#endif /* (LED_Driver_SCB_MODE_UNCONFIG_CONST_CFG) */


/*******************************************************************************
* Function Name: LED_Driver_SpiPostEnable
****************************************************************************//**
*
*  Restores HSIOM settings for the SPI master output pins (SCLK and/or SS0-SS3) 
*  to be controlled by the SCB SPI.
*
*******************************************************************************/
void LED_Driver_SpiPostEnable(void)
{
#if(LED_Driver_SCB_MODE_UNCONFIG_CONST_CFG)

    if (LED_Driver_CHECK_SPI_MASTER)
    {
    #if (LED_Driver_CTS_SCLK_PIN)
        /* Set SCB SPI to drive output pin */
        LED_Driver_SET_HSIOM_SEL(LED_Driver_CTS_SCLK_HSIOM_REG, LED_Driver_CTS_SCLK_HSIOM_MASK,
                                       LED_Driver_CTS_SCLK_HSIOM_POS, LED_Driver_CTS_SCLK_HSIOM_SEL_SPI);
    #endif /* (LED_Driver_CTS_SCLK_PIN) */

    #if (LED_Driver_RTS_SS0_PIN)
        /* Set SCB SPI to drive output pin */
        LED_Driver_SET_HSIOM_SEL(LED_Driver_RTS_SS0_HSIOM_REG, LED_Driver_RTS_SS0_HSIOM_MASK,
                                       LED_Driver_RTS_SS0_HSIOM_POS, LED_Driver_RTS_SS0_HSIOM_SEL_SPI);
    #endif /* (LED_Driver_RTS_SS0_PIN) */

    #if (LED_Driver_SS1_PIN)
        /* Set SCB SPI to drive output pin */
        LED_Driver_SET_HSIOM_SEL(LED_Driver_SS1_HSIOM_REG, LED_Driver_SS1_HSIOM_MASK,
                                       LED_Driver_SS1_HSIOM_POS, LED_Driver_SS1_HSIOM_SEL_SPI);
    #endif /* (LED_Driver_SS1_PIN) */

    #if (LED_Driver_SS2_PIN)
        /* Set SCB SPI to drive output pin */
        LED_Driver_SET_HSIOM_SEL(LED_Driver_SS2_HSIOM_REG, LED_Driver_SS2_HSIOM_MASK,
                                       LED_Driver_SS2_HSIOM_POS, LED_Driver_SS2_HSIOM_SEL_SPI);
    #endif /* (LED_Driver_SS2_PIN) */

    #if (LED_Driver_SS3_PIN)
        /* Set SCB SPI to drive output pin */
        LED_Driver_SET_HSIOM_SEL(LED_Driver_SS3_HSIOM_REG, LED_Driver_SS3_HSIOM_MASK,
                                       LED_Driver_SS3_HSIOM_POS, LED_Driver_SS3_HSIOM_SEL_SPI);
    #endif /* (LED_Driver_SS3_PIN) */
    }

#else

    #if (LED_Driver_SPI_MASTER_SCLK_PIN)
        /* Set SCB SPI to drive output pin */
        LED_Driver_SET_HSIOM_SEL(LED_Driver_SCLK_M_HSIOM_REG, LED_Driver_SCLK_M_HSIOM_MASK,
                                       LED_Driver_SCLK_M_HSIOM_POS, LED_Driver_SCLK_M_HSIOM_SEL_SPI);
    #endif /* (LED_Driver_MISO_SDA_TX_PIN_PIN) */

    #if (LED_Driver_SPI_MASTER_SS0_PIN)
        /* Set SCB SPI to drive output pin */
        LED_Driver_SET_HSIOM_SEL(LED_Driver_SS0_M_HSIOM_REG, LED_Driver_SS0_M_HSIOM_MASK,
                                       LED_Driver_SS0_M_HSIOM_POS, LED_Driver_SS0_M_HSIOM_SEL_SPI);
    #endif /* (LED_Driver_SPI_MASTER_SS0_PIN) */

    #if (LED_Driver_SPI_MASTER_SS1_PIN)
        /* Set SCB SPI to drive output pin */
        LED_Driver_SET_HSIOM_SEL(LED_Driver_SS1_M_HSIOM_REG, LED_Driver_SS1_M_HSIOM_MASK,
                                       LED_Driver_SS1_M_HSIOM_POS, LED_Driver_SS1_M_HSIOM_SEL_SPI);
    #endif /* (LED_Driver_SPI_MASTER_SS1_PIN) */

    #if (LED_Driver_SPI_MASTER_SS2_PIN)
        /* Set SCB SPI to drive output pin */
        LED_Driver_SET_HSIOM_SEL(LED_Driver_SS2_M_HSIOM_REG, LED_Driver_SS2_M_HSIOM_MASK,
                                       LED_Driver_SS2_M_HSIOM_POS, LED_Driver_SS2_M_HSIOM_SEL_SPI);
    #endif /* (LED_Driver_SPI_MASTER_SS2_PIN) */

    #if (LED_Driver_SPI_MASTER_SS3_PIN)
        /* Set SCB SPI to drive output pin */
        LED_Driver_SET_HSIOM_SEL(LED_Driver_SS3_M_HSIOM_REG, LED_Driver_SS3_M_HSIOM_MASK,
                                       LED_Driver_SS3_M_HSIOM_POS, LED_Driver_SS3_M_HSIOM_SEL_SPI);
    #endif /* (LED_Driver_SPI_MASTER_SS3_PIN) */

#endif /* (LED_Driver_SCB_MODE_UNCONFIG_CONST_CFG) */

    /* Restore TX interrupt sources. */
    LED_Driver_SetTxInterruptMode(LED_Driver_IntrTxMask);
}


/*******************************************************************************
* Function Name: LED_Driver_SpiStop
****************************************************************************//**
*
*  Changes the HSIOM settings for the SPI master output pins 
*  (SCLK and/or SS0-SS3) to keep them inactive after the block is disabled. 
*  The output pins are controlled by the GPIO data register.
*
*******************************************************************************/
void LED_Driver_SpiStop(void)
{
#if(LED_Driver_SCB_MODE_UNCONFIG_CONST_CFG)

    if (LED_Driver_CHECK_SPI_MASTER)
    {
    #if (LED_Driver_CTS_SCLK_PIN)
        /* Set output pin state after block is disabled */
        LED_Driver_uart_cts_spi_sclk_Write(LED_Driver_GET_SPI_SCLK_INACTIVE);

        /* Set GPIO to drive output pin */
        LED_Driver_SET_HSIOM_SEL(LED_Driver_CTS_SCLK_HSIOM_REG, LED_Driver_CTS_SCLK_HSIOM_MASK,
                                       LED_Driver_CTS_SCLK_HSIOM_POS, LED_Driver_CTS_SCLK_HSIOM_SEL_GPIO);
    #endif /* (LED_Driver_uart_cts_spi_sclk_PIN) */

    #if (LED_Driver_RTS_SS0_PIN)
        /* Set output pin state after block is disabled */
        LED_Driver_uart_rts_spi_ss0_Write(LED_Driver_GET_SPI_SS0_INACTIVE);

        /* Set GPIO to drive output pin */
        LED_Driver_SET_HSIOM_SEL(LED_Driver_RTS_SS0_HSIOM_REG, LED_Driver_RTS_SS0_HSIOM_MASK,
                                       LED_Driver_RTS_SS0_HSIOM_POS, LED_Driver_RTS_SS0_HSIOM_SEL_GPIO);
    #endif /* (LED_Driver_uart_rts_spi_ss0_PIN) */

    #if (LED_Driver_SS1_PIN)
        /* Set output pin state after block is disabled */
        LED_Driver_spi_ss1_Write(LED_Driver_GET_SPI_SS1_INACTIVE);

        /* Set GPIO to drive output pin */
        LED_Driver_SET_HSIOM_SEL(LED_Driver_SS1_HSIOM_REG, LED_Driver_SS1_HSIOM_MASK,
                                       LED_Driver_SS1_HSIOM_POS, LED_Driver_SS1_HSIOM_SEL_GPIO);
    #endif /* (LED_Driver_SS1_PIN) */

    #if (LED_Driver_SS2_PIN)
        /* Set output pin state after block is disabled */
        LED_Driver_spi_ss2_Write(LED_Driver_GET_SPI_SS2_INACTIVE);

        /* Set GPIO to drive output pin */
        LED_Driver_SET_HSIOM_SEL(LED_Driver_SS2_HSIOM_REG, LED_Driver_SS2_HSIOM_MASK,
                                       LED_Driver_SS2_HSIOM_POS, LED_Driver_SS2_HSIOM_SEL_GPIO);
    #endif /* (LED_Driver_SS2_PIN) */

    #if (LED_Driver_SS3_PIN)
        /* Set output pin state after block is disabled */
        LED_Driver_spi_ss3_Write(LED_Driver_GET_SPI_SS3_INACTIVE);

        /* Set GPIO to drive output pin */
        LED_Driver_SET_HSIOM_SEL(LED_Driver_SS3_HSIOM_REG, LED_Driver_SS3_HSIOM_MASK,
                                       LED_Driver_SS3_HSIOM_POS, LED_Driver_SS3_HSIOM_SEL_GPIO);
    #endif /* (LED_Driver_SS3_PIN) */
    
        /* Store TX interrupt sources (exclude level triggered) for master. */
        LED_Driver_IntrTxMask = LO16(LED_Driver_GetTxInterruptMode() & LED_Driver_INTR_SPIM_TX_RESTORE);
    }
    else
    {
        /* Store TX interrupt sources (exclude level triggered) for slave. */
        LED_Driver_IntrTxMask = LO16(LED_Driver_GetTxInterruptMode() & LED_Driver_INTR_SPIS_TX_RESTORE);
    }

#else

#if (LED_Driver_SPI_MASTER_SCLK_PIN)
    /* Set output pin state after block is disabled */
    LED_Driver_sclk_m_Write(LED_Driver_GET_SPI_SCLK_INACTIVE);

    /* Set GPIO to drive output pin */
    LED_Driver_SET_HSIOM_SEL(LED_Driver_SCLK_M_HSIOM_REG, LED_Driver_SCLK_M_HSIOM_MASK,
                                   LED_Driver_SCLK_M_HSIOM_POS, LED_Driver_SCLK_M_HSIOM_SEL_GPIO);
#endif /* (LED_Driver_MISO_SDA_TX_PIN_PIN) */

#if (LED_Driver_SPI_MASTER_SS0_PIN)
    /* Set output pin state after block is disabled */
    LED_Driver_ss0_m_Write(LED_Driver_GET_SPI_SS0_INACTIVE);

    /* Set GPIO to drive output pin */
    LED_Driver_SET_HSIOM_SEL(LED_Driver_SS0_M_HSIOM_REG, LED_Driver_SS0_M_HSIOM_MASK,
                                   LED_Driver_SS0_M_HSIOM_POS, LED_Driver_SS0_M_HSIOM_SEL_GPIO);
#endif /* (LED_Driver_SPI_MASTER_SS0_PIN) */

#if (LED_Driver_SPI_MASTER_SS1_PIN)
    /* Set output pin state after block is disabled */
    LED_Driver_ss1_m_Write(LED_Driver_GET_SPI_SS1_INACTIVE);

    /* Set GPIO to drive output pin */
    LED_Driver_SET_HSIOM_SEL(LED_Driver_SS1_M_HSIOM_REG, LED_Driver_SS1_M_HSIOM_MASK,
                                   LED_Driver_SS1_M_HSIOM_POS, LED_Driver_SS1_M_HSIOM_SEL_GPIO);
#endif /* (LED_Driver_SPI_MASTER_SS1_PIN) */

#if (LED_Driver_SPI_MASTER_SS2_PIN)
    /* Set output pin state after block is disabled */
    LED_Driver_ss2_m_Write(LED_Driver_GET_SPI_SS2_INACTIVE);

    /* Set GPIO to drive output pin */
    LED_Driver_SET_HSIOM_SEL(LED_Driver_SS2_M_HSIOM_REG, LED_Driver_SS2_M_HSIOM_MASK,
                                   LED_Driver_SS2_M_HSIOM_POS, LED_Driver_SS2_M_HSIOM_SEL_GPIO);
#endif /* (LED_Driver_SPI_MASTER_SS2_PIN) */

#if (LED_Driver_SPI_MASTER_SS3_PIN)
    /* Set output pin state after block is disabled */
    LED_Driver_ss3_m_Write(LED_Driver_GET_SPI_SS3_INACTIVE);

    /* Set GPIO to drive output pin */
    LED_Driver_SET_HSIOM_SEL(LED_Driver_SS3_M_HSIOM_REG, LED_Driver_SS3_M_HSIOM_MASK,
                                   LED_Driver_SS3_M_HSIOM_POS, LED_Driver_SS3_M_HSIOM_SEL_GPIO);
#endif /* (LED_Driver_SPI_MASTER_SS3_PIN) */

    #if (LED_Driver_SPI_MASTER_CONST)
        /* Store TX interrupt sources (exclude level triggered). */
        LED_Driver_IntrTxMask = LO16(LED_Driver_GetTxInterruptMode() & LED_Driver_INTR_SPIM_TX_RESTORE);
    #else
        /* Store TX interrupt sources (exclude level triggered). */
        LED_Driver_IntrTxMask = LO16(LED_Driver_GetTxInterruptMode() & LED_Driver_INTR_SPIS_TX_RESTORE);
    #endif /* (LED_Driver_SPI_MASTER_CONST) */

#endif /* (LED_Driver_SCB_MODE_UNCONFIG_CONST_CFG) */
}


#if (LED_Driver_SPI_MASTER_CONST)
    /*******************************************************************************
    * Function Name: LED_Driver_SetActiveSlaveSelect
    ****************************************************************************//**
    *
    *  Selects one of the four slave select lines to be active during the transfer.
    *  After initialization the active slave select line is 0.
    *  The component should be in one of the following states to change the active
    *  slave select signal source correctly:
    *   - The component is disabled.
    *   - The component has completed transfer (TX FIFO is empty and the
    *     SCB_INTR_MASTER_SPI_DONE status is set).
    *
    *  This function does not check that these conditions are met.
    *  This function is only applicable to SPI Master mode of operation.
    *
    *  \param slaveSelect: slave select line which will be active while the following
    *   transfer.
    *   - LED_Driver_SPI_SLAVE_SELECT0 - Slave select 0.
    *   - LED_Driver_SPI_SLAVE_SELECT1 - Slave select 1.
    *   - LED_Driver_SPI_SLAVE_SELECT2 - Slave select 2.
    *   - LED_Driver_SPI_SLAVE_SELECT3 - Slave select 3.
    *
    *******************************************************************************/
    void LED_Driver_SpiSetActiveSlaveSelect(uint32 slaveSelect)
    {
        uint32 spiCtrl;

        spiCtrl = LED_Driver_SPI_CTRL_REG;

        spiCtrl &= (uint32) ~LED_Driver_SPI_CTRL_SLAVE_SELECT_MASK;
        spiCtrl |= (uint32)  LED_Driver_GET_SPI_CTRL_SS(slaveSelect);

        LED_Driver_SPI_CTRL_REG = spiCtrl;
    }
#endif /* (LED_Driver_SPI_MASTER_CONST) */


#if !(LED_Driver_CY_SCBIP_V0 || LED_Driver_CY_SCBIP_V1)
    /*******************************************************************************
    * Function Name: LED_Driver_SpiSetSlaveSelectPolarity
    ****************************************************************************//**
    *
    *  Sets active polarity for slave select line.
    *  The component should be in one of the following states to change the active
    *  slave select signal source correctly:
    *   - The component is disabled.
    *   - The component has completed transfer.
    *  
    *  This function does not check that these conditions are met.
    *
    *  \param slaveSelect: slave select line to change active polarity.
    *   - LED_Driver_SPI_SLAVE_SELECT0 - Slave select 0.
    *   - LED_Driver_SPI_SLAVE_SELECT1 - Slave select 1.
    *   - LED_Driver_SPI_SLAVE_SELECT2 - Slave select 2.
    *   - LED_Driver_SPI_SLAVE_SELECT3 - Slave select 3.
    *
    *  \param polarity: active polarity of slave select line.
    *   - LED_Driver_SPI_SS_ACTIVE_LOW  - Slave select is active low.
    *   - LED_Driver_SPI_SS_ACTIVE_HIGH - Slave select is active high.
    *
    *******************************************************************************/
    void LED_Driver_SpiSetSlaveSelectPolarity(uint32 slaveSelect, uint32 polarity)
    {
        uint32 ssPolarity;

        /* Get position of the polarity bit associated with slave select line */
        ssPolarity = LED_Driver_GET_SPI_CTRL_SSEL_POLARITY((uint32) 1u << slaveSelect);

        if (0u != polarity)
        {
            LED_Driver_SPI_CTRL_REG |= (uint32)  ssPolarity;
        }
        else
        {
            LED_Driver_SPI_CTRL_REG &= (uint32) ~ssPolarity;
        }
    }
#endif /* !(LED_Driver_CY_SCBIP_V0 || LED_Driver_CY_SCBIP_V1) */


#if(LED_Driver_SPI_WAKE_ENABLE_CONST)
    /*******************************************************************************
    * Function Name: LED_Driver_SpiSaveConfig
    ****************************************************************************//**
    *
    *  Clears INTR_SPI_EC.WAKE_UP and enables it. This interrupt
    *  source triggers when the master assigns the SS line and wakes up the device.
    *
    *******************************************************************************/
    void LED_Driver_SpiSaveConfig(void)
    {
        /* Clear and enable SPI wakeup interrupt source */
        LED_Driver_ClearSpiExtClkInterruptSource(LED_Driver_INTR_SPI_EC_WAKE_UP);
        LED_Driver_SetSpiExtClkInterruptMode(LED_Driver_INTR_SPI_EC_WAKE_UP);
    }


    /*******************************************************************************
    * Function Name: LED_Driver_SpiRestoreConfig
    ****************************************************************************//**
    *
    *  Disables the INTR_SPI_EC.WAKE_UP interrupt source. After wakeup
    *  slave does not drive the MISO line and the master receives 0xFF.
    *
    *******************************************************************************/
    void LED_Driver_SpiRestoreConfig(void)
    {
        /* Disable SPI wakeup interrupt source */
        LED_Driver_SetSpiExtClkInterruptMode(LED_Driver_NO_INTR_SOURCES);
    }
#endif /* (LED_Driver_SPI_WAKE_ENABLE_CONST) */


/* [] END OF FILE */
