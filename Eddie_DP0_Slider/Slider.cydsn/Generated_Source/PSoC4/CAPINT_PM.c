/*******************************************************************************
* File Name: CAPINT.c  
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
#include "CAPINT.h"

static CAPINT_BACKUP_STRUCT  CAPINT_backup = {0u, 0u, 0u};


/*******************************************************************************
* Function Name: CAPINT_Sleep
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
*  \snippet CAPINT_SUT.c usage_CAPINT_Sleep_Wakeup
*******************************************************************************/
void CAPINT_Sleep(void)
{
    #if defined(CAPINT__PC)
        CAPINT_backup.pcState = CAPINT_PC;
    #else
        #if (CY_PSOC4_4200L)
            /* Save the regulator state and put the PHY into suspend mode */
            CAPINT_backup.usbState = CAPINT_CR1_REG;
            CAPINT_USB_POWER_REG |= CAPINT_USBIO_ENTER_SLEEP;
            CAPINT_CR1_REG &= CAPINT_USBIO_CR1_OFF;
        #endif
    #endif
    #if defined(CYIPBLOCK_m0s8ioss_VERSION) && defined(CAPINT__SIO)
        CAPINT_backup.sioState = CAPINT_SIO_REG;
        /* SIO requires unregulated output buffer and single ended input buffer */
        CAPINT_SIO_REG &= (uint32)(~CAPINT_SIO_LPM_MASK);
    #endif  
}


/*******************************************************************************
* Function Name: CAPINT_Wakeup
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
*  Refer to CAPINT_Sleep() for an example usage.
*******************************************************************************/
void CAPINT_Wakeup(void)
{
    #if defined(CAPINT__PC)
        CAPINT_PC = CAPINT_backup.pcState;
    #else
        #if (CY_PSOC4_4200L)
            /* Restore the regulator state and come out of suspend mode */
            CAPINT_USB_POWER_REG &= CAPINT_USBIO_EXIT_SLEEP_PH1;
            CAPINT_CR1_REG = CAPINT_backup.usbState;
            CAPINT_USB_POWER_REG &= CAPINT_USBIO_EXIT_SLEEP_PH2;
        #endif
    #endif
    #if defined(CYIPBLOCK_m0s8ioss_VERSION) && defined(CAPINT__SIO)
        CAPINT_SIO_REG = CAPINT_backup.sioState;
    #endif
}


/* [] END OF FILE */
