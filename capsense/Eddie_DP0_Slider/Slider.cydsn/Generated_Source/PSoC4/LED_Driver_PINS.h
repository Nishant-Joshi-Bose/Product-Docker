/***************************************************************************//**
* \file LED_Driver_PINS.h
* \version 3.20
*
* \brief
*  This file provides constants and parameter values for the pin components
*  buried into SCB Component.
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

#if !defined(CY_SCB_PINS_LED_Driver_H)
#define CY_SCB_PINS_LED_Driver_H

#include "cydevice_trm.h"
#include "cyfitter.h"
#include "cytypes.h"


/***************************************
*   Conditional Compilation Parameters
****************************************/

/* Unconfigured pins */
#define LED_Driver_REMOVE_RX_WAKE_SCL_MOSI_PIN  (1u)
#define LED_Driver_REMOVE_RX_SCL_MOSI_PIN      (1u)
#define LED_Driver_REMOVE_TX_SDA_MISO_PIN      (1u)
#define LED_Driver_REMOVE_CTS_SCLK_PIN      (1u)
#define LED_Driver_REMOVE_RTS_SS0_PIN      (1u)
#define LED_Driver_REMOVE_SS1_PIN                 (1u)
#define LED_Driver_REMOVE_SS2_PIN                 (1u)
#define LED_Driver_REMOVE_SS3_PIN                 (1u)

/* Mode defined pins */
#define LED_Driver_REMOVE_I2C_PINS                (1u)
#define LED_Driver_REMOVE_SPI_MASTER_PINS         (0u)
#define LED_Driver_REMOVE_SPI_MASTER_SCLK_PIN     (0u)
#define LED_Driver_REMOVE_SPI_MASTER_MOSI_PIN     (0u)
#define LED_Driver_REMOVE_SPI_MASTER_MISO_PIN     (1u)
#define LED_Driver_REMOVE_SPI_MASTER_SS0_PIN      (1u)
#define LED_Driver_REMOVE_SPI_MASTER_SS1_PIN      (1u)
#define LED_Driver_REMOVE_SPI_MASTER_SS2_PIN      (1u)
#define LED_Driver_REMOVE_SPI_MASTER_SS3_PIN      (1u)
#define LED_Driver_REMOVE_SPI_SLAVE_PINS          (1u)
#define LED_Driver_REMOVE_SPI_SLAVE_MOSI_PIN      (1u)
#define LED_Driver_REMOVE_SPI_SLAVE_MISO_PIN      (1u)
#define LED_Driver_REMOVE_UART_TX_PIN             (1u)
#define LED_Driver_REMOVE_UART_RX_TX_PIN          (1u)
#define LED_Driver_REMOVE_UART_RX_PIN             (1u)
#define LED_Driver_REMOVE_UART_RX_WAKE_PIN        (1u)
#define LED_Driver_REMOVE_UART_RTS_PIN            (1u)
#define LED_Driver_REMOVE_UART_CTS_PIN            (1u)

/* Unconfigured pins */
#define LED_Driver_RX_WAKE_SCL_MOSI_PIN (0u == LED_Driver_REMOVE_RX_WAKE_SCL_MOSI_PIN)
#define LED_Driver_RX_SCL_MOSI_PIN     (0u == LED_Driver_REMOVE_RX_SCL_MOSI_PIN)
#define LED_Driver_TX_SDA_MISO_PIN     (0u == LED_Driver_REMOVE_TX_SDA_MISO_PIN)
#define LED_Driver_CTS_SCLK_PIN     (0u == LED_Driver_REMOVE_CTS_SCLK_PIN)
#define LED_Driver_RTS_SS0_PIN     (0u == LED_Driver_REMOVE_RTS_SS0_PIN)
#define LED_Driver_SS1_PIN                (0u == LED_Driver_REMOVE_SS1_PIN)
#define LED_Driver_SS2_PIN                (0u == LED_Driver_REMOVE_SS2_PIN)
#define LED_Driver_SS3_PIN                (0u == LED_Driver_REMOVE_SS3_PIN)

/* Mode defined pins */
#define LED_Driver_I2C_PINS               (0u == LED_Driver_REMOVE_I2C_PINS)
#define LED_Driver_SPI_MASTER_PINS        (0u == LED_Driver_REMOVE_SPI_MASTER_PINS)
#define LED_Driver_SPI_MASTER_SCLK_PIN    (0u == LED_Driver_REMOVE_SPI_MASTER_SCLK_PIN)
#define LED_Driver_SPI_MASTER_MOSI_PIN    (0u == LED_Driver_REMOVE_SPI_MASTER_MOSI_PIN)
#define LED_Driver_SPI_MASTER_MISO_PIN    (0u == LED_Driver_REMOVE_SPI_MASTER_MISO_PIN)
#define LED_Driver_SPI_MASTER_SS0_PIN     (0u == LED_Driver_REMOVE_SPI_MASTER_SS0_PIN)
#define LED_Driver_SPI_MASTER_SS1_PIN     (0u == LED_Driver_REMOVE_SPI_MASTER_SS1_PIN)
#define LED_Driver_SPI_MASTER_SS2_PIN     (0u == LED_Driver_REMOVE_SPI_MASTER_SS2_PIN)
#define LED_Driver_SPI_MASTER_SS3_PIN     (0u == LED_Driver_REMOVE_SPI_MASTER_SS3_PIN)
#define LED_Driver_SPI_SLAVE_PINS         (0u == LED_Driver_REMOVE_SPI_SLAVE_PINS)
#define LED_Driver_SPI_SLAVE_MOSI_PIN     (0u == LED_Driver_REMOVE_SPI_SLAVE_MOSI_PIN)
#define LED_Driver_SPI_SLAVE_MISO_PIN     (0u == LED_Driver_REMOVE_SPI_SLAVE_MISO_PIN)
#define LED_Driver_UART_TX_PIN            (0u == LED_Driver_REMOVE_UART_TX_PIN)
#define LED_Driver_UART_RX_TX_PIN         (0u == LED_Driver_REMOVE_UART_RX_TX_PIN)
#define LED_Driver_UART_RX_PIN            (0u == LED_Driver_REMOVE_UART_RX_PIN)
#define LED_Driver_UART_RX_WAKE_PIN       (0u == LED_Driver_REMOVE_UART_RX_WAKE_PIN)
#define LED_Driver_UART_RTS_PIN           (0u == LED_Driver_REMOVE_UART_RTS_PIN)
#define LED_Driver_UART_CTS_PIN           (0u == LED_Driver_REMOVE_UART_CTS_PIN)


/***************************************
*             Includes
****************************************/

#if (LED_Driver_RX_WAKE_SCL_MOSI_PIN)
    #include "LED_Driver_uart_rx_wake_i2c_scl_spi_mosi.h"
#endif /* (LED_Driver_RX_SCL_MOSI) */

#if (LED_Driver_RX_SCL_MOSI_PIN)
    #include "LED_Driver_uart_rx_i2c_scl_spi_mosi.h"
#endif /* (LED_Driver_RX_SCL_MOSI) */

#if (LED_Driver_TX_SDA_MISO_PIN)
    #include "LED_Driver_uart_tx_i2c_sda_spi_miso.h"
#endif /* (LED_Driver_TX_SDA_MISO) */

#if (LED_Driver_CTS_SCLK_PIN)
    #include "LED_Driver_uart_cts_spi_sclk.h"
#endif /* (LED_Driver_CTS_SCLK) */

#if (LED_Driver_RTS_SS0_PIN)
    #include "LED_Driver_uart_rts_spi_ss0.h"
