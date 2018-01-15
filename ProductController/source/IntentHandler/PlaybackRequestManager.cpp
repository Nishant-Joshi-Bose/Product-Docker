﻿////////////////////////////////////////////////////////////////////////////////////////////////////
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

    if( action == ( uint16_t )Action::ACTION_TV )
    {
        SoundTouchInterface::playbackRequestJson playbackRequestData;
        playbackRequestData.set_source( "PRODUCT" );
        playbackRequestData.set_sourceaccount( "TV" );

        AsyncCallback< SoundTouchInterface::NowPlayingJson >
        postPlaybackRequestResponseCallback( std::bind( &PlaybackRequestManager::PostPlaybackRequestResponse,
                                                        this, std::placeholders::_1 ),
                                             &GetTask( ) );

        AsyncCallback< FRONT_DOOR_CLIENT_ERRORS >
        postPlaybackRequestErrorCallback( std::bind( &PlaybackRequestManager::PostPlaybackRequestError,
                                                     this,
                                                     std::placeholders::_1 ),
                                          &GetTask( ) );

        GetFrontDoorClient( )->SendPost<SoundTouchInterface::NowPlayingJson>( "/content/playbackRequest",
                                                                              playbackRequestData,
                                                                              postPlaybackRequestResponseCallback,
                                                                              postPlaybackRequestErrorCallback );

        BOSE_INFO( s_logger, "An attempt to play the TV source has been made." );
    }
    else if( action == ( uint16_t )Action::ACTION_SOUNDTOUCH )
    {
        SoundTouchInterface::playbackRequestJson& playbackRequestData =
            m_CustomProductController.GetLastSoundTouchPlayback( );

        AsyncCallback< SoundTouchInterface::NowPlayingJson >
        postPlaybackRequestResponseCallback( std::bind( &PlaybackRequestManager::PostPlaybackRequestResponse,
                                                        this, std::placeholders::_1 ),
                                             &GetTask( ) );

        AsyncCallback< FRONT_DOOR_CLIENT_ERRORS >
        postPlaybackRequestErrorCallback( std::bind( &PlaybackRequestManager::PostPlaybackRequestError,
                                                     this,
                                                     std::placeholders::_1 ),
                                          &GetTask( ) );

        GetFrontDoorClient( )->SendPost<SoundTouchInterface::NowPlayingJson>( "/content/playbackRequest",
                                                                              playbackRequestData,
                                                                              postPlaybackRequestResponseCallback,
                                                                              postPlaybackRequestErrorCallback );

        BOSE_INFO( s_logger, "An attempt to play the last SoundTouch source has been made." );
    }
    else
    {
        BOSE_ERROR( s_logger, "An invalid intent action has been supplied." );
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   PlaybackRequestManager::PostPlaybackRequestResponse
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void PlaybackRequestManager::PostPlaybackRequestResponse( const SoundTouchInterface::NowPlayingJson&
                                                          response )
{
    BOSE_DEBUG( s_logger, "%s in %s", "PlaybackRequestManager", __FUNCTION__ );
    BOSE_DEBUG( s_logger, "A response to the playback request %s was received." ,
                response.source( ).sourcedisplayname( ).c_str( ) );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   PlaybackRequestManager::PostPlaybackRequestError
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void PlaybackRequestManager::PostPlaybackRequestError( const FRONT_DOOR_CLIENT_ERRORS errorCode )
{
    BOSE_DEBUG( s_logger, "%s in %s", "PlaybackRequestManager", __FUNCTION__ );
    BOSE_ERROR( s_logger, "An error %d was returned to the playback request.", errorCode );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                           End of the Product Application Namespace                           ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
