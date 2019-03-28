////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file      MuteManager.cpp
///
/// @brief     This source code file contains functionality to implement an intent manager class for
///            volume and mute control based on product specific key actions
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
#include "FrontDoorClient.h"
#include "CustomProductController.h"
#include "MuteManager.h"
#include "ProductEndpointDefines.h"

///
/// Class Name Declaration for Logging
///
namespace
{
constexpr char CLASS_NAME[ ] = "MuteManager";
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                             Start of Product Namespace                                       ///
////////////////////////////////////////////////////////////////////////////////////////////////////
namespace ProductApp
{

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
      m_CustomProductController( static_cast< CustomProductController & >( productController ) ),
      m_ProductTask( m_CustomProductController.GetTask( ) ),
      m_ProductNotify( m_CustomProductController.GetMessageHandler( ) ),
      m_FrontDoorClient( frontDoorClient )
{
    BOSE_INFO( s_logger, "%s is being constructed.", CLASS_NAME );

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
    auto fNotify = [ this ]( SoundTouchInterface::volume v )
    {
        ReceiveFrontDoorVolume( v );
    };

    m_NotifierCallback = m_FrontDoorClient->RegisterNotification< SoundTouchInterface::volume >
                         ( FRONTDOOR_AUDIO_VOLUME_API, AsyncCallback< SoundTouchInterface::volume > ( fNotify, m_ProductTask ) );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief  MuteManager::Handle
///
/// @brief  This method is used to process volume and mute product specific key actions.
///
/// @param  KeyHandlerUtil::ActionType_t& action
///
/// @return This method returns true base on its handling of the key action sent.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool MuteManager::Handle( KeyHandlerUtil::ActionType_t& action )
{
    BOSE_INFO( s_logger, "%s::%s is handling the intent %s", CLASS_NAME, __func__, CommonIntentHandler::GetIntentName( action ).c_str( ) );

    if( action == ( uint16_t )Action::ACTION_MUTE )
    {
        ToggleMute( );
        return true;
    }

    if( action == ( uint16_t )Action::ACTION_ASSERT_MUTE )
    {
        if( !m_muted )
        {
            ToggleMute( );
        }
        return true;
    }

    if( action == ( uint16_t )Action::ACTION_ASSERT_UNMUTE )
    {
        if( m_muted )
        {
            ToggleMute( );
        }
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
    auto errFunc = []( FrontDoor::Error error )
    {
        BOSE_ERROR( s_logger, "An error code %d subcode %d and error string <%s> was returned from a frontdoor mute request.",
                    error.code(),
                    error.subcode(),
                    error.message().c_str() );
    };
    auto respFunc = [ this ]( SoundTouchInterface::volume v )
    {
        ReceiveFrontDoorVolume( v );
    };

    AsyncCallback<SoundTouchInterface::volume> respCb( respFunc, m_ProductTask );
    AsyncCallback<FrontDoor::Error> errCb( errFunc, m_ProductTask );

    SoundTouchInterface::volume pbVolume;
    pbVolume.set_muted( !m_muted );
#if 0 // @TODO waiting for  CASTLE-29661; see PGC-4261
    pbVolume.mutable_feedback()->set_enable( true );
#endif

    BOSE_VERBOSE( s_logger, "Toggling FrontDoor mute" );
    m_FrontDoorClient->SendPut<SoundTouchInterface::volume, FrontDoor::Error>(
        FRONTDOOR_AUDIO_VOLUME_API, pbVolume, respCb, errCb );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                               End of ProductApp Namespace                                    ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
