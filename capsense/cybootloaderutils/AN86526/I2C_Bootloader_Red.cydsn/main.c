/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/
/****************************************************************************
*  Project Name: I2C_Bootloader_Red
*  Device Tested: CY8C4245AXI-483
*  Software Version: PSoC Creator 3.2 SP2
*  Compilers Tested: ARM GCC 
*  Related Hardware: CY8CKIT-042
*****************************************************************************
* Project Description:
* This project demonstartes how to create an I2C Bootloader Project for
* PSoC 4 using PSoC Creator. And the PWM controls the RGB LED blinking
* with red color on target CY8CKIT-042.
***************************************************************************** */
#include <device.h>

void main()
{
	/* Initialize PWM */
    PWM_Start();
    PWM_TriggerCommand(PWM_MASK,PWM_CMD_START);

    Bootloader_Start();

    /* CyGlobalIntEnable; */ /* Uncomment this line to enable global interrupts. */
    for(;;)
    {
        /* Place your application code here. */
    }
}


/* [] END OF FILE */
