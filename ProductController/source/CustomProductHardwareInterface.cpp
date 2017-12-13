////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file      CustomProductHardwareInterface.cpp
///
/// @brief     This source code file contains custom Professor functionality for managing the
///            hardware, which interfaces with the Low Power Microprocessor or LPM on Riviera
///            APQ boards.
///
/// @note      This custom class inherits a ProductHardwareInterface class found in a common
///            code repository. This base inherited class starts and runs an LPM client connection,
///            as well as provides several common hardware based methods.
///
/// @author    Stuart J. Lumby
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
#include "Utilities.h"
#include "LpmClientFactory.h"
#include "AutoLpmServiceMessages.pb.h"
#include "ProfessorProductController.h"
#include "CustomProductHardwareInterface.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
///                          Start of the Product Application Namespace                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
namespace ProductApp
{

////////////////////////////////////////////////////////////////////////////////////////////////////
///
///            Constant Definitions
///
////////////////////////////////////////////////////////////////////////////////////////////////////
constexpr uint32_t BLUETOOTH_MAC_LENGTH = 6;

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   CustomProductHardwareInterface::CustomProductHardwareInterface
///
/// @brief  ProfessorProductController& ProductController
///
////////////////////////////////////////////////////////////////////////////////////////////////////
CustomProductHardwareInterface::CustomProductHardwareInterface( ProfessorProductController&
                                                                ProductController )

