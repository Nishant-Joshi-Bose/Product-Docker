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
#include "led.h"
#include "util.h"
#include "comms.h"

/*
 * The TLC5947 comms protocol is:
 * Blank turns off the pwm when ON and should be set to on at the start of your program
 * LED intensity data is sent as a stream of 24 12-bit values msb..lsb
 * Set Latch ON before writing and turn it OFF when done
 * This is basically just bit-banged SPI with chip select
 * See the TODO below about using CS as Latch
 *
 * The external API allows passing the 12-bit intensities as 16-bit and converts them internally
 * to make it easier for the outside to send data.  (i.e. instead of every client of the API
 * having to have code that converts to 12-bit and send that somehow).
 */

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
static uint8_t ledBuffer[LED_BUFFER_SIZE];

// TODO see if we can swap blank and latch pins and use spi cs as latch; it will remove the need to turn off interrupts
static void LedsUpdate(void)
{
    CapSense_ISR_Disable();
    SPIM_SpiUartPutArray(ledBuffer, sizeof(ledBuffer));
    Blank_Write(1);
    Latch_Write(1);
    CyDelayUs  (1);
    Latch_Write(0);
    Blank_Write(0);
    CapSense_ISR_Enable();
}

// Used by the animator
void LedsShowPattern(uint8_t *pattern)
{
    memcpy(ledBuffer, pattern, LED_BUFFER_SIZE);
    LedsUpdate();
}

// Set one LED
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

// Set the entire set all at once with 24 different intensities
static void LedsSetAll(const uint16_t *ledValues)
{
    for (uint8_t i = 0; i < MAX_LEDS; i++)
    {
        LedsSetLed(i, ledValues[i], FALSE);
    }
    LedsUpdate();
}

// Syntactic sugar; it's so sweet
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
