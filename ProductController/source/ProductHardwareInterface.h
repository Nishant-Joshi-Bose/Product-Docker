////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file      ProductHardwareInterface.h
///
/// @brief     This header file contains declarations for managing the hardware, which interfaces
///            with the Low Power Microprocessor or LPM.
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
#include "Utilities.h"
#include "LpmClientIF.h"
#include "ProductMessage.pb.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
///                          Start of the Product Application Namespace                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
namespace ProductApp
{

////////////////////////////////////////////////////////////////////////////////////////////////////
///
///            Forward Class Declarations
///
////////////////////////////////////////////////////////////////////////////////////////////////////
class ProfessorProductController;

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @class ProductCommandLine
///
/// @brief This class is used to managing the hardware, which interfaces with the Low Power
///        Microprocessor or LPM.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
class ProductHardwareInterface
{
public:

    ////////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief  ProductHardwareInterface Constructor
    ///
    /// @param  ProfessorProductController& ProductController
    ///
    ////////////////////////////////////////////////////////////////////////////////////////////////
    ProductHardwareInterface( ProfessorProductController& ProductController );

    //////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief  The following public methods are used to run and stop instances of the
    ///         ProductHardwareInterface class, respectively.
    ///
    //////////////////////////////////////////////////////////////////////////////////////////////
    bool Run( );
    void Stop( );

    //////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief These methods handle LPM status requests.
    ///
    //////////////////////////////////////////////////////////////////////////////////////////////
    bool RequestLpmStatus( Callback< LpmServiceMessages::IpcLpmHealthStatusPayload_t > callback );
    void HandleLpmStatus( LpmServiceMessages::IpcLpmHealthStatusPayload_t              status );

    //////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief This method is used to set a callback to receive key events from the LPM hardware.
    ///
    //////////////////////////////////////////////////////////////////////////////////////////////
    bool RegisterForKeyEvents( Callback< LpmServiceMessages::IpcKeyInformation_t > callback );

    //////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief These declarations are calls used to set the power state of the hardware.
    ///
    //////////////////////////////////////////////////////////////////////////////////////////////
    bool RequestLpmSystemState( const LpmServiceMessages::IpcLpmSystemState_t state );
    void RequestLpmSystemStateFailed( const LpmServiceMessages::IpcLpmSystemState_t state, uint32_t operationCode );
    void RequestLpmSystemStatePassed( const LpmServiceMessages::IpcLpmSystemState_t state, const LpmServiceMessages::IpcLpmStateResponse_t  stateResponse );

    //////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief These declarations are utility calls, which send messages to the LPM hardware manager
    ///         process as a client.
    ///
    //////////////////////////////////////////////////////////////////////////////////////////////
    bool SendSetVolume( uint32_t volume );
    bool SendUserMute( bool      mute );
    bool SendInternalMute( bool  mute );
    bool SendAudioPathPresentationLatency( uint32_t  latency );
    bool SendLipSyncDelay( uint32_t                  audioDelay );
    bool SendToneAndLevelControl( IpcToneControl_t&  controls );
    bool SendSpeakerList( IpcAccessoryList_t&        accessoryList );
    bool SendSetSystemTimeoutEnableBits( Ipc_TimeoutControl_t& timeoutControl );
    bool SendWiFiRadioStatus( uint32_t frequencyInKhz );
    bool SendRebootRequest( );
    void SetBlueToothMacAddress( const std::string&       bluetoothMacAddress );
    void SetBlueToothDeviceName( const std::string&       bluetoothDeviceName );
    bool SendBlueToothDeviceData( const std::string&       bluetoothDeviceName,
                                  const unsigned long long bluetoothMacAddress );
    bool SendSourceSelection( const LPM_IPC_SOURCE_ID      sourceSelect );
    bool CECSetPhysicalAddress( const uint32_t cecPhyAddr );

    //////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief These methods pertain to the accessory speakers and pairing.
    ///
    //////////////////////////////////////////////////////////////////////////////////////////////
    bool SendAccessoryPairing( bool enabled, const Callback<LpmServiceMessages::IpcSpeakerPairingMode_t>& cb );
    bool SendAccessoryActive( bool rears, bool subs,  const Callback<IpcSpeakersActive_t> &cb );
    bool SendAccessoryDisband( );

    //////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief These allow other modules to have a notification of when lpm is connected or not.
    ///
    //////////////////////////////////////////////////////////////////////////////////////////////
    void RegisterForLpmClientConnectEvent( Callback<bool> );

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @name   ProductHardwareInterface::RegisterForLpmEvents
    ///
    /// @brief  This method is a templated function to be used to register lpm any event through the
    ///         LPM hardware client that will be recieved from LpmClient.
    ///
    /// @param  IpcOpcodes_t         opcode This argument specifies the hardware event.
    ///
    /// @param  CallbackForKeyEvents inputs These arguments specifies the callback method or function
    ///                                     to which events are to be sent by the LPM.
    ///
    /// @return bool This method returns whether the registration was successful.
    ///
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    template< typename T >
    bool RegisterForLpmEvents( IpcOpcodes_t opcode, const Callback<T> &callback )
    {
        if( m_connected == false || m_LpmClient == nullptr )
        {
            return false;
        }
        else
        {
            m_LpmClient->RegisterEvent( opcode, callback );
            return true;
        }
    };

private:

    //////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief These declarations store the main task for processing LPM hardware events and
    ///        requests. It is passed by the ProductController instance.
    ///
    //////////////////////////////////////////////////////////////////////////////////////////////
    NotifyTargetTaskIF*        m_ProductTask   = nullptr;
    Callback< ProductMessage > m_ProductNotify = nullptr;
    LpmClientIF::LpmClientPtr  m_LpmClient     = nullptr;

    //////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief This member determines whether a connections to the LPM server connection is
    ///        established.
    ///
    //////////////////////////////////////////////////////////////////////////////////////////////
    bool m_connected = false;

    //////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief These declarations are used to handle Bluetooth functionality.
    ///
    //////////////////////////////////////////////////////////////////////////////////////////////
    bool               m_gettingBlueToothData = false;
    unsigned long long m_blueToothMacAddress  = 0ULL;
    std::string        m_blueToothDeviceName  { "" };

    //////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief This method is called when an LPM server connection is established.
    ///
    //////////////////////////////////////////////////////////////////////////////////////////////
    void Connected( bool  connected );

    //////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief This is used to store callbacks to submodules that care about the LPM connection.
    ///        They are userful for registering your call backs with a call to RegisterForLpmEvents
    ///        on boot up.
    ///
    //////////////////////////////////////////////////////////////////////////////////////////////
    std::vector < Callback<bool> > m_lpmConnectionNotifies;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
///                           End of the Product Application Namespace                           ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
