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
#include "ProductController.h"
#include "Intents.h"

static DPrint s_logger( "PlaybackRequestManager" );

namespace ProductApp
{
PlaybackRequestManager::PlaybackRequestManager( NotifyTargetTaskIF& task,
                                                const CliClientMT& cliClient,
                                                const FrontDoorClientIF_t& frontDoorClient,
                                                ProductController& controller ):
    IntentManager( task, cliClient, frontDoorClient, controller ),
    m_NowPlayingRsp( std::bind( &PlaybackRequestManager::PostPlaybackRequestCbRsp, this,
                                std::placeholders::_1 ), &task )
{
    m_frontDoorClientErrorCb = AsyncCallback<FRONT_DOOR_CLIENT_ERRORS>
                               ( std::bind( &PlaybackRequestManager::FrontDoorClientErrorCb,
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

bool PlaybackRequestManager::Handle( KeyHandlerUtil::ActionType_t& intent )
{
    if( intent == ( uint16_t ) Action::AUX_IN )
    {
        //If AUX source is already active, ignore the intent.
        if( not( ( GetProductController().GetNowPlaying().has_state() ) &&
                 ( GetProductController().GetNowPlaying().has_container() ) &&
                 ( GetProductController().GetNowPlaying().container().has_contentitem() ) &&
                 ( GetProductController().GetNowPlaying().container().contentitem().source() == "PRODUCT" ) &&
                 ( GetProductController().GetNowPlaying().container().contentitem().sourceaccount() == "AUX" ) )
          )
        {
            SoundTouchInterface::PlaybackRequest playbackRequestData;
            playbackRequestData.set_source( "PRODUCT" );
            playbackRequestData.set_sourceaccount( "AUX" );

            GetFrontDoorClient()->SendPost<SoundTouchInterface::NowPlaying>( "/content/playbackRequest", playbackRequestData,
                                                                             m_NowPlayingRsp, m_frontDoorClientErrorCb );
        }
        else
        {
            BOSE_LOG( INFO, "AUX Source is already active, ignoring the intent" );
        }
    }

    //Fire the cb so the control goes back to the ProductController
    if( GetCallbackObject() != nullptr )
    {
        ( *GetCallbackObject() )( intent );
    }
    return true;
}

void PlaybackRequestManager::PostPlaybackRequestCbRsp( const SoundTouchInterface::NowPlaying& resp )
{
    BOSE_DEBUG( s_logger, "%s", __func__ );
    BOSE_LOG( INFO, "GOT Response to PlaybackRequest: " << resp.source().sourcedisplayname() );
}

} // namespace ProductApp
