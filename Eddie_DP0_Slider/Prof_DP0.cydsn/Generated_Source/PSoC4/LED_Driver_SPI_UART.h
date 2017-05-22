/***************************************************************************//**
* \file LED_Driver_SPI_UART.h
* \version 3.20
*
* \brief
*  This file provides constants and parameter values for the SCB Component in
*  SPI and UART modes.
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

#if !defined(CY_SCB_SPI_UART_LED_Driver_H)
#define CY_SCB_SPI_UART_LED_Driver_H

#include "LED_Driver.h"


/***************************************
*   SPI Initial Parameter Constants
****************************************/

#define LED_Driver_SPI_MODE                   (1u)
#define LED_Driver_SPI_SUB_MODE               (0u)
#define LED_Driver_SPI_CLOCK_MODE             (0u)
#define LED_Driver_SPI_OVS_FACTOR             (16u)
#define LED_Driver_SPI_MEDIAN_FILTER_ENABLE   (0u)
#define LED_Driver_SPI_LATE_MISO_SAMPLE_ENABLE (0u)
#define LED_Driver_SPI_RX_DATA_BITS_NUM       (12u)
#define LED_Driver_SPI_TX_DATA_BITS_NUM       (12u)
#define LED_Driver_SPI_WAKE_ENABLE            (0u)
#define LED_Driver_SPI_BITS_ORDER             (1u)
#define LED_Driver_SPI_TRANSFER_SEPARATION    (1u)
#define LED_Driver_SPI_NUMBER_OF_SS_LINES     (0u)
#define LED_Driver_SPI_RX_BUFFER_SIZE         (8u)
#define LED_Driver_SPI_TX_BUFFER_SIZE         (8u)

#define LED_Driver_SPI_INTERRUPT_MODE         (0u)

#define LED_Driver_SPI_INTR_RX_MASK           (0u)
#define LED_Driver_SPI_INTR_TX_MASK           (0u)

#define LED_Driver_SPI_RX_TRIGGER_LEVEL       (7u)
#define LED_Driver_SPI_TX_TRIGGER_LEVEL       (0u)

#define LED_Driver_SPI_BYTE_MODE_ENABLE       (0u)
#define LED_Driver_SPI_FREE_RUN_SCLK_ENABLE   (0u)
#define LED_Driver_SPI_SS0_POLARITY           (0u)
#define LED_Driver_SPI_SS1_POLARITY           (0u)
#define LED_Driver_SPI_SS2_POLARITY           (0u)
#define LED_Driver_SPI_SS3_POLARITY           (0u)


/***************************************
*   UART Initial Parameter Constants
****************************************/

#define LED_Driver_UART_SUB_MODE              (0u)
#define LED_Driver_UART_DIRECTION             (3u)
#define LED_Driver_UART_DATA_BITS_NUM         (8u)
#define LED_Driver_UART_PARITY_TYPE           (2u)
#define LED_Driver_UART_STOP_BITS_NUM         (2u)
#define LED_Driver_UART_OVS_FACTOR            (12u)
#define LED_Driver_UART_IRDA_LOW_POWER        (0u)
#define LED_Driver_UART_MEDIAN_FILTER_ENABLE  (0u)
#define LED_Driver_UART_RETRY_ON_NACK         (0u)
#define LED_Driver_UART_IRDA_POLARITY         (0u)
#define LED_Driver_UART_DROP_ON_FRAME_ERR     (0u)
#define LED_Driver_UART_DROP_ON_PARITY_ERR    (0u)
#define LED_Driver_UART_WAKE_ENABLE           (0u)
#define LED_Driver_UART_RX_BUFFER_SIZE        (8u)
#define LED_Driver_UART_TX_BUFFER_SIZE        (8u)
#define LED_Driver_UART_MP_MODE_ENABLE        (0u)
#define LED_Driver_UART_MP_ACCEPT_ADDRESS     (0u)
#define LED_Driver_UART_MP_RX_ADDRESS         (2u)
#define LED_Driver_UART_MP_RX_ADDRESS_MASK    (255u)

#define LED_Driver_UART_INTERRUPT_MODE        (0u)

#define LED_Driver_UART_INTR_RX_MASK          (0u)
#define LED_Driver_UART_INTR_TX_MASK          (0u)

#define LED_Driver_UART_RX_TRIGGER_LEVEL      (7u)
#define LED_Driver_UART_TX_TRIGGER_LEVEL      (0u)

#define LED_Driver_UART_BYTE_MODE_ENABLE      (0u)
#define LED_Driver_UART_CTS_ENABLE            (0u)
#define LED_Driver_UART_CTS_POLARITY          (0u)
#define LED_Driver_UART_RTS_ENABLE            (0u)
#define LED_Driver_UART_RTS_POLARITY          (0u)
#define LED_Driver_UART_RTS_FIFO_LEVEL        (4u)

/* SPI mode enum */
#define LED_Driver_SPI_SLAVE  (0u)
#define LED_Driver_SPI_MASTER (1u)

/* UART direction enum */
#define LED_Driver_UART_RX    (1u)
#define LED_Driver_UART_TX    (2u)
#define LED_Driver_UART_TX_RX (3u)


/***************************************
*   Conditional Compilation Parameters
****************************************/

#if(LED_Driver_SCB_MODE_UNCONFIG_CONST_CFG)

    /* Mode */
    #define LED_Driver_SPI_SLAVE_CONST        (1u)
    #define LED_Driver_SPI_MASTER_CONST       (1u)

    /* Direction */
    #define LED_Driver_RX_DIRECTION           (1u)
    #define LED_Driver_TX_DIRECTION           (1u)
    #define LED_Driver_UART_RX_DIRECTION      (1u)
    #define LED_Driver_UART_TX_DIRECTION      (1u)

    /* Only external RX and TX buffer for Uncofigured mode */
    #define LED_Driver_INTERNAL_RX_SW_BUFFER   (0u)
    #define LED_Driver_INTERNAL_TX_SW_BUFFER   (0u)

    /* Get RX and TX buffer size */
    #define LED_Driver_INTERNAL_RX_BUFFER_SIZE    (LED_Driver_rxBufferSize + 1u)
    #define LED_Driver_RX_BUFFER_SIZE             (LED_Driver_rxBufferSize)
    #define LED_Driver_TX_BUFFER_SIZE             (LED_Driver_txBufferSize)

    /* Return true if buffer is provided */
    #define LED_Driver_CHECK_RX_SW_BUFFER (NULL != LED_Driver_rxBuffer)
    #define LED_Driver_CHECK_TX_SW_BUFFER (NULL != LED_Driver_txBuffer)

    /* Always provide global variables to support RX and TX buffers */
    #define LED_Driver_INTERNAL_RX_SW_BUFFER_CONST    (1u)
    #define LED_Driver_INTERNAL_TX_SW_BUFFER_CONST    (1u)

    /* Get wakeup enable option */
    #define LED_Driver_SPI_WAKE_ENABLE_CONST  (1u)
    #define LED_Driver_UART_WAKE_ENABLE_CONST (1u)
    #define LED_Driver_CHECK_SPI_WAKE_ENABLE  ((0u != LED_Driver_scbEnableWake) && LED_Driver_SCB_MODE_SPI_RUNTM_CFG)
    #define LED_Driver_CHECK_UART_WAKE_ENABLE ((0u != LED_Driver_scbEnableWake) && LED_Driver_SCB_MODE_UART_RUNTM_CFG)

    /* SPI/UART: TX or RX FIFO size */
    #if (LED_Driver_CY_SCBIP_V0 || LED_Driver_CY_SCBIP_V1)
        #define LED_Driver_SPI_UART_FIFO_SIZE             (LED_Driver_FIFO_SIZE)
        #define LED_Driver_CHECK_UART_RTS_CONTROL_FLOW    (0u)
    #else
        #define LED_Driver_SPI_UART_FIFO_SIZE (LED_Driver_GET_FIFO_SIZE(LED_Driver_CTRL_REG & \
                                                                                    LED_Driver_CTRL_BYTE_MODE))

        #define LED_Driver_CHECK_UART_RTS_CONTROL_FLOW \
                    ((LED_Driver_SCB_MODE_UART_RUNTM_CFG) && \
                     (0u != LED_Driver_GET_UART_FLOW_CTRL_TRIGGER_LEVEL(LED_Driver_UART_FLOW_CTRL_REG)))
    #endif /* (LED_Driver_CY_SCBIP_V0 || LED_Driver_CY_SCBIP_V1) */

