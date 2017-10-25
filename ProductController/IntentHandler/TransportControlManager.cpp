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

bool TransportControlManager::Handle( KeyHandlerUtil::ActionType_t intent )
{
    SoundTouchInterface::TransportControl transportControl;
    if( intent == ( uint16_t ) Action::PLAY_PAUSE )
    {
        if( ValidSourceAvailable() )
        {
            if( CurrentlyPlaying() )
            {
                // Send Pause
                transportControl.\
                  set_state( SoundTouchInterface::TransportControl::pause );
            }
            else
            {
                // Send Play
                transportControl.\
                  set_state( SoundTouchInterface::TransportControl::play );
            }
            GetFrontDoorClient()->\
              SendPut<SoundTouchInterface::\
              NowPlayingJson>( "/content/transportControl", transportControl,
                               m_NowPlayingRsp, m_frontDoorClientErrorCb );
        }
        else
        {
            BOSE_DEBUG( s_logger, "No source available, play_pause intent is"
                        "  ignored for now" );
        }
    }

    //Fire the cb so the control goes back to the ProductController
    if( CallBack() != nullptr )
    {
        ( *CallBack() )( intent );
    }
    return true;
}

inline bool TransportControlManager::ValidSourceAvailable()
{
    BOSE_DEBUG( s_logger, "%s", __func__ );
    const EddieProductController *eddiePC = \
        dynamic_cast<const EddieProductController*>( &GetProductController() );
    if( eddiePC != nullptr )
    {
        if( eddiePC->GetNowPlaying().has_source() )
        {
            BOSE_DEBUG( s_logger, "Found nowPlaying" );
            return true;
        }
    }
    else
    {
        BOSE_ERROR( s_logger, "Error while casting to Eddie PC" );
    }
    return false;
}

inline bool TransportControlManager::CurrentlyPlaying()
{
    BOSE_DEBUG( s_logger, "%s", __func__ );
    const EddieProductController *eddiePC = \
        dynamic_cast<const EddieProductController*>( &GetProductController() );
    if( eddiePC != nullptr )
    {
        if( eddiePC->GetNowPlaying().state().has_status() && 
            eddiePC->GetNowPlaying().state().status() == SoundTouchInterface::StatusJson::play)
        {
            BOSE_DEBUG( s_logger, "Found nowPlaying" );
            return true;
        }
    }
    else
    {
        BOSE_ERROR( s_logger, "Error while casting to Eddie PC" );
    }
    return false;
}


void TransportControlManager::PutTransportControlCbRsp( const SoundTouchInterface::NowPlayingJson& resp )
{
    // No Need to handle this as Product Controller will get a nowPlaying that
    // will update update the information.
    BOSE_DEBUG( s_logger, "%s", __func__ );
    return;
}

void TransportControlManager::FrontDoorClientErrorCb( const FRONT_DOOR_CLIENT_ERRORS errorCode )
{
    // Nothing to do for now, printing this if anyone cares.
    BOSE_ERROR( s_logger, "%s:error code- %d", __func__, errorCode );
    return;
}

}
