/*******************************************************************************
* File Name: CAPINT.h  
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

#if !defined(CY_PINS_CAPINT_H) /* Pins CAPINT_H */
#define CY_PINS_CAPINT_H

#include "cytypes.h"
#include "cyfitter.h"
#include "CAPINT_aliases.h"


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
} CAPINT_BACKUP_STRUCT;

/** @} structures */


/***************************************
*        Function Prototypes             
***************************************/
/**
* \addtogroup group_general
* @{
*/
uint8   CAPINT_Read(void);
void    CAPINT_Write(uint8 value);
uint8   CAPINT_ReadDataReg(void);
#if defined(CAPINT__PC) || (CY_PSOC4_4200L) 
    void    CAPINT_SetDriveMode(uint8 mode);
#endif
void    CAPINT_SetInterruptMode(uint16 position, uint16 mode);
uint8   CAPINT_ClearInterrupt(void);
/** @} general */

/**
* \addtogroup group_power
* @{
*/
void CAPINT_Sleep(void); 
void CAPINT_Wakeup(void);
/** @} power */


/***************************************
*           API Constants        
***************************************/
#if defined(CAPINT__PC) || (CY_PSOC4_4200L) 
    /* Drive Modes */
    #define CAPINT_DRIVE_MODE_BITS        (3)
    #define CAPINT_DRIVE_MODE_IND_MASK    (0xFFFFFFFFu >> (32 - CAPINT_DRIVE_MODE_BITS))

    /**
    * \addtogroup group_constants
    * @{
    */
        /** \addtogroup driveMode Drive mode constants
         * \brief Constants to be passed as "mode" parameter in the CAPINT_SetDriveMode() function.
         *  @{
         */
        #define CAPINT_DM_ALG_HIZ         (0x00u) /**< \brief High Impedance Analog   */
        #define CAPINT_DM_DIG_HIZ         (0x01u) /**< \brief High Impedance Digital  */
        #define CAPINT_DM_RES_UP          (0x02u) /**< \brief Resistive Pull Up       */
        #define CAPINT_DM_RES_DWN         (0x03u) /**< \brief Resistive Pull Down     */
        #define CAPINT_DM_OD_LO           (0x04u) /**< \brief Open Drain, Drives Low  */
        #define CAPINT_DM_OD_HI           (0x05u) /**< \brief Open Drain, Drives High */
        #define CAPINT_DM_STRONG          (0x06u) /**< \brief Strong Drive            */
        #define CAPINT_DM_RES_UPDWN       (0x07u) /**< \brief Resistive Pull Up/Down  */
        /** @} driveMode */
    /** @} group_constants */
#endif

/* Digital Port Constants */
#define CAPINT_MASK               CAPINT__MASK
#define CAPINT_SHIFT              CAPINT__SHIFT
#define CAPINT_WIDTH              1u

/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in CAPINT_SetInterruptMode() function.
     *  @{
     */
        #define CAPINT_INTR_NONE      ((uint16)(0x0000u)) /**< \brief Disabled             */
        #define CAPINT_INTR_RISING    ((uint16)(0x5555u)) /**< \brief Rising edge trigger  */
        #define CAPINT_INTR_FALLING   ((uint16)(0xaaaau)) /**< \brief Falling edge trigger */
        #define CAPINT_INTR_BOTH      ((uint16)(0xffffu)) /**< \brief Both edge trigger    */
    /** @} intrMode */
/** @} group_constants */

/* SIO LPM definition */
#if defined(CAPINT__SIO)
    #define CAPINT_SIO_LPM_MASK       (0x03u)
#endif

/* USBIO definitions */
#if !defined(CAPINT__PC) && (CY_PSOC4_4200L)
    #define CAPINT_USBIO_ENABLE               ((uint32)0x80000000u)
    #define CAPINT_USBIO_DISABLE              ((uint32)(~CAPINT_USBIO_ENABLE))
    #define CAPINT_USBIO_SUSPEND_SHIFT        CYFLD_USBDEVv2_USB_SUSPEND__OFFSET
    #define CAPINT_USBIO_SUSPEND_DEL_SHIFT    CYFLD_USBDEVv2_USB_SUSPEND_DEL__OFFSET
    #define CAPINT_USBIO_ENTER_SLEEP          ((uint32)((1u << CAPINT_USBIO_SUSPEND_SHIFT) \
                                                        | (1u << CAPINT_USBIO_SUSPEND_DEL_SHIFT)))
    #define CAPINT_USBIO_EXIT_SLEEP_PH1       ((uint32)~((uint32)(1u << CAPINT_USBIO_SUSPEND_SHIFT)))
    #define CAPINT_USBIO_EXIT_SLEEP_PH2       ((uint32)~((uint32)(1u << CAPINT_USBIO_SUSPEND_DEL_SHIFT)))
    #define CAPINT_USBIO_CR1_OFF              ((uint32)0xfffffffeu)
#endif


/***************************************
*             Registers        
***************************************/
/* Main Port Registers */
#if defined(CAPINT__PC)
    /* Port Configuration */
    #define CAPINT_PC                 (* (reg32 *) CAPINT__PC)
#endif
/* Pin State */
#define CAPINT_PS                     (* (reg32 *) CAPINT__PS)
/* Data Register */
#define CAPINT_DR                     (* (reg32 *) CAPINT__DR)
/* Input Buffer Disable Override */
#define CAPINT_INP_DIS                (* (reg32 *) CAPINT__PC2)

/* Interrupt configuration Registers */
#define CAPINT_INTCFG                 (* (reg32 *) CAPINT__INTCFG)
#define CAPINT_INTSTAT                (* (reg32 *) CAPINT__INTSTAT)

/* "Interrupt cause" register for Combined Port Interrupt (AllPortInt) in GSRef component */
#if defined (CYREG_GPIO_INTR_CAUSE)
    #define CAPINT_INTR_CAUSE         (* (reg32 *) CYREG_GPIO_INTR_CAUSE)
#endif

/* SIO register */
#if defined(CAPINT__SIO)
    #define CAPINT_SIO_REG            (* (reg32 *) CAPINT__SIO)
#endif /* (CAPINT__SIO_CFG) */

/* USBIO registers */
#if !defined(CAPINT__PC) && (CY_PSOC4_4200L)
    #define CAPINT_USB_POWER_REG       (* (reg32 *) CYREG_USBDEVv2_USB_POWER_CTRL)
    #define CAPINT_CR1_REG             (* (reg32 *) CYREG_USBDEVv2_CR1)
    #define CAPINT_USBIO_CTRL_REG      (* (reg32 *) CYREG_USBDEVv2_USB_USBIO_CTRL)
#endif    
    
    
/***************************************
* The following code is DEPRECATED and 
* must not be used in new designs.
***************************************/
/**
* \addtogroup group_deprecated
* @{
*/
#define CAPINT_DRIVE_MODE_SHIFT       (0x00u)
#define CAPINT_DRIVE_MODE_MASK        (0x07u << CAPINT_DRIVE_MODE_SHIFT)
/** @} deprecated */

#endif /* End Pins CAPINT_H */


/* [] END OF FILE */
