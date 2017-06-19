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
#include "led.h"
#include "util.h"
#include "comms.h"

/*
 * Buffer contains 12bit values in bytes
 * nibbleAlsb nibbleAmid, nibbleAmsb nibbleBlsb, nibbleBmid nibbleBmsb, ...
 *  0 aa ab bb  0,1
 *  3 cc cd dd  2,3
 *  6 ee ef ff  4,5
 *  9 gg gh hh  6,7
 * 12 ii ij jj  8,9
 * 15 kk kl ll  10,11
 * 18 mm mn nn  12,13
 * 21 oo op pp  14,15
 * 24 qq qr rr  16,17
 * 27 ss st tt  18,19
 * 30 uu uv vv  20,21
 * 33 ww wx xx  22,23
 */
#define LED_BUFFER_SIZE (MAX_LEDS * 12 / 8)
uint8_t ledBuffer[LED_BUFFER_SIZE];

static void LedsUpdate(void)
{
    CapSense_ISR_Disable();
//    SPIM_SpiUartPutArray((const uint16*)ledBuffer, sizeof(ledBuffer) / 2);
    SPIM_SpiUartPutArray(ledBuffer, sizeof(ledBuffer));
    Blank_Write(1);
    Latch_Write(1);
    CyDelayUs  (1);
    Latch_Write(0);
    Blank_Write(0);
    CapSense_ISR_Enable();
}// update_led_buffer

static void LedsSetLed(uint8_t led, uint16_t ledValue, BOOL update)
{
    uint8_t ledBuffPos = (led/2)*3;
    if (led % 2 == 0)
    {
        // Even led -> aa aX (X is untouched)
        ledBuffer[ledBuffPos] = (ledValue >> 4) & 0xFF;
        ledBuffer[ledBuffPos+1] = (ledBuffer[ledBuffPos+1] & 0x0F) | ((ledValue << 4) & 0xF0);
    }
    else
    {
        // Odd led -> XX Xa aa (X is untouched)
        ledBuffer[ledBuffPos+1] = (ledBuffer[ledBuffPos+1] & 0xF0) | ((ledValue >> 8) & 0x0F);
        ledBuffer[ledBuffPos+2] = ledValue & 0xFF;
    }
    if (update)
    {
        LedsUpdate();
    }
}

static void LedsSetAll(const uint16_t *ledValues)
{
    for (uint8_t i = 0; i < MAX_LEDS; i++)
    {
        LedsSetLed(i, ledValues[i], FALSE);
    }
    LedsUpdate();
}

static void LedsClear(void)
{
    for (uint8_t i = 0; i < MAX_LEDS; i++)
    {
        LedsSetLed(i, 0x00, FALSE);
    }
    LedsUpdate();
}

BOOL LedsHandleCommand(const uint8_t *buff)
{
    if (buff[0] == COMMS_COMMAND_LEDS_CLEARALL)
    {
        LedsClear();
    }
    else if (buff[0] == COMMS_COMMAND_LEDS_SETALL)
    {
        LedsSetAll((uint16_t *)&buff[2]);
    }
    else // COMMS_COMMAND_LEDS_SETONE
    {
        uint16_t ledValue = buff[2] | (buff[3] << 8);
        LedsSetLed(buff[1], ledValue, TRUE);
    }
    return TRUE;
}

void LedsInit(void)
{
    // Datasheet says set this high on bootup or get random leds
    Blank_Write(1);

    SPIM_Start();
    LedsClear();
}
