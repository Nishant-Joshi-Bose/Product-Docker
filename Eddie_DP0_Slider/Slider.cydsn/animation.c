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
#include "comms.h"
#include "animation.h"

#define PATTERN_SIZE (MAX_LEDS * 12 / 8)
#define MAX_PATTERNS 40

static BOOL animationRunning = FALSE;
static BOOL patternDurationReached = FALSE;
// Each row in the animation buffer is one pattern + the duration in ms
static uint8_t animationBuffer[MAX_PATTERNS * (PATTERN_SIZE + sizeof(uint16_t))];

CY_ISR(AnimationTimerHandler)
{
    Timer_1_ClearInterrupt(Timer_1_INTR_MASK_TC);
    Timer_1_Stop();

    patternDurationReached = TRUE;
}

void AnimationInit(void)
{
    TimerISR_StartEx(AnimationTimerHandler);
}

static void StartAnimation(uint16_t durationInMillis)
{
    animationRunning = TRUE;
    patternDurationReached = FALSE;
    Timer_1_Stop();
    Timer_1_WriteCounter(durationInMillis * 10); // Timer clock is set to 10KHz
    Timer_1_Start();
}

BOOL AnimationHandleCommand(const uint8_t *buff)
{
    if (buff[0] == COMMS_COMMAND_ANIMATION_LOADSTART)
    {
        if (animationRunning)
        {
            CommsSendStatus(COMMS_STATUS_FAILURE);
            return FALSE;
        }
    }
    else if (buff[0] == COMMS_COMMAND_ANIMATION_LOADPATTERN)
    {
    }
    else if (buff[0] == COMMS_COMMAND_ANIMATION_START)
    {
    }
    else if (buff[0] == COMMS_COMMAND_ANIMATION_STOPATEND)
    {
    }
    else if (buff[0] == COMMS_COMMAND_ANIMATION_STOPIMMEDIATE)
    {
    }
    else if (buff[0] == COMMS_COMMAND_ANIMATION_RESUME)
    {
    }
    return FALSE;
}

/* [] END OF FILE */
