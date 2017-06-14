/**
 * @file button.c
 * @author 
 *
 * 
 */

#include <project.h>
#include "util.h"
#include "button.h"

#define MAX_CAPSENSE_BUTTONS 24

typedef struct {
    uint8_t id;
    uint32_t pressed;
} Button_t;

static Button_t buttons[MAX_CAPSENSE_BUTTONS];
static uint8_t nButtons = 0;

void ButtonsInit(void)
{
    memset(buttons, 0, sizeof(buttons));
}

static void SendButtonEvent(const Button_t *button)
{

}

void ButtonsScan(void)
{
    for (uint8_t i = 0; i < nButtons; i++)
    {
        uint32_t buttonActive = CapSense_IsWidgetActive(buttons[i].id);
        if (buttonActive != buttons[i].pressed)
        {
            buttons[i].pressed = buttonActive;
            SendButtonEvent((const)&buttons[i]);
        }
    }
}


