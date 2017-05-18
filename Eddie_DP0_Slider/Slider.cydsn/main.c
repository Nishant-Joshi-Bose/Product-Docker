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
#include <project.h>
#include "util.h"
#include "led.h"
#include "animation.h"
#include "command_telemetry.h"
#include "comms.h"

//=============================================================================
//=================================================================== constants
//=============================================================================
#define SLIDER_RESOLUTION 600
#define START_OF_CMD      '{'
#define END_OF_CMD        '}'
#define NUMBER_OF_BUTTONS (CapSense_TOTAL_WIDGETS - 1)

typedef struct {
    uint8 capsense_id   ; 
    BOOL  is_button_down;
    char* name          ;
}t_push_button_struct;

t_push_button_struct push_button_vec[NUMBER_OF_BUTTONS] = { {CapSense_BUT0_WDGT_ID, FALSE, "BT0"},
                                                            {CapSense_BUT1_WDGT_ID, FALSE, "BT1"},
                                                            {CapSense_BUT3_WDGT_ID, FALSE, "BT3"   }, 
                                                            {CapSense_BUT4_WDGT_ID, FALSE, "BT4"   }, 
                                                            {CapSense_BUT5_WDGT_ID, FALSE, "BT5"   }, 
                                                            {CapSense_BUT6_WDGT_ID, FALSE, "BT6"   }, 
                                                            {CapSense_BUT8_WDGT_ID, FALSE, "BT8"   }};
                                                 
//=============================================================================
//========================================================= function prototypes
//=============================================================================
void   clock_interrupt_handler     (void);
uint16 display_finger_tracker      (uint16 last_centroid_pos, uint32 centroid_pos);
uint16 get_button_pos              (uint32 cap_old_pos, uint32 cap_new_pos);
uint16 get_capsense_touched_sensors(void);
uint16 get_led_pos                 (uint32 centroid_pos);
void   reset_timer                 (void);
void   push_button_scan            (void);
                  
//=============================================================================
//======================================================================== main
//=============================================================================
int main()
{
    BOOL                    cap_sense_is_untouched  = TRUE;
    BOOL                    cap_sense_was_untouched = FALSE;
    uint16                  cap_cur_pos             = CapSense_SLIDER_NO_TOUCH;
    uint16                  cap_old_pos             = CapSense_SLIDER_NO_TOUCH;
    uint16                  led_pos                 = CapSense_SLIDER_NO_TOUCH;
    uint16                  new_led_pos             = CapSense_SLIDER_NO_TOUCH;
    static t_enum_direction finger_direction        = DIRECT_STOP;
    char*                   command                 = NULL;
    uint16                  tmp                     = 0;
    uint16                  current_callback_anim   = 0;

    CyGlobalIntEnable; // enable global interrupts before starting capsense and I2C blocks

    //TimerISR_StartEx(clock_interrupt_handler);
    //Timer_1_Start   ();
    initialize_leds ();

    CommsInit();

    CapSense_Start();

    for(;;)
    {

        if (CommsIsInputBufferReady())
        {
            if (get_is_telemetry_enable() == FALSE)
            {
                set_is_telemetry_enable(TRUE);
            }// the telemetry were disabled but we received something from the client

            char *receive_string = (char *)CommsGetInputBuffer();

            if ((receive_string[0] != START_OF_CMD) || (receive_string[strlen(receive_string) - 1] != END_OF_CMD))
            {
                send_alarm_telemetry(ALARM_WARNING, "", "invalid start-end character(s)");
            }// if the command is not well formed
            else
            {
                receive_string[strlen(receive_string) - 1] = '\0';
                if (parse_and_execute_command(&(receive_string[1])) == FALSE)
                {
                    send_alarm_telemetry(ALARM_WARNING, command, "failed to execute the command");
                }// If we failed to execute the command
                else
                {
                    send_alarm_telemetry(ALARM_LOG, command, "executed");
                }
            }// else, the command seems well formed

            CommsResetInputBuffer();
        }
        
        if (cap_sense_is_untouched && get_is_anim_on_button_up_enable())
        {
            run_current_animation(TRUE);
        }
        
        if (CapSense_IsBusy() != CapSense_NOT_BUSY)
        {
            continue;    
        }
        
        // process data for previous scan and initiate new scan only when the capsense hardware is idle
        if (CapSense_ProcessAllWidgets() == CYRET_INVALID_STATE)
        {
            continue;    
        }
        
        push_button_scan();

        cap_cur_pos            = CapSense_GetCentroidPos(0);
        cap_sense_is_untouched = ((cap_cur_pos == CapSense_SLIDER_NO_TOUCH) && (cap_old_pos == CapSense_SLIDER_NO_TOUCH));
        
        if (cap_sense_is_untouched)
        {
            if (! cap_sense_was_untouched)
            {
                if (get_is_tracker_enable()) 
                {
                    set_all_leds_intensity(0x00);
                }
                
                finger_direction = DIRECT_STOP;
                send_button_telemetry(get_button_pos(cap_old_pos, cap_cur_pos), "up", finger_direction, cap_cur_pos, cap_old_pos);
                cap_cur_pos = CapSense_SLIDER_NO_TOUCH;
                cap_old_pos = CapSense_SLIDER_NO_TOUCH;
                led_pos     = NUMBER_OF_WHITE_LEDS + 1;
               
                if (get_is_auto_anim_rotation_enable())
                {
                    current_callback_anim = get_current_callback_anim();
                    current_callback_anim++;
                    current_callback_anim %= ANIM_UNKNOWN;
                    set_current_callback_anim(current_callback_anim);
                }
            }
   
            if (get_is_anim_on_button_up_enable())
            {
                run_current_animation(FALSE);
            }
        }// if capsense is untouched
        else
        {
            if (cap_sense_was_untouched)
            {
                tmp = get_button_pos(cap_old_pos, cap_cur_pos);
                reset_timer();
                send_button_telemetry(tmp, "down"    , finger_direction, cap_cur_pos, cap_old_pos);
                send_button_telemetry(tmp, "position", finger_direction, cap_cur_pos, cap_old_pos);
                if (get_is_anim_on_button_up_enable())
                {
                    set_all_leds_intensity(0x00);
                    send_animation_telemetry(current_callback_anim, "stopping");
                }
            }
            
            if ((cap_cur_pos != CapSense_SLIDER_NO_TOUCH) && (cap_cur_pos != cap_old_pos))
            {
                if (cap_old_pos != CapSense_SLIDER_NO_TOUCH)
                {
                    finger_direction = (cap_cur_pos > cap_old_pos ? DIRECT_RIGHT : DIRECT_LEFT);
                }
                
                new_led_pos = display_finger_tracker (cap_old_pos, cap_cur_pos);
                if (new_led_pos != led_pos)
                {
                    led_pos = new_led_pos;
                    send_button_telemetry(get_button_pos(cap_old_pos, cap_cur_pos), "position", finger_direction, cap_cur_pos, cap_old_pos);
                }
            }
            
            cap_old_pos = cap_cur_pos;
        }// else, capsense is touched
        cap_sense_was_untouched = cap_sense_is_untouched;

#if USE_TUNER
        CapSense_RunTuner(); // sync capsense parameters via tuner before the beginning of new capsense scan
#endif // USE_TUNER

        CapSense_ScanAllWidgets();
    }// for ever
}// main

