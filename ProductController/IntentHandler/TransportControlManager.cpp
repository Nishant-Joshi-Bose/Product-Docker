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

static DPrint s_logger( "TransportControlManager" );

namespace ProductApp
{

TransportControlManager::TransportControlManager( NotifyTargetTaskIF& task,
                                                  const CliClientMT& cliClient,
                                                  const FrontDoorClientIF_t& frontDoorClient,
                                                  const ProductController& controller ):
    IntentManager( task, cliClient, frontDoorClient, controller ),
    m_NowPlayingRsp( nullptr, &task ),
    m_play( true )
{
    m_frontDoorClientErrorCb = AsyncCallback<FRONT_DOOR_CLIENT_ERRORS>\
                               ( std::bind( &TransportControlManager::FrontDoorClientErrorCb,
                                            this, std::placeholders::_1 ), &task );

    m_NowPlayingRsp = AsyncCallback<SoundTouchInterface::NowPlayingJson>\
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

bool TransportControlManager::Handle( KeyHandlerUtil::ActionType_t intent )
{
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
                const EddieProductController *eddiePC =
                    dynamic_cast<const EddieProductController*>( &GetProductController() );
                if( eddiePC != nullptr )
                {
                    SoundTouchInterface::playbackRequestJson pbReqJson;
                    SoundTouchInterface::NowPlayingJson nowPlayData = eddiePC->GetNowPlaying();
                    SourceUtils::ConstructPlaybackRequestFromNowPlaying( pbReqJson,
                                                                         nowPlayData );
                    GetFrontDoorClient()->\
                    SendPost<SoundTouchInterface::\
                    NowPlayingJson>( "/content/playbackRequest", pbReqJson,
                                     m_NowPlayingRsp, m_frontDoorClientErrorCb );
                }
                else
                {
                    BOSE_ERROR( s_logger, "Error while casting to Eddie PC" );
                }
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
            SendPut<SoundTouchInterface::\
            NowPlayingJson>( "/content/transportControl", transportControl,
                             m_NowPlayingRsp, m_frontDoorClientErrorCb );
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
        GetCallbackObject()->Send( intent );
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

inline SoundTouchInterface::StatusJson TransportControlManager::CurrentStatusJson()
{
    BOSE_DEBUG( s_logger, "%s", __func__ );
    const EddieProductController *eddiePC =
        dynamic_cast<const EddieProductController*>( &GetProductController() );
    if( eddiePC != nullptr )
    {
        if( eddiePC->GetNowPlaying().state().has_status() &&
            eddiePC->GetNowPlaying().has_state() )
        {
            BOSE_DEBUG( s_logger, "Found status = %d",
                        eddiePC->GetNowPlaying().state().status() );
            return ( eddiePC->GetNowPlaying().state().status() );
        }
        else
        {
            BOSE_DEBUG( s_logger, "No Status in GetNowPlaying()" );
        }
    }
    else
    {
        BOSE_ERROR( s_logger, "Error while casting to Eddie PC" );
    }
    return ( SoundTouchInterface::StatusJson::error );
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