#endif /* (LED_Driver_RTS_SS0_PIN) */

#if (LED_Driver_SS1_PIN)
    #include "LED_Driver_spi_ss1.h"
#endif /* (LED_Driver_SS1_PIN) */

#if (LED_Driver_SS2_PIN)
    #include "LED_Driver_spi_ss2.h"
#endif /* (LED_Driver_SS2_PIN) */

#if (LED_Driver_SS3_PIN)
    #include "LED_Driver_spi_ss3.h"
#endif /* (LED_Driver_SS3_PIN) */

#if (LED_Driver_I2C_PINS)
    #include "LED_Driver_scl.h"
    #include "LED_Driver_sda.h"
#endif /* (LED_Driver_I2C_PINS) */

#if (LED_Driver_SPI_MASTER_PINS)
#if (LED_Driver_SPI_MASTER_SCLK_PIN)
    #include "LED_Driver_sclk_m.h"
#endif /* (LED_Driver_SPI_MASTER_SCLK_PIN) */

#if (LED_Driver_SPI_MASTER_MOSI_PIN)
    #include "LED_Driver_mosi_m.h"
#endif /* (LED_Driver_SPI_MASTER_MOSI_PIN) */

#if (LED_Driver_SPI_MASTER_MISO_PIN)
    #include "LED_Driver_miso_m.h"
#endif /*(LED_Driver_SPI_MASTER_MISO_PIN) */
#endif /* (LED_Driver_SPI_MASTER_PINS) */

#if (LED_Driver_SPI_SLAVE_PINS)
    #include "LED_Driver_sclk_s.h"
    #include "LED_Driver_ss_s.h"

#if (LED_Driver_SPI_SLAVE_MOSI_PIN)
    #include "LED_Driver_mosi_s.h"
#endif /* (LED_Driver_SPI_SLAVE_MOSI_PIN) */

#if (LED_Driver_SPI_SLAVE_MISO_PIN)
    #include "LED_Driver_miso_s.h"
#endif /*(LED_Driver_SPI_SLAVE_MISO_PIN) */
#endif /* (LED_Driver_SPI_SLAVE_PINS) */

#if (LED_Driver_SPI_MASTER_SS0_PIN)
    #include "LED_Driver_ss0_m.h"
#endif /* (LED_Driver_SPI_MASTER_SS0_PIN) */

#if (LED_Driver_SPI_MASTER_SS1_PIN)
    #include "LED_Driver_ss1_m.h"
#endif /* (LED_Driver_SPI_MASTER_SS1_PIN) */

#if (LED_Driver_SPI_MASTER_SS2_PIN)
    #include "LED_Driver_ss2_m.h"
#endif /* (LED_Driver_SPI_MASTER_SS2_PIN) */

#if (LED_Driver_SPI_MASTER_SS3_PIN)
    #include "LED_Driver_ss3_m.h"
#endif /* (LED_Driver_SPI_MASTER_SS3_PIN) */

#if (LED_Driver_UART_TX_PIN)
    #include "LED_Driver_tx.h"
#endif /* (LED_Driver_UART_TX_PIN) */

#if (LED_Driver_UART_RX_TX_PIN)
    #include "LED_Driver_rx_tx.h"
#endif /* (LED_Driver_UART_RX_TX_PIN) */

#if (LED_Driver_UART_RX_PIN)
    #include "LED_Driver_rx.h"
#endif /* (LED_Driver_UART_RX_PIN) */

#if (LED_Driver_UART_RX_WAKE_PIN)
    #include "LED_Driver_rx_wake.h"
#endif /* (LED_Driver_UART_RX_WAKE_PIN) */

#if (LED_Driver_UART_RTS_PIN)
    #include "LED_Driver_rts.h"
#endif /* (LED_Driver_UART_RTS_PIN) */

#if (LED_Driver_UART_CTS_PIN)
    #include "LED_Driver_cts.h"
#endif /* (LED_Driver_UART_CTS_PIN) */


/***************************************
*              Registers
***************************************/

#if (LED_Driver_RX_SCL_MOSI_PIN)
    #define LED_Driver_RX_SCL_MOSI_HSIOM_REG   (*(reg32 *) LED_Driver_uart_rx_i2c_scl_spi_mosi__0__HSIOM)
    #define LED_Driver_RX_SCL_MOSI_HSIOM_PTR   ( (reg32 *) LED_Driver_uart_rx_i2c_scl_spi_mosi__0__HSIOM)
    
    #define LED_Driver_RX_SCL_MOSI_HSIOM_MASK      (LED_Driver_uart_rx_i2c_scl_spi_mosi__0__HSIOM_MASK)
    #define LED_Driver_RX_SCL_MOSI_HSIOM_POS       (LED_Driver_uart_rx_i2c_scl_spi_mosi__0__HSIOM_SHIFT)
    #define LED_Driver_RX_SCL_MOSI_HSIOM_SEL_GPIO  (LED_Driver_uart_rx_i2c_scl_spi_mosi__0__HSIOM_GPIO)
    #define LED_Driver_RX_SCL_MOSI_HSIOM_SEL_I2C   (LED_Driver_uart_rx_i2c_scl_spi_mosi__0__HSIOM_I2C)
    #define LED_Driver_RX_SCL_MOSI_HSIOM_SEL_SPI   (LED_Driver_uart_rx_i2c_scl_spi_mosi__0__HSIOM_SPI)
    #define LED_Driver_RX_SCL_MOSI_HSIOM_SEL_UART  (LED_Driver_uart_rx_i2c_scl_spi_mosi__0__HSIOM_UART)
    
#elif (LED_Driver_RX_WAKE_SCL_MOSI_PIN)
    #define LED_Driver_RX_WAKE_SCL_MOSI_HSIOM_REG   (*(reg32 *) LED_Driver_uart_rx_wake_i2c_scl_spi_mosi__0__HSIOM)
    #define LED_Driver_RX_WAKE_SCL_MOSI_HSIOM_PTR   ( (reg32 *) LED_Driver_uart_rx_wake_i2c_scl_spi_mosi__0__HSIOM)
    
    #define LED_Driver_RX_WAKE_SCL_MOSI_HSIOM_MASK      (LED_Driver_uart_rx_wake_i2c_scl_spi_mosi__0__HSIOM_MASK)
    #define LED_Driver_RX_WAKE_SCL_MOSI_HSIOM_POS       (LED_Driver_uart_rx_wake_i2c_scl_spi_mosi__0__HSIOM_SHIFT)
    #define LED_Driver_RX_WAKE_SCL_MOSI_HSIOM_SEL_GPIO  (LED_Driver_uart_rx_wake_i2c_scl_spi_mosi__0__HSIOM_GPIO)
    #define LED_Driver_RX_WAKE_SCL_MOSI_HSIOM_SEL_I2C   (LED_Driver_uart_rx_wake_i2c_scl_spi_mosi__0__HSIOM_I2C)
    #define LED_Driver_RX_WAKE_SCL_MOSI_HSIOM_SEL_SPI   (LED_Driver_uart_rx_wake_i2c_scl_spi_mosi__0__HSIOM_SPI)
    #define LED_Driver_RX_WAKE_SCL_MOSI_HSIOM_SEL_UART  (LED_Driver_uart_rx_wake_i2c_scl_spi_mosi__0__HSIOM_UART)    
   
    #define LED_Driver_RX_WAKE_SCL_MOSI_INTCFG_REG (*(reg32 *) LED_Driver_uart_rx_wake_i2c_scl_spi_mosi__0__INTCFG)
    #define LED_Driver_RX_WAKE_SCL_MOSI_INTCFG_PTR ( (reg32 *) LED_Driver_uart_rx_wake_i2c_scl_spi_mosi__0__INTCFG)
    #define LED_Driver_RX_WAKE_SCL_MOSI_INTCFG_TYPE_POS  (LED_Driver_uart_rx_wake_i2c_scl_spi_mosi__SHIFT)
    #define LED_Driver_RX_WAKE_SCL_MOSI_INTCFG_TYPE_MASK ((uint32) LED_Driver_INTCFG_TYPE_MASK << \
                                                                           LED_Driver_RX_WAKE_SCL_MOSI_INTCFG_TYPE_POS)
