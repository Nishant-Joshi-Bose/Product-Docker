#include <stdlib.h>
#include <iostream>
#include <unistd.h>
#include "DPrint.h"
#include "LpmClientFactory.h"
#include "APTaskFactory.h"
#include "SystemUtils.h"

static DPrint s_logger{ "PlatformReset" };

int main()
{
    BOSE_CRITICAL( s_logger, "Connecting" );

    auto task = IL::CreateTask( "PlatformResetTask" );

    auto lpmClient = LpmClientFactory::Create( "PlatformResetLpmClient", task );

    lpmClient->Connect( [lpmClient]( bool connected )
    {
        if( !connected )
        {
            BOSE_CRITICAL( s_logger, "lpmClient not connected" );
            return;
        }
        BOSE_CRITICAL( s_logger, "Requesting reboot" );

        IpcRebootRequest_t req{};
        req.set_type( REBOOT_TYPE_SYSTEM );
        lpmClient->Reboot( req );

        BOSE_CRITICAL( s_logger, "Reboot request sent" );
    } );

    BOSE_CRITICAL( s_logger, "Waiting for LPM reboot" );
    sleep( 30 );
    BOSE_CRITICAL( s_logger, "LPM didn't reboot, asking APQ to reboot" );
    SystemUtils::Spawn( { "/sbin/reboot" } );
}
