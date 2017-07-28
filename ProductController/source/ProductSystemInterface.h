////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file      ProductSystemInterface.h
///
/// @brief     This header file declares a ProductSystemInterface class that acts as a container to
///            handle all the main functionality related to this program that is not product
///            specific. A single instance of this class is created in the main function for the
///            Product Controller, where the Run method for this instance is called to start and run
///            this program.
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
#include "SoundTouchClient.h"       /// This file contains declaration to interface with the system.
#include "SystemService.pb.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
///
///            Included Subclasses
///
////////////////////////////////////////////////////////////////////////////////////////////////////
class ProductHardwareManager;
class ProductDeviceManager;
class ProductUserInterface;
class ProductCommandLine;
class ProductController;

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @class ProductSystemInterface
///
/// @brief This class acts as a container to handle all the main functionality related to this
///        program that is not product specific, including controlling the product states, as well
///        as to instantiating subclasses to manage the device and lower level hardware, and to
///        interface with the user and higher level applications. Note that only one instantiation
///        of this class is to be created through its GetInstance static method, which returns a
///        single static reference to an instance of this class.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
class ProductSystemInterface : public APTask, public SoundTouch::Client
{
  public:

      //////////////////////////////////////////////////////////////////////////////////////////////
      ///
      /// @name   ProductSystemInterface::GetInstance
      ///
      /// @brief  This static method creates the one and only instance of a ProductSystemInterface
      ///         object. That only one instance is created in a thread safe way is guaranteed by
      ///         the C++ Version 11 compiler.
      ///
      /// @param  void This method does not take any arguments.
      ///
      /// @return This method returns a pointer to a ProductSystemInterface object.
      ///
      //////////////////////////////////////////////////////////////////////////////////////////////
      static ProductSystemInterface* GetInstance( );

      ~ProductSystemInterface( );

      //////////////////////////////////////////////////////////////////////////////////////////////
      ///
      /// @brief  The following public methods are used to start the ProductSystemInterface instance
      ///         task, set up processing bes_loggerfore this task runs, and end the task
      ///         respectively.
      ///
      //////////////////////////////////////////////////////////////////////////////////////////////
      void Run    ( void );
      void OnEntry( void ) override;
      void Process( void );
      void End   ( void );

  private:

      //////////////////////////////////////////////////////////////////////////////////////////////
      ///
      /// @name   ProductSystemInterface
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
      ProductSystemInterface( );

      //////////////////////////////////////////////////////////////////////////////////////////////
      ///
      /// @brief The following copy constructor and equality operator for this class are private
      ///        and are set to be undefined through the delete keyword. This prevents this class
      ///        from being copied directly, so that only the static method GetInstance to this
      ///        class can be used to get the one sole instance of it.
      ///
      //////////////////////////////////////////////////////////////////////////////////////////////
      ProductSystemInterface( ProductSystemInterface const& ) = delete;
      void operator =       ( ProductSystemInterface const& ) = delete;

      //////////////////////////////////////////////////////////////////////////////////////////////
      ///
      /// @brief The following subclass instances are used to manage the lower level hardware and
      ///        the device, as well as to interface with the user and higher level system
      ///        applications, respectively.
      ///
      //////////////////////////////////////////////////////////////////////////////////////////////
      ProductHardwareManager* m_ProductHardwareManager = nullptr;
      ProductDeviceManager*   m_ProductDeviceManager   = nullptr;
      ProductUserInterface*   m_ProductUserInterface   = nullptr;
      ProductController*      m_ProductController      = nullptr;

      NotifyTargetTaskIF* m_mainTask;
      bool                m_running;

