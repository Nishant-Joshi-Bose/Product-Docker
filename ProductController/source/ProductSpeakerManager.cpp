////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file      ProductSpeakerManager.cpp
///
/// @brief     This header file contains declarations for managing the wireless accessories,
///            including pairing and active speaker control.
///
/// @author    Derek Richardson
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
#include "DPrint.h"
#include "HsmState.h"
#include "FrontDoorClient.h"
#include "ProductSpeakerManager.h"
#include "ProductControllerStates.h"

namespace
{
DPrint s_logger( "ProductSpeakerManager" );
const std::string accessoryFrontDoorURL = "/accessories";
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                             Start of Product Namespace                                       ///
////////////////////////////////////////////////////////////////////////////////////////////////////
namespace ProductApp
{


ProductSpeakerManager* ProductSpeakerManager::GetInstance( NotifyTargetTaskIF*        mainTask,
                                                           Callback< ProductMessage > productNotify,
                                                           ProductHardwareInterface*  hardwareInterface )
{
    static ProductSpeakerManager* instance = new ProductSpeakerManager( mainTask, productNotify, hardwareInterface );

    BOSE_DEBUG( s_logger, "The instance %8p of the Product Speaker Manager was returned.", instance );

    return instance;
}

ProductSpeakerManager::ProductSpeakerManager( NotifyTargetTaskIF*        task,
                                              Callback< ProductMessage > ProductNotify,
                                              ProductHardwareInterface*  hardwareInterface ):
    m_mainTask( task ),
    m_ProductNotify( ProductNotify ),
    m_hardwareInterface( hardwareInterface ),
    m_FrontDoorClientIF( FrontDoor::FrontDoorClient::Create( "ProductSpeakerManager" ) ),
    m_lpmConnected( false )
{
    BOSE_INFO( s_logger, __func__ );
    Init();
}

void ProductSpeakerManager::SetLpmConnectionState( bool connected )
{
    BOSE_INFO( s_logger, "ProductSpeakerManager::LpmConnected: connected = %s", connected ? "true" : "false" );
    m_lpmConnected = connected;
    if( m_lpmConnected )
    {
        RegisterLpmClientEvents();
    }
}

void ProductSpeakerManager::Init()
{
    // @todo - figure out how to better platform controllability of accessories
    ProductPb::AccessorySpeakerState::SpeakerControls* controlable = m_accessorySpeakerState.mutable_controllable();
    controlable->set_subs( true );
    controlable->set_rears( true );

    ProductPb::AccessorySpeakerState::SpeakerControls* enabled = m_accessorySpeakerState.mutable_enabled();
    enabled->set_subs( true );
    enabled->set_rears( true );

    m_accessorySpeakerState.set_pairing( false );

    Callback<bool> cb( std::bind( &ProductSpeakerManager::SetLpmConnectionState, this, std::placeholders::_1 ) );
    m_hardwareInterface->RegisterForLpmClientConnectEvent( cb );
}

bool ProductSpeakerManager::Run()
{
    RegisterFrontDoorEvents();
    return true;
}

void ProductSpeakerManager::RegisterLpmClientEvents()
{
    // Get ASYNC accessory list change events
    Callback<LpmServiceMessages::IpcAccessoryList_t> listCB( std::bind( &ProductSpeakerManager::RecieveAccessoryListCallback, this, std::placeholders::_1 ) );
    bool success = m_hardwareInterface->RegisterForLpmEvents<LpmServiceMessages::IpcAccessoryList_t>( LpmServiceMessages::IPC_AUDIO_RSP_SPEAKER_LIST, listCB );
    BOSE_INFO( s_logger, "Registered for accessory list from lpm %s", ( success ? "successfully" : "unsuccessfully" ) );

    // Get ASYNC pairing mode events
    Callback<LpmServiceMessages::IpcSpeakerPairingMode_t> pairCB( std::bind( &ProductSpeakerManager::PairingCallback, this, std::placeholders::_1 ) );
    success = m_hardwareInterface->RegisterForLpmEvents<LpmServiceMessages::IpcSpeakerPairingMode_t>( LpmServiceMessages::IPC_AUDIO_SPEAKER_PAIRING, pairCB );
    BOSE_INFO( s_logger, "Registered for accessory pairing events %s", ( success ? "successfully" : "unsuccessfully" ) );

    // @todo - how do we want to recover from things like this
}

void ProductSpeakerManager::RegisterFrontDoorEvents()
{
    BOSE_INFO( s_logger, __func__ );
    {
        AsyncCallback<Callback<ProductPb::AccessorySpeakerState>> getAccessoriesCb( std::bind( &ProductSpeakerManager::AccessoriesGetHandler,
                                                                                    this, std::placeholders::_1 ) , m_mainTask );
        m_registerGetAccessoriesCb = m_FrontDoorClientIF->RegisterGet( accessoryFrontDoorURL , getAccessoriesCb );
    }
    {
        AsyncCallback<ProductPb::AccessorySpeakerState, Callback<ProductPb::AccessorySpeakerState>> putAccessoriesCb( std::bind( &ProductSpeakerManager::AccessoriesPutHandler,
                this, std::placeholders::_1, std::placeholders::_2 ) , m_mainTask );
        m_registerPutAccessoriesCb = m_FrontDoorClientIF->RegisterPut<ProductPb::AccessorySpeakerState>( accessoryFrontDoorURL , putAccessoriesCb );
    }

}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                             Callback functions for the front door                            ///
////////////////////////////////////////////////////////////////////////////////////////////////////

void ProductSpeakerManager::AccessoriesGetHandler( const Callback<ProductPb::AccessorySpeakerState> &resp )
{
    BOSE_INFO( s_logger, __func__ );
    resp( m_accessorySpeakerState );
}

void ProductSpeakerManager::AccessoriesPutHandler( const ProductPb::AccessorySpeakerState &req, const Callback<ProductPb::AccessorySpeakerState> &resp )
{
    BOSE_INFO( s_logger, __func__ );
    if( req.has_enabled() )
    {
        SetSpeakersEnabled( req.enabled(), resp );
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
///                      Functions for the front door to start and stop pairing                  ///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductSpeakerManager::PairingFrontDoorRequestCallback( const Callback<ProductPb::AccessorySpeakerState> &frontDoorCB, LpmServiceMessages::IpcSpeakerPairingMode_t pair )
{
    if( pair.has_pairingenabled() )
    {
        m_accessorySpeakerState.set_pairing( pair.pairingenabled() );
    }
    frontDoorCB( m_accessorySpeakerState );
}

void ProductSpeakerManager::DoPairingFrontDoor( bool pair, const Callback<ProductPb::AccessorySpeakerState> &frontDoorCB )
{
    // Need a blocking way to do this or similar but putting off till IP4 when app setup is required
    // For now this will be happy path
    // ChangeState( PROFESSOR_PRODUCT_CONTROLLER_STATE_ACCESSORY_PAIRING );
    Callback<LpmServiceMessages::IpcSpeakerPairingMode_t> doPairingCb( std::bind( &ProductSpeakerManager::PairingFrontDoorRequestCallback, this, frontDoorCB, std::placeholders::_1 ) );
    m_hardwareInterface->SendAccessoryPairing( pair, doPairingCb );
}


////////////////////////////////////////////////////////////////////////////////////////////////////
///          Public functions for the product controller state to start and stop pairing         ///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductSpeakerManager::DoPairing( )
{
    if( !m_accessorySpeakerState.pairing() )
    {
        Callback<LpmServiceMessages::IpcSpeakerPairingMode_t> doPairingCb( std::bind( &ProductSpeakerManager::PairingCallback, this, std::placeholders::_1 ) );
        m_hardwareInterface->SendAccessoryPairing( true, doPairingCb );
    }
}

void ProductSpeakerManager::StopPairing( )
{
    if( m_accessorySpeakerState.pairing() )
    {
        Callback<LpmServiceMessages::IpcSpeakerPairingMode_t> doPairingCb( std::bind( &ProductSpeakerManager::PairingCallback, this, std::placeholders::_1 ) );
        m_hardwareInterface->SendAccessoryPairing( false, doPairingCb );
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////////
///                 Functions to handle setting disbanding of current accessories                ///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductSpeakerManager::DisbandAccessories( const Callback<ProductPb::AccessorySpeakerState> &frontDoorCB )
{
    // @todo - send actual / figure out the desired behavior here
    // Put off till IP4 when app setup is due
    // Since not implemented just respond right away
    frontDoorCB( m_accessorySpeakerState );
}


////////////////////////////////////////////////////////////////////////////////////////////////////
///                 Functions to handle setting rears or sub to active or inactive               ///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductSpeakerManager::SetSpeakersEnabledCallback( const Callback<ProductPb::AccessorySpeakerState> &frontDoorCB, const LpmServiceMessages::IpcSpeakersActive_t req )
{
    ProductPb::AccessorySpeakerState::SpeakerControls* enabled = m_accessorySpeakerState.mutable_enabled();
    // Send message with two bools one for rears one for subs
    if( req.has_rearsenabled() && m_accessorySpeakerState.controllable().rears() )
    {
        enabled->set_rears( req.rearsenabled() );
    }
    if( req.has_subsenabled() && m_accessorySpeakerState.controllable().subs() )
    {
        enabled->set_subs( req.subsenabled() );
    }
    frontDoorCB( m_accessorySpeakerState );
}

void ProductSpeakerManager::SetSpeakersEnabled( const ProductPb::AccessorySpeakerState::SpeakerControls req, const Callback<ProductPb::AccessorySpeakerState> &frontDoorCB )
{
    Callback<LpmServiceMessages::IpcSpeakersActive_t> doPairingCb( std::bind( &ProductSpeakerManager::SetSpeakersEnabledCallback, this, frontDoorCB, std::placeholders::_1 ) );
    bool rears = true;
    bool subs = true;
    if( req.has_rears() && m_accessorySpeakerState.controllable().rears() )
    {
        rears = req.rears();
    }
    else
    {
        rears = m_accessorySpeakerState.enabled().rears();
    }
    if( req.has_subs() && m_accessorySpeakerState.controllable().subs() )
    {
        subs = req.subs();
    }
    else
    {
        subs = m_accessorySpeakerState.enabled().subs();
    }
    m_hardwareInterface->SendAccessoryActive( rears, subs, doPairingCb );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                 Functions to handle async events from LPM and push to front door             ///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductSpeakerManager::RecieveAccessoryListCallback( LpmServiceMessages::IpcAccessoryList_t accList )
{
    BOSE_INFO( s_logger, __func__ );

    m_accessorySpeakerState.clear_rears();
    m_accessorySpeakerState.clear_subs();

    uint8_t numOfSpeakers = 0;

    for( uint8_t i = 0; i < accList.accessory_size(); i++ )
    {
        const auto& accDesc = accList.accessory( i );
        if( accDesc.has_status() && AccessoryStatusIsConnected( accDesc.status() ) )
        {
            if( accDesc.has_type() && AccessoryTypeIsRear( accDesc.type() ) )
            {
                const auto& spkrInfo = m_accessorySpeakerState.add_rears();
                AccessoryDescriptionToAccessorySpeakerInfo( accDesc, spkrInfo );
                numOfSpeakers++;
            }
            else if( accDesc.has_type() && AccessoryTypeIsSub( accDesc.type() ) )
            {
                const auto& spkrInfo = m_accessorySpeakerState.add_subs();
                AccessoryDescriptionToAccessorySpeakerInfo( accDesc, spkrInfo );
                numOfSpeakers++;
            }
        }
    }

    m_FrontDoorClientIF->SendNotification( accessoryFrontDoorURL, m_accessorySpeakerState );
}

void ProductSpeakerManager::PairingCallback( LpmServiceMessages::IpcSpeakerPairingMode_t pair )
{
    if( pair.has_pairingenabled() )
    {
        m_accessorySpeakerState.set_pairing( pair.pairingenabled() );
    }

    m_FrontDoorClientIF->SendNotification( accessoryFrontDoorURL, m_accessorySpeakerState );

    ProductMessage productMessage;
    productMessage.mutable_accessorypairing( )->set_active( m_accessorySpeakerState.pairing() );
    IL::BreakThread( std::bind( m_ProductNotify, productMessage ), m_mainTask );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                 Stuff to convert IpcAccessoryList_t to AccessorySpeakerState                 ///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool ProductSpeakerManager::AccessoryStatusIsConnected( unsigned int status )
{
    switch( static_cast<LpmServiceMessages::AccessoryConnectionStatus_t>( status ) )
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

bool ProductSpeakerManager::AccessoryTypeIsRear( unsigned int type )
{
    switch( static_cast<LpmServiceMessages::AccessoryType_t>( type ) )
    {
    case LpmServiceMessages::ACCESSORY_MAXWELL:
        return true;
    default:
        return false;
    }
}

bool ProductSpeakerManager::AccessoryTypeIsSub( unsigned int type )
{
    switch( static_cast<LpmServiceMessages::AccessoryType_t>( type ) )
    {
    case LpmServiceMessages::ACCESSORY_LOVEY:
    case LpmServiceMessages::ACCESSORY_SKIPPER:
        return true;
    default:
        return false;
    }
}

const char* ProductSpeakerManager::AccessoryTypeToString( unsigned int type )
{
    switch( static_cast<LpmServiceMessages::AccessoryType_t>( type ) )
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

void ProductSpeakerManager::AccessoryDescriptionToAccessorySpeakerInfo( const LpmServiceMessages::AccessoryDescription_t &accDesc,
                                                                        ProductPb::AccessorySpeakerState::AccessorySpeakerInfo* spkrInfo )
{
    if( accDesc.has_type() )
    {
        spkrInfo->set_type( AccessoryTypeToString( accDesc.type() ) );
    }
    spkrInfo->set_available( true );
    if( accDesc.has_status() && ( accDesc.status() == LpmServiceMessages::ACCESSORY_CONNECTION_WIRELESS ) )
    {
        spkrInfo->set_wireless( true );
    }
    else
    {
        spkrInfo->set_wireless( false );
    }

    // TODO - add logic around this when app API is better defined for now always valid
    spkrInfo->set_configurationstatus( "VALID" );

}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                               End of ProductApp Namespace                                    ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