#else

    /* Internal RX and TX buffer: for SPI or UART */
    #if (LED_Driver_SCB_MODE_SPI_CONST_CFG)

        /* SPI Direction */
        #define LED_Driver_SPI_RX_DIRECTION (1u)
        #define LED_Driver_SPI_TX_DIRECTION (1u)

        /* Get FIFO size */
        #define LED_Driver_SPI_UART_FIFO_SIZE LED_Driver_GET_FIFO_SIZE(LED_Driver_SPI_BYTE_MODE_ENABLE)

        /* SPI internal RX and TX buffers */
        #define LED_Driver_INTERNAL_SPI_RX_SW_BUFFER  (LED_Driver_SPI_RX_BUFFER_SIZE > \
                                                                LED_Driver_SPI_UART_FIFO_SIZE)
        #define LED_Driver_INTERNAL_SPI_TX_SW_BUFFER  (LED_Driver_SPI_TX_BUFFER_SIZE > \
                                                                LED_Driver_SPI_UART_FIFO_SIZE)

        /* Internal SPI RX and TX buffer */
        #define LED_Driver_INTERNAL_RX_SW_BUFFER  (LED_Driver_INTERNAL_SPI_RX_SW_BUFFER)
        #define LED_Driver_INTERNAL_TX_SW_BUFFER  (LED_Driver_INTERNAL_SPI_TX_SW_BUFFER)

        /* Internal SPI RX and TX buffer size */
        #define LED_Driver_INTERNAL_RX_BUFFER_SIZE    (LED_Driver_SPI_RX_BUFFER_SIZE + 1u)
        #define LED_Driver_RX_BUFFER_SIZE             (LED_Driver_SPI_RX_BUFFER_SIZE)
        #define LED_Driver_TX_BUFFER_SIZE             (LED_Driver_SPI_TX_BUFFER_SIZE)

        /* Get wakeup enable option */
        #define LED_Driver_SPI_WAKE_ENABLE_CONST  (0u != LED_Driver_SPI_WAKE_ENABLE)
        #define LED_Driver_UART_WAKE_ENABLE_CONST (0u)

    #else

        /* UART Direction */
        #define LED_Driver_UART_RX_DIRECTION (0u != (LED_Driver_UART_DIRECTION & LED_Driver_UART_RX))
        #define LED_Driver_UART_TX_DIRECTION (0u != (LED_Driver_UART_DIRECTION & LED_Driver_UART_TX))

        /* Get FIFO size */
        #define LED_Driver_SPI_UART_FIFO_SIZE LED_Driver_GET_FIFO_SIZE(LED_Driver_UART_BYTE_MODE_ENABLE)

        /* UART internal RX and TX buffers */
        #define LED_Driver_INTERNAL_UART_RX_SW_BUFFER  (LED_Driver_UART_RX_BUFFER_SIZE > \
                                                                LED_Driver_SPI_UART_FIFO_SIZE)
        #define LED_Driver_INTERNAL_UART_TX_SW_BUFFER  (LED_Driver_UART_TX_BUFFER_SIZE > \
                                                                    LED_Driver_SPI_UART_FIFO_SIZE)

        /* Internal UART RX and TX buffer */
        #define LED_Driver_INTERNAL_RX_SW_BUFFER  (LED_Driver_INTERNAL_UART_RX_SW_BUFFER)
        #define LED_Driver_INTERNAL_TX_SW_BUFFER  (LED_Driver_INTERNAL_UART_TX_SW_BUFFER)

        /* Internal UART RX and TX buffer size */
        #define LED_Driver_INTERNAL_RX_BUFFER_SIZE    (LED_Driver_UART_RX_BUFFER_SIZE + 1u)
        #define LED_Driver_RX_BUFFER_SIZE             (LED_Driver_UART_RX_BUFFER_SIZE)
        #define LED_Driver_TX_BUFFER_SIZE             (LED_Driver_UART_TX_BUFFER_SIZE)

        /* Get wakeup enable option */
        #define LED_Driver_SPI_WAKE_ENABLE_CONST  (0u)
        #define LED_Driver_UART_WAKE_ENABLE_CONST (0u != LED_Driver_UART_WAKE_ENABLE)

    #endif /* (LED_Driver_SCB_MODE_SPI_CONST_CFG) */

    /* Mode */
    #define LED_Driver_SPI_SLAVE_CONST    (LED_Driver_SPI_MODE == LED_Driver_SPI_SLAVE)
    #define LED_Driver_SPI_MASTER_CONST   (LED_Driver_SPI_MODE == LED_Driver_SPI_MASTER)

    /* Direction */
    #define LED_Driver_RX_DIRECTION ((LED_Driver_SCB_MODE_SPI_CONST_CFG) ? \
                                            (LED_Driver_SPI_RX_DIRECTION) : (LED_Driver_UART_RX_DIRECTION))

    #define LED_Driver_TX_DIRECTION ((LED_Driver_SCB_MODE_SPI_CONST_CFG) ? \
                                            (LED_Driver_SPI_TX_DIRECTION) : (LED_Driver_UART_TX_DIRECTION))

    /* Internal RX and TX buffer: for SPI or UART. Used in conditional compilation check */
    #define LED_Driver_CHECK_RX_SW_BUFFER (LED_Driver_INTERNAL_RX_SW_BUFFER)
    #define LED_Driver_CHECK_TX_SW_BUFFER (LED_Driver_INTERNAL_TX_SW_BUFFER)

    /* Provide global variables to support RX and TX buffers */
    #define LED_Driver_INTERNAL_RX_SW_BUFFER_CONST    (LED_Driver_INTERNAL_RX_SW_BUFFER)
    #define LED_Driver_INTERNAL_TX_SW_BUFFER_CONST    (LED_Driver_INTERNAL_TX_SW_BUFFER)

    /* Wake up enable */
    #define LED_Driver_CHECK_SPI_WAKE_ENABLE  (LED_Driver_SPI_WAKE_ENABLE_CONST)
    #define LED_Driver_CHECK_UART_WAKE_ENABLE (LED_Driver_UART_WAKE_ENABLE_CONST)

    /* UART flow control: not applicable for CY_SCBIP_V0 || CY_SCBIP_V1 */
    #define LED_Driver_CHECK_UART_RTS_CONTROL_FLOW    (LED_Driver_SCB_MODE_UART_CONST_CFG && \
                                                             LED_Driver_UART_RTS_ENABLE)

#endif /* End (LED_Driver_SCB_MODE_UNCONFIG_CONST_CFG) */


/***************************************
*       Type Definitions
***************************************/

/**
* \addtogroup group_structures
* @{
*/ 

