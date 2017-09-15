////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file      ProductDeviceManager.cpp
///
/// @brief     This file contains source code for managing the device, which includes processing
///            audio events (such as audio volume, muting, source selection, and latency events),
///            application reboot requests, speaker settings, and product HDMI control.
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
///            Included Header Files
///
////////////////////////////////////////////////////////////////////////////////////////////////////
#include "SystemUtils.h"                /// This file contains system utility declarations.
#include "DPrint.h"                     /// This file contains the DPrint class used for logging.
#include "Services.h"                   /// This file declares application server names.
#include "CliClient.h"                  /// This file declares functionality for a command line interface.
#include "APProductIF.h"                /// This file declares functionality for audio event registration.
#include "APProductFactory.h"           /// This file declares functionality for audio event registration.
#include "AudioControls.pb.h"           /// This file declares data structures for audio events.
#include "BoseLinkServerMsgReboot.pb.h" /// This file declares reboot information.
#include "BoseLinkServerMsgIds.pb.h"    /// This file declares reboot information.
#include "IPCDirectory.h"               /// This file contains classes to determin service information.
#include "IPCDirectoryIF.h"             /// This file contains classes to determin service information.
#include "A4VSystemTimeout.pb.h"        /// This file declares time out information.
#include "A4VPersistence.pb.h"          /// This file declares persistence data for non-volatile storage.
#include "RebroadcastLatencyMode.pb.h"  /// This file contains latency data in Protocol Buffer format.
#include "NetworkPortDefines.h"         /// This file declares application server names.
#include "ProductController.h"          /// This file declares the ProductController class.
#include "ProductHardwareManager.h"     /// This file declares the ProductHardwareManager class.
#include "ProductDeviceManager.h"       /// This file declares the ProductDeviceManager class.
#include "ProductUserInterface.h"       /// This file declares the ProductUserInterface class.
#include "ProductSystemInterface.h"     /// This file declares the ProductSystemInterface class.

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief The following aliases refer to the Bose Sound Touch class utilities for inter-process and
///        inter-thread communications.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
typedef APProductIF::APProductPtr               ProductPointer;
typedef APClientSocketListenerIF::ListenerPtr   ClientPointer;
typedef APServerSocketListenerIF::ListenerPtr   ServerPointer;
typedef APClientSocketListenerIF::SocketPtr     ClientSocket;
typedef APServerSocketListenerIF::SocketPtr     ServerSocket;
typedef IPCMessageRouterIF::IPCMessageRouterPtr RouterPointer;
typedef CLIClient::CmdPtr                       CommandPointer;
typedef CLIClient::CLICmdDescriptor             CommandDescription;

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// The following declares a DPrint class type object and a standard string for logging information
/// in this source code file.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
static const DPrint s_logger    { "Product" };
static const char   s_logName[] = "Product Device"  ;

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductDeviceManager::GetInstance
///
/// @brief  This static method creates the one and only instance of a ProductDeviceManager object.
///         The C++ Version 11 compiler guarantees that only one instance is created in a thread
///         safe way.
///
/// @param  void This method does not take any arguments.
///
/// @return This method returns a pointer to a ProductDeviceManager object.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
ProductDeviceManager* ProductDeviceManager::GetInstance( )
{
       static ProductDeviceManager* instance = new ProductDeviceManager( );

       s_logger.LogInfo( "%-18s : The instance %8p of the Product Device Manager was returned. ",
                         s_logName,
                         instance );

       return instance;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductDeviceManager::ProductDeviceManager
///
/// @brief  This method is the ProductDeviceManager constructor, which is declared as being private
///         to ensure that only one instance of this class can be created through the class
///         GetInstance method.
///
/// @param  void This method does not take any arguments.
///
/// @return This method does not return anything.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
ProductDeviceManager::ProductDeviceManager( )
{
       return;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductDeviceManager::Run
///
/// @brief  This method registers for product events and requests, as well as reboot requests
///         through three registration methods.
///
/// @param  void This method does not take any arguments.
///
/// @return This method does not return anything.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductDeviceManager::Run( )
{
     m_ProductController      =   ProductController::GetInstance( );
     m_ProductHardwareManager = m_ProductController->GetHardwareManagerInstance( );
     m_ProductUserInterface   = m_ProductController->GetUserInterfaceInstance  ( );
     m_ProductSystemInterface = m_ProductController->GetSystemInterfaceInstance( );

     m_mainTask = m_ProductController->GetMainTask( );

     this->RegisterForProductEvents  ( );
     this->RegisterForProductRequests( );
     this->RegisterForRebootRequests ( );

     s_logger.LogInfo( "%-18s : Registration for device events and requests has been made. ", s_logName );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductDeviceManager::RegisterForProductEvents
///
/// @brief  This method registers for product events.
///
/// @param  void This method does not take any arguments.
///
/// @return This method does not return anything.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductDeviceManager::RegisterForProductEvents( )
{
     ///////////////////////////////////////////////////////////////////////////////////////////////
     /// A callback is created that is called whenever an audio selection has occurred.
     ///////////////////////////////////////////////////////////////////////////////////////////////
     m_ProductPointer = APProductFactory::Create( "ProductDeviceManager", m_mainTask );

     ///////////////////////////////////////////////////////////////////////////////////////////////
     /// A callback is created that is called whenever an audio selection has occurred.
     ///////////////////////////////////////////////////////////////////////////////////////////////
     {
          Callback< uint32_t > callback( std::bind( &ProductDeviceManager::SelectCallback,
                                                    this,
                                                    std::placeholders::_1 ) );
          m_ProductPointer->RegisterForSelect( callback );
     }

     ///////////////////////////////////////////////////////////////////////////////////////////////
     /// A callback is created that is called whenever an audio selection has occurred.
     ///////////////////////////////////////////////////////////////////////////////////////////////
     {
          Callback< uint32_t > callback( std::bind( &ProductDeviceManager::DeselectCallback,
                                                    this,
                                                    std::placeholders::_1 ) );
          m_ProductPointer->RegisterForDeselect( callback );
     }

     ///////////////////////////////////////////////////////////////////////////////////////////////
     /// A callback is created that is called whenever volume change occurs.
     ///////////////////////////////////////////////////////////////////////////////////////////////
     {
          Callback< uint32_t > callback( std::bind( &ProductDeviceManager::VolumeCallback,
                                                    this,
                                                    std::placeholders::_1 ) );
          m_ProductPointer->RegisterForVolume( callback );
     }

     ///////////////////////////////////////////////////////////////////////////////////////////////
     /// A callback is created that is called whenever an internal mute has occurred.
     ///////////////////////////////////////////////////////////////////////////////////////////////
     {
          Callback< bool > callback( std::bind( &ProductDeviceManager::UserMuteCallback,
                                                this,
                                                std::placeholders::_1 ) );
          m_ProductPointer->RegisterForUserMute( callback );
     }

     ///////////////////////////////////////////////////////////////////////////////////////////////
     /// A callback is created that is called whenever an internal mute has occurred.
     ///////////////////////////////////////////////////////////////////////////////////////////////
     {
          Callback< bool > callback( std::bind( &ProductDeviceManager::InternalMuteCallback,
                                                this,
                                                std::placeholders::_1 ) );
          m_ProductPointer->RegisterForInternalMute( callback );
     }

     ///////////////////////////////////////////////////////////////////////////////////////////////
     /// A callback for whenever an internal mute has occurred is created.
     ///////////////////////////////////////////////////////////////////////////////////////////////
     {
          Callback< uint32_t > callback( std::bind( &ProductDeviceManager::RebroadcastLatencyCallback,
                                                    this,
                                                    std::placeholders::_1 ) );
          m_ProductPointer->RegisterForRebroadcastLatency( callback );
     }

     Callback< bool >callback( std::bind( &ProductDeviceManager::ConnectCallback,
                                          this,
                                          std::placeholders::_1 ) );

     m_ProductPointer->Connect( callback );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductDeviceManager::SelectCallback
///
/// @brief  This method passes audio source selection events to the SelectCallbackAction method on
///         the main task.
///
/// @param  source This integer argument identifies the source to be selected.
///
/// @return This method does not return anything.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductDeviceManager::SelectCallback( uint32_t source )
{
     IL::BreakThread( std::bind( &ProductDeviceManager::SelectCallbackAction,
                                 this,
                                 source ),
                      m_mainTask );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductDeviceManager::SelectCallbackAction
///
/// @brief  This method processes the audio source selection event and sets it on the product.
///
/// @param  source This integer argument identifies the source to be selected.
///
/// @return This method does not return anything.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductDeviceManager::SelectCallbackAction( uint32_t source )
{
     s_logger.LogInfo( "%-18s : Selection of source %d has been made.", s_logName, source );

     m_ProductPointer->SetSelectionStatus( APProductIF::SELECTED, source );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductDeviceManager::DeselectCallback
///
/// @brief  This method passes audio source deselection events to the SelectCallbackAction method
///         on the main task.
///
/// @param  source This integer argument identifies the source to be deselected.
///
/// @return This method does not return anything.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductDeviceManager::DeselectCallback( uint32_t source )
{
     IL::BreakThread( std::bind( &ProductDeviceManager::DeselectCallbackAction,
                                 this,
                                 source ),
                      m_mainTask );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductDeviceManager::DeselectCallbackAction
///
/// @brief  This method processes the audio source deselection event and sets it on the product.
///
/// @param  source This integer argument identifies the source to be deselected.
///
/// @return This method does not return anything.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductDeviceManager::DeselectCallbackAction( uint32_t source )
{
     s_logger.LogInfo( "%-18s : Deselection of source %d has been sent.", s_logName, source );

     m_ProductPointer->SetSelectionStatus( APProductIF::DESELECTED, source );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductDeviceManager::VolumeCallback
///
/// @brief  This method passes audio volume events to the VolumeCallbackAction method on the main
///         task.
///
/// @param  volume This integer argument specifies the volume level.
///
/// @return This method does not return anything.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductDeviceManager::VolumeCallback( uint32_t volume )
{
     IL::BreakThread( std::bind( &ProductDeviceManager::VolumeCallbackAction,
                                 this,
                                 volume ),
                      m_mainTask );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductDeviceManager::VolumeCallbackAction
///
/// @brief  This method processes the audio volume event and sets it in the hardware. It is a public
///         method and can also be called through the system interface directly.
///
/// @param  volume This integer argument specifies the volume level.
///
/// @return This method does not return anything.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductDeviceManager::VolumeCallbackAction( uint32_t volume )
{
     s_logger.LogInfo( "%-18s : A volume level of %d will be set.", s_logName, volume );

     m_currentVolume = volume;

     m_ProductHardwareManager->SendSetVolume( volume );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductDeviceManager::UserMuteCallback
///
/// @brief  This method passes user mute events to the UserMuteCallbackAction method on the main
///         task.
///
/// @param  mute [input] This Boolean argument specifies whether to mute if it is true, or unmute if
///                      it is false.
///
/// @return This method does not return anything.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductDeviceManager::UserMuteCallback( bool mute )
{
     IL::BreakThread( std::bind( &ProductDeviceManager::UserMuteCallbackAction,
                                 this,
                                 mute ),
                      m_mainTask );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductDeviceManager::UserMuteCallbackAction
///
/// @brief  This method process the user mute event and sets it in the hardware. It is a public
///         method and can also be called through the system interface directly.
///
/// @param  mute [input] This Boolean argument specifies whether to mute if it is true, or unmute if
///                      it is false.
///
/// @return This method does not return anything.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductDeviceManager::UserMuteCallbackAction( bool mute )
{
     s_logger.LogInfo( "%-18s : A user mute %s will be set.", s_logName, ( mute ? "on" : "off" ) );

     m_userMute = mute;

     m_ProductHardwareManager->SendUserMute( m_userMute );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductDeviceManager::InternalMuteCallback
///
/// @brief  This method passes internal mute events to the InternalMuteCallbackAction method on the
///         main task.
///
/// @param  mute [input] This Boolean argument specifies whether to mute if it is true, or unmute if
///                      it is false.
///
/// @return This method does not return anything.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductDeviceManager::InternalMuteCallback( bool mute )
{
     IL::BreakThread( std::bind( &ProductDeviceManager::InternalMuteCallbackAction,
                                 this,
                                 mute ),
                      m_mainTask );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductDeviceManager::InternalMuteCallbackAction
///
/// @brief  This method process the internal mute event and sets it in the hardware. It is a public
///         method and can also be called through the system interface directly.
///
/// @param  mute [input] This Boolean argument specifies whether to mute if it is true, or unmute if
///                      it is false.
///
/// @return This method does not return anything.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductDeviceManager::InternalMuteCallbackAction( bool mute )
{
     s_logger.LogInfo( "%-18s : An internal mute %s has been sent.", s_logName, ( mute ? "on" : "off" ) );

     m_internalMute = mute;

     m_ProductHardwareManager->SendInternalMute( m_internalMute );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductDeviceManager::RebroadcastLatencyCallback
///
/// @brief  This method passes rebroadcast latency events to the RebroadcastLatencyCallbackAction
///         method on the main task.
///
/// @param  latency [input] This integer argument specifies the latency value.
///
/// @return This method does not return anything.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductDeviceManager::RebroadcastLatencyCallback( uint32_t latency )
{
     IL::BreakThread( std::bind( &ProductDeviceManager::RebroadcastLatencyCallbackAction,
                                 this,
                                 latency ),
                      m_mainTask );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductDeviceManager::RebroadcastLatencyCallbackAction
///
/// @brief  This method processes the rebroadcast latency event and sets it in the hardware. It is
///         a public method and can also be called through the system interface directly.
///
/// @param  latency [input] This integer argument specifies the latency value.
///
/// @return This method does not return anything.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductDeviceManager::RebroadcastLatencyCallbackAction( uint32_t latency )
{
     s_logger.LogInfo( "%-18s : A latency value of %d will be set.", s_logName, latency);

     m_zonePresentationLatency = latency;

     ///////////////////////////////////////////////////////////////////////////////////////////////
     /// The latency is set based on the argument. Multi-room zone information will need to be
     /// processed in future development.
     ///////////////////////////////////////////////////////////////////////////////////////////////
     m_ProductHardwareManager->SendAudioPathPresentationLatency( latency );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductDeviceManager::ConnectCallback
///
/// @brief  This method passes a connection event to the ConnectCallbackAction method on
///         the main task.
///
/// @param  connect [input] This Boolean argument identifies the source to be selected.
///
/// @return This method does not return anything.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductDeviceManager::ConnectCallback( bool connect )
{
     IL::BreakThread( std::bind( &ProductDeviceManager::ConnectCallbackAction,
                                 this,
                                 connect ),
                      m_mainTask );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductDeviceManager::ConnectCallbackAction
///
/// @brief  This method processes the connection event and sets the volume and mute state in the
///         hardware. It is a public method and can also be called through the system interface
///         directly.
///
/// @param  volume This integer argument specifies the volume level.
///
/// @return This method does not return anything.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductDeviceManager::ConnectCallbackAction( bool connect )
{
     s_logger.LogInfo( "%-18s : A connection %s event has been sent.", s_logName, ( connect ? "on" : "off" ) );

     m_ProductPointer->SetVolume  ( m_currentVolume );
     m_ProductPointer->SetUserMute( m_userMute      );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductDeviceManager::RegisterForProductRequests
///
/// @brief  This method registers for product information requests.
///
/// @param  void This method does not take any arguments.
///
/// @return This method does not return anything.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductDeviceManager::RegisterForProductRequests( )
{
     m_audioDSPControlHandler = new WebExtensionsUrlHandler< audiodspcontrols >
                                    ( m_mainTask ,
                                      WEB_EX_PORT_AUDIO_DSP_CONTROL,
                                      m_audioDSPControlStore,
                                      std::bind( &ProductDeviceManager::SendDSPAudioControls,
                                                  this ),
                                      true,
                                      std::bind( &ProductDeviceManager::SanitizeAudioDSPControls,
                                                  this,
                                                  std::placeholders::_1 ) );

     m_audioProductToneControlHandler = new WebExtensionsUrlHandler< audioproducttonecontrols >
                                            ( m_mainTask ,
                                              WEB_EX_PORT_PRODUCT_TONE_CONTROLS,
                                              m_audioProductToneControlStore,
                                              std::bind( &ProductDeviceManager::SendToneAndLevelControls,
                                                          this ),
                                              true,
                                              std::bind( &ProductDeviceManager::SanitizeAudioToneControls,
                                                          this,
                                                          std::placeholders::_1 ) );

     m_audioProductSpeakerLevelHandler = new WebExtensionsUrlHandler< audioproductlevelcontrols >
                                             ( m_mainTask ,
                                               WEB_EX_PORT_PRODUCT_LEVEL_CONTROLS,
                                               m_audioProductSpeakerLevelStore,
                                               std::bind( &ProductDeviceManager::SendToneAndLevelControls,
                                                           this ),
                                               true,
                                               std::bind( &ProductDeviceManager::SanitizeAudioLevelControls,
                                                           this,
                                                           std::placeholders::_1 ) );

     m_audioSpeakerAttributeAndSettingHandler = new WebExtensionsUrlHandler< audiospeakerattributeandsetting >
                                                    ( m_mainTask ,
                                                      WEB_EX_PORT_SPEAKER_AVAILABILITY,
                                                      m_audioSpeakerAttributeAndSettingStore,
                                                      std::bind( &ProductDeviceManager::SendSpeakerSettings,
                                                                  this ),
                                                      true,
                                                      std::bind( &ProductDeviceManager::SanitizeSpeakerAttributeAndSetting,
                                                                  this,
                                                                  std::placeholders::_1 ) );

     m_systemTimeoutControlHandler = new WebExtensionsUrlHandler< systemtimeoutcontrol >
                                         ( m_mainTask ,
                                           WEB_EX_PORT_PRODUCT_SYSTEM_TIMEOUT_CONTROL,
                                           m_systemTimeoutControlStore,
                                           std::bind( &ProductDeviceManager::SendSystemTimeoutEnableBits,
                                                       this ),
                                           true );

     m_rebroadcastLatencyModeHandler = new WebExtensionsUrlHandler< rebroadcastlatencymode >
                                           ( m_mainTask ,
                                             WEB_EX_PORT_PRODUCT_REBROADCAST_LATENCY_MODE,
                                             m_rebroadcastLatencyModeStore,
                                             std::bind( &ProductDeviceManager::SendRebroadcastLatencyMode,
                                                         this ),
                                             true,
                                             std::bind( &ProductDeviceManager::SanitizeRebroadcastLatencyMode,
                                                         this,
                                                         std::placeholders::_1 ) );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name    ProductDeviceManager::SanitizeAudioDSPControls
///
/// @brief   This method is called before the DSP DRC Mode and Audio Sync Delay is set in the
///          hardware, and is used to check the validity of the data to be set.
///
/// @param   protobuf [input] This argument stores DSP DRC Mode and Audio Sync Delay in the protobuf
///                           structure audiodspcontrols.
///
/// @returns This method returns a false Boolean value if the argument data supplied is invalid. It
///          returns a true Boolean value otherwise.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool ProductDeviceManager::SanitizeAudioDSPControls( audiodspcontrols& protobuf )
{
     if( protobuf.audiomode( ) != AUDIO_MODE_NORMAL && protobuf.audiomode( ) != AUDIO_MODE_DIALOG )
     {
         ///////////////////////////////////////////////////////////////////////////////////////////
         /// An invalid value has been specified in this case.
         ///////////////////////////////////////////////////////////////////////////////////////////
         if( protobuf.has_audiomode( ) )
         {
             return false;
         }

         protobuf.set_audiomode( GetAudioDSPControlStore( ).audiomode( ) );
     }

     ///////////////////////////////////////////////////////////////////////////////////////////
     /// An invariable parameter has been specified in this case.
     ///////////////////////////////////////////////////////////////////////////////////////////
     if( protobuf.has_supportedaudiomodes( ) )
     {
         return false;
     }

     return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductDeviceManager::SendDSPAudioControls
///
/// @brief  This method sends the DSP Audio DRC Mode and Audio Sync Delay to set in the hardware,
///         using the stored values.
///
/// @param  void This method does not take any arguments.
///
/// @return This method does not return anything.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductDeviceManager::SendDSPAudioControls( )
{
     IpcAudioMode_t audioMode  = ( IpcAudioMode_t ) GetAudioDSPControlStore( ).audiomode( );
     uint32_t       audioDelay = ( uint32_t       ) GetAudioDSPControlStore( ).videosyncaudiodelay( );

     std::string audioModeString;

     switch( audioMode )
     {
        case IPC_AUDIO_MODE_UNSPECIFIED:
             audioModeString.assign( "Unspecified" );
             break;
        case IPC_AUDIO_MODE_DIRECT:
             audioModeString.assign( "Direct" );
             break;
        case IPC_AUDIO_MODE_NORMAL:
             audioModeString.assign( "Normal" );
             break;
        case IPC_AUDIO_MODE_DIALOG:
             audioModeString.assign( "Dialog" );
             break;
        case IPC_AUDIO_MODE_NIGHT:
             audioModeString.assign( "Night" );
             break;
        default:
             audioModeString.assign( "Unknown" );
             break;
     }

     s_logger.LogInfo( "%-18s : Audio mode is to be set to %s and audio delay to %d.",
                       s_logName,
                       audioModeString.c_str( ),
                       audioDelay );

     m_ProductHardwareManager->SendSetDSPAudioMode( audioMode );
     m_ProductHardwareManager->SendLipSyncDelay   ( audioDelay );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name    ProductDeviceManager::SanitizeAudioToneControls
///
/// @brief   This method is called before the audio tone control settings are sent to hardware,
///          and is used to check the validity of the audio tone control data.
///
/// @param   protobuf [input] This argument stores audio tone controls in the Protocol Buffer
///                           structure audioproducttonecontrols.
///
/// @returns This method returns a false Boolean value if the argument data supplied is invalid. It
///          returns a true Boolean value otherwise.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool ProductDeviceManager::SanitizeAudioToneControls( audioproducttonecontrols& protobuf ) const
{
     if( protobuf.has_bass( ) )
     {
         const auto& bassControl = protobuf.bass( );

         if( bassControl.has_minvalue( ) ||
             bassControl.has_maxvalue( ) ||
             bassControl.has_step    ( )    )
         {
             return false;
         }

         const auto  bassValue = bassControl.value( );
         const auto& bassStore = m_audioProductToneControlStore.bass( );

         if( ( bassValue < bassStore.minvalue( ) ) ||
             ( bassValue > bassStore.maxvalue( ) ) ||
             ( bassValue % bassStore.step    ( ) )    )
         {
               return false;
         }
     }

     if( protobuf.has_treble( ) )
     {
         const auto& trebleControl = protobuf.treble( );

         if( trebleControl.has_minvalue( ) ||
             trebleControl.has_maxvalue( ) ||
             trebleControl.has_step    ( )    )
         {
             return false;
         }

         const auto  trebleValue = trebleControl.value( );
         const auto& trebleStore = m_audioProductToneControlStore.treble( );

         if( ( trebleValue < trebleStore.minvalue( ) ) ||
             ( trebleValue > trebleStore.maxvalue( ) ) ||
             ( trebleValue % trebleStore.step    ( ) )    )
         {
             return false;
         }
     }

     return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name    ProductDeviceManager::SanitizeAudioLevelControls
///
/// @brief   This method is called before the audio level control settings are sent to the hardware,
///          and is used to check the validity of the audio level control data.
///
/// @param   protobuf [input] This argument stores audio level controls in the protobuf strucutre
///                           audioproductlevelcontrols.
///
/// @returns This method returns a false Boolean value if the argument data supplied is invalid. It
///          returns a true Boolean value otherwise.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool ProductDeviceManager::SanitizeAudioLevelControls( audioproductlevelcontrols& protobuf ) const
{
     if( protobuf.has_frontcenterspeakerlevel( ) )
     {
         const auto& frontcenterspeakerlevelControl = protobuf.frontcenterspeakerlevel( );

         if( frontcenterspeakerlevelControl.has_minvalue( ) ||
             frontcenterspeakerlevelControl.has_maxvalue( ) ||
             frontcenterspeakerlevelControl.has_step    ( )    )
         {
             return false;
         }

         const auto  frontcenterspeakerlevelValue = frontcenterspeakerlevelControl.value( );
         const auto& frontcenterspeakerlevelStore = m_audioProductSpeakerLevelStore.frontcenterspeakerlevel( );

         if( ( frontcenterspeakerlevelValue < frontcenterspeakerlevelStore.minvalue( ) ) ||
             ( frontcenterspeakerlevelValue > frontcenterspeakerlevelStore.maxvalue( ) ) ||
             ( frontcenterspeakerlevelValue % frontcenterspeakerlevelStore.step    ( ) )    )
         {
             return false;
         }
     }

     if( protobuf.has_rearsurroundspeakerslevel( ) )
     {
         const auto& rearsurroundspeakerslevelControl = protobuf.rearsurroundspeakerslevel( );

         if( rearsurroundspeakerslevelControl.has_minvalue( ) ||
             rearsurroundspeakerslevelControl.has_maxvalue( ) ||
             rearsurroundspeakerslevelControl.has_step    ( )    )
         {
             return false;
         }

         const auto  rearsurroundspeakerslevelValue = rearsurroundspeakerslevelControl.value( );
         const auto& rearsurroundspeakerslevelStore = m_audioProductSpeakerLevelStore.rearsurroundspeakerslevel( );

         if( ( rearsurroundspeakerslevelValue < rearsurroundspeakerslevelStore.minvalue( ) ) ||
             ( rearsurroundspeakerslevelValue > rearsurroundspeakerslevelStore.maxvalue( ) ) ||
             ( rearsurroundspeakerslevelValue % rearsurroundspeakerslevelStore.step    ( ) )    )
         {
               return false;
         }
     }

     return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductDeviceManager::SendToneAndLevelControls
///
/// @brief  This method sends the set tone and volume level controls to the hardware, using stored
///         values from two different protobufs.
///
/// @param  void This method does not take any arguments.
///
/// @return This method does not return anything.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductDeviceManager::SendToneAndLevelControls( )
{
     IpcToneControl_t controls         = { 0 };
     const auto&      audioToneControl = GetAudioToneControlStore( );

     controls.bass   = audioToneControl.bass  ( ).value( );
     controls.treble = audioToneControl.treble( ).value( );

     const auto& audioSpeakerLevel = GetAudioSpeakerLevelStore( );

     controls.centerSpeaker   = audioSpeakerLevel.frontcenterspeakerlevel  ( ).value( );
     controls.surroundSpeaker = audioSpeakerLevel.rearsurroundspeakerslevel( ).value( );

     s_logger.LogInfo( "%-18s : Audio tone and level settings are as follows: ", s_logName );
     s_logger.LogInfo( "%-18s :                ", s_logName );
     s_logger.LogInfo( "%-18s : Bass      : %d ", s_logName, controls.bass );
     s_logger.LogInfo( "%-18s : Treble    : %d ", s_logName, controls.treble );
     s_logger.LogInfo( "%-18s : Center    : %d ", s_logName, controls.centerSpeaker );
     s_logger.LogInfo( "%-18s : Surround  : %d ", s_logName, controls.surroundSpeaker );

     m_ProductHardwareManager->SendToneAndLevelControl( controls );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name    ProductDeviceManager::SanitizeSpeakerAttributeAndSetting
///
/// @brief   This method is called before the speaker settings are sent to the client application,
///          and is used to check the validity of the speaker settings data.
///
/// @param   protobuf [input] This argument stores speaker settings in the protobuf strucutre
///                           audiospeakerattributeandsetting.
///
/// @returns This method returns a false Boolean value if the argument data supplied is invalid. It
///          returns a true Boolean value otherwise.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool ProductDeviceManager::SanitizeSpeakerAttributeAndSetting( audiospeakerattributeandsetting& protobuf )
{
     s_logger.LogInfo( "%-18s : Validation of the Speaker settings are to be checked.", s_logName );

     ////////////////////////////////////////////////////////////////////////////////////////////////
     /// The following code is commented out until a hardware interface with the LPM is complete.
     ////////////////////////////////////////////////////////////////////////////////////////////////
     auto rearDataStore = protobuf.mutable_rear( );

     ////////////////////////////////////////////////////////////////////////////////////////////////
     /// Do not allow any activation for non-controllable speakers.
     ////////////////////////////////////////////////////////////////////////////////////////////////
     if( !GetSpeakerAttributeAndSettingStore( ).rear( ).controllable( ) )
     {
          if( rearDataStore->has_active( ) )
          {
              return false;
          }
     }

     if( rearDataStore->has_available   ( ) ||
         rearDataStore->has_wireless    ( ) ||
         rearDataStore->has_controllable( )    )
     {
         return false;
     }

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// Note that only one subwoofer is supported at the moment so the subwoofer02 data is ignored.
    ///////////////////////////////////////////////////////////////////////////////////////////////
    auto subwoofer01DataStore = protobuf.mutable_subwoofer01( );

    ////////////////////////////////////////////////////////////////////////////////////////////////
    /// Do not allow any activation for non-controllable speakers.
    ////////////////////////////////////////////////////////////////////////////////////////////////
    if( !GetSpeakerAttributeAndSettingStore( ).subwoofer01( ).controllable( ) )
    {
          if( subwoofer01DataStore->has_active( ) )
          {
              return false;
          }
    }

    if( subwoofer01DataStore->has_available   ( ) ||
        subwoofer01DataStore->has_wireless    ( ) ||
        subwoofer01DataStore->has_controllable( )    )
    {
        return false;
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductDeviceManager::SendSpeakerSettings
///
/// @brief  This method sends the speaker settings to the hardware. It is called after the
///         device information has been set. The data should have been sanitized to prevent unmatched
///         expectations, such as setting a non-existing accessory on the device to active.
///
/// @param  void This method does not take any arguments.
///
/// @return This method does not return anything.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductDeviceManager::SendSpeakerSettings( )
{
     s_logger.LogInfo( "%-18s : Validation of the Speaker settings are to be checked.", s_logName );

     ////////////////////////////////////////////////////////////////////////////////////////////////
     /// The following code is commented out until a hardware interface with the LPM is complete.
     ////////////////////////////////////////////////////////////////////////////////////////////////

     IpcAccessoryList_t accessoryList;
     const audiospeakerattributeandsetting& speakerAttributeAndSetting = GetSpeakerAttributeAndSettingStore( );

     ///////////////////////////////////////////////////////////////////////////////////////////////
     /// Note that only one subwoofer is supported at the moment so the subwoofer02 data is ignored.
     ///////////////////////////////////////////////////////////////////////////////////////////////
     accessoryList.accessory[ ACCESSORY_POSITION_SUB ].active =
     speakerAttributeAndSetting.subwoofer01( ).active( ) ? ACCESSORY_ACTIVATED : ACCESSORY_DEACTIVATED;

     accessoryList.accessory[ ACCESSORY_POSITION_LEFT_REAR ].active =
     speakerAttributeAndSetting.rear( ).active( ) ? ACCESSORY_ACTIVATED : ACCESSORY_DEACTIVATED;

     accessoryList.accessory[ ACCESSORY_POSITION_RIGHT_REAR ].active =
     speakerAttributeAndSetting.rear( ).active( ) ? ACCESSORY_ACTIVATED : ACCESSORY_DEACTIVATED;

     s_logger.LogInfo( "%-18s : Speaker activation settings are as follows: ", s_logName );
     s_logger.LogInfo( "%-18s :                                             ", s_logName );
     s_logger.LogInfo( "%-18s : Left  Speaker : %d ", s_logName, ( uint32_t )accessoryList.accessory[ACCESSORY_POSITION_LEFT_REAR].active );
     s_logger.LogInfo( "%-18s : Right Speaker : %d ", s_logName, ( uint32_t )accessoryList.accessory[ACCESSORY_POSITION_RIGHT_REAR].active );
     s_logger.LogInfo( "%-18s : Sub   Speaker : %d ", s_logName, ( uint32_t )accessoryList.accessory[ACCESSORY_POSITION_SUB].active );

     m_ProductHardwareManager->SendSpeakerList( accessoryList );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductDeviceManager::SendSystemTimeoutEnableBits
///
/// @brief  This method sends time out control information to the hardware.
///
/// @param  void This method does not take any arguments.
///
/// @return This method does not return anything.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductDeviceManager::SendSystemTimeoutEnableBits( )
{
     Ipc_TimeoutControl_t timeoutControl;

     timeoutControl.cmd     = ( Ipc_TimeoutCommand_t )( TIMEOUT_CMD_SET_ENABLED );
     timeoutControl.enable  = GetSystemTimeoutEnableBits( ).autopowerdown( );
     timeoutControl.timeout = IPC_TIMEOUT_USER_TIMEOUTS;

     s_logger.LogInfo( "%-18s : Auto power down will be set to %s.", s_logName, timeoutControl.enable ? "on" : "off" );

     m_ProductHardwareManager->SendSetSystemTimeoutEnableBits( timeoutControl );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name    SanitizeRebroadcastLatencyMode
///
/// @brief   called when POST rebroadcastlatecymode is received from API caller
///
/// @param   rebroadcastlatecymode& [input] protobuf
///
/// @returns false if arguments are insane
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool ProductDeviceManager::SanitizeRebroadcastLatencyMode( rebroadcastlatencymode& protobuf )
{
     if( protobuf.has_controllable( ) )
     {
         return false;
     }

     if( !protobuf.has_mode( ) )
     {
         protobuf.set_mode( GetRebroadcastLatencyModeStore( ).mode( ) );
     }

     return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name SendRebroadcastLatencyMode
///
/// @brief Sends the Rebroadcast Latency Mode to the LPM. This method is called after the device instance data
///        has been set . Also persists the value.
///
/// @returns void
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductDeviceManager::SendRebroadcastLatencyMode( )
{
     std::string latencyMode = REBROADCAST_LATENCY_MODE_Name( GetRebroadcastLatencyModeStore( ).mode( ) );

     s_logger.LogInfo( "%-18s : A latency value of %u is being set based on a %s mode.",
                       s_logName,
                       m_zonePresentationLatency,
                       latencyMode.c_str( ) );

     ///////////////////////////////////////////////////////////////////////////////////////////////
     /// The latency is set based on the argument. Multi-room zone information will need to be
     /// processed in future development.
     ///////////////////////////////////////////////////////////////////////////////////////////////
     m_ProductHardwareManager->SendAudioPathPresentationLatency( m_zonePresentationLatency );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductDeviceManager::RegisterForRebootRequests
///
/// @brief  This method registers for reboot requests.
///
/// @param  void This method does not take any arguments.
///
/// @return This method does not return anything.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductDeviceManager::RegisterForRebootRequests( )
{
     m_serverListener = IL::CreateServerListener( "ProductDeviceManagerListener", m_mainTask );


     AsyncCallback< ServerSocket > callback( std::bind( &ProductDeviceManager::AcceptClient,
                                                        this,
                                                        std::placeholders::_1 ),
                                             m_mainTask );

     m_serverListener->Serve( IPCDirectory::Get( )->DefaultAddress( A4V_SERVER ), callback );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductDeviceManager::AcceptClient
///
/// @brief  This method accepts and establishes client connections for making reboot requests.
///
/// @param  client [input] This argument is a pointer to a client socket class instance that wishes
///                        to connect and register for reboot requests.
///
/// @return This method does not return anything.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductDeviceManager::AcceptClient( ServerSocket client )
{
     std::string   clientName   = client->GetPeerAddrInfo( ).ToString( );
     RouterPointer messageRouter = IPCMessageRouterFactory::CreateRouter( "ServerRouter" + clientName,
                                                                           m_mainTask );

     s_logger.LogInfo( "%-18s : A client connection %s for reboot requests has been established.",
                       s_logName,
                       clientName.c_str( ) );

     ///////////////////////////////////////////////////////////////////////////////////////////////
     /// The callback for handling reboot requests is established and attached to a message router
     /// for processing.
     ///////////////////////////////////////////////////////////////////////////////////////////////
     {
         AsyncCallback< BoseLinkServerMsgReboot > callback( std::bind( &ProductDeviceManager::SendRebootRequestHandler,
                                                                        this,
                                                                        std::placeholders::_1 ),
                                                            m_mainTask );

         messageRouter->Attach< BoseLinkServerMsgReboot >( BOSELINK_SERVER_MSG_ID_REBOOT, callback );
     }

     ///////////////////////////////////////////////////////////////////////////////////////////////
     /// The callback for handling client disconnections is created for the message router, and
     /// message routing is served establishing the client connection.
     ///////////////////////////////////////////////////////////////////////////////////////////////
     {
         AsyncCallback< void > callback( std::bind( &ProductDeviceManager::HandleClientDisconnect,
                                                     this ),
                                         m_mainTask );

         messageRouter->Serve( std::move( client ), callback );
     }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductDeviceManager::HandleClientDisconnect
///
/// @brief  This method is a callback for handling client disconnections for reboot requests.
///
/// @param  void This method does not take any arguments.
///
/// @return This method does not return anything.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductDeviceManager::HandleClientDisconnect( )
{
     s_logger.LogInfo( "%-18s : A client connection for reboot requests has been disconnected.", s_logName );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductDeviceManager::SendRebootRequestHandler
///
/// @brief  This method is used to send a reboot request to the LPM hardware. It is used to called
///         another method SendRebootRequest, which actually sends the request to the hardware. The
///         SendRebootRequest is public though and can also be called through the system interface
///         directly.
///
/// @param  BoseLinkServerMsgReboot [input] This argument contains the reboot message request data.
///
/// @return This method does not return anything.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductDeviceManager::SendRebootRequestHandler( BoseLinkServerMsgReboot rebootRequest)
{
     s_logger.LogInfo( "%-18s : A reboot after a %d delay has been requested.", s_logName, rebootRequest.delay( ) );

     unsigned int delay = rebootRequest.delay( );

     SendRebootRequest( delay );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductDeviceManager::SendRebootRequest
///
/// @brief  This method is used to send a reboot request to the LPM hardware.
///
/// @param  BoseLinkServerMsgReboot [input] This argument contains the reboot message request data.
///
/// @return This method does not return anything.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductDeviceManager::SendRebootRequest( unsigned int delay )
{
     s_logger.LogInfo( "%-18s : A reboot after a %d delay is being processed.", s_logName, delay );

     sleep( delay );

     m_ProductHardwareManager->RebootRequest( );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductDeviceManager::SetBlueToothMacAddress
///
/// @brief  This method is used to set the Bluetooth MAC Address and send it to the LPM hardware,
///         as long as the associated Bluetooth device name has been previously obtained.
///
/// @param  bluetoothMacAddress [input] This argument is a standard string representing the
///                                     Bluetooth MAC Address.
///
/// @return This method does not return anything.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductDeviceManager::SetBlueToothMacAddress( const std::string& bluetoothMacAddress )
{
     ///////////////////////////////////////////////////////////////////////////////////////////////
     /// The Bluetooth MAC Address needs to be reformatted to remove and colon characters. For
     /// example, a Bluetooth MAC Address 88:4A:EA:5A:37:AD of would be reformatted to 884AEA5A37AD.
     /// ///////////////////////////////////////////////////////////////////////////////////////////////
     char blueToothReformattedMacAddress[ ( 2 * BLUETOOTH_MAC_LENGTH ) + 1 ];

     blueToothReformattedMacAddress[  0 ] = bluetoothMacAddress[  0 ];
     blueToothReformattedMacAddress[  1 ] = bluetoothMacAddress[  1 ];
     blueToothReformattedMacAddress[  2 ] = bluetoothMacAddress[  3 ];
     blueToothReformattedMacAddress[  3 ] = bluetoothMacAddress[  4 ];
     blueToothReformattedMacAddress[  4 ] = bluetoothMacAddress[  6 ];
     blueToothReformattedMacAddress[  5 ] = bluetoothMacAddress[  7 ];
     blueToothReformattedMacAddress[  6 ] = bluetoothMacAddress[  9 ];
     blueToothReformattedMacAddress[  7 ] = bluetoothMacAddress[ 10 ];
     blueToothReformattedMacAddress[  8 ] = bluetoothMacAddress[ 12 ];
     blueToothReformattedMacAddress[  9 ] = bluetoothMacAddress[ 13 ];
     blueToothReformattedMacAddress[ 10 ] = bluetoothMacAddress[ 15 ];
     blueToothReformattedMacAddress[ 11 ] = bluetoothMacAddress[ 16 ];
     blueToothReformattedMacAddress[ 12 ] = '\0';

     m_blueToothMacAddress = strtoull( blueToothReformattedMacAddress, nullptr, 16 );

     if( m_gettingBlueToothData )
     {
         SendBlueToothDeviceData( m_blueToothDeviceName, m_blueToothMacAddress );
         m_gettingBlueToothData = false;
     }
     else
     {
         m_gettingBlueToothData = true;
     }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductDeviceManager::SetBlueToothDeviceName
///
/// @brief  This method is used to set the Bluetooth MAC Address and send it to the LPM hardware,
///         as long as the associated Bluetooth device name has been previously obtained.
///
/// @param  bluetoothDeviceName [input] This argument is a standard string representing the
///                                     Bluetooth device name.
///
/// @return This method does not return anything.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductDeviceManager::SetBlueToothDeviceName( const std::string& bluetoothDeviceName )
{
     m_blueToothDeviceName.assign( bluetoothDeviceName );

     if( m_gettingBlueToothData )
     {
         SendBlueToothDeviceData( m_blueToothDeviceName, m_blueToothMacAddress );
         m_gettingBlueToothData = false;
     }
     else
     {
         m_gettingBlueToothData = true;
     }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductDeviceManager::SendBlueToothDeviceData
///
/// @brief  This method is used to send the Bluetooth device and MAC Address data to the LPM hardware.
///
/// @param  bluetoothDeviceName [input] This argument is a standard string representing the
///                                     Bluetooth device name.
///
/// @param  bluetoothMacAddress [input] This argument is a standard string representing the
///                                     Bluetooth MAC Address.
///
/// @return This method does not return anything.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductDeviceManager::SendBlueToothDeviceData( const std::string&       bluetoothDeviceName,
                                                    const unsigned long long bluetoothMacAddress )
{
     m_ProductHardwareManager->SendBlueToothDeviceData( bluetoothDeviceName, bluetoothMacAddress );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductDeviceManager::HandleLowPowerStandby
///
/// @brief  This method is used to set the system in low power standby mode through the LPM hardware.
///
/// @param  void This method does not take any arguments.
///
/// @return This method does not return anything.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductDeviceManager::HandleLowPowerStandby( )
{
     m_ProductHardwareManager->HandleLowPowerStandby( );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
