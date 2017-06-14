/**
 * @file button.c
 * @author 
 *
 * 
 */

#include <project.h>
#include "util.h"
#include "button.h"
#include "comms.h"

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

BOOL ButtonsSetup(const uint8_t *buff)
{
    if (buff[1] > MAX_CAPSENSE_BUTTONS)
    {
        return FALSE;
    }
    nButtons = buff[1];
    // We just have to expect they're going to give us proper data
    // We don't really have any good way to validate
    for (uint8_t i = 0; i < nButtons; i++)
    {
        buttons[i].id = buff[1+i];
    }
    return TRUE;
}

static void SendButtonEvent(const Button_t *button)
{
    uint8_t buff[COMMS_TX_BUFFER_SIZE];
    memset(buff, 0, COMMS_TX_BUFFER_SIZE);
    buff[0] = COMMS_RESPONSE_BUTTON;
    buff[1] = button->id;
    buff[2] = button->pressed ? TRUE : FALSE;
    CommsSendData(buff);
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


