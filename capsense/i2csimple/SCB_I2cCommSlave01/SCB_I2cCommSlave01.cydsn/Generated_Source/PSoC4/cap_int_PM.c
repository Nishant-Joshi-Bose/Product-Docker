/*******************************************************************************
* File Name: cap_int.c  
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
#include "cap_int.h"

static cap_int_BACKUP_STRUCT  cap_int_backup = {0u, 0u, 0u};


/*******************************************************************************
* Function Name: cap_int_Sleep
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
*  \snippet cap_int_SUT.c usage_cap_int_Sleep_Wakeup
*******************************************************************************/
void cap_int_Sleep(void)
{
    #if defined(cap_int__PC)
        cap_int_backup.pcState = cap_int_PC;
    #else
        #if (CY_PSOC4_4200L)
            /* Save the regulator state and put the PHY into suspend mode */
            cap_int_backup.usbState = cap_int_CR1_REG;
            cap_int_USB_POWER_REG |= cap_int_USBIO_ENTER_SLEEP;
            cap_int_CR1_REG &= cap_int_USBIO_CR1_OFF;
        #endif
    #endif
    #if defined(CYIPBLOCK_m0s8ioss_VERSION) && defined(cap_int__SIO)
        cap_int_backup.sioState = cap_int_SIO_REG;
        /* SIO requires unregulated output buffer and single ended input buffer */
        cap_int_SIO_REG &= (uint32)(~cap_int_SIO_LPM_MASK);
    #endif  
}


/*******************************************************************************
* Function Name: cap_int_Wakeup
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
*  Refer to cap_int_Sleep() for an example usage.
*******************************************************************************/
void cap_int_Wakeup(void)
{
    #if defined(cap_int__PC)
        cap_int_PC = cap_int_backup.pcState;
    #else
        #if (CY_PSOC4_4200L)
            /* Restore the regulator state and come out of suspend mode */
            cap_int_USB_POWER_REG &= cap_int_USBIO_EXIT_SLEEP_PH1;
            cap_int_CR1_REG = cap_int_backup.usbState;
            cap_int_USB_POWER_REG &= cap_int_USBIO_EXIT_SLEEP_PH2;
        #endif
    #endif
    #if defined(CYIPBLOCK_m0s8ioss_VERSION) && defined(cap_int__SIO)
        cap_int_SIO_REG = cap_int_backup.sioState;
    #endif
}


/* [] END OF FILE */