/* LED_Driver_SPI_INIT_STRUCT */
typedef struct
{   
    /** Mode of operation for SPI. The following defines are available choices:
     *  - LED_Driver_SPI_SLAVE 
     *  - LED_Driver_SPI_MASTE
    */
    uint32 mode;
    
    /** Submode of operation for SPI. The following defines are available 
     *  choices: 
     *  - LED_Driver_SPI_MODE_MOTOROLA 
     *  - LED_Driver_SPI_MODE_TI_COINCIDES
     *  - LED_Driver_SPI_MODE_TI_PRECEDES
     *  - LED_Driver_SPI_MODE_NATIONAL
    */
    uint32 submode;
    
    /** Determines the sclk relationship for Motorola submode. Ignored 
     *  for other submodes. The following defines are available choices:
     *  - LED_Driver_SPI_SCLK_CPHA0_CPOL0
     *  - LED_Driver_SPI_SCLK_CPHA0_CPOL1
     *  - LED_Driver_SPI_SCLK_CPHA1_CPOL0
     *  - LED_Driver_SPI_SCLK_CPHA1_CPOL1
    */
    uint32 sclkMode;
    
    /** Oversampling factor for the SPI clock. Ignored for Slave mode operation.
    */
    uint32 oversample;
    
    /** Applies median filter on the input lines: 0 – not applied, 1 – applied.
    */
    uint32 enableMedianFilter;
    
    /** Applies late sampling of MISO line: 0 – not applied, 1 – applied.
     *  Ignored for slave mode.
    */
    uint32 enableLateSampling;
    
    /** Enables wakeup from low power mode: 0 – disable, 1 – enable. 
     *  Ignored for master mode.
    */
    uint32 enableWake;
    
    /** Number of data bits for RX direction.
     *  Different dataBitsRx and dataBitsTx are only allowed for National 
     *  submode.
    */
    uint32 rxDataBits;
    
    /** Number of data bits for TX direction.
     *  Different dataBitsRx and dataBitsTx are only allowed for National 
     *  submode.
    */
    uint32 txDataBits;
    
    /** Determines the bit ordering. The following defines are available 
     *  choices:
     *  - LED_Driver_BITS_ORDER_LSB_FIRST
     *  - LED_Driver_BITS_ORDER_MSB_FIRST
    */
    uint32 bitOrder;
    
    /** Determines whether transfers are back to back or have SS disabled 
     *  between words. Ignored for slave mode. The following defines are 
     *  available choices: 
     *  - LED_Driver_SPI_TRANSFER_CONTINUOUS
     *  - LED_Driver_SPI_TRANSFER_SEPARATED
    */
    uint32 transferSeperation;
    
    /** Size of the RX buffer in bytes/words (depends on rxDataBits parameter). 
     *  A value equal to the RX FIFO depth implies the usage of buffering in 
     *  hardware. A value greater than the RX FIFO depth results in a software 
     *  buffer.
     *  The LED_Driver_INTR _RX_NOT_EMPTY interrupt has to be enabled to 
     *  transfer data into the software buffer.
     *  - The RX and TX FIFO depth is equal to 8 bytes/words for PSoC 4100 / 
     *    PSoC 4200 devices.
     *  - The RX and TX FIFO depth is equal to 8 bytes/words or 16 
     *    bytes (Byte mode is enabled) for PSoC 4100 BLE / PSoC 4200 BLE / 
     *    PSoC 4100M / PSoC 4200M / PSoC 4200L / PSoC 4000S / PSoC 4100S / 
     *    PSoC Analog Coprocessor devices.
    */
    uint32 rxBufferSize; 
    
    /** Buffer space provided for a RX software buffer:
     *  - A NULL pointer must be provided to use hardware buffering.
     *  - A pointer to an allocated buffer must be provided to use software 
     *    buffering. The buffer size must equal (rxBufferSize + 1) in bytes if 
     *    dataBitsRx is less or equal to 8, otherwise (2 * (rxBufferSize + 1)) 
     *    in bytes. The software RX buffer always keeps one element empty. 
     *    For correct operation the allocated RX buffer has to be one element 
     *    greater than maximum packet size expected to be received.
    */
    uint8* rxBuffer;
    
    /** Size of the TX buffer in bytes/words(depends on txDataBits parameter). 
     *  A value equal to the TX FIFO depth implies the usage of buffering in 
     *  hardware. A value greater than the TX FIFO depth results in a software 
     *  buffer.
     *  - The RX and TX FIFO depth is equal to 8 bytes/words for PSoC 4100 / 
     *    PSoC 4200 devices.
     *  - The RX and TX FIFO depth is equal to 8 bytes/words or 16 
     *    bytes (Byte mode is enabled) for PSoC 4100 BLE / PSoC 4200 BLE / 
     *    PSoC 4100M / PSoC 4200M / PSoC 4200L / PSoC 4000S / PSoC 4100S / 
     *    PSoC Analog Coprocessor devices.
    */
    uint32 txBufferSize;
    
    /** Buffer space provided for a TX software buffer:
     *  - A NULL pointer must be provided to use hardware buffering.
     *  - A pointer to an allocated buffer must be provided to use software 
     *    buffering. The buffer size must equal txBufferSize if dataBitsTx is 
     *    less or eqal to 8, otherwise (2* txBufferSize).
    */
    uint8* txBuffer;
    
    /** Enables component interrupt: 0 – disable, 1 – enable.
     *  The interrupt has to be enabled if software buffer is used.
    */
    uint32 enableInterrupt;
    
    /** Mask of enabled interrupt sources for the RX direction. This mask is 
     *  written regardless of the setting of the enable Interrupt field. 
     *  Multiple sources are enabled by providing a value that is the OR of
     *  all of the following sources to enable:
     *  - LED_Driver_INTR_RX_FIFO_LEVEL
     *  - LED_Driver_INTR_RX_NOT_EMPTY
     *  - LED_Driver_INTR_RX_FULL
     *  - LED_Driver_INTR_RX_OVERFLOW
     *  - LED_Driver_INTR_RX_UNDERFLOW
     *  - LED_Driver_INTR_SLAVE_SPI_BUS_ERROR
    */
    uint32 rxInterruptMask;
    
    /** FIFO level for an RX FIFO level interrupt. This value is written 
     *  regardless of whether the RX FIFO level interrupt source is enabled.
    */
    uint32 rxTriggerLevel;
    
    /** Mask of enabled interrupt sources for the TX direction. This mask is 
     *  written regardless of the setting of the enable Interrupt field. 
     *  Multiple sources are enabled by providing a value that is the OR of
     *  all of the following sources to enable:
     *  - LED_Driver_INTR_TX_FIFO_LEVEL
     *  - LED_Driver_INTR_TX_NOT_FULL
     *  - LED_Driver_INTR_TX_EMPTY
     *  - LED_Driver_INTR_TX_OVERFLOW
     *  - LED_Driver_INTR_TX_UNDERFLOW
     *  - LED_Driver_INTR_MASTER_SPI_DONE
    */
    uint32 txInterruptMask;

    /** FIFO level for a TX FIFO level interrupt. This value is written 
     * regardless of whether the TX FIFO level interrupt source is enabled.
    */
    uint32 txTriggerLevel;
    
    /** When enabled the TX and RX FIFO depth is doubled and equal to 
     *  16 bytes: 0 – disable, 1 – enable. This implies that number of 
     *  TX and RX data bits must be less than or equal to 8.
     *
     *  Ignored for all devices other than PSoC 4100 BLE / PSoC 4200 BLE / 
     *  PSoC 4100M / PSoC 4200M / PSoC 4200L / PSoC 4000S / PSoC 4100S / 
     *  PSoC Analog Coprocessor.
    */
    uint8 enableByteMode;
    
    /** Enables continuous SCLK generation by the SPI master: 0 – disable, 
     *  1 – enable.
     *
     *  Ignored for all devices other than PSoC 4100 BLE / PSoC 4200 BLE / 
     *  PSoC 4100M / PSoC 4200M / PSoC 4200L / PSoC 4000S / PSoC 4100S / 
     *  PSoC Analog Coprocessor.
    */
    uint8 enableFreeRunSclk;
    
    /** Active polarity of slave select lines 0-3. This is bitmask where bit 
     *  LED_Driver_SPI_SLAVE_SELECT0 corresponds to slave select 0 
     *  polarity, bit LED_Driver_SPI_SLAVE_SELECT1 – slave select 1 
     *  polarity and so on. Polarity constants are: 
     *  - LED_Driver_SPI_SS_ACTIVE_LOW 
     *  - LED_Driver_SPI_SS_ACTIVE_HIGH
     *
     *  Ignored for all devices other than PSoC 4100 BLE / PSoC 4200 BLE / 
     *  PSoC 4100M / PSoC 4200M / PSoC 4200L / PSoC 4000S / PSoC 4100S / 
     *  PSoC Analog Coprocessor.
    */
    uint8 polaritySs;
} LED_Driver_SPI_INIT_STRUCT;


