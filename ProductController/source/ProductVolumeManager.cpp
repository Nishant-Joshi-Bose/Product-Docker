////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file      ProductVolumeManager.cpp
///
/// @brief     This file contains source code to implement audio volume management.
///
/// @author    Manoranjani Malisetti
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
constexpr char FRONTDOOR_AUDIO_VOLUME[ ]           = "/audio/volume";
constexpr char FRONTDOOR_AUDIO_VOLUME_INCREMENT[ ] = "/audio/volume/increment";
constexpr char FRONTDOOR_AUDIO_VOLUME_DECREMENT[ ] = "/audio/volume/decrement";

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @todo The following constant is temporary for documentation purposes; remove it and the
///       associated conditionals in the code once volume notifications are supported by CAPS.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
static constexpr bool VOLUME_NOTIFICATIONS_SUPPORTED = false;

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
    Initialize( );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductVolumeManager::Initialize
///
/// @brief  This method performs one-time initialization of this instance. This is a good place to
///         put functionality that needs to be done in the constructor but that might depend on the
///         object  being fully-initialized.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductVolumeManager::Initialize( )
{
    m_FrontDoorClient = FrontDoor::FrontDoorClient::Create( "ProductVolumeManager" );

    auto fVolume = [ this ]( int32_t v )
    {
        UpdateFrontDoorVolume( v );
    };
    m_Volume = new AudioVolume<int32_t>( fVolume );

    if( VOLUME_NOTIFICATIONS_SUPPORTED )
    {
        auto fNotify = [ this ]( SoundTouchInterface::volume v )
        {
            ReceiveFrontDoorVolume( v );
        };

        m_FrontDoorClient->RegisterNotification< SoundTouchInterface::volume >
        ( FRONTDOOR_AUDIO_VOLUME, fNotify );
    }
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

}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name ProductVolumeManager::UpdateFrontDoorVolume
///
/// @brief This method writes a new volume to the FrontDoor.
///
/// @param  volume Volume to send (0 - 100)
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductVolumeManager::UpdateFrontDoorVolume( int32_t volume )
{
    ///
    /// @todo Currently the CAPS interface only supports volume setting directly (and not as a
    ///       difference); once difference is in place remove volume class and just send a
    ///       volume_up or volume_down command, when the corresponding intents are received.
    ///
    auto respFunc = [ this ]( SoundTouchInterface::volume v )
    {
        BOSE_VERBOSE( s_logger, "Got volume set response (%d)", v.value() );

        ///
        /// This is a temporary workaround to volume notifications not being available
        ///
        if( !VOLUME_NOTIFICATIONS_SUPPORTED )
        {
            ReceiveFrontDoorVolume( v );
        }
    };

    auto errFunc = []( FRONT_DOOR_CLIENT_ERRORS e )
    {
        BOSE_ERROR( s_logger, "Error updating FrontDoor volume" );
    };

    AsyncCallback<SoundTouchInterface::volume> respCb( respFunc, m_ProductTask );
    AsyncCallback<FRONT_DOOR_CLIENT_ERRORS> errCb( errFunc, m_ProductTask );

    SoundTouchInterface::volume pbVolume;
    pbVolume.set_value( volume );

    BOSE_VERBOSE( s_logger, "Updating FrontDoor volume %d", volume );
    m_FrontDoorClient->SendPut<SoundTouchInterface::volume>(
        ProductApp::FRONTDOOR_AUDIO_VOLUME, pbVolume, respFunc, errCb );
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
    int32_t vol = volume.value();

    BOSE_VERBOSE( s_logger, "Got volume notify (%d)", vol );

    ///
    /// Send volume information to the LPM as well (this is currently same range as CAPS, 0-100).
    ///
    m_ProductHardwareInterface->SendSetVolume( vol );
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
    ( *m_Volume )++;
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
    ( *m_Volume )--;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                               End of ProductApp Namespace                                    ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
