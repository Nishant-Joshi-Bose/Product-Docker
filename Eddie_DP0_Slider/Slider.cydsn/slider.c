/**
 * @file slider.c
 * @author 
 *
 * 
 */

#include <project.h>
#include "util.h"
#include "slider.h"

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

void SlidersInit(void)
{
#if defined(CapSense_CSX_EN) && (CapSense_CSX_EN == CapSense_ENABLE)
    for (uint8_t i = 0; i < MAX_CAPSENSE_SLIDERS; i++)
    {
        sliders[i].id = 0;
        sliders[i].lastPos = CapSense_SLIDER_NO_TOUCH;
    }
#endif
}

BOOL SlidersSetup(const uint8_t *buff)
{
#if defined(CapSense_CSX_EN) && (CapSense_CSX_EN == CapSense_ENABLE)
    return FALSE;
#else
    return FALSE;
#endif
}

#if defined(CapSense_CSX_EN) && (CapSense_CSX_EN == CapSense_ENABLE)
static void SendSliderEvent(const Slider_t *slider, SliderState_t state)
{

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

            SendSliderEvent(&sliders[i], state);

            sliders[i].lastPos = curPos;
        }
    }
#endif
}