/* LED_Driver_UART_INIT_STRUCT */
typedef struct
{
    /** Mode of operation for the UART. The following defines are available 
     *  choices:
     *  - LED_Driver_UART_MODE_STD
     *  - LED_Driver_UART_MODE_SMARTCARD 
     *  - LED_Driver_UART_MODE_IRDA
    */
    uint32 mode;
    
    /** Direction of operation for the UART. The following defines are available
     *  choices:
     *  - LED_Driver_UART_TX_RX
     *  - LED_Driver_UART_RX
     *  - LED_Driver_UART_TX
    */
    uint32 direction;
    
    /** Number of data bits.
    */
    uint32 dataBits;
    
    /** Determines the parity. The following defines are available choices:
     *  - LED_Driver_UART_PARITY_EVEN
     *  - LED_Driver_UART_PARITY_ODD
     *  - LED_Driver_UART_PARITY_NONE
    */
    uint32 parity;
    
    /** Determines the number of stop bits. The following defines are available 
     *  choices:
     *  - LED_Driver_UART_STOP_BITS_1
     *  - LED_Driver_UART_STOP_BITS_1_5
     *  - LED_Driver_UART_STOP_BITS_2
    */
    uint32 stopBits;
    
    /** Oversampling factor for the UART.
     * 
     *  Note The oversampling factor values are changed when enableIrdaLowPower 
     *  is enabled:
     *  - LED_Driver_UART_IRDA_LP_OVS16
     *  - LED_Driver_UART_IRDA_LP_OVS32
     *  - LED_Driver_UART_IRDA_LP_OVS48
     *  - LED_Driver_UART_IRDA_LP_OVS96
     *  - LED_Driver_UART_IRDA_LP_OVS192
     *  - LED_Driver_UART_IRDA_LP_OVS768
     *  - LED_Driver_UART_IRDA_LP_OVS1536
    */
    uint32 oversample;
    
    /** Enables IrDA low power RX mode operation: 0 – disable, 1 – enable.
     *  The TX functionality does not work when enabled.
    */
    uint32 enableIrdaLowPower;
    
    /** Applies median filter on the input lines:  0 – not applied, 1 – applied. 
    */
    uint32 enableMedianFilter;
    
    /** Enables retry when NACK response was received: 0 – disable, 1 – enable. 
     *  Only current content of TX FIFO is re-sent.
     *  Ignored for modes other than SmartCard.
    */
    uint32 enableRetryNack;
    
    /** Inverts polarity of RX line: 0 – non-inverting, 1 – inverting. 
     *  Ignored for modes other than IrDA.
    */
    uint32 enableInvertedRx;
    
    /** Drop data from RX FIFO if parity error is detected: 0 – disable, 
     *  1 – enable. 
    */
    uint32 dropOnParityErr;
    
    /** Drop data from RX FIFO if a frame error is detected: 0 – disable, 
     *  1 – enable.
    */
    uint32 dropOnFrameErr;
    
    /** Enables wakeup from low power mode: 0 – disable, 1 – enable. 
     *  Ignored for modes other than standard UART. The RX functionality 
     *  has to be enabled.
    */
    uint32 enableWake;
    
    /** Size of the RX buffer in bytes/words (depends on rxDataBits parameter). 
     *  A value equal to the RX FIFO depth implies the usage of buffering in 
     *  hardware. A value greater than the RX FIFO depth results in a software 
     *  buffer.
     *  The LED_Driver_INTR _RX_NOT_EMPTY interrupt has to be enabled to 
     *  transfer data into the software buffer.
     *  - The RX and TX FIFO depth is equal to 8 bytes/words for PSoC 4100 / 
     *    PSoC 4200 devices.
     *  - The RX and TX FIFO depth is equal to 8 bytes/words or 16 
     *    bytes (Byte mode is enabled) for PSoC 4100 BLE / PSoC 4200 BLE / 
     *    PSoC 4100M / PSoC 4200M / PSoC 4200L / PSoC 4000S / PSoC 4100S / 
     *    PSoC Analog Coprocessor devices.
    */
    uint32 rxBufferSize;
    
    /** Buffer space provided for a RX software buffer:
     *  - A NULL pointer must be provided to use hardware buffering.
     *  - A pointer to an allocated buffer must be provided to use software 
     *    buffering. The buffer size must equal (rxBufferSize + 1) in bytes if 
     *    dataBitsRx is less or equal to 8, otherwise (2 * (rxBufferSize + 1)) 
     *    in bytes. The software RX buffer always keeps one element empty. 
     *    For correct operation the allocated RX buffer has to be one element 
     *    greater than maximum packet size expected to be received.
    */
    uint8* rxBuffer;
    
    /** Size of the TX buffer in bytes/words(depends on txDataBits parameter). 
     *  A value equal to the TX FIFO depth implies the usage of buffering in 
     *  hardware. A value greater than the TX FIFO depth results in a software 
     *  buffer.
     *  - The RX and TX FIFO depth is equal to 8 bytes/words for PSoC 4100 / 
     *    PSoC 4200 devices.
     *  - The RX and TX FIFO depth is equal to 8 bytes/words or 16 
     *    bytes (Byte mode is enabled) for PSoC 4100 BLE / PSoC 4200 BLE / 
     *    PSoC 4100M / PSoC 4200M / PSoC 4200L / PSoC 4000S / PSoC 4100S / 
     *    PSoC Analog Coprocessor devices.
    */
    uint32 txBufferSize;
    
    /** Buffer space provided for a TX software buffer:
     *  - A NULL pointer must be provided to use hardware buffering.
     *  - A pointer to an allocated buffer must be provided to use software 
     *    buffering. The buffer size must equal txBufferSize if dataBitsTx is 
     *    less or eqal to 8, otherwise (2* txBufferSize).
    */
    uint8* txBuffer;
    
    /** Enables multiprocessor mode: 0 – disable, 1 – enable.
    */
    uint32 enableMultiproc;
    
    /** Enables matched address to be accepted: 0 – disable, 1 – enable.
    */
    uint32 multiprocAcceptAddr;
    
    /** 8 bit address to match in Multiprocessor mode. Ignored for other modes.
    */
    uint32 multiprocAddr;
    
    /** 8 bit mask of address bits that are compared for a Multiprocessor 
     *  address match. Ignored for other modes.
     *  - Bit value 0 – excludes bit from address comparison.
     *  - Bit value 1 – the bit needs to match with the corresponding bit 
     *   of the device address.
    */
    uint32 multiprocAddrMask;
    
    /** Enables component interrupt: 0 – disable, 1 – enable.
     *  The interrupt has to be enabled if software buffer is used.
    */
    uint32 enableInterrupt;
    
    /** Mask of interrupt sources to enable in the RX direction. This mask is 
     *  written regardless of the setting of the enableInterrupt field. 
     *  Multiple sources are enabled by providing a value that is the OR of 
     *  all of the following sources to enable:
     *  - LED_Driver_INTR_RX_FIFO_LEVEL
     *  - LED_Driver_INTR_RX_NOT_EMPTY
     *  - LED_Driver_INTR_RX_FULL
     *  - LED_Driver_INTR_RX_OVERFLOW
     *  - LED_Driver_INTR_RX_UNDERFLOW
     *  - LED_Driver_INTR_RX_FRAME_ERROR
     *  - LED_Driver_INTR_RX_PARITY_ERROR
    */
    uint32 rxInterruptMask;
    
    /** FIFO level for an RX FIFO level interrupt. This value is written 
     *  regardless of whether the RX FIFO level interrupt source is enabled.
    */
    uint32 rxTriggerLevel;
    
    /** Mask of interrupt sources to enable in the TX direction. This mask is 
     *  written regardless of the setting of the enableInterrupt field. 
     *  Multiple sources are enabled by providing a value that is the OR of 
     *  all of the following sources to enable:
     *  - LED_Driver_INTR_TX_FIFO_LEVEL
     *  - LED_Driver_INTR_TX_NOT_FULL
     *  - LED_Driver_INTR_TX_EMPTY
     *  - LED_Driver_INTR_TX_OVERFLOW
     *  - LED_Driver_INTR_TX_UNDERFLOW
     *  - LED_Driver_INTR_TX_UART_DONE
     *  - LED_Driver_INTR_TX_UART_NACK
     *  - LED_Driver_INTR_TX_UART_ARB_LOST
    */
    uint32 txInterruptMask;
    
    /** FIFO level for a TX FIFO level interrupt. This value is written 
     *  regardless of whether the TX FIFO level interrupt source is enabled.
    */
    uint32 txTriggerLevel;
    
    /** When enabled the TX and RX FIFO depth is doubled and equal to 
     *  16 bytes: 0 – disable, 1 – enable. This implies that number of 
     *  Data bits must be less than or equal to 8.
     *
     *  Ignored for all devices other than PSoC 4100 BLE / PSoC 4200 BLE / 
     *  PSoC 4100M / PSoC 4200M / PSoC 4200L / PSoC 4000S / PSoC 4100S / 
     *  PSoC Analog Coprocessor.
    */
    uint8 enableByteMode;
    
    /** Enables usage of CTS input signal by the UART transmitter : 0 – disable,
     *  1 – enable.
     *
     *  Ignored for all devices other than PSoC 4100 BLE / PSoC 4200 BLE / 
     *  PSoC 4100M / PSoC 4200M / PSoC 4200L / PSoC 4000S / PSoC 4100S / 
     *  PSoC Analog Coprocessor.
    */    
    uint8 enableCts;
    
    /** Sets active polarity of CTS input signal: 
     *  - LED_Driver_UART_CTS_ACTIVE_LOW
     *  - LED_Driver_UART_CTS_ACTIVE_HIGH
     *
     *  Ignored for all devices other than PSoC 4100 BLE / PSoC 4200 BLE / 
     *  PSoC 4100M / PSoC 4200M / PSoC 4200L / PSoC 4000S / PSoC 4100S / 
     *  PSoC Analog Coprocessor.
    */
    uint8 ctsPolarity;
    
    /** RX FIFO level for RTS signal activation. While the RX FIFO has fewer 
     *  entries than the RTS FIFO level value the RTS signal remains active, 
     *  otherwise the RTS signal becomes inactive. By setting this field to 0, 
     *  RTS signal activation is disabled.
     *
     *  Ignored for all devices other than PSoC 4100 BLE / PSoC 4200 BLE / 
     *  PSoC 4100M / PSoC 4200M / PSoC 4200L / PSoC 4000S / PSoC 4100S / 
     *  PSoC Analog Coprocessor.
    */
    uint8 rtsRxFifoLevel;

    /** Sets active polarity of RTS output signal: 
     *  - LED_Driver_UART_RTS_ ACTIVE_LOW 
     *  - LED_Driver_UART_RTS_ACTIVE_HIGH
     *
     *  Ignored for all devices other than PSoC 4100 BLE / PSoC 4200 BLE / 
     *  PSoC 4100M / PSoC 4200M / PSoC 4200L / PSoC 4000S / PSoC 4100S / 
     *  PSoC Analog Coprocessor.
    */
    uint8 rtsPolarity;
} LED_Driver_UART_INIT_STRUCT;

