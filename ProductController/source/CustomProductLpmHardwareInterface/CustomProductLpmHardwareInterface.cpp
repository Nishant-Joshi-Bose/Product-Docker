////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file      CustomProductLpmHardwareInterface.cpp
///
/// @brief     This source code file contains custom Professor functionality for managing the
///            hardware, which interfaces with the Low Power Microprocessor or LPM on Riviera
///            APQ boards.
///
/// @note      This custom class inherits a ProductLpmHardwareInterface class found in a common
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
#include "CustomProductLpmHardwareInterface.h"

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
/// @name   CustomProductLpmHardwareInterface::CustomProductLpmHardwareInterface
///
/// @brief  ProfessorProductController& ProductController
///
////////////////////////////////////////////////////////////////////////////////////////////////////
CustomProductLpmHardwareInterface::CustomProductLpmHardwareInterface( ProfessorProductController&
                                                                      ProductController )

    : ProductLpmHardwareInterface( ProductController.GetTask( ),
                                   ProductController.GetMessageHandler( ) )
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   CustomProductLpmHardwareInterface::NotifyVolumeLevel
///
/// @brief  This method send a notification of the volume level through the LPM hardware to other
///         interested processes.
///
/// @param  uint32_t volume
///
/// @return This method returns a false Boolean value if the LPM is not connected. Otherwise, it
///         attempts the request and returns true.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductLpmHardwareInterface::NotifyVolumeLevel( uint32_t volume )
{
    BOSE_DEBUG( s_logger, "A volume level of %d is being sent as a notifiation.", volume );

    if( isConnected( ) == false || GetLpmClient( ) == nullptr )
    {
        BOSE_ERROR( s_logger, "A notification of the volume level failed. There is no LPM connection." );

        return false;
    }

    BOSE_DEBUG( s_logger, "An LPM volume level notification will be made." );

    IpcAudioSetVolume_t volumeSetting;

    volumeSetting.set_volume( volume );

    GetLpmClient( )->SetVolume( volumeSetting );

    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   CustomProductLpmHardwareInterface::NotifyMuteState
///
/// @brief  This method send a notification of the mute state through the LPM hardware to other
///         interested processes.
///
/// @param  bool mute
///
/// @return This method returns a false Boolean value if the LPM is not connected. Otherwise, it
///         attempts the request and returns true.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductLpmHardwareInterface::NotifyMuteState( bool mute )
{
    BOSE_DEBUG( s_logger, "A mute %s is being sent as a notification.", mute ? "on" : "off" );

    if( isConnected( ) == false || GetLpmClient( ) == nullptr )
    {
        BOSE_ERROR( s_logger, "A notification of the mute state failed. There is no LPM connection." );

        return false;
    }

    BOSE_DEBUG( s_logger, "An LPM mute state notification will be made." );

    ///
    /// @todo The LPM mute setting will be changed to contain only one field to set the mute state.
    ///       The two fields used here are from legacy LPM code. Note that the JIRA Story PGC-551
    ///       has been created to track this future change.
    ///
    IpcAudioMute_t muteSetting;

    if( mute )
    {
        muteSetting.set_internalmute( 1 );
        muteSetting.set_unifymute( 1 );
    }
    else
    {
        muteSetting.set_internalmute( 0 );
        muteSetting.set_unifymute( 0 );
    }

    GetLpmClient( )->SetMute( muteSetting );

    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name  CustomProductLpmHardwareInterface::SendAccessoryPairing
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
bool CustomProductLpmHardwareInterface::SendAccessoryPairing( bool enabled,
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
/// @name   CustomProductLpmHardwareInterface::SendAccessoryActive
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
bool CustomProductLpmHardwareInterface::SendAccessoryActive( bool rears,
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
/// @name  CustomProductLpmHardwareInterface::SendAccessoryDisband
///
/// @brief This method sends a request to disband all accessories.
///
/// @param none
///
/// @return none
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductLpmHardwareInterface::SendAccessoryDisband( const Callback<IpcAccessoryDisbandCommand_t> &cb )
{
    if( isConnected( ) == false || GetLpmClient( ) == nullptr )
    {
        BOSE_ERROR( s_logger, "An LPM accessory disband request could not be made, as no connection is available." );

        return false;
    }

    BOSE_DEBUG( s_logger, "An LPM accessory disband request will be made." );

    IpcAccessoryDisbandCommand_t command;
    command.set_disband( true );

    GetLpmClient()->DisbandAccessorySpeakers( command, cb );

    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   CustomProductLpmHardwareInterface::SendSpeakerList
///
/// @brief  This method sends speaker list information to the LPM hardware.
///
/// @param  IpcAccessoryList_t accessoryList
///
/// @return This method returns a false Boolean value if the LPM is not connected. Otherwise, it
///         attempts the request and returns true.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductLpmHardwareInterface::SendSpeakerList( IpcAccessoryList_t& accessoryList )
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
/// @name  CustomProductLpmHardwareInterface::SendWiFiRadioStatus
///
/// @brief This method sends the wireless radio frequency to the LPM hardware.
///
/// @param uint32_t frequency
///
/// @return This method returns a false Boolean value if the LPM is not connected. Otherwise, it
///         attempts the request and returns true.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductLpmHardwareInterface::SendWiFiRadioStatus( uint32_t frequencyInKhz )
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
/// @name   CustomProductLpmHardwareInterface::SetBlueToothMacAddress
///
/// @brief  This method is used to set the Bluetooth MAC Address and send it to the LPM hardware,
///         as long as the associated Bluetooth device name has been previously obtained.
///
/// @param  std::string bluetoothMacAddress This argument is a standard string representing the
///                                         Bluetooth MAC Address.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductLpmHardwareInterface::SetBlueToothMacAddress( const std::string& bluetoothMacAddress )
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
/// @name   CustomProductLpmHardwareInterface::SetBlueToothDeviceName
///
/// @brief  This method is used to set the Bluetooth Device Name and send it to the LPM hardware,
///         as long as the associated Bluetooth MAC Address has been previously obtained.
///
/// @param  std::string bluetoothDeviceName This argument is a standard string representing the
///                                         Bluetooth device name.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductLpmHardwareInterface::SetBlueToothDeviceName( const std::string& bluetoothDeviceName )
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
/// @name   CustomProductLpmHardwareInterface::SendBlueToothDeviceData
///
/// @brief  This method is used to send the Bluetooth device and MAC Address data to the LPM hardware.
///
/// @param  std::string bluetoothDeviceName This argument is a standard string representing the
///                                         Bluetooth device name.
///
/// @param  unsigned long long bluetoothMacAddress This argument is a standard string representing
///                                                the Bluetooth MAC Addresmount -o remount,rw /opt/Bose
///
/// @return This method returns a false Boolean value if the LPM is not connected. Otherwise, it
///         attempts the request and returns true.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductLpmHardwareInterface::SendBlueToothDeviceData( const std::string&       bluetoothDeviceName,
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
/// @name  CustomProductLpmHardwareInterface::SendSourceSelection
///
/// @brief This method sends a request to the LPM hardware.
///
/// @param IPCSource_t& sourceSelect
///
/// @return This method returns a false Boolean value if the LPM is not connected. Otherwise, it
///         attempts the request and returns true.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductLpmHardwareInterface::SendSourceSelection( const LPM_IPC_SOURCE_ID sourceSelect )
{
    IPCSource_t source;

    source.set_source( sourceSelect );
// not sure what these two do, but they're commented out in the latest proto
// file
//    source.set_open_field( 0 );
//    source.set_status( 0 );

    if( isConnected( ) == false || GetLpmClient( ) == nullptr )
    {
        BOSE_ERROR( s_logger, "An LPM source selection request could not be made, as no connection is available." );

        return false;
    }

    GetLpmClient( )->SendCecCurrentSource( source );
    BOSE_DEBUG( s_logger, "An LPM source selection sent to LPM. %d", source.source() );

    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name  CustomProductLpmHardwareInterface::SetCecPhysicalAddress
///
/// @brief This method sends a request to the LPM hardware.
///
/// @param None
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductLpmHardwareInterface::SetCecPhysicalAddress( const uint32_t cecPhysicalAddress )
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
///
/// @name  CustomProductHardwareLpmInterface::SendAdaptIQControl
///
/// @brief This method sends an AdaptIQ control request to the DSP
///
/// @param action
///
/// @return bool The method returns true when the control request was successfully sent.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductLpmHardwareInterface::SendAdaptIQControl( ProductAdaptIQControl::AdaptIQAction action )
{
    if( isConnected( ) == false || GetLpmClient( ) == nullptr )
    {
        BOSE_ERROR( s_logger, "%s failed, as no connection is available.", __func__ );

        return false;
    }

    BOSE_DEBUG( s_logger, "%s : send action %s", __func__, ProductAdaptIQControl::AdaptIQAction_Name( action ).c_str() );

    IpcAiqControlPayload_t msg;
    switch( action )
    {
    case ProductAdaptIQControl::Start:
        /* Start has no equivalent in IPC; instead it triggers the AIQ SM to boot the AIQ image */
        break;
    case ProductAdaptIQControl::Cancel:
        msg.set_control( IpcAiqControl_t::AIQ_CONTROL_STOP );
        break;
    case ProductAdaptIQControl::Advance:
        msg.set_control( IpcAiqControl_t::AIQ_CONTROL_ADVANCE );
        break;
    case ProductAdaptIQControl::Previous:
        msg.set_control( IpcAiqControl_t::AIQ_CONTROL_PREVIOUS );
        break;
    default:
        break;
    }
    // Note this message gets routed through the LPM directly to the DSP
    GetLpmClient( )->SendAdaptIQControl( msg, Ipc_Device_t::IPC_DEVICE_DSP );

    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name  CustomProductHardwareLpmInterface::SetStreamConfig
///
/// @brief This method send setStreamConfig request to DSP,
///         which includes mainStreamAudioSettings, inputRoute, and streamMix parameters
///
/// @param string streamConfig
/// Callback<IpcDspStreamConfigRespPayload_t> cb
///
/// @return bool The method returns true when the setStreamConfig request was successfully sent.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductLpmHardwareInterface::SetStreamConfig( std::string& serializedAudioSettings, std::string& serializedInputRoute, const Callback<bool>& cb )
{
    if( isConnected( ) == false || GetLpmClient( ) == nullptr )
    {
        BOSE_ERROR( s_logger, "%s failed, as no connection is available.", __func__ );

        return false;
    }

    auto respCb = [cb]( IpcDspStreamConfigRespPayload_t resp )
    {
        cb.Send( ( resp.success() > 0 ) ? true : false );
    };
    // TODO: PGC-218: Enable routing of setStreamConfig() message to DSP
    // Convert serialized json string from APProduct into IpcDspStreamConfigReqPayload_t type
    IpcDspStreamConfigReqPayload_t msg;
    msg.set_inputroute( 1 );
    GetLpmClient( )->SetStreamConfigRequest( msg, respCb, Ipc_Device_t::IPC_DEVICE_DSP );
    return true;
}

/// @name  CustomProductHardwareLpmInterface::BootDSPImage
///
/// @brief This method loads the specified DSP image
///
/// @param action
///
/// @return bool The method returns true when the control request was successfully sent.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductLpmHardwareInterface::BootDSPImage( LpmServiceMessages::IpcImage_t image )
{
    LpmServiceMessages::IpcDeviceBoot_t p;

    p.set_image( image );
    GetLpmClient( )->DspRebootToImage( p );

    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                           End of the Product Application Namespace                           ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