#else
    /* None of pins LED_Driver_RX_SCL_MOSI_PIN or LED_Driver_RX_WAKE_SCL_MOSI_PIN present.*/
#endif /* (LED_Driver_RX_SCL_MOSI_PIN) */

#if (LED_Driver_TX_SDA_MISO_PIN)
    #define LED_Driver_TX_SDA_MISO_HSIOM_REG   (*(reg32 *) LED_Driver_uart_tx_i2c_sda_spi_miso__0__HSIOM)
    #define LED_Driver_TX_SDA_MISO_HSIOM_PTR   ( (reg32 *) LED_Driver_uart_tx_i2c_sda_spi_miso__0__HSIOM)
    
    #define LED_Driver_TX_SDA_MISO_HSIOM_MASK      (LED_Driver_uart_tx_i2c_sda_spi_miso__0__HSIOM_MASK)
    #define LED_Driver_TX_SDA_MISO_HSIOM_POS       (LED_Driver_uart_tx_i2c_sda_spi_miso__0__HSIOM_SHIFT)
    #define LED_Driver_TX_SDA_MISO_HSIOM_SEL_GPIO  (LED_Driver_uart_tx_i2c_sda_spi_miso__0__HSIOM_GPIO)
    #define LED_Driver_TX_SDA_MISO_HSIOM_SEL_I2C   (LED_Driver_uart_tx_i2c_sda_spi_miso__0__HSIOM_I2C)
    #define LED_Driver_TX_SDA_MISO_HSIOM_SEL_SPI   (LED_Driver_uart_tx_i2c_sda_spi_miso__0__HSIOM_SPI)
    #define LED_Driver_TX_SDA_MISO_HSIOM_SEL_UART  (LED_Driver_uart_tx_i2c_sda_spi_miso__0__HSIOM_UART)
#endif /* (LED_Driver_TX_SDA_MISO_PIN) */

#if (LED_Driver_CTS_SCLK_PIN)
    #define LED_Driver_CTS_SCLK_HSIOM_REG   (*(reg32 *) LED_Driver_uart_cts_spi_sclk__0__HSIOM)
    #define LED_Driver_CTS_SCLK_HSIOM_PTR   ( (reg32 *) LED_Driver_uart_cts_spi_sclk__0__HSIOM)
    
    #define LED_Driver_CTS_SCLK_HSIOM_MASK      (LED_Driver_uart_cts_spi_sclk__0__HSIOM_MASK)
    #define LED_Driver_CTS_SCLK_HSIOM_POS       (LED_Driver_uart_cts_spi_sclk__0__HSIOM_SHIFT)
    #define LED_Driver_CTS_SCLK_HSIOM_SEL_GPIO  (LED_Driver_uart_cts_spi_sclk__0__HSIOM_GPIO)
    #define LED_Driver_CTS_SCLK_HSIOM_SEL_I2C   (LED_Driver_uart_cts_spi_sclk__0__HSIOM_I2C)
    #define LED_Driver_CTS_SCLK_HSIOM_SEL_SPI   (LED_Driver_uart_cts_spi_sclk__0__HSIOM_SPI)
    #define LED_Driver_CTS_SCLK_HSIOM_SEL_UART  (LED_Driver_uart_cts_spi_sclk__0__HSIOM_UART)
#endif /* (LED_Driver_CTS_SCLK_PIN) */

#if (LED_Driver_RTS_SS0_PIN)
    #define LED_Driver_RTS_SS0_HSIOM_REG   (*(reg32 *) LED_Driver_uart_rts_spi_ss0__0__HSIOM)
    #define LED_Driver_RTS_SS0_HSIOM_PTR   ( (reg32 *) LED_Driver_uart_rts_spi_ss0__0__HSIOM)
    
    #define LED_Driver_RTS_SS0_HSIOM_MASK      (LED_Driver_uart_rts_spi_ss0__0__HSIOM_MASK)
    #define LED_Driver_RTS_SS0_HSIOM_POS       (LED_Driver_uart_rts_spi_ss0__0__HSIOM_SHIFT)
    #define LED_Driver_RTS_SS0_HSIOM_SEL_GPIO  (LED_Driver_uart_rts_spi_ss0__0__HSIOM_GPIO)
    #define LED_Driver_RTS_SS0_HSIOM_SEL_I2C   (LED_Driver_uart_rts_spi_ss0__0__HSIOM_I2C)
    #define LED_Driver_RTS_SS0_HSIOM_SEL_SPI   (LED_Driver_uart_rts_spi_ss0__0__HSIOM_SPI)
#if !(LED_Driver_CY_SCBIP_V0 || LED_Driver_CY_SCBIP_V1)
    #define LED_Driver_RTS_SS0_HSIOM_SEL_UART  (LED_Driver_uart_rts_spi_ss0__0__HSIOM_UART)
#endif /* !(LED_Driver_CY_SCBIP_V0 || LED_Driver_CY_SCBIP_V1) */
#endif /* (LED_Driver_RTS_SS0_PIN) */

#if (LED_Driver_SS1_PIN)
    #define LED_Driver_SS1_HSIOM_REG  (*(reg32 *) LED_Driver_spi_ss1__0__HSIOM)
    #define LED_Driver_SS1_HSIOM_PTR  ( (reg32 *) LED_Driver_spi_ss1__0__HSIOM)
    
    #define LED_Driver_SS1_HSIOM_MASK     (LED_Driver_spi_ss1__0__HSIOM_MASK)
    #define LED_Driver_SS1_HSIOM_POS      (LED_Driver_spi_ss1__0__HSIOM_SHIFT)
    #define LED_Driver_SS1_HSIOM_SEL_GPIO (LED_Driver_spi_ss1__0__HSIOM_GPIO)
    #define LED_Driver_SS1_HSIOM_SEL_I2C  (LED_Driver_spi_ss1__0__HSIOM_I2C)
    #define LED_Driver_SS1_HSIOM_SEL_SPI  (LED_Driver_spi_ss1__0__HSIOM_SPI)
#endif /* (LED_Driver_SS1_PIN) */

#if (LED_Driver_SS2_PIN)
    #define LED_Driver_SS2_HSIOM_REG     (*(reg32 *) LED_Driver_spi_ss2__0__HSIOM)
    #define LED_Driver_SS2_HSIOM_PTR     ( (reg32 *) LED_Driver_spi_ss2__0__HSIOM)
    
    #define LED_Driver_SS2_HSIOM_MASK     (LED_Driver_spi_ss2__0__HSIOM_MASK)
    #define LED_Driver_SS2_HSIOM_POS      (LED_Driver_spi_ss2__0__HSIOM_SHIFT)
    #define LED_Driver_SS2_HSIOM_SEL_GPIO (LED_Driver_spi_ss2__0__HSIOM_GPIO)
    #define LED_Driver_SS2_HSIOM_SEL_I2C  (LED_Driver_spi_ss2__0__HSIOM_I2C)
    #define LED_Driver_SS2_HSIOM_SEL_SPI  (LED_Driver_spi_ss2__0__HSIOM_SPI)
