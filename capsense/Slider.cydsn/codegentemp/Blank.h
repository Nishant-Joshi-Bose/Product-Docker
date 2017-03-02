/*******************************************************************************
* File Name: Blank.h  
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

#if !defined(CY_PINS_Blank_H) /* Pins Blank_H */
#define CY_PINS_Blank_H

#include "cytypes.h"
#include "cyfitter.h"
#include "Blank_aliases.h"


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
} Blank_BACKUP_STRUCT;

/** @} structures */


/***************************************
*        Function Prototypes             
***************************************/
/**
* \addtogroup group_general
* @{
*/
uint8   Blank_Read(void);
void    Blank_Write(uint8 value);
uint8   Blank_ReadDataReg(void);
#if defined(Blank__PC) || (CY_PSOC4_4200L) 
    void    Blank_SetDriveMode(uint8 mode);
#endif
void    Blank_SetInterruptMode(uint16 position, uint16 mode);
uint8   Blank_ClearInterrupt(void);
/** @} general */

/**
* \addtogroup group_power
* @{
*/
void Blank_Sleep(void); 
void Blank_Wakeup(void);
/** @} power */


/***************************************
*           API Constants        
***************************************/
#if defined(Blank__PC) || (CY_PSOC4_4200L) 
    /* Drive Modes */
    #define Blank_DRIVE_MODE_BITS        (3)
    #define Blank_DRIVE_MODE_IND_MASK    (0xFFFFFFFFu >> (32 - Blank_DRIVE_MODE_BITS))

    /**
    * \addtogroup group_constants
    * @{
    */
        /** \addtogroup driveMode Drive mode constants
         * \brief Constants to be passed as "mode" parameter in the Blank_SetDriveMode() function.
         *  @{
         */
        #define Blank_DM_ALG_HIZ         (0x00u) /**< \brief High Impedance Analog   */
        #define Blank_DM_DIG_HIZ         (0x01u) /**< \brief High Impedance Digital  */
        #define Blank_DM_RES_UP          (0x02u) /**< \brief Resistive Pull Up       */
        #define Blank_DM_RES_DWN         (0x03u) /**< \brief Resistive Pull Down     */
        #define Blank_DM_OD_LO           (0x04u) /**< \brief Open Drain, Drives Low  */
        #define Blank_DM_OD_HI           (0x05u) /**< \brief Open Drain, Drives High */
        #define Blank_DM_STRONG          (0x06u) /**< \brief Strong Drive            */
        #define Blank_DM_RES_UPDWN       (0x07u) /**< \brief Resistive Pull Up/Down  */
        /** @} driveMode */
    /** @} group_constants */
#endif

/* Digital Port Constants */
#define Blank_MASK               Blank__MASK
#define Blank_SHIFT              Blank__SHIFT
#define Blank_WIDTH              1u

/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in Blank_SetInterruptMode() function.
     *  @{
     */
        #define Blank_INTR_NONE      ((uint16)(0x0000u)) /**< \brief Disabled             */
        #define Blank_INTR_RISING    ((uint16)(0x5555u)) /**< \brief Rising edge trigger  */
        #define Blank_INTR_FALLING   ((uint16)(0xaaaau)) /**< \brief Falling edge trigger */
        #define Blank_INTR_BOTH      ((uint16)(0xffffu)) /**< \brief Both edge trigger    */
    /** @} intrMode */
/** @} group_constants */

/* SIO LPM definition */
#if defined(Blank__SIO)
    #define Blank_SIO_LPM_MASK       (0x03u)
#endif

/* USBIO definitions */
#if !defined(Blank__PC) && (CY_PSOC4_4200L)
    #define Blank_USBIO_ENABLE               ((uint32)0x80000000u)
    #define Blank_USBIO_DISABLE              ((uint32)(~Blank_USBIO_ENABLE))
    #define Blank_USBIO_SUSPEND_SHIFT        CYFLD_USBDEVv2_USB_SUSPEND__OFFSET
    #define Blank_USBIO_SUSPEND_DEL_SHIFT    CYFLD_USBDEVv2_USB_SUSPEND_DEL__OFFSET
    #define Blank_USBIO_ENTER_SLEEP          ((uint32)((1u << Blank_USBIO_SUSPEND_SHIFT) \
                                                        | (1u << Blank_USBIO_SUSPEND_DEL_SHIFT)))
    #define Blank_USBIO_EXIT_SLEEP_PH1       ((uint32)~((uint32)(1u << Blank_USBIO_SUSPEND_SHIFT)))
    #define Blank_USBIO_EXIT_SLEEP_PH2       ((uint32)~((uint32)(1u << Blank_USBIO_SUSPEND_DEL_SHIFT)))
    #define Blank_USBIO_CR1_OFF              ((uint32)0xfffffffeu)
#endif


/***************************************
*             Registers        
***************************************/
/* Main Port Registers */
#if defined(Blank__PC)
    /* Port Configuration */
    #define Blank_PC                 (* (reg32 *) Blank__PC)
#endif
/* Pin State */
#define Blank_PS                     (* (reg32 *) Blank__PS)
/* Data Register */
#define Blank_DR                     (* (reg32 *) Blank__DR)
/* Input Buffer Disable Override */
#define Blank_INP_DIS                (* (reg32 *) Blank__PC2)

/* Interrupt configuration Registers */
#define Blank_INTCFG                 (* (reg32 *) Blank__INTCFG)
#define Blank_INTSTAT                (* (reg32 *) Blank__INTSTAT)

/* "Interrupt cause" register for Combined Port Interrupt (AllPortInt) in GSRef component */
#if defined (CYREG_GPIO_INTR_CAUSE)
    #define Blank_INTR_CAUSE         (* (reg32 *) CYREG_GPIO_INTR_CAUSE)
#endif

/* SIO register */
#if defined(Blank__SIO)
    #define Blank_SIO_REG            (* (reg32 *) Blank__SIO)
#endif /* (Blank__SIO_CFG) */

/* USBIO registers */
#if !defined(Blank__PC) && (CY_PSOC4_4200L)
    #define Blank_USB_POWER_REG       (* (reg32 *) CYREG_USBDEVv2_USB_POWER_CTRL)
    #define Blank_CR1_REG             (* (reg32 *) CYREG_USBDEVv2_CR1)
    #define Blank_USBIO_CTRL_REG      (* (reg32 *) CYREG_USBDEVv2_USB_USBIO_CTRL)
#endif    
    
    
/***************************************
* The following code is DEPRECATED and 
* must not be used in new designs.
***************************************/
/**
* \addtogroup group_deprecated
* @{
*/
#define Blank_DRIVE_MODE_SHIFT       (0x00u)
#define Blank_DRIVE_MODE_MASK        (0x07u << Blank_DRIVE_MODE_SHIFT)
/** @} deprecated */

#endif /* End Pins Blank_H */


/* [] END OF FILE */
