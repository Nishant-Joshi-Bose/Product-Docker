////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file      VolumeMuteControlManager.cpp
///
/// @brief     This source code file contains functionality to implement an intent manager class for
///            volume and mute control based on Professor product specific key actions
///
/// @author    Stuart J. Lumby
///
/// @todo      This intent manager needs to incorporate changes for the ramping up or down of the
///            volume found in the common code in the repository CastleProductControllerCommon.
///            This requirement is logged under the JIRA Story PGC-600.
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
#include "FrontDoorClient.h"
#include "ProfessorProductController.h"
#include "CustomProductLpmHardwareInterface.h"
#include "VolumeMuteControlManager.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
///                             Start of Product Namespace                                       ///
////////////////////////////////////////////////////////////////////////////////////////////////////
namespace ProductApp
{

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// The following constants define FrontDoor endpoints used by the VolumeManager
///
////////////////////////////////////////////////////////////////////////////////////////////////////
constexpr char  FRONTDOOR_AUDIO_VOLUME[ ]           = "/audio/volume";
constexpr char  FRONTDOOR_AUDIO_VOLUME_INCREMENT[ ] = "/audio/volume/increment";
constexpr char  FRONTDOOR_AUDIO_VOLUME_DECREMENT[ ] = "/audio/volume/decrement";

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief VolumeMuteControlManager::VolumeMuteControlManager
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
VolumeMuteControlManager::VolumeMuteControlManager( NotifyTargetTaskIF&        task,
                                                    const CliClientMT&         commandLineClient,
                                                    const FrontDoorClientIF_t& frontDoorClient,
                                                    ProductController&         productController )