#endif /* (LED_Driver_SS2_PIN) */

#if (LED_Driver_SS3_PIN)
    #define LED_Driver_SS3_HSIOM_REG     (*(reg32 *) LED_Driver_spi_ss3__0__HSIOM)
    #define LED_Driver_SS3_HSIOM_PTR     ( (reg32 *) LED_Driver_spi_ss3__0__HSIOM)
    
    #define LED_Driver_SS3_HSIOM_MASK     (LED_Driver_spi_ss3__0__HSIOM_MASK)
    #define LED_Driver_SS3_HSIOM_POS      (LED_Driver_spi_ss3__0__HSIOM_SHIFT)
    #define LED_Driver_SS3_HSIOM_SEL_GPIO (LED_Driver_spi_ss3__0__HSIOM_GPIO)
    #define LED_Driver_SS3_HSIOM_SEL_I2C  (LED_Driver_spi_ss3__0__HSIOM_I2C)
    #define LED_Driver_SS3_HSIOM_SEL_SPI  (LED_Driver_spi_ss3__0__HSIOM_SPI)
#endif /* (LED_Driver_SS3_PIN) */

#if (LED_Driver_I2C_PINS)
    #define LED_Driver_SCL_HSIOM_REG  (*(reg32 *) LED_Driver_scl__0__HSIOM)
    #define LED_Driver_SCL_HSIOM_PTR  ( (reg32 *) LED_Driver_scl__0__HSIOM)
    
    #define LED_Driver_SCL_HSIOM_MASK     (LED_Driver_scl__0__HSIOM_MASK)
    #define LED_Driver_SCL_HSIOM_POS      (LED_Driver_scl__0__HSIOM_SHIFT)
    #define LED_Driver_SCL_HSIOM_SEL_GPIO (LED_Driver_sda__0__HSIOM_GPIO)
    #define LED_Driver_SCL_HSIOM_SEL_I2C  (LED_Driver_sda__0__HSIOM_I2C)
    
    #define LED_Driver_SDA_HSIOM_REG  (*(reg32 *) LED_Driver_sda__0__HSIOM)
    #define LED_Driver_SDA_HSIOM_PTR  ( (reg32 *) LED_Driver_sda__0__HSIOM)
    
    #define LED_Driver_SDA_HSIOM_MASK     (LED_Driver_sda__0__HSIOM_MASK)
    #define LED_Driver_SDA_HSIOM_POS      (LED_Driver_sda__0__HSIOM_SHIFT)
    #define LED_Driver_SDA_HSIOM_SEL_GPIO (LED_Driver_sda__0__HSIOM_GPIO)
    #define LED_Driver_SDA_HSIOM_SEL_I2C  (LED_Driver_sda__0__HSIOM_I2C)
#endif /* (LED_Driver_I2C_PINS) */

#if (LED_Driver_SPI_SLAVE_PINS)
    #define LED_Driver_SCLK_S_HSIOM_REG   (*(reg32 *) LED_Driver_sclk_s__0__HSIOM)
    #define LED_Driver_SCLK_S_HSIOM_PTR   ( (reg32 *) LED_Driver_sclk_s__0__HSIOM)
    
    #define LED_Driver_SCLK_S_HSIOM_MASK      (LED_Driver_sclk_s__0__HSIOM_MASK)
    #define LED_Driver_SCLK_S_HSIOM_POS       (LED_Driver_sclk_s__0__HSIOM_SHIFT)
    #define LED_Driver_SCLK_S_HSIOM_SEL_GPIO  (LED_Driver_sclk_s__0__HSIOM_GPIO)
    #define LED_Driver_SCLK_S_HSIOM_SEL_SPI   (LED_Driver_sclk_s__0__HSIOM_SPI)
    
    #define LED_Driver_SS0_S_HSIOM_REG    (*(reg32 *) LED_Driver_ss0_s__0__HSIOM)
    #define LED_Driver_SS0_S_HSIOM_PTR    ( (reg32 *) LED_Driver_ss0_s__0__HSIOM)
    
    #define LED_Driver_SS0_S_HSIOM_MASK       (LED_Driver_ss0_s__0__HSIOM_MASK)
    #define LED_Driver_SS0_S_HSIOM_POS        (LED_Driver_ss0_s__0__HSIOM_SHIFT)
    #define LED_Driver_SS0_S_HSIOM_SEL_GPIO   (LED_Driver_ss0_s__0__HSIOM_GPIO)  
    #define LED_Driver_SS0_S_HSIOM_SEL_SPI    (LED_Driver_ss0_s__0__HSIOM_SPI)
#endif /* (LED_Driver_SPI_SLAVE_PINS) */

#if (LED_Driver_SPI_SLAVE_MOSI_PIN)
    #define LED_Driver_MOSI_S_HSIOM_REG   (*(reg32 *) LED_Driver_mosi_s__0__HSIOM)
    #define LED_Driver_MOSI_S_HSIOM_PTR   ( (reg32 *) LED_Driver_mosi_s__0__HSIOM)
    
    #define LED_Driver_MOSI_S_HSIOM_MASK      (LED_Driver_mosi_s__0__HSIOM_MASK)
    #define LED_Driver_MOSI_S_HSIOM_POS       (LED_Driver_mosi_s__0__HSIOM_SHIFT)
    #define LED_Driver_MOSI_S_HSIOM_SEL_GPIO  (LED_Driver_mosi_s__0__HSIOM_GPIO)
    #define LED_Driver_MOSI_S_HSIOM_SEL_SPI   (LED_Driver_mosi_s__0__HSIOM_SPI)
#endif /* (LED_Driver_SPI_SLAVE_MOSI_PIN) */

#if (LED_Driver_SPI_SLAVE_MISO_PIN)
    #define LED_Driver_MISO_S_HSIOM_REG   (*(reg32 *) LED_Driver_miso_s__0__HSIOM)
    #define LED_Driver_MISO_S_HSIOM_PTR   ( (reg32 *) LED_Driver_miso_s__0__HSIOM)
    
    #define LED_Driver_MISO_S_HSIOM_MASK      (LED_Driver_miso_s__0__HSIOM_MASK)
    #define LED_Driver_MISO_S_HSIOM_POS       (LED_Driver_miso_s__0__HSIOM_SHIFT)
    #define LED_Driver_MISO_S_HSIOM_SEL_GPIO  (LED_Driver_miso_s__0__HSIOM_GPIO)
    #define LED_Driver_MISO_S_HSIOM_SEL_SPI   (LED_Driver_miso_s__0__HSIOM_SPI)
#endif /* (LED_Driver_SPI_SLAVE_MISO_PIN) */

#if (LED_Driver_SPI_MASTER_MISO_PIN)
    #define LED_Driver_MISO_M_HSIOM_REG   (*(reg32 *) LED_Driver_miso_m__0__HSIOM)
    #define LED_Driver_MISO_M_HSIOM_PTR   ( (reg32 *) LED_Driver_miso_m__0__HSIOM)
    
    #define LED_Driver_MISO_M_HSIOM_MASK      (LED_Driver_miso_m__0__HSIOM_MASK)
    #define LED_Driver_MISO_M_HSIOM_POS       (LED_Driver_miso_m__0__HSIOM_SHIFT)
    #define LED_Driver_MISO_M_HSIOM_SEL_GPIO  (LED_Driver_miso_m__0__HSIOM_GPIO)
    #define LED_Driver_MISO_M_HSIOM_SEL_SPI   (LED_Driver_miso_m__0__HSIOM_SPI)
