////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file      MuteManager.cpp
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
#include "MuteManager.h"

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

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief MuteManager::MuteManager
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
MuteManager::MuteManager( NotifyTargetTaskIF&        task,
                          const CliClientMT&         commandLineClient,
                          const FrontDoorClientIF_t& frontDoorClient,
                          ProductController&         productController )

    : IntentManager( task, commandLineClient, frontDoorClient, productController ),
      m_CustomProductController( static_cast< ProfessorProductController & >( productController ) ),
      m_ProductTask( m_CustomProductController.GetTask( ) ),
      m_ProductNotify( m_CustomProductController.GetMessageHandler( ) )
{
    BOSE_INFO( s_logger, "%s is being constructed.", "MuteManager" );

    Initialize( );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   MuteManager::Initialize
///
/// @brief  This method registers for volume notifications from the Front Door.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void MuteManager::Initialize( )
{
    m_FrontDoorClient = FrontDoor::FrontDoorClient::Create( "MuteManager" );

    auto fNotify = [ this ]( SoundTouchInterface::volume v )
    {
        ReceiveFrontDoorVolume( v );
    };

    m_NotifierCallback = m_FrontDoorClient->RegisterNotification< SoundTouchInterface::volume >
                         ( FRONTDOOR_AUDIO_VOLUME, fNotify );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief  MuteManager::Handle
///
/// @brief  This method is used to process volume and mute Professor product specific key actions.
///
/// @param  KeyHandlerUtil::ActionType_t& action
///
/// @return This method returns true base on its handling of the key action sent.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool MuteManager::Handle( KeyHandlerUtil::ActionType_t& action )
{
    BOSE_INFO( s_logger, "%s is in %s handling the action %u.", "MuteManager",
               __func__, action );

    if( action == ( uint16_t )Action::ACTION_MUTE )
    {
        ToggleMute( );
        return true;
    }

    BOSE_ERROR( s_logger, "%s is in %s handling the unexpected action %u.", "MuteManager",
                __func__, action );
    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief MuteManager::Stop
///
/// @todo  Resources, memory, or any client server connections that may need to be released by
///        this module when stopped will need to be determined.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void MuteManager::Stop( )
{
    m_NotifierCallback.Disconnect( );
}


////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief MuteManager::ReceiveFrontDoorVolume
///
/// @param  volume Object containing volume received from the FrontDoor
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void MuteManager::ReceiveFrontDoorVolume( SoundTouchInterface::volume const& volume )
{
    BOSE_VERBOSE( s_logger, "volume received by %s is %s", __func__, ProtoToMarkup::ToJson( volume, false ).c_str() );
    ///
    /// Update mute status
    ///
    if( volume.has_muted() )
    {
        m_muted = volume.muted();
    }
    else
    {
        BOSE_ERROR( s_logger, "%s is in %s and has_muted is false.", "MuteManager",
                    __func__ );
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name MuteManager::ToggleMute
///
/// @brief This method toggles mute
///
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void MuteManager::ToggleMute( )
{
    auto errFunc = []( const FrontDoor::Error & e )
    {
        BOSE_ERROR( s_logger, "Error setting FrontDoor mute" );
    };
    auto respFunc = [ this ]( SoundTouchInterface::volume v )
    {
        ReceiveFrontDoorVolume( v );
    };

    AsyncCallback<SoundTouchInterface::volume> respCb( respFunc, m_ProductTask );
    AsyncCallback<FrontDoor::Error> errCb( errFunc, m_ProductTask );

    SoundTouchInterface::volume pbVolume;
    pbVolume.set_muted( !m_muted );

    BOSE_VERBOSE( s_logger, "Toggling FrontDoor mute" );
    m_FrontDoorClient->SendPut<SoundTouchInterface::volume, FrontDoor::Error>(
        ProductApp::FRONTDOOR_AUDIO_VOLUME, pbVolume, respFunc, errCb );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                               End of ProductApp Namespace                                    ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
