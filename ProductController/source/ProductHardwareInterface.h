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
#include "ThreadMutex.h"
#include "NotifyTargetTaskIF.h"
#include "APTask.h"
#include "APClientSocketListenerIF.h"
#include "APServerSocketListenerIF.h"
#include "IPCMessageRouterIF.h"
#include "APProductIF.h"
#include "ProductMessage.pb.h"
#include "LpmClientIF.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
///                          Start of the Product Application Namespace                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
namespace ProductApp
{

////////////////////////////////////////////////////////////////////////////////////////////////////
///
///            Included Subclasses
///
////////////////////////////////////////////////////////////////////////////////////////////////////
class ProductCommandLine;
class ProductController;

class ProductHardwareInterface
{
public:

    //////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief The following aliases refer to the Bose Sound Touch SDK utilities for inter-process
    ///        and inter-thread communications.
    ///
    //////////////////////////////////////////////////////////////////////////////////////////////
    typedef APClientSocketListenerIF::ListenerPtr   ClientListener;
    typedef APClientSocketListenerIF::SocketPtr     ClientSocket;
    typedef APServerSocketListenerIF::ListenerPtr   ServerListener;
    typedef APServerSocketListenerIF::SocketPtr     ServerSocket;
    typedef IPCMessageRouterIF::IPCMessageRouterPtr MessageRouter;

    //////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @name   ProductHardwareInterface::GetInstance
    ///set
    /// @brief  This static method creates the one and only instance of a ProductHardwareInterface
    ///         object. That only one instance is created in a thread safe way is guaranteed by
    ///         the C++ Version 11 compiler.
    ///
    /// @param  NotifyTargetTaskIF* task This argument specifies the task in which to run the
    ///                                  hardware interface.
    ///
    /// @param  Callback< ProductMessage > ProductNotify This argument specifies a callback to
    ///                                                  send messages back to the product
    ///                                                  controller.
    ///
    /// @return This method returns a pointer to a ProductHardwareInterface object.
    ///
    //////////////////////////////////////////////////////////////////////////////////////////////
    static ProductHardwareInterface* GetInstance( NotifyTargetTaskIF*        ProductTask,
                                                  Callback< ProductMessage > ProductNotify );

    //////////////////////////////////////////////////////////////////////////////////////////////
    /// This declaration is used to start and run the hardware manager.
    //////////////////////////////////////////////////////////////////////////////////////////////
    bool Run( );
    void Stop( );

    //////////////////////////////////////////////////////////////////////////////////////////////
    /// These methods handle LPM status requests.
    //////////////////////////////////////////////////////////////////////////////////////////////
    bool RequestLpmStatus( Callback< LpmServiceMessages::IpcLpmHealthStatusPayload_t > callback );
    void HandleLpmStatus( LpmServiceMessages::IpcLpmHealthStatusPayload_t              status );

    //////////////////////////////////////////////////////////////////////////////////////////////
    /// This method is used to set a callback to receive key events from the LPM hardware.
    //////////////////////////////////////////////////////////////////////////////////////////////
    bool RegisterForKeyEvents( Callback< LpmServiceMessages::IpcKeyInformation_t > callback );

    //////////////////////////////////////////////////////////////////////////////////////////////
    /// These declarations are calls used to set the power state of the hardware.
    //////////////////////////////////////////////////////////////////////////////////////////////
    bool RequestNormalOperations( );
    void RequestNormalOperationsFailed( uint32_t operationCode );
    void RequestNormalOperationsPassed( const LpmServiceMessages::IpcLpmStateResponse_t stateResponse );
    bool RequestPowerStateOff( );
    void RequestPowerStateOffFailed( uint32_t operationCode );
    void RequestPowerStateOffPassed( const LpmServiceMessages::IpcLpmStateResponse_t stateResponse );
    bool RequestPowerStateStandby( );
    void RequestPowerStateStandbyFailed( uint32_t operationCode );
    void RequestPowerStateStandbyPassed( const LpmServiceMessages::IpcLpmStateResponse_t stateResponse );
    bool RequestPowerStateAutowake( );
    void RequestPowerStateAutowakeFailed( uint32_t operationCode );
    void RequestPowerStateAutowakePassed( const LpmServiceMessages::IpcLpmStateResponse_t stateResponse );
    bool RequestPowerStateFull( );
    void RequestPowerStateFullFailed( uint32_t operationCode );
    void RequestPowerStateFullPassed( const LpmServiceMessages::IpcLpmStateResponse_t stateResponse );