#endif /* (LED_Driver_SPI_MASTER_MISO_PIN) */

#if (LED_Driver_SPI_MASTER_MOSI_PIN)
    #define LED_Driver_MOSI_M_HSIOM_REG   (*(reg32 *) LED_Driver_mosi_m__0__HSIOM)
    #define LED_Driver_MOSI_M_HSIOM_PTR   ( (reg32 *) LED_Driver_mosi_m__0__HSIOM)
    
    #define LED_Driver_MOSI_M_HSIOM_MASK      (LED_Driver_mosi_m__0__HSIOM_MASK)
    #define LED_Driver_MOSI_M_HSIOM_POS       (LED_Driver_mosi_m__0__HSIOM_SHIFT)
    #define LED_Driver_MOSI_M_HSIOM_SEL_GPIO  (LED_Driver_mosi_m__0__HSIOM_GPIO)
    #define LED_Driver_MOSI_M_HSIOM_SEL_SPI   (LED_Driver_mosi_m__0__HSIOM_SPI)
#endif /* (LED_Driver_SPI_MASTER_MOSI_PIN) */

#if (LED_Driver_SPI_MASTER_SCLK_PIN)
    #define LED_Driver_SCLK_M_HSIOM_REG   (*(reg32 *) LED_Driver_sclk_m__0__HSIOM)
    #define LED_Driver_SCLK_M_HSIOM_PTR   ( (reg32 *) LED_Driver_sclk_m__0__HSIOM)
    
    #define LED_Driver_SCLK_M_HSIOM_MASK      (LED_Driver_sclk_m__0__HSIOM_MASK)
    #define LED_Driver_SCLK_M_HSIOM_POS       (LED_Driver_sclk_m__0__HSIOM_SHIFT)
    #define LED_Driver_SCLK_M_HSIOM_SEL_GPIO  (LED_Driver_sclk_m__0__HSIOM_GPIO)
    #define LED_Driver_SCLK_M_HSIOM_SEL_SPI   (LED_Driver_sclk_m__0__HSIOM_SPI)
#endif /* (LED_Driver_SPI_MASTER_SCLK_PIN) */

#if (LED_Driver_SPI_MASTER_SS0_PIN)
    #define LED_Driver_SS0_M_HSIOM_REG    (*(reg32 *) LED_Driver_ss0_m__0__HSIOM)
    #define LED_Driver_SS0_M_HSIOM_PTR    ( (reg32 *) LED_Driver_ss0_m__0__HSIOM)
    
    #define LED_Driver_SS0_M_HSIOM_MASK       (LED_Driver_ss0_m__0__HSIOM_MASK)
    #define LED_Driver_SS0_M_HSIOM_POS        (LED_Driver_ss0_m__0__HSIOM_SHIFT)
    #define LED_Driver_SS0_M_HSIOM_SEL_GPIO   (LED_Driver_ss0_m__0__HSIOM_GPIO)
    #define LED_Driver_SS0_M_HSIOM_SEL_SPI    (LED_Driver_ss0_m__0__HSIOM_SPI)
#endif /* (LED_Driver_SPI_MASTER_SS0_PIN) */

#if (LED_Driver_SPI_MASTER_SS1_PIN)
    #define LED_Driver_SS1_M_HSIOM_REG    (*(reg32 *) LED_Driver_ss1_m__0__HSIOM)
    #define LED_Driver_SS1_M_HSIOM_PTR    ( (reg32 *) LED_Driver_ss1_m__0__HSIOM)
    
    #define LED_Driver_SS1_M_HSIOM_MASK       (LED_Driver_ss1_m__0__HSIOM_MASK)
    #define LED_Driver_SS1_M_HSIOM_POS        (LED_Driver_ss1_m__0__HSIOM_SHIFT)
    #define LED_Driver_SS1_M_HSIOM_SEL_GPIO   (LED_Driver_ss1_m__0__HSIOM_GPIO)
    #define LED_Driver_SS1_M_HSIOM_SEL_SPI    (LED_Driver_ss1_m__0__HSIOM_SPI)
#endif /* (LED_Driver_SPI_MASTER_SS1_PIN) */

#if (LED_Driver_SPI_MASTER_SS2_PIN)
    #define LED_Driver_SS2_M_HSIOM_REG    (*(reg32 *) LED_Driver_ss2_m__0__HSIOM)
    #define LED_Driver_SS2_M_HSIOM_PTR    ( (reg32 *) LED_Driver_ss2_m__0__HSIOM)
    
    #define LED_Driver_SS2_M_HSIOM_MASK       (LED_Driver_ss2_m__0__HSIOM_MASK)
    #define LED_Driver_SS2_M_HSIOM_POS        (LED_Driver_ss2_m__0__HSIOM_SHIFT)
    #define LED_Driver_SS2_M_HSIOM_SEL_GPIO   (LED_Driver_ss2_m__0__HSIOM_GPIO)
    #define LED_Driver_SS2_M_HSIOM_SEL_SPI    (LED_Driver_ss2_m__0__HSIOM_SPI)
#endif /* (LED_Driver_SPI_MASTER_SS2_PIN) */

#if (LED_Driver_SPI_MASTER_SS3_PIN)
    #define LED_Driver_SS3_M_HSIOM_REG    (*(reg32 *) LED_Driver_ss3_m__0__HSIOM)
    #define LED_Driver_SS3_M_HSIOM_PTR    ( (reg32 *) LED_Driver_ss3_m__0__HSIOM)
    
    #define LED_Driver_SS3_M_HSIOM_MASK      (LED_Driver_ss3_m__0__HSIOM_MASK)
    #define LED_Driver_SS3_M_HSIOM_POS       (LED_Driver_ss3_m__0__HSIOM_SHIFT)
    #define LED_Driver_SS3_M_HSIOM_SEL_GPIO  (LED_Driver_ss3_m__0__HSIOM_GPIO)
    #define LED_Driver_SS3_M_HSIOM_SEL_SPI   (LED_Driver_ss3_m__0__HSIOM_SPI)
#endif /* (LED_Driver_SPI_MASTER_SS3_PIN) */

#if (LED_Driver_UART_RX_PIN)
    #define LED_Driver_RX_HSIOM_REG   (*(reg32 *) LED_Driver_rx__0__HSIOM)
    #define LED_Driver_RX_HSIOM_PTR   ( (reg32 *) LED_Driver_rx__0__HSIOM)
    
    #define LED_Driver_RX_HSIOM_MASK      (LED_Driver_rx__0__HSIOM_MASK)
    #define LED_Driver_RX_HSIOM_POS       (LED_Driver_rx__0__HSIOM_SHIFT)
    #define LED_Driver_RX_HSIOM_SEL_GPIO  (LED_Driver_rx__0__HSIOM_GPIO)
    #define LED_Driver_RX_HSIOM_SEL_UART  (LED_Driver_rx__0__HSIOM_UART)
#endif /* (LED_Driver_UART_RX_PIN) */

#if (LED_Driver_UART_RX_WAKE_PIN)
    #define LED_Driver_RX_WAKE_HSIOM_REG   (*(reg32 *) LED_Driver_rx_wake__0__HSIOM)
    #define LED_Driver_RX_WAKE_HSIOM_PTR   ( (reg32 *) LED_Driver_rx_wake__0__HSIOM)
    
    #define LED_Driver_RX_WAKE_HSIOM_MASK      (LED_Driver_rx_wake__0__HSIOM_MASK)
    #define LED_Driver_RX_WAKE_HSIOM_POS       (LED_Driver_rx_wake__0__HSIOM_SHIFT)
    #define LED_Driver_RX_WAKE_HSIOM_SEL_GPIO  (LED_Driver_rx_wake__0__HSIOM_GPIO)
    #define LED_Driver_RX_WAKE_HSIOM_SEL_UART  (LED_Driver_rx_wake__0__HSIOM_UART)
