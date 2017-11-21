////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file      ProductHardwareInterface.cpp
///
/// @brief     This header file contains declarations for managing the hardware, which interfaces
///            primarily with the Low Power Microprocessor or LPM.
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
#include "Services.h"
#include "ProductHardwareInterface.h"
#include "ProductController.h"
#include "ProductMessage.pb.h"
#include "LpmClientIF.h"
#include "LpmClientFactory.h"
#include "BreakThread.h"
#include "RivieraLPM_IpcProtocol.h"
#include "AutoLpmServiceMessages.pb.h"

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
constexpr uint32_t MILLISECOND_TIMEOUT_START = 30 * 1000;

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
/// @name   ProductHardwareInterface::GetInstance
///
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
////////////////////////////////////////////////////////////////////////////////////////////////////
ProductHardwareInterface* ProductHardwareInterface::GetInstance( NotifyTargetTaskIF*        ProductTask,
                                                                 Callback< ProductMessage > ProductNotify )
{
    static ProductHardwareInterface* instance = new ProductHardwareInterface( ProductTask,
                                                                              ProductNotify );

    BOSE_DEBUG( s_logger, "The instance %8p of the Product Hardware Manager was returned.", instance );

    return instance;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductHardwareInterface::ProductHardwareInterface
///
/// @brief  This method is the ProductHardwareInterface constructor, which is declared as being
///         private to ensure that only one instance of this class can be created through the class
///         GetInstance method.
///
/// @param  NotifyTargetTaskIF* task This argument specifies the task in which to run the
///                                  hardware interface.
///
/// @param  Callback< ProductMessage > ProductNotify This argument specifies a callback to
///                                                  send messages back to the product
///                                                  controller.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
ProductHardwareInterface::ProductHardwareInterface( NotifyTargetTaskIF*        ProductTask,
                                                    Callback< ProductMessage > ProductNotify )
    : m_ProductTask( ProductTask ),
      m_ProductNotify( ProductNotify ),
      m_connected( false )
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductHardwareInterface::Run
///
/// @brief  This method creates the LPM hardware client, and attempts to connect to the LPM server.
///
/// @return This method returns a true value after attempting to connect to the LPM server.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool ProductHardwareInterface::Run( )
{
    BOSE_DEBUG( s_logger, "The hardware connection to the LPM is being established." );

    m_LpmClient = LpmClientFactory::Create( "ProductLpmClient", m_ProductTask );

    Callback< bool > ConnectedCallback( std::bind( &ProductHardwareInterface::Connected,
                                                   this,
                                                   std::placeholders::_1 ) );

    m_LpmClient->Connect( ConnectedCallback );

    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductHardwareInterface::RegisterForLpmClientConnectEvent
///
/// @brief  Callback< bool > callback Callback for connection and disconnection events.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductHardwareInterface::RegisterForLpmClientConnectEvent( Callback<bool> callback )
{
    m_lpmConnectionNotifies.push_back( callback );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductHardwareInterface::Connected
///
/// @brief  This method sets up the LPM hardware client.
///
/// @param  bool connected
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductHardwareInterface::Connected( bool connected )
{
    m_connected = connected;

    ///
    /// If the LPM client is connected, request normal operations and request the LPM status;
    /// otherwise, re-attempt to connect to the LPM through its Run method. The product
    /// controller can not do any useful operations without an LPM connection.
    ///
    if( connected )
    {
        BOSE_DEBUG( s_logger, "A hardware connection to the LPM has been established." );
        BOSE_DEBUG( s_logger, "An attempt to request the LPM status will be made." );
        BOSE_DEBUG( s_logger, "An attempt to request normal operations will be made." );

        Callback< LpmServiceMessages::IpcLpmHealthStatusPayload_t >
        CallbackForLpmStatus( std::bind( &ProductHardwareInterface::HandleLpmStatus,
                                         this,
                                         std::placeholders::_1 ) );

        RequestSystemStateStandby( );
        RequestLpmStatus( CallbackForLpmStatus );
        auto func = std::bind(
                        &ProductHardwareInterface::CECMsgHandler,
                        this,
                        std::placeholders::_1 );

        AsyncCallback<LpmServiceMessages::IpcCecMessage_t> CallbackForCecMsgs( func, m_ProductTask );
        m_LpmClient->RegisterEvent<LpmServiceMessages::IpcCecMessage_t>( IPC_CEC_MSG, CallbackForCecMsgs );
    }
    else
    {
        BOSE_DEBUG( s_logger, "A hardware connection to the LPM could not be established." );
        BOSE_DEBUG( s_logger, "An attempt to reconnect to the LPM will be made." );

        IL::BreakThread( std::bind( &ProductHardwareInterface::Run, this ), m_ProductTask );
    }

    ///
    /// Send the LPM connected event to all modules that have registered for it via the
    /// RegisterForLpmClientConnectEvent method. Note that the module must have registered
    /// for the connection event before the ProductHardware is ran through its Run method;
    /// otherwise, the module may not get the connected event.
    ///
    for( auto &connectedCallback : m_lpmConnectionNotifies )
    {
        connectedCallback( m_connected );
    }

    ///
    /// Send the LPM connected status to the product controller.
    ///
    ProductMessage productMessage;
    productMessage.mutable_lpmstatus( )->set_connected( connected );

    IL::BreakThread( std::bind( m_ProductNotify, productMessage ), m_ProductTask );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductHardwareInterface::RequestLpmStatus
///
/// @param  Callback< LpmServiceMessages::IpcLpmHealthStatusPayload_t > callback
///
/// @return This method returns a false Boolean value if the LPM is not connected. Otherwise, it
///         attempts the request and returns true.
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
/// @name  ProductHardwareInterface::HandleLpmStatus
///
/// @param LpmServiceMessages::IpcLpmHealthStatusPayload_t status
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductHardwareInterface::HandleLpmStatus( LpmServiceMessages::IpcLpmHealthStatusPayload_t status )
{
    BOSE_DEBUG( s_logger, "An LPM status was received with the following values: " );

    BOSE_DEBUG( s_logger, "                      " );

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
    BOSE_DEBUG( s_logger, "Minimum Latency   : %s", status.has_minimumoutputlatencyms( )           ?
                std::to_string( status.minimumoutputlatencyms( ) ).c_str( ) :
                "Unknown" );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductHardwareInterface::RequestSystemStateLowPower
///
/// @return This method returns a false Boolean value if the LPM is not connected. Otherwise, it
///         attempts the request and returns true.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool ProductHardwareInterface::RequestSystemStateLowPower( )
{
    if( m_connected == false || m_LpmClient == nullptr )
    {
        BOSE_ERROR( s_logger, "An LPM request for low power could not be made." );
        BOSE_ERROR( s_logger, "No LPM connection is currently available." );

        return false;
    }
    else
    {
        BOSE_DEBUG( s_logger, "An LPM request for low power will be made." );

        IpcLpmSystemStateSet_t systemStateRequest;

        systemStateRequest.set_state( SYSTEM_STATE_LOW_POWER );

        Callback< uint32_t >
        FailedCallback( std::bind( &ProductHardwareInterface::RequestSystemStateLowPowerFailed,
                                   this,
                                   std::placeholders::_1 ) );

        Callback< IpcLpmStateResponse_t >
        PassedCallback( std::bind( &ProductHardwareInterface::RequestSystemStateLowPowerPassed,
                                   this,
                                   std::placeholders::_1 ) );

        m_LpmClient->SetSystemStateTimeout( systemStateRequest,
                                            PassedCallback,
                                            FailedCallback,
                                            MILLISECOND_TIMEOUT_START );
        return true;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name    ProductHardwareInterface::RequestSystemStateLowPowerFailed
///
/// @param   uint32_t operationCode
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductHardwareInterface::RequestSystemStateLowPowerFailed( uint32_t operationCode )
{
    BOSE_ERROR( s_logger, "An LPM request for a low power code %u failed.", operationCode );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name    ProductHardwareInterface::RequestSystemStateLowPowerPassed
///
/// @param   IpcLpmStateResponse_t stateResponse
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductHardwareInterface::RequestSystemStateLowPowerPassed( const IpcLpmStateResponse_t stateResponse )
{
    if( stateResponse.has_sysstate( ) )
    {
        if( stateResponse.sysstate( ) == SYSTEM_STATE_LOW_POWER )
        {
            BOSE_DEBUG( s_logger, "The system state is now set to a low powered state." );
        }
        else
        {
            BOSE_ERROR( s_logger, "The system state could not be set to a low powered state." );
            BOSE_ERROR( s_logger, "The system state is now set to the %s state.",
                        IpcLpmSystemState_t_Name( stateResponse.sysstate( ) ).c_str( ) );
        }
    }
    else
    {
        BOSE_ERROR( s_logger, "The system state could not be set to a low powered state." );
        BOSE_ERROR( s_logger, "The system state is now set to an unknown state." );
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductHardwareInterface::RequestSystemStateStandby
///
/// @return This method returns a false Boolean value if the LPM is not connected. Otherwise, it
///         attempts the request and returns true.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool ProductHardwareInterface::RequestSystemStateStandby( )
{
    if( m_connected == false || m_LpmClient == nullptr )
    {
        BOSE_ERROR( s_logger, "An LPM request for a standby power state could not be made." );
        BOSE_ERROR( s_logger, "No LPM connection is currently available." );

        return false;
    }
    else
    {
        BOSE_DEBUG( s_logger, "An LPM request for a standby power state will be made." );

        IpcLpmSystemStateSet_t systemStateRequest;

        systemStateRequest.set_state( SYSTEM_STATE_STANDBY );

        Callback< uint32_t >
        FailedCallback( std::bind( &ProductHardwareInterface::RequestSystemStateStandbyFailed,
                                   this,
                                   std::placeholders::_1 ) );

        Callback< IpcLpmStateResponse_t >
        PassedCallback( std::bind( &ProductHardwareInterface::RequestSystemStateStandbyPassed,
                                   this,
                                   std::placeholders::_1 ) );

        m_LpmClient->SetSystemStateTimeout( systemStateRequest,
                                            PassedCallback,
                                            FailedCallback,
                                            MILLISECOND_TIMEOUT_START );
        return true;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name  ProductHardwareInterface::RequestSystemStateStandbyFailed
///
/// @param uint32_t operationCode
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductHardwareInterface::RequestSystemStateStandbyFailed( uint32_t operationCode )
{
    BOSE_ERROR( s_logger, "An LPM request for a standby power state code %u failed.", operationCode );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name  ProductHardwareInterface::RequestSystemStateStandbyPassed
///
/// @param IpcLpmStateResponse_t stateResponse
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductHardwareInterface::RequestSystemStateStandbyPassed( const IpcLpmStateResponse_t stateResponse )
{
    if( stateResponse.has_pwrstate( ) && stateResponse.has_sysstate( ) )
    {
        if( stateResponse.pwrstate( ) == POWER_STATE_NETWORK_STANDBY &&
            stateResponse.sysstate( ) == SYSTEM_STATE_STANDBY )
        {
            BOSE_DEBUG( s_logger, "The power state is now set to a standby powered state." );
        }
        else
        {
            BOSE_ERROR( s_logger, "The power state was not set to a standby powered state." );
            BOSE_ERROR( s_logger, "The power state is now set to a %s state.",
                        IpcLPMPowerState_t_Name( stateResponse.pwrstate( ) ).c_str( ) );
        }
    }
    else
    {
        BOSE_ERROR( s_logger, "The power state could not be set to a standby powered state." );
        BOSE_ERROR( s_logger, "The power state is now set to an unknown state." );
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductHardwareInterface::RequestSystemStateIdle
///
/// @return This method returns a false Boolean value if the LPM is not connected. Otherwise, it
///         attempts the request and returns true.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool ProductHardwareInterface::RequestSystemStateIdle( )
{
    if( m_connected == false || m_LpmClient == nullptr )
    {
        BOSE_ERROR( s_logger, "An LPM request for an autowake power state could not be made." );
        BOSE_ERROR( s_logger, "No LPM connection is currently available." );

        return false;
    }
    else
    {
        BOSE_DEBUG( s_logger, "An LPM request for an autowake power state will be made." );

        IpcLpmSystemStateSet_t systemStateRequest;

        systemStateRequest.set_state( SYSTEM_STATE_IDLE );

        Callback< uint32_t >
        FailedCallback( std::bind( &ProductHardwareInterface::RequestSystemStateIdleFailed,
                                   this,
                                   std::placeholders::_1 ) );

        Callback< IpcLpmStateResponse_t >
        PassedCallback( std::bind( &ProductHardwareInterface::RequestSystemStateIdlePassed,
                                   this,
                                   std::placeholders::_1 ) );

        m_LpmClient->SetSystemStateTimeout( systemStateRequest,
                                            PassedCallback,
                                            FailedCallback,
                                            MILLISECOND_TIMEOUT_START );
        return true;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name  ProductHardwareInterface::RequestSystemStateIdleFailed
///
/// @param uint32_t operationCode
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductHardwareInterface::RequestSystemStateIdleFailed( uint32_t operationCode )
{
    BOSE_ERROR( s_logger, "An LPM request for an autowake power state code %u failed.", operationCode );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name  ProductHardwareInterface::RequestSystemStateIdlePassed
///
/// @param IpcLpmStateResponse_t stateResponse
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductHardwareInterface::RequestSystemStateIdlePassed( const IpcLpmStateResponse_t stateResponse )
{
    if( stateResponse.has_pwrstate( ) && stateResponse.has_sysstate( ) )
    {
        if( stateResponse.pwrstate( ) == POWER_STATE_AUTO_WAKE_STANDBY &&
            stateResponse.sysstate( ) == SYSTEM_STATE_IDLE )
        {
            BOSE_DEBUG( s_logger, "The power state is now set to an autowake powered state." );
        }
        else
        {
            BOSE_ERROR( s_logger, "The power state could not be set to an autowake powered state." );
            BOSE_ERROR( s_logger, "The power state is now set to a %s state.",
                        IpcLPMPowerState_t_Name( stateResponse.pwrstate( ) ).c_str( ) );
        }
    }
    else
    {
        BOSE_ERROR( s_logger, "The power state could not be set to an autowake powered state." );
        BOSE_ERROR( s_logger, "The power state is now set to an unknown state." );
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductHardwareInterface::RequestSystemStateOn
///
/// @return This method returns a false Boolean value if the LPM is not connected. Otherwise, it
///         attempts the request and returns true.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool ProductHardwareInterface::RequestSystemStateOn( )
{
    if( m_connected == false || m_LpmClient == nullptr )
    {
        BOSE_ERROR( s_logger, "An LPM request for a full power state could not be made." );
        BOSE_ERROR( s_logger, "No LPM connection is currently available." );

        return false;
    }
    else
    {
        BOSE_DEBUG( s_logger, "An LPM request for a full power state will be made." );

        IpcLpmSystemStateSet_t systemStateRequest;

        systemStateRequest.set_state( SYSTEM_STATE_ON );

        Callback< uint32_t >
        FailedCallback( std::bind( &ProductHardwareInterface::RequestSystemStateOnFailed,
                                   this,
                                   std::placeholders::_1 ) );

        Callback< IpcLpmStateResponse_t >
        PassedCallback( std::bind( &ProductHardwareInterface::RequestSystemStateOnPassed,
                                   this,
                                   std::placeholders::_1 ) );

        m_LpmClient->SetSystemStateTimeout( systemStateRequest,
                                            PassedCallback,
                                            FailedCallback,
                                            MILLISECOND_TIMEOUT_START );
        return true;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name  RequestSystemStateOnFailed
///
/// @param uint32_t operationCode
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductHardwareInterface::RequestSystemStateOnFailed( uint32_t operationCode )
{
    BOSE_ERROR( s_logger, "An LPM request for a full power state code %u failed.", operationCode );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name  RequestSystemStateOnPassed
///
/// @param IpcLpmStateResponse_t stateResponse
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductHardwareInterface::RequestSystemStateOnPassed( const IpcLpmStateResponse_t stateResponse )
{
    if( stateResponse.has_pwrstate( ) && stateResponse.has_sysstate( ) )
    {
        if( stateResponse.pwrstate( ) == POWER_STATE_FULL_POWER &&
            stateResponse.sysstate( ) == SYSTEM_STATE_ON )
        {
            BOSE_DEBUG( s_logger, "The power state is now set to a full powered state." );
        }
        else
        {
            BOSE_ERROR( s_logger, "The power state could not be set to a full powered state." );
            BOSE_ERROR( s_logger, "The power state is now set to a %s state.",
                        IpcLPMPowerState_t_Name( stateResponse.pwrstate( ) ).c_str( ) );
        }
    }
    else
    {
        BOSE_ERROR( s_logger, "The power state could not be set to a full powered state." );
        BOSE_ERROR( s_logger, "The power state is now set to an unknown state." );
    }
}
////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name  ProductHardwareInterface::SendAccessoryPairing
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
bool ProductHardwareInterface::SendAccessoryPairing( bool enabled, const Callback< IpcSpeakerPairingMode_t >&
                                                     pairingCallback )
{
    if( m_connected == false || m_LpmClient == nullptr )
    {
        BOSE_ERROR( s_logger, "An LPM accessory pairing request could not be made, as no connection is available." );

        return false;
    }

    BOSE_DEBUG( s_logger, "Accessory pairing %s", ( enabled ? "enabled" : "disabled" ) );

    IpcSpeakerPairingMode_t pairing;

    pairing.set_pairingenabled( enabled );

    m_LpmClient->OpenSpeakerPairing( pairing, pairingCallback );

    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductHardwareInterface::SendAccessoryActive
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
bool ProductHardwareInterface::SendAccessoryActive( bool rears, bool subs, const Callback<IpcSpeakersActive_t>& callback )
{
    if( m_connected == false || m_LpmClient == nullptr )
    {
        BOSE_ERROR( s_logger, "An LPM set volume request could not be made, as no connection is available." );

        return false;
    }

    BOSE_DEBUG( s_logger, "Accessory active sent rears: %d, subs: %d", rears, subs );

    IpcSpeakersActive_t speakerActive;

    speakerActive.set_rearsenabled( rears );
    speakerActive.set_subsenabled( subs );

    m_LpmClient->SetSpeakersActive( speakerActive, callback );

    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name  ProductHardwareInterface::SendAccessoryDisband
///
/// @brief This method sends a request to disband all accessories.
///
/// @param none
///
/// @return none
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool ProductHardwareInterface::SendAccessoryDisband( )
{
    if( m_connected == false || m_LpmClient == nullptr )
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
/// @name   ProductHardwareInterface::SendSetVolume
///
/// @brief  This method sends a volume request to the LPM hardware.
///
/// @param  uint32_t volume
///
/// @return This method returns a false Boolean value if the LPM is not connected. Otherwise, it
///         attempts the request and returns true.
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
/// @name   ProductHardwareInterface::SendUserMute
///
/// @brief  This method sends a mute request to the LPM hardware.
///
/// @param  bool mute
///
/// @return This method returns a false Boolean value if the LPM is not connected. Otherwise, it
///         attempts the request and returns true.
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
            muteSetting.set_unifymute( 1 );
        }
        else
        {
            muteSetting.set_internalmute( 0 );
            muteSetting.set_unifymute( 1 );
        }

        m_LpmClient->SetMute( muteSetting );

        return true;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductHardwareInterface::SendInternalMute
///
/// @brief  This method sends an internal mute request to the LPM hardware.
///
/// @param  bool mute
///
/// @return This method returns a false Boolean value if the LPM is not connected. Otherwise, it
///         attempts the request and returns true.
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
            muteSetting.set_unifymute( 0 );
        }
        else
        {
            muteSetting.set_internalmute( 0 );
            muteSetting.set_unifymute( 0 );
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
/// @param uint32_t latency
///
/// @return This method returns a false Boolean value if the LPM is not connected. Otherwise, it
///         attempts the request and returns true.
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
/// @name   ProductHardwareInterface::SendLipSyncDelay
///
/// @brief  This method sends a request to the LPM hardware.
///
/// @param  uint32_t audioDelay
///
/// @return This method returns a false Boolean value if the LPM is not connected. Otherwise, it
///         attempts the request and returns true.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool ProductHardwareInterface::SendLipSyncDelay( uint32_t audioDelay )
{
    BOSE_DEBUG( s_logger, "Audio lip sync delay is to be set to %d.", audioDelay );

    if( m_connected == false || m_LpmClient == nullptr )
    {
        BOSE_ERROR( s_logger, "An LPM lip sync delay request could not be made, as no connection is available." );

        return false;
    }
    else
    {
        BOSE_DEBUG( s_logger, "An LPM lip sync delay request is currently not supported." );

        return true;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductHardwareInterface::SendToneAndLevelControl
///
/// @brief  This method sends a request to the LPM hardware.
///
/// @param  IpcToneControl_t& controls
///
/// @return This method returns a false Boolean value if the LPM is not connected. Otherwise, it
///         attempts the request and returns true.
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
/// @name   ProductHardwareInterface::SendSpeakerList
///
/// @brief  This method sends speaker list information to the LPM hardware.
///
/// @param  IpcAccessoryList_t accessoryList
///
/// @return This method returns a false Boolean value if the LPM is not connected. Otherwise, it
///         attempts the request and returns true.
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
        BOSE_DEBUG( s_logger, "An LPM speaker activation settings request is currently not supported." );

        return true;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductHardwareInterface::SendSetSystemTimeoutEnableBits
///
/// @brief  This method sends a request to the LPM hardware.
///
/// @param  Ipc_TimeoutControl_t& timeoutControl
///
/// @return This method returns a false Boolean value if the LPM is not connected. Otherwise, it
///         attempts the request and returns true.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool ProductHardwareInterface::SendSetSystemTimeoutEnableBits( Ipc_TimeoutControl_t& timeoutControl )
{
    BOSE_DEBUG( s_logger, "Auto power down will be set to %s.", timeoutControl.enable( ) ? "on" : "off" );

    if( m_connected == false || m_LpmClient == nullptr )
    {
        BOSE_ERROR( s_logger, "No LPM connection is currently available." );

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
/// @name  ProductHardwareInterface::SendWiFiRadioStatus
///
/// @brief This method sends the wireless radio frequency to the LPM hardware.
///
/// @param uint32_t frequency
///
/// @return This method returns a false Boolean value if the LPM is not connected. Otherwise, it
///         attempts the request and returns true.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool ProductHardwareInterface::SendWiFiRadioStatus( uint32_t frequencyInKhz )
{
    BOSE_DEBUG( s_logger, "An attempt to send the wireless frequency %d KHz to the LPM is being made.",
                frequencyInKhz );

    if( m_connected == false || m_LpmClient == nullptr )
    {
        BOSE_ERROR( s_logger, "No LPM connection is currently available." );

        return false;
    }
    else
    {
        BOSE_DEBUG( s_logger, "Sending the wireless radio frequency to the LPM is currently not supported." );

        return true;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name  ProductHardwareInterface::SendRebootRequest
///
/// @return This method returns a false Boolean value if the LPM is not connected. Otherwise, it
///         attempts the request and returns true.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool ProductHardwareInterface::SendRebootRequest( )
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

        IpcRebootRequest_t reboot;
        reboot.set_type( REBOOT_TYPE_SYSTEM );

        m_LpmClient->Reboot( reboot );

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
/// @param  std::string bluetoothMacAddress This argument is a standard string representing the
///                                         Bluetooth MAC Address.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductHardwareInterface::SetBlueToothMacAddress( const std::string& bluetoothMacAddress )
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
/// @param  std::string bluetoothDeviceName This argument is a standard string representing the
///                                         Bluetooth device name.
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
/// @param IPCSource_t& sourceSelect
///
/// @return This method returns a false Boolean value if the LPM is not connected. Otherwise, it
///         attempts the request and returns true.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool ProductHardwareInterface::SendSourceSelection( const LPM_IPC_SOURCE_ID sourceSelect )
{
    IPCSource_t source;

    source.set_source( sourceSelect );
    source.set_open_field( 0 );
    source.set_status( 0 );

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
///
/// @name  ProductHardwareInterface::CECSetPhysicalAddress
///
/// @brief This method sends a request to the LPM hardware.
///
/// @param None
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool ProductHardwareInterface::CECSetPhysicalAddress( const uint32_t cecPhysicalAddress )
{
    BOSE_DEBUG( s_logger, "CEC Physical Address will be sent to  LPM" );

    if( m_connected == false || m_LpmClient == nullptr )
    {
        BOSE_ERROR( s_logger, "An LPM send CEC Physical Address request could not be made, as no connection is available." );

        return false;
    }
    else
    {
        IpcCecPhyscialAddress_t cecAddrSetting;
        cecAddrSetting.set_cecphyaddr( cecPhysicalAddress );
        m_LpmClient->SendCecPhysicalAddress( cecAddrSetting );

        return true;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name  ProductHardwareInterface::CECMsgHandler
///
/// @brief This method handles the CEC messages received from LPM
///
/// @param None
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool ProductHardwareInterface::CECMsgHandler( const LpmServiceMessages::IpcCecMessage_t cecMessage )
{
    BOSE_DEBUG( s_logger, "Received CEC Message from LPM" );

    if( m_connected == false || m_LpmClient == nullptr )
    {
        BOSE_ERROR( s_logger, "CEC Message could not be received, as no connection is available." );

        return false;
    }
    else
    {

        BOSE_DEBUG( s_logger, "CEC Message received from LPM. Send to QS" );
        //TODO - Send the message to QuickSetService
        return true;
    }
}
////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief ProductHardwareInterface::Stop
///
/// @todo  Resources, memory, or any client server connections that may need to be released by
///        this module when stopped will need to be determined.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductHardwareInterface::Stop( )
{
    return;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                           End of the Product Application Namespace                           ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
