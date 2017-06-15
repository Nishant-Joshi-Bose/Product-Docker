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

// Automatically determine buttons to scan based on PSoC configuration
void ButtonsInit(void)
{
    memset(buttons, 0, sizeof(buttons));
#ifdef CapSense_BUT0_WDGT_ID
    buttons[nButtons++].id = CapSense_BUT0_WDGT_ID;
#endif
#ifdef CapSense_BUT1_WDGT_ID
    buttons[nButtons++].id = CapSense_BUT1_WDGT_ID;
#endif
#ifdef CapSense_BUT2_WDGT_ID
    buttons[nButtons++].id = CapSense_BUT2_WDGT_ID;
#endif
#ifdef CapSense_BUT3_WDGT_ID
    buttons[nButtons++].id = CapSense_BUT3_WDGT_ID;
#endif
#ifdef CapSense_BUT4_WDGT_ID
    buttons[nButtons++].id = CapSense_BUT4_WDGT_ID;
#endif
#ifdef CapSense_BUT5_WDGT_ID
    buttons[nButtons++].id = CapSense_BUT5_WDGT_ID;
#endif
#ifdef CapSense_BUT6_WDGT_ID
    buttons[nButtons++].id = CapSense_BUT6_WDGT_ID;
#endif
#ifdef CapSense_BUT7_WDGT_ID
    buttons[nButtons++].id = CapSense_BUT7_WDGT_ID;
#endif
#ifdef CapSense_BUT8_WDGT_ID
    buttons[nButtons++].id = CapSense_BUT8_WDGT_ID;
#endif
#ifdef CapSense_BUT9_WDGT_ID
    buttons[nButtons++].id = CapSense_BUT9_WDGT_ID;
#endif
#ifdef CapSense_BUT10_WDGT_ID
    buttons[nButtons++].id = CapSense_BUT10_WDGT_ID;
#endif
#ifdef CapSense_BUT11_WDGT_ID
    buttons[nButtons++].id = CapSense_BUT11_WDGT_ID;
#endif
#ifdef CapSense_BUT12_WDGT_ID
    buttons[nButtons++].id = CapSense_BUT12_WDGT_ID;
#endif
#ifdef CapSense_BUT13_WDGT_ID
    buttons[nButtons++].id = CapSense_BUT13_WDGT_ID;
#endif
#ifdef CapSense_BUT14_WDGT_ID
    buttons[nButtons++].id = CapSense_BUT14_WDGT_ID;
#endif
#ifdef CapSense_BUT15_WDGT_ID
    buttons[nButtons++].id = CapSense_BUT15_WDGT_ID;
#endif
#ifdef CapSense_BUT16_WDGT_ID
    buttons[nButtons++].id = CapSense_BUT16_WDGT_ID;
#endif
#ifdef CapSense_BUT17_WDGT_ID
    buttons[nButtons++].id = CapSense_BUT17_WDGT_ID;
#endif
#ifdef CapSense_BUT18_WDGT_ID
    buttons[nButtons++].id = CapSense_BUT18_WDGT_ID;
#endif
#ifdef CapSense_BUT19_WDGT_ID
    buttons[nButtons++].id = CapSense_BUT19_WDGT_ID;
#endif
#ifdef CapSense_BUT20_WDGT_ID
    buttons[nButtons++].id = CapSense_BUT20_WDGT_ID;
#endif
#ifdef CapSense_BUT21_WDGT_ID
    buttons[nButtons++].id = CapSense_BUT21_WDGT_ID;
#endif
#ifdef CapSense_BUT22_WDGT_ID
    buttons[nButtons++].id = CapSense_BUT22_WDGT_ID;
#endif
#ifdef CapSense_BUT23_WDGT_ID
    buttons[nButtons++].id = CapSense_BUT23_WDGT_ID;
#endif
}

#ifdef CONFIG_VIA_COMMS
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
#endif

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
            SendButtonEvent(&buttons[i]);
        }
    }
}
