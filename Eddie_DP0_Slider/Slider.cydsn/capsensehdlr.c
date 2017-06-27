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
#include "slider.h"

void CapsenseHandlerInit(void)
{
    CapSense_Start();
}

void CapsenseHandlerScan(void)
{
    if (CapSense_IsBusy() != CapSense_NOT_BUSY)
    {
        return;
    }

    // process data for previous scan and initiate new scan only when the capsense hardware is idle
    if (CapSense_ProcessAllWidgets() == CYRET_INVALID_STATE)
    {
        return;
    }

    ButtonsScan();
    SlidersScan();

#if USE_TUNER
    CapSense_RunTuner(); // sync capsense parameters via tuner before the beginning of new capsense scan
#endif // USE_TUNER

   CapSense_ScanAllWidgets();
}
