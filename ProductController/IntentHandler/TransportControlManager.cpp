///////////////////////////////////////////////////////////////////////////////
/// @file TransportControlManager.cpp
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
#include "TransportControlManager.h"

static DPrint s_logger( "TransportControlManager" );

namespace ProductApp
{

bool TransportControlManager::Handle( KeyHandlerUtil::ActionType_t intent )
{
    // This function will build and send message through FrontDoor depending
    // on whethere the product controller is currently playing or paused.
    // If no source is selected ignoring the intent for play_pause
    // Return control to ProductController through callback if registered.

    SoundTouchInterface::TransportControl data;
    if( intent == ( uint16_t ) Action::PLAY_PAUSE )
    {
        if( ValidSourceAvailable() )
        {
            if( CurrentlyPlaying() )
            {
                // Send Pause
                data.set_state( SoundTouchInterface::TransportControl::pause );
            }
            else
            {
                // Send Play
                data.set_state( SoundTouchInterface::TransportControl::play );
            }
            m_frontDoorClient->SendPut<SoundTouchInterface::NowPlayingJson>( "/content/transportControl", data, m_NowPlayingJsonRsp, errorCb() );
        }
        else
        {
            BOSE_DEBUG( s_logger, "No source available, play_pause intent is"
                        "  ignored for now" );
        }
    }

    //Fire the cb so the control goes back to the ProductController
    if( cb() != nullptr )
    {
        ( *cb() )( intent );
    }
    return true;
}

bool TransportControlManager::ValidSourceAvailable()
{
    BOSE_DEBUG( s_logger, "%s", __func__ );
    return false;
}

bool TransportControlManager::CurrentlyPlaying()
{
    BOSE_DEBUG( s_logger, "%s", __func__ );
    return false;
}

void TransportControlManager::PutTransportControlCbRsp( const SoundTouchInterface::NowPlayingJson& resp )
{
    BOSE_DEBUG( s_logger, "%s", __func__ );
//  BOSE_LOG( INFO, "GOT Response " << resp.source().sourcedisplayname() );
}

void TransportControlManager::CallBackError( const FRONT_DOOR_CLIENT_ERRORS errorCode )
{
    BOSE_ERROR( s_logger, "%s:error code- %d", __func__, errorCode );
}

}
