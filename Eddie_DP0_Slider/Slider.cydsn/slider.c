/*
 * @file
 *
 * @brief
 *
 * @author Shelby Apps Team
 *
 * @attention
 *     BOSE CORPORATION.
 *     COPYRIGHT 2017 BOSE CORPORATION ALL RIGHTS RESERVED.
 *     This program may not be reproduced, in whole or in part in any
 *     form or any means whatsoever without the written permission of:
 *         BOSE CORPORATION
 *         The Mountain,
 *         Framingham, MA 01701-9168
 */

#include <project.h>
#include "util.h"
#include "slider.h"
#include "comms.h"

// Only compile slider stuff in if sliders were configured in PSoC creator
#if defined(CapSense_CSX_EN) && (CapSense_CSX_EN == CapSense_ENABLE)

typedef struct {
    uint8_t id;
    uint32_t lastPos;
} Slider_t;

// Automatically configure sliders based on PSoC configuration
// We currently only support 2
static Slider_t sliders[] =
{
#ifdef CapSense_LINEARSLIDER0_WDGT_ID
        { CapSense_LINEARSLIDER0_WDGT_ID, CapSense_SLIDER_NO_TOUCH },
#endif
#ifdef CapSense_LINEARSLIDER1_WDGT_ID
        { CapSense_LINEARSLIDER1_WDGT_ID, CapSense_SLIDER_NO_TOUCH },
#endif
};
#endif

#if defined(CapSense_CSX_EN) && (CapSense_CSX_EN == CapSense_ENABLE)
static void SendSliderEvent(const Slider_t *slider, SliderState_t state)
{
    uint8_t buff[COMMS_TX_BUFFER_SIZE];
    memset(buff, 0, COMMS_TX_BUFFER_SIZE);
    buff[0] = COMMS_RESPONSE_SLIDER;
    buff[1] = slider->id;
    buff[2] = (uint8_t)(slider->lastPos & 0xFF); // LSB first
    buff[3] = (uint8_t)((slider->lastPos >> 8) & 0xFF);
    buff[4] = (uint8_t)state;
    CommsSendData(buff);
}
#endif

void SlidersScan(void)
{
#if defined(CapSense_CSX_EN) && (CapSense_CSX_EN == CapSense_ENABLE)
    for (volatile uint8_t i = 0; i < sizeof(sliders); i++)
    {
        uint32_t curPos = CapSense_GetCentroidPos(sliders[i].id);

        if (curPos != sliders[i].lastPos)
        {
            // If we did this outside the PSoC we could save a couple bytes in the comms protocol
            SliderState_t state = SLIDER_MOVE; // Default
            // Up
            if (curPos == CapSense_SLIDER_NO_TOUCH)
            {
                state = SLIDER_UP;
            }
            // Down
            else if (sliders[i].lastPos == CapSense_SLIDER_NO_TOUCH)
            {
                state = SLIDER_DOWN;
            }

            sliders[i].lastPos = curPos;

            SendSliderEvent(&sliders[i], state);
        }
    }
#endif
}
