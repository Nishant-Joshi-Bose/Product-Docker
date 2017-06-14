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
    
void set_led               (char led, uint16 intensity);
void set_led_triplet       (char left_led, uint16 intensity_left, uint16 intensity_center, uint16 intensity_right);
void set_led_pair          (char left_led, uint16 intensity_left, uint16 intensity_center);
void set_all_leds_intensity(uint16 intensity);
void set_led_strip         (uint16 led_vec[MAX_LEDS]);
void set_all_white_leds    (uint16 intensity);
void set_all_colored_leds  (uint16 intensity);
void initialize_leds       (void);

BOOL LedsHandleCommand(const uint8_t *buff);

#endif // LED_H
/* [] END OF FILE */
