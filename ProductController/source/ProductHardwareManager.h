////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file      ProductHardwareManager.h
///
/// @brief     This header file contains declarations for managing the hardware, which interfaces
///            with the Low Power Microprocessor or LPM.
///
/// @author    Stuart J. Lumby
///
/// @date      07/15/2017
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
#include "ThreadMutex.h"
#include "NotifyTargetTaskIF.h"
#include "APTask.h"
#include "CliClient.h"              /// This file declares functionality for a command line interface.
#include "APClientSocketListenerIF.h"
#include "APServerSocketListenerIF.h"
#include "IPCMessageRouterIF.h"
#include "APProductIF.h"
#include "A4V_IpcProtocol.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
///
///            Included Subclasses
///
////////////////////////////////////////////////////////////////////////////////////////////////////
class ProductDeviceManager;
class ProductUserInterface;
class ProductSystemInterface;
class ProductCommandLine;
class ProductController;

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief The following aliases refer to the Bose Sound Touch class utilities for inter-process and
///        inter-thread communications.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
typedef APProductIF::APProductPtr               ProductPointer;
typedef APClientSocketListenerIF::ListenerPtr   ClientPointer;
typedef APServerSocketListenerIF::ListenerPtr   ServerPointer;
typedef IPCMessageRouterIF::IPCMessageRouterPtr RouterPointer;
typedef CLIClient::CmdPtr                       CommandPointer;
typedef CLIClient::CLICmdDescriptor             CommandDescription ;

class ProductHardwareManager
{
  public:

      //////////////////////////////////////////////////////////////////////////////////////////////
      ///
      /// @name   ProductHardwareManager::GetInstance
      ///
      /// @brief  This static method creates the one and only instance of a ProductHardwareManager
      ///         object. That only one instance is created in a thread safe way is guaranteed by
      ///         the C++ Version 11 compiler.
      ///
      /// @param  void This method does not take any arguments.
      ///
      /// @return This method returns a reference to a ProductHardwareManager object.
      ///
      //////////////////////////////////////////////////////////////////////////////////////////////
      static ProductHardwareManager* GetInstance( );

      //////////////////////////////////////////////////////////////////////////////////////////////
      /// This declaration is used to start and run the hardware manager.
      //////////////////////////////////////////////////////////////////////////////////////////////
      void Run( void );

      //////////////////////////////////////////////////////////////////////////////////////////////
      /// These declarations are utility calls used mostly by the ProductDevice class, which send
      /// messages to the LPM hardware manager process as a client.
      //////////////////////////////////////////////////////////////////////////////////////////////
      void SendSetVolume                   ( uint32_t              volume );
      void SendUserMute                    ( bool                  mute );
      void SendInternalMute                ( bool                  mute );
      void SendAudioPathPresentationLatency( uint32_t              latency );
      void SendSetDSPAudioMode             ( IpcAudioMode_t        audioMode );
      void SendLipSyncDelay                ( uint32_t              audioDelay );
      void SendToneAndLevelControl         ( IpcToneControl_t&     controls );
      void SendSpeakerList                 ( IpcAccessoryList_t&   accessoryList );
      void SendSetSystemTimeoutEnableBits  ( Ipc_TimeoutControl_t& timeoutControl );
      void RebootRequest                   ( void );
      void HandleLowPowerStandby           ( void );
      void SendBlueToothDeviceData         ( const std::string&          bluetoothDeviceName,
                                             const unsigned long long    bluetoothMacAddress );
      void SendSourceSelection             ( const IPCSource_t&          sourceSelect );

private:

    //////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @name   ProductHardwareManager
    ///
    /// @brief  The constructor for this class is set to be private. This definition prevents this
    ///         class from being instantiated directly, so that only the static method GetInstance
    ///         to this class can be used to get the one sole instance of it.
    ///
    /// @param  void This method does not take any arguments.
    ///
    /// @return This method does not return anything.
    ///
    //////////////////////////////////////////////////////////////////////////////////////////////
    ProductHardwareManager( );

    //////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief The following copy constructor and equality operator for this class are private
    ///        and are set to be undefined through the delete keyword. This prevents this class
    ///        from being copied directly, so that only the static method GetInstance to this
    ///        class can be used to get the one sole instance of it.
    ///
    //////////////////////////////////////////////////////////////////////////////////////////////
    ProductHardwareManager( ProductHardwareManager const& ) = delete;
    void operator =       ( ProductHardwareManager const& ) = delete;

    //////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief The following subclass instances are used to manage the lower level hardware and
    ///        the device, as well as to interface with the user and higher level system
    ///        applications, respectively.
    ///
    //////////////////////////////////////////////////////////////////////////////////////////////
    ProductDeviceManager*   m_ProductDeviceManager   = nullptr;
    ProductUserInterface*   m_ProductUserInterface   = nullptr;
    ProductSystemInterface* m_ProductSystemInterface = nullptr;
    ProductController*      m_ProductController      = nullptr;

    //////////////////////////////////////////////////////////////////////////////////////////////
    /// This declaration stores the main task for processing device events and requests. It is
    /// inherited by the ProductController instance.
    //////////////////////////////////////////////////////////////////////////////////////////////
    NotifyTargetTaskIF* m_mainTask = nullptr;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
