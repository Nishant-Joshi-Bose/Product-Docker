////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file      ProductHardwareInterface.cpp
///
/// @brief     This header file contains declarations for managing the hardware, which interfaces
///            primarily with the Low Power Microprocessor or LPM.
///
/// @author    Stuart J. Lumby
///
/// @date      09/22/2017
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
#include "SystemUtils.h"
#include "DPrint.h"
#include "Services.h"
#include "RebroadcastLatencyMode.pb.h"
#include "ProductHardwareInterface.h"
#include "ProductController.h"
#include "ProductMessage.pb.h"
#include "LpmClientIF.h"
#include "LpmClientFactory.h"
#include "BreakThread.h"
#include "RivieraLPM_IpcProtocol.h"
#include "AutoLpmServiceMessages.pb.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                Start of ProductApp Namespace                                 ///
////////////////////////////////////////////////////////////////////////////////////////////////////
namespace ProductApp
{

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
typedef APClientSocketListenerIF::ListenerPtr   ClientListener;
typedef APClientSocketListenerIF::SocketPtr     ClientSocket;
typedef APServerSocketListenerIF::ListenerPtr   ServerListener;
typedef APServerSocketListenerIF::SocketPtr     ServerSocket;
typedef IPCMessageRouterIF::IPCMessageRouterPtr MessageRouter;

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// The following declares a DPrint class type object for logging information in this source code
/// file.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
static DPrint s_logger { "Product" };

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductHardwareInterface::GetInstance
///
/// @brief  This static method creates the one and only instance of a ProductHardwareInterface
///         object. The C++ Version 11 compiler guarantees that only one instance is created in a
///         thread safe way, whenever a pointer is defined as being static.
///
/// @return This method returns a pointer to a ProductHardwareInterface object.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
ProductHardwareInterface* ProductHardwareInterface::GetInstance( NotifyTargetTaskIF*        task,
        Callback< ProductMessage > ProductNotify )
{
    static ProductHardwareInterface* instance = new ProductHardwareInterface( task,
            ProductNotify );

    BOSE_DEBUG( s_logger, "The instance %8p of the Product Hardware Manager was returned.", instance );

    return instance;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductHardwareInterface::ProductHardwareInterface
///
/// @brief  This method is the ProductHardwareInterface constructor, which is declared as being private
///         to ensure that only one instance of this class can be created through the class
///         GetInstance method.
///
/// @return This method does not return anything.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
ProductHardwareInterface::ProductHardwareInterface( NotifyTargetTaskIF*        task,
        Callback< ProductMessage > ProductNotify )
    : m_mainTask     ( task          ),
      m_ProductNotify( ProductNotify ),
      m_connected    ( false         )
{
    return;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductHardwareInterface::Run
///
/// @brief  This method creates the LPM hardware client, and attempts to connect to the LPM server.
///
/// @param  bool This method returns a true value after connected to the LPM server.
///
/// @return This method does not return anything.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool ProductHardwareInterface::Run( )
{
    BOSE_DEBUG( s_logger, "The hardware connection to the LPM is being established." );

    m_LpmClient = LpmClientFactory::Create( "ProductLpmClient", m_mainTask );

    Callback< bool > ConnectedCallback( std::bind( &ProductHardwareInterface::Connected,
                                        this,
                                        std::placeholders::_1 ) );

    m_LpmClient->Connect( ConnectedCallback );

    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductHardwareInterface::Connected
///
/// @brief  This method sets up the LPM hardware client.
///
/// @param  void This method does not take any arguments.
///
/// @return This method does not return anything.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductHardwareInterface::Connected( bool connected )
{
    if( !connected )
    {
        BOSE_DEBUG( s_logger, "A hardware connection to the LPM could not be established." );
        BOSE_DEBUG( s_logger, "An attempt to reconnect to the LPM will be made." );

        m_connected = false;

        ProductMessage productMessage;
        productMessage.set_id( LPM_HARDWARE_DOWN );

        IL::BreakThread( std::bind( m_ProductNotify, productMessage ),      m_mainTask );
        IL::BreakThread( std::bind( &ProductHardwareInterface::Run, this ), m_mainTask );

        return;
    }
    else
    {
        BOSE_DEBUG( s_logger, "A hardware connection to the LPM has been established." );
        BOSE_DEBUG( s_logger, "An attempt to obtain the LPM status will now be made." );

        m_connected = true;

        ProductMessage productMessage;
        productMessage.set_id( LPM_HARDWARE_UP );

        IL::BreakThread( std::bind( m_ProductNotify, productMessage ), m_mainTask );

        Callback< LpmServiceMessages::IpcLpmHealthStatusPayload_t >
        CallbackForLpmStatus( std::bind( &ProductHardwareInterface::HandleLpmStatus,
                                         this,
                                         std::placeholders::_1 ) );

        RequestLpmStatus( CallbackForLpmStatus );
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief ProductHardwareInterface::RequestLpmStatus
///
/// @return
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool ProductHardwareInterface::RequestLpmStatus( Callback< LpmServiceMessages::IpcLpmHealthStatusPayload_t >
        callback )
{
    if( m_connected == false || m_LpmClient == nullptr )
    {
        BOSE_ERROR( s_logger, "An LPM status request could not be made, as no connection is available." );

        return false;
    }
    else
    {
        BOSE_DEBUG( s_logger, "An LPM status request will be made." );

        DeviceStatusReq_t statusRequestType;

        statusRequestType.set_requesttype( LPM_STATUS );

        m_LpmClient->RequestStatus( statusRequestType, callback );

        return true;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief ProductHardwareInterface::HandleLpmStatus
///
/// @param status
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductHardwareInterface::HandleLpmStatus( LpmServiceMessages::IpcLpmHealthStatusPayload_t status )
{
    BOSE_DEBUG( s_logger, "An LPM status was received with the following values: ");
    BOSE_DEBUG( s_logger, "                      ");
    BOSE_DEBUG( s_logger, "Image             : %s", status.has_image( )                            ?
                std::to_string( status.image( ) ).c_str( )          :
                "Unknown" );
    BOSE_DEBUG( s_logger, "Software Version  : %s", status.has_swversion( )                        ?
                status.swversion( ).c_str( )                        :
                "Unknown" );
    BOSE_DEBUG( s_logger, "Power State       : %s", status.has_power( )                            ?
                std::to_string( status.power( ) ).c_str( )          :
                "Unknown" );
    BOSE_DEBUG( s_logger, "Source            : %s", status.has_source( )                           ?
                std::to_string( status.source( ) ).c_str( )         :
                "Unknown" );
    BOSE_DEBUG( s_logger, "Volume            : %s", status.has_volume( )                           ?
                std::to_string( status.volume( ) ).c_str( )         :
                "Unknown" );
    BOSE_DEBUG( s_logger, "Mute Status       : %s", status.has_mute( )                             ?
                std::to_string( status.mute( ) ).c_str( )           :
                "Unknown" );
    BOSE_DEBUG( s_logger, "Up Time           : %s", status.has_uptime( )                           ?
                std::to_string( status.uptime( ) ).c_str( )         :
                "Unknown" );
    BOSE_DEBUG( s_logger, "Region Code       : %s", status.has_regioncode( )                       ?
                std::to_string( status.regioncode( ) ).c_str( )     :
                "Unknown" );
    BOSE_DEBUG( s_logger, "Country Code      : %s", status.has_countrycode( )                      ?
                std::to_string( status.countrycode( ) ).c_str( )    :
                "Unknown" );
    BOSE_DEBUG( s_logger, "Speaker Package   : %s", status.has_speakerpackage( )                   ?
                std::to_string( status.speakerpackage( ) ).c_str( ) :
                "Unknown" );
    BOSE_DEBUG( s_logger, "Audio Mode        : %s", status.has_audiomode( )                        ?
                std::to_string( status.audiomode( ) ).c_str( )      :
                "Unknown" );
    BOSE_DEBUG( s_logger, "Base              : %s", status.has_bass( )                             ?
                std::to_string( status.bass( ) ).c_str( )           :
                "Unknown" );
    BOSE_DEBUG( s_logger, "Treble            : %s", status.has_treble( )                           ?
                std::to_string( status.treble( ) ).c_str( )         :
                "Unknown" );
    BOSE_DEBUG( s_logger, "Center            : %s", status.has_center( )                           ?
                std::to_string( status.center( ) ).c_str( )         :
                "Unknown" );
    BOSE_DEBUG( s_logger, "Surround          : %s", status.has_surround( )                         ?
                std::to_string( status.surround( ) ).c_str( )       :
                "Unknown" );
    BOSE_DEBUG( s_logger, "Lip Sync Delay    : %s", status.has_lipsyncdelayms( )                   ?
                std::to_string( status.lipsyncdelayms( ) ).c_str( ) :
                "Unknown" );
    BOSE_DEBUG( s_logger, "System Color      : %s", status.has_systemcolor( )                      ?
                std::to_string( status.systemcolor( ) ).c_str( )    :
                "Unknown" );
    BOSE_DEBUG( s_logger, "Language          : %s", status.has_language( )                         ?
                std::to_string( status.language( ) ).c_str( )       :
                "Unknown" );
    BOSE_DEBUG( s_logger, "Serial            : %s", status.has_serial( )                           ?
                status.serial( ).c_str( )                           :
                "Unknown" );
    BOSE_DEBUG( s_logger, "Status            : %s", status.has_status( )                           ?
                std::to_string( status.status( ) ).c_str( )         :
                "Unknown" );
    BOSE_DEBUG( s_logger, "Out of Box Status : %s", status.has_outofboxstatus( )                   ?
                std::to_string( status.outofboxstatus( ) ).c_str( ) :
                "Unknown" );
    BOSE_DEBUG( s_logger, "Network Status    : %s", status.has_networkstatus( )                    ?
                std::to_string( status.networkstatus( ) ).c_str( )  :
                "Unknown" );
    BOSE_DEBUG( s_logger, "Total Latency     : %s", status.has_totallatencyms( )                   ?
                std::to_string( status.totallatencyms( ) ).c_str( ) :
                "Unknown" );
    BOSE_DEBUG( s_logger, "Minimum Latency   : %s", status.has_minimumoutputlatencyms( )                   ?
                std::to_string( status.minimumoutputlatencyms( ) ).c_str( ) :
                "Unknown" );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductHardwareInterface::RegisterForKeyEvents
///
/// @brief  This method is used to register for key events through the LPM hardware client.
///
/// @param  CallbackForKeyEvents [ inputs ] This arguments specifies the callback method or function
///                                         to which key enents are to be sent by the LPM.
///
/// @return This method does not return anything.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool ProductHardwareInterface::RegisterForKeyEvents( Callback< LpmServiceMessages::IpcKeyInformation_t >
        callback )
{
    if( m_connected == false || m_LpmClient == nullptr )
    {
        BOSE_ERROR( s_logger, "An LPM request for key events could not be made, as no connection is available." );

        return false;
    }
    else
    {
        BOSE_DEBUG( s_logger, "An LPM request for key events will be made." );

        m_LpmClient->RegisterEvent( IPC_KEY, callback );

        return true;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name  ProductHardwareInterface::SendSetVolume
///
/// @brief This method sends a request to the LPM hardware.
///
/// @param volume [input]
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool ProductHardwareInterface::SendSetVolume( uint32_t volume )
{
    BOSE_DEBUG( s_logger, "A volume level of %d is being set.", volume );

    if( m_connected == false || m_LpmClient == nullptr )
    {
        BOSE_ERROR( s_logger, "An LPM set volume request could not be made, as no connection is available." );

        return false;
    }
    else
    {
        BOSE_DEBUG( s_logger, "An LPM set volume request will be made." );

        IpcAudioSetVolume_t volumeSetting;

        volumeSetting.set_volume( volume );

        m_LpmClient->SetVolume( volumeSetting );

        return true;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name  ProductHardwareInterface::SendUserMute
///
/// @brief This method sends a request to the LPM hardware.
///
/// @param mute [input]
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool ProductHardwareInterface::SendUserMute( bool mute )
{
    BOSE_DEBUG( s_logger, "A user mute %s is being set.", mute ? "on" : "off" );

    if( m_connected == false || m_LpmClient == nullptr )
    {
        BOSE_ERROR( s_logger, "An LPM user mute request could not be made, as no connection is available." );

        return false;
    }
    else
    {
        BOSE_DEBUG( s_logger, "An LPM user mute request will be made." );

        IpcAudioMute_t muteSetting;

        if( mute )
        {
            muteSetting.set_internalmute( 1 );
            muteSetting.set_unifymute   ( 1 );
        }
        else
        {
            muteSetting.set_internalmute( 0 );
            muteSetting.set_unifymute   ( 1 );
        }

        m_LpmClient->SetMute( muteSetting );

        return true;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name  ProductHardwareInterface::SendInternalMute
///
/// @brief This method sends a request to the LPM hardware.This method sends a request to the LPM hardware.
///
/// @param mute [input]
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool ProductHardwareInterface::SendInternalMute( bool mute )
{
    BOSE_DEBUG( s_logger, "An internal mute %s is being set.", mute ? "on" : "off" );

    if( m_connected == false || m_LpmClient == nullptr )
    {
        BOSE_ERROR( s_logger, "An LPM internal mute request could not be made, as no connection is available." );

        return false;
    }
    else
    {
        BOSE_DEBUG( s_logger, "An LPM internal mute request will be made." );

        IpcAudioMute_t muteSetting;

        if( mute )
        {
            muteSetting.set_internalmute( 1 );
            muteSetting.set_unifymute   ( 0 );
        }
        else
        {
            muteSetting.set_internalmute( 0 );
            muteSetting.set_unifymute   ( 0 );
        }

        m_LpmClient->SetMute( muteSetting );

        return true;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name  ProductHardwareInterface::SendAudioPathPresentationLatency
///
/// @brief This method sends a request to the LPM hardware.
///
/// @param latency [input]
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool ProductHardwareInterface::SendAudioPathPresentationLatency( uint32_t latency )
{
    BOSE_DEBUG( s_logger, "Audio path latency of %d is being set.", latency );

    if( m_connected == false || m_LpmClient == nullptr )
    {
        BOSE_ERROR( s_logger, "An LPM set latency request could not be made, as no connection is available." );

        return false;
    }
    else
    {
        BOSE_DEBUG( s_logger, "An LPM set latency request is currently not supported." );

        return true;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name  ProductHardwareInterface::SendSetDSPAudioMode
///
/// @brief This method sends a request to the LPM hardware.
///
/// @param audioMode [input]
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool ProductHardwareInterface::SendSetDSPAudioMode( IpcAudioMode_t audioMode )
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

    BOSE_DEBUG( s_logger, "Audio mode is to be set to %s.", audioModeString.c_str( ) );

    if( m_connected == false || m_LpmClient == nullptr )
    {
        BOSE_ERROR( s_logger, "An LPM set latency request could not be made, as no connection is available." );

        return false;
    }
    else
    {
        BOSE_DEBUG( s_logger, "An LPM set latency request is currently not supported." );

        return true;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name  ProductHardwareInterface::SendLipSyncDelay
///
/// @brief This method sends a request to the LPM hardware.
///
/// @param audioDelay [input]
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool ProductHardwareInterface::SendLipSyncDelay( uint32_t audioDelay )
{
    BOSE_DEBUG( s_logger, "Audio lip sync delay is to be set to %d.", audioDelay );

    if( m_connected == false || m_LpmClient == nullptr )
    {
        BOSE_ERROR( s_logger, "An LPM set latency request could not be made, as no connection is available." );

        return false;
    }
    else
    {
        BOSE_DEBUG( s_logger, "An LPM set latency request is currently not supported." );

        return true;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name  ProductHardwareInterface::SendToneAndLevelControl
///
/// @brief This method sends a request to the LPM hardware.
///
/// @param controls [input]
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool ProductHardwareInterface::SendToneAndLevelControl( IpcToneControl_t& controls )
{
    BOSE_DEBUG( s_logger, "Audio tone and level settings are to be set as follows: " );
    BOSE_DEBUG( s_logger, "               " );
    BOSE_DEBUG( s_logger, "Bass      : %d ", controls.bass( ) );
    BOSE_DEBUG( s_logger, "Treble    : %d ", controls.treble( ) );
    BOSE_DEBUG( s_logger, "Center    : %d ", controls.centerspeaker( ) );
    BOSE_DEBUG( s_logger, "Surround  : %d ", controls.surroundspeaker( ) );
    BOSE_DEBUG( s_logger, "               " );

    if( m_connected == false || m_LpmClient == nullptr )
    {
        BOSE_ERROR( s_logger, "An LPM audio and tone level request could not be made, as no connection is available." );

        return false;
    }
    else
    {
        BOSE_DEBUG( s_logger, "An LPM audio and tone level request is currently not supported." );

        return true;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name  ProductHardwareInterface::SendSpeakerList
///
/// @brief This method sends a request to the LPM hardware.
///
/// @param accessoryList [input]
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool ProductHardwareInterface::SendSpeakerList( IpcAccessoryList_t& accessoryList )
{
    BOSE_DEBUG( s_logger, "Speaker activation settings are to be set as follows: " );

    if( m_connected == false || m_LpmClient == nullptr )
    {
        BOSE_ERROR( s_logger, "An LPM speaker activation settings request could not be made, as no connection is available." );

        return false;
    }
    else
    {
        BOSE_DEBUG( s_logger, "An LPM activation settings request is currently not supported." );

        return true;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name  ProductHardwareInterface::SendSetSystemTimeoutEnableBits
///
/// @brief This method sends a request to the LPM hardware.
///
/// @param timeoutControl [input]
///
/// @return This method does not return anything..
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool ProductHardwareInterface::SendSetSystemTimeoutEnableBits( Ipc_TimeoutControl_t& timeoutControl )
{
    BOSE_DEBUG( s_logger, "Auto power down will be set to %s.", timeoutControl.enable( ) ? "on" : "off" );

    if( m_connected == false || m_LpmClient == nullptr )
    {
        BOSE_ERROR( s_logger, "An LPM speaker activation settings request could not be made, as no connection is available." );

        return false;
    }
    else
    {
        BOSE_DEBUG( s_logger, "An LPM activation settings request is currently not supported." );

        return true;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name  ProductHardwareInterface::RebootRequest
///
/// @brief This method sends a request to the LPM hardware.
///
/// @param  void This method does not take any arguments.
///
/// @return This method does not return anything..
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool ProductHardwareInterface::RebootRequest( )
{
    BOSE_DEBUG( s_logger, "A reboot request is being sent." );

    if( m_connected == false || m_LpmClient == nullptr )
    {
        BOSE_ERROR( s_logger, "An LPM reboot request could not be made, as no connection is available." );

        return false;
    }
    else
    {
        BOSE_DEBUG( s_logger, "An LPM reboot request is being sent." );
    }

    m_LpmClient->Reboot( );

    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name  ProductHardwareInterface::HandleLowPowerStandby
///
/// @brief This method sends a request to the LPM hardware.
///
/// @param  void This method does not take any arguments.
///
/// @return This method does not return anything..
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool ProductHardwareInterface::HandleLowPowerStandby( )
{
    BOSE_DEBUG( s_logger, "Low Power standby is being set." );

    if( m_connected == false || m_LpmClient == nullptr )
    {
        BOSE_ERROR( s_logger, "An LPM low power standby request could not be made, as no connection is available." );

        return false;
    }
    else
    {
        BOSE_DEBUG( s_logger, "An LPM low power standby request is currently not supported." );

        return true;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductHardwareInterface::SetBlueToothMacAddress
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
void ProductHardwareInterface::SetBlueToothMacAddress( const std::string& bluetoothMacAddress )
{
    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// The Bluetooth MAC Address needs to be reformatted to remove and colon characters. For
    /// example, a Bluetooth MAC Address 88:4A:EA:5A:37:AD of would be reformatted to 884AEA5A37AD.
    /// ///////////////////////////////////////////////////////////////////////////////////////////////
    char blueToothReformattedMacAddress[( 2 * BLUETOOTH_MAC_LENGTH ) + 1 ];

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
/// @name   ProductHardwareInterface::SetBlueToothDeviceName
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
void ProductHardwareInterface::SetBlueToothDeviceName( const std::string& bluetoothDeviceName )
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
/// @name   ProductHardwareInterface::SendBlueToothDeviceData
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
bool ProductHardwareInterface::SendBlueToothDeviceData( const std::string&       bluetoothDeviceName,
        const unsigned long long bluetoothMacAddress )
{
    BOSE_DEBUG( s_logger, "Bluetooth data is being set to the Device %s with MAC Address 0x%016llX.",
                bluetoothDeviceName.c_str( ),
                bluetoothMacAddress );

    if( m_connected == false || m_LpmClient == nullptr )
    {
        BOSE_ERROR( s_logger, "An LPM bluetooth data request could not be made, as no connection is available." );

        return false;
    }
    else
    {
        BOSE_DEBUG( s_logger, "An LPM bluetooth data request is currently not supported." );

        return true;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name  ProductHardwareInterface::SendSourceSelection
///
/// @brief This method sends a request to the LPM hardware.
///
/// @param bluetoothDeviceName [input]
///
/// @param bluetoothMacAddress [input]
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool ProductHardwareInterface::SendSourceSelection( const IPCSource_t& sourceSelect )
{
    BOSE_DEBUG( s_logger, "The source selection will be set to the value %d with status %d",
                sourceSelect.source( ),
                sourceSelect.status( ) );

    if( m_connected == false || m_LpmClient == nullptr )
    {
        BOSE_ERROR( s_logger, "An LPM source selection request could not be made, as no connection is available." );

        return false;
    }
    else
    {
        BOSE_DEBUG( s_logger, "An LPM source selection request is currently not supported." );

        return true;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                               End of ProductApp Namespace                                    ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
