/*******************************************************************************
* File Name: XLAT.h  
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

#if !defined(CY_PINS_XLAT_H) /* Pins XLAT_H */
#define CY_PINS_XLAT_H

#include "cytypes.h"
#include "cyfitter.h"
#include "XLAT_aliases.h"


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
} XLAT_BACKUP_STRUCT;

/** @} structures */


/***************************************
*        Function Prototypes             
***************************************/
/**
* \addtogroup group_general
* @{
*/
uint8   XLAT_Read(void);
void    XLAT_Write(uint8 value);
uint8   XLAT_ReadDataReg(void);
#if defined(XLAT__PC) || (CY_PSOC4_4200L) 
    void    XLAT_SetDriveMode(uint8 mode);
#endif
void    XLAT_SetInterruptMode(uint16 position, uint16 mode);
uint8   XLAT_ClearInterrupt(void);
/** @} general */

/**
* \addtogroup group_power
* @{
*/
void XLAT_Sleep(void); 
void XLAT_Wakeup(void);
/** @} power */


/***************************************
*           API Constants        
***************************************/
#if defined(XLAT__PC) || (CY_PSOC4_4200L) 
    /* Drive Modes */
    #define XLAT_DRIVE_MODE_BITS        (3)
    #define XLAT_DRIVE_MODE_IND_MASK    (0xFFFFFFFFu >> (32 - XLAT_DRIVE_MODE_BITS))

    /**
    * \addtogroup group_constants
    * @{
    */
        /** \addtogroup driveMode Drive mode constants
         * \brief Constants to be passed as "mode" parameter in the XLAT_SetDriveMode() function.
         *  @{
         */
        #define XLAT_DM_ALG_HIZ         (0x00u) /**< \brief High Impedance Analog   */
        #define XLAT_DM_DIG_HIZ         (0x01u) /**< \brief High Impedance Digital  */
        #define XLAT_DM_RES_UP          (0x02u) /**< \brief Resistive Pull Up       */
        #define XLAT_DM_RES_DWN         (0x03u) /**< \brief Resistive Pull Down     */
        #define XLAT_DM_OD_LO           (0x04u) /**< \brief Open Drain, Drives Low  */
        #define XLAT_DM_OD_HI           (0x05u) /**< \brief Open Drain, Drives High */
        #define XLAT_DM_STRONG          (0x06u) /**< \brief Strong Drive            */
        #define XLAT_DM_RES_UPDWN       (0x07u) /**< \brief Resistive Pull Up/Down  */
        /** @} driveMode */
    /** @} group_constants */
#endif

/* Digital Port Constants */
#define XLAT_MASK               XLAT__MASK
#define XLAT_SHIFT              XLAT__SHIFT
#define XLAT_WIDTH              1u

/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in XLAT_SetInterruptMode() function.
     *  @{
     */
        #define XLAT_INTR_NONE      ((uint16)(0x0000u)) /**< \brief Disabled             */
        #define XLAT_INTR_RISING    ((uint16)(0x5555u)) /**< \brief Rising edge trigger  */
        #define XLAT_INTR_FALLING   ((uint16)(0xaaaau)) /**< \brief Falling edge trigger */
        #define XLAT_INTR_BOTH      ((uint16)(0xffffu)) /**< \brief Both edge trigger    */
    /** @} intrMode */
/** @} group_constants */

/* SIO LPM definition */
#if defined(XLAT__SIO)
    #define XLAT_SIO_LPM_MASK       (0x03u)
#endif

/* USBIO definitions */
#if !defined(XLAT__PC) && (CY_PSOC4_4200L)
    #define XLAT_USBIO_ENABLE               ((uint32)0x80000000u)
    #define XLAT_USBIO_DISABLE              ((uint32)(~XLAT_USBIO_ENABLE))
    #define XLAT_USBIO_SUSPEND_SHIFT        CYFLD_USBDEVv2_USB_SUSPEND__OFFSET
    #define XLAT_USBIO_SUSPEND_DEL_SHIFT    CYFLD_USBDEVv2_USB_SUSPEND_DEL__OFFSET
    #define XLAT_USBIO_ENTER_SLEEP          ((uint32)((1u << XLAT_USBIO_SUSPEND_SHIFT) \
                                                        | (1u << XLAT_USBIO_SUSPEND_DEL_SHIFT)))
    #define XLAT_USBIO_EXIT_SLEEP_PH1       ((uint32)~((uint32)(1u << XLAT_USBIO_SUSPEND_SHIFT)))
    #define XLAT_USBIO_EXIT_SLEEP_PH2       ((uint32)~((uint32)(1u << XLAT_USBIO_SUSPEND_DEL_SHIFT)))
    #define XLAT_USBIO_CR1_OFF              ((uint32)0xfffffffeu)
#endif


/***************************************
*             Registers        
***************************************/
/* Main Port Registers */
#if defined(XLAT__PC)
    /* Port Configuration */
    #define XLAT_PC                 (* (reg32 *) XLAT__PC)
#endif
/* Pin State */
#define XLAT_PS                     (* (reg32 *) XLAT__PS)
/* Data Register */
#define XLAT_DR                     (* (reg32 *) XLAT__DR)
/* Input Buffer Disable Override */
#define XLAT_INP_DIS                (* (reg32 *) XLAT__PC2)

/* Interrupt configuration Registers */
#define XLAT_INTCFG                 (* (reg32 *) XLAT__INTCFG)
#define XLAT_INTSTAT                (* (reg32 *) XLAT__INTSTAT)

/* "Interrupt cause" register for Combined Port Interrupt (AllPortInt) in GSRef component */
#if defined (CYREG_GPIO_INTR_CAUSE)
    #define XLAT_INTR_CAUSE         (* (reg32 *) CYREG_GPIO_INTR_CAUSE)
#endif

/* SIO register */
#if defined(XLAT__SIO)
    #define XLAT_SIO_REG            (* (reg32 *) XLAT__SIO)
#endif /* (XLAT__SIO_CFG) */

/* USBIO registers */
#if !defined(XLAT__PC) && (CY_PSOC4_4200L)
    #define XLAT_USB_POWER_REG       (* (reg32 *) CYREG_USBDEVv2_USB_POWER_CTRL)
    #define XLAT_CR1_REG             (* (reg32 *) CYREG_USBDEVv2_CR1)
    #define XLAT_USBIO_CTRL_REG      (* (reg32 *) CYREG_USBDEVv2_USB_USBIO_CTRL)
#endif    
    
    
/***************************************
* The following code is DEPRECATED and 
* must not be used in new designs.
***************************************/
/**
* \addtogroup group_deprecated
* @{
*/
#define XLAT_DRIVE_MODE_SHIFT       (0x00u)
#define XLAT_DRIVE_MODE_MASK        (0x07u << XLAT_DRIVE_MODE_SHIFT)
/** @} deprecated */

#endif /* End Pins XLAT_H */


/* [] END OF FILE */