/** @} structures */


/***************************************
*        Function Prototypes
***************************************/

/**
* \addtogroup group_spi
* @{
*/
/* SPI specific functions */
#if(LED_Driver_SCB_MODE_UNCONFIG_CONST_CFG)
    void LED_Driver_SpiInit(const LED_Driver_SPI_INIT_STRUCT *config);
#endif /* (LED_Driver_SCB_MODE_UNCONFIG_CONST_CFG) */


#if(LED_Driver_SCB_MODE_SPI_INC)
    /*******************************************************************************
    * Function Name: LED_Driver_SpiIsBusBusy
    ****************************************************************************//**
    *
    *  Returns the current status on the bus. The bus status is determined using 
    *  the slave select signal.
    *  - Motorola and National Semiconductor sub-modes: The bus is busy after 
    *    the slave select line is activated and lasts until the slave select line 
    *    is deactivated.
    *  - Texas Instrument sub-modes: The bus is busy at the moment of the initial 
    *    pulse on the slave select line and lasts until the transfer is complete.
    *    If SPI Master is configured to use "Separated transfers" 
    *    (see Continuous versus Separated Transfer Separation), the bus is busy 
    *    during each element transfer and is free between each element transfer. 
    *    The Master does not activate SS line immediately after data has been 
    *    written into the TX FIFO.
    *
    *  \return slaveSelect: Current status on the bus. 
    *   If the returned value is nonzero, the bus is busy. 
    *   If zero is returned, the bus is free. The bus status is determined using 
    *   the slave select signal.
    *
    *******************************************************************************/
    #define LED_Driver_SpiIsBusBusy() ((uint32) (0u != (LED_Driver_SPI_STATUS_REG & \
                                                              LED_Driver_SPI_STATUS_BUS_BUSY)))

    #if (LED_Driver_SPI_MASTER_CONST)
        void LED_Driver_SpiSetActiveSlaveSelect(uint32 slaveSelect);
    #endif /*(LED_Driver_SPI_MASTER_CONST) */

    #if !(LED_Driver_CY_SCBIP_V0 || LED_Driver_CY_SCBIP_V1)
        void LED_Driver_SpiSetSlaveSelectPolarity(uint32 slaveSelect, uint32 polarity);
    #endif /* !(LED_Driver_CY_SCBIP_V0 || LED_Driver_CY_SCBIP_V1) */
#endif /* (LED_Driver_SCB_MODE_SPI_INC) */
/** @} spi */

/**
* \addtogroup group_uart
* @{
*/
/* UART specific functions */
#if(LED_Driver_SCB_MODE_UNCONFIG_CONST_CFG)
    void LED_Driver_UartInit(const LED_Driver_UART_INIT_STRUCT *config);
#endif /* (LED_Driver_SCB_MODE_UNCONFIG_CONST_CFG) */

#if(LED_Driver_SCB_MODE_UART_INC)
    void LED_Driver_UartSetRxAddress(uint32 address);
    void LED_Driver_UartSetRxAddressMask(uint32 addressMask);

    /* UART RX direction APIs */
    #if(LED_Driver_UART_RX_DIRECTION)
        uint32 LED_Driver_UartGetChar(void);
        uint32 LED_Driver_UartGetByte(void);

        #if !(LED_Driver_CY_SCBIP_V0 || LED_Driver_CY_SCBIP_V1)
            /* UART APIs for Flow Control */
            void LED_Driver_UartSetRtsPolarity(uint32 polarity);
            void LED_Driver_UartSetRtsFifoLevel(uint32 level);
        #endif /* !(LED_Driver_CY_SCBIP_V0 || LED_Driver_CY_SCBIP_V1) */
    #endif /* (LED_Driver_UART_RX_DIRECTION) */

    /* UART TX direction APIs */
    #if(LED_Driver_UART_TX_DIRECTION)
        /*******************************************************************************
        * Function Name: LED_Driver_UartPutChar
        ****************************************************************************//**
        *
        *  Places a byte of data in the transmit buffer to be sent at the next available
        *  bus time. This function is blocking and waits until there is a space 
        *  available to put requested data in the transmit buffer.
        *  For UART Multi Processor mode this function can send 9-bits data as well. 
        *  Use LED_Driver_UART_MP_MARK to add a mark to create an address byte.
        *
        *  \param txDataByte: the data to be transmitted.
        *
        *******************************************************************************/
        #define LED_Driver_UartPutChar(ch)    LED_Driver_SpiUartWriteTxData((uint32)(ch))
        
        void LED_Driver_UartPutString(const char8 string[]);
        void LED_Driver_UartPutCRLF(uint32 txDataByte);

        #if !(LED_Driver_CY_SCBIP_V0 || LED_Driver_CY_SCBIP_V1)
            /* UART APIs for Flow Control */
            void LED_Driver_UartEnableCts(void);
            void LED_Driver_UartDisableCts(void);
            void LED_Driver_UartSetCtsPolarity(uint32 polarity);
        #endif /* !(LED_Driver_CY_SCBIP_V0 || LED_Driver_CY_SCBIP_V1) */
    #endif /* (LED_Driver_UART_TX_DIRECTION) */
#endif /* (LED_Driver_SCB_MODE_UART_INC) */
/** @} uart */

/**
* \addtogroup group_spi_uart
* @{
*/
#if(LED_Driver_RX_DIRECTION)
    uint32 LED_Driver_SpiUartReadRxData(void);
    uint32 LED_Driver_SpiUartGetRxBufferSize(void);
    void   LED_Driver_SpiUartClearRxBuffer(void);
#endif /* (LED_Driver_RX_DIRECTION) */

/* Common APIs TX direction */
#if(LED_Driver_TX_DIRECTION)
    void   LED_Driver_SpiUartWriteTxData(uint32 txData);
    void   LED_Driver_SpiUartPutArray(const uint16 wrBuf[], uint32 count);
    uint32 LED_Driver_SpiUartGetTxBufferSize(void);
    void   LED_Driver_SpiUartClearTxBuffer(void);
#endif /* (LED_Driver_TX_DIRECTION) */
/** @} spi_uart */

CY_ISR_PROTO(LED_Driver_SPI_UART_ISR);

#if(LED_Driver_UART_RX_WAKEUP_IRQ)
    CY_ISR_PROTO(LED_Driver_UART_WAKEUP_ISR);
#endif /* (LED_Driver_UART_RX_WAKEUP_IRQ) */


/***************************************
*     Buffer Access Macro Definitions
***************************************/

