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
#include "util.h"

static uint_fast64_t timer_interrrupt_count = 0;
static const char*   software_version       = "0.1";

//=============================================================================
//================================================== get_timer_interrrupt_count
//=============================================================================
uint_fast64_t get_timer_interrrupt_count()
{
    return timer_interrrupt_count;
}// get_timer_interrrupt_count

//=============================================================================
//================================================== set_timer_interrrupt_count
//=============================================================================
void set_timer_interrrupt_count(uint_fast64_t the_timer_interrrupt_count)
{
    timer_interrrupt_count = the_timer_interrrupt_count;
}// set_timer_interrrupt_count

//=============================================================================
//======================================================== get_software_version
//=============================================================================
const char* get_software_version()
{
    return software_version;    
}// get_software_version


/* [] END OF FILE */
