///////////////////////////////////////////////////////////////////////////////
/// @file AuxInHandler.cpp
///
/// @brief Handler for AuxIn intent
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
#include "AuxInHandler.h"
#include "ProductController.h"
#include "Intents.h"
#include "ProductSTS.pb.h"
#include "SHELBY_SOURCE.h"

static DPrint s_logger( "AuxInHandler" );

namespace ProductApp
{
AuxInHandler::AuxInHandler( NotifyTargetTaskIF& task,
                                                const CliClientMT& cliClient,
                                                const FrontDoorClientIF_t& frontDoorClient,
                                                ProductController& controller ):
    IntentManager( task, cliClient, frontDoorClient, controller ),
    m_NowPlayingRsp( std::bind( &AuxInHandler::PostPlaybackRequestCbRsp, this,
                                std::placeholders::_1 ), &task )
{
    m_frontDoorClientErrorCb = AsyncCallback<FrontDoor::Error>
                               ( std::bind( &AuxInHandler::FrontDoorClientErrorCb,
                                            this, std::placeholders::_1 ), &task );
}

///////////////////////////////////////////////////////////////////////////////
/// @name  Handle
/// @brief Function to build and send FrontDoor message to activate and play
//         AUX source if it is selected,
//         Else the function ignores the intent.
//         The callBack function is called to give control back to the state
//         machine if HSM has registered a call back.
/// @return true: Successful
//          false: Error
////////////////////////////////////////////////////////////////////////////////

bool AuxInHandler::Handle( KeyHandlerUtil::ActionType_t& intent )
{
    using namespace ProductSTS;

    if( intent == ( uint16_t ) Action::AUX_IN )
    {
        //If AUX source is already active, ignore the intent.
        if( not( ( GetProductController().GetNowPlaying().has_state() ) &&
                 ( GetProductController().GetNowPlaying().has_container() ) &&
                 ( GetProductController().GetNowPlaying().container().has_contentitem() ) &&
                 ( GetProductController().GetNowPlaying().container().contentitem().source() == SHELBY_SOURCE::PRODUCT ) &&
                 ( GetProductController().GetNowPlaying().container().contentitem().sourceaccount() == ProductSourceSlot_Name( AUX ) )
               ) )
        {
            SoundTouchInterface::PlaybackRequest playbackRequestData;
            playbackRequestData.set_source( SHELBY_SOURCE::PRODUCT );
            playbackRequestData.set_sourceaccount( ProductSourceSlot_Name( AUX ) );

            GetFrontDoorClient()->SendPost<SoundTouchInterface::NowPlaying, FrontDoor::Error>( "/content/playbackRequest", playbackRequestData,
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

void AuxInHandler::PostPlaybackRequestCbRsp( const SoundTouchInterface::NowPlaying& resp )
{
    BOSE_DEBUG( s_logger, "%s", __func__ );
    BOSE_LOG( INFO, "GOT Response to PlaybackRequest: " << resp.source().sourcedisplayname() );
}

} // namespace ProductApp
