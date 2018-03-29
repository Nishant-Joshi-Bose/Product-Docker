////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file      SpeakerPairingManager.cpp
///
/// @brief     This source code file contains functionality to implement an intent manager class for
///            managing the wireless accessories, including pairing and active speaker control.
///            Wiki here : https://wiki.bose.com/display/A4V/PC+FD+Accessory+Pairing
///
/// @author    Derek Richardson
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
#include "FrontDoorClient.h"
#include "ProfessorProductController.h"
#include "CustomProductLpmHardwareInterface.h"
#include "SpeakerPairingManager.h"
#include "ProductEndpointDefines.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
///                          Start of the Product Application Namespace                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
namespace ProductApp
{

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief Global Constant Expressions
///
////////////////////////////////////////////////////////////////////////////////////////////////////
const std::string accessoryFrontDoorURL = FRONTDOOR_ACCESSORIES_API;
constexpr uint32_t PAIRING_MAX_TIME_MILLISECOND_TIMEOUT_START = 4 * 60 * 1000;
constexpr uint32_t PAIRING_MAX_TIME_MILLISECOND_TIMEOUT_RETRY = 0 ;

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief SpeakerPairingManager::SpeakerPairingManager
///
/// @param NotifyTargetTaskIF&        task
///
/// @param const CliClientMT&         commandLineClient
///
/// @param const FrontDoorClientIF_t& frontDoorClient
///
/// @param ProductController&         productController
///
////////////////////////////////////////////////////////////////////////////////////////////////////
SpeakerPairingManager::SpeakerPairingManager( NotifyTargetTaskIF&        task,
                                              const CliClientMT&         commandLineClient,
                                              const FrontDoorClientIF_t& frontDoorClient,
                                              ProductController&         productController )