    //////////////////////////////////////////////////////////////////////////////////////////////
    /// These declarations are utility calls, which send messages to the LPM hardware manager
    /// process as a client.
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
    bool CECMsgHandler( const LpmServiceMessages::IpcCecMessage_t cecMsg );

    //////////////////////////////////////////////////////////////////////////////////////////////
    /// These are messages pertaining to the accessory speakers
    /// Wish these could live in the ProductSpeakerManager but that doesn't have direct
    /// LpmClient access
    //////////////////////////////////////////////////////////////////////////////////////////////
    bool SendAccessoryPairing( bool enabled, const Callback<LpmServiceMessages::IpcSpeakerPairingMode_t>& cb );
    bool SendAccessoryActive( bool rears, bool subs,  const Callback<IpcSpeakersActive_t> &cb );
    bool SendAccessoryDisband( );

    //////////////////////////////////////////////////////////////////////////////////////////////
    /// These allow other modules to have a notification of when lpm is connected or not
    //////////////////////////////////////////////////////////////////////////////////////////////
    void RegisterForLpmClientConnectEvent( Callback<bool> );

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @name   ProductHardwareInterface::RegisterForLpmEvents
    ///
    /// @brief  This method is a templated function to be used to register lpm any event through the
    ///         LPM hardware client that will be recieved from LpmClient.
    ///
    /// @param  IpcOpcodes_t [ opcode ] The event you care about
    ///
    /// @param  CallbackForKeyEvents [ inputs ] This arguments specifies the callback method or function
    ///                                         to which key enents are to be sent by the LPM.
    ///
    /// @return bool - This method returns whether the register was successful
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

    ////////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief  ProductHardwareInterface
    ///
    /// @brief  The constructor for this class is set to be private. This definition prevents this
    ///         class from being instantiated directly, so that only the static method GetInstance
    ///         to this class can be used to get the one sole instance of it.
    ///
    /// @param  NotifyTargetTaskIF* ProductTask
    ///
    /// @param Callback< ProductMessage > ProductNotify
    ///
    ////////////////////////////////////////////////////////////////////////////////////////////////
    ProductHardwareInterface( NotifyTargetTaskIF*        ProductTask,
                              Callback< ProductMessage > ProductNotify );

    //////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief The following copy constructor and equality operator for this class are private
    ///        and are set to be undefined through the delete keyword. This prevents this class
    ///        from being copied directly, so that only the static method GetInstance to this
    ///        class can be used to get the one sole instance of it.
    ///
    //////////////////////////////////////////////////////////////////////////////////////////////
    ProductHardwareInterface( ProductHardwareInterface const& ) = delete;
    ProductHardwareInterface operator = ( ProductHardwareInterface const& ) = delete;

    //////////////////////////////////////////////////////////////////////////////////////////////
    /// These declarations store the main task for processing LPM hardware events and requests. It
    /// is passed by the ProductController instance.
    //////////////////////////////////////////////////////////////////////////////////////////////
    NotifyTargetTaskIF*        m_ProductTask   = nullptr;
    Callback< ProductMessage > m_ProductNotify = nullptr;
    LpmClientIF::LpmClientPtr  m_LpmClient     = nullptr;

    //////////////////////////////////////////////////////////////////////////////////////////////
    /// This member determines whether a connections to the LPM server connection is established.
    //////////////////////////////////////////////////////////////////////////////////////////////
    bool m_connected = false;

    //////////////////////////////////////////////////////////////////////////////////////////////
    /// These declarations are used to handle Bluetooth functionality.
    //////////////////////////////////////////////////////////////////////////////////////////////
    bool               m_gettingBlueToothData = false;
    unsigned long long m_blueToothMacAddress  = 0ULL;
    std::string        m_blueToothDeviceName  { "" };

    //////////////////////////////////////////////////////////////////////////////////////////////
    /// This method is called when an LPM server connection is established.
    //////////////////////////////////////////////////////////////////////////////////////////////
    void Connected( bool  connected );

    //////////////////////////////////////////////////////////////////////////////////////////////
    /// This is used to store callbacks to submodules that care about lpm connection
    /// Userful for registering your call backs with a call to RegisterForLpmEvents on boot
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
