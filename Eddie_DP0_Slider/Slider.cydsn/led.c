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
#include "led.h"
#include "util.h"

//=============================================================================
//======================================================== types and structures
//=============================================================================
typedef struct tlc_5947_led_driver 
{
    char8  dummy[12];
    uint16 led0 :12;
    uint16 led1 :12;
    uint16 led2 :12;
    uint16 led3 :12;
    uint16 led4 :12;
    uint16 led5 :12;
    uint16 led6 :12;
    uint16 led7 :12;
    uint16 led8 :12;
    uint16 led9 :12;
    uint16 led10:12;
    uint16 led11:12;
    uint16 led12:12;
    uint16 led13:12;
    uint16 led14:12;
    uint16 led15:12;
    uint16 led16:12;
    uint16 led17:12;
    uint16 led18:12;
    uint16 led19:12;
    uint16 led20:12;
    uint16 led21:12;
    uint16 led22:12;
    uint16 led23:12;
}t_tlc_5947_led_driver;

//=============================================================================
//============================================================ static variables
//=============================================================================
t_tlc_5947_led_driver led_buffer;

static uint8 size_write = sizeof(led_buffer) / sizeof(uint16);

static void update_led_buffer()
{
    CapSense_ISR_Disable();
    SPIM_SpiUartPutArray((const uint16*)&led_buffer, size_write);
    Blank_Write(1);
    Latch_Write(1);
    CyDelayUs  (1);
    Latch_Write(0);
    Blank_Write(0);
    CapSense_ISR_Enable();
}// update_led_buffer

static void set_led_buffer (char led, uint16 intensity)
{
    uint16  set_value = intensity; // min(intensity, MAX_INTENSITY);
    uint16  led_min   = min(led, MAX_LEDS - 1);

    switch (led_min)
    {
        case 23: led_buffer.led0  = set_value;  // E_LED_LEFT_BLUE
                 break;
        case 22: led_buffer.led1  = set_value;  // E_LED_LEFT_GREEN
                 break;
        case 21: led_buffer.led2  = set_value;  // E_LED_LEFT_RED 
                 break;
        case 20: led_buffer.led3  = set_value;  // E_LED_RIGHT_BLUE  
                 break;
        case 19: led_buffer.led4  = set_value;  // E_LED_RIGHT_GREEN  
                 break;
        case 18: led_buffer.led5  = set_value;  // E_LED_RIGHT_RED
                 break;
        case 17: led_buffer.led6  = set_value;  // not used 
                 break;
        case 16: led_buffer.led7  = set_value;  // not used
                 break;
        case 15: led_buffer.led8  = set_value;  // not used
                 break;
        case 14: led_buffer.led9  = set_value;  // WHITE 14
                 break;
        case 13: led_buffer.led10 = set_value;  // WHITE 13 
                 break;
        case 12: led_buffer.led11 = set_value;  // WHITE 12
                 break;
        case 11: led_buffer.led12 = set_value;  // WHITE 11
                 break;
        case 10: led_buffer.led13 = set_value;  // WHITE 10
                 break;
        case  9: led_buffer.led14 = set_value;  // WHITE 9
                 break;
        case  8: led_buffer.led15 = set_value;  // WHITE 8
                 break;
        case  7: led_buffer.led16 = set_value;  // WHITE 7
                 break;
        case  6: led_buffer.led17 = set_value;  // WHITE 6
                 break;
        case  5: led_buffer.led18 = set_value;  // WHITE 5
                 break;
        case  4: led_buffer.led19 = set_value;  // WHITE 4
                 break;
        case  3: led_buffer.led20 = set_value;  // WHITE 3
                 break;
        case  2: led_buffer.led21 = set_value;  // WHITE 2
                 break;
        case  1: led_buffer.led22 = set_value;  // WHITE 1
                 break;
        case  0: led_buffer.led23 = set_value;  // WHITE 0
                 break;
        default: return;
    }
    
    return;
}

void initialize_leds()
{
    SPIM_Start();
    set_all_leds_intensity(0x00);
}

void set_led_strip (uint16 led_vec[MAX_LEDS])
{
    led_buffer.led0  = led_vec[23];
    led_buffer.led1  = led_vec[22];
    led_buffer.led2  = led_vec[21];
    led_buffer.led3  = led_vec[20];
    led_buffer.led4  = led_vec[19];
    led_buffer.led5  = led_vec[18];
    led_buffer.led6  = led_vec[17];
    led_buffer.led7  = led_vec[16];
    led_buffer.led8  = led_vec[15];
    led_buffer.led9  = led_vec[14];
    led_buffer.led10 = led_vec[13];
    led_buffer.led11 = led_vec[12];
    led_buffer.led12 = led_vec[11];
    led_buffer.led13 = led_vec[10];
    led_buffer.led14 = led_vec[ 9];
    led_buffer.led15 = led_vec[ 8];
    led_buffer.led16 = led_vec[ 7];
    led_buffer.led17 = led_vec[ 6];
    led_buffer.led18 = led_vec[ 5];
    led_buffer.led19 = led_vec[ 4];
    led_buffer.led20 = led_vec[ 3];
    led_buffer.led21 = led_vec[ 2];
    led_buffer.led22 = led_vec[ 1];
    led_buffer.led23 = led_vec[ 0];

    update_led_buffer();
}

void set_led (char led, uint16 intensity)
{
    set_led_buffer(led, intensity);
    update_led_buffer();
}

void set_all_leds_intensity (uint16 intensity)
{
    int i;
    
    for (i = 0; i < MAX_LEDS; i++)
    {
        set_led_buffer(i, intensity);  
    }
        
    update_led_buffer();
}

BOOL LedsHandleCommand(const uint8_t *buff)
{
    return FALSE;
}

/* [] END OF FILE */