      void OnMessage( SoundTouchInterface::updates_NowPlayingUpdate              message ) override;
      void OnMessage( SoundTouchInterface::updates_PresetsUpdate                 message ) override;
      void OnMessage( SoundTouchInterface::updates_RecentsUpdate                 message ) override;
      void OnMessage( SoundTouchInterface::updates_BrowseUpdate                  message ) override;
      void OnMessage( SoundTouchInterface::updates_NowSelectionUpdate            message ) override;
      void OnMessage( SoundTouchInterface::updates_VolumeUpdate                  message ) override;
      void OnMessage( SoundTouchInterface::updates_SourcesUpdate                 message ) override;
      void OnMessage( SoundTouchInterface::updates_SwUpdateStatusUpdate          message ) override;
      void OnMessage( SoundTouchInterface::updates_ConnectionStateUpdate         message ) override;
      void OnMessage( SoundTouchInterface::updates_GroupUpdate                   message ) override;
      void OnMessage( SoundTouchInterface::updates_LanguageUpdate                message ) override;
      void OnMessage( SoundTouchInterface::updates_NameUpdate                    message ) override;
      void OnMessage( SoundTouchInterface::updates_SetupAPUpdate                 message ) override;
      void OnMessage( SoundTouchInterface::updates_SoundTouchConfigurationUpdate message ) override;
      void OnMessage( SoundTouchInterface::userActivityUpdate                    message ) override;
      void OnMessage( SoundTouchInterface::userInactivityUpdate                  message ) override;
      void OnMessage( SoundTouchInterface::LowPowerStandbyUpdate                 message ) override;
      void OnMessage( SoundTouchInterface::SoftwareUpdateStatus                  message ) override;
      void OnMessage( SoundTouchInterface::criticalErrorUpdate                   message ) override;
      void OnMessage( SoundTouchInterface::errorUpdate                           message ) override;
      void OnMessage( SoundTouchInterface::SoundTouchSdkInfo                     message ) override;
      void OnMessage( SoundTouchInterface::reboot                                message ) override;
      void OnMessage( SoundTouchInterface::updates_ZoneUpdate                    message ) override;
      void OnMessage( SoundTouchInterface::updates_InfoUpdate                    message ) override;
      void OnMessage( SoundTouchInterface::nowPlaying                            message, int32_t requestId ) override;
      void OnMessage( SoundTouchInterface::presets                               message, int32_t requestId ) override;
      void OnMessage( SoundTouchInterface::recents                               message, int32_t requestId ) override;
      void OnMessage( SoundTouchInterface::sources                               message, int32_t requestId ) override;
      void OnMessage( SoundTouchInterface::zone                                  message, int32_t requestId ) override;
      void OnMessage( SoundTouchInterface::status                                message, int32_t requestId ) override;
      void OnMessage( SoundTouchInterface::errors                                message, int32_t requestId ) override;
      void OnMessage( SoundTouchInterface::networkInfo                           message, int32_t requestId ) override;
      void OnMessage( SoundTouchInterface::GetActiveWirelessProfileResponse      message, int32_t requestId ) override;
      void OnMessage( SoundTouchInterface::sysLanguage                           message, int32_t requestId ) override;
      void OnMessage( SoundTouchInterface::volume                                message, int32_t requestId ) override;
      void OnMessage( SoundTouchInterface::name                                  message, int32_t requestId ) override;
      void OnMessage( SoundTouchInterface::setupStateResponse                    message, int32_t requestId ) override;
      void OnMessage( SoundTouchInterface::setupState                            message, int32_t requestId ) override;
      void OnMessage( SoundTouchInterface::SoundTouchConfigurationStatus         message, int32_t requestId ) override;
      void OnMessage( SoundTouchInterface::BluetoothInfo                         message, int32_t requestId ) override;
      void OnMessage( WebInterface::pingRequest                                  message, int32_t requestId ) override;
      void OnMessage( SoundTouchInterface::activateProductRequest                request, SoundTouchInterface::msg message) override;
      void OnMessage( SoundTouchInterface::initializationCompleteRequest         request, SoundTouchInterface::msg message) override;
      void OnMessage( SoundTouchInterface::toggleStandbyRequest                  request, SoundTouchInterface::msg message) override;

      // bool UnhandledRequest ( SoundTouchInterface::msg_Header const& cookie,
      //                         std::string const&                     body,
      //                         std::string const&                     operation ) override;
      // bool UnhandledResponse( SoundTouchInterface::msg const&        message   ) override;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
