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
#include "animation.h"

CY_ISR(AnimationTimerHandler)
{
}

void AnimationInit(void)
{
    TimerISR_StartEx(AnimationTimerHandler);
}

/* [] END OF FILE */
