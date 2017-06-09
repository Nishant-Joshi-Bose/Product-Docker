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

//=============================================================================
//=================================================================== constants
//=============================================================================
#define INT_DIGITS                20 // enough for 64 bits int
#define DEFAULT_NUMBER_OF_BUTTONS 6  // [1..NUMBER_OF_WHITE_LEDS]

//=============================================================================
//============================================================ static variables
//=============================================================================
static BOOL          is_tracker_enable      = TRUE; // when enable: the finger tracker lightbar animation will be follow the finger motion.
static uint16        number_of_buttons      = DEFAULT_NUMBER_OF_BUTTONS;
static uint_fast64_t timer_interrrupt_count = 0;
static const char*   software_version       = "0.08";

//=============================================================================
//======================================================================== atoi
//=============================================================================
int atoi(register char* string)
{
    register int          result = 0;
    register unsigned int digit  = 0;
    int                   sign   = 0;

    while (*string == ' ') 
    {
        string++;
    }// skip all white space on left

    if (*string == '-') 
    {
    	sign = 1;
    	string++;
    } 
    else 
    {
    	sign = 0;
    	if (*string == '+') 
        {
    	    string++;
    	}
    }

    for ( ; ; string++) 
    {
    	digit = *string - '0';
    	if (digit > 9) 
        {
    	    break;
    	}
    	result = (10 * result) + digit;
    }

    return sign ? -result : result;
}// atoi

//=============================================================================
//======================================================================== itoa
//=============================================================================
char* itoa(long long int i)
{
    static char buf[INT_DIGITS + 2];                    // room for INT_DIGITS digits, sign and '\0'
    char*       p              = buf + INT_DIGITS + 1;  // points to terminating '\0'
    BOOL        negative_value = (i < 0) ? TRUE : FALSE;
  
    do 
    {
        *--p  = '0' + (i % 10);
        i    /= 10;
    } while (i != 0);
    
    if (negative_value) {
        *--p = '-';
    }
    
    return p;
}// itoa

//=============================================================================
//======================================================= get_is_tracker_enable
//=============================================================================
BOOL get_is_tracker_enable()
{
    return is_tracker_enable;
    
}// get_is_tracker_enable

//=============================================================================
//======================================================= set_is_tracker_enable
//=============================================================================
void set_is_tracker_enable(BOOL the_is_tracker_enable)
{
    is_tracker_enable = the_is_tracker_enable;
}// set_is_tracker_enable

//=============================================================================
//======================================================= get_number_of_buttons
//=============================================================================
uint16 get_number_of_buttons()
{
    return number_of_buttons;
}// get_number_of_buttons

//=============================================================================
//======================================================= set_number_of_buttons 
//=============================================================================
void set_number_of_buttons(uint16 the_number_of_buttons)
{
    the_number_of_buttons = number_of_buttons;
}// set_number_of_buttons

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
