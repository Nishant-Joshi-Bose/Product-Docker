////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file      ProductDeviceManager.h
///
/// @brief     This header file contains declarations for managing the device, which includes
///            processing audio events (such as audio volume, muting, source selection, and latency
///            events), application reboot requests, speaker settings, and product HDMI control.
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
/// The following compiler directive prevents this header file from being included more than
/// once, which will cause multiple declaration compiler errors, in any file that includes it.
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
#include "CliClient.h"              /// This file declares functionality for a command line interface.
#include "APTask.h"
#include "BreakThread.h"
#include "APClientSocketListenerIF.h"
#include "APServerSocketListenerIF.h"
#include "IPCDirectory.h"
#include "IPCDirectoryIF.h"
#include "IPCMessageRouterIF.h"
#include "IPCDirectoryIF.h"
#include "AudioControls.pb.h"
#include "BoseLinkServerMsgReboot.pb.h"
#include "BoseLinkServerMsgIds.pb.h"
#include "WebExtensionsUrlHandler.h"
#include "NetworkPortDefines.h"
#include "A4VSystemTimeout.pb.h"
#include "A4VPersistence.pb.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
///
///            Included Subclasses
///
////////////////////////////////////////////////////////////////////////////////////////////////////
class ProductHardwareManager;
class ProductUserInterface;
class ProductSystemInterface;
class ProductCommandLine;
class ProductController;

////////////////////////////////////////////////////////////////////////////////////////////////////
///
///            Definitions
///
////////////////////////////////////////////////////////////////////////////////////////////////////
#define BLUETOOTH_MAC_LENGTH ( 6 )

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
typedef APServerSocketListenerIF::SocketPtr     ServerSocket;
typedef IPCMessageRouterIF::IPCMessageRouterPtr RouterPointer;
typedef CLIClient::CmdPtr                       CommandPointer;
typedef CLIClient::CLICmdDescriptor             CommandDescription ;

class ProductDeviceManager
{
  public:

      //////////////////////////////////////////////////////////////////////////////////////////////
      ///
      /// @name   ProductDeviceManager::GetInstance
      ///
      /// @brief  This static method creates the one and only instance of a ProductDeviceManager
      ///         object. That only one instance is created in a thread safe way is guaranteed by
      ///         the C++ Version 11 compiler.
      ///
      /// @param  void This method does not take any arguments.
      ///
      /// @return This method returns a reference to a ProductDeviceManager object.
      ///
      //////////////////////////////////////////////////////////////////////////////////////////////
      static ProductDeviceManager* GetInstance( );

      //////////////////////////////////////////////////////////////////////////////////////////////
      /// This declaration is used to start and run the device manager.
      //////////////////////////////////////////////////////////////////////////////////////////////
      void Run( void );

      //////////////////////////////////////////////////////////////////////////////////////////////
      /// These declarations are used to handle audio event on the product.
      //////////////////////////////////////////////////////////////////////////////////////////////
      void SelectCallbackAction            ( uint32_t      source  );
      void DeselectCallbackAction          ( uint32_t      source  );
      void VolumeCallbackAction            ( uint32_t      volume  );
      void UserMuteCallbackAction          ( bool          mute    );
      void InternalMuteCallbackAction      ( bool          mute    );
      void RebroadcastLatencyCallbackAction( uint32_t      latency );
      void ConnectCallbackAction           ( bool          connect );

      //////////////////////////////////////////////////////////////////////////////////////////////
      /// These declarations are used to handle Bluetooth functionality.
      //////////////////////////////////////////////////////////////////////////////////////////////
      void SetBlueToothMacAddress ( const std::string& bluetoothMacAddress );
      void SetBlueToothDeviceName ( const std::string& bluetoothDeviceName );
      void SendBlueToothDeviceData( const std::string& bluetoothDeviceName, const unsigned long long bluetoothMacAddress );

      //////////////////////////////////////////////////////////////////////////////////////////////
      /// The following declaration is used to set the system in low power standby mode.
      //////////////////////////////////////////////////////////////////////////////////////////////
      void HandleLowPowerStandby( void );

      //////////////////////////////////////////////////////////////////////////////////////////////
      /// This declaration is used to handle reboot requests.
      //////////////////////////////////////////////////////////////////////////////////////////////
      void SendRebootRequest( unsigned int delay );

  private:

      //////////////////////////////////////////////////////////////////////////////////////////////
      ///
      /// @name   ProductDeviceManager
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
      ProductDeviceManager(  );

      //////////////////////////////////////////////////////////////////////////////////////////////
      ///
      /// @brief The following copy constructor and equality operator for this class are private
      ///        and are set to be undefined through the delete keyword. This prevents this class
      ///        from being copied directly, so that only the static method GetInstance to this
      ///        class can be used to get the one sole instance of it.
      ///
      //////////////////////////////////////////////////////////////////////////////////////////////
      ProductDeviceManager( ProductDeviceManager const& ) = delete;
      void operator =     ( ProductDeviceManager const& ) = delete;

      //////////////////////////////////////////////////////////////////////////////////////////////
      ///
      /// @brief The following subclass instances are used to manage the lower level hardware and
      ///        the device, as well as to interface with the user and higher level system
      ///        applications, respectively.
      ///
      //////////////////////////////////////////////////////////////////////////////////////////////
      ProductHardwareManager* m_ProductHardwareManager = nullptr;
      ProductUserInterface*   m_ProductUserInterface   = nullptr;
      ProductSystemInterface* m_ProductSystemInterface = nullptr;
      ProductController*      m_ProductController      = nullptr;

