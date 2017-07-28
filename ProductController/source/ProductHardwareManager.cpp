////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file      ProductHardwareManager.h
///
/// @name  ProductHardwareManager::    This header file contains declarations for managing the hardware, which interfaces
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
///            Included Header Files
///
////////////////////////////////////////////////////////////////////////////////////////////////////
#include "SystemUtils.h"               /// This file contains system utility declarations.
#include "DPrint.h"                    /// This file contains the DPrint class used for logging.
#include "Services.h"                  /// This file declares application server names.
#include "LpmService.pb.h"             /// This file declares LPM hardware Protocol Buffer data.
#include "RebroadcastLatencyMode.pb.h" /// This file contains latency data in Protocol Buffer format.
#include "ProductHardwareManager.h"    /// This file declares the ProductHardwareManager class.
#include "ProductDeviceManager.h"      /// This file declares the ProductDeviceManager class.
#include "ProductUserInterface.h"      /// This file declares the ProductUserInterface class.
#include "ProductSystemInterface.h"    /// This file declares the ProductSystemInterface class.
#include "ProductController.h"         /// This file declares the ProductController class.

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
static const DPrint s_logger    { "Product Hardware" };
static const char   s_logName[] = "Product Hardware"  ;

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductHardwareManager::GetInstance
///
/// @brief  This static method creates the one and only instance of a ProductHardwareManager object.
///         The C++ Version 11 compiler guarantees that only one instance is created in a thread
///         safe way.
///
/// @param  void This method does not take any arguments.
///
/// @return This method returns a pointer to a ProductHardwareManager object.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
ProductHardwareManager* ProductHardwareManager::GetInstance( )
{
       static ProductHardwareManager* instance = new    ProductHardwareManager( );

       return instance;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductHardwareManager::ProductHardwareManager
///
/// @brief  This method is the ProductHardwareManager constructor, which is declared as being private
///         to ensure that only one instance of this class can be created through the class
///         GetInstance method.
///
/// @param  void This method does not take any arguments.
///
/// @return This method does not return anything.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
ProductHardwareManager::ProductHardwareManager( )
                      : m_ProductDeviceManager  ( ProductDeviceManager  ::GetInstance( ) ),
                        m_ProductUserInterface  ( ProductUserInterface  ::GetInstance( ) ),
                        m_ProductSystemInterface( ProductSystemInterface::GetInstance( ) ),
                        m_ProductController     ( ProductController     ::GetInstance( ) )
{
       return;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductHardwareManager::Run
///
/// @brief  This method sets up the LPM hardware client.
///
/// @param  void This method does not take any arguments.
///
/// @return This method does not return anything.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductHardwareManager::Run( )
{
     m_mainTask = m_ProductController->GetMainTask( );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name  ProductHardwareManager::SendSetVolume
///
/// @brief This method sends a request to the LPM hardware.
///
/// @param volume [input]
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductHardwareManager::SendSetVolume( uint32_t volume )
{
     s_logger.LogInfo( "%s: A volume level of %d is being set.", s_logName, volume );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name  ProductHardwareManager::SendUserMute
///
/// @brief This method sends a request to the LPM hardware.
///
/// @param mute [input]
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductHardwareManager::SendUserMute( bool mute )
{
     s_logger.LogInfo( "%s: A user mute %s is being set.", s_logName, mute ? "on" : "off" );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name  ProductHardwareManager::SendInternalMute
///
/// @brief This method sends a request to the LPM hardware.This method sends a request to the LPM hardware.
///
/// @param mute [input]
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductHardwareManager::SendInternalMute( bool mute )
{
    s_logger.LogInfo( "%s: An internal mute %s is being set.", s_logName, mute ? "on" : "off" );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name  ProductHardwareManager::SendAudioPathPresentationLatency
///
/// @brief This method sends a request to the LPM hardware.
///
/// @param latency [input]
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductHardwareManager::SendAudioPathPresentationLatency( uint32_t latency )
{
     s_logger.LogInfo( "%s: Audio path latency of %d is being set.", s_logName, latency );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name  ProductHardwareManager::SendSetDSPAudioMode
///
/// @brief This method sends a request to the LPM hardware.
///
/// @param audioMode [input]
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductHardwareManager::SendSetDSPAudioMode( IpcAudioMode_t audioMode )
{
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

    s_logger.LogInfo( "%s: Audio mode is to be set to %s.",
                      s_logName,
                      audioModeString.c_str( ) );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name  ProductHardwareManager::SendLipSyncDelay
///
/// @brief This method sends a request to the LPM hardware.
///
/// @param audioDelay [input]
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductHardwareManager::SendLipSyncDelay( uint32_t audioDelay )
{
     s_logger.LogInfo( "%s: Audio lip sync delay is to be set to %d.", s_logName, audioDelay );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name  ProductHardwareManager::SendToneAndLevelControl
///
/// @brief This method sends a request to the LPM hardware.
///
/// @param controls [input]
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductHardwareManager::SendToneAndLevelControl( IpcToneControl_t& controls )
{
     s_logger.LogInfo( "%s: Audio tone and level settings are to be set as follows: ", s_logName );
     s_logger.LogInfo( "%s:                ", s_logName );
     s_logger.LogInfo( "%s: Bass      : %d ", s_logName, controls.bass );
     s_logger.LogInfo( "%s: Treble    : %d ", s_logName, controls.treble );
     s_logger.LogInfo( "%s: Center    : %d ", s_logName, controls.centerSpeaker );
     s_logger.LogInfo( "%s: Surround  : %d ", s_logName, controls.surroundSpeaker );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name  ProductHardwareManager::SendSpeakerList
///
/// @brief This method sends a request to the LPM hardware.
///
/// @param accessoryList [input]
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductHardwareManager::SendSpeakerList( IpcAccessoryList_t& accessoryList )
{
     s_logger.LogInfo( "%s: Speaker activation settings are tManagero be set as follows: ", s_logName );
     s_logger.LogInfo( "%s:                                                       ", s_logName );
     s_logger.LogInfo( "%s: Left  Speaker : %d ", s_logName, ( uint32_t )accessoryList.accessory[ACCESSORY_POSITION_LEFT_REAR].active );
     s_logger.LogInfo( "%s: Right Speaker : %d ", s_logName, ( uint32_t )accessoryList.accessory[ACCESSORY_POSITION_RIGHT_REAR].active );
     s_logger.LogInfo( "%s: Sub   Speaker : %d ", s_logName, ( uint32_t )accessoryList.accessory[ACCESSORY_POSITION_SUB].active );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name  ProductHardwareManager::SendSetSystemTimeoutEnableBits
///
/// @brief This method sends a request to the LPM hardware.
///
/// @param timeoutControl [input]
///
/// @return This method does not return anything..
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductHardwareManager::SendSetSystemTimeoutEnableBits( Ipc_TimeoutControl_t& timeoutControl )
{
     s_logger.LogInfo( "%s: Auto power down will be set to %s.", s_logName, timeoutControl.enable ? "on" : "off" );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name  ProductHardwareManager::RebootRequest
///
/// @brief This method sends a request to the LPM hardware.
///
/// @param  void This method does not take any arguments.
///
/// @return This method does not return anything..
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductHardwareManager::RebootRequest( )
{
     s_logger.LogInfo( "%s: A reboot request is being sent.", s_logName );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name  ProductHardwareManager::HandleLowPowerStandby
///
/// @brief This method sends a request to the LPM hardware.
///
/// @param  void This method does not take any arguments.
///
/// @return This method does not return anything..
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductHardwareManager::HandleLowPowerStandby( )
{
     s_logger.LogInfo( "%s: Low Power standby is being set.", s_logName );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name  ProductHardwareManager::SendBlueToothDeviceData
///
/// @brief This method sends a request to the LPM hardware.
///
/// @param bluetoothDeviceName [input]
///
/// @param bluetoothMacAddress [input]
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductHardwareManager::SendBlueToothDeviceData( const std::string&       bluetoothDeviceName,
                                                      const unsigned long long bluetoothMacAddress )
{
    s_logger.LogInfo( "%s: Bluetooth data is being set to the Device %s with MAC Address 0x%016llX.",
                      s_logName,
                      bluetoothDeviceName.c_str( ),
                      bluetoothMacAddress );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name  ProductHardwareManager::SendSourceSelection
///
/// @brief This method sends a request to the LPM hardware.
///
/// @param bluetoothDeviceName [input]
///
/// @param bluetoothMacAddress [input]
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductHardwareManager::SendSourceSelection( const IPCSource_t& sourceSelect )
{
    s_logger.LogInfo( "%s: The source selection will be set to the value %d with status %d",
                      s_logName,
                      sourceSelect.source,
                      sourceSelect.status );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
