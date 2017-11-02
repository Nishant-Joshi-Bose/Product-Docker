////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file      ProductVolumeManager.cpp
///
/// @brief     This file implements audio olume management.
///
/// @author    Chris Houston
///
/// @date      11/1/2017
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

////////////////////////////////////////////////////////////////////////////////////////////////
///
/// The following constants define FrontDoor endpoints used by the VolumeManager
///
////////////////////////////////////////////////////////////////////////////////////////////////
constexpr char FRONTDOOR_AUDIO_VOLUME[]                         = "/audio/volume";
constexpr char FRONTDOOR_AUDIO_VOLUME_INCREMENT[]               = "/audio/volume/increment";
constexpr char FRONTDOOR_AUDIO_VOLUME_DECREMENT[]               = "/audio/volume/decrement";

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductVolumeManager::GetInstance
///
/// @brief  This static method creates the one and only instance of a ProductVolumeManager object.
///         The C++ Version 11 compiler guarantees that only one instance is created in a thread
///         safe way.
///
/// @param mainTask
///
/// @param ProductNotify
///
/// @param HardwareInterface
///
/// @return This method returns a pointer to a ProductVolumeManager object.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
ProductVolumeManager* ProductVolumeManager::GetInstance( NotifyTargetTaskIF*        mainTask,
                                                         Callback< ProductMessage > ProductNotify,
                                                         ProductHardwareInterface*  HardwareInterface )
{
    static ProductVolumeManager* instance = nullptr;

    if( instance == nullptr )
    {
        instance = new ProductVolumeManager( mainTask, ProductNotify, HardwareInterface );
        instance->Initialize();
    }

    BOSE_DEBUG( s_logger, "The instance %p of the ProductVolumeManager was returned.", instance );
    return instance;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductVolumeManager::Initialize
///
/// @brief  This method performs one-time initialization of this instance.  This is a good place
///         to put things that you may have wanted to do in the constructor but that might depend on
///         the object being fully-initialized.
///
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductVolumeManager::Initialize( )
{

    m_FrontDoorClient = FrontDoor::FrontDoorClient::Create( "ProductControllerStateOn" );

    auto fVolume = [ this ]( int32_t v )
    {
        UpdateFrontDoorVolume( v );
    };
    m_Volume = new AudioVolume<int32_t>( fVolume );

    auto fNotify = [ this ]( SoundTouchInterface::volume v )
    {
        ReceiveFrontDoorVolume( v );
    };
    m_FrontDoorClient->RegisterNotification< SoundTouchInterface::volume >
    ( FRONTDOOR_AUDIO_VOLUME, fNotify );
}


////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductVolumeManager::
///
/// @brief  This method is the ProductVolumeManager constructor, which is declared as being private to
///         ensure that only one instance of this class can be created through the class GetInstance
///         method.
///
/// @param mainTask
///
/// @param ProductNotify
///
/// @param HardwareInterface
///
/// @return This method does not return anything.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
ProductVolumeManager::ProductVolumeManager( NotifyTargetTaskIF*        mainTask,
                                            Callback< ProductMessage > ProductNotify,
                                            ProductHardwareInterface*  HardwareInterface )
    : m_mainTask( mainTask ),
      m_ProductNotify( ProductNotify ),
      m_ProductHardwareInterface( HardwareInterface )
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductVolumeManager::Run
///
/// @brief  This method starts the main task for the ProductVolumeManager instance. The OnEntry method
///         for the ProductVolumeManager instance is called just before the main task starts. Also,
///         this main task is used for most of the internal processing for each of the subclass
///         instances.
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
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductVolumeManager::Stop( void )
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
    // TODO - currently the CAPS interface only supports volume setting directly (not delta);
    // once delta is in place remove volume class and just send a volume_up/volume_down command
    // when the corresponding intents are received
    auto respFunc = []( SoundTouchInterface::volume v )
    {
        BOSE_VERBOSE( s_logger, "Got volume set response (%d)", v.value() );
    };

    auto errFunc = []( FRONT_DOOR_CLIENT_ERRORS e )
    {
        BOSE_ERROR( s_logger, "Error updating FrontDoor volume" );
    };

    AsyncCallback<SoundTouchInterface::volume> respCb( respFunc, m_mainTask );
    AsyncCallback<FRONT_DOOR_CLIENT_ERRORS> errCb( errFunc, m_mainTask );

    SoundTouchInterface::volume pbVolume;
    pbVolume.set_value( volume );

    BOSE_VERBOSE( s_logger, "Updating FrontDoor volume %d", volume );
    m_FrontDoorClient->SendPost<SoundTouchInterface::volume>(
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

    // send to lpm as well (this is currently same range as CAPS, 0-100)
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