      //////////////////////////////////////////////////////////////////////////////////////////////
      /// This declaration stores the main task for processing device events and requests. It is
      /// inherited by the ProductController instance.
      //////////////////////////////////////////////////////////////////////////////////////////////
      NotifyTargetTaskIF* m_mainTask = nullptr;

      //////////////////////////////////////////////////////////////////////////////////////////////
      /// These declarations are used for handling product audio events.
      //////////////////////////////////////////////////////////////////////////////////////////////
      ProductPointer m_ProductPointer = nullptr;

      void RegisterForProductEvents        ( void );
      void SelectCallback                  ( uint32_t      source  );
      void DeselectCallback                ( uint32_t      source  );
      void VolumeCallback                  ( uint32_t      volume  );
      void UserMuteCallback                ( bool          mute    );
      void InternalMuteCallback            ( bool          mute    );
      void RebroadcastLatencyCallback      ( uint32_t      latency );
      void ConnectCallback                 ( bool          connect );

      //////////////////////////////////////////////////////////////////////////////////////////////
      /// These declarations are used for handling product reboot request by system application
      /// clients.
      //////////////////////////////////////////////////////////////////////////////////////////////
      ServerPointer m_serverListener = nullptr;

      void RegisterForRebootRequests       ( void );
      void AcceptClient                    ( ServerSocket client );
      void HandleClientDisconnect          ( void );
      void SendRebootRequestHandler        ( BoseLinkServerMsgReboot rebootRequest );

      //////////////////////////////////////////////////////////////////////////////////////////////
      /// These declarations are used to handle Bluetooth functionality.
      //////////////////////////////////////////////////////////////////////////////////////////////
      bool               m_gettingBlueToothData = false;
      unsigned long long m_blueToothMacAddress  = 0ULL;
      std::string        m_blueToothDeviceName  { "" };

      //////////////////////////////////////////////////////////////////////////////////////////////
      /// The following declarations are for handling product specific requests.
      //////////////////////////////////////////////////////////////////////////////////////////////
      void RegisterForProductRequests( void );

      uint32_t m_currentVolume            = 0;
      uint32_t m_zonePresentationLatency  = 0;
      uint32_t m_audioPresentationLatency = 0;
      bool     m_userMute                 = true;
      bool     m_internalMute             = true;

      audiodspcontrols                m_audioDSPControlStore;
      audioproducttonecontrols        m_audioProductToneControlStore;
      audioproductlevelcontrols       m_audioProductSpeakerLevelStore;
      audiospeakerattributeandsetting m_audioSpeakerAttributeAndSettingStore;
      systemtimeoutcontrol            m_systemTimeoutControlStore;
      rebroadcastlatencymode          m_rebroadcastLatencyModeStore;

      WebExtensionsUrlHandler< audiodspcontrols                >* m_audioDSPControlHandler                 = nullptr;
      WebExtensionsUrlHandler< audioproducttonecontrols        >* m_audioProductToneControlHandler         = nullptr;
      WebExtensionsUrlHandler< audioproductlevelcontrols       >* m_audioProductSpeakerLevelHandler        = nullptr;
      WebExtensionsUrlHandler< audiospeakerattributeandsetting >* m_audioSpeakerAttributeAndSettingHandler = nullptr;
      WebExtensionsUrlHandler< systemtimeoutcontrol            >* m_systemTimeoutControlHandler            = nullptr;
      WebExtensionsUrlHandler< rebroadcastlatencymode          >* m_rebroadcastLatencyModeHandler          = nullptr;

      audiodspcontrols& GetAudioDSPControlStore( )
      {
          return m_audioDSPControlStore;
      }

      audioproducttonecontrols& GetAudioToneControlStore( )
      {
          return m_audioProductToneControlStore;
      }

      audioproductlevelcontrols& GetAudioSpeakerLevelStore( )
      {
          return m_audioProductSpeakerLevelStore;
      }

      audiospeakerattributeandsetting& GetSpeakerAttributeAndSettingStore( )
      {
          return m_audioSpeakerAttributeAndSettingStore;
      }

      systemtimeoutcontrol& GetSystemTimeoutEnableBits( )
      {
          return m_systemTimeoutControlStore;
      }

      rebroadcastlatencymode& GetRebroadcastLatencyModeStore( )
      {
          return m_rebroadcastLatencyModeStore;
      }

      bool SanitizeAudioDSPControls          ( audiodspcontrols&                protobuf );
      bool SanitizeAudioToneControls         ( audioproducttonecontrols&        protobuf ) const;
      bool SanitizeAudioLevelControls        ( audioproductlevelcontrols&       protobuf ) const;
      bool SanitizeSpeakerAttributeAndSetting( audiospeakerattributeandsetting& protobuf );
      bool SanitizeRebroadcastLatencyMode    ( rebroadcastlatencymode&          protobuf );

      void SendDSPAudioControls              ( void );
      void SendToneAndLevelControls          ( void );
      void SendSpeakerSettings               ( void );
      void SendSystemTimeoutEnableBits       ( void );
      void SendRebroadcastLatencyMode        ( void );
};

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
