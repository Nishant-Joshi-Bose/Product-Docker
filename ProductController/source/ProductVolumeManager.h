////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file      ProductVolumeManager.h
///
/// @brief     This file contains the source code to handle communication with A4VVideoManager service
///
/// @author    Manoranjani Malisetti
///
/// @date      10/19/2017
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


///////////////////////////////////////////////////////////////////////////////////////////////////
///
/// The following compiler directive prevents this header file from being included more than once,
/// which may cause multiple declaration compiler errors.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////
///
///            Included Header Files
///
////////////////////////////////////////////////////////////////////////////////////////////////////
#include "NotifyTargetTaskIF.h"
#include "AudioVolume.h"
#include "FrontDoorClient.h"
#include "AudioService.pb.h"

namespace ProductApp
{
////////////////////////////////////////////////////////////////////////////////////////////////////
///
///            Included Subclasses
///
////////////////////////////////////////////////////////////////////////////////////////////////////
class ProductHardwareInterface;
class ProductController;

class ProductVolumeManager
{
public:

    //////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @name   ProductVolumeManager::GetInstance
    ///
    /// @brief  This static method creates the one and only instance of a ProductVolumeManager
    ///         object. That only one instance is created in a thread safe way is guaranteed by
    ///         the C++ Version 11 compiler.
    ///
    /// @param  task [input]         This argument specifies the task in which to run the hardware
    ///                               interface.
    ///
    /// @param  ProductNotifyCallback This argument specifies a callback to send messages back to
    ///                               the product controller.
    ///
    /// @return This method returns a reference to a ProductVolumeManager object.
    ///
    //////////////////////////////////////////////////////////////////////////////////////////////
    static ProductVolumeManager* GetInstance( NotifyTargetTaskIF*        task,
                                              Callback< ProductMessage > ProductNotifyCallback,
                                              ProductHardwareInterface*  HardwareInterface );

    //////////////////////////////////////////////////////////////////////////////////////////////
    /// This declaration is used to start and run the hardware manager.
    //////////////////////////////////////////////////////////////////////////////////////////////
    bool Run( void );
    void Stop( void );

    //////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief The following methods allow for manipulation of system volume
    ///
    //////////////////////////////////////////////////////////////////////////////////////////////
    void Increment( );
    void Decrement( );

private:

    ////////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief ProductVolumeManager
    ///
    /// @brief  The constructor for this class is set to be private. This definition prevents this
    ///         class from being instantiated directly, so that only the static method GetInstance
    ///         to this class can be used to get the one sole instance of it.
    ///
    /// @param  task
    ///
    /// @param ProductNotifyCallback
    ///
    ////////////////////////////////////////////////////////////////////////////////////////////////
    ProductVolumeManager( NotifyTargetTaskIF*        task,
                          Callback< ProductMessage > ProductNotifyCallback,
                          ProductHardwareInterface*  HardwareInterface );

    //////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief The following copy constructor and equality operator for this class are private
    ///        and are set to be undefined through the delete keyword. This prevents this class
    ///        from being copied directly, so that only the static method GetInstance to this
    ///        class can be used to get the one sole instance of it.
    ///
    //////////////////////////////////////////////////////////////////////////////////////////////
    ProductVolumeManager( ProductVolumeManager const& ) = delete;
    void operator     = ( ProductVolumeManager const& ) = delete;

    //////////////////////////////////////////////////////////////////////////////////////////////
    /// These declarations store the main task for processing LPM hardware events and requests. It
    /// is passed by the ProductController instance.
    //////////////////////////////////////////////////////////////////////////////////////////////
    NotifyTargetTaskIF*        m_mainTask       = nullptr;
    Callback< ProductMessage > m_ProductNotify  = nullptr;

    //////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief The following subclass instances are used to manage the lower level hardware and
    ///        the device.
    ///
    //////////////////////////////////////////////////////////////////////////////////////////////
    ProductHardwareInterface* m_ProductHardwareInterface = nullptr;

    //////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief The following method provides for one-time initialization after the constructor
    ///        has completed
    ///
    //////////////////////////////////////////////////////////////////////////////////////////////
    void Initialize();

    //////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief The following methods communicate with the FrontDoor to set and receive volume
    ///
    //////////////////////////////////////////////////////////////////////////////////////////////
    void UpdateFrontDoorVolume( int32_t volume );
    void ReceiveFrontDoorVolume( SoundTouchInterface::volume& volume );

    std::shared_ptr<FrontDoorClientIF>      m_FrontDoorClient;
    std::shared_ptr<AudioVolume<int32_t>>   m_Volume;

};
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
