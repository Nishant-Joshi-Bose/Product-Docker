/*******************************************************************************
* File Name: Latch.c  
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
#include "Latch.h"

static Latch_BACKUP_STRUCT  Latch_backup = {0u, 0u, 0u};


/*******************************************************************************
* Function Name: Latch_Sleep
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
*  \snippet Latch_SUT.c usage_Latch_Sleep_Wakeup
*******************************************************************************/
void Latch_Sleep(void)
{
    #if defined(Latch__PC)
        Latch_backup.pcState = Latch_PC;
    #else
        #if (CY_PSOC4_4200L)
            /* Save the regulator state and put the PHY into suspend mode */
            Latch_backup.usbState = Latch_CR1_REG;
            Latch_USB_POWER_REG |= Latch_USBIO_ENTER_SLEEP;
            Latch_CR1_REG &= Latch_USBIO_CR1_OFF;
        #endif
    #endif
    #if defined(CYIPBLOCK_m0s8ioss_VERSION) && defined(Latch__SIO)
        Latch_backup.sioState = Latch_SIO_REG;
        /* SIO requires unregulated output buffer and single ended input buffer */
        Latch_SIO_REG &= (uint32)(~Latch_SIO_LPM_MASK);
    #endif  
}


/*******************************************************************************
* Function Name: Latch_Wakeup
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
*  Refer to Latch_Sleep() for an example usage.
*******************************************************************************/
void Latch_Wakeup(void)
{
    #if defined(Latch__PC)
        Latch_PC = Latch_backup.pcState;
    #else
        #if (CY_PSOC4_4200L)
            /* Restore the regulator state and come out of suspend mode */
            Latch_USB_POWER_REG &= Latch_USBIO_EXIT_SLEEP_PH1;
            Latch_CR1_REG = Latch_backup.usbState;
            Latch_USB_POWER_REG &= Latch_USBIO_EXIT_SLEEP_PH2;
        #endif
    #endif
    #if defined(CYIPBLOCK_m0s8ioss_VERSION) && defined(Latch__SIO)
        Latch_SIO_REG = Latch_backup.sioState;
    #endif
}


/* [] END OF FILE */
