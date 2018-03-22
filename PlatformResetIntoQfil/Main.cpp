#include <stdlib.h>
#include <iostream>
#include <unistd.h>
#include "DPrint.h"
#include "LpmClientFactory.h"
#include "APTaskFactory.h"
#include "SystemUtils.h"

static DPrint s_logger{ "PlatformResetIntoQfil" };

int main()
{
    BOSE_CRITICAL( s_logger, "Connecting to LPMService" );

    sync();

    auto task = IL::CreateTask( "PlatformResetIntoQfilTask" );

    auto lpmClient = LpmClientFactory::Create( "PlatformResetIntoQfilLpmClient", task );

    lpmClient->Connect( [lpmClient]( bool connected )
    {
        if( !connected )
        {
            BOSE_CRITICAL( s_logger, "lpmClient not connected" );
            return;
        }
        BOSE_CRITICAL( s_logger, "Requesting reboot APQ into QFIL mode" );

        IpcRebootRequest_t req{};
        req.set_type( REBOOT_TYPE_QFIL );
        lpmClient->Reboot( req );

        BOSE_CRITICAL( s_logger, "Reboot APQ into QFIL mode request sent" );
    } );

    BOSE_CRITICAL( s_logger, "Waiting for LPM to reboot APQ into QFIL mode" );
    sleep( 10 );
}
