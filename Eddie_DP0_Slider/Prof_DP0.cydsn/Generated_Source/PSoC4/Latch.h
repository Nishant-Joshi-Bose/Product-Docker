/*******************************************************************************
* File Name: Latch.h  
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

#if !defined(CY_PINS_Latch_H) /* Pins Latch_H */
#define CY_PINS_Latch_H

#include "cytypes.h"
#include "cyfitter.h"
#include "Latch_aliases.h"


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
} Latch_BACKUP_STRUCT;

/** @} structures */


/***************************************
*        Function Prototypes             
***************************************/
/**
* \addtogroup group_general
* @{
*/
uint8   Latch_Read(void);
void    Latch_Write(uint8 value);
uint8   Latch_ReadDataReg(void);
#if defined(Latch__PC) || (CY_PSOC4_4200L) 
    void    Latch_SetDriveMode(uint8 mode);
#endif
void    Latch_SetInterruptMode(uint16 position, uint16 mode);
uint8   Latch_ClearInterrupt(void);
/** @} general */

/**
* \addtogroup group_power
* @{
*/
void Latch_Sleep(void); 
void Latch_Wakeup(void);
/** @} power */


/***************************************
*           API Constants        
***************************************/
#if defined(Latch__PC) || (CY_PSOC4_4200L) 
    /* Drive Modes */
    #define Latch_DRIVE_MODE_BITS        (3)
    #define Latch_DRIVE_MODE_IND_MASK    (0xFFFFFFFFu >> (32 - Latch_DRIVE_MODE_BITS))

    /**
    * \addtogroup group_constants
    * @{
    */
        /** \addtogroup driveMode Drive mode constants
         * \brief Constants to be passed as "mode" parameter in the Latch_SetDriveMode() function.
         *  @{
         */
        #define Latch_DM_ALG_HIZ         (0x00u) /**< \brief High Impedance Analog   */
        #define Latch_DM_DIG_HIZ         (0x01u) /**< \brief High Impedance Digital  */
        #define Latch_DM_RES_UP          (0x02u) /**< \brief Resistive Pull Up       */
        #define Latch_DM_RES_DWN         (0x03u) /**< \brief Resistive Pull Down     */
        #define Latch_DM_OD_LO           (0x04u) /**< \brief Open Drain, Drives Low  */
        #define Latch_DM_OD_HI           (0x05u) /**< \brief Open Drain, Drives High */
        #define Latch_DM_STRONG          (0x06u) /**< \brief Strong Drive            */
        #define Latch_DM_RES_UPDWN       (0x07u) /**< \brief Resistive Pull Up/Down  */
        /** @} driveMode */
    /** @} group_constants */
#endif

/* Digital Port Constants */
#define Latch_MASK               Latch__MASK
#define Latch_SHIFT              Latch__SHIFT
#define Latch_WIDTH              1u

/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in Latch_SetInterruptMode() function.
     *  @{
     */
        #define Latch_INTR_NONE      ((uint16)(0x0000u)) /**< \brief Disabled             */
        #define Latch_INTR_RISING    ((uint16)(0x5555u)) /**< \brief Rising edge trigger  */
        #define Latch_INTR_FALLING   ((uint16)(0xaaaau)) /**< \brief Falling edge trigger */
        #define Latch_INTR_BOTH      ((uint16)(0xffffu)) /**< \brief Both edge trigger    */
    /** @} intrMode */
/** @} group_constants */

/* SIO LPM definition */
#if defined(Latch__SIO)
    #define Latch_SIO_LPM_MASK       (0x03u)
#endif

/* USBIO definitions */
#if !defined(Latch__PC) && (CY_PSOC4_4200L)
    #define Latch_USBIO_ENABLE               ((uint32)0x80000000u)
    #define Latch_USBIO_DISABLE              ((uint32)(~Latch_USBIO_ENABLE))
    #define Latch_USBIO_SUSPEND_SHIFT        CYFLD_USBDEVv2_USB_SUSPEND__OFFSET
    #define Latch_USBIO_SUSPEND_DEL_SHIFT    CYFLD_USBDEVv2_USB_SUSPEND_DEL__OFFSET
    #define Latch_USBIO_ENTER_SLEEP          ((uint32)((1u << Latch_USBIO_SUSPEND_SHIFT) \
                                                        | (1u << Latch_USBIO_SUSPEND_DEL_SHIFT)))
    #define Latch_USBIO_EXIT_SLEEP_PH1       ((uint32)~((uint32)(1u << Latch_USBIO_SUSPEND_SHIFT)))
    #define Latch_USBIO_EXIT_SLEEP_PH2       ((uint32)~((uint32)(1u << Latch_USBIO_SUSPEND_DEL_SHIFT)))
    #define Latch_USBIO_CR1_OFF              ((uint32)0xfffffffeu)
#endif


/***************************************
*             Registers        
***************************************/
/* Main Port Registers */
#if defined(Latch__PC)
    /* Port Configuration */
    #define Latch_PC                 (* (reg32 *) Latch__PC)
#endif
/* Pin State */
#define Latch_PS                     (* (reg32 *) Latch__PS)
/* Data Register */
#define Latch_DR                     (* (reg32 *) Latch__DR)
/* Input Buffer Disable Override */
#define Latch_INP_DIS                (* (reg32 *) Latch__PC2)

/* Interrupt configuration Registers */
#define Latch_INTCFG                 (* (reg32 *) Latch__INTCFG)
#define Latch_INTSTAT                (* (reg32 *) Latch__INTSTAT)

/* "Interrupt cause" register for Combined Port Interrupt (AllPortInt) in GSRef component */
#if defined (CYREG_GPIO_INTR_CAUSE)
    #define Latch_INTR_CAUSE         (* (reg32 *) CYREG_GPIO_INTR_CAUSE)
#endif

/* SIO register */
#if defined(Latch__SIO)
    #define Latch_SIO_REG            (* (reg32 *) Latch__SIO)
#endif /* (Latch__SIO_CFG) */

/* USBIO registers */
#if !defined(Latch__PC) && (CY_PSOC4_4200L)
    #define Latch_USB_POWER_REG       (* (reg32 *) CYREG_USBDEVv2_USB_POWER_CTRL)
    #define Latch_CR1_REG             (* (reg32 *) CYREG_USBDEVv2_CR1)
    #define Latch_USBIO_CTRL_REG      (* (reg32 *) CYREG_USBDEVv2_USB_USBIO_CTRL)
#endif    
    
    
/***************************************
* The following code is DEPRECATED and 
* must not be used in new designs.
***************************************/
/**
* \addtogroup group_deprecated
* @{
*/
#define Latch_DRIVE_MODE_SHIFT       (0x00u)
#define Latch_DRIVE_MODE_MASK        (0x07u << Latch_DRIVE_MODE_SHIFT)
/** @} deprecated */

#endif /* End Pins Latch_H */


/* [] END OF FILE */
