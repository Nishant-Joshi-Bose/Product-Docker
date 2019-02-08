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
/// @attention Copyright (C) 2018 Bose Corporation All Rights Reserved
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
#include "CustomProductController.h"
#include "CustomProductLpmHardwareInterface.h"
#include "SpeakerPairingManager.h"
#include "ProductEndpointDefines.h"
#include "PGCErrorCodes.h"
#include "ProductDataCollectionDefines.h"
#include "APTimer.h"

///
/// Class Name Declaration for Logging
///
namespace
{
constexpr char CLASS_NAME[ ] = "SpeakerPairingManager";
}

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
constexpr uint32_t PAIRING_MAX_TIME_MILLISECOND_TIMEOUT_START = 4 * 60 * 1000;
constexpr uint32_t PAIRING_MAX_TIME_MILLISECOND_TIMEOUT_RETRY = 0;
constexpr uint32_t REAR_ACCESSORY_MAX_CONNECT_TIME_MS = 30 * 1000;

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
      m_CustomProductController( static_cast< CustomProductController & >( productController ) ),
      m_ProductTask( m_CustomProductController.GetTask( ) ),
      m_ProductLpmHardwareInterface( m_CustomProductController.GetLpmHardwareInterface( ) ),
      m_FrontDoorClientIF( frontDoorClient ),
      m_DataCollectionClient( productController.GetDataCollectionClient() ),
      m_lpmConnected( false ),
      m_timerRearAccessoryConnect( APTimer::Create( m_ProductTask, "RearAccessoryConnectTimer" ) )
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

    auto func = [this]( bool enabled )
    {
        if( enabled && m_firstAccessoryListReceived )
        {
            // Connection to DataCollection server established, send current accessories list (unless we never got one since boot)
            m_DataCollectionClient->SendData( std::make_shared< ProductPb::AccessorySpeakerState >( m_accessorySpeakerState ),
                                              DATA_COLLECTION_ACCESSORIES );
        }
    };
    m_DataCollectionClient->RegisterForEnabledNotifications( Callback<bool>( func ) );
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
    BOSE_INFO( s_logger, "%s::%s is handling the intent %s", CLASS_NAME, __func__, CommonIntentHandler::GetIntentName( action ).c_str( ) );

    if( action == ( uint16_t )Action::ACTION_LPM_PAIR_SPEAKERS )
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
    listCB( std::bind( &SpeakerPairingManager::ReceiveAccessoryListCallback,
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
    BOSE_INFO( s_logger, "%s entering method %s.", CLASS_NAME, __func__ );

    {
        AsyncCallback<Callback< ProductPb::AccessorySpeakerState >, Callback<FrontDoor::Error> >
        getAccessoriesCb( std::bind( &SpeakerPairingManager::AccessoriesGetHandler,
                                     this,
                                     std::placeholders::_1,
                                     std::placeholders::_2 ),
                          m_ProductTask );

        m_registerGetAccessoriesCb =
            m_FrontDoorClientIF->RegisterGet( FRONTDOOR_ACCESSORIES_API,
                                              getAccessoriesCb,
                                              FrontDoor::PUBLIC,
                                              FRONTDOOR_PRODUCT_CONTROLLER_VERSION,
                                              FRONTDOOR_PRODUCT_CONTROLLER_GROUP_NAME );
    }
    {
        AsyncCallback< ProductPb::AccessorySpeakerState, Callback< ProductPb::AccessorySpeakerState >, Callback<FrontDoor::Error> >
        putAccessoriesCb( std::bind( &SpeakerPairingManager::AccessoriesPutHandler,
                                     this,
                                     std::placeholders::_1,
                                     std::placeholders::_2,
                                     std::placeholders::_3 ),
                          m_ProductTask );

        m_registerPutAccessoriesCb =
            m_FrontDoorClientIF->RegisterPut<ProductPb::AccessorySpeakerState>(
                FRONTDOOR_ACCESSORIES_API,
                putAccessoriesCb,
                FrontDoor::PUBLIC,
                FRONTDOOR_PRODUCT_CONTROLLER_VERSION,
                FRONTDOOR_PRODUCT_CONTROLLER_GROUP_NAME );
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
/// @param Callback<ProductPb::AccessorySpeakerState> resp
///
/// @param Callback<FrontDoor::Error> error
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void SpeakerPairingManager::AccessoriesGetHandler( Callback<ProductPb::AccessorySpeakerState> resp,
                                                   Callback<FrontDoor::Error> error )
{
    BOSE_INFO( s_logger, "%s entering method %s.", CLASS_NAME, __func__ );

    resp( m_accessorySpeakerState );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief SpeakerPairingManager::AccessoriesPutHandler
///
/// @param ProductPb::AccessorySpeakerState req
///
/// @param Callback<ProductPb::AccessorySpeakerState> resp
///
/// @param Callback<FrontDoor::Error> error
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void SpeakerPairingManager::AccessoriesPutHandler( ProductPb::AccessorySpeakerState req,
                                                   Callback<ProductPb::AccessorySpeakerState> resp,
                                                   Callback<FrontDoor::Error> error )
{
    BOSE_INFO( s_logger, "%s entering method %s.", CLASS_NAME, __func__ );

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
        FrontDoor::Error errorMessage;
        errorMessage.set_code( PGCErrorCodes::ERROR_CODE_PRODUCT_CONTROLLER_CUSTOM );
        errorMessage.set_subcode( PGCErrorCodes::ERROR_SUBCODE_ACCESSORIES );
        errorMessage.set_message( "Accessory message did not have valid command option." );
        error.Send( errorMessage );
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
        static_cast< KeyHandlerUtil::ActionType_t >( pair ? Action::ACTION_START_PAIR_SPEAKERS_LAN : Action::ACTION_STOP_PAIR_SPEAKERS );

    ProductMessage productMessage;
    productMessage.set_action( pairingAction );
    m_CustomProductController.SendAsynchronousProductMessage( productMessage );

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

    m_DataCollectionClient->SendData( std::make_shared< ProductPb::AccessorySpeakerState >( m_accessorySpeakerState ),
                                      DATA_COLLECTION_ACCESSORIES );
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
/// @brief SpeakerPairingManager::ReceiveAccessoryListCallback
///
/// @param LpmServiceMessages::IpcAccessoryList_t accList
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void SpeakerPairingManager::ReceiveAccessoryListCallback( LpmServiceMessages::IpcAccessoryList_t accList )
{
    BOSE_INFO( s_logger, "%s entering method %s. Accessory list = %s", CLASS_NAME, __func__, accList.DebugString().c_str() );

    m_accessoryListReceived = true;
    m_firstAccessoryListReceived = true;

    ProductPb::AccessorySpeakerState oldAccessorySpeakerState;
    oldAccessorySpeakerState.CopyFrom( m_accessorySpeakerState );

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

    // Subwoofers are in VALID config if it is really connected or expected as LPM controls that to mitigate improper tuning
    for( int i = 0; i < m_accessorySpeakerState.subs_size(); i++ )
    {
        m_accessorySpeakerState.mutable_subs( i )->set_configurationstatus( "VALID" );
    }

    // check if any previously connected subwoofer is disconnected
    if( oldAccessorySpeakerState.subs_size() > m_accessorySpeakerState.subs_size() )
    {
        BOSE_INFO( s_logger, "subs changed from %d to %d", oldAccessorySpeakerState.subs_size(), m_accessorySpeakerState.subs_size() );
        DetectMissingSub( oldAccessorySpeakerState );
    }

    // Rears we send off to get valid config
    uint8_t oldAccessorySpeakerStateRearSize = oldAccessorySpeakerState.rears().size();
    const char* rearConfig = AccessoryRearConiguration( numOfLeftRears, numOfRightRears, oldAccessorySpeakerStateRearSize );
    for( int i = 0; i < m_accessorySpeakerState.rears_size(); i++ )
    {
        m_accessorySpeakerState.mutable_rears( i )->set_configurationstatus( rearConfig );
    }

    // Check if rears disconnected
    if( ( strcmp( rearConfig, "VALID" ) == 0 ) )
    {
        // AccessoryRearConiguration() returns rearConfig "VALID" if any of these is true:
        // (1) One Left and One Right
        // (2) No Left and No right
        // (3) Both left and right rears disconnect, one of them reconnect.
        if( m_waitSecondRearAccessoryConnect == false )
        {
            DetectMissingRears( oldAccessorySpeakerState );
        }
        else
        {
            if( ( numOfLeftRears == 1 ) and ( numOfRightRears == 1 ) )
            {
                BOSE_INFO( s_logger, "Both rears connected, stop timer" );
                m_timerRearAccessoryConnect->Stop();
                m_waitSecondRearAccessoryConnect = false;
            }
        }
    }
    else
    {
        if( m_waitSecondRearAccessoryConnect == true )
        {
            m_timerRearAccessoryConnect->Stop();
            m_waitSecondRearAccessoryConnect = false;
        }
    }

    SendAccessoryPairingStateToProduct();

    m_FrontDoorClientIF->SendNotification( FRONTDOOR_ACCESSORIES_API, m_accessorySpeakerState );

    if( oldAccessorySpeakerState.SerializeAsString() != m_accessorySpeakerState.SerializeAsString() )
    {
        m_DataCollectionClient->SendData( std::make_shared< ProductPb::AccessorySpeakerState >( m_accessorySpeakerState ),
                                          DATA_COLLECTION_ACCESSORIES );
    }

    ProductMessage productMessage;
    productMessage.set_accessoriesareknown( true );
    m_CustomProductController.SendAsynchronousProductMessage( productMessage );
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
    BOSE_INFO( s_logger, "%s entering method %s with pair mode %d", CLASS_NAME, __func__, pair.pairingenabled( ) );

    if( pair.pairingenabled( ) && !m_accessorySpeakerState.pairing() )
    {
        m_accessoryListReceived = false;
    }

    m_accessorySpeakerState.set_pairing( pair.pairingenabled( ) );
    SendAccessoryPairingStateToProduct();
    // Need to notify here only if pairing is being set. If pairing has finished and is set to false,
    // will notify UI with full message from ReceiveAccessoryListCallback.  However, if accessory
    // list has already been received we need to send the notification here too.
    if( m_accessorySpeakerState.pairing( ) || m_accessoryListReceived )
    {
        m_FrontDoorClientIF->SendNotification( FRONTDOOR_ACCESSORIES_API, m_accessorySpeakerState );
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief SpeakerPairingManager::SendAccessoryPairingStateToProduct
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void SpeakerPairingManager::SendAccessoryPairingStateToProduct( )
{
    BOSE_INFO( s_logger, __func__ );
    if( m_accessorySpeakerState.pairing() || m_accessoryListReceived )
    {
        ProductMessage productMessage;
        productMessage.mutable_accessorypairing( )->CopyFrom( m_accessorySpeakerState );
        m_CustomProductController.SendAsynchronousProductMessage( productMessage );
    }
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
///         uint8_t oldRearSize - num of rear accessories in previous list
///
/// @return This method returns a char* based on whether the configuration is valid
///
////////////////////////////////////////////////////////////////////////////////////////////////////
const char* SpeakerPairingManager::AccessoryRearConiguration( uint8_t numLeft, uint8_t numRight, uint8_t oldRearSize )
{
    // If both rear surrounds are connected with soundbar and they are both powered off,
    // LPM sends the accessory list with status of both rears set to NONE (0), thus rears_size() = 0.
    // When they are powered on again, although it is possible they connect with soundbar at the same time,
    // it is more likely there is a delay of a few seconds.
    // When the first connects, we receive the accesory list from LPM with one rear status set to CONNECTED and the other
    // NONE and oldRearSize is 0.
    // If AccessoryRearConiguration() returns MISSING_LEFT or MISSING_RIGHT, the lightbar blinks for non-critical error.
    // This is confusing to user if power is applied to both rear surrounds at the same time.
    // In this case, return "VALID" even though only one rear is connected. Start a timer to wait for the second rear.
    // If the second rear connects before the timer goes off, the lightbar does not blink.
    // If the second rear does not connect before the timer goes off, the timer callback sets the configurationStatus
    // of the first rear accessory to MISSING_SECOND_REAR so the lightbar blinks.
    // This can happen if one rear surround is powered on but the other is not

    if( oldRearSize != 0 )
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
    }

    return "VALID";
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief  SpeakerPairingManager::AccessoryStatusIsConnected
///
/// @param  unsigned int status
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
        return "BOSE_BASS_MODULE_300";
    case LpmServiceMessages::ACCESSORY_SKIPPER:
        return "BOSE_BASS_MODULE_700";
    case LpmServiceMessages::ACCESSORY_MAXWELL:
        return "BOSE_SURROUND_SPEAKER";
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


    switch( accDesc.status() )
    {
    case LpmServiceMessages::ACCESSORY_CONNECTION_WIRELESS:
    case LpmServiceMessages::ACCESSORY_CONNECTION_WIRED:
    case LpmServiceMessages::ACCESSORY_CONNECTION_BOTH:
        spkrInfo->set_available( true );
        break;
    case LpmServiceMessages::ACCESSORY_CONNECTION_EXPECTED:       // Expected accessory but haven't heard from it
    case LpmServiceMessages::ACCESSORY_CONNECTION_LEGACY_PAIRING: // This is a half paired state used for very old accessories
    case LpmServiceMessages::ACCESSORY_CONNECTION_NONE:           // Not connected at all
        spkrInfo->set_available( false );
        break;
    }

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
///
///   Functions to detect whether any accessory speaker is disconnected or reconnected
///
////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief SpeakerPairingManager::DetectMissingSub
///
/// @param const ProductPb::AccessorySpeakerState& oldAccessorySpeakerState
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void SpeakerPairingManager::DetectMissingSub( const ProductPb::AccessorySpeakerState& oldAccessorySpeakerState )
{
    BOSE_INFO( s_logger, "%s entering method %s", CLASS_NAME, __func__ );
    if( m_accessorySpeakerState.subs_size() == 0 )
    {
        // New accessorySpeakerState does not include sub but oldAccessorySpeakerState does (subs_size > 0).
        // This can happen if:
        // (1) one sub was connected but now disconnected, or
        // (2) two subs were connected but lose connection simultaneously
        // Copy one sub info from old speakerState to the current list,
        // but set configuration to MISSING_BASS so lightbar blinks.
        const auto& old_sub_info = oldAccessorySpeakerState.subs( 0 );
        const auto& spkrInfo = m_accessorySpeakerState.add_subs();
        spkrInfo->set_type( old_sub_info.type() );
        spkrInfo->set_wireless( old_sub_info.wireless() );
        spkrInfo->set_serialnum( old_sub_info.serialnum() );
        spkrInfo->set_version( old_sub_info.version() );
        spkrInfo->set_available( false );
        spkrInfo->set_configurationstatus( "MISSING_BASS" );
    }
    else
    {
        // New accessorySpeakerState includes sub but the number 1 is less than previous list 2.
        // This happens if two subs were connected, but one of them lost connection.
        // Find which one lost connection
        int missed_sub_index = -1;

        for( int i = 0; i < oldAccessorySpeakerState.subs_size(); i++ )
        {
            bool sub_missing = true;
            for( int j = 0; j < m_accessorySpeakerState.subs_size(); j++ )
            {
                if( oldAccessorySpeakerState.subs( i ).serialnum() == m_accessorySpeakerState.subs( j ).serialnum() )
                {
                    // It exists in both list, so not missing
                    sub_missing = false;
                    break;
                }
            }
            // Can't find it in new list, it is missing
            if( sub_missing == true )
            {
                missed_sub_index = i;
                break;
            }
        }
        if( missed_sub_index >= 0 )
        {
            // Found the disconnected sub, copy the info from old list but set configurationStatus to MISSING_BASS
            // The status of the other sub is still valid
            BOSE_INFO( s_logger, "Sub %d disconnected", missed_sub_index );
            const auto missed_sub_info = oldAccessorySpeakerState.subs( missed_sub_index );
            const auto& spkrInfo = m_accessorySpeakerState.add_subs();
            spkrInfo->set_type( missed_sub_info.type() );
            spkrInfo->set_wireless( missed_sub_info.wireless() );
            spkrInfo->set_serialnum( missed_sub_info.serialnum() );
            spkrInfo->set_version( missed_sub_info.version() );
            spkrInfo->set_available( false );
            spkrInfo->set_configurationstatus( "MISSING_ONE_BASS" );
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief SpeakerPairingManager::RearAccessoryConnectTimeout
///
///
////////////////////////////////////////////////////////////////////////////////////////////////////

void SpeakerPairingManager::RearAccessoryConnectTimeout()
{
    // The second rear accessory did not re-connect and the timer goes off
    // set the configurationStatus of the connected rear to MISSING_SECOND_REAR so lightbar blinks
    BOSE_INFO( s_logger, "%s entering method %s", CLASS_NAME, __func__ );
    m_accessorySpeakerState.mutable_rears( 0 )->set_configurationstatus( "MISSING_SECOND_REAR" );
    m_FrontDoorClientIF->SendNotification( FRONTDOOR_ACCESSORIES_API, m_accessorySpeakerState );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief SpeakerPairingManager::DetectMissingRears
///
/// @param const ProductPb::AccessorySpeakerState& oldAccessorySpeakerState
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void SpeakerPairingManager::DetectMissingRears( const ProductPb::AccessorySpeakerState& oldAccessorySpeakerState )
{
    BOSE_INFO( s_logger, "%s entering method %s", CLASS_NAME, __func__ );
    if( ( m_accessorySpeakerState.rears_size() == 0 ) and ( oldAccessorySpeakerState.rears_size() != 0 ) )
    {
        // The new speakerState does not include any rear surround but the old speakerState has rear surround
        // This can happen if both surrounds are disconnected simultaneously, e.g., on the same power strip and turned off.
        BOSE_INFO( s_logger, "rears changed from %d to 0", oldAccessorySpeakerState.rears_size() );
        if( oldAccessorySpeakerState.rears( 0 ).configurationstatus() == "VALID" )
        {
            // If old status VALID but new status does not contain rear, add one rear but set status to MISSING_REARS so lightbar blinks
            const auto& spkrInfo = m_accessorySpeakerState.add_rears();
            const auto& old_rear_info = oldAccessorySpeakerState.rears( 0 );
            spkrInfo->set_type( old_rear_info.type() );
            spkrInfo->set_wireless( old_rear_info.wireless() );
            spkrInfo->set_serialnum( old_rear_info.serialnum() );
            spkrInfo->set_version( old_rear_info.version() );
            spkrInfo->set_available( false );
            spkrInfo->set_configurationstatus( "MISSING_REARS" );
        }
    }
    if( ( m_accessorySpeakerState.rears_size() == 1 ) and ( m_accessorySpeakerState.rears( 0 ).configurationstatus() == "VALID" ) )
    {
        // Both Maxwells were disconnected, one is re-connected.
        // The second Maxwell is not connected yet, it is expected to connect with a few seconds
        // Start the timer to wait for it.
        BOSE_INFO( s_logger, "One rear connected, start timer" );
        m_timerRearAccessoryConnect->SetTimeouts( REAR_ACCESSORY_MAX_CONNECT_TIME_MS, 0 );
        m_timerRearAccessoryConnect->Start( std::bind( &SpeakerPairingManager::RearAccessoryConnectTimeout, this ) );
        m_waitSecondRearAccessoryConnect = true;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                           End of the Product Application Namespace                           ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
