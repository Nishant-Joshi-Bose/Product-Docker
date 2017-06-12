/*******************************************************************************
* File Name: cap_int.h  
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

#if !defined(CY_PINS_cap_int_H) /* Pins cap_int_H */
#define CY_PINS_cap_int_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cap_int_aliases.h"


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
} cap_int_BACKUP_STRUCT;

/** @} structures */


/***************************************
*        Function Prototypes             
***************************************/
/**
* \addtogroup group_general
* @{
*/
uint8   cap_int_Read(void);
void    cap_int_Write(uint8 value);
uint8   cap_int_ReadDataReg(void);
#if defined(cap_int__PC) || (CY_PSOC4_4200L) 
    void    cap_int_SetDriveMode(uint8 mode);
#endif
void    cap_int_SetInterruptMode(uint16 position, uint16 mode);
uint8   cap_int_ClearInterrupt(void);
/** @} general */

/**
* \addtogroup group_power
* @{
*/
void cap_int_Sleep(void); 
void cap_int_Wakeup(void);
/** @} power */


/***************************************
*           API Constants        
***************************************/
#if defined(cap_int__PC) || (CY_PSOC4_4200L) 
    /* Drive Modes */
    #define cap_int_DRIVE_MODE_BITS        (3)
    #define cap_int_DRIVE_MODE_IND_MASK    (0xFFFFFFFFu >> (32 - cap_int_DRIVE_MODE_BITS))

    /**
    * \addtogroup group_constants
    * @{
    */
        /** \addtogroup driveMode Drive mode constants
         * \brief Constants to be passed as "mode" parameter in the cap_int_SetDriveMode() function.
         *  @{
         */
        #define cap_int_DM_ALG_HIZ         (0x00u) /**< \brief High Impedance Analog   */
        #define cap_int_DM_DIG_HIZ         (0x01u) /**< \brief High Impedance Digital  */
        #define cap_int_DM_RES_UP          (0x02u) /**< \brief Resistive Pull Up       */
        #define cap_int_DM_RES_DWN         (0x03u) /**< \brief Resistive Pull Down     */
        #define cap_int_DM_OD_LO           (0x04u) /**< \brief Open Drain, Drives Low  */
        #define cap_int_DM_OD_HI           (0x05u) /**< \brief Open Drain, Drives High */
        #define cap_int_DM_STRONG          (0x06u) /**< \brief Strong Drive            */
        #define cap_int_DM_RES_UPDWN       (0x07u) /**< \brief Resistive Pull Up/Down  */
        /** @} driveMode */
    /** @} group_constants */
#endif

/* Digital Port Constants */
#define cap_int_MASK               cap_int__MASK
#define cap_int_SHIFT              cap_int__SHIFT
#define cap_int_WIDTH              1u

/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in cap_int_SetInterruptMode() function.
     *  @{
     */
        #define cap_int_INTR_NONE      ((uint16)(0x0000u)) /**< \brief Disabled             */
        #define cap_int_INTR_RISING    ((uint16)(0x5555u)) /**< \brief Rising edge trigger  */
        #define cap_int_INTR_FALLING   ((uint16)(0xaaaau)) /**< \brief Falling edge trigger */
        #define cap_int_INTR_BOTH      ((uint16)(0xffffu)) /**< \brief Both edge trigger    */
    /** @} intrMode */
/** @} group_constants */

/* SIO LPM definition */
#if defined(cap_int__SIO)
    #define cap_int_SIO_LPM_MASK       (0x03u)
#endif

/* USBIO definitions */
#if !defined(cap_int__PC) && (CY_PSOC4_4200L)
    #define cap_int_USBIO_ENABLE               ((uint32)0x80000000u)
    #define cap_int_USBIO_DISABLE              ((uint32)(~cap_int_USBIO_ENABLE))
    #define cap_int_USBIO_SUSPEND_SHIFT        CYFLD_USBDEVv2_USB_SUSPEND__OFFSET
    #define cap_int_USBIO_SUSPEND_DEL_SHIFT    CYFLD_USBDEVv2_USB_SUSPEND_DEL__OFFSET
    #define cap_int_USBIO_ENTER_SLEEP          ((uint32)((1u << cap_int_USBIO_SUSPEND_SHIFT) \
                                                        | (1u << cap_int_USBIO_SUSPEND_DEL_SHIFT)))
    #define cap_int_USBIO_EXIT_SLEEP_PH1       ((uint32)~((uint32)(1u << cap_int_USBIO_SUSPEND_SHIFT)))
    #define cap_int_USBIO_EXIT_SLEEP_PH2       ((uint32)~((uint32)(1u << cap_int_USBIO_SUSPEND_DEL_SHIFT)))
    #define cap_int_USBIO_CR1_OFF              ((uint32)0xfffffffeu)
#endif


/***************************************
*             Registers        
***************************************/
/* Main Port Registers */
#if defined(cap_int__PC)
    /* Port Configuration */
    #define cap_int_PC                 (* (reg32 *) cap_int__PC)
#endif
/* Pin State */
#define cap_int_PS                     (* (reg32 *) cap_int__PS)
/* Data Register */
#define cap_int_DR                     (* (reg32 *) cap_int__DR)
/* Input Buffer Disable Override */
#define cap_int_INP_DIS                (* (reg32 *) cap_int__PC2)

/* Interrupt configuration Registers */
#define cap_int_INTCFG                 (* (reg32 *) cap_int__INTCFG)
#define cap_int_INTSTAT                (* (reg32 *) cap_int__INTSTAT)

/* "Interrupt cause" register for Combined Port Interrupt (AllPortInt) in GSRef component */
#if defined (CYREG_GPIO_INTR_CAUSE)
    #define cap_int_INTR_CAUSE         (* (reg32 *) CYREG_GPIO_INTR_CAUSE)
#endif

/* SIO register */
#if defined(cap_int__SIO)
    #define cap_int_SIO_REG            (* (reg32 *) cap_int__SIO)
#endif /* (cap_int__SIO_CFG) */

/* USBIO registers */
#if !defined(cap_int__PC) && (CY_PSOC4_4200L)
    #define cap_int_USB_POWER_REG       (* (reg32 *) CYREG_USBDEVv2_USB_POWER_CTRL)
    #define cap_int_CR1_REG             (* (reg32 *) CYREG_USBDEVv2_CR1)
    #define cap_int_USBIO_CTRL_REG      (* (reg32 *) CYREG_USBDEVv2_USB_USBIO_CTRL)
#endif    
    
    
/***************************************
* The following code is DEPRECATED and 
* must not be used in new designs.
***************************************/
/**
* \addtogroup group_deprecated
* @{
*/
#define cap_int_DRIVE_MODE_SHIFT       (0x00u)
#define cap_int_DRIVE_MODE_MASK        (0x07u << cap_int_DRIVE_MODE_SHIFT)
/** @} deprecated */

#endif /* End Pins cap_int_H */


/* [] END OF FILE */