#if(LED_Driver_SCB_MODE_UNCONFIG_CONST_CFG)
    /* RX direction */
    void   LED_Driver_PutWordInRxBuffer  (uint32 idx, uint32 rxDataByte);
    uint32 LED_Driver_GetWordFromRxBuffer(uint32 idx);

    /* TX direction */
    void   LED_Driver_PutWordInTxBuffer  (uint32 idx, uint32 txDataByte);
    uint32 LED_Driver_GetWordFromTxBuffer(uint32 idx);

#else
    /* RX direction */
    #if(LED_Driver_INTERNAL_RX_SW_BUFFER_CONST)
        #define LED_Driver_PutWordInRxBuffer(idx, rxDataByte) \
                do{                                                 \
                    LED_Driver_rxBufferInternal[(idx)] = ((uint16) (rxDataByte)); \
                }while(0)

        #define LED_Driver_GetWordFromRxBuffer(idx) LED_Driver_rxBufferInternal[(idx)]

    #endif /* (LED_Driver_INTERNAL_RX_SW_BUFFER_CONST) */

    /* TX direction */
    #if(LED_Driver_INTERNAL_TX_SW_BUFFER_CONST)
        #define LED_Driver_PutWordInTxBuffer(idx, txDataByte) \
                    do{                                             \
                        LED_Driver_txBufferInternal[(idx)] = ((uint16) (txDataByte)); \
                    }while(0)

        #define LED_Driver_GetWordFromTxBuffer(idx) LED_Driver_txBufferInternal[(idx)]

    #endif /* (LED_Driver_INTERNAL_TX_SW_BUFFER_CONST) */

#endif /* (LED_Driver_TX_SW_BUFFER_ENABLE) */


/***************************************
*         SPI API Constants
***************************************/

/* SPI sub mode enum */
#define LED_Driver_SPI_MODE_MOTOROLA      (0x00u)
#define LED_Driver_SPI_MODE_TI_COINCIDES  (0x01u)
#define LED_Driver_SPI_MODE_TI_PRECEDES   (0x11u)
#define LED_Driver_SPI_MODE_NATIONAL      (0x02u)
#define LED_Driver_SPI_MODE_MASK          (0x03u)
#define LED_Driver_SPI_MODE_TI_PRECEDES_MASK  (0x10u)
#define LED_Driver_SPI_MODE_NS_MICROWIRE  (LED_Driver_SPI_MODE_NATIONAL)

/* SPI phase and polarity mode enum */
#define LED_Driver_SPI_SCLK_CPHA0_CPOL0   (0x00u)
#define LED_Driver_SPI_SCLK_CPHA0_CPOL1   (0x02u)
#define LED_Driver_SPI_SCLK_CPHA1_CPOL0   (0x01u)
#define LED_Driver_SPI_SCLK_CPHA1_CPOL1   (0x03u)

/* SPI bits order enum */
#define LED_Driver_BITS_ORDER_LSB_FIRST   (0u)
#define LED_Driver_BITS_ORDER_MSB_FIRST   (1u)

/* SPI transfer separation enum */
#define LED_Driver_SPI_TRANSFER_SEPARATED     (0u)
#define LED_Driver_SPI_TRANSFER_CONTINUOUS    (1u)

/* SPI slave select constants */
#define LED_Driver_SPI_SLAVE_SELECT0    (LED_Driver_SCB__SS0_POSISTION)
#define LED_Driver_SPI_SLAVE_SELECT1    (LED_Driver_SCB__SS1_POSISTION)
#define LED_Driver_SPI_SLAVE_SELECT2    (LED_Driver_SCB__SS2_POSISTION)
#define LED_Driver_SPI_SLAVE_SELECT3    (LED_Driver_SCB__SS3_POSISTION)

/* SPI slave select polarity settings */
#define LED_Driver_SPI_SS_ACTIVE_LOW  (0u)
#define LED_Driver_SPI_SS_ACTIVE_HIGH (1u)

#define LED_Driver_INTR_SPIM_TX_RESTORE   (LED_Driver_INTR_TX_OVERFLOW)

#define LED_Driver_INTR_SPIS_TX_RESTORE     (LED_Driver_INTR_TX_OVERFLOW | \
                                                 LED_Driver_INTR_TX_UNDERFLOW)

/***************************************
*         UART API Constants
***************************************/

/* UART sub-modes enum */
#define LED_Driver_UART_MODE_STD          (0u)
#define LED_Driver_UART_MODE_SMARTCARD    (1u)
#define LED_Driver_UART_MODE_IRDA         (2u)

/* UART direction enum */
#define LED_Driver_UART_RX    (1u)
#define LED_Driver_UART_TX    (2u)
#define LED_Driver_UART_TX_RX (3u)

/* UART parity enum */
#define LED_Driver_UART_PARITY_EVEN   (0u)
#define LED_Driver_UART_PARITY_ODD    (1u)
#define LED_Driver_UART_PARITY_NONE   (2u)

/* UART stop bits enum */
#define LED_Driver_UART_STOP_BITS_1   (2u)
#define LED_Driver_UART_STOP_BITS_1_5 (3u)
#define LED_Driver_UART_STOP_BITS_2   (4u)

/* UART IrDA low power OVS enum */
#define LED_Driver_UART_IRDA_LP_OVS16     (16u)
#define LED_Driver_UART_IRDA_LP_OVS32     (32u)
#define LED_Driver_UART_IRDA_LP_OVS48     (48u)
#define LED_Driver_UART_IRDA_LP_OVS96     (96u)
#define LED_Driver_UART_IRDA_LP_OVS192    (192u)
#define LED_Driver_UART_IRDA_LP_OVS768    (768u)
#define LED_Driver_UART_IRDA_LP_OVS1536   (1536u)

/* Uart MP: mark (address) and space (data) bit definitions */
#define LED_Driver_UART_MP_MARK       (0x100u)
#define LED_Driver_UART_MP_SPACE      (0x000u)

/* UART CTS/RTS polarity settings */
#define LED_Driver_UART_CTS_ACTIVE_LOW    (0u)
#define LED_Driver_UART_CTS_ACTIVE_HIGH   (1u)
#define LED_Driver_UART_RTS_ACTIVE_LOW    (0u)
#define LED_Driver_UART_RTS_ACTIVE_HIGH   (1u)

/* Sources of RX errors */
#define LED_Driver_INTR_RX_ERR        (LED_Driver_INTR_RX_OVERFLOW    | \
                                             LED_Driver_INTR_RX_UNDERFLOW   | \
                                             LED_Driver_INTR_RX_FRAME_ERROR | \
                                             LED_Driver_INTR_RX_PARITY_ERROR)

/* Shifted INTR_RX_ERR defines ONLY for LED_Driver_UartGetByte() */
#define LED_Driver_UART_RX_OVERFLOW       (LED_Driver_INTR_RX_OVERFLOW << 8u)
#define LED_Driver_UART_RX_UNDERFLOW      (LED_Driver_INTR_RX_UNDERFLOW << 8u)
#define LED_Driver_UART_RX_FRAME_ERROR    (LED_Driver_INTR_RX_FRAME_ERROR << 8u)
#define LED_Driver_UART_RX_PARITY_ERROR   (LED_Driver_INTR_RX_PARITY_ERROR << 8u)
#define LED_Driver_UART_RX_ERROR_MASK     (LED_Driver_UART_RX_OVERFLOW    | \
                                                 LED_Driver_UART_RX_UNDERFLOW   | \
                                                 LED_Driver_UART_RX_FRAME_ERROR | \
                                                 LED_Driver_UART_RX_PARITY_ERROR)

#define LED_Driver_INTR_UART_TX_RESTORE   (LED_Driver_INTR_TX_OVERFLOW  | \
                                                 LED_Driver_INTR_TX_UART_NACK | \
                                                 LED_Driver_INTR_TX_UART_DONE | \
                                                 LED_Driver_INTR_TX_UART_ARB_LOST)


/***************************************
*     Vars with External Linkage
***************************************/

#if(LED_Driver_SCB_MODE_UNCONFIG_CONST_CFG)
    extern const LED_Driver_SPI_INIT_STRUCT  LED_Driver_configSpi;
    extern const LED_Driver_UART_INIT_STRUCT LED_Driver_configUart;
#endif /* (LED_Driver_SCB_MODE_UNCONFIG_CONST_CFG) */

#if (LED_Driver_UART_WAKE_ENABLE_CONST && LED_Driver_UART_RX_WAKEUP_IRQ)
    extern uint8 LED_Driver_skipStart;
