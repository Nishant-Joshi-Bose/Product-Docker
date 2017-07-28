////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file      ProductSystemInterface.h
///
/// @brief     This file contains source code that implements the ProductSystemInterface class that
///            acts as a container to handle all the main functionality related to this program that
///            is not product specific. In these regards, this class is used as a container to control
///            the product states, as well as to instantiate subclasses to manage the device and
///            lower level hardware, and interface with the user and higher level applications.
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
#include "SystemUtils.h"            /// This file contains Bose system utility declarations.
#include "DPrint.h"                 /// This file contains the DPrint class used for logging.
#include "CliClient.h"              /// This file declares functionality for a command line interface.
#include "SystemService.pb.h"
#include "ProductController.h"      /// This file declares the ProductController class.
#include "ProductHardwareManager.h" /// This file declares the ProductHardwareManager class.
#include "ProductDeviceManager.h"   /// This file declares the ProductDeviceManager class.
#include "ProductUserInterface.h"   /// This file declares the ProductUserInterface class.
#include "ProductSystemInterface.h" /// This file declares the ProductSystemInterface class.

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

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// The following declares a DPrint class type object and a standard string for logging information
/// in this source code file.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
static const DPrint s_logger    { "Product System" };
static const char   s_logName[] = "Product System"  ;

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductSystemInterface::GetInstance
///
/// @brief  This static method creates the one and only instance of a ProductSystemInterface object.
///         The C++ Version 11 compiler guarantees that only one instance is created in a thread
///         safe way.
///
/// @param  void This method does not take any arguments.
///
/// @return This method returns a pointer to a ProductSystemInterface object.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
ProductSystemInterface* ProductSystemInterface::GetInstance( )
{
       static ProductSystemInterface* instance = new ProductSystemInterface( );

       return instance;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductSystemInterface::ProductSystemInterface
///
/// @brief  This method is the ProductSystemInterface constructor, which is declared as being
///         private to ensure that only one instance of this class can be created through the class
///         GetInstance method.
///
/// @param  void This method does not take any arguments.
///
/// @return This method does not return anything.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
ProductSystemInterface::ProductSystemInterface( )
                      : APTask( "ProductSystemInterfaceMainTask" ),
                        m_ProductHardwareManager ( ProductHardwareManager::GetInstance( ) ),
                        m_ProductDeviceManager   ( ProductDeviceManager  ::GetInstance( ) ),
                        m_ProductUserInterface   ( ProductUserInterface  ::GetInstance( ) ),
                        m_ProductController      ( ProductController     ::GetInstance( ) )
{
       return;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductSystemInterface::~ProductSystemInterface
///
/// @brief  This method is the ProductSystemInterface destructor, which stops the Sound Touch system
///         process.
///
/// @param  void This method does not take any arguments.
///
/// @return This method does not return anything.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
ProductSystemInterface::~ProductSystemInterface( )
{
       m_running = false;

       return;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductSystemInterface::Run
///
/// @brief  This method starts the main task for the ProductSystemInterface instance. The OnEntry method
///         for the ProductSystemInterface instance is called just before the main task starts. Also,
///         this main task is used for most of the internal processing for each of the subclass
///         instances.
///
/// @param  void This method does not take any arguments.
///
/// @return This method does not return anything.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductSystemInterface::Run( )
{
     m_running  = true;
     m_mainTask = this;

     this->Start( );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name  ProductSystemInterface::OnEntry
///
/// @param  void This method does not take any arguments.
///
/// @return This method does not return anything.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductSystemInterface::OnEntry( )
{
    IL::BreakThread( std::bind( &ProductSystemInterface::Process,
                                this ),
                     m_mainTask );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name  ProductSystemInterface::Process
///
/// @param  void This method does not take any arguments.
///
/// @return This method does not return anything.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductSystemInterface::Process( void )
{
     while( m_running )
     {
        ProductSystemInterface::GetInstance( )->ClientSocket::Run( );
     }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name  ProductSystemInterface::End
///
/// @param  void This method does not take any arguments.
///
/// @return This method does not return anything.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductSystemInterface::End( void )
{
     m_running = false;

     Stop( );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name  ProductSystemInterface::OnMessage
///
/// @param message
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductSystemInterface::OnMessage( SoundTouchInterface::updates_VolumeUpdate message )
{
     s_logger.LogInfo("%s: An Sound Touch volume update message was sent. ", s_logName);

     bool        hasTargetVolumeValue = false;
     bool        hasActualVolumeValue = false;
     bool        hasMuteSettingValue  = false;
     bool        hasDeviceIdValue     = false;
     uint32_t    targetVolumeValue    = 0;
     uint32_t    actualVolumeValue    = 0;
     bool        muteSettingValue     = false;
     std::string deviceIdValue        ( "" );

     if( message.has_volume( ) )
     {
         SoundTouchInterface::volume volumeSettings = message.volume( );

         if( volumeSettings.has_targetvolume( ) )
         {
             SoundTouchInterface::volume_TargetVolume targetVolume = volumeSettings.targetvolume( );
             if( targetVolume.has_text( ) )
             {
                 hasTargetVolumeValue = true;
                 targetVolumeValue    = targetVolume.text( );
             }
         }
         if( volumeSettings.has_actualvolume( ) )
         {
             SoundTouchInterface::volume_ActualVolume actualVolume = volumeSettings.actualvolume( );
             if( actualVolume.has_text( ) )
             {
                 hasActualVolumeValue = true;
                 actualVolumeValue    = actualVolume.text( );
             }
         }
         if( volumeSettings.has_muteenabled( ) )
         {
             SoundTouchInterface::volume_MuteEnabled muteSetting = volumeSettings.muteenabled( );
             if( muteSetting.has_text( ) )
             {
                 hasMuteSettingValue = true;
                 muteSettingValue    = muteSetting.text( );
             }
         }
         if( volumeSettings.has_deviceid( ) )
         {
             hasDeviceIdValue = true;
             deviceIdValue    = volumeSettings.deviceid( );
         }
     }

     s_logger.LogInfo("%s:The volume update has occurred with the following parameters:", s_logName);
     s_logger.LogInfo("%s:                                                             ", s_logName);
     s_logger.LogInfo("%s:    Actual Volume : %s ", s_logName, hasTargetVolumeValue ? std::to_string( targetVolumeValue ).c_str( ) : "Unspecified");
     s_logger.LogInfo("%s:    Target Volume : %s ", s_logName, hasActualVolumeValue ? std::to_string( actualVolumeValue ).c_str( ) : "Unspecified");
     s_logger.LogInfo("%s:    Mute Setting  : %s ", s_logName, hasMuteSettingValue  ? ( muteSettingValue ? "On" : "Off" ) : "Unspecified");
     s_logger.LogInfo("%s:    Device ID     : %s ", s_logName, hasDeviceIdValue     ? ( deviceIdValue.c_str( ) ): "Unspecified");
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name  ProductSystemInterface::OnMessage
///
/// @param message
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductSystemInterface::OnMessage( SoundTouchInterface::criticalErrorUpdate message )
{
     s_logger.LogInfo("%s: A Sound Touch critical error update message was sent.", s_logName);

     ///////////////////////////////////////////////////////////////////////////////////////////////
     /// The critical error is just output at this point. Handling of these errors are to be
     /// developed later.
     ///////////////////////////////////////////////////////////////////////////////////////////////
     if( message.has_criticalerror( ) )
     {
         if( message.criticalerror( ).has_type( ) )
         {
             switch( message.criticalerror( ).type( ) )
             {
                case SoundTouchInterface::CriticalErrorType::ERROR_NONE:
                     s_logger.LogInfo("%s: There was no critical error.",
                                      s_logName);
                     break;
                case SoundTouchInterface::CriticalErrorType::ERROR_BATTERY_EXTREME_TEMP:
                     s_logger.LogInfo("%s: The battery is at an extreme temperature.",
                                      s_logName);
                     break;
                case SoundTouchInterface::CriticalErrorType::ERROR_BATTERY_PRECHARGE:
                     s_logger.LogInfo("%s: The battery is in a precharge state.",
                                      s_logName);
                     break;
                case SoundTouchInterface::CriticalErrorType::ERROR_AMP_FAULT:
                     s_logger.LogInfo("%s: An amp fault has occurred.",
                                      s_logName);
                     break;
                case SoundTouchInterface::CriticalErrorType::ERROR_FACTORY_RESET:
                     s_logger.LogInfo("%s: A factory reset is taking place.",
                                      s_logName);
                     break;
                case SoundTouchInterface::CriticalErrorType::ERROR_PRODUCT_UPDATE_REQUIRED:
                     s_logger.LogInfo("%s: A product update is required.",
                                      s_logName);
                     break;
                default:
                     s_logger.LogInfo("%s: An unknown critical error %d has occurred.",
                                      s_logName,
                                      message.criticalerror( ).type( ) );
                     break;
             }
         }
         else
         {
             s_logger.LogInfo("%s: No critical error type was specified.", s_logName);
         }
     }
     else
     {
         s_logger.LogInfo("%s: No critical error was specified.", s_logName);
     }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name  ProductSystemInterface::OnMessage
///
/// @param message
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductSystemInterface::OnMessage( SoundTouchInterface::updates_NowPlayingUpdate message )
{
     s_logger.LogInfo("%s: A Sound Touch now playing update message was sent.", s_logName);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name  ProductSystemInterface::OnMessage
///
/// @param message
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductSystemInterface::OnMessage( SoundTouchInterface::updates_PresetsUpdate message )
{
     s_logger.LogInfo("%s: A Sound Touch presets update message was sent.", s_logName);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name  ProductSystemInterface::OnMessage
///
/// @param message
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductSystemInterface::OnMessage( SoundTouchInterface::updates_RecentsUpdate message )
{
     s_logger.LogInfo("%s: A Sound Touch recent update message was sent.", s_logName);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name  ProductSystemInterface::OnMessage
///
/// @param message
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductSystemInterface::OnMessage( SoundTouchInterface::updates_BrowseUpdate message )
{
     s_logger.LogInfo("%s: A Sound Touch browse update message was sent.", s_logName);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name  ProductSystemInterface::OnMessage
///
/// @param message
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductSystemInterface::OnMessage( SoundTouchInterface::updates_NowSelectionUpdate message )
{
     s_logger.LogInfo("%s: A Sound Touch now selection update message was sent.", s_logName);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name  ProductSystemInterface::OnMessage
///
/// @param message
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductSystemInterface::OnMessage( SoundTouchInterface::updates_SourcesUpdate message )
{
     s_logger.LogInfo("%s: A Sound Touch sources update message was sent.", s_logName);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name  ProductSystemInterface::OnMessage
///
/// @param message
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductSystemInterface::OnMessage( SoundTouchInterface::updates_SwUpdateStatusUpdate message )
{
     s_logger.LogInfo("%s: A Sound Touch software update status message was sent.", s_logName);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name  ProductSystemInterface::OnMessage
///
/// @param message
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductSystemInterface::OnMessage( SoundTouchInterface::updates_ConnectionStateUpdate message )
{
     s_logger.LogInfo("%s: A Sound Touch connection state update message was sent.", s_logName);

     ///////////////////////////////////////////////////////////////////////////////////////////////
     /// The following call into the SoundTouch::Client class method, which this class inherits
     /// is made. This call may not need to be made based on the current product state.
     ///////////////////////////////////////////////////////////////////////////////////////////////
     ProductSystemInterface::GetInstance( )->GetNetworkInfo( );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name  ProductSystemInterface::OnMessage
///
/// @param message
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductSystemInterface::OnMessage( SoundTouchInterface::updates_GroupUpdate message )
{
     s_logger.LogInfo("%s: A Sound Touch group update message was sent.", s_logName);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name  ProductSystemInterface::OnMessage
///
/// @param message
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductSystemInterface::OnMessage( SoundTouchInterface::updates_LanguageUpdate message )
{
     s_logger.LogInfo("%s: A Sound Touch language update message was sent.", s_logName);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name  ProductSystemInterface::OnMessage
///
/// @param message
///
/// @param requestId
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductSystemInterface::OnMessage( SoundTouchInterface::BluetoothInfo message,
                                        int32_t                            requestId )
{
     s_logger.LogInfo("%s: A Sound Touch Bluetooth information message was sent.", s_logName);

     if( message.has_bluetoothmacaddress( ) )
     {
         s_logger.LogError("%s: The Bluetooth MAC Address is %s.",
                           s_logName,
                           message.bluetoothmacaddress( ).c_str( ) );

         m_ProductDeviceManager->SetBlueToothMacAddress( message.bluetoothmacaddress( ) );
     }
     else
     {
         s_logger.LogError("%s: The Bluetooth message has no MAC Address specified.", s_logName );

         return;
     }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name  ProductSystemInterface::OnMessage
///
/// @param message
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductSystemInterface::OnMessage( SoundTouchInterface::updates_NameUpdate message )
{
     s_logger.LogInfo("%s: A Bluetooth device name update message was sent.", s_logName);

     if( message.has_text( ) )
     {
         s_logger.LogInfo("%s: The Bluetooth device name is %s.", s_logName, message.text( ).c_str( ) );

         m_ProductDeviceManager->SetBlueToothDeviceName( message.text( ) );
     }
     else
     {
         s_logger.LogInfo("%s: The Bluetooth device name was not specified.", s_logName);
     }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name  ProductSystemInterface::OnMessage
///
/// @param message
///
/// @param requestId
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductSystemInterface::OnMessage( SoundTouchInterface::name message,
                                        int32_t                   requestId )
{
     s_logger.LogInfo("%s: A Bluetooth device name message was sent.", s_logName);

     if( message.has_text( ) )
     {
         s_logger.LogInfo("%s: The Bluetooth device name is %s.", s_logName, message.text( ).c_str( ) );

         m_ProductDeviceManager->SetBlueToothDeviceName( message.text( ) );
     }
     else
     {
         s_logger.LogInfo("%s :The Bluetooth device name was not specified.", s_logName);
     }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name  ProductSystemInterface::OnMessage
///
/// @param message
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductSystemInterface::OnMessage( SoundTouchInterface::updates_SetupAPUpdate message )
{
     s_logger.LogInfo("%s: A Sound Touch AP update message was sent.", s_logName);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name  ProductSystemInterface::OnMessage
///
/// @param message
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductSystemInterface::OnMessage( SoundTouchInterface::updates_SoundTouchConfigurationUpdate message )
{
     s_logger.LogInfo("%s:An Sound Touch configuration update message was sent.", s_logName);

     if( message.has_soundtouchconfigurationstatus( ) )
     {
         SoundTouchInterface::SoundTouchConfigurationStatus configurationStatus = message.soundtouchconfigurationstatus( );

         if( configurationStatus.has_status( ) )
         {
             switch( configurationStatus.status( ) )
             {
                case SoundTouchInterface::SoundTouchConfigurationState::SOUNDTOUCH_CONFIGURED:
                     s_logger.LogInfo("%s: The Sound Touch is in a configured state.", s_logName);
                     break;
                case SoundTouchInterface::SoundTouchConfigurationState::SOUNDTOUCH_NOT_CONFIGURED:
                     s_logger.LogInfo("%s: The Sound Touch is not in a configured state.", s_logName);
                     break;
                case SoundTouchInterface::SoundTouchConfigurationState::SOUNDTOUCH_CONFIGURING:
                     s_logger.LogInfo("%s: The Sound Touch is in a configuring state.", s_logName);
                     break;
                default:
                     s_logger.LogInfo("%s: The Sound Touch is in an unknown %d state.", s_logName,
                                       configurationStatus.status( ));
                     break;
             }
         }
     }
     else
     {
         s_logger.LogError("%s: The Sound Touch state is not specified.", s_logName);
     }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name  ProductSystemInterface::OnMessage
///
/// @param message
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductSystemInterface::OnMessage( SoundTouchInterface::userActivityUpdate message )
{
     s_logger.LogInfo("%s: A Sound Touch user activity update message was sent.", s_logName);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name  ProductSystemInterface::OnMessage
///
/// @param message
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductSystemInterface::OnMessage( SoundTouchInterface::userInactivityUpdate message )
{
     s_logger.LogInfo("%s: A Sound Touch user inactivity update message was sent.", s_logName);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name  ProductSystemInterface::OnMessage
///
/// @param message
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductSystemInterface::OnMessage( SoundTouchInterface::LowPowerStandbyUpdate message )
{
     s_logger.LogInfo("%s: A Sound Touch low power standby update message was sent.", s_logName);

     if( message.has_deviceid( ) )
     {
         s_logger.LogInfo("%s: A Sound Touch low power standby was sent for the device %s.",
                          s_logName,
                          message.deviceid( ).c_str( ) );
     }
     else
     {
         s_logger.LogInfo("%s: A Sound Touch low power standby was sent for an unspecified device.",
                          s_logName);
     }

     m_ProductDeviceManager->HandleLowPowerStandby( );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name  ProductSystemInterface::OnMessage
///
/// @param message
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductSystemInterface::OnMessage( SoundTouchInterface::SoftwareUpdateStatus message )
{
     s_logger.LogInfo("%s: A SoftwareUpdateStatus message was sent.", s_logName);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name  ProductSystemInterface::OnMessage
///
/// @param message
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductSystemInterface::OnMessage( SoundTouchInterface::errorUpdate message )
{
     s_logger.LogInfo("%s: A Sound Touch error update message was sent.", s_logName);

     ///////////////////////////////////////////////////////////////////////////////////////////////
     /// Handling of these errors are to be developed later.
     ///////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name  ProductSystemInterface::OnMessage
///
/// @param message
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductSystemInterface::OnMessage( SoundTouchInterface::SoundTouchSdkInfo message )
{
     s_logger.LogInfo("%s: A SoundTouch SDK information message was sent.", s_logName);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name  ProductSystemInterface::OnMessage
///
/// @param message
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductSystemInterface::OnMessage( SoundTouchInterface::reboot message )
{
     s_logger.LogInfo("%s: A Sound Touch reboot message was sent.", s_logName);

     m_ProductDeviceManager->SendRebootRequest( 0 );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// The remaining OnMessage methods are yet to be developed.
////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name  ProductSystemInterface::OnMessage
///
/// @param message
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductSystemInterface::OnMessage( SoundTouchInterface::updates_ZoneUpdate message )
{
     s_logger.LogInfo("%s: A Sound Touch zone update message was sent.", s_logName);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name  ProductSystemInterface::OnMessage
///
/// @param message
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductSystemInterface::OnMessage( SoundTouchInterface::updates_InfoUpdate message )
{
     s_logger.LogInfo("%s: A Sound Touch information update message was sent.", s_logName);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name  ProductSystemInterface::OnMessage
///
/// @param message
///
/// @param requestId
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductSystemInterface::OnMessage( SoundTouchInterface::nowPlaying message,
                                        int32_t                         requestId )
{
     s_logger.LogInfo("%s: A Sound Touch now playing message was sent.", s_logName);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name  ProductSystemInterface::OnMessage
///
/// @param message
///
/// @param requestId
///
///////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductSystemInterface::OnMessage( SoundTouchInterface::presets message,
                                        int32_t                      requestId )
{
     s_logger.LogInfo("%s: A Sound Touch presets message was sent.", s_logName);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name  ProductSystemInterface::OnMessage
///
/// @param message
///
/// @param requestId
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductSystemInterface::OnMessage( SoundTouchInterface::recents message,
                                        int32_t                      requestId )
{
     s_logger.LogInfo("%s: A Sound Touch recents message was sent.", s_logName);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name  ProductSystemInterface::OnMessage
///
/// @param message
///
/// @param requestId
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductSystemInterface::OnMessage( SoundTouchInterface::sources message,
                                        int32_t                      requestId )
{
     s_logger.LogInfo("%s: A Sound Touch sources message was sent.", s_logName);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name  ProductSystemInterface::OnMessage
///
/// @param message
///
/// @param requestId
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductSystemInterface::OnMessage( SoundTouchInterface::zone message,
                                        int32_t                   requestId )
{
     s_logger.LogInfo("%s: A Sound Touch zone message was sent.", s_logName);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name  ProductSystemInterface::OnMessage
///
/// @param message
///
/// @param requestId
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductSystemInterface::OnMessage( SoundTouchInterface::status message,
                                        int32_t                     requestId )
{
     s_logger.LogInfo("%s: A Sound Touch status message was sent.", s_logName);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name  ProductSystemInterface::OnMessage
///
/// @param message
///
/// @param requestId
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductSystemInterface::OnMessage( SoundTouchInterface::errors message,
                                        int32_t                     requestId )
{
     s_logger.LogInfo("%s: A Sound Touch error message was sent.", s_logName);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name  ProductSystemInterface::OnMessage
///
/// @param message
///
/// @param requestId
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductSystemInterface::OnMessage( SoundTouchInterface::networkInfo message,
                                        int32_t                          requestId )
{
     s_logger.LogInfo("%s: A Sound Touch network information message was sent.", s_logName);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name  ProductSystemInterface::OnMessage
///
/// @param message
///
/// @param requestId
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductSystemInterface::OnMessage( SoundTouchInterface::GetActiveWirelessProfileResponse message,
                                        int32_t                                               requestId )
{
     s_logger.LogInfo("%s: A Sound Touch active wireless profile response message was sent.", s_logName);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name  ProductSystemInterface::OnMessage
///
/// @param message
///
/// @param requestId
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductSystemInterface::OnMessage( SoundTouchInterface::sysLanguage message,
                                        int32_t                          requestId )
{
     s_logger.LogInfo("%s: A Sound Touch system language message was sent.", s_logName);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name  ProductSystemInterface::OnMessage
///
/// @param message
///
/// @param requestId
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductSystemInterface::OnMessage( SoundTouchInterface::volume message,
                                        int32_t                     requestId )
{
     s_logger.LogInfo("%s: A Sound Touch volume message was sent.", s_logName);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name  ProductSystemInterface::OnMessage
///
/// @param message
///
/// @param requestId
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductSystemInterface::OnMessage( SoundTouchInterface::setupStateResponse message,
                                        int32_t                                 requestId )
{
     s_logger.LogInfo("%s: A Sound Touch setup state response message was sent.", s_logName);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name  ProductSystemInterface::OnMessage
///
/// @param message
///
/// @param requestId
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductSystemInterface::OnMessage( SoundTouchInterface::setupState message,
                                        int32_t                         requestId )
{
     s_logger.LogInfo("%s: A Sound Touch setup state message was sent.", s_logName);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name  ProductSystemInterface::OnMessage
///
/// @param message
///
/// @param requestId
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductSystemInterface::OnMessage( SoundTouchInterface::SoundTouchConfigurationStatus message,
                                        int32_t                                            requestId )
{
     s_logger.LogInfo("%s: A Sound Touch configuration status message was sent.", s_logName);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name  ProductSystemInterface::OnMessage
///
/// @param message
///
/// @param requestId
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductSystemInterface::OnMessage( WebInterface::pingRequest message,
                                        int32_t                   requestId )
{
     s_logger.LogInfo("%s: A Sound Touch ping request message was sent.", s_logName);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name  ProductSystemInterface::OnMessage
///
/// @param request
///
/// @param message
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductSystemInterface::OnMessage( SoundTouchInterface::activateProductRequest request,
                                        SoundTouchInterface::msg                    message )
{
     s_logger.LogInfo("%s: A Sound Touch activate product request message was sent.", s_logName);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name  ProductSystemInterface::OnMessage
///
/// @param request
///
/// @param message
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductSystemInterface::OnMessage( SoundTouchInterface::initializationCompleteRequest request,
                                        SoundTouchInterface::msg                           message )
{
     s_logger.LogInfo("%s: A Sound Touch initialization complete request message was sent.", s_logName);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief ProductSystemInterface::OnMessage
///
/// @param request
///
/// @param message
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductSystemInterface::OnMessage( SoundTouchInterface::toggleStandbyRequest request,
                                        SoundTouchInterface::msg                  message )
{
     s_logger.LogInfo("%s: A Sound Touch toggle standby request message was sent.", s_logName);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
