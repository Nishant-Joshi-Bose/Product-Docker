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

#include "util.h"

static uint8_t   software_version_major       = 0x01;
static uint8_t   software_version_minor       = 0x00;

uint8_t get_software_version_major(void)
{
    return software_version_major;
}

uint8_t get_software_version_minor(void)
{
    return software_version_minor;
}

#ifndef NDEBUG
// Funky 1-wire debug thingy
#define DELAYZ 158 // 60us on this chip
#define DelayZ { delayz=DELAYZ; while (delayz--); }
void DebugTell(uint16_t data)
{
    volatile uint8_t delayz;
    uint32_t interrupts = CyEnterCriticalSection();
    uint8_t bit = 0;
    while (bit++ < 16)
    {
        DebugPin_Write(0);
//        CyDelayUs(60);
        DelayZ;
        if (!(data & 0x8000))
        {
            DebugPin_Write(1);
        }
//        CyDelayUs(60);
        DelayZ;
        DebugPin_Write(1);
//        CyDelayUs(60);
        DelayZ;
        data <<= 1;
    }
    CyExitCriticalSection(interrupts);
}
#endif