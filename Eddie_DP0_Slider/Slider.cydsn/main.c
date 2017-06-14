﻿/* ========================================
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
#include "comms.h"
#include "capsensehdlr.h"


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
}

void reset_timer(void)
{
    uint8 enableInterrupts = CyEnterCriticalSection();

        Timer_1_WriteCounter(0);
        set_timer_interrrupt_count(0);

    CyExitCriticalSection(enableInterrupts);
}


int main()
{
    CyGlobalIntEnable; // enable global interrupts before starting capsense and I2C blocks

    LedsInit();
    CommsInit();

    CapsenseHandlerInit();

    TimerISR_StartEx(clock_interrupt_handler);
    Timer_1_Start   ();

    for(;;)
    {
        CommsHandleIncoming();
        CapsenseHandlerScan();
    }
}