#endif /* (LED_Driver_UART_WAKE_ENABLE_CONST && LED_Driver_UART_RX_WAKEUP_IRQ) */


/***************************************
*    Specific SPI Macro Definitions
***************************************/

#define LED_Driver_GET_SPI_INTR_SLAVE_MASK(sourceMask)  ((sourceMask) & LED_Driver_INTR_SLAVE_SPI_BUS_ERROR)
#define LED_Driver_GET_SPI_INTR_MASTER_MASK(sourceMask) ((sourceMask) & LED_Driver_INTR_MASTER_SPI_DONE)
#define LED_Driver_GET_SPI_INTR_RX_MASK(sourceMask) \
                                             ((sourceMask) & (uint32) ~LED_Driver_INTR_SLAVE_SPI_BUS_ERROR)

#define LED_Driver_GET_SPI_INTR_TX_MASK(sourceMask) \
                                             ((sourceMask) & (uint32) ~LED_Driver_INTR_MASTER_SPI_DONE)


/***************************************
*    Specific UART Macro Definitions
***************************************/

#define LED_Driver_UART_GET_CTRL_OVS_IRDA_LP(oversample) \
        ((LED_Driver_UART_IRDA_LP_OVS16   == (oversample)) ? LED_Driver_CTRL_OVS_IRDA_LP_OVS16 : \
         ((LED_Driver_UART_IRDA_LP_OVS32   == (oversample)) ? LED_Driver_CTRL_OVS_IRDA_LP_OVS32 : \
          ((LED_Driver_UART_IRDA_LP_OVS48   == (oversample)) ? LED_Driver_CTRL_OVS_IRDA_LP_OVS48 : \
           ((LED_Driver_UART_IRDA_LP_OVS96   == (oversample)) ? LED_Driver_CTRL_OVS_IRDA_LP_OVS96 : \
            ((LED_Driver_UART_IRDA_LP_OVS192  == (oversample)) ? LED_Driver_CTRL_OVS_IRDA_LP_OVS192 : \
             ((LED_Driver_UART_IRDA_LP_OVS768  == (oversample)) ? LED_Driver_CTRL_OVS_IRDA_LP_OVS768 : \
              ((LED_Driver_UART_IRDA_LP_OVS1536 == (oversample)) ? LED_Driver_CTRL_OVS_IRDA_LP_OVS1536 : \
                                                                          LED_Driver_CTRL_OVS_IRDA_LP_OVS16)))))))

#define LED_Driver_GET_UART_RX_CTRL_ENABLED(direction) ((0u != (LED_Driver_UART_RX & (direction))) ? \
                                                                     (LED_Driver_RX_CTRL_ENABLED) : (0u))

#define LED_Driver_GET_UART_TX_CTRL_ENABLED(direction) ((0u != (LED_Driver_UART_TX & (direction))) ? \
                                                                     (LED_Driver_TX_CTRL_ENABLED) : (0u))


/***************************************
*        SPI Register Settings
***************************************/

#define LED_Driver_CTRL_SPI      (LED_Driver_CTRL_MODE_SPI)
#define LED_Driver_SPI_RX_CTRL   (LED_Driver_RX_CTRL_ENABLED)
#define LED_Driver_SPI_TX_CTRL   (LED_Driver_TX_CTRL_ENABLED)


/***************************************
*       SPI Init Register Settings
***************************************/

#define LED_Driver_SPI_SS_POLARITY \
             (((uint32) LED_Driver_SPI_SS0_POLARITY << LED_Driver_SPI_SLAVE_SELECT0) | \
              ((uint32) LED_Driver_SPI_SS1_POLARITY << LED_Driver_SPI_SLAVE_SELECT1) | \
              ((uint32) LED_Driver_SPI_SS2_POLARITY << LED_Driver_SPI_SLAVE_SELECT2) | \
              ((uint32) LED_Driver_SPI_SS3_POLARITY << LED_Driver_SPI_SLAVE_SELECT3))

#if(LED_Driver_SCB_MODE_SPI_CONST_CFG)

    /* SPI Configuration */
    #define LED_Driver_SPI_DEFAULT_CTRL \
                    (LED_Driver_GET_CTRL_OVS(LED_Driver_SPI_OVS_FACTOR) | \
                     LED_Driver_GET_CTRL_BYTE_MODE (LED_Driver_SPI_BYTE_MODE_ENABLE) | \
                     LED_Driver_GET_CTRL_EC_AM_MODE(LED_Driver_SPI_WAKE_ENABLE)      | \
                     LED_Driver_CTRL_SPI)

    #define LED_Driver_SPI_DEFAULT_SPI_CTRL \
                    (LED_Driver_GET_SPI_CTRL_CONTINUOUS    (LED_Driver_SPI_TRANSFER_SEPARATION)       | \
                     LED_Driver_GET_SPI_CTRL_SELECT_PRECEDE(LED_Driver_SPI_SUB_MODE &                   \
                                                                  LED_Driver_SPI_MODE_TI_PRECEDES_MASK)     | \
                     LED_Driver_GET_SPI_CTRL_SCLK_MODE     (LED_Driver_SPI_CLOCK_MODE)                | \
                     LED_Driver_GET_SPI_CTRL_LATE_MISO_SAMPLE(LED_Driver_SPI_LATE_MISO_SAMPLE_ENABLE) | \
                     LED_Driver_GET_SPI_CTRL_SCLK_CONTINUOUS(LED_Driver_SPI_FREE_RUN_SCLK_ENABLE)     | \
                     LED_Driver_GET_SPI_CTRL_SSEL_POLARITY (LED_Driver_SPI_SS_POLARITY)               | \
                     LED_Driver_GET_SPI_CTRL_SUB_MODE      (LED_Driver_SPI_SUB_MODE)                  | \
                     LED_Driver_GET_SPI_CTRL_MASTER_MODE   (LED_Driver_SPI_MODE))

    /* RX direction */
    #define LED_Driver_SPI_DEFAULT_RX_CTRL \
                    (LED_Driver_GET_RX_CTRL_DATA_WIDTH(LED_Driver_SPI_RX_DATA_BITS_NUM)     | \
                     LED_Driver_GET_RX_CTRL_BIT_ORDER (LED_Driver_SPI_BITS_ORDER)           | \
                     LED_Driver_GET_RX_CTRL_MEDIAN    (LED_Driver_SPI_MEDIAN_FILTER_ENABLE) | \
                     LED_Driver_SPI_RX_CTRL)

    #define LED_Driver_SPI_DEFAULT_RX_FIFO_CTRL \
                    LED_Driver_GET_RX_FIFO_CTRL_TRIGGER_LEVEL(LED_Driver_SPI_RX_TRIGGER_LEVEL)

    /* TX direction */
    #define LED_Driver_SPI_DEFAULT_TX_CTRL \
                    (LED_Driver_GET_TX_CTRL_DATA_WIDTH(LED_Driver_SPI_TX_DATA_BITS_NUM) | \
                     LED_Driver_GET_TX_CTRL_BIT_ORDER (LED_Driver_SPI_BITS_ORDER)       | \
                     LED_Driver_SPI_TX_CTRL)

    #define LED_Driver_SPI_DEFAULT_TX_FIFO_CTRL \
                    LED_Driver_GET_TX_FIFO_CTRL_TRIGGER_LEVEL(LED_Driver_SPI_TX_TRIGGER_LEVEL)

    /* Interrupt sources */
    #define LED_Driver_SPI_DEFAULT_INTR_SPI_EC_MASK   (LED_Driver_NO_INTR_SOURCES)

    #define LED_Driver_SPI_DEFAULT_INTR_I2C_EC_MASK   (LED_Driver_NO_INTR_SOURCES)
    #define LED_Driver_SPI_DEFAULT_INTR_SLAVE_MASK \
                    (LED_Driver_SPI_INTR_RX_MASK & LED_Driver_INTR_SLAVE_SPI_BUS_ERROR)

    #define LED_Driver_SPI_DEFAULT_INTR_MASTER_MASK \
                    (LED_Driver_SPI_INTR_TX_MASK & LED_Driver_INTR_MASTER_SPI_DONE)

    #define LED_Driver_SPI_DEFAULT_INTR_RX_MASK \
                    (LED_Driver_SPI_INTR_RX_MASK & (uint32) ~LED_Driver_INTR_SLAVE_SPI_BUS_ERROR)

    #define LED_Driver_SPI_DEFAULT_INTR_TX_MASK \
                    (LED_Driver_SPI_INTR_TX_MASK & (uint32) ~LED_Driver_INTR_MASTER_SPI_DONE)