    : IntentManager( task, commandLineClient, frontDoorClient, productController ),
      m_CustomProductController( static_cast< ProfessorProductController & >( productController ) ),
      m_ProductTask( m_CustomProductController.GetTask( ) ),
      m_ProductNotify( m_CustomProductController.GetMessageHandler( ) ),
      m_ProductLpmHardwareInterface( m_CustomProductController.GetLpmHardwareInterface( ) )
{
    BOSE_INFO( s_logger, "%s is being constructed.", "VolumeMuteControlManager" );

    Initialize( );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   VolumeMuteControlManager::Initialize
///
/// @brief  This method registers for volume notifications from the Front Door.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void VolumeMuteControlManager::Initialize( )
{
    m_FrontDoorClient = FrontDoor::FrontDoorClient::Create( "VolumeMuteControlManager" );

    auto fNotify = [ this ]( SoundTouchInterface::volume v )
    {
        ReceiveFrontDoorVolume( v );
    };

    m_NotifierCallback = m_FrontDoorClient->RegisterNotification< SoundTouchInterface::volume >
                         ( FRONTDOOR_AUDIO_VOLUME, fNotify );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief  VolumeMuteControlManager::Handle
///
/// @brief  This method is used to process volume and mute Professor product specific key actions.
///
/// @param  KeyHandlerUtil::ActionType_t& action
///
/// @return This method returns true base on its handling of the key action sent.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool VolumeMuteControlManager::Handle( KeyHandlerUtil::ActionType_t& action )
{
    BOSE_INFO( s_logger, "%s is in %s handling the action %u.", "VolumeMuteControlManager",
               __func__,action );

    if( action == ( uint16_t )Action::ACTION_VOLUME_UP_1 )
    {
        Increment( 1 );
    }
    else if( action == ( uint16_t )Action::ACTION_VOLUME_DOWN_1 )
    {
        Decrement( 1 );
    }
    else if( action == ( uint16_t )Action::ACTION_MUTE )
    {
        ToggleMute( );
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief VolumeMuteControlManager::Stop
///
/// @todo  Resources, memory, or any client server connections that may need to be released by
///        this module when stopped will need to be determined.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void VolumeMuteControlManager::Stop( )
{
    m_NotifierCallback.Disconnect( );
}


////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief VolumeMuteControlManager::ReceiveFrontDoorVolume
///
/// @param  volume Object containing volume received from the FrontDoor
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void VolumeMuteControlManager::ReceiveFrontDoorVolume( SoundTouchInterface::volume const& volume )
{
    if( volume.has_value() )
    {
        BOSE_VERBOSE( s_logger, "Got volume notify (%d)", volume.value() );

        ///
        /// Change in volume actions can be put here.
        ///
    }

    ///
    /// Update mute status if it is available.
    ///
    if( volume.has_muted() )
    {
        m_muted = volume.muted();
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name VolumeMuteControlManager::Increment
///
/// @brief This method increments the volume
///
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void VolumeMuteControlManager::Increment( unsigned int step )
{
    auto errFunc = []( const EndPointsError::Error & e )
    {
        BOSE_ERROR( s_logger, "Error incrementing FrontDoor volume" );
    };
    auto respFunc = [ this ]( SoundTouchInterface::volume v )
    {
        BOSE_VERBOSE( s_logger, "Got volume increment response" );
    };

    AsyncCallback<SoundTouchInterface::volume> respCb( respFunc, m_ProductTask );
    AsyncCallback<EndPointsError::Error> errCb( errFunc, m_ProductTask );

    SoundTouchInterface::volume pbVolume;
    pbVolume.set_delta( step );
    printf( "%s: %d\n", __func__, step );

    BOSE_VERBOSE( s_logger, "Incrementing FrontDoor volume" );
    m_FrontDoorClient->SendPut<SoundTouchInterface::volume, EndPointsError::Error>(
        ProductApp::FRONTDOOR_AUDIO_VOLUME_INCREMENT, pbVolume, respFunc, errCb );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name VolumeMuteControlManager::Decrement
///
/// @brief This method decrements the volume
///
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void VolumeMuteControlManager::Decrement( unsigned int step )
{
    auto errFunc = []( const EndPointsError::Error & e )
    {
        BOSE_ERROR( s_logger, "Error incrementing FrontDoor volume" );
    };
    auto respFunc = [ this ]( SoundTouchInterface::volume v )
    {
        BOSE_VERBOSE( s_logger, "Got volume decrement response" );
    };

    AsyncCallback<SoundTouchInterface::volume> respCb( respFunc, m_ProductTask );
    AsyncCallback<EndPointsError::Error> errCb( errFunc, m_ProductTask );

    SoundTouchInterface::volume pbVolume;
    pbVolume.set_delta( step );
    printf( "%s: %d\n", __func__, step );

    BOSE_VERBOSE( s_logger, "Decrementing FrontDoor volume" );
    m_FrontDoorClient->SendPut<SoundTouchInterface::volume, EndPointsError::Error>(
        ProductApp::FRONTDOOR_AUDIO_VOLUME_DECREMENT, pbVolume, respFunc, errCb );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name VolumeMuteControlManager::ToggleMute
///
/// @brief This method toggles mute
///
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void VolumeMuteControlManager::ToggleMute( )
{
    auto errFunc = []( const EndPointsError::Error & e )
    {
        BOSE_ERROR( s_logger, "Error setting FrontDoor mute" );
    };
    auto respFunc = [ this ]( SoundTouchInterface::volume v )
    {
        BOSE_VERBOSE( s_logger, "Got mute response" );
    };

    AsyncCallback<SoundTouchInterface::volume> respCb( respFunc, m_ProductTask );
    AsyncCallback<EndPointsError::Error> errCb( errFunc, m_ProductTask );

    m_muted = !m_muted;
    SoundTouchInterface::volume pbVolume;
    pbVolume.set_muted( m_muted );

    BOSE_VERBOSE( s_logger, "Toggling FrontDoor mute" );
    m_FrontDoorClient->SendPut<SoundTouchInterface::volume, EndPointsError::Error>(
        ProductApp::FRONTDOOR_AUDIO_VOLUME, pbVolume, respFunc, errCb );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                               End of ProductApp Namespace                                    ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
