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
#ifndef UTIL_H
#define UTIL_H
    
#include <project.h>
    
//=============================================================================
//=================================================================== constants
//=============================================================================
#ifndef BOOL
#define BOOL uint8
#endif // BOOL

#ifndef TRUE
#define TRUE  1
#define FALSE 0
#endif // TRUE

#ifndef min
#define min(a,b) ((a)<(b)?(a):(b))
#define max(a,b) ((a)>(b)?(a):(b))    
#endif // min

#define FOR_COLIN                    0 // will stop R, G, B animations to change intensity
#define FORCE_BUTTON_WIDTH_TO_1      1 // will force the display of button to 1 LED to avoid glare
#define USE_UART                     1 // will enable the UART, otherwise, the I2C bus will in use
#define USE_TUNER                    0 // will enable the Cypress capsense tuner
#define USE_BOOTLOADER               0 // will enable the bootloader usage
#define USE_128_POINTS_FOR_BREATHING 0 // will use 128 points for the LED breathing algo, default is 256
#define USE_LED                      0 // will disable the LED SPI access, for debugging prupose only
#define USE_ALL_BUTTON_TELE          0 // ....
#define TED_TEST_BUTTON              1

//=============================================================================
//======================================================== types and structures
//=============================================================================
typedef enum enum_direction 
{
    DIRECT_LEFT  = 'L',
    DIRECT_RIGHT = 'R',
    DIRECT_STOP  = 'S'
}t_enum_direction;

//=============================================================================
//========================================================= function prototypes
//=============================================================================  
char*         itoa                      (long long int i);
int           atoi                      (register char* string);
BOOL          get_is_tracker_enable     ();
void          set_is_tracker_enable     (BOOL is_tracker_enable);
uint16        get_number_of_buttons     ();
void          set_number_of_buttons     (uint16 number_of_buttons);
uint_fast64_t get_timer_interrrupt_count();
void          set_timer_interrrupt_count(uint_fast64_t timer_interrrupt_count);
const char*   get_software_version      ();

#endif // UTIL_H
/* [] END OF FILE */
