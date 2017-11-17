#include <stdlib.h>
#include <iostream>
#include "DPrint.h"

int main()
{
    /* Placeholder. Will eventually be replaced with code to ask the LPM to
       reboot us. */
    DPrint logger{ "PlatformReset" };
    BOSE_CRITICAL( logger, "Requesting reboot" );
    system( "sync; reboot" );
}