    : ProductHardwareInterface( ProductController.GetTask( ),
                                ProductController.GetMessageHandler( ) )
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name  CustomProductHardwareInterface::SendAccessoryPairing
///
/// @brief This method sends a request to start or stop pairing
///
/// @param bool enabled This argument determines whether to start or stop pairing.
///
/// @param Callback< IpcSpeakerPairingMode_t > pairingCallback This is a callback to return pairing mode.
///
/// @return bool The method returns true when the pairing enabled command was successfully sent.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductHardwareInterface::SendAccessoryPairing( bool enabled,
                                                           const Callback< IpcSpeakerPairingMode_t >& pairingCallback )
{
    if( isConnected( ) == false || GetLpmClient( ) == nullptr )
    {
        BOSE_ERROR( s_logger, "An LPM accessory pairing request could not be made, as no connection is available." );

        return false;
    }

    BOSE_DEBUG( s_logger, "Accessory pairing %s", ( enabled ? "enabled" : "disabled" ) );

    IpcSpeakerPairingMode_t pairing;

    pairing.set_pairingenabled( enabled );

    GetLpmClient( )->OpenSpeakerPairing( pairing, pairingCallback );

    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   CustomProductHardwareInterface::SendAccessoryActive
///
/// @brief  This method sends a request to set the rear and sub speakers active or inactive.
///
/// @param  bool rears This argument determines whether to enable rear speakers.
///
/// @param  bool subs  This argument determines whether to enable sub speakers.
///
/// @param  Callback<IpcSpeakersActive_t> callback
///
/// @return The method will return a Boolean whether the accessory active command was sent.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductHardwareInterface::SendAccessoryActive( bool rears,
                                                          bool subs,
                                                          const Callback<IpcSpeakersActive_t>& callback )
{
    if( isConnected( ) == false || GetLpmClient( ) == nullptr )
    {
        BOSE_ERROR( s_logger, "An LPM set volume request could not be made, as no connection is available." );

        return false;
    }

    BOSE_DEBUG( s_logger, "Accessory active sent rears: %d, subs: %d", rears, subs );

    IpcSpeakersActive_t speakerActive;

    speakerActive.set_rearsenabled( rears );
    speakerActive.set_subsenabled( subs );

    GetLpmClient( )->SetSpeakersActive( speakerActive, callback );

    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name  CustomProductHardwareInterface::SendAccessoryDisband
///
/// @brief This method sends a request to disband all accessories.
///
/// @param none
///
/// @return none
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductHardwareInterface::SendAccessoryDisband( )
{
    if( isConnected( ) == false || GetLpmClient( ) == nullptr )
    {
        BOSE_ERROR( s_logger, "An LPM accessory disband request could not be made, as no connection is available." );

        return false;
    }

    BOSE_DEBUG( s_logger, "An LPM accessory disband request will be made." );

    // TODO
    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name  CustomProductHardwareInterface::SendAudioPathPresentationLatency
///
/// @brief This method sends a request to the LPM hardware.
///
/// @param uint32_t latency
///
/// @return This method returns a false Boolean value if the LPM is not connected. Otherwise, it
///         attempts the request and returns true.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductHardwareInterface::SendAudioPathPresentationLatency( uint32_t latency )
{
    BOSE_DEBUG( s_logger, "Audio path latency of %d is being set.", latency );

    if( isConnected( ) == false || GetLpmClient( ) == nullptr )
    {
        BOSE_ERROR( s_logger, "An LPM set latency request could not be made, as no connection is available." );

        return false;
    }

    BOSE_DEBUG( s_logger, "An LPM set latency request is currently not supported." );

    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   CustomProductHardwareInterface::SendLipSyncDelay
///
/// @brief  This method sends a request to the LPM hardware.
///
/// @param  uint32_t audioDelay
///
/// @return This method returns a false Boolean value if the LPM is not connected. Otherwise, it
///         attempts the request and returns true.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductHardwareInterface::SendLipSyncDelay( uint32_t audioDelay )
{
    BOSE_DEBUG( s_logger, "Audio lip sync delay is to be set to %d.", audioDelay );

    if( isConnected( ) == false || GetLpmClient( ) == nullptr )
    {
        BOSE_ERROR( s_logger, "An LPM lip sync delay request could not be made, as no connection is available." );

        return false;
    }

    BOSE_DEBUG( s_logger, "An LPM lip sync delay request is currently not supported." );

    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   CustomProductHardwareInterface::SendToneAndLevelControl
///
/// @brief  This method sends a request to the LPM hardware.
///
/// @param  IpcToneControl_t& controls
///
/// @return This method returns a false Boolean value if the LPM is not connected. Otherwise, it
///         attempts the request and returns true.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductHardwareInterface::SendToneAndLevelControl( IpcToneControl_t& controls )
{
    BOSE_DEBUG( s_logger, "Audio tone and level settings are to be set as follows: " );
    BOSE_DEBUG( s_logger, "               " );
    BOSE_DEBUG( s_logger, "Bass      : %d ", controls.bass( ) );
    BOSE_DEBUG( s_logger, "Treble    : %d ", controls.treble( ) );
    BOSE_DEBUG( s_logger, "Center    : %d ", controls.centerspeaker( ) );
    BOSE_DEBUG( s_logger, "Surround  : %d ", controls.surroundspeaker( ) );
    BOSE_DEBUG( s_logger, "               " );

    if( isConnected( ) == false || GetLpmClient( ) == nullptr )
    {
        BOSE_ERROR( s_logger, "An LPM audio and tone level request could not be made, as no connection is available." );

        return false;
    }

    BOSE_DEBUG( s_logger, "An LPM audio and tone level request is currently not supported." );

    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   CustomProductHardwareInterface::SendSpeakerList
///
/// @brief  This method sends speaker list information to the LPM hardware.
///
/// @param  IpcAccessoryList_t accessoryList
///
/// @return This method returns a false Boolean value if the LPM is not connected. Otherwise, it
///         attempts the request and returns true.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductHardwareInterface::SendSpeakerList( IpcAccessoryList_t& accessoryList )
{
    BOSE_DEBUG( s_logger, "Speaker activation settings are to be set as follows: " );

    if( isConnected( ) == false || GetLpmClient( ) == nullptr )
    {
        BOSE_ERROR( s_logger, "An LPM speaker activation settings request could not be made, as no connection is available." );

        return false;
    }

    BOSE_DEBUG( s_logger, "An LPM speaker activation settings request is currently not supported." );

    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   CustomProductHardwareInterface::SendSetSystemTimeoutEnableBits
///
/// @brief  This method sends a request to the LPM hardware.
///
/// @param  Ipc_TimeoutControl_t& timeoutControl
///
/// @return This method returns a false Boolean value if the LPM is not connected. Otherwise, it
///         attempts the request and returns true.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductHardwareInterface::SendSetSystemTimeoutEnableBits( Ipc_TimeoutControl_t& timeoutControl )
{
    BOSE_DEBUG( s_logger, "Auto power down will be set to %s.", timeoutControl.enable( ) ? "on" : "off" );

    if( isConnected( ) == false || GetLpmClient( ) == nullptr )
    {
        BOSE_ERROR( s_logger, "No LPM connection is currently available." );

        return false;
    }

    BOSE_DEBUG( s_logger, "An LPM activation settings request is currently not supported." );

    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name  CustomProductHardwareInterface::SendWiFiRadioStatus
///
/// @brief This method sends the wireless radio frequency to the LPM hardware.
///
/// @param uint32_t frequency
///
/// @return This method returns a false Boolean value if the LPM is not connected. Otherwise, it
///         attempts the request and returns true.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductHardwareInterface::SendWiFiRadioStatus( uint32_t frequencyInKhz )
{
    BOSE_DEBUG( s_logger, "An attempt to send the wireless frequency %d KHz to the LPM is being made.",
                frequencyInKhz );

    if( isConnected( ) == false || GetLpmClient( ) == nullptr )
    {
        BOSE_ERROR( s_logger, "No LPM connection is currently available." );

        return false;
    }

    BOSE_DEBUG( s_logger, "Sending the wireless radio frequency to the LPM is currently not supported." );

    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   CustomProductHardwareInterface::SetBlueToothMacAddress
///
/// @brief  This method is used to set the Bluetooth MAC Address and send it to the LPM hardware,
///         as long as the associated Bluetooth device name has been previously obtained.
///
/// @param  std::string bluetoothMacAddress This argument is a standard string representing the
///                                         Bluetooth MAC Address.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductHardwareInterface::SetBlueToothMacAddress( const std::string& bluetoothMacAddress )
{
    ////////////////////////////////////////////////////////////////////////////////////////////////
    /// The Bluetooth MAC Address needs to be reformatted to remove any colon characters. For
    /// example, a Bluetooth MAC Address 88:4A:EA:5A:37:AD of would be reformatted to 884AEA5A37AD.
    /// ////////////////////////////////////////////////////////////////////////////////////////////
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

    ///
    /// The member m_gettingBlueToothData will be set to false once the device name and MAC address
    /// have both been obtained and sent to the LPM interface. It is true when it is missing either
    /// of these values.
    ///
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
/// @name   CustomProductHardwareInterface::SetBlueToothDeviceName
///
/// @brief  This method is used to set the Bluetooth Device Name and send it to the LPM hardware,
///         as long as the associated Bluetooth MAC Address has been previously obtained.
///
/// @param  std::string bluetoothDeviceName This argument is a standard string representing the
///                                         Bluetooth device name.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductHardwareInterface::SetBlueToothDeviceName( const std::string& bluetoothDeviceName )
{
    m_blueToothDeviceName.assign( bluetoothDeviceName );

    ///
    /// The member m_gettingBlueToothData will be set to false once the device name and MAC address
    /// have both been obtained and sent to the LPM interface. It is true when it is missing either
    /// of these values.
    ///
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
/// @name   CustomProductHardwareInterface::SendBlueToothDeviceData
///
/// @brief  This method is used to send the Bluetooth device and MAC Address data to the LPM hardware.
///
/// @param  std::string bluetoothDeviceName This argument is a standard string representing the
///                                         Bluetooth device name.
///
/// @param  unsigned long long bluetoothMacAddress This argument is a standard string representing
///                                                the Bluetooth MAC Address.
///
/// @return This method returns a false Boolean value if the LPM is not connected. Otherwise, it
///         attempts the request and returns true.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductHardwareInterface::SendBlueToothDeviceData( const std::string&       bluetoothDeviceName,
                                                              const unsigned long long bluetoothMacAddress )
{
    BOSE_DEBUG( s_logger, "Bluetooth data is being set to the Device %s with MAC Address 0x%016llX.",
                bluetoothDeviceName.c_str( ),
                bluetoothMacAddress );

    if( isConnected( ) == false || GetLpmClient( ) == nullptr )
    {
        BOSE_ERROR( s_logger, "An LPM bluetooth data request could not be made, as no connection is available." );

        return false;
    }

    BOSE_DEBUG( s_logger, "An LPM bluetooth data request is currently not supported." );

    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name  CustomProductHardwareInterface::SendSourceSelection
///
/// @brief This method sends a request to the LPM hardware.
///
/// @param IPCSource_t& sourceSelect
///
/// @return This method returns a false Boolean value if the LPM is not connected. Otherwise, it
///         attempts the request and returns true.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductHardwareInterface::SendSourceSelection( const LPM_IPC_SOURCE_ID sourceSelect )
{
    IPCSource_t source;

    source.set_source( sourceSelect );
    source.set_open_field( 0 );
    source.set_status( 0 );

    if( isConnected( ) == false || GetLpmClient( ) == nullptr )
    {
        BOSE_ERROR( s_logger, "An LPM source selection request could not be made, as no connection is available." );

        return false;
    }

    BOSE_DEBUG( s_logger, "An LPM source selection request is currently not supported." );

    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name  CustomProductHardwareInterface::SetCecPhysicalAddress
///
/// @brief This method sends a request to the LPM hardware.
///
/// @param None
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductHardwareInterface::SetCecPhysicalAddress( const uint32_t cecPhysicalAddress )
{
    BOSE_DEBUG( s_logger, "CEC Physical Address will be sent to  LPM" );

    if( isConnected( ) == false || GetLpmClient( ) == nullptr )
    {
        BOSE_ERROR( s_logger, "An LPM send CEC Physical Address request could not be made, as no connection is available." );

        return false;
    }

    IpcCecPhyscialAddress_t cecAddrSetting;
    cecAddrSetting.set_cecphyaddr( cecPhysicalAddress );
    GetLpmClient( )->SendCecPhysicalAddress( cecAddrSetting );

    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                           End of the Product Application Namespace                           ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
