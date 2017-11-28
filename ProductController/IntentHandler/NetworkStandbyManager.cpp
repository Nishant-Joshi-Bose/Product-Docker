///////////////////////////////////////////////////////////////////////////////
/// @file NetworkStandbyManager.cpp
///
/// @brief Implementation of Transport Control Manager
///
/// @attention
///    BOSE CORPORATION.
///    COPYRIGHT 2017 BOSE CORPORATION ALL RIGHTS RESERVED.
///    This program may not be reproduced, in whole or in part in any
///    form or any means whatsoever without the written permission of:
///        BOSE CORPORATION
///        The Mountain
///        Framingham, MA 01701-9168
///
///////////////////////////////////////////////////////////////////////////////

#include "DPrint.h"
#include "NetworkStandbyManager.h"
#include "SourceUtils.h"

static DPrint s_logger( "NetworkStandbyManager" );

namespace ProductApp
{

NetworkStandbyManager::NetworkStandbyManager( NotifyTargetTaskIF& task,
                                              const CliClientMT& cliClient,
                                              const FrontDoorClientIF_t& frontDoorClient,
                                              EddieProductController& controller ):
    IntentManager( task, cliClient, frontDoorClient, controller )
{
    m_frontDoorClientErrorCb = AsyncCallback<FRONT_DOOR_CLIENT_ERRORS>\
                               ( std::bind( &NetworkStandbyManager::FrontDoorClientErrorCb,
                                            this, std::placeholders::_1 ), &task );
}

///////////////////////////////////////////////////////////////////////////////
/// @name  Handle
/// @brief Function to build and send FrontDoor message to execute the
//         to either play or pause if source is alreay selected,
//         Else the function ignores the intent.
//         The callBack function is called to give control back to the state
//         machine if HSM has registered a call back.
/// @return true: Successful
//          false: Error
////////////////////////////////////////////////////////////////////////////////

bool NetworkStandbyManager::Handle( KeyHandlerUtil::ActionType_t& intent )
{
    BOSE_DEBUG( s_logger, "%s", __func__ );
    if( ValidSourceAvailable() )
    {
        {
            BOSE_DEBUG( s_logger, "SendPut through Frontdoor for transportControl "
                        " for intent : %d", intent );

            GetFrontDoorClient()->\
            SendGet<SoundTouchInterface::\
            status>( "/content/stopPlayback", {}, m_frontDoorClientErrorCb );
        }
    }
    else
    {
        BOSE_DEBUG( s_logger, "No source available, PlayControl intent "
                    "  ignored for now" );
    }

    //Fire the cb so the control goes back to the ProductController
    if( GetCallbackObject() != nullptr )
    {
        ( *GetCallbackObject() )( intent );
    }
    return true;
}

}
