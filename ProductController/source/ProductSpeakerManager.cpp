////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file      ProductSpeakerManager.cpp
///
/// @brief     This file contains source code for managing the wireless accessories, including
///            pairing and active speaker control.
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
#include "Utilities.h"
#include "HsmState.h"
#include "FrontDoorClient.h"
#include "ProfessorProductController.h"
#include "CustomProductLpmHardwareInterface.h"
#include "ProductSpeakerManager.h"

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
const std::string accessoryFrontDoorURL = "/accessories";

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief ProductSpeakerManager::ProductSpeakerManager
///
/// @param ProfessorProductController& productController
///
////////////////////////////////////////////////////////////////////////////////////////////////////
ProductSpeakerManager::ProductSpeakerManager( ProfessorProductController& ProductController )

    : m_ProductTask( ProductController.GetTask( ) ),
      m_ProductNotify( ProductController.GetMessageHandler( ) ),
      m_ProductLpmHardwareInterface( ProductController.GetLpmHardwareInterface( ) ),
      m_FrontDoorClientIF( FrontDoor::FrontDoorClient::Create( "ProductSpeakerManager" ) ),
      m_lpmConnected( false )
{
    BOSE_INFO( s_logger, __func__ );
    Init( );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief ProductSpeakerManager::Init
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductSpeakerManager::Init( )
{
    ///
    /// @todo Figure out how to better platform controllability of accessories.
    ///
    ProductPb::AccessorySpeakerState::SpeakerControls* controlable = m_accessorySpeakerState.mutable_controllable( );
    controlable->set_subs( true );
    controlable->set_rears( true );

    ProductPb::AccessorySpeakerState::SpeakerControls* enabled = m_accessorySpeakerState.mutable_enabled( );
    enabled->set_subs( true );
    enabled->set_rears( true );

    m_accessorySpeakerState.set_pairing( false );

    Callback<bool> cb( std::bind( &ProductSpeakerManager::SetLpmConnectionState, this, std::placeholders::_1 ) );
    m_ProductLpmHardwareInterface->RegisterForLpmConnection( cb );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief ProductSpeakerManager::SetLpmConnectionState
///
/// @param bool connected
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductSpeakerManager::SetLpmConnectionState( bool connected )
{
    BOSE_INFO( s_logger, "ProductSpeakerManager::LpmConnected: connected = %s", connected ? "true" : "false" );

    m_lpmConnected = connected;

    if( m_lpmConnected )
    {
        RegisterLpmClientEvents( );
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief ProductSpeakerManager::Run
///
/// @return
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool ProductSpeakerManager::Run( )
{
    RegisterFrontDoorEvents( );

    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief ProductSpeakerManager::RegisterLpmClientEvents
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductSpeakerManager::RegisterLpmClientEvents( )
{
    ///
    /// Get ASYNC accessory list change events
    ///
    Callback< LpmServiceMessages::IpcAccessoryList_t >
    listCB( std::bind( &ProductSpeakerManager::RecieveAccessoryListCallback,
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
    pairCB( std::bind( &ProductSpeakerManager::PairingCallback,
                       this,
                       std::placeholders::_1 ) );

    success = m_ProductLpmHardwareInterface->RegisterForLpmEvents< LpmServiceMessages::IpcSpeakerPairingMode_t >
              ( LpmServiceMessages::IPC_AUDIO_SPEAKER_PAIRING, pairCB );

    BOSE_INFO( s_logger, "%s registered for accessory pairing events from the LPM hardware.",
               ( success ? "Successfully" : "Unsuccessfully" ) );

    ///
    /// @todo Functionality may be needed to recover from a failure to register.
    ///
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief ProductSpeakerManager::RegisterFrontDoorEvents
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductSpeakerManager::RegisterFrontDoorEvents( )
{
    BOSE_INFO( s_logger, __func__ );

    {
        AsyncCallback<Callback< ProductPb::AccessorySpeakerState > >
        getAccessoriesCb( std::bind( &ProductSpeakerManager::AccessoriesGetHandler,
                                     this,
                                     std::placeholders::_1 ) ,
                          m_ProductTask );

        m_registerGetAccessoriesCb =
            m_FrontDoorClientIF->RegisterGet( accessoryFrontDoorURL ,
                                              getAccessoriesCb );
    }
    {
        AsyncCallback< ProductPb::AccessorySpeakerState, Callback< ProductPb::AccessorySpeakerState > >
        putAccessoriesCb( std::bind( &ProductSpeakerManager::AccessoriesPutHandler,
                                     this,
                                     std::placeholders::_1,
                                     std::placeholders::_2 ) ,
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
/// @brief ProductSpeakerManager::AccessoriesGetHandler
///
/// @param resp
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductSpeakerManager::AccessoriesGetHandler( const Callback<ProductPb::AccessorySpeakerState> &resp )
{
    BOSE_INFO( s_logger, __func__ );

    resp( m_accessorySpeakerState );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief ProductSpeakerManager::AccessoriesPutHandler
///
/// @param const ProductPb::AccessorySpeakerState& req
///
/// @param const Callback<ProductPb::AccessorySpeakerState>& resp
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductSpeakerManager::AccessoriesPutHandler( const ProductPb::AccessorySpeakerState& req,
                                                   const Callback<ProductPb::AccessorySpeakerState>& resp )
{
    BOSE_INFO( s_logger, __func__ );

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
/// @brief ProductSpeakerManager::PairingFrontDoorRequestCallback
///
/// @param const Callback<ProductPb::AccessorySpeakerState>& frontDoorCB
///
/// @param LpmServiceMessages::IpcSpeakerPairingMode_t pair
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductSpeakerManager::PairingFrontDoorRequestCallback( const Callback<ProductPb::AccessorySpeakerState>& frontDoorCB,
                                                             LpmServiceMessages::IpcSpeakerPairingMode_t pair )
{
    if( pair.has_pairingenabled( ) )
    {
        m_accessorySpeakerState.set_pairing( pair.pairingenabled( ) );
    }

    frontDoorCB( m_accessorySpeakerState );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief ProductSpeakerManager::DoPairingFrontDoor
///
/// @param bool pair
///
/// @param const Callback<ProductPb::AccessorySpeakerState> &frontDoorCB
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductSpeakerManager::DoPairingFrontDoor( bool pair,
                                                const Callback<ProductPb::AccessorySpeakerState> &frontDoorCB )
{
    ///
    /// @todo Need a blocking way to implement this functionality, but will be put off until IP4
    ///       when setup is required. For now send a message to the product controller to change to
    ///       an accessory pairing state.
    ///
    Callback< LpmServiceMessages::IpcSpeakerPairingMode_t >
    doPairingCb( std::bind( &ProductSpeakerManager::PairingFrontDoorRequestCallback,
                            this,
                            frontDoorCB,
                            std::placeholders::_1 ) );

    m_ProductLpmHardwareInterface->SendAccessoryPairing( pair, doPairingCb );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
///          Public Methods for the Product Controller State to Start and Stop Pairing
///
////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief ProductSpeakerManager::DoPairing
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductSpeakerManager::DoPairing( )
{
    if( !m_accessorySpeakerState.pairing( ) )
    {
        Callback< LpmServiceMessages::IpcSpeakerPairingMode_t >
        doPairingCb( std::bind( &ProductSpeakerManager::PairingCallback,
                                this,
                                std::placeholders::_1 ) );

        m_ProductLpmHardwareInterface->SendAccessoryPairing( true, doPairingCb );
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief ProductSpeakerManager::StopPairing
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductSpeakerManager::StopPairing( )
{
    if( m_accessorySpeakerState.pairing( ) )
    {
        Callback< LpmServiceMessages::IpcSpeakerPairingMode_t >
        doPairingCb( std::bind( &ProductSpeakerManager::PairingCallback,
                                this,
                                std::placeholders::_1 ) );

        m_ProductLpmHardwareInterface->SendAccessoryPairing( false, doPairingCb );
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
///                 Functions to handle setting disbanding of current accessories
///
////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief ProductSpeakerManager::DisbandAccessories
///
/// @param const Callback<ProductPb::AccessorySpeakerState> &frontDoorCB
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductSpeakerManager::DisbandAccessories( const Callback<ProductPb::AccessorySpeakerState> &frontDoorCB )
{
    ///
    /// @todo Send actual state, determine the desired behavior here. Put off till IP4 when setup is
    ///       due, since this has not implemented yet, just respond right away.
    ///
    frontDoorCB( m_accessorySpeakerState );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
///        Functions to Handle Setting Rears or Sub Speakers to Active or Inactive
///
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief ProductSpeakerManager::SetSpeakersEnabledCallback
///
/// @param const Callback<ProductPb::AccessorySpeakerState> &frontDoorCB
///
/// @param const LpmServiceMessages::IpcSpeakersActive_t req
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductSpeakerManager::SetSpeakersEnabledCallback( const Callback<ProductPb::AccessorySpeakerState> &frontDoorCB,
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
/// @brief ProductSpeakerManager::SetSpeakersEnabled
///
/// @param const ProductPb::AccessorySpeakerState::SpeakerControls req
///
/// @param const Callback<ProductPb::AccessorySpeakerState> &frontDoorCB
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductSpeakerManager::SetSpeakersEnabled( const ProductPb::AccessorySpeakerState::SpeakerControls req,
                                                const Callback<ProductPb::AccessorySpeakerState> &frontDoorCB )
{
    bool rears = true;
    bool subs = true;

    Callback< LpmServiceMessages::IpcSpeakersActive_t >
    doPairingCb( std::bind( &ProductSpeakerManager::SetSpeakersEnabledCallback,
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

    m_ProductLpmHardwareInterface->SendAccessoryActive( rears, subs, doPairingCb );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
///   Methods to Handle Asynchronous Events from LPM and Sending Notifications to the Front Door
///
////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief ProductSpeakerManager::RecieveAccessoryListCallback
///
/// @param LpmServiceMessages::IpcAccessoryList_t accList
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductSpeakerManager::RecieveAccessoryListCallback( LpmServiceMessages::IpcAccessoryList_t accList )
{
    BOSE_INFO( s_logger, __func__ );

    m_accessorySpeakerState.clear_rears( );
    m_accessorySpeakerState.clear_subs( );

    uint8_t numOfSpeakers = 0;

    for( uint8_t i = 0; i < accList.accessory_size( ); i++ )
    {
        const auto& accDesc = accList.accessory( i );
        if( accDesc.has_status( ) && AccessoryStatusIsConnected( accDesc.status( ) ) )
        {
            if( accDesc.has_type( ) && AccessoryTypeIsRear( accDesc.type( ) ) )
            {
                const auto& spkrInfo = m_accessorySpeakerState.add_rears( );
                AccessoryDescriptionToAccessorySpeakerInfo( accDesc, spkrInfo );
                numOfSpeakers++;
            }
            else if( accDesc.has_type( ) && AccessoryTypeIsSub( accDesc.type( ) ) )
            {
                const auto& spkrInfo = m_accessorySpeakerState.add_subs( );
                AccessoryDescriptionToAccessorySpeakerInfo( accDesc, spkrInfo );
                numOfSpeakers++;
            }
        }
    }

    m_FrontDoorClientIF->SendNotification( accessoryFrontDoorURL, m_accessorySpeakerState );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief ProductSpeakerManager::PairingCallback
///
/// @param LpmServiceMessages::IpcSpeakerPairingMode_t pair
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductSpeakerManager::PairingCallback( LpmServiceMessages::IpcSpeakerPairingMode_t pair )
{
    if( pair.has_pairingenabled( ) )
    {
        m_accessorySpeakerState.set_pairing( pair.pairingenabled( ) );
    }

    m_FrontDoorClientIF->SendNotification( accessoryFrontDoorURL, m_accessorySpeakerState );

    ProductMessage productMessage;
    productMessage.mutable_accessorypairing( )->set_active( m_accessorySpeakerState.pairing( ) );
    IL::BreakThread( std::bind( m_ProductNotify, productMessage ), m_ProductTask );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
///                 Utility Methods for Determining Accessories Status and Types
///
////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief  ProductSpeakerManager::AccessoryStatusIsConnected
///
/// @param  unsigned intstatus
///
/// @return This method returns true if the accessory is connected; otherwise, it returns false.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool ProductSpeakerManager::AccessoryStatusIsConnected( unsigned int status )
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
/// @brief  ProductSpeakerManager::AccessoryTypeIsRear
///
/// @param  unsigned int type
///
/// @return This method returns true if the accessory type is a rear-speaker; otherwise, it returns
///         false.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool ProductSpeakerManager::AccessoryTypeIsRear( unsigned int type )
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
/// @brief  ProductSpeakerManager::AccessoryTypeIsSub
///
/// @param  unsigned int type
///
/// @return This method returns true if the accessory type is a sub-speaker; otherwise, it returns
///         false.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool ProductSpeakerManager::AccessoryTypeIsSub( unsigned int type )
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
/// @brief  ProductSpeakerManager::AccessoryTypeToString
///
/// @param  unsigned int type
///
/// @return This method returns the accessory type as a string.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
const char* ProductSpeakerManager::AccessoryTypeToString( unsigned int type )
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
/// @brief ProductSpeakerManager::AccessoryDescriptionToAccessorySpeakerInfo
///
/// @param const LpmServiceMessages::AccessoryDescription_t &accDesc
///
/// @param ProductPb::AccessorySpeakerState::AccessorySpeakerInfo* spkrInfo
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductSpeakerManager::AccessoryDescriptionToAccessorySpeakerInfo( const LpmServiceMessages::AccessoryDescription_t &accDesc,
                                                                        ProductPb::AccessorySpeakerState::AccessorySpeakerInfo* spkrInfo )
{
    if( accDesc.has_type( ) )
    {
        spkrInfo->set_type( AccessoryTypeToString( accDesc.type( ) ) );
    }

    spkrInfo->set_available( true );

    if( accDesc.has_status( ) && ( accDesc.status( ) == LpmServiceMessages::ACCESSORY_CONNECTION_WIRELESS ) )
    {
        spkrInfo->set_wireless( true );
    }
    else
    {
        spkrInfo->set_wireless( false );
    }

    ///
    /// @todo Add logic around this call when the application API is better defined. For now it is
    ///       always valid.
    ///
    spkrInfo->set_configurationstatus( "VALID" );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                           End of the Product Application Namespace                           ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
