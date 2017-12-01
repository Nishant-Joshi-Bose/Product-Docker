////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file      ProductVolumeManager.cpp
///
/// @brief     This file contains source code to implement audio volume management.
///
/// @author    Chris Houston
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
#include <string>
#include "DPrint.h"
#include "Utilities.h"
#include "ProfessorProductController.h"
#include "ProductHardwareInterface.h"
#include "ProductVolumeManager.h"

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
constexpr int   VOLUME_STEP_SIZE                    = 1;

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name  ProductVolumeManager::ProductVolumeManager
///
/// @param ProfessorProductController& ProductController
///
////////////////////////////////////////////////////////////////////////////////////////////////////
ProductVolumeManager::ProductVolumeManager( ProfessorProductController& ProductController )

    : m_ProductTask( ProductController.GetTask( ) ),
      m_ProductNotify( ProductController.GetMessageHandler( ) ),
      m_ProductHardwareInterface( ProductController.GetHardwareInterface( ) )
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductVolumeManager::Run
///
/// @brief  This method starts the main task for the ProductVolumeManager class.
///
/// @param  void This method does not take any arguments.
///
/// @return This method does not return anything.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool ProductVolumeManager::Run( )
{
    m_FrontDoorClient = FrontDoor::FrontDoorClient::Create( "ProductVolumeManager" );

    auto fNotify = [ this ]( SoundTouchInterface::volume v )
    {
        ReceiveFrontDoorVolume( v );
    };

    m_NotifierCallback = m_FrontDoorClient->RegisterNotification< SoundTouchInterface::volume >
                         ( FRONTDOOR_AUDIO_VOLUME, fNotify );

    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief ProductVolumeManager::Stop
///
/// @todo  Resources, memory, or any client server connections that may need to be released by
///        this module when stopped will need to be determined.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductVolumeManager::Stop( )
{
    m_NotifierCallback.Disconnect( );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief ProductVolumeManager::ReceiveFrontDoorVolume
///
/// @param  volume Object containing volume received from the FrontDoor
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductVolumeManager::ReceiveFrontDoorVolume( SoundTouchInterface::volume const& volume )
{
    if( volume.has_value() )
    {
        BOSE_VERBOSE( s_logger, "Got volume notify (%d)", volume.value() );
        ///
        /// You can do something here with volume
        ///

    }

    /// update mute status if it's available
    if( volume.has_muted() )
    {
        m_muted = volume.muted();
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name ProductVolumeManager::Increment
///
/// @brief This method increments the volume
///
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductVolumeManager::Increment( )
{
    auto errFunc = []( FRONT_DOOR_CLIENT_ERRORS e )
    {
        BOSE_ERROR( s_logger, "Error incrementing FrontDoor volume" );
    };
    auto respFunc = [ this ]( SoundTouchInterface::volume v )
    {
        BOSE_VERBOSE( s_logger, "Got volume increment response" );
    };

    AsyncCallback<SoundTouchInterface::volume> respCb( respFunc, m_ProductTask );
    AsyncCallback<FRONT_DOOR_CLIENT_ERRORS> errCb( errFunc, m_ProductTask );

    SoundTouchInterface::volume pbVolume;
    pbVolume.set_delta( VOLUME_STEP_SIZE );

    BOSE_VERBOSE( s_logger, "Incrementing FrontDoor volume" );
    m_FrontDoorClient->SendPut<SoundTouchInterface::volume>(
        ProductApp::FRONTDOOR_AUDIO_VOLUME_INCREMENT, pbVolume, respFunc, errCb );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name ProductVolumeManager::Decrement
///
/// @brief This method decrements the volume
///
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductVolumeManager::Decrement( )
{
    auto errFunc = []( FRONT_DOOR_CLIENT_ERRORS e )
    {
        BOSE_ERROR( s_logger, "Error incrementing FrontDoor volume" );
    };
    auto respFunc = [ this ]( SoundTouchInterface::volume v )
    {
        BOSE_VERBOSE( s_logger, "Got volume decrement response" );
    };

    AsyncCallback<SoundTouchInterface::volume> respCb( respFunc, m_ProductTask );
    AsyncCallback<FRONT_DOOR_CLIENT_ERRORS> errCb( errFunc, m_ProductTask );

    SoundTouchInterface::volume pbVolume;
    pbVolume.set_delta( VOLUME_STEP_SIZE );

    BOSE_VERBOSE( s_logger, "Decrementing FrontDoor volume" );
    m_FrontDoorClient->SendPut<SoundTouchInterface::volume>(
        ProductApp::FRONTDOOR_AUDIO_VOLUME_DECREMENT, pbVolume, respFunc, errCb );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name ProductVolumeManager::ToggleMute
///
/// @brief This method toggles mute
///
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductVolumeManager::ToggleMute( )
{
    auto errFunc = []( FRONT_DOOR_CLIENT_ERRORS e )
    {
        BOSE_ERROR( s_logger, "Error setting FrontDoor mute" );
    };
    auto respFunc = [ this ]( SoundTouchInterface::volume v )
    {
        BOSE_VERBOSE( s_logger, "Got mute response" );
    };

    AsyncCallback<SoundTouchInterface::volume> respCb( respFunc, m_ProductTask );
    AsyncCallback<FRONT_DOOR_CLIENT_ERRORS> errCb( errFunc, m_ProductTask );

    m_muted = !m_muted;
    SoundTouchInterface::volume pbVolume;
    pbVolume.set_muted( m_muted );

    BOSE_VERBOSE( s_logger, "Toggling FrontDoor mute" );
    m_FrontDoorClient->SendPut<SoundTouchInterface::volume>(
        ProductApp::FRONTDOOR_AUDIO_VOLUME, pbVolume, respFunc, errCb );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                               End of ProductApp Namespace                                    ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
