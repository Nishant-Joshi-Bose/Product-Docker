/**
 * @file slider.c
 * @author 
 *
 * 
 */

#include <project.h>
#include "util.h"
#include "slider.h"
#include "comms.h"

// Only compile slider stuff in if sliders were configured in PSoC creator
#if defined(CapSense_CSX_EN) && (CapSense_CSX_EN == CapSense_ENABLE)

#define MAX_CAPSENSE_SLIDERS 2

typedef struct {
    uint8_t id;
    uint32_t lastPos;
} Slider_t;

static Slider_t sliders[MAX_CAPSENSE_SLIDERS];
static uint8_t nSliders = 0;
#endif

// Automatically configure sliders based on PSoC configuration
void SlidersInit(void)
{
#if defined(CapSense_CSX_EN) && (CapSense_CSX_EN == CapSense_ENABLE)
#ifdef CONFIG_VIA_COMMS
    for (uint8_t i = 0; i < MAX_CAPSENSE_SLIDERS; i++)
    {
        sliders[i].id = 0;
        sliders[i].lastPos = CapSense_SLIDER_NO_TOUCH;
    }
#endif
#ifdef CapSense_LINEARSLIDER0_WDGT_ID
    sliders[nSliders].id = CapSense_LINEARSLIDER0_WDGT_ID;
    sliders[nSliders].lastPos = CapSense_SLIDER_NO_TOUCH;
    nSliders++;
#endif
#ifdef CapSense_LINEARSLIDER1_WDGT_ID
    sliders[nSliders].id = CapSense_LINEARSLIDER1_WDGT_ID;
    sliders[nSliders].lastPos = CapSense_SLIDER_NO_TOUCH;
    nSliders++;
#endif
#endif
}

#ifdef CONFIG_VIA_COMMS
BOOL SlidersSetup(const uint8_t *buff)
{
#if defined(CapSense_CSX_EN) && (CapSense_CSX_EN == CapSense_ENABLE)
    if (buff[1] > MAX_CAPSENSE_SLIDERS)
    {
        return FALSE;
    }
    nSliders = buff[1];
    // We just have to expect they're going to give us proper data
    // We don't really have any good way to validate
    for (uint8_t i = 0; i < nSliders; i++)
    {
        sliders[i].id = buff[1+i];
    }
    return TRUE;
#else
    return FALSE;
#endif
}
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
    for (uint8_t i = 0; i < nSliders; i++)
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
