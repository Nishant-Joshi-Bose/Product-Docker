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
#include "SourceUtils.h"
#include "ProductController.h"
#include "Intents.h"

static DPrint s_logger( "TransportControlManager" );

namespace ProductApp
{

TransportControlManager::TransportControlManager( NotifyTargetTaskIF& task,
                                                  const CliClientMT& cliClient,
                                                  const FrontDoorClientIF_t& frontDoorClient,
                                                  ProductController& controller ):
    IntentManager( task, cliClient, frontDoorClient, controller ),
    m_NowPlayingRsp( nullptr, &task ),
    m_play( true )
{
    m_frontDoorClientErrorCb = AsyncCallback<FRONT_DOOR_CLIENT_ERRORS>
                               ( std::bind( &TransportControlManager::FrontDoorClientErrorCb,
                                            this, std::placeholders::_1 ), &task );

    m_NowPlayingRsp = AsyncCallback<SoundTouchInterface::NowPlayingJson>
                      ( std::bind( &TransportControlManager::PutTransportControlCbRsp,
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

bool TransportControlManager::Handle( KeyHandlerUtil::ActionType_t& intent )
{
    if( ValidSourceAvailable() )
    {
        SoundTouchInterface::TransportControl transportControl;
        SoundTouchInterface::StatusJson status = CurrentNowPlayingStatusJson();
        bool sendTransportControlMsg = false;

        switch( intent )
        {
        case( uint16_t ) Action::PLAY_PAUSE:
        {
            if( status == SoundTouchInterface::StatusJson::play )
            {
                if( CanPauseInJson() )
                {
                    transportControl.set_state( SoundTouchInterface::TransportControl::pause );
                }
                else
                {
                    transportControl.set_state( SoundTouchInterface::TransportControl::stop );
                }
                sendTransportControlMsg = true;
                Pause();
            }
            else if( ( status == SoundTouchInterface::StatusJson::paused ) ||
                     ( status == SoundTouchInterface::StatusJson::stopped ) )
            {
                transportControl.set_state( SoundTouchInterface::TransportControl::play );
                sendTransportControlMsg = true;
                Play();
            }
            else if( status == SoundTouchInterface::StatusJson::buffering )
            {
                if( TogglePlayPause() )
                {
                    transportControl.set_state( SoundTouchInterface::TransportControl::play );
                }
                else
                {
                    transportControl.set_state( SoundTouchInterface::TransportControl::pause );
                }
                sendTransportControlMsg = true;
            }
            else
            {
                SoundTouchInterface::playbackRequestJson pbReqJson;
                SoundTouchInterface::NowPlayingJson nowPlayData = GetProductController().GetNowPlaying();
                SourceUtils::ConstructPlaybackRequestFromNowPlaying( pbReqJson, nowPlayData );
                GetFrontDoorClient()->SendPost<SoundTouchInterface::NowPlayingJson>
                ( "/content/playbackRequest", pbReqJson, m_NowPlayingRsp, m_frontDoorClientErrorCb );
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
                transportControl.set_state( SoundTouchInterface::TransportControl::skipNext );
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
                transportControl.set_state( SoundTouchInterface::TransportControl::skipPrevious );
                sendTransportControlMsg = true;
            }
        }
        break;
        }
        if( sendTransportControlMsg )
        {
            BOSE_DEBUG( s_logger, "SendPut through Frontdoor for transportControl for intent : %d", intent );

            GetFrontDoorClient()->SendPut<SoundTouchInterface::NowPlayingJson>
            ( "/content/transportControl", transportControl, m_NowPlayingRsp, m_frontDoorClientErrorCb );
        }

    }
    else
    {
        BOSE_DEBUG( s_logger, "No source available, PlayControl intent ignored for now" );
    }

    //Fire the cb so the control goes back to the ProductController
    if( GetCallbackObject() != nullptr )
    {
        ( *GetCallbackObject() )( intent );
    }
    return true;
}

inline bool TransportControlManager::CanPauseInJson()
{
    BOSE_DEBUG( s_logger, "%s", __func__ );
    if( GetProductController().GetNowPlaying().has_source() &&
        GetProductController().GetNowPlaying().has_state() &&
        GetProductController().GetNowPlaying().state().canpause() )
    {
        BOSE_DEBUG( s_logger, "Found canpause = %d",
                    GetProductController().GetNowPlaying().state().canpause() );
        return ( GetProductController().GetNowPlaying().state().canpause() );
    }
    else
    {
        BOSE_DEBUG( s_logger, "Cannot Pause: Must be a non-pausable (it that is a word) content" );
    }
    return ( false );
}

void TransportControlManager::PutTransportControlCbRsp( const SoundTouchInterface::NowPlayingJson& resp )
{
    // No Need to handle this as Product Controller will get a nowPlaying that
    // will update update the information.
    BOSE_DEBUG( s_logger, "%s", __func__ );
    return;
}

}
