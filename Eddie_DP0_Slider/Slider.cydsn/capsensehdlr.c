/*
 * capsensehdlr.c
 *
 *  Created on: Jun 13, 2017
 *      Author: mm1005301
 */

#include <project.h>
#include "util.h"
#include "button.h"
#include "slider.h"

void CapsenseHandlerInit(void)
{
    CapSense_Start();
    ButtonsInit();
    SlidersInit();
}

void CapsenseHandlerScan(void)
{
    if (CapSense_IsBusy() != CapSense_NOT_BUSY)
    {
        continue;
    }

    // process data for previous scan and initiate new scan only when the capsense hardware is idle
    if (CapSense_ProcessAllWidgets() == CYRET_INVALID_STATE)
    {
        continue;
    }

    ButtonsScan();
    SlidersScan();

#if USE_TUNER
    CapSense_RunTuner(); // sync capsense parameters via tuner before the beginning of new capsense scan
#endif // USE_TUNER

   CapSense_ScanAllWidgets();
}

