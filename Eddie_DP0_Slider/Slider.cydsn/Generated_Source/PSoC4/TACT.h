/*******************************************************************************
* File Name: TACT.h  
* Version 2.20
*
* Description:
*  This file contains Pin function prototypes and register defines
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_PINS_TACT_H) /* Pins TACT_H */
#define CY_PINS_TACT_H

#include "cytypes.h"
#include "cyfitter.h"
#include "TACT_aliases.h"


/***************************************
*     Data Struct Definitions
***************************************/

/**
* \addtogroup group_structures
* @{
*/
    
/* Structure for sleep mode support */
typedef struct
{
    uint32 pcState; /**< State of the port control register */
    uint32 sioState; /**< State of the SIO configuration */
    uint32 usbState; /**< State of the USBIO regulator */
} TACT_BACKUP_STRUCT;

/** @} structures */


/***************************************
*        Function Prototypes             
***************************************/
/**
* \addtogroup group_general
* @{
*/
uint8   TACT_Read(void);
void    TACT_Write(uint8 value);
uint8   TACT_ReadDataReg(void);
#if defined(TACT__PC) || (CY_PSOC4_4200L) 
    void    TACT_SetDriveMode(uint8 mode);
#endif
void    TACT_SetInterruptMode(uint16 position, uint16 mode);
uint8   TACT_ClearInterrupt(void);
/** @} general */

/**
* \addtogroup group_power
* @{
*/
void TACT_Sleep(void); 
void TACT_Wakeup(void);
/** @} power */


/***************************************
*           API Constants        
***************************************/
#if defined(TACT__PC) || (CY_PSOC4_4200L) 
    /* Drive Modes */
    #define TACT_DRIVE_MODE_BITS        (3)
    #define TACT_DRIVE_MODE_IND_MASK    (0xFFFFFFFFu >> (32 - TACT_DRIVE_MODE_BITS))

    /**
    * \addtogroup group_constants
    * @{
    */
        /** \addtogroup driveMode Drive mode constants
         * \brief Constants to be passed as "mode" parameter in the TACT_SetDriveMode() function.
         *  @{
         */
        #define TACT_DM_ALG_HIZ         (0x00u) /**< \brief High Impedance Analog   */
        #define TACT_DM_DIG_HIZ         (0x01u) /**< \brief High Impedance Digital  */
        #define TACT_DM_RES_UP          (0x02u) /**< \brief Resistive Pull Up       */
        #define TACT_DM_RES_DWN         (0x03u) /**< \brief Resistive Pull Down     */
        #define TACT_DM_OD_LO           (0x04u) /**< \brief Open Drain, Drives Low  */
        #define TACT_DM_OD_HI           (0x05u) /**< \brief Open Drain, Drives High */
        #define TACT_DM_STRONG          (0x06u) /**< \brief Strong Drive            */
        #define TACT_DM_RES_UPDWN       (0x07u) /**< \brief Resistive Pull Up/Down  */
        /** @} driveMode */
    /** @} group_constants */
#endif

/* Digital Port Constants */
#define TACT_MASK               TACT__MASK
#define TACT_SHIFT              TACT__SHIFT
#define TACT_WIDTH              1u

/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in TACT_SetInterruptMode() function.
     *  @{
     */
        #define TACT_INTR_NONE      ((uint16)(0x0000u)) /**< \brief Disabled             */
        #define TACT_INTR_RISING    ((uint16)(0x5555u)) /**< \brief Rising edge trigger  */
        #define TACT_INTR_FALLING   ((uint16)(0xaaaau)) /**< \brief Falling edge trigger */
        #define TACT_INTR_BOTH      ((uint16)(0xffffu)) /**< \brief Both edge trigger    */
    /** @} intrMode */
/** @} group_constants */

/* SIO LPM definition */
#if defined(TACT__SIO)
    #define TACT_SIO_LPM_MASK       (0x03u)
#endif

/* USBIO definitions */
#if !defined(TACT__PC) && (CY_PSOC4_4200L)
    #define TACT_USBIO_ENABLE               ((uint32)0x80000000u)
    #define TACT_USBIO_DISABLE              ((uint32)(~TACT_USBIO_ENABLE))
    #define TACT_USBIO_SUSPEND_SHIFT        CYFLD_USBDEVv2_USB_SUSPEND__OFFSET
    #define TACT_USBIO_SUSPEND_DEL_SHIFT    CYFLD_USBDEVv2_USB_SUSPEND_DEL__OFFSET
    #define TACT_USBIO_ENTER_SLEEP          ((uint32)((1u << TACT_USBIO_SUSPEND_SHIFT) \
                                                        | (1u << TACT_USBIO_SUSPEND_DEL_SHIFT)))
    #define TACT_USBIO_EXIT_SLEEP_PH1       ((uint32)~((uint32)(1u << TACT_USBIO_SUSPEND_SHIFT)))
    #define TACT_USBIO_EXIT_SLEEP_PH2       ((uint32)~((uint32)(1u << TACT_USBIO_SUSPEND_DEL_SHIFT)))
    #define TACT_USBIO_CR1_OFF              ((uint32)0xfffffffeu)
#endif


/***************************************
*             Registers        
***************************************/
/* Main Port Registers */
#if defined(TACT__PC)
    /* Port Configuration */
    #define TACT_PC                 (* (reg32 *) TACT__PC)
#endif
/* Pin State */
#define TACT_PS                     (* (reg32 *) TACT__PS)
/* Data Register */
#define TACT_DR                     (* (reg32 *) TACT__DR)
/* Input Buffer Disable Override */
#define TACT_INP_DIS                (* (reg32 *) TACT__PC2)

/* Interrupt configuration Registers */
#define TACT_INTCFG                 (* (reg32 *) TACT__INTCFG)
#define TACT_INTSTAT                (* (reg32 *) TACT__INTSTAT)

/* "Interrupt cause" register for Combined Port Interrupt (AllPortInt) in GSRef component */
#if defined (CYREG_GPIO_INTR_CAUSE)
    #define TACT_INTR_CAUSE         (* (reg32 *) CYREG_GPIO_INTR_CAUSE)
#endif

/* SIO register */
#if defined(TACT__SIO)
    #define TACT_SIO_REG            (* (reg32 *) TACT__SIO)
#endif /* (TACT__SIO_CFG) */

/* USBIO registers */
#if !defined(TACT__PC) && (CY_PSOC4_4200L)
    #define TACT_USB_POWER_REG       (* (reg32 *) CYREG_USBDEVv2_USB_POWER_CTRL)
    #define TACT_CR1_REG             (* (reg32 *) CYREG_USBDEVv2_CR1)
    #define TACT_USBIO_CTRL_REG      (* (reg32 *) CYREG_USBDEVv2_USB_USBIO_CTRL)
#endif    
    
    
/***************************************
* The following code is DEPRECATED and 
* must not be used in new designs.
***************************************/
/**
* \addtogroup group_deprecated
* @{
*/
#define TACT_DRIVE_MODE_SHIFT       (0x00u)
#define TACT_DRIVE_MODE_MASK        (0x07u << TACT_DRIVE_MODE_SHIFT)
/** @} deprecated */

#endif /* End Pins TACT_H */


/* [] END OF FILE */