#endif /* (LED_Driver_UART_WAKE_RX_PIN) */

#if (LED_Driver_UART_CTS_PIN)
    #define LED_Driver_CTS_HSIOM_REG   (*(reg32 *) LED_Driver_cts__0__HSIOM)
    #define LED_Driver_CTS_HSIOM_PTR   ( (reg32 *) LED_Driver_cts__0__HSIOM)
    
    #define LED_Driver_CTS_HSIOM_MASK      (LED_Driver_cts__0__HSIOM_MASK)
    #define LED_Driver_CTS_HSIOM_POS       (LED_Driver_cts__0__HSIOM_SHIFT)
    #define LED_Driver_CTS_HSIOM_SEL_GPIO  (LED_Driver_cts__0__HSIOM_GPIO)
    #define LED_Driver_CTS_HSIOM_SEL_UART  (LED_Driver_cts__0__HSIOM_UART)
#endif /* (LED_Driver_UART_CTS_PIN) */

#if (LED_Driver_UART_TX_PIN)
    #define LED_Driver_TX_HSIOM_REG   (*(reg32 *) LED_Driver_tx__0__HSIOM)
    #define LED_Driver_TX_HSIOM_PTR   ( (reg32 *) LED_Driver_tx__0__HSIOM)
    
    #define LED_Driver_TX_HSIOM_MASK      (LED_Driver_tx__0__HSIOM_MASK)
    #define LED_Driver_TX_HSIOM_POS       (LED_Driver_tx__0__HSIOM_SHIFT)
    #define LED_Driver_TX_HSIOM_SEL_GPIO  (LED_Driver_tx__0__HSIOM_GPIO)
    #define LED_Driver_TX_HSIOM_SEL_UART  (LED_Driver_tx__0__HSIOM_UART)
#endif /* (LED_Driver_UART_TX_PIN) */

#if (LED_Driver_UART_RX_TX_PIN)
    #define LED_Driver_RX_TX_HSIOM_REG   (*(reg32 *) LED_Driver_rx_tx__0__HSIOM)
    #define LED_Driver_RX_TX_HSIOM_PTR   ( (reg32 *) LED_Driver_rx_tx__0__HSIOM)
    
    #define LED_Driver_RX_TX_HSIOM_MASK      (LED_Driver_rx_tx__0__HSIOM_MASK)
    #define LED_Driver_RX_TX_HSIOM_POS       (LED_Driver_rx_tx__0__HSIOM_SHIFT)
    #define LED_Driver_RX_TX_HSIOM_SEL_GPIO  (LED_Driver_rx_tx__0__HSIOM_GPIO)
    #define LED_Driver_RX_TX_HSIOM_SEL_UART  (LED_Driver_rx_tx__0__HSIOM_UART)
#endif /* (LED_Driver_UART_RX_TX_PIN) */

#if (LED_Driver_UART_RTS_PIN)
    #define LED_Driver_RTS_HSIOM_REG      (*(reg32 *) LED_Driver_rts__0__HSIOM)
    #define LED_Driver_RTS_HSIOM_PTR      ( (reg32 *) LED_Driver_rts__0__HSIOM)
    
    #define LED_Driver_RTS_HSIOM_MASK     (LED_Driver_rts__0__HSIOM_MASK)
    #define LED_Driver_RTS_HSIOM_POS      (LED_Driver_rts__0__HSIOM_SHIFT)    
    #define LED_Driver_RTS_HSIOM_SEL_GPIO (LED_Driver_rts__0__HSIOM_GPIO)
    #define LED_Driver_RTS_HSIOM_SEL_UART (LED_Driver_rts__0__HSIOM_UART)    
#endif /* (LED_Driver_UART_RTS_PIN) */


/***************************************
*        Registers Constants
***************************************/

/* HSIOM switch values. */ 
#define LED_Driver_HSIOM_DEF_SEL      (0x00u)
#define LED_Driver_HSIOM_GPIO_SEL     (0x00u)
/* The HSIOM values provided below are valid only for LED_Driver_CY_SCBIP_V0 
* and LED_Driver_CY_SCBIP_V1. It is not recommended to use them for 
* LED_Driver_CY_SCBIP_V2. Use pin name specific HSIOM constants provided 
* above instead for any SCB IP block version.
*/
#define LED_Driver_HSIOM_UART_SEL     (0x09u)
#define LED_Driver_HSIOM_I2C_SEL      (0x0Eu)
#define LED_Driver_HSIOM_SPI_SEL      (0x0Fu)

/* Pins settings index. */
#define LED_Driver_RX_WAKE_SCL_MOSI_PIN_INDEX   (0u)
#define LED_Driver_RX_SCL_MOSI_PIN_INDEX       (0u)
#define LED_Driver_TX_SDA_MISO_PIN_INDEX       (1u)
#define LED_Driver_CTS_SCLK_PIN_INDEX       (2u)
#define LED_Driver_RTS_SS0_PIN_INDEX       (3u)
#define LED_Driver_SS1_PIN_INDEX                  (4u)
#define LED_Driver_SS2_PIN_INDEX                  (5u)
#define LED_Driver_SS3_PIN_INDEX                  (6u)

/* Pins settings mask. */
#define LED_Driver_RX_WAKE_SCL_MOSI_PIN_MASK ((uint32) 0x01u << LED_Driver_RX_WAKE_SCL_MOSI_PIN_INDEX)
#define LED_Driver_RX_SCL_MOSI_PIN_MASK     ((uint32) 0x01u << LED_Driver_RX_SCL_MOSI_PIN_INDEX)
#define LED_Driver_TX_SDA_MISO_PIN_MASK     ((uint32) 0x01u << LED_Driver_TX_SDA_MISO_PIN_INDEX)
#define LED_Driver_CTS_SCLK_PIN_MASK     ((uint32) 0x01u << LED_Driver_CTS_SCLK_PIN_INDEX)
#define LED_Driver_RTS_SS0_PIN_MASK     ((uint32) 0x01u << LED_Driver_RTS_SS0_PIN_INDEX)
#define LED_Driver_SS1_PIN_MASK                ((uint32) 0x01u << LED_Driver_SS1_PIN_INDEX)
#define LED_Driver_SS2_PIN_MASK                ((uint32) 0x01u << LED_Driver_SS2_PIN_INDEX)
#define LED_Driver_SS3_PIN_MASK                ((uint32) 0x01u << LED_Driver_SS3_PIN_INDEX)

/* Pin interrupt constants. */
#define LED_Driver_INTCFG_TYPE_MASK           (0x03u)
#define LED_Driver_INTCFG_TYPE_FALLING_EDGE   (0x02u)

/* Pin Drive Mode constants. */
#define LED_Driver_PIN_DM_ALG_HIZ  (0u)
#define LED_Driver_PIN_DM_DIG_HIZ  (1u)
#define LED_Driver_PIN_DM_OD_LO    (4u)
#define LED_Driver_PIN_DM_STRONG   (6u)


/***************************************
*          Macro Definitions
***************************************/

/* Return drive mode of the pin */
#define LED_Driver_DM_MASK    (0x7u)
#define LED_Driver_DM_SIZE    (3u)
#define LED_Driver_GET_P4_PIN_DM(reg, pos) \
    ( ((reg) & (uint32) ((uint32) LED_Driver_DM_MASK << (LED_Driver_DM_SIZE * (pos)))) >> \
                                                              (LED_Driver_DM_SIZE * (pos)) )