//=============================================================================
//===================================================== clock_interrupt_handler
//=============================================================================
void clock_interrupt_handler(void)
{
    uint8 enableInterrupts = CyEnterCriticalSection();
        /* Check interrupt source */
       	if (Timer_1_INTR_MASK_CC_MATCH == Timer_1_GetInterruptSourceMasked())
        {
            /* Clear interrupt and then process Capture interrupt */
            Timer_1_ClearInterrupt(Timer_1_INTR_MASK_CC_MATCH);
        }
        else
        {
            /* Clear interrupt and then process Terminal Count interrupt */
            Timer_1_ClearInterrupt(Timer_1_INTR_MASK_TC);
        }
        set_timer_interrrupt_count(get_timer_interrrupt_count() + 1);    
    CyExitCriticalSection(enableInterrupts);
}// clock_interrupt_handler

//=============================================================================
//============================================================== display_button
//=============================================================================
void display_button(uint16 button, uint16 intensity, uint16 button_width)
{
    int side_leds_intensity = intensity / 4;

#if FORCE_BUTTON_WIDTH_TO_1
        side_leds_intensity = 0;
#endif // FORCE_BUTTON_WIDTH_TO_1

#if 0
    uint16 led_start = button    * button_width;
    uint16 led_stop  = led_start + button_width;
    uint16 i;
    
    set_led(led_start, side_leds_intensity);
    set_led(led_stop , side_leds_intensity);
    
    for (i = led_start + 1; i < led_stop; i++)
    {
        set_led(i, intensity);      
    }      
#else // This would work only with 6 buttons...For testing purpose only
    button_width = 0; // to avoid warning
    switch (button)
    {
        case 1: set_led(1, side_leds_intensity);
                set_led(2, intensity          );
                set_led(3, side_leds_intensity);
                break;
        case 2: set_led(3, side_leds_intensity);
                set_led(4, intensity          );
                set_led(5, side_leds_intensity);
                break;
        case 3: set_led(5, side_leds_intensity);
                set_led(6, intensity          );
                set_led(7, side_leds_intensity);
                break;
        case 4: set_led(7, side_leds_intensity);
                set_led(8, intensity          );
                set_led(9, side_leds_intensity);
                break;
        case 5: set_led(9 , side_leds_intensity);
                set_led(10, intensity          );
                set_led(11, side_leds_intensity);
                break;
        case 6 : 
        default:
                set_led(11, side_leds_intensity);
                set_led(12, intensity          );
                set_led(13, side_leds_intensity);
                break; 
    }// switch on button id    
#endif // 0    
}// display_button

