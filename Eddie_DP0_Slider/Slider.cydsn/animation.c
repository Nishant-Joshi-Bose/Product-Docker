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
#define PATTERN_ROW_SIZE (PATTERN_SIZE + sizeof(uint16_t)) // +duration

static BOOL animationRunning = FALSE;
static BOOL patternDurationReached = FALSE;
// Each row in the animation buffer is one pattern + the duration in ms
static uint8_t animationBuffer[MAX_PATTERNS * PATTERN_ROW_SIZE];
static uint8_t currentPattern = 0;
static uint8_t animationLength = 0;
static BOOL autoStart = FALSE;
static BOOL loop = FALSE;
static BOOL loading = FALSE;
static uint8_t lastPattern = 0xff;

CY_ISR(AnimationTimerHandler)
{
    Timer_1_ClearInterrupt(Timer_1_INTR_MASK_TC);
    Timer_1_Stop();

    patternDurationReached = TRUE;
}

void AnimationInit(void)
{
    TimerISR_StartEx(AnimationTimerHandler);
    Timer_1_Stop();
}

static void RunPattern(uint8_t patternIdx)
{
    patternDurationReached = FALSE;
    uint8_t *pattern = &(animationBuffer[patternIdx * PATTERN_ROW_SIZE]);
    uint16_t duration = *((uint16_t*)pattern);
    pattern += 2;
    Timer_1_Stop();
    Timer_1_WriteCounter(duration * 10); // Timer clock is set to 10KHz
    // The timer could expire by the time the pattern is loaded but we still need to show it
    Timer_1_Start();
    LedsShowPattern(pattern);
}

void AnimationRun(void)
{
    if (!animationRunning)
    {
        return;
    }

    if (patternDurationReached)
    {
        currentPattern++;
        patternDurationReached = FALSE;
    }

    if (currentPattern > animationLength)
    {
        currentPattern = 0;
        if (!loop)
        {
            animationRunning = FALSE;
            uint8_t buff[COMMS_TX_BUFFER_SIZE] = {COMMS_RESPONSE_ANIMATION_STOPPED};
            CommsSendData(buff);
            lastPattern = 0xff;
            return;
        }
    }

    if (currentPattern != lastPattern)
    {
        RunPattern(currentPattern);
    }

    lastPattern = currentPattern;
}

BOOL AnimationHandleCommand(const uint8_t *buff)
{
    if (buff[0] == COMMS_COMMAND_ANIMATION_LOADSTART)
    {
        if (animationRunning || loading)
        {
            CommsSendStatus(COMMS_STATUS_FAILURE);
            return FALSE;
        }
        currentPattern = 0;
        animationLength = buff[1];
        autoStart = buff[2];
        loop = buff[3];
        loading = TRUE;
    }
    else if (buff[0] == COMMS_COMMAND_ANIMATION_LOADPATTERN)
    {
        if (!loading)
        {
            CommsSendStatus(COMMS_STATUS_FAILURE);
            return FALSE;
        }
        uint16_t duration = *((uint16_t *)(&buff[2]));
        // Pack the pattern's 16-bit intensities into 12-bits
        uint8_t *animBufferPos = &animationBuffer[currentPattern * PATTERN_ROW_SIZE];
        uint8_t const *inBufferPos = &buff[4];

        // Stick the duration at the front
        *((uint16_t *)animBufferPos) = duration;
        animBufferPos += 2;
        
        for (uint8_t i = 0; i < MAX_LEDS; i++) {
            *animBufferPos++ = (*inBufferPos++ >> 4) & 0xFF;
            *animBufferPos++ = ((*inBufferPos << 4) & 0xF0) | ((*(inBufferPos+1) >> 8) & 0x0F);
            inBufferPos++; inBufferPos++;
            *animBufferPos++ = *inBufferPos++;
        }

        currentPattern++;
        if (currentPattern >= animationLength)
        {
            loading = FALSE;
            currentPattern = 0;
            lastPattern = 0xff;
            if (autoStart)
            {
                animationRunning = TRUE;
                static uint8_t buff[COMMS_TX_BUFFER_SIZE] = {COMMS_RESPONSE_ANIMATION_STARTED};
                CommsSendData(buff);
                return TRUE;
            }
        }
    }
    else if (buff[0] == COMMS_COMMAND_ANIMATION_START)
    {
        if (loading || currentPattern != 0 || animationLength == 0)
        {
            CommsSendStatus(COMMS_STATUS_FAILURE);
            return FALSE;
        }
        animationRunning = TRUE;
        currentPattern = 0;
        lastPattern = 0xff;
        uint8_t buff[COMMS_TX_BUFFER_SIZE] = {COMMS_RESPONSE_ANIMATION_STARTED};
        CommsSendData(buff);
        return TRUE;
    }
    else if (buff[0] == COMMS_COMMAND_ANIMATION_STOPATEND)
    {
        loop = FALSE;
    }
    else if (buff[0] == COMMS_COMMAND_ANIMATION_STOPIMMEDIATE)
    {
        animationRunning = FALSE;
        currentPattern = 0;
        uint8_t buff[COMMS_TX_BUFFER_SIZE] = {COMMS_RESPONSE_ANIMATION_STOPPED};
        CommsSendData(buff);
        return TRUE;
    }
    else if (buff[0] == COMMS_COMMAND_ANIMATION_RESUME)
    {
        // TBD
    }

    uint8_t respBuff[COMMS_TX_BUFFER_SIZE] = {buff[0], 0x01};
    CommsSendData(respBuff);
    return FALSE;
}

/* [] END OF FILE */
