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
#include "ProductController.h"
#include "Intents.h"
#include "EndPointsDefines.h"
#include "ProductSourceInfo.h"

using namespace ProductPb;

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
      m_ProductController( productController )
{
    BOSE_INFO( s_logger, "%s is being constructed.", __func__ );

    auto handleSources = [ this ]( const SoundTouchInterface::Sources & sources )
    {
        UpdateSources( sources );
    };
    m_ProductController.GetSourceInfo().RegisterSourceListener( handleSources );
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
        if( m_gameSourcePlaybackRq.has_source() )
        {
            GetFrontDoorClient( )->SendPost<SoundTouchInterface::NowPlaying, FrontDoor::Error>( FRONTDOOR_CONTENT_PLAYBACKREQUEST_API,
                    m_gameSourcePlaybackRq,
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
        if( m_dvdSourcePlaybackRq.has_source() )
        {
            GetFrontDoorClient( )->SendPost<SoundTouchInterface::NowPlaying, FrontDoor::Error>( FRONTDOOR_CONTENT_PLAYBACKREQUEST_API,
                    m_dvdSourcePlaybackRq,
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
        if( m_cablesatSourcePlaybackRq.has_source() )
        {
            GetFrontDoorClient( )->SendPost<SoundTouchInterface::NowPlaying, FrontDoor::Error>( FRONTDOOR_CONTENT_PLAYBACKREQUEST_API,
                    m_cablesatSourcePlaybackRq,
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
///
/// @brief PlaybackRequestManager::UpdateSources
///
/// @param  const SoundTouchInterface::Sources&
///
/// @return This method returns true base on its handling of a playback request.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void PlaybackRequestManager::UpdateSources( const SoundTouchInterface::Sources& sources )
{
    BOSE_INFO( s_logger, "%s got source update %s", __func__, ProtoToMarkup::ToJson( sources ).c_str() );
    // Repopulate the playbackRequest info associated with user configurable Game, DVD, Cable/Sat activation key
    m_gameSourcePlaybackRq.Clear();
    m_dvdSourcePlaybackRq.Clear();
    m_cablesatSourcePlaybackRq.Clear();
    for( auto i = 0 ; i < sources.sources_size(); i++ )
    {
        const auto& source = sources.sources( i );
        const auto& activationKey = source.details().activationkey();
        if( activationKey ==  "ACTIVATION_KEY_GAME" )
        {
            m_gameSourcePlaybackRq.set_source( source.sourcename() );
            m_gameSourcePlaybackRq.set_sourceaccount( source.sourceaccountname() );
        }
        else if( activationKey == "ACTIVATION_KEY_BD_DVD" )
        {
            m_dvdSourcePlaybackRq.set_source( source.sourcename() );
            m_dvdSourcePlaybackRq.set_sourceaccount( source.sourceaccountname() );
        }
        else if( activationKey == "ACTIVATION_KEY_CBL_SAT" )
        {
            m_cablesatSourcePlaybackRq.set_source( source.sourcename() );
            m_cablesatSourcePlaybackRq.set_sourceaccount( source.sourceaccountname() );
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                           End of the Product Application Namespace                           ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