//=============================================================================
//====================================================== display_finger_tracker
//=============================================================================
uint16 display_finger_tracker(uint16 cap_old_pos, uint32 cap_new_pos)
{
    uint16 button_width = NUMBER_OF_WHITE_LEDS / get_number_of_buttons();
    
    if (get_is_tracker_enable())
    {
        if (button_width > 1)
        {
            display_button(get_button_pos(cap_old_pos, cap_old_pos), 0x00         , button_width);
            display_button(get_button_pos(cap_new_pos, cap_new_pos), MAX_INTENSITY, button_width);
        }
        else
        {
            set_led(get_led_pos(cap_old_pos), 0x00         );
            set_led(get_led_pos(cap_new_pos), MAX_INTENSITY);
        }
    }// if the finger tracker is enable

     return get_led_pos(cap_new_pos);
}// display_finger_tracker

//=============================================================================
//============================================================== get_button_pos
//=============================================================================
uint16 get_button_pos(uint32 cap_old_pos, uint32 cap_new_pos)
{   
    static uint16 button_pos = 0;
    
    if ((cap_old_pos == CapSense_SLIDER_NO_TOUCH) && (cap_new_pos != CapSense_SLIDER_NO_TOUCH))
    {
        button_pos = (cap_new_pos / (SLIDER_RESOLUTION / get_number_of_buttons())) + 1;
    }// if it's a button down
    else 
    {
        if (cap_new_pos <= 80)
        {
            button_pos = 1;       
        }
        else if ((cap_new_pos >= 120) && (cap_new_pos <= 180))
        {
            button_pos = 2;       
        }
        else if ((cap_new_pos >= 220) && (cap_new_pos <= 280))
        {
            button_pos = 3;       
        }
        else if ((cap_new_pos >= 320) && (cap_new_pos <= 380))
        {
            button_pos = 4;       
        }
        else if ((cap_new_pos >= 420) && (cap_new_pos <= 480))
        {
            button_pos = 5;       
        }
        else if ((cap_new_pos >= 520) && (cap_new_pos <= 600))
        {
            button_pos = 6;       
        }    
    }// else, it's a slide
    
    return button_pos;    
}// get_button_pos

//=============================================================================
//================================================================= get_led_pos
//=============================================================================
uint16 get_led_pos(uint32 centroid_pos)
{    
    return min(NUMBER_OF_WHITE_LEDS, max(1, centroid_pos / (SLIDER_RESOLUTION / NUMBER_OF_WHITE_LEDS)));
}// get_led_pos

//=============================================================================
//================================================ get_capsense_touched_sensors
//=============================================================================
uint16 get_capsense_touched_sensors (void)
{
    uint16 i;
    uint16 max_sensor = CapSense_GET_SENSOR_COUNT(0);
    uint16 nb_sensors = 0;
      
    for (i = 0; i < max_sensor; i++)
    {
        if (CapSense_IsSensorActive(0, i))
        {
            nb_sensors++;
        }      
    }
   
    return nb_sensors;
}// get_capsense_touched_sensors

//=============================================================================
//================================================================= reset_timer
//=============================================================================
void reset_timer(void)
{
    uint8 enableInterrupts = CyEnterCriticalSection();
     
        Timer_1_WriteCounter(0);
        set_timer_interrrupt_count(0);
    
    CyExitCriticalSection(enableInterrupts);
}// reset_timer

//=============================================================================
//============================================================ push_button_scan
//=============================================================================
void push_button_scan (void){
    
    uint8 i = 0;
    char  telemetry[64];
   
    memset(telemetry, '\0', sizeof(telemetry));
    
    for (i = 0; i < NUMBER_OF_BUTTONS; i++)
    {
        if (CapSense_IsWidgetActive(push_button_vec[i].capsense_id) != push_button_vec[i].is_button_down) 
        {
            push_button_vec[i].is_button_down = CapSense_IsWidgetActive(push_button_vec[i].capsense_id);
            send_push_button_telemetry(push_button_vec[i].capsense_id, push_button_vec[i].name, push_button_vec[i].is_button_down);
        }// If the button state changed
    }// for all the buttons
          
}// push_button_scan


/* [] END OF FILE */

