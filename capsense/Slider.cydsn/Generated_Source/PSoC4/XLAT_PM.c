/*******************************************************************************
* File Name: XLAT.c  
* Version 2.20
*
* Description:
*  This file contains APIs to set up the Pins component for low power modes.
*
* Note:
*
********************************************************************************
* Copyright 2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#include "cytypes.h"
#include "XLAT.h"

static XLAT_BACKUP_STRUCT  XLAT_backup = {0u, 0u, 0u};


/*******************************************************************************
* Function Name: XLAT_Sleep
****************************************************************************//**
*
* \brief Stores the pin configuration and prepares the pin for entering chip 
*  deep-sleep/hibernate modes. This function must be called for SIO and USBIO
*  pins. It is not essential if using GPIO or GPIO_OVT pins.
*
* <b>Note</b> This function is available in PSoC 4 only.
*
* \return 
*  None 
*  
* \sideeffect
*  For SIO pins, this function configures the pin input threshold to CMOS and
*  drive level to Vddio. This is needed for SIO pins when in device 
*  deep-sleep/hibernate modes.
*
* \funcusage
*  \snippet XLAT_SUT.c usage_XLAT_Sleep_Wakeup
*******************************************************************************/
void XLAT_Sleep(void)
{
    #if defined(XLAT__PC)
        XLAT_backup.pcState = XLAT_PC;
    #else
        #if (CY_PSOC4_4200L)
            /* Save the regulator state and put the PHY into suspend mode */
            XLAT_backup.usbState = XLAT_CR1_REG;
            XLAT_USB_POWER_REG |= XLAT_USBIO_ENTER_SLEEP;
            XLAT_CR1_REG &= XLAT_USBIO_CR1_OFF;
        #endif
    #endif
    #if defined(CYIPBLOCK_m0s8ioss_VERSION) && defined(XLAT__SIO)
        XLAT_backup.sioState = XLAT_SIO_REG;
        /* SIO requires unregulated output buffer and single ended input buffer */
        XLAT_SIO_REG &= (uint32)(~XLAT_SIO_LPM_MASK);
    #endif  
}


/*******************************************************************************
* Function Name: XLAT_Wakeup
****************************************************************************//**
*
* \brief Restores the pin configuration that was saved during Pin_Sleep().
*
* For USBIO pins, the wakeup is only triggered for falling edge interrupts.
*
* <b>Note</b> This function is available in PSoC 4 only.
*
* \return 
*  None
*  
* \funcusage
*  Refer to XLAT_Sleep() for an example usage.
*******************************************************************************/
void XLAT_Wakeup(void)
{
    #if defined(XLAT__PC)
        XLAT_PC = XLAT_backup.pcState;
    #else
        #if (CY_PSOC4_4200L)
            /* Restore the regulator state and come out of suspend mode */
            XLAT_USB_POWER_REG &= XLAT_USBIO_EXIT_SLEEP_PH1;
            XLAT_CR1_REG = XLAT_backup.usbState;
            XLAT_USB_POWER_REG &= XLAT_USBIO_EXIT_SLEEP_PH2;
        #endif
    #endif
    #if defined(CYIPBLOCK_m0s8ioss_VERSION) && defined(XLAT__SIO)
        XLAT_SIO_REG = XLAT_backup.sioState;
    #endif
}


/* [] END OF FILE */
