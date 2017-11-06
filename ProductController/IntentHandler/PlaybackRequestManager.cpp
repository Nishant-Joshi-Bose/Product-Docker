///////////////////////////////////////////////////////////////////////////////
/// @file PlaybackRequestManager.cpp
///
/// @brief Implementation of Playback Request Manager
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
#include "PlaybackRequestManager.h"

static DPrint s_logger( "PlaybackRequestManager" );

namespace ProductApp
{
PlaybackRequestManager::PlaybackRequestManager( NotifyTargetTaskIF& task,
                                                const CliClientMT& cliClient,
                                                const FrontDoorClientIF_t& frontDoorClient,
                                                const ProductController& controller ):
    IntentManager( task, cliClient, frontDoorClient, controller ),
    m_NowPlayingRsp( nullptr, &task )
{
    m_frontDoorClientErrorCb = AsyncCallback<FRONT_DOOR_CLIENT_ERRORS>\
                               ( std::bind( &PlaybackRequestManager::FrontDoorClientErrorCb,
                                            this, std::placeholders::_1 ), &task );

    m_NowPlayingRsp = AsyncCallback<SoundTouchInterface::NowPlayingJson>
                      ( std::bind( &PlaybackRequestManager::PostPlaybackRequestCbRsp,
                                   this, std::placeholders::_1 ), &task );

}

///////////////////////////////////////////////////////////////////////////////
/// @name  Handle
/// @brief Function to build and send FrontDoor message to activate and play
//         source if it is selected,
//         Else the function ignores the intent.
//         The callBack function is called to give control back to the state
//         machine if HSM has registered a call back.
/// @return true: Successful
//          false: Error
////////////////////////////////////////////////////////////////////////////////

bool PlaybackRequestManager::Handle( KeyHandlerUtil::ActionType_t intent )
{
    if( intent == ( uint16_t ) Action::AUX_IN )
    {
        //ToDo: If AUX is currently playing, ignore the key.
        SoundTouchInterface::playbackRequestJson playbackRequestData;
        playbackRequestData.set_source( "PRODUCT" );
        playbackRequestData.set_sourceaccount( "AUX" );

        GetFrontDoorClient()->SendPost<SoundTouchInterface::NowPlayingJson>( "/content/playbackRequest", playbackRequestData,
                                                                             m_NowPlayingRsp, m_frontDoorClientErrorCb );
    }

    //Fire the cb so the control goes back to the ProductController
    if( GetCallbackObject() != nullptr )
    {
        GetCallbackObject()->Send( intent );
    }
    return true;
}

void PlaybackRequestManager::PostPlaybackRequestCbRsp( const SoundTouchInterface::NowPlayingJson& resp )
{
    BOSE_DEBUG( s_logger, "%s", __func__ );
    BOSE_LOG( INFO, "GOT Response to playbackRequest: " << resp.source().sourcedisplayname() );
}

void PlaybackRequestManager::FrontDoorClientErrorCb( const FRONT_DOOR_CLIENT_ERRORS errorCode )
{
    BOSE_ERROR( s_logger, "%s:error code- %d", __func__, errorCode );
}

} // namespace ProductApp
