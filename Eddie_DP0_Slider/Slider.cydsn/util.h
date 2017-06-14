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

#define USE_TUNER                    0 // will enable the Cypress capsense tuner

uint_fast64_t get_timer_interrrupt_count();
void          set_timer_interrrupt_count(uint_fast64_t timer_interrrupt_count);
const char*   get_software_version      ();

#endif // UTIL_H
/* [] END OF FILE */
