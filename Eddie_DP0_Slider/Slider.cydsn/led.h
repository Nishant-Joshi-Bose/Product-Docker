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
#ifndef LED_H
#define LED_H

#include <project.h>
#include "util.h"

#define MAX_LEDS               24 

void LedsInit(void);
BOOL LedsHandleCommand(const uint8_t *buff);
void LedsShowPattern(uint8_t *pattern);

#endif // LED_H
/* [] END OF FILE */
