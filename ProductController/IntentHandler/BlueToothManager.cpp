///////////////////////////////////////////////////////////////////////////////
/// @file BlueToothManager.cpp
///
/// @brief Implementation of Bluetooth Manager for actions from Bluetooth
//         intends in the product Controller
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
#include "BlueToothManager.h"

static DPrint s_logger( "BlueToothManager" );

namespace ProductApp
{
///////////////////////////////////////////////////////////////////////////////
/// @name  Handle
/// @brief Function to build and send FrontDoor message to execute the
//         to Bluetooth intends coming out of the product controller.
//         The callBack function is called to give control back to the state
//         machine if HSM has registered a call back.
/// @return true: Successful
//          false: Error
////////////////////////////////////////////////////////////////////////////////

bool BlueToothManager::Handle( KeyHandlerUtil::ActionType_t intent )
{
#if 0
    if( ValidSourceAvailable() )
    {
        SoundTouchInterface::TransportControl transportControl;
        SoundTouchInterface::StatusJson status = CurrentStatusJson();
        bool sendTransportControlMsg = false;
        switch( intent )
        {
        case( uint16_t ) Action::PLAY_PAUSE:
        {
            if( status == SoundTouchInterface::StatusJson::play )
            {
                // Send Pause
                transportControl.\
                set_state( SoundTouchInterface::TransportControl::pause );
                sendTransportControlMsg = true;
                Pause();
            }
            else if( status == SoundTouchInterface::StatusJson::paused )
            {
                // Send Pause
                transportControl.\
                set_state( SoundTouchInterface::TransportControl::play );
                sendTransportControlMsg = true;
                Play();
            }
            else if( status == SoundTouchInterface::StatusJson::buffering )
            {
                if( TogglePlayPause() )
                {
                    // Send Play
                    transportControl.\
                    set_state( SoundTouchInterface::TransportControl::play );
                }
                else
                {
                    // Send Pause
                    transportControl.\
                    set_state( SoundTouchInterface::TransportControl::pause );
                }
                sendTransportControlMsg = true;
            }
            else
            {
                // Send playbackRequest of source persisted : To do,
                // Talk to Ranjeet: What param in playbackRequest
                // Talk to Vikram: How can nowPlaying that is persisted be used.
                sendTransportControlMsg = true;
            }
        }
        break;

        case( uint16_t ) Action::NEXT_TRACK:
        {
            if( ( status == SoundTouchInterface::StatusJson::play ) ||
                ( status == SoundTouchInterface::StatusJson::paused )  ||
                ( status == SoundTouchInterface::StatusJson::buffering ) )
            {
                // Send NEXT_TRACK
                transportControl.\
                set_state( SoundTouchInterface::TransportControl::skipNext );
                sendTransportControlMsg = true;
            }
        }
        break;
        case( uint16_t ) Action::PREV_TRACK:
        {
            if( ( status == SoundTouchInterface::StatusJson::play ) ||
                ( status == SoundTouchInterface::StatusJson::paused )  ||
                ( status == SoundTouchInterface::StatusJson::buffering ) )
            {
                // Send NEXT_TRACK
                transportControl.\
                set_state( SoundTouchInterface::TransportControl::skipPrevious );
                sendTransportControlMsg = true;
            }
        }
        break;
        }
        if( sendTransportControlMsg )
        {
            BOSE_DEBUG( s_logger, "SendPut through Frontdoor for transportControl "
                        " for intent : %d", intent );

            GetFrontDoorClient()->\
            SendPost<SoundTouchInterface::\
            NowPlayingJson>( "/content/transportControl", transportControl,
                             m_NowPlayingRsp, m_frontDoorClientErrorCb );
        }

    }
    else
    {
        BOSE_DEBUG( s_logger, "No source available, PlayControl intent "
                    "  ignored for now" );
    }
#endif

    //Fire the cb so the control goes back to the ProductController
    if( CallBack() != nullptr )
    {
        ( *CallBack() )( intent );
    }
    return true;
}

#if 0
void BlueToothManager::PutTransportControlCbRsp( const SoundTouchInterface::NowPlayingJson& resp )
{
    // No Need to handle this as Product Controller will get a nowPlaying that
    // will update update the information.
    BOSE_DEBUG( s_logger, "%s", __func__ );
    return;
}
#endif

void BlueToothManager::FrontDoorClientErrorCb( const FRONT_DOOR_CLIENT_ERRORS errorCode )
{
    // Nothing to do for now, printing this if anyone cares.
    BOSE_ERROR( s_logger, "%s:error code- %d", __func__, errorCode );
    return;
}
}
