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

/*****************************************************************************
*  Project Name: Bootloadable_Blue
*  Device Tested: CY8C4245AXI-483
*  Software Version: PSoC Creator 3.2 SP2
*  Compilers Tested: ARM GCC
*  Related Hardware: CY8CKIT-042
*****************************************************************************
***************************************************************************** */

/* Project Description:
* Example Project to demonstrate how to create bootloadable projects and how to 
* enter bootloader from bootloadable project. And The RGB LED blinks with blue 
* color on target CY8CKIT-042.
***************************************************************************** */

#include <device.h>

/* The flag to enter the bootloader */
uint8 bootload_flag;

void main()
{
	/* Turn on LED */
    Pin_LED_Write(0u);
    
	/* Start pin isr */
	isr_EnterBootloader_Start();
	
	CyGlobalIntEnable;
	
    for(;;)
    {
        /* 'bootload_flag' is set in the isr_EnterBootloader.c ,when 
		the switch connected to Pin_StartBootloader is pressed */
		if(bootload_flag)
		{	
			/* Turn off LED */
			Pin_LED_Write(1u);
			
			/* Enter Bootloader */
			Bootloadable_Load();
			
		    /* If the 'CyBtldr_Load' function is executed succesfully ,
			the program execution will never execute the next instruction */
			bootload_flag = 0u;
			Pin_LED_Write(0u);
		}
		
		/* Toggle the LED */
		Pin_LED_Write(~Pin_LED_Read());
		
		/* Delay 200ms */
		CyDelay(200u);
    }
}


/* [] END OF FILE */
