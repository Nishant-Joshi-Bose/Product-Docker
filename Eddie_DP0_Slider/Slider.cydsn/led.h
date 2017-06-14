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

//=============================================================================
//=================================================================== constants
//=============================================================================
#define MAX_LEDS               24 
#define NUMBER_OF_WHITE_LEDS   15
#define NUMBER_OF_COLORED_LEDS 6
#define MAX_INTENSITY          0x0FFF //0x00FF // FIXME! 0x0FFF to save the battery
#define MAX_COLOR              0x0FFF
#define TOTAL_NUMBER_OF_LEDS   (NUMBER_OF_WHITE_LEDS + NUMBER_OF_WHITE_LEDS)
    
//=============================================================================
//======================================================== types and structures
//=============================================================================
typedef struct struct_rgb_color {
    char8 red  ;
    char8 green;
    char8 blue ;
}t_struct_rgb_color;

typedef enum enum_primary_color
{
    E_PRIMARY_COLOR_RED = 0,    
    E_PRIMARY_COLOR_GREEN  ,
    E_PRIMARY_COLOR_BLUE   ,
    E_PRIMARY_COLOR_NUMBER
}t_enum_primary_color;

typedef enum enum_color_led_pos
{
    E_LED_RIGHT_RED = 18, 
    E_LED_RIGHT_GREEN   ,   
    E_LED_RIGHT_BLUE    ,
    E_LED_LEFT_RED      ,
    E_LED_LEFT_GREEN    ,
    E_LED_LEFT_BLUE
}t_enum_color_led_pos;

//=============================================================================
//========================================================= function prototypes
//============================================================================= 
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
