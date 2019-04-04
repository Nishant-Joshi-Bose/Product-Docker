////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file      ProductBLERemoteManager.cpp
///
/// @brief     This file implements BLE remote management.
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
#include <string>
#include <algorithm>
#include "Utilities.h"
#include "CustomProductController.h"
#include "CustomProductLpmHardwareInterface.h"
#include "ProductBLERemoteManager.h"
#include "SharedProto.pb.h"
#include "ProtoToMarkup.h"
#include "EndPointsDefines.h"
#include "ProductEndpointDefines.h"
#include "ProductSTS.pb.h"
#include "SystemSourcesProperties.pb.h"
#include "SHELBY_SOURCE.h"
#include "CustomProductKeyInputManager.h"
#include "SystemUtils.h"

using namespace ProductPb;
using namespace KeplerPb;
using namespace A4V_RemoteCommunicationServiceMessages;
using namespace A4VRemoteCommunication;
using namespace RCS_PB_MSG;

namespace
{
constexpr char s_configFile[] = "/opt/Bose/etc/KeplerConfig.json";
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                             Start of Product Namespace                                       ///
////////////////////////////////////////////////////////////////////////////////////////////////////
namespace ProductApp
{


////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductBLERemoteManager::ProductBLERemoteManager
///
/// @param ProductController
///
/// @return This method does not return anything.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
ProductBLERemoteManager::ProductBLERemoteManager( CustomProductController& ProductController ) :
    m_ProductTask( ProductController.GetTask( ) ),
    m_ProductNotify( ProductController.GetMessageHandler( ) ),
    m_ProductController( ProductController )
{
    BOptional<std::string> config = SystemUtils::ReadFile( s_configFile );
    if( config )
    {
        try
        {
            ProtoToMarkup::FromJson( *config, &m_keplerConfig );
            BOSE_INFO( s_logger, "%s %s loaded", __PRETTY_FUNCTION__, s_configFile );
        }
        catch( const ProtoToMarkup::MarkupError & e )
        {
            BOSE_ERROR( s_logger, "%s KeplerConfig markup error - %s", __PRETTY_FUNCTION__, e.what( ) );
        }
    }
    else
    {
        BOSE_ERROR( s_logger, "%s failed to load config %s", __PRETTY_FUNCTION__, s_configFile );
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductBLERemoteManager::InitializeFrontDoor
///
/// @brief
///
/// @param  This method does not take any arguments.
///
/// @return This method does not return anything.
///
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductBLERemoteManager::InitializeFrontDoor( )
{
    auto frontDoorClient = m_ProductController.GetFrontDoorClient();

    auto handleNowSelection = [ this ]( const SoundTouchInterface::NowSelectionInfo & nowSelection )
    {
        UpdateNowSelection( nowSelection );
    };

    AsyncCallback< SoundTouchInterface::NowSelectionInfo > nowSelCb( handleNowSelection, m_ProductTask );
    frontDoorClient->RegisterNotification<SoundTouchInterface::NowSelectionInfo>( FRONTDOOR_CONTENT_NOWSELECTIONINFO_API, nowSelCb );
    frontDoorClient->SendGet<SoundTouchInterface::NowSelectionInfo, FrontDoor::Error>( FRONTDOOR_CONTENT_NOWSELECTIONINFO_API, nowSelCb, {} );

    //System power control notification registration and callback handling
    auto handleSystemPowerControl = [this]( SystemPowerPb::SystemPowerControl systemPowerControlState )
    {
        if( systemPowerControlState.power() == SystemPowerPb::SystemPowerControl_State_ON )
        {
            PowerOn();
        }
        else
        {
            PowerOff();
        }
    };
    //System power control get registration
    AsyncCallback< SystemPowerPb::SystemPowerControl > powerCb( handleSystemPowerControl, m_ProductTask );
    frontDoorClient->RegisterNotification<SystemPowerPb::SystemPowerControl>( FRONTDOOR_SYSTEM_POWER_CONTROL_API, powerCb );
    frontDoorClient->SendGet<SystemPowerPb::SystemPowerControl, FrontDoor::Error>( FRONTDOOR_SYSTEM_POWER_CONTROL_API, powerCb, {} );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductBLERemoteManager::InitializeRCS
///
/// @brief
///
/// @param  This method does not take any arguments.
///
/// @return This method does not return anything.
///
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductBLERemoteManager::InitializeRCS( )
{
    m_RCSClient = A4VRemoteCommClient::Create( "RCSTestClient", m_ProductTask );

    m_RCSClient->RegisterDisconnectCb( {} );
    m_RCSClient->Connect( {} );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductBLERemoteManager::Run
///
/// @brief  This method starts the main task for the ProductBLERemoteManager class.
///
/// @param  This method does not take any arguments.
///
/// @return This method does not return anything.
///
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductBLERemoteManager::Run( )
{
    InitializeFrontDoor();
    InitializeRCS();
    m_ProductController.GetSourceInfo().RegisterSourceListener(
        [ this ]( const SoundTouchInterface::Sources & sources )
    {
        UpdateAvailableSources( sources );
    } );

    auto pairingNotify = [ this ]( PairingNotify n )
    {
        auto wasRemoteConnected = m_remoteConnected;
        m_remoteConnected = n.has_status() && ( n.status() == RemoteStatus::PSTATE_BONDED );
        if( m_remoteConnected && !wasRemoteConnected )
        {
            UpdateBacklight( );
        }

        if( n.has_status() )
        {
            m_remoteStatus = n.status();
            CheckPairing( );
        }
    };
    AsyncCallback< PairingNotify > pairingCb( pairingNotify, m_ProductTask );
    m_RCSClient->setupPairingNotify( pairingCb );


    AsyncCallback< bool > sourceSelectAllowedCb(
        [ this ]( bool allowed )
    {
        BOSE_INFO( s_logger, "%s %s", __PRETTY_FUNCTION__, allowed ? "true" : "false" );
        m_sourceSelectAllowed = allowed;
        UpdateBacklight();
    }, m_ProductTask );
    m_ProductController.RegisterAllowSourceSelectListener( sourceSelectAllowedCb );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief ProductBLERemoteManager::Stop
///
/// @param  This method does not take any arguments.
///
/// @return This method does not return anything.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductBLERemoteManager::Stop( )
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductBLERemoteManager::UpdateAvailableSources
///
/// @brief  This method updates the list of available sources
///
/// @param  list of available sources
///
/// @return This method does not return anything.
///
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductBLERemoteManager::UpdateAvailableSources( const SoundTouchInterface::Sources& sources )
{
    BOSE_INFO( s_logger, "%s update sources %s", __func__, ProtoToMarkup::ToJson( sources ).c_str() );
    m_sources = sources;
    UpdateBacklight();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief ProductBLERemoteManager::UpdateNowSelection
///
/// @param  nowSelection - currently-selected
///
/// @return This method does not return anything.
///
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductBLERemoteManager::UpdateNowSelection( const SoundTouchInterface::NowSelectionInfo& nowSelection )
{
    BOSE_INFO( s_logger, "%s update nowSelection %s", __func__, ProtoToMarkup::ToJson( nowSelection ).c_str() );
    m_nowSelection = nowSelection;
    UpdateBacklight();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief ProductBLERemoteManager::InitLedsMsg
///
/// @param  LedsRawMsg_t Raw LED message to initialize
///
/// @return This method does not return anything.
///
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductBLERemoteManager::InitLedsMsg( LedsRawMsg_t& leds )
{
    for( auto i = ZONE_FIRST; i <= ZONE_LAST; i++ )
    {
        SetZone( leds, i, LedsRawMsg_t::ZONE_BACKLIGHT_OFF );
    }

    leds.set_sound_touch( LedsRawMsg_t::SOURCE_LED_OFF );
    leds.set_tv( LedsRawMsg_t::SOURCE_LED_OFF );
    leds.set_bluetooth( LedsRawMsg_t::SOURCE_LED_OFF );
    leds.set_game( LedsRawMsg_t::SOURCE_LED_OFF );
    leds.set_clapboard( LedsRawMsg_t::SOURCE_LED_OFF );
    leds.set_set_top_box( LedsRawMsg_t::SOURCE_LED_OFF );

    leds.set_backlight_enable( true );
    leds.set_demo_mode( false );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief ProductBLERemoteManager::SetZone
///
/// @param  leds - Raw LED message to set zone in
///         zone - Number of zone to set
///
/// @return This method does not return anything.
///
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductBLERemoteManager::SetZone( LedsRawMsg_t& leds, int zone, LedsRawMsg_t::eLedZoneBits_t state )
{
    if( ( zone < ZONE_FIRST ) || ( zone > ZONE_LAST ) )
    {
        BOSE_ERROR( s_logger, "%s invalid zone %d specificed", __PRETTY_FUNCTION__, zone );
        return;
    }
    static void ( LedsRawMsg_t::*setZone[] )( LedsRawMsg_t::eLedZoneBits_t ) =
    {
        &LedsRawMsg_t::set_zone_01,
        &LedsRawMsg_t::set_zone_02,
        &LedsRawMsg_t::set_zone_03,
        &LedsRawMsg_t::set_zone_04,
        &LedsRawMsg_t::set_zone_05,
        &LedsRawMsg_t::set_zone_06,
        &LedsRawMsg_t::set_zone_07,
        &LedsRawMsg_t::set_zone_08,
        &LedsRawMsg_t::set_zone_09,
        &LedsRawMsg_t::set_zone_10,
    };
    ( leds.*setZone[zone - ZONE_FIRST] )( state );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name ProductBLERemoteManager::GetKeplerState
///
/// @param state - Kepler state ID
///
/// @return This method returns a reference to a StateEntry object corresponding to the
///         supplied state ID
///
////////////////////////////////////////////////////////////////////////////////////////////////////
const KeplerConfig::StateEntry& ProductBLERemoteManager::GetKeplerState( KeplerConfig::State state ) const
{
    auto matchState = [ state ]( const KeplerConfig::StateEntry & s )
    {
        return ( s.state() == state );
    };
    const auto& states = m_keplerConfig.states( );
    const auto& it = std::find_if( states.begin(), states.end(), matchState );

    if( it == states.end() )
    {
        BOSE_ERROR( s_logger, "%s couldn't find state %d (malformed config file?)", __PRETTY_FUNCTION__, state );
        return m_defaultState;
    }

    return *it;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name ProductBLERemoteManager::DetermineKeplerState
///
/// @brief This function uses various pieces of information about the system state to determine
///        the corresponding Kepler state.
///
/// @param None
///
/// @return This method returns a tuple.
///     * The first value is a reference to the state
///     * The second value indicates whether the source is configured/available
///     * The third value indicates which zones should be lit
///
///
////////////////////////////////////////////////////////////////////////////////////////////////////
const std::tuple<const KeplerConfig::StateEntry&, bool, const KeplerConfig::ZoneConfiguration> ProductBLERemoteManager::DetermineKeplerState( ) const
{
    using namespace ProductSTS;
    using namespace SystemSourcesProperties;

    // Power and zone membership have priority over everything else
    if( !m_poweredOn )
    {
        const auto& state = GetKeplerState( KeplerConfig::STATE_OFF );
        return std::make_tuple( state, true, state.zoneconfig() );
    }

    if( !m_nowSelection.has_contentitem() )
    {
        const auto& state = GetKeplerState( KeplerConfig::STATE_OFF );
        return std::make_tuple( state, true, state.zoneconfig() );
    }

    if( !m_nowSelection.contentitem( ).islocal( ) )
    {
        const auto& state = GetKeplerState( KeplerConfig::STATE_ZONE );
        return std::make_tuple( state, true, state.zoneconfig() );
    }

    auto sourceItem = m_ProductController.GetSourceInfo().FindSource( m_nowSelection.contentitem() );
    if( !sourceItem )
    {
        const auto& state = GetKeplerState( KeplerConfig::STATE_OFF );
        return std::make_tuple( state, true, state.zoneconfig() );
    }

    // sourceaccountname and details may possibly be empty
    // this is okay, we'll still do the right thing (empty details will result in creation
    // of details field, which in turn will have an empty activationkey)
    const auto& sourceName = sourceItem->sourcename();
    const auto& accountName = sourceItem->sourceaccountname();
    const auto& activationKey = sourceItem->details().activationkey();

    auto matchState = [ sourceName, accountName, activationKey ]( const KeplerConfig::StateEntry & s )
    {
        if( s.has_activationkey() )
        {
            return ( ( s.sourcename() == sourceName ) && ( ACTIVATION_KEY__Name( s.activationkey() ) == activationKey ) );
        }
        else
        {
            return ( ( s.sourcename() == sourceName ) && ( s.sourceaccountname() == accountName ) );
        }
    };

    const auto& states = m_keplerConfig.states( );
    const auto& itState = std::find_if( states.begin(), states.end(), matchState );

    if( itState != states.end() )
    {
        bool configured = m_ProductController.GetSourceInfo().IsSourceAvailable( *sourceItem );

        if( !itState->has_activationkey() )
        {
            return std::make_tuple( *itState, configured, itState->zoneconfig() );
        }

        // this is a SLOT source, we need to look up the zone configuration based on device type
        const auto& devType = sourceItem->details().devicetype();
        auto matchDev = [ devType ]( const KeplerConfig::DeviceEntry & d )
        {
            return ( devType == DEVICE_TYPE__Name( d.devicetype() ) );
        };
        const auto& devs = m_keplerConfig.devices( );
        const auto& itDev = std::find_if( devs.begin(), devs.end(), matchDev );
        if( itDev == devs.end() )
        {
            BOSE_ERROR( s_logger, "%s couldn't find dev entry %s (malformed config file?)", __PRETTY_FUNCTION__, devType.c_str() );
            return std::make_tuple( *itState, configured, m_defaultZoneConfig );
        }
        return std::make_tuple( *itState, configured, itDev->zoneconfig() );
    }

    // Everthing else is SoundTouch
    const auto& state = GetKeplerState( KeplerConfig::STATE_SOUNDTOUCH );
    bool configured = ( m_ProductController.GetPassportAccountAssociationStatus() == PassportPB::ASSOCIATED );
    return std::make_tuple( state, configured, state.zoneconfig() );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief ProductBLERemoteManager::GetSourceKeysBacklight
///
/// @param  leds LED message to update with source key backlight status
///
/// @return This method does not return anything.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductBLERemoteManager::GetSourceKeysBacklight( LedsRawMsg_t& leds ) const
{
    using namespace ProductSTS;
    using namespace SystemSourcesProperties;

    // Set source key backlights based on which sources are available

    // TV and Bluetooth sources are always available
    leds.set_tv( LedsRawMsg_t::SOURCE_LED_ON );
    leds.set_bluetooth( LedsRawMsg_t::SOURCE_LED_ON );

    // SoundTouch is based on Passport account status
    leds.set_sound_touch( m_ProductController.GetPassportAccountAssociationStatus() == PassportPB::ASSOCIATED ?
                          LedsRawMsg_t::SOURCE_LED_ON : LedsRawMsg_t::SOURCE_LED_OFF );

    // SLOT_ sources are based on the status field
    // We also verify that the source has a "details" field (a configured SLOT_ source without a details field
    // would indicate a malformed source database)
    for( const auto& source : m_sources.sources( ) )
    {
        // TODO: has_details should really be incorporated in to IsSourceAvailable; this will require ProductControllerCommmon to change
        if( ( not m_ProductController.GetSourceInfo().IsSourceAvailable( source ) ) or ( not source.has_details() ) )
        {
            continue;
        }

        const auto& sourceDetailsActivationKey = source.details().activationkey();
        if( sourceDetailsActivationKey == ACTIVATION_KEY__Name( ACTIVATION_KEY_GAME ) )
        {
            leds.set_game( LedsRawMsg_t::SOURCE_LED_ON );
        }
        else if( sourceDetailsActivationKey == ACTIVATION_KEY__Name( ACTIVATION_KEY_CBL_SAT ) )
        {
            leds.set_set_top_box( LedsRawMsg_t::SOURCE_LED_ON );
        }
        else if( sourceDetailsActivationKey == ACTIVATION_KEY__Name( ACTIVATION_KEY_BD_DVD ) )
        {
            leds.set_clapboard( LedsRawMsg_t::SOURCE_LED_ON );
        }
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief ProductBLERemoteManager::UpdateBacklight
///
/// @param  This method does not take any arguments.
///
/// @return This method does not return anything.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductBLERemoteManager::UpdateBacklight( )
{
    LedsRawMsg_t    leds;

    // Set initial state of LEDs
    InitLedsMsg( leds );

    // Set source key backlights based on sources that are available
    if( m_sourceSelectAllowed )
    {
        GetSourceKeysBacklight( leds );

        // Determine backlight and active source key
        auto config = DetermineKeplerState( );
        const auto& state = std::get<0>( config );

        // Light the selected source key
        switch( state.sourcekey() )
        {
        case KeplerConfig::KEY_SOUNDTOUCH:
            leds.set_sound_touch( LedsRawMsg_t::SOURCE_LED_ACTIVE );
            break;
        case KeplerConfig::KEY_TV:
            leds.set_tv( LedsRawMsg_t::SOURCE_LED_ACTIVE );
            break;
        case KeplerConfig::KEY_BLUETOOTH:
            leds.set_bluetooth( LedsRawMsg_t::SOURCE_LED_ACTIVE );
            break;
        case KeplerConfig::KEY_GAME:
            leds.set_game( LedsRawMsg_t::SOURCE_LED_ACTIVE );
            break;
        case KeplerConfig::KEY_BD_DVD:
            leds.set_clapboard( LedsRawMsg_t::SOURCE_LED_ACTIVE );
            break;
        case KeplerConfig::KEY_CBL_SAT:
            leds.set_set_top_box( LedsRawMsg_t::SOURCE_LED_ACTIVE );
            break;
        default:
            break;
        }

        // Apply zone backlighting
        auto configured = std::get<1>( config );
        const auto& zoneBL = std::get<2>( config );
        const auto& zones = configured ? zoneBL.zonesconfigured() : zoneBL.zonesunconfigured();
        for( const auto& z : zones )
        {
            SetZone( leds, z, LedsRawMsg_t::ZONE_BACKLIGHT_ON );
        }
    }
    else
    {
        leds.set_zone_09( RCS_PB_MSG::LedsRawMsg_t::ZONE_BACKLIGHT_ON );
    }

    m_RCSClient->Led_Set(
        leds.sound_touch(), leds.tv(), leds.bluetooth(), leds.game(), leds.clapboard(), leds.set_top_box(),
        leds.zone_01(), leds.zone_02(), leds.zone_03(), leds.zone_04(), leds.zone_05(),
        leds.zone_06(), leds.zone_07(), leds.zone_08(), leds.zone_09(), leds.zone_10(),
        leds.backlight_enable(), leds.demo_mode()
    );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief ProductBLERemoteManager::Pairing_Start
///
/// @param  timeout. Pairing timeout, given in seconds, defaults to 0 for no timeout.
///
/// @return This method does not return anything.
///
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductBLERemoteManager::Pairing_Start( uint32_t timeout )
{
    Unpairing_Start();
    m_RCSClient->Pairing_Start( {}, timeout );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief ProductBLERemoteManager::Pairing_Cancel
///
/// @param  This method does not take any arguments.
///
/// @return This method does not return anything.
///
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductBLERemoteManager::Pairing_Cancel( )
{
    m_pairingPending = false;
    m_RCSClient->Pairing_Cancel( );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief ProductBLERemoteManager::Unpairing_Start
///
/// @param  This method does not take any arguments.
///
/// @return This method does not return anything.
///
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductBLERemoteManager::Unpairing_Start( )
{
    m_RCSClient->Unpairing_Start( {} );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief ProductBLERemoteManager::Unpairing_Cancel
///
/// @param  This method does not take any arguments.
///
/// @return This method does not return anything.
///
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductBLERemoteManager::Unpairing_Cancel( )
{
    m_RCSClient->Unpairing_Cancel( );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief ProductBLERemoteManager::IsConnected
///
/// @param  This method does not take any arguments.
///
/// @return This method does not return anything.
///
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool ProductBLERemoteManager::IsConnected( )
{
    return m_remoteConnected;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief ProductBLERemoteManager::PossiblyPair
///
/// @param  This method does not take any arguments.
///
/// @return This method does not return anything.
///
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductBLERemoteManager::PossiblyPair( )
{
    m_pairingPending = true;
    CheckPairing();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief ProductBLERemoteManager::CheckPairing
///
/// @param  This method does not take any arguments.
///
/// @return This method does not return anything.
///
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductBLERemoteManager::CheckPairing( )
{
    // Some of these cases might not do anything; don't remove them, they're here for
    // documentation
    switch( m_remoteStatus )
    {
    // Indeterminate states
    case RemoteStatus::PSTATE_INIT:
    case RemoteStatus::PSTATE_UNPAIRING:
    case RemoteStatus::PSTATE_UNKNOWN:
        break;

    // In these states, any pairing pending request isn't necessary (we're already paired)
    // !!! Assumption - if we're OUT_OF_RANGE, we must be paired
    // Apparently SCANNING is the state we're in when we're trying to pair, so it's safe to clear
    // the flag here too (pairing has already been initiated)
    case RemoteStatus::PSTATE_SCANNING:
    case RemoteStatus::PSTATE_BONDED:
    case RemoteStatus::PSTATE_OUT_OF_RANGE:
        m_pairingPending = false;
        break;

    // Initiate pairing if we're unpaired and there's a pending request
    case RemoteStatus::PSTATE_UNPAIRED:
        if( m_pairingPending )
        {
            m_pairingPending = false;
            Pairing_Start( m_PairingTimeout );
        }
        break;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                               End of ProductApp Namespace                                    ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
