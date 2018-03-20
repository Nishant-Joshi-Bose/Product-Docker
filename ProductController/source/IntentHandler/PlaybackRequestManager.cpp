////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file      PlaybackRequestManager.cpp
///
/// @brief     This source code file contains functionality to implement an intent manager class for
///            starting playbacks for Professor product specific source selection key actions,
///            typically based on remote key actions for TV and SoundTouch sources.
///
/// @author    Stuart J. Lumby
///
/// @attention Copyright (C) 2017 Bose Corporation All Rights Reserved
///
///            Bose Corporation
///            The Mountain Road,
///            Framingham, MA 01701-9168
///            U.S.A.
///
///            This program may not be reproduced, in whole or in part, in any form by any means
///            whatsoever without the written permission of Bose Corporation.
///
////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////
///
///            Included Header Files
///
////////////////////////////////////////////////////////////////////////////////////////////////////
#include "Utilities.h"
#include "PlaybackRequestManager.h"
#include "ProfessorProductController.h"
#include "Intents.h"
#include "EndPointsDefines.h"
#include "ProductSourceInfo.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
///                            Start of Product Application Namespace                            ///
////////////////////////////////////////////////////////////////////////////////////////////////////
namespace ProductApp
{

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief PlaybackRequestManager::PlaybackRequestManager
///
/// @param NotifyTargetTaskIF&        task
///
/// @param const CliClientMT&         commandLineClient
///
/// @param const FrontDoorClientIF_t& frontDoorClient
///
/// @param ProductController&         productController
///
////////////////////////////////////////////////////////////////////////////////////////////////////
PlaybackRequestManager::PlaybackRequestManager( NotifyTargetTaskIF&         task,
                                                const CliClientMT&          commandLineClient,
                                                const FrontDoorClientIF_t&  frontDoorClient,
                                                ProductController&          productController )

    : IntentManager( task, commandLineClient, frontDoorClient, productController ),
      m_CustomProductController( static_cast< ProfessorProductController & >( productController ) )
{
    BOSE_INFO( s_logger, "%s is being constructed.", "PlaybackRequestManager" );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief  PlaybackRequestManager::Handle
///
/// @brief  This method is used to build and send Front Door messages to activate and play a source
///         if it is selected.
///
/// @param  KeyHandlerUtil::ActionType_t& action
///
/// @return This method returns true base on its handling of a playback request.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool PlaybackRequestManager::Handle( KeyHandlerUtil::ActionType_t& action )
{
    BOSE_INFO( s_logger, "%s in %s", "PlaybackRequestManager", __FUNCTION__ );

    auto playbackRequestResponseCallback = [ this ]( const SoundTouchInterface::NowPlaying & response )
    {
        BOSE_DEBUG( s_logger, "A response to the playback request was received: %s" ,
                    ProtoToMarkup::ToJson( response, false ).c_str( ) );
    };

    auto playbackRequestErrorCallback = [ this ]( const FrontDoor::Error & error )
    {
        BOSE_ERROR( s_logger, "An error code %d subcode %d and error string <%s> was returned from a playback request.",
                    error.code(),
                    error.subcode(),
                    error.message().c_str() );
    };

    if( action == ( uint16_t )Action::ACTION_TV )
    {
        SoundTouchInterface::PlaybackRequest playbackRequestData;

        playbackRequestData.set_source( "PRODUCT" );
        playbackRequestData.set_sourceaccount( "TV" );

        GetFrontDoorClient( )->SendPost<SoundTouchInterface::NowPlaying, FrontDoor::Error>( FRONTDOOR_CONTENT_PLAYBACKREQUEST_API,
                playbackRequestData,
                playbackRequestResponseCallback,
                playbackRequestErrorCallback );

        BOSE_INFO( s_logger, "An attempt to play the TV source has been made." );
    }
    else if( action == ( uint16_t )Action::ACTION_GAME )
    {
        auto& playbackRequestData = m_CustomProductController.GetSourceInfo()->GetGameSourcePlaybackRq();

        if( playbackRequestData.has_source() )
        {
            GetFrontDoorClient( )->SendPost<SoundTouchInterface::NowPlaying, FrontDoor::Error>( FRONTDOOR_CONTENT_PLAYBACKREQUEST_API,
                    playbackRequestData,
                    playbackRequestResponseCallback,
                    playbackRequestErrorCallback );
            BOSE_INFO( s_logger, "An attempt to play the Game source has been made." );
        }
        else
        {
            BOSE_INFO( s_logger, "Game key is not configured to play any source, ignore this playback intent." );
        }
    }
    else if( action == ( uint16_t )Action::ACTION_DVD )
    {
        auto& playbackRequestData = m_CustomProductController.GetSourceInfo()->GetDvdSourcePlaybackRq();

        if( playbackRequestData.has_source() )
        {
            GetFrontDoorClient( )->SendPost<SoundTouchInterface::NowPlaying, FrontDoor::Error>( FRONTDOOR_CONTENT_PLAYBACKREQUEST_API,
                    playbackRequestData,
                    playbackRequestResponseCallback,
                    playbackRequestErrorCallback );
            BOSE_INFO( s_logger, "An attempt to play the Dvd source has been made." );
        }
        else
        {
            BOSE_INFO( s_logger, "DVD key is not configured to play any source, ignore this playback intent." );
        }
    }
    else if( action == ( uint16_t )Action::ACTION_CABLESAT )
    {
        auto& playbackRequestData = m_CustomProductController.GetSourceInfo()->GetCablesatSourcePlaybackRq();

        if( playbackRequestData.has_source() )
        {
            GetFrontDoorClient( )->SendPost<SoundTouchInterface::NowPlaying, FrontDoor::Error>( FRONTDOOR_CONTENT_PLAYBACKREQUEST_API,
                    playbackRequestData,
                    playbackRequestResponseCallback,
                    playbackRequestErrorCallback );
            BOSE_INFO( s_logger, "An attempt to play the user configurable Cable/Sat source has been made." );
        }
        else
        {
            BOSE_INFO( s_logger, "Cable/Sat key is not configured to play any source, ignore this playback intent." );
        }
    }
    else if( action == ( uint16_t )Action::ACTION_APAPTIQ_START )
    {
        SoundTouchInterface::PlaybackRequest playbackRequestData;

        playbackRequestData.set_source( "PRODUCT" );
        playbackRequestData.set_sourceaccount( "ADAPTiQ" );

        GetFrontDoorClient( )->SendPost<SoundTouchInterface::NowPlaying, FrontDoor::Error>( FRONTDOOR_CONTENT_PLAYBACKREQUEST_API,
                playbackRequestData,
                playbackRequestResponseCallback,
                playbackRequestErrorCallback );

        BOSE_INFO( s_logger, "An attempt to start AdaptIQ has been made." );
    }
    else
    {
        BOSE_ERROR( s_logger, "An invalid intent action has been supplied." );
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                           End of the Product Application Namespace                           ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