#if (LED_Driver_TX_SDA_MISO_PIN)
    #define LED_Driver_CHECK_TX_SDA_MISO_PIN_USED \
                (LED_Driver_PIN_DM_ALG_HIZ != \
                    LED_Driver_GET_P4_PIN_DM(LED_Driver_uart_tx_i2c_sda_spi_miso_PC, \
                                                   LED_Driver_uart_tx_i2c_sda_spi_miso_SHIFT))
#endif /* (LED_Driver_TX_SDA_MISO_PIN) */

#if (LED_Driver_RTS_SS0_PIN)
    #define LED_Driver_CHECK_RTS_SS0_PIN_USED \
                (LED_Driver_PIN_DM_ALG_HIZ != \
                    LED_Driver_GET_P4_PIN_DM(LED_Driver_uart_rts_spi_ss0_PC, \
                                                   LED_Driver_uart_rts_spi_ss0_SHIFT))
#endif /* (LED_Driver_RTS_SS0_PIN) */

/* Set bits-mask in register */
#define LED_Driver_SET_REGISTER_BITS(reg, mask, pos, mode) \
                    do                                           \
                    {                                            \
                        (reg) = (((reg) & ((uint32) ~(uint32) (mask))) | ((uint32) ((uint32) (mode) << (pos)))); \
                    }while(0)

/* Set bit in the register */
#define LED_Driver_SET_REGISTER_BIT(reg, mask, val) \
                    ((val) ? ((reg) |= (mask)) : ((reg) &= ((uint32) ~((uint32) (mask)))))

#define LED_Driver_SET_HSIOM_SEL(reg, mask, pos, sel) LED_Driver_SET_REGISTER_BITS(reg, mask, pos, sel)
#define LED_Driver_SET_INCFG_TYPE(reg, mask, pos, intType) \
                                                        LED_Driver_SET_REGISTER_BITS(reg, mask, pos, intType)
#define LED_Driver_SET_INP_DIS(reg, mask, val) LED_Driver_SET_REGISTER_BIT(reg, mask, val)

/* LED_Driver_SET_I2C_SCL_DR(val) - Sets I2C SCL DR register.
*  LED_Driver_SET_I2C_SCL_HSIOM_SEL(sel) - Sets I2C SCL HSIOM settings.
*/
/* SCB I2C: scl signal */
#if (LED_Driver_CY_SCBIP_V0)
#if (LED_Driver_I2C_PINS)
    #define LED_Driver_SET_I2C_SCL_DR(val) LED_Driver_scl_Write(val)

    #define LED_Driver_SET_I2C_SCL_HSIOM_SEL(sel) \
                          LED_Driver_SET_HSIOM_SEL(LED_Driver_SCL_HSIOM_REG,  \
                                                         LED_Driver_SCL_HSIOM_MASK, \
                                                         LED_Driver_SCL_HSIOM_POS,  \
                                                         (sel))
    #define LED_Driver_WAIT_SCL_SET_HIGH  (0u == LED_Driver_scl_Read())

/* Unconfigured SCB: scl signal */
#elif (LED_Driver_RX_WAKE_SCL_MOSI_PIN)
    #define LED_Driver_SET_I2C_SCL_DR(val) \
                            LED_Driver_uart_rx_wake_i2c_scl_spi_mosi_Write(val)

    #define LED_Driver_SET_I2C_SCL_HSIOM_SEL(sel) \
                    LED_Driver_SET_HSIOM_SEL(LED_Driver_RX_WAKE_SCL_MOSI_HSIOM_REG,  \
                                                   LED_Driver_RX_WAKE_SCL_MOSI_HSIOM_MASK, \
                                                   LED_Driver_RX_WAKE_SCL_MOSI_HSIOM_POS,  \
                                                   (sel))

    #define LED_Driver_WAIT_SCL_SET_HIGH  (0u == LED_Driver_uart_rx_wake_i2c_scl_spi_mosi_Read())

#elif (LED_Driver_RX_SCL_MOSI_PIN)
    #define LED_Driver_SET_I2C_SCL_DR(val) \
                            LED_Driver_uart_rx_i2c_scl_spi_mosi_Write(val)


    #define LED_Driver_SET_I2C_SCL_HSIOM_SEL(sel) \
                            LED_Driver_SET_HSIOM_SEL(LED_Driver_RX_SCL_MOSI_HSIOM_REG,  \
                                                           LED_Driver_RX_SCL_MOSI_HSIOM_MASK, \
                                                           LED_Driver_RX_SCL_MOSI_HSIOM_POS,  \
                                                           (sel))

    #define LED_Driver_WAIT_SCL_SET_HIGH  (0u == LED_Driver_uart_rx_i2c_scl_spi_mosi_Read())

#else
    #define LED_Driver_SET_I2C_SCL_DR(val)        do{ /* Does nothing */ }while(0)
    #define LED_Driver_SET_I2C_SCL_HSIOM_SEL(sel) do{ /* Does nothing */ }while(0)

    #define LED_Driver_WAIT_SCL_SET_HIGH  (0u)
#endif /* (LED_Driver_I2C_PINS) */

/* SCB I2C: sda signal */
#if (LED_Driver_I2C_PINS)
    #define LED_Driver_WAIT_SDA_SET_HIGH  (0u == LED_Driver_sda_Read())
/* Unconfigured SCB: sda signal */
#elif (LED_Driver_TX_SDA_MISO_PIN)
    #define LED_Driver_WAIT_SDA_SET_HIGH  (0u == LED_Driver_uart_tx_i2c_sda_spi_miso_Read())
#else
    #define LED_Driver_WAIT_SDA_SET_HIGH  (0u)
#endif /* (LED_Driver_MOSI_SCL_RX_PIN) */
#endif /* (LED_Driver_CY_SCBIP_V0) */

/* Clear UART wakeup source */
#if (LED_Driver_RX_SCL_MOSI_PIN)
    #define LED_Driver_CLEAR_UART_RX_WAKE_INTR        do{ /* Does nothing */ }while(0)
    
#elif (LED_Driver_RX_WAKE_SCL_MOSI_PIN)
    #define LED_Driver_CLEAR_UART_RX_WAKE_INTR \
            do{                                      \
                (void) LED_Driver_uart_rx_wake_i2c_scl_spi_mosi_ClearInterrupt(); \
            }while(0)

#elif(LED_Driver_UART_RX_WAKE_PIN)
    #define LED_Driver_CLEAR_UART_RX_WAKE_INTR \
            do{                                      \
                (void) LED_Driver_rx_wake_ClearInterrupt(); \
            }while(0)
#else
#endif /* (LED_Driver_RX_SCL_MOSI_PIN) */


/***************************************
* The following code is DEPRECATED and
* must not be used.
***************************************/

/* Unconfigured pins */
#define LED_Driver_REMOVE_MOSI_SCL_RX_WAKE_PIN    LED_Driver_REMOVE_RX_WAKE_SCL_MOSI_PIN
#define LED_Driver_REMOVE_MOSI_SCL_RX_PIN         LED_Driver_REMOVE_RX_SCL_MOSI_PIN
#define LED_Driver_REMOVE_MISO_SDA_TX_PIN         LED_Driver_REMOVE_TX_SDA_MISO_PIN
#ifndef LED_Driver_REMOVE_SCLK_PIN
#define LED_Driver_REMOVE_SCLK_PIN                LED_Driver_REMOVE_CTS_SCLK_PIN
#endif /* LED_Driver_REMOVE_SCLK_PIN */
#ifndef LED_Driver_REMOVE_SS0_PIN
#define LED_Driver_REMOVE_SS0_PIN                 LED_Driver_REMOVE_RTS_SS0_PIN
#endif /* LED_Driver_REMOVE_SS0_PIN */

