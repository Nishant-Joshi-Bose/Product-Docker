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

////////////////////////////////////////////////////////////////////////////////////////////////////
///                             Start of Product Namespace                                       ///
////////////////////////////////////////////////////////////////////////////////////////////////////
namespace ProductApp
{

const char* ProductBLERemoteManager::m_configFile = "/opt/Bose/etc/KeplerConfig.json";

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
    m_ProductController( ProductController ),
    m_statusTimer( APTimer::Create( ProductController.GetTask( ), "BLERemoteManager" ) )
{
    BOptional<std::string> config = SystemUtils::ReadFile( m_configFile );
    if( config )
    {
        try
        {
            ProtoToMarkup::FromJson( *config, &m_keplerConfig );
        }
        catch( const ProtoToMarkup::MarkupError & e )
        {
            BOSE_ERROR( s_logger, "KeplerConfig markup error - %s", e.what( ) );
        }
    }
    else
    {
        BOSE_ERROR( s_logger, "%s failed to lod config %s", __PRETTY_FUNCTION__, m_configFile );
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductBLERemoteManager::InitializeFrontDoor
///
/// @brief
///
/// @param  void This method does not take any arguments.
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
    {
        //audio zone callback for notifications
        AsyncCallback< SoundTouchInterface::zone >
        nowPlayingCb( std::bind( &ProductBLERemoteManager::UpdateCapsAudioZone,
                                 this,
                                 std::placeholders::_1 ),
                      m_ProductTask );

        //audio zone notification registration
        frontDoorClient->RegisterNotification< SoundTouchInterface::zone >(
            FRONTDOOR_AUDIO_ZONE_API, nowPlayingCb );
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductBLERemoteManager::InitializeRCS
///
/// @brief
///
/// @param  void This method does not take any arguments.
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
/// @param  void This method does not take any arguments.
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

    m_statusTimer->SetTimeouts( 1000, 1000 );
    m_statusTimer->Start( [ = ]( )
    {
        AsyncCallback<PairingNotify> cb(
            [ = ]( PairingNotify n )
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
        }, m_ProductTask );
        m_RCSClient->Pairing_GetStatus( cb );
    } );

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
/// @param  void This method does not take any arguments.
///
/// @return This method does not return anything.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductBLERemoteManager::Stop( void )
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
/// @brief ProductBLERemoteManager::UpdateCapsAudioZone
///
/// @param  SoundTouchInterface::zone zoneInfo
///
/// @return This method does not return anything.
///
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductBLERemoteManager::UpdateCapsAudioZone( const SoundTouchInterface::zone& zoneInfo )
{
    BOSE_INFO( s_logger, "%s, zone- (%s)", __func__,  ProtoToMarkup::ToJson( zoneInfo, false ).c_str() );

    bool zoneMember = false;
    for( const auto& m : zoneInfo.members( ) )
    {
        if( m.guid( ) == m_ProductController.GetProductGuid( ) )
        {
            zoneMember = true;
            break;
        }
    }
    if( zoneMember != m_IsZoneMember )
    {
        BOSE_INFO( s_logger, "%s now %sa zone member", __func__,  zoneMember ? "" : "not " );
        m_IsZoneMember = zoneMember;
        UpdateBacklight();
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief ProductBLERemoteManager::InitLedsMsg
///
/// @param  leds Raw LED message to initialize
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
    switch( zone )
    {
    case 1:
        leds.set_zone_01( state );
        break;
    case 2:
        leds.set_zone_02( state );
        break;
    case 3:
        leds.set_zone_03( state );
        break;
    case 4:
        leds.set_zone_04( state );
        break;
    case 5:
        leds.set_zone_05( state );
        break;
    case 6:
        leds.set_zone_06( state );
        break;
    case 7:
        leds.set_zone_07( state );
        break;
    case 8:
        leds.set_zone_08( state );
        break;
    case 9:
        leds.set_zone_09( state );
        break;
    case 10:
        leds.set_zone_10( state );
        break;
    default:
        BOSE_ERROR( s_logger, "Unsupported zone %d in backlight config", zone );
        break;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name ProductBLERemoteManager::DetermineKeplerConfig
///
/// @brief This function determines (mostly) the illumination state of the Kepler remote using
///        various pieces of information about the rest of the system.
///
/// @param None
///
/// @return This method returns a tuple.
///     * The first value indicates which zone backlight configuration to apply
///     * The second value indicates which source key to illuminate
///     * The third value indicates if the currently-selected source is configured
///
///
////////////////////////////////////////////////////////////////////////////////////////////////////
std::tuple<KeplerConfig::Source, A4VRemoteCommClientIF::ledSourceType_t, bool> ProductBLERemoteManager::DetermineKeplerState( )
{
    using namespace ProductSTS;
    using namespace SystemSourcesProperties;

    // Power and zone membership have priority over everything else
    if( !m_poweredOn )
    {
        return std::make_tuple( KeplerConfig::OFF, LedsSourceTypeMsg_t::NOT_SETUP_COMPLETE, true );
    }
    else if( m_IsZoneMember )
    {
        return std::make_tuple( KeplerConfig::ZONE, LedsSourceTypeMsg_t::SOUND_TOUCH, true );
    }

    // For the rest we determine the source by what's currently playing
    if( !m_nowSelection.has_contentitem() )
    {
        return std::make_tuple( KeplerConfig::OFF, LedsSourceTypeMsg_t::NOT_SETUP_COMPLETE, true );
    }

    auto sourceItem = m_ProductController.GetSourceInfo().FindSource( m_nowSelection.contentitem() );
    if( !sourceItem )
    {
        return std::make_tuple( KeplerConfig::OFF, LedsSourceTypeMsg_t::NOT_SETUP_COMPLETE, true );
    }

    const auto& sourceName = sourceItem->sourcename();
    const auto& sourceAccountName = sourceItem->sourceaccountname();

    // No use going any farther if we have invalid source here
    if( sourceName.compare( SHELBY_SOURCE::INVALID_SOURCE ) == 0 )
    {
        return std::make_tuple( KeplerConfig::OFF, LedsSourceTypeMsg_t::NOT_SETUP_COMPLETE, true );
    }

    // Product sources can be setup ...
    if( sourceName.compare( SHELBY_SOURCE::SETUP ) == 0 )
    {
        if( sourceAccountName.compare( SetupSourceSlot_Name( ADAPTIQ ) ) == 0 )
        {
            return std::make_tuple( KeplerConfig::ADAPTIQ, LedsSourceTypeMsg_t::NOT_SETUP_COMPLETE, true );
        }
        else if( sourceAccountName.compare( SetupSourceSlot_Name( PAIRING ) ) == 0 )
        {
            return std::make_tuple( KeplerConfig::ACCESSORY_PAIRING, LedsSourceTypeMsg_t::NOT_SETUP_COMPLETE, true );
        }
        return std::make_tuple( KeplerConfig::OOB, LedsSourceTypeMsg_t::NOT_SETUP_COMPLETE, true );
    }

    if( sourceName.compare( SHELBY_SOURCE::PRODUCT ) == 0 )
    {
        auto configured = m_ProductController.GetSourceInfo().IsSourceAvailable( *sourceItem );

        // .... TV, or SLOT_n
        if( sourceAccountName.compare( ProductSourceSlot_Name( TV ) ) == 0 )
        {
            return std::make_tuple( KeplerConfig::TV, LedsSourceTypeMsg_t::TV, configured );
        }

        auto keplerSource   = KeplerConfig::OFF;
        auto ledsSource     = LedsSourceTypeMsg_t::NOT_SETUP_COMPLETE;

        // For slot sources, illuminated source key is independent of backlight configuration
        // (backlight configuration is determined by the type of device a source key is configured for)
        const auto& sourceDetailsActivationKey = sourceItem->details().activationkey();
        if( sourceDetailsActivationKey.compare( ACTIVATION_KEY__Name( ACTIVATION_KEY_GAME ) ) == 0 )
        {
            ledsSource = LedsSourceTypeMsg_t::GAME;
        }
        else if( sourceDetailsActivationKey.compare( ACTIVATION_KEY__Name( ACTIVATION_KEY_CBL_SAT ) ) == 0 )
        {
            ledsSource = LedsSourceTypeMsg_t::SET_TOP_BOX;
        }
        else if( sourceDetailsActivationKey.compare( ACTIVATION_KEY__Name( ACTIVATION_KEY_BD_DVD ) ) == 0 )
        {
            ledsSource = LedsSourceTypeMsg_t::DVD;
        }
        else
        {
            BOSE_ERROR( s_logger, "%s unhandled activation key %s", __func__, sourceDetailsActivationKey.c_str() );
        }

        const auto& sourceDetailsDeviceType = sourceItem->details().devicetype();
        if(
            ( sourceDetailsDeviceType.compare( DEVICE_TYPE__Name( DEVICE_TYPE_GAME ) ) == 0 ) or
            ( sourceDetailsDeviceType.compare( DEVICE_TYPE__Name( DEVICE_TYPE_STREAMING ) ) == 0 ) )
        {
            keplerSource = KeplerConfig::GAME;
        }
        else if( sourceDetailsDeviceType.compare( DEVICE_TYPE__Name( DEVICE_TYPE_BD_DVD ) ) == 0 )
        {
            keplerSource = KeplerConfig::BD_DVD;
        }
        else if( sourceDetailsDeviceType.compare( DEVICE_TYPE__Name( DEVICE_TYPE_CBL_SAT ) ) == 0 )
        {
            keplerSource = KeplerConfig::CBL_SAT;
        }
        else
        {
            BOSE_ERROR( s_logger, "%s unhandled device type %s", __func__, sourceDetailsDeviceType.c_str() );
        }

        return std::make_tuple( keplerSource, ledsSource, configured );
    }
    else if( sourceName.compare( SHELBY_SOURCE::BLUETOOTH ) == 0 )
    {
        return std::make_tuple( KeplerConfig::BLUETOOTH, LedsSourceTypeMsg_t::BLUETOOTH, true );
    }

    // Everthing else is SoundTouch
    return std::make_tuple( KeplerConfig::SOUNDTOUCH, LedsSourceTypeMsg_t::SOUND_TOUCH,
                            m_ProductController.GetPassportAccountAssociationStatus() == PassportPB::ASSOCIATED );
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
void ProductBLERemoteManager::GetSourceKeysBacklight( LedsRawMsg_t& leds )
{
    using namespace ProductSTS;
    using namespace SystemSourcesProperties;

    // Set source key backlights based on sources are available

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
        if( ( not m_ProductController.GetSourceInfo().IsSourceAvailable( source ) ) or ( not source.has_details() ) )
        {
            continue;
        }

        const auto& sourceDetailsActivationKey = source.details().activationkey();
        if( sourceDetailsActivationKey.compare( ACTIVATION_KEY__Name( ACTIVATION_KEY_GAME ) ) == 0 )
        {
            leds.set_game( LedsRawMsg_t::SOURCE_LED_ON );
        }
        else if( sourceDetailsActivationKey.compare( ACTIVATION_KEY__Name( ACTIVATION_KEY_CBL_SAT ) ) == 0 )
        {
            leds.set_set_top_box( LedsRawMsg_t::SOURCE_LED_ON );
        }
        else if( sourceDetailsActivationKey.compare( ACTIVATION_KEY__Name( ACTIVATION_KEY_BD_DVD ) ) == 0 )
        {
            leds.set_clapboard( LedsRawMsg_t::SOURCE_LED_ON );
        }
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief ProductBLERemoteManager::UpdateBacklight
///
/// @param  void This method does not take any arguments.
///
/// @return This method does not return anything.
///
///
/// TODO: This function is probably long-overdue for refactoring
///
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
    }

    // Determine backlight and active source key
    auto config = DetermineKeplerState( );

    // Light the selected source key
    switch( std::get<1>( config ) )
    {
    case LedsSourceTypeMsg_t::SOUND_TOUCH:
        leds.set_sound_touch( LedsRawMsg_t::SOURCE_LED_ACTIVE );
        break;
    case LedsSourceTypeMsg_t::TV:
        leds.set_tv( LedsRawMsg_t::SOURCE_LED_ACTIVE );
        break;
    case LedsSourceTypeMsg_t::BLUETOOTH:
        leds.set_bluetooth( LedsRawMsg_t::SOURCE_LED_ACTIVE );
        break;
    case LedsSourceTypeMsg_t::GAME:
        leds.set_game( LedsRawMsg_t::SOURCE_LED_ACTIVE );
        break;
    case LedsSourceTypeMsg_t::DVD:
        leds.set_clapboard( LedsRawMsg_t::SOURCE_LED_ACTIVE );
        break;
    case LedsSourceTypeMsg_t::SET_TOP_BOX:
        leds.set_set_top_box( LedsRawMsg_t::SOURCE_LED_ACTIVE );
        break;
    default:
        break;
    }

    // Apply zone backlighting
    auto zoneBL         = std::get<0>( config );
    const auto& blCfg   = m_keplerConfig.backlightconfig( );

    auto cmpBlCfg = [ zoneBL ]( const KeplerConfig::BacklightEntry & bl )
    {
        return ( bl.source() == zoneBL );
    };

    auto itBL = std::find_if( blCfg.begin(), blCfg.end(), cmpBlCfg );
    if( itBL == blCfg.end() )
    {
        // Don't light anything on a broken configuration; hopefully this will make it more obvious that something is broken
        BOSE_ERROR( s_logger, "No config found for backlight source %s",  KeplerConfig_Source_Name( zoneBL ).c_str() );;
        return;
    }

    // first process unconditional zones
    for( const auto& z : itBL->zones() )
    {
        SetZone( leds, z, LedsRawMsg_t::ZONE_BACKLIGHT_ON );
    }

    // now add in zones that are lit only if the source is configured
    if( std::get<2>( config ) )
    {
        for( const auto& z : itBL->zonesconfigured() )
        {
            SetZone( leds, z, LedsRawMsg_t::ZONE_BACKLIGHT_ON );
        }
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
/// @param  void This method does not take any arguments.
///
/// @return This method does not return anything.
///
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductBLERemoteManager::Pairing_Cancel( void )
{
    m_pairingPending = false;
    m_RCSClient->Pairing_Cancel( );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief ProductBLERemoteManager::Unpairing_Start
///
/// @param  void This method does not take any arguments.
///
/// @return This method does not return anything.
///
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductBLERemoteManager::Unpairing_Start( void )
{
    m_RCSClient->Unpairing_Start( {} );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief ProductBLERemoteManager::Unpairing_Cancel
///
/// @param  void This method does not take any arguments.
///
/// @return This method does not return anything.
///
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductBLERemoteManager::Unpairing_Cancel( void )
{
    m_RCSClient->Unpairing_Cancel( );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief ProductBLERemoteManager::IsConnected
///
/// @param  void This method does not take any arguments.
///
/// @return This method does not return anything.
///
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool ProductBLERemoteManager::IsConnected( void )
{
    return m_remoteConnected;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief ProductBLERemoteManager::PossiblyPair
///
/// @param  void This method does not take any arguments.
///
/// @return This method does not return anything.
///
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductBLERemoteManager::PossiblyPair( void )
{
    m_pairingPending = true;
    CheckPairing();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief ProductBLERemoteManager::CheckPairing
///
/// @param  void This method does not take any arguments.
///
/// @return This method does not return anything.
///
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductBLERemoteManager::CheckPairing( void )
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