    : IntentManager( task, commandLineClient, frontDoorClient, productController ),
      m_CustomProductController( static_cast< ProfessorProductController & >( productController ) ),
      m_ProductTask( m_CustomProductController.GetTask( ) ),
      m_ProductNotify( m_CustomProductController.GetMessageHandler( ) ),
      m_ProductLpmHardwareInterface( m_CustomProductController.GetLpmHardwareInterface( ) ),
      m_FrontDoorClientIF( frontDoorClient ),
      m_lpmConnected( false ),
      m_timer( APTimer::Create( productController.GetTask( ), "AccessoryPairingTimer" ) )
{
    BOSE_INFO( s_logger, "%s is being constructed.", "SpeakerPairingManager" );

    Initialize( );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief SpeakerPairingManager::Initialize
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void SpeakerPairingManager::Initialize( )
{
    ProductPb::AccessorySpeakerState::SpeakerControls* controlable = m_accessorySpeakerState.mutable_controllable( );
    controlable->set_subs( false );
    controlable->set_rears( false );

    ProductPb::AccessorySpeakerState::SpeakerControls* enabled = m_accessorySpeakerState.mutable_enabled( );
    enabled->set_subs( false );
    enabled->set_rears( false );

    m_accessorySpeakerState.set_pairing( false );

    Callback<bool> cb( std::bind( &SpeakerPairingManager::SetLpmConnectionState, this, std::placeholders::_1 ) );
    m_ProductLpmHardwareInterface->RegisterForLpmConnection( cb );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief  SpeakerPairingManager::Handle
///
/// @brief  This method is used to build and send Front Door messages to activate and play a source
///         if it is selected.
///
/// @param  KeyHandlerUtil::ActionType_t& action
///
/// @return This method returns true base on its handling of a playback request.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool SpeakerPairingManager::Handle( KeyHandlerUtil::ActionType_t& action )
{
    BOSE_INFO( s_logger, "%s is in %s handling the action %u.", "SpeakerPairingManager",
               __FUNCTION__,
               action );

    if( action == ( uint16_t )Action::ACTION_START_PAIR_SPEAKERS )
    {
        // This kicks off the state change to pairing
        BOSE_INFO( s_logger, "Speaker pairing is to be started." );
        ProductMessage productMessage;
        productMessage.mutable_accessorypairing( )->set_active( true );
        IL::BreakThread( std::bind( m_ProductNotify, productMessage ), m_ProductTask );
    }
    else if( action == ( uint16_t )Action::ACTION_LPM_PAIR_SPEAKERS )
    {
        BOSE_INFO( s_logger, "Speaker pairing is to be engaged." );
        // This initiates the actual pairing on entry to the state
        DoPairing( );
    }
    else if( action == ( uint16_t )Action::ACTION_STOP_PAIR_SPEAKERS )
    {
        BOSE_INFO( s_logger, "Speaker pairing is to be stopped." );
        // Stops pairing and then exits the state
        StopPairing( );
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief SpeakerPairingManager::SetLpmConnectionState
///
/// @param bool connected
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void SpeakerPairingManager::SetLpmConnectionState( bool connected )
{
    BOSE_INFO( s_logger, "SpeakerPairingManager::LpmConnected: connected = %s", connected ? "true" : "false" );

    m_lpmConnected = connected;

    if( m_lpmConnected )
    {
        RegisterLpmClientEvents( );
        RegisterFrontDoorEvents( );
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief SpeakerPairingManager::RegisterLpmClientEvents
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void SpeakerPairingManager::RegisterLpmClientEvents( )
{
    ///
    /// Get ASYNC accessory list change events
    ///
    Callback< LpmServiceMessages::IpcAccessoryList_t >
    listCB( std::bind( &SpeakerPairingManager::RecieveAccessoryListCallback,
                       this,
                       std::placeholders::_1 ) );

    bool success =  m_ProductLpmHardwareInterface->RegisterForLpmEvents< LpmServiceMessages::IpcAccessoryList_t >
                    ( LpmServiceMessages::IPC_AUDIO_RSP_SPEAKER_LIST, listCB );

    BOSE_INFO( s_logger, "%s registered for accessory list from the LPM hardware.",
               ( success ? "Successfully" : "Unsuccessfully" ) );

    ///
    /// Get ASYNC pairing mode events
    ///
    Callback< LpmServiceMessages::IpcSpeakerPairingMode_t >
    pairCB( std::bind( &SpeakerPairingManager::PairingCallback,
                       this,
                       std::placeholders::_1 ) );

    success = m_ProductLpmHardwareInterface->RegisterForLpmEvents< LpmServiceMessages::IpcSpeakerPairingMode_t >
              ( LpmServiceMessages::IPC_AUDIO_SPEAKER_PAIRING, pairCB );

    BOSE_INFO( s_logger, "%s registered for accessory pairing events from the LPM hardware.",
               ( success ? "Successfully" : "Unsuccessfully" ) );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief SpeakerPairingManager::RegisterFrontDoorEvents
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void SpeakerPairingManager::RegisterFrontDoorEvents( )
{
    BOSE_INFO( s_logger, "SpeakerPairingManager entering method %s.", __FUNCTION__ );

    {
        AsyncCallback<Callback< ProductPb::AccessorySpeakerState >, Callback<FrontDoor::Error> >
        getAccessoriesCb( std::bind( &SpeakerPairingManager::AccessoriesGetHandler,
                                     this,
                                     std::placeholders::_1,
                                     std::placeholders::_2 ) ,
                          m_ProductTask );

        m_registerGetAccessoriesCb =
            m_FrontDoorClientIF->RegisterGet( accessoryFrontDoorURL ,
                                              getAccessoriesCb );
    }
    {
        AsyncCallback< ProductPb::AccessorySpeakerState, Callback< ProductPb::AccessorySpeakerState >, Callback<FrontDoor::Error> >
        putAccessoriesCb( std::bind( &SpeakerPairingManager::AccessoriesPutHandler,
                                     this,
                                     std::placeholders::_1,
                                     std::placeholders::_2,
                                     std::placeholders::_3 ) ,
                          m_ProductTask );

        m_registerPutAccessoriesCb =
            m_FrontDoorClientIF->RegisterPut<ProductPb::AccessorySpeakerState>( accessoryFrontDoorURL ,
                                                                                putAccessoriesCb );
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
///                      Callback Methods for the Front Door Inter-Processing
///
////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief SpeakerPairingManager::AccessoriesGetHandler
///
/// @param resp
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void SpeakerPairingManager::AccessoriesGetHandler( const Callback<ProductPb::AccessorySpeakerState> &resp,
                                                   const Callback<FrontDoor::Error>& error )
{
    BOSE_INFO( s_logger, "SpeakerPairingManager entering method %s.", __FUNCTION__ );

    resp( m_accessorySpeakerState );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief SpeakerPairingManager::AccessoriesPutHandler
///
/// @param const ProductPb::AccessorySpeakerState& req
///
/// @param const Callback<ProductPb::AccessorySpeakerState>& resp
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void SpeakerPairingManager::AccessoriesPutHandler( const ProductPb::AccessorySpeakerState& req,
                                                   const Callback<ProductPb::AccessorySpeakerState>& resp,
                                                   const Callback<FrontDoor::Error>& error )
{
    BOSE_INFO( s_logger, "SpeakerPairingManager entering method %s.", __FUNCTION__ );

    if( req.has_enabled( ) )
    {
        SetSpeakersEnabled( req.enabled( ), resp );
    }
    else if( req.has_pairing( ) )
    {
        DoPairingFrontDoor( req.pairing( ), resp );
    }
    else if( req.has_disbandaccessories( ) )
    {
        DisbandAccessories( resp );
    }
    else
    {
        BOSE_ERROR( s_logger, "Received invalid put request!" );
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
///                      Functions for the front door to start and stop pairing
///
////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief SpeakerPairingManager::DoPairingFrontDoor
///
/// @param bool pair
///
/// @param const Callback<ProductPb::AccessorySpeakerState> &frontDoorCB
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void SpeakerPairingManager::DoPairingFrontDoor( bool pair,
                                                const Callback<ProductPb::AccessorySpeakerState> &frontDoorCB )
{
    KeyHandlerUtil::ActionType_t pairingAction =
        static_cast< KeyHandlerUtil::ActionType_t >( pair ? Action::ACTION_START_PAIR_SPEAKERS : Action::ACTION_STOP_PAIR_SPEAKERS );

    ProductMessage productMessage;
    productMessage.set_action( pairingAction );
    IL::BreakThread( std::bind( m_ProductNotify, productMessage ), m_ProductTask );

    frontDoorCB( m_accessorySpeakerState );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
///          Public Methods for the Product Controller State to Start and Stop Pairing
///
////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief SpeakerPairingManager::DoPairing
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void SpeakerPairingManager::DoPairing( )
{
    Callback< LpmServiceMessages::IpcSpeakerPairingMode_t >
    doPairingCb( std::bind( &SpeakerPairingManager::PairingCallback,
                            this,
                            std::placeholders::_1 ) );

    m_ProductLpmHardwareInterface->SendAccessoryPairing( true, doPairingCb );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief SpeakerPairingManager::StopPairing
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void SpeakerPairingManager::StopPairing( )
{
    Callback< LpmServiceMessages::IpcSpeakerPairingMode_t >
    doPairingCb( std::bind( &SpeakerPairingManager::PairingCallback,
                            this,
                            std::placeholders::_1 ) );

    m_ProductLpmHardwareInterface->SendAccessoryPairing( false, doPairingCb );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
///                 Functions to handle setting disbanding of current accessories
///
////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief SpeakerPairingManager::DisbandAccessories
///
/// @param const Callback<ProductPb::AccessorySpeakerState> &frontDoorCB
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void SpeakerPairingManager::DisbandAccessories( const Callback<ProductPb::AccessorySpeakerState> &frontDoorCB )
{
    Callback< LpmServiceMessages::IpcAccessoryDisbandCommand_t >
    disbandCb( std::bind( &SpeakerPairingManager::DisbandAccessoriesCallback,
                          this,
                          frontDoorCB,
                          std::placeholders::_1 ) );
    m_ProductLpmHardwareInterface->SendAccessoryDisband( disbandCb );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief SpeakerPairingManager::DisbandAccessoriesCallback
///
/// @param const Callback<ProductPb::AccessorySpeakerState> &frontDoorCB - callback to send ne list to
/// @param const LpmServiceMessages::IpcAccessoryDisbandCommand_t accDisband - if it succeeded
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void SpeakerPairingManager::DisbandAccessoriesCallback( const Callback<ProductPb::AccessorySpeakerState> &frontDoorCB,
                                                        LpmServiceMessages::IpcAccessoryDisbandCommand_t accDisband )
{
    if( accDisband.disband() )
    {
        m_accessorySpeakerState.mutable_subs()->Clear();
        m_accessorySpeakerState.mutable_rears()->Clear();

        m_accessorySpeakerState.mutable_controllable()->set_subs( true );
        m_accessorySpeakerState.mutable_controllable()->set_rears( true );
    }

    frontDoorCB( m_accessorySpeakerState );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
///        Functions to Handle Setting Rears or Sub Speakers to Active or Inactive
///
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief SpeakerPairingManager::SetSpeakersEnabledCallback
///
/// @param const Callback<ProductPb::AccessorySpeakerState> &frontDoorCB
///
/// @param const LpmServiceMessages::IpcSpeakersActive_t req
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void SpeakerPairingManager::SetSpeakersEnabledCallback( const Callback<ProductPb::AccessorySpeakerState> &frontDoorCB,
                                                        const LpmServiceMessages::IpcSpeakersActive_t req )
{
    ProductPb::AccessorySpeakerState::SpeakerControls* enabled = m_accessorySpeakerState.mutable_enabled( );

    ///
    /// Send message with two bools: one for rears one for subs.
    ///
    if( req.has_rearsenabled( ) && m_accessorySpeakerState.controllable( ).rears( ) )
    {
        enabled->set_rears( req.rearsenabled( ) );
    }
    if( req.has_subsenabled( ) && m_accessorySpeakerState.controllable( ).subs( ) )
    {
        enabled->set_subs( req.subsenabled( ) );
    }

    frontDoorCB( m_accessorySpeakerState );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief SpeakerPairingManager::SetSpeakersEnabled
///
/// @param const ProductPb::AccessorySpeakerState::SpeakerControls req
///
/// @param const Callback<ProductPb::AccessorySpeakerState> &frontDoorCB
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void SpeakerPairingManager::SetSpeakersEnabled( const ProductPb::AccessorySpeakerState::SpeakerControls req,
                                                const Callback<ProductPb::AccessorySpeakerState> &frontDoorCB )
{
    bool rears = true;
    bool subs = true;

    Callback< LpmServiceMessages::IpcSpeakersActive_t >
    doEnabledCb( std::bind( &SpeakerPairingManager::SetSpeakersEnabledCallback,
                            this,
                            frontDoorCB, std::placeholders::_1 ) );

    if( req.has_rears( ) && m_accessorySpeakerState.controllable( ).rears( ) )
    {
        rears = req.rears( );
    }
    else
    {
        rears = m_accessorySpeakerState.enabled( ).rears( );
    }
    if( req.has_subs( ) && m_accessorySpeakerState.controllable( ).subs( ) )
    {
        subs = req.subs( );
    }
    else
    {
        subs = m_accessorySpeakerState.enabled( ).subs( );
    }

    m_ProductLpmHardwareInterface->SendAccessoryActive( rears, subs, doEnabledCb );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
///   Methods to Handle Asynchronous Events from LPM and Sending Notifications to the Front Door
///
////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief SpeakerPairingManager::RecieveAccessoryListCallback
///
/// @param LpmServiceMessages::IpcAccessoryList_t accList
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void SpeakerPairingManager::RecieveAccessoryListCallback( LpmServiceMessages::IpcAccessoryList_t accList )
{
    BOSE_INFO( s_logger, "SpeakerPairingManager entering method %s.", __FUNCTION__ );

    m_accessorySpeakerState.clear_rears( );
    m_accessorySpeakerState.clear_subs( );

    uint8_t numOfLeftRears  = 0;
    uint8_t numOfRightRears = 0;
    bool rearsEnabled = false;
    bool subsEnabled  = false;

    for( uint8_t i = 0; i < accList.accessory_size( ); i++ )
    {
        const auto& accDesc = accList.accessory( i );
        if( accDesc.has_status( ) && AccessoryStatusIsConnected( accDesc.status( ) ) )
        {
            if( accDesc.has_type( ) && AccessoryTypeIsRear( accDesc.type( ) ) )
            {
                const auto& spkrInfo = m_accessorySpeakerState.add_rears( );
                AccessoryDescriptionToAccessorySpeakerInfo( accDesc, spkrInfo );
                if( accDesc.position() == LpmServiceMessages::ACCESSORY_POSITION_LEFT_REAR )
                {
                    numOfLeftRears++;
                }
                else if( accDesc.position() == LpmServiceMessages::ACCESSORY_POSITION_RIGHT_REAR )
                {
                    numOfRightRears++;
                }
                rearsEnabled |= ( accDesc.active() != LpmServiceMessages::ACCESSORY_DEACTIVATED );
            }
            else if( accDesc.has_type( ) && AccessoryTypeIsSub( accDesc.type( ) ) )
            {
                const auto& spkrInfo = m_accessorySpeakerState.add_subs( );
                AccessoryDescriptionToAccessorySpeakerInfo( accDesc, spkrInfo );
                subsEnabled |= ( accDesc.active() != LpmServiceMessages::ACCESSORY_DEACTIVATED );
            }
        }
    }

    // If we have at least one we want to mark them as controllable
    m_accessorySpeakerState.mutable_controllable()->set_rears( m_accessorySpeakerState.rears_size() > 0 );
    m_accessorySpeakerState.mutable_controllable()->set_subs( m_accessorySpeakerState.subs_size() > 0 );

    // Set controllable fields
    m_accessorySpeakerState.mutable_enabled()->set_rears( rearsEnabled );
    m_accessorySpeakerState.mutable_enabled()->set_subs( subsEnabled );

    // Subwoofers are always in VALID config as LPM controls that to mitigate improper tuning
    for( int i = 0; i < m_accessorySpeakerState.subs_size(); i++ )
    {
        m_accessorySpeakerState.mutable_subs( i )->set_configurationstatus( "VALID" );
    }

    // Rears we send off to get valid config
    const char* rearConfig = AccessoryRearConiguration( numOfLeftRears, numOfRightRears );
    for( int i = 0; i < m_accessorySpeakerState.rears_size(); i++ )
    {
        m_accessorySpeakerState.mutable_rears( i )->set_configurationstatus( rearConfig );
    }

    m_FrontDoorClientIF->SendNotification( accessoryFrontDoorURL, m_accessorySpeakerState );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief SpeakerPairingManager::PairingCallback
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void SpeakerPairingManager::HandleTimeOut()
{
    BOSE_INFO( s_logger, "SpeakerPairingManager entering method %s", __FUNCTION__ );
    StopPairing( );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief SpeakerPairingManager::PairingCallback
///
/// @param LpmServiceMessages::IpcSpeakerPairingMode_t pair
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void SpeakerPairingManager::PairingCallback( LpmServiceMessages::IpcSpeakerPairingMode_t pair )
{
    BOSE_INFO( s_logger, "SpeakerPairingManager entering method %s with pair mode %d", __FUNCTION__, pair.pairingenabled( ) );

    if( pair.pairingenabled( ) && m_accessorySpeakerState.pairing() )
    {
        m_timer->SetTimeouts( PAIRING_MAX_TIME_MILLISECOND_TIMEOUT_START,
                              PAIRING_MAX_TIME_MILLISECOND_TIMEOUT_RETRY );

        m_timer->Start( std::bind( &SpeakerPairingManager::HandleTimeOut,
                                   this ) );
    }
    else
    {
        m_timer->Stop( );
    }

    m_accessorySpeakerState.set_pairing( pair.pairingenabled( ) );

    ProductMessage productMessage;
    productMessage.mutable_accessorypairing( )->set_active( m_accessorySpeakerState.pairing( ) );


    IL::BreakThread( std::bind( m_ProductNotify, productMessage ), m_ProductTask );

    // Need to always notify here because we need to let brussels know for UI and to rectify
    // our white lie made earlier in DoPairingFrontDoor when the request was made where we say it
    // was started before it does.
    m_FrontDoorClientIF->SendNotification( accessoryFrontDoorURL, m_accessorySpeakerState );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
///                 Utility Methods for Determining Accessories Status and Types
///
////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief  SpeakerPairingManager::AccessoryRearConiguration
///
/// @param  uint8_t numLeft  - number of left channel rears
///         uint8_t numRight - number of right channel rears
///
/// @return This method returns a char* based on whether the configuration is valid
///
////////////////////////////////////////////////////////////////////////////////////////////////////
const char* SpeakerPairingManager::AccessoryRearConiguration( uint8_t numLeft, uint8_t numRight )
{
    if( numLeft == 0 )
    {
        if( numRight == 1 )
        {
            return "MISSING_LEFT";
        }
        else if( numRight > 1 )
        {
            return "TWO_RIGHT";
        }
    }

    if( numRight == 0 )
    {
        if( numLeft == 1 )
        {
            return "MISSING_RIGHT";
        }
        else if( numLeft > 1 )
        {
            return "TWO_LEFT";
        }
    }

    return "VALID";
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief  SpeakerPairingManager::AccessoryStatusIsConnected
///
/// @param  unsigned intstatus
///
/// @return This method returns true if the accessory is connected; otherwise, it returns false.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool SpeakerPairingManager::AccessoryStatusIsConnected( unsigned int status )
{
    switch( static_cast< LpmServiceMessages::AccessoryConnectionStatus_t >( status ) )
    {
    case LpmServiceMessages::ACCESSORY_CONNECTION_WIRED:
    case LpmServiceMessages::ACCESSORY_CONNECTION_WIRELESS:
    case LpmServiceMessages::ACCESSORY_CONNECTION_BOTH:
    case LpmServiceMessages::ACCESSORY_CONNECTION_EXPECTED:
        return true;
    default:
        return false;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief  SpeakerPairingManager::AccessoryTypeIsRear
///
/// @param  unsigned int type
///
/// @return This method returns true if the accessory type is a rear-speaker; otherwise, it returns
///         false.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool SpeakerPairingManager::AccessoryTypeIsRear( unsigned int type )
{
    switch( static_cast< LpmServiceMessages::AccessoryType_t >( type ) )
    {
    case LpmServiceMessages::ACCESSORY_MAXWELL:
        return true;
    default:
        return false;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief  SpeakerPairingManager::AccessoryTypeIsSub
///
/// @param  unsigned int type
///
/// @return This method returns true if the accessory type is a sub-speaker; otherwise, it returns
///         false.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool SpeakerPairingManager::AccessoryTypeIsSub( unsigned int type )
{
    switch( static_cast< LpmServiceMessages::AccessoryType_t >( type ) )
    {
    case LpmServiceMessages::ACCESSORY_LOVEY:
    case LpmServiceMessages::ACCESSORY_SKIPPER:
        return true;
    default:
        return false;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief  SpeakerPairingManager::AccessoryTypeToString
///
/// @param  unsigned int type
///
/// @return This method returns the accessory type as a string.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
const char* SpeakerPairingManager::AccessoryTypeToString( unsigned int type )
{
    switch( static_cast< LpmServiceMessages::AccessoryType_t >( type ) )
    {
    case LpmServiceMessages::ACCESSORY_LOVEY:
        return "Acoustimass 150";
    case LpmServiceMessages::ACCESSORY_SKIPPER:
        return "Acoustimass 300";
    case LpmServiceMessages::ACCESSORY_MAXWELL:
        return "Virtually Invisible 300";
    default:
        BOSE_ERROR( s_logger, "Received invalid accessory to convert to string" );
        return "Invalid";
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief SpeakerPairingManager::AccessoryDescriptionToAccessorySpeakerInfo
///
/// @param const LpmServiceMessages::AccessoryDescription_t &accDesc
///
/// @param ProductPb::AccessorySpeakerState::AccessorySpeakerInfo* spkrInfo
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void SpeakerPairingManager::AccessoryDescriptionToAccessorySpeakerInfo( const LpmServiceMessages::AccessoryDescription_t &accDesc,
                                                                        ProductPb::AccessorySpeakerState::AccessorySpeakerInfo* spkrInfo )
{
    if( accDesc.has_type( ) )
    {
        spkrInfo->set_type( AccessoryTypeToString( accDesc.type( ) ) );
    }

    // If it is expected it went missing so need to show it isn't availible
    spkrInfo->set_available( accDesc.status( ) != LpmServiceMessages::ACCESSORY_CONNECTION_EXPECTED );

    if( accDesc.has_status( ) && ( ( accDesc.status( ) == LpmServiceMessages::ACCESSORY_CONNECTION_WIRELESS ) ||
                                   ( accDesc.status( ) == LpmServiceMessages::ACCESSORY_CONNECTION_EXPECTED ) ) )
    {
        spkrInfo->set_wireless( true );
    }
    else
    {
        spkrInfo->set_wireless( false );
    }

    spkrInfo->set_serialnum( accDesc.sn() );
    spkrInfo->set_version( accDesc.version() );

}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                           End of the Product Application Namespace                           ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