/* Unconfigured pins */
#define LED_Driver_MOSI_SCL_RX_WAKE_PIN   LED_Driver_RX_WAKE_SCL_MOSI_PIN
#define LED_Driver_MOSI_SCL_RX_PIN        LED_Driver_RX_SCL_MOSI_PIN
#define LED_Driver_MISO_SDA_TX_PIN        LED_Driver_TX_SDA_MISO_PIN
#ifndef LED_Driver_SCLK_PIN
#define LED_Driver_SCLK_PIN               LED_Driver_CTS_SCLK_PIN
#endif /* LED_Driver_SCLK_PIN */
#ifndef LED_Driver_SS0_PIN
#define LED_Driver_SS0_PIN                LED_Driver_RTS_SS0_PIN
#endif /* LED_Driver_SS0_PIN */

#if (LED_Driver_MOSI_SCL_RX_WAKE_PIN)
    #define LED_Driver_MOSI_SCL_RX_WAKE_HSIOM_REG     LED_Driver_RX_WAKE_SCL_MOSI_HSIOM_REG
    #define LED_Driver_MOSI_SCL_RX_WAKE_HSIOM_PTR     LED_Driver_RX_WAKE_SCL_MOSI_HSIOM_REG
    #define LED_Driver_MOSI_SCL_RX_WAKE_HSIOM_MASK    LED_Driver_RX_WAKE_SCL_MOSI_HSIOM_REG
    #define LED_Driver_MOSI_SCL_RX_WAKE_HSIOM_POS     LED_Driver_RX_WAKE_SCL_MOSI_HSIOM_REG

    #define LED_Driver_MOSI_SCL_RX_WAKE_INTCFG_REG    LED_Driver_RX_WAKE_SCL_MOSI_HSIOM_REG
    #define LED_Driver_MOSI_SCL_RX_WAKE_INTCFG_PTR    LED_Driver_RX_WAKE_SCL_MOSI_HSIOM_REG

    #define LED_Driver_MOSI_SCL_RX_WAKE_INTCFG_TYPE_POS   LED_Driver_RX_WAKE_SCL_MOSI_HSIOM_REG
    #define LED_Driver_MOSI_SCL_RX_WAKE_INTCFG_TYPE_MASK  LED_Driver_RX_WAKE_SCL_MOSI_HSIOM_REG
#endif /* (LED_Driver_RX_WAKE_SCL_MOSI_PIN) */

#if (LED_Driver_MOSI_SCL_RX_PIN)
    #define LED_Driver_MOSI_SCL_RX_HSIOM_REG      LED_Driver_RX_SCL_MOSI_HSIOM_REG
    #define LED_Driver_MOSI_SCL_RX_HSIOM_PTR      LED_Driver_RX_SCL_MOSI_HSIOM_PTR
    #define LED_Driver_MOSI_SCL_RX_HSIOM_MASK     LED_Driver_RX_SCL_MOSI_HSIOM_MASK
    #define LED_Driver_MOSI_SCL_RX_HSIOM_POS      LED_Driver_RX_SCL_MOSI_HSIOM_POS
#endif /* (LED_Driver_MOSI_SCL_RX_PIN) */

#if (LED_Driver_MISO_SDA_TX_PIN)
    #define LED_Driver_MISO_SDA_TX_HSIOM_REG      LED_Driver_TX_SDA_MISO_HSIOM_REG
    #define LED_Driver_MISO_SDA_TX_HSIOM_PTR      LED_Driver_TX_SDA_MISO_HSIOM_REG
    #define LED_Driver_MISO_SDA_TX_HSIOM_MASK     LED_Driver_TX_SDA_MISO_HSIOM_REG
    #define LED_Driver_MISO_SDA_TX_HSIOM_POS      LED_Driver_TX_SDA_MISO_HSIOM_REG
#endif /* (LED_Driver_MISO_SDA_TX_PIN_PIN) */

#if (LED_Driver_SCLK_PIN)
    #ifndef LED_Driver_SCLK_HSIOM_REG
    #define LED_Driver_SCLK_HSIOM_REG     LED_Driver_CTS_SCLK_HSIOM_REG
    #define LED_Driver_SCLK_HSIOM_PTR     LED_Driver_CTS_SCLK_HSIOM_PTR
    #define LED_Driver_SCLK_HSIOM_MASK    LED_Driver_CTS_SCLK_HSIOM_MASK
    #define LED_Driver_SCLK_HSIOM_POS     LED_Driver_CTS_SCLK_HSIOM_POS
    #endif /* LED_Driver_SCLK_HSIOM_REG */
#endif /* (LED_Driver_SCLK_PIN) */

#if (LED_Driver_SS0_PIN)
    #ifndef LED_Driver_SS0_HSIOM_REG
    #define LED_Driver_SS0_HSIOM_REG      LED_Driver_RTS_SS0_HSIOM_REG
    #define LED_Driver_SS0_HSIOM_PTR      LED_Driver_RTS_SS0_HSIOM_PTR
    #define LED_Driver_SS0_HSIOM_MASK     LED_Driver_RTS_SS0_HSIOM_MASK
    #define LED_Driver_SS0_HSIOM_POS      LED_Driver_RTS_SS0_HSIOM_POS
    #endif /* LED_Driver_SS0_HSIOM_REG */
#endif /* (LED_Driver_SS0_PIN) */

#define LED_Driver_MOSI_SCL_RX_WAKE_PIN_INDEX LED_Driver_RX_WAKE_SCL_MOSI_PIN_INDEX
#define LED_Driver_MOSI_SCL_RX_PIN_INDEX      LED_Driver_RX_SCL_MOSI_PIN_INDEX
#define LED_Driver_MISO_SDA_TX_PIN_INDEX      LED_Driver_TX_SDA_MISO_PIN_INDEX
#ifndef LED_Driver_SCLK_PIN_INDEX
#define LED_Driver_SCLK_PIN_INDEX             LED_Driver_CTS_SCLK_PIN_INDEX
#endif /* LED_Driver_SCLK_PIN_INDEX */
#ifndef LED_Driver_SS0_PIN_INDEX
#define LED_Driver_SS0_PIN_INDEX              LED_Driver_RTS_SS0_PIN_INDEX
#endif /* LED_Driver_SS0_PIN_INDEX */

#define LED_Driver_MOSI_SCL_RX_WAKE_PIN_MASK LED_Driver_RX_WAKE_SCL_MOSI_PIN_MASK
#define LED_Driver_MOSI_SCL_RX_PIN_MASK      LED_Driver_RX_SCL_MOSI_PIN_MASK
#define LED_Driver_MISO_SDA_TX_PIN_MASK      LED_Driver_TX_SDA_MISO_PIN_MASK
#ifndef LED_Driver_SCLK_PIN_MASK
#define LED_Driver_SCLK_PIN_MASK             LED_Driver_CTS_SCLK_PIN_MASK
#endif /* LED_Driver_SCLK_PIN_MASK */
#ifndef LED_Driver_SS0_PIN_MASK
#define LED_Driver_SS0_PIN_MASK              LED_Driver_RTS_SS0_PIN_MASK
#endif /* LED_Driver_SS0_PIN_MASK */

#endif /* (CY_SCB_PINS_LED_Driver_H) */


/* [] END OF FILE */
