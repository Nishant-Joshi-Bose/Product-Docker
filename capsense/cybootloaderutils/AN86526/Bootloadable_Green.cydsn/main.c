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
*  Project Name: Bootloadable_Green
*  Device Tested: CY8C4245AXI-483
*  Software Version: PSoC Creator 3.2 SP2
*  Compilers Tested: ARM GCC
*  Related Hardware: CY8CKIT-042
******************************************************************************
* Project Description:
* Example Project to demonstrate how to create bootloadable project 
* for PSoC 4 using PSoC Creator. And The RGB LED blinks with green color 
* on target CY8CKIT-042.
***************************************************************************** */

#include <device.h>
void main()
{
    for(;;)
    {
		/* Toggle the LED */
		Pin_LED_Write(~Pin_LED_Read());
		
		/* Delay 1 second */
		CyDelay(1000u);        
    }
}


/* [] END OF FILE */