#endif /* (LED_Driver_SCB_MODE_SPI_CONST_CFG) */


/***************************************
*        UART Register Settings
***************************************/

#define LED_Driver_CTRL_UART      (LED_Driver_CTRL_MODE_UART)
#define LED_Driver_UART_RX_CTRL   (LED_Driver_RX_CTRL_LSB_FIRST) /* LSB for UART goes first */
#define LED_Driver_UART_TX_CTRL   (LED_Driver_TX_CTRL_LSB_FIRST) /* LSB for UART goes first */


/***************************************
*      UART Init Register Settings
***************************************/

#if(LED_Driver_SCB_MODE_UART_CONST_CFG)

    /* UART configuration */
    #if(LED_Driver_UART_MODE_IRDA == LED_Driver_UART_SUB_MODE)

        #define LED_Driver_DEFAULT_CTRL_OVS   ((0u != LED_Driver_UART_IRDA_LOW_POWER) ?              \
                                (LED_Driver_UART_GET_CTRL_OVS_IRDA_LP(LED_Driver_UART_OVS_FACTOR)) : \
                                (LED_Driver_CTRL_OVS_IRDA_OVS16))

    #else

        #define LED_Driver_DEFAULT_CTRL_OVS   LED_Driver_GET_CTRL_OVS(LED_Driver_UART_OVS_FACTOR)

    #endif /* (LED_Driver_UART_MODE_IRDA == LED_Driver_UART_SUB_MODE) */

    #define LED_Driver_UART_DEFAULT_CTRL \
                                (LED_Driver_GET_CTRL_BYTE_MODE  (LED_Driver_UART_BYTE_MODE_ENABLE)  | \
                                 LED_Driver_GET_CTRL_ADDR_ACCEPT(LED_Driver_UART_MP_ACCEPT_ADDRESS) | \
                                 LED_Driver_DEFAULT_CTRL_OVS                                              | \
                                 LED_Driver_CTRL_UART)

    #define LED_Driver_UART_DEFAULT_UART_CTRL \
                                    (LED_Driver_GET_UART_CTRL_MODE(LED_Driver_UART_SUB_MODE))

    /* RX direction */
    #define LED_Driver_UART_DEFAULT_RX_CTRL_PARITY \
                                ((LED_Driver_UART_PARITY_NONE != LED_Driver_UART_PARITY_TYPE) ?      \
                                  (LED_Driver_GET_UART_RX_CTRL_PARITY(LED_Driver_UART_PARITY_TYPE) | \
                                   LED_Driver_UART_RX_CTRL_PARITY_ENABLED) : (0u))

    #define LED_Driver_UART_DEFAULT_UART_RX_CTRL \
                    (LED_Driver_GET_UART_RX_CTRL_MODE(LED_Driver_UART_STOP_BITS_NUM)                    | \
                     LED_Driver_GET_UART_RX_CTRL_POLARITY(LED_Driver_UART_IRDA_POLARITY)                | \
                     LED_Driver_GET_UART_RX_CTRL_MP_MODE(LED_Driver_UART_MP_MODE_ENABLE)                | \
                     LED_Driver_GET_UART_RX_CTRL_DROP_ON_PARITY_ERR(LED_Driver_UART_DROP_ON_PARITY_ERR) | \
                     LED_Driver_GET_UART_RX_CTRL_DROP_ON_FRAME_ERR(LED_Driver_UART_DROP_ON_FRAME_ERR)   | \
                     LED_Driver_UART_DEFAULT_RX_CTRL_PARITY)

    #define LED_Driver_UART_DEFAULT_RX_CTRL \
                                (LED_Driver_GET_RX_CTRL_DATA_WIDTH(LED_Driver_UART_DATA_BITS_NUM)        | \
                                 LED_Driver_GET_RX_CTRL_MEDIAN    (LED_Driver_UART_MEDIAN_FILTER_ENABLE) | \
                                 LED_Driver_GET_UART_RX_CTRL_ENABLED(LED_Driver_UART_DIRECTION))

    #define LED_Driver_UART_DEFAULT_RX_FIFO_CTRL \
                                LED_Driver_GET_RX_FIFO_CTRL_TRIGGER_LEVEL(LED_Driver_UART_RX_TRIGGER_LEVEL)

    #define LED_Driver_UART_DEFAULT_RX_MATCH_REG  ((0u != LED_Driver_UART_MP_MODE_ENABLE) ?          \
                                (LED_Driver_GET_RX_MATCH_ADDR(LED_Driver_UART_MP_RX_ADDRESS) | \
                                 LED_Driver_GET_RX_MATCH_MASK(LED_Driver_UART_MP_RX_ADDRESS_MASK)) : (0u))

    /* TX direction */
    #define LED_Driver_UART_DEFAULT_TX_CTRL_PARITY (LED_Driver_UART_DEFAULT_RX_CTRL_PARITY)

    #define LED_Driver_UART_DEFAULT_UART_TX_CTRL \
                                (LED_Driver_GET_UART_TX_CTRL_MODE(LED_Driver_UART_STOP_BITS_NUM)       | \
                                 LED_Driver_GET_UART_TX_CTRL_RETRY_NACK(LED_Driver_UART_RETRY_ON_NACK) | \
                                 LED_Driver_UART_DEFAULT_TX_CTRL_PARITY)

    #define LED_Driver_UART_DEFAULT_TX_CTRL \
                                (LED_Driver_GET_TX_CTRL_DATA_WIDTH(LED_Driver_UART_DATA_BITS_NUM) | \
                                 LED_Driver_GET_UART_TX_CTRL_ENABLED(LED_Driver_UART_DIRECTION))

    #define LED_Driver_UART_DEFAULT_TX_FIFO_CTRL \
                                LED_Driver_GET_TX_FIFO_CTRL_TRIGGER_LEVEL(LED_Driver_UART_TX_TRIGGER_LEVEL)

    #define LED_Driver_UART_DEFAULT_FLOW_CTRL \
                        (LED_Driver_GET_UART_FLOW_CTRL_TRIGGER_LEVEL(LED_Driver_UART_RTS_FIFO_LEVEL) | \
                         LED_Driver_GET_UART_FLOW_CTRL_RTS_POLARITY (LED_Driver_UART_RTS_POLARITY)   | \
                         LED_Driver_GET_UART_FLOW_CTRL_CTS_POLARITY (LED_Driver_UART_CTS_POLARITY)   | \
                         LED_Driver_GET_UART_FLOW_CTRL_CTS_ENABLE   (LED_Driver_UART_CTS_ENABLE))

    /* Interrupt sources */
    #define LED_Driver_UART_DEFAULT_INTR_I2C_EC_MASK  (LED_Driver_NO_INTR_SOURCES)
    #define LED_Driver_UART_DEFAULT_INTR_SPI_EC_MASK  (LED_Driver_NO_INTR_SOURCES)
    #define LED_Driver_UART_DEFAULT_INTR_SLAVE_MASK   (LED_Driver_NO_INTR_SOURCES)
    #define LED_Driver_UART_DEFAULT_INTR_MASTER_MASK  (LED_Driver_NO_INTR_SOURCES)
    #define LED_Driver_UART_DEFAULT_INTR_RX_MASK      (LED_Driver_UART_INTR_RX_MASK)
    #define LED_Driver_UART_DEFAULT_INTR_TX_MASK      (LED_Driver_UART_INTR_TX_MASK)

#endif /* (LED_Driver_SCB_MODE_UART_CONST_CFG) */


/***************************************
* The following code is DEPRECATED and
* must not be used.
***************************************/

#define LED_Driver_SPIM_ACTIVE_SS0    (LED_Driver_SPI_SLAVE_SELECT0)
#define LED_Driver_SPIM_ACTIVE_SS1    (LED_Driver_SPI_SLAVE_SELECT1)
#define LED_Driver_SPIM_ACTIVE_SS2    (LED_Driver_SPI_SLAVE_SELECT2)
#define LED_Driver_SPIM_ACTIVE_SS3    (LED_Driver_SPI_SLAVE_SELECT3)

#endif /* CY_SCB_SPI_UART_LED_Driver_H */


/* [] END OF FILE */
