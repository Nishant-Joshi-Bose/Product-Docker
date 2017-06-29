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
#include "button.h"
#include "comms.h"

typedef struct {
    uint8_t id;
    uint32_t pressed;
} Button_t;

// Automatically determine buttons to scan based on PSoC configuration
// We currently only support 24 max
static Button_t buttons[] =
{
#ifdef CapSense_BUT0_WDGT_ID
        {CapSense_BUT0_WDGT_ID, 0},
#endif
#ifdef CapSense_BUT1_WDGT_ID
        {CapSense_BUT1_WDGT_ID, 0},
#endif
#ifdef CapSense_BUT2_WDGT_ID
        {CapSense_BUT2_WDGT_ID, 0},
#endif
#ifdef CapSense_BUT3_WDGT_ID
        {CapSense_BUT3_WDGT_ID, 0},
#endif
#ifdef CapSense_BUT4_WDGT_ID
        {CapSense_BUT4_WDGT_ID, 0},
#endif
#ifdef CapSense_BUT5_WDGT_ID
        {CapSense_BUT5_WDGT_ID, 0},
#endif
#ifdef CapSense_BUT6_WDGT_ID
        {CapSense_BUT6_WDGT_ID, 0},
#endif
#ifdef CapSense_BUT7_WDGT_ID
        {CapSense_BUT7_WDGT_ID, 0},
#endif
#ifdef CapSense_BUT8_WDGT_ID
        {CapSense_BUT8_WDGT_ID, 0},
#endif
#ifdef CapSense_BUT9_WDGT_ID
        {CapSense_BUT9_WDGT_ID, 0},
#endif
#ifdef CapSense_BUT10_WDGT_ID
        {CapSense_BUT10_WDGT_ID, 0},
#endif
#ifdef CapSense_BUT11_WDGT_ID
        {CapSense_BUT11_WDGT_ID, 0},
#endif
#ifdef CapSense_BUT12_WDGT_ID
        {CapSense_BUT12_WDGT_ID, 0},
#endif
#ifdef CapSense_BUT13_WDGT_ID
        {CapSense_BUT13_WDGT_ID, 0},
#endif
#ifdef CapSense_BUT14_WDGT_ID
        {CapSense_BUT14_WDGT_ID, 0},
#endif
#ifdef CapSense_BUT15_WDGT_ID
        {CapSense_BUT15_WDGT_ID, 0},
#endif
#ifdef CapSense_BUT16_WDGT_ID
        {CapSense_BUT16_WDGT_ID, 0},
#endif
#ifdef CapSense_BUT17_WDGT_ID
        {CapSense_BUT17_WDGT_ID, 0},
#endif
#ifdef CapSense_BUT18_WDGT_ID
        {CapSense_BUT18_WDGT_ID, 0},
#endif
#ifdef CapSense_BUT19_WDGT_ID
        {CapSense_BUT19_WDGT_ID, 0},
#endif
#ifdef CapSense_BUT20_WDGT_ID
        {CapSense_BUT20_WDGT_ID, 0},
#endif
#ifdef CapSense_BUT21_WDGT_ID
        {CapSense_BUT21_WDGT_ID, 0},
#endif
#ifdef CapSense_BUT22_WDGT_ID
        {CapSense_BUT22_WDGT_ID, 0},
#endif
#ifdef CapSense_BUT23_WDGT_ID
        {CapSense_BUT23_WDGT_ID, 0},
#endif
};

void SendButtonEvent(uint8_t buttonId, uint8_t pressed)
{
    uint8_t buff[COMMS_TX_BUFFER_SIZE];
    memset(buff, 0, COMMS_TX_BUFFER_SIZE);
    buff[0] = COMMS_RESPONSE_BUTTON;
    buff[1] = buttonId;
    buff[2] = pressed ? TRUE : FALSE;
    CommsSendData(buff);
}

void ButtonsScan(void)
{
    for (volatile uint8_t i = 0; i < sizeof(buttons) / sizeof(Button_t); i++)
    {
        uint32_t buttonActive = CapSense_IsWidgetActive(buttons[i].id);
        if (buttonActive != buttons[i].pressed)
        {
            buttons[i].pressed = buttonActive;
            SendButtonEvent(buttons[i].id, buttons[i].pressed);
        }
    }
}
