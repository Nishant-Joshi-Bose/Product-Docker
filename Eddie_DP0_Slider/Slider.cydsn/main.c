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
#include "comms.h"
#include "capsensehdlr.h"

//=============================================================================
//========================================================= function prototypes
//=============================================================================
void   clock_interrupt_handler     (void);
void   reset_timer                 (void);
//=============================================================================
//======================================================================== main
//=============================================================================
int main()
{
    BOOL                    cap_sense_is_untouched  = TRUE;
    BOOL                    cap_sense_was_untouched = FALSE;
#ifndef PROFESSOR
    uint16                  cap_cur_pos             = CapSense_SLIDER_NO_TOUCH;
    uint16                  cap_old_pos             = CapSense_SLIDER_NO_TOUCH;
#endif
    uint16                  led_pos                 = CapSense_SLIDER_NO_TOUCH;
    uint16                  new_led_pos             = CapSense_SLIDER_NO_TOUCH;
    static t_enum_direction finger_direction        = DIRECT_STOP;
    char*                   command                 = NULL;
    uint16                  tmp                     = 0;
    uint16                  current_callback_anim   = 0;

    CyGlobalIntEnable; // enable global interrupts before starting capsense and I2C blocks

    TimerISR_StartEx(clock_interrupt_handler);
    Timer_1_Start   ();
    initialize_leds ();

    CommsInit();

    CapsenseHandlerInit();

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

        CapsenseHandlerScanButtons();
        CapsenseHandlerScanSliders();

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


/* [] END OF FILE */

