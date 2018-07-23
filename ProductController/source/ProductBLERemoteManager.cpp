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
#include "Utilities.h"
#include "ProfessorProductController.h"
#include "CustomProductLpmHardwareInterface.h"
#include "ProductBLERemoteManager.h"
#include "SharedProto.pb.h"
#include "ProtoToMarkup.h"
#include "EndPointsDefines.h"
#include "ProductSTS.pb.h"
#include "SystemSourcesProperties.pb.h"
#include "SHELBY_SOURCE.h"

using namespace ProductPb;
using namespace A4V_RemoteCommunicationServiceMessages;
using namespace A4VRemoteCommunication;

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
ProductBLERemoteManager::ProductBLERemoteManager( ProfessorProductController& ProductController ) :
    m_ProductTask( ProductController.GetTask( ) ),
    m_ProductNotify( ProductController.GetMessageHandler( ) ),
    m_ProductController( ProductController ),
    m_statusTimer( APTimer::Create( ProductController.GetTask( ), "BLERemoteManager" ) )
{
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
    m_FrontDoorClient = FrontDoor::FrontDoorClient::Create( "ProductBLERemoteManager" );

    auto handleNowSelection = [ this ]( const SoundTouchInterface::NowSelectionInfo & nowSelection )
    {
        UpdateNowSelection( nowSelection );
    };

    m_FrontDoorClient->RegisterNotification<SoundTouchInterface::NowSelectionInfo>( FRONTDOOR_CONTENT_NOWSELECTIONINFO_API, handleNowSelection );
    m_FrontDoorClient->SendGet<SoundTouchInterface::NowSelectionInfo, FrontDoor::Error>( FRONTDOOR_CONTENT_NOWSELECTIONINFO_API, handleNowSelection, {} );

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
    m_FrontDoorClient->RegisterNotification<SystemPowerPb::SystemPowerControl>( FRONTDOOR_SYSTEM_POWER_CONTROL_API, handleSystemPowerControl );
    m_FrontDoorClient->SendGet<SystemPowerPb::SystemPowerControl, FrontDoor::Error>( FRONTDOOR_SYSTEM_POWER_CONTROL_API, handleSystemPowerControl, {} );

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

    // TODO this is a hack for the fact that RCS doesn't provide a status notification, and
    // callers of IsConnected probably want to know right away (i.e. no callback), so we poll
    // for now; this will be replaced when a status notification is available
    m_statusTimer->SetTimeouts( 1000, 1000 );
    m_statusTimer->Start( [ = ]( )
    {
        auto cb = [ = ]( RCS_PB_MSG::PairingNotify n )
        {
            m_remoteConnected = n.has_status() && ( n.status() == RemoteStatus::PSTATE_BONDED );
            if( n.has_status() )
            {
                m_remoteStatus = n.status();
                CheckPairing( );
            }
        };
        m_RCSClient->Pairing_GetStatus( cb );
    } );

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
/// @brief ProductBLERemoteManager::UpdateBacklight
///
/// @param  void This method does not take any arguments.
///
/// @return This method does not return anything.
///
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductBLERemoteManager::UpdateBacklight( )
{
    using namespace ProductSTS;
    using namespace SystemSourcesProperties;

    RCS_PB_MSG::LedsRawMsg_t leds;

    // BlueTooth and TV are always available for now
    leds.set_tv( RCS_PB_MSG::LedsRawMsg_t::SOURCE_LED_ON );
    leds.set_bluetooth( RCS_PB_MSG::LedsRawMsg_t::SOURCE_LED_ON );

    // default the others to off
    leds.set_game( RCS_PB_MSG::LedsRawMsg_t::SOURCE_LED_OFF );
    leds.set_clapboard( RCS_PB_MSG::LedsRawMsg_t::SOURCE_LED_OFF );
    leds.set_set_top_box( RCS_PB_MSG::LedsRawMsg_t::SOURCE_LED_OFF );
    leds.set_sound_touch( RCS_PB_MSG::LedsRawMsg_t::SOURCE_LED_OFF );

    // default all zones to off
    leds.set_zone_01( RCS_PB_MSG::LedsRawMsg_t::ZONE_BACKLIGHT_OFF );
    leds.set_zone_02( RCS_PB_MSG::LedsRawMsg_t::ZONE_BACKLIGHT_OFF );
    leds.set_zone_03( RCS_PB_MSG::LedsRawMsg_t::ZONE_BACKLIGHT_OFF );
    leds.set_zone_04( RCS_PB_MSG::LedsRawMsg_t::ZONE_BACKLIGHT_OFF );
    leds.set_zone_05( RCS_PB_MSG::LedsRawMsg_t::ZONE_BACKLIGHT_OFF );
    leds.set_zone_06( RCS_PB_MSG::LedsRawMsg_t::ZONE_BACKLIGHT_OFF );
    leds.set_zone_07( RCS_PB_MSG::LedsRawMsg_t::ZONE_BACKLIGHT_OFF );
    leds.set_zone_08( RCS_PB_MSG::LedsRawMsg_t::ZONE_BACKLIGHT_OFF );
    leds.set_zone_09( RCS_PB_MSG::LedsRawMsg_t::ZONE_BACKLIGHT_OFF );
    leds.set_zone_10( RCS_PB_MSG::LedsRawMsg_t::ZONE_BACKLIGHT_OFF );

    leds.set_backlight_enable( true );
    leds.set_demo_mode( false );

    if( m_ProductController.GetPassportAccountAssociationStatus() == PassportPB::ASSOCIATED )
    {
        leds.set_sound_touch( RCS_PB_MSG::LedsRawMsg_t::SOURCE_LED_ON );
    }

    for( auto i = 0; i < m_sources.sources_size(); i++ )
    {
        const auto& source = m_sources.sources( i );

        if( ( not source.visible() ) or ( not source.has_details() ) )
        {
            // source isn't configured, don't light it
            continue;
        }

        const auto& sourceDetailsActivationKey = source.details().activationkey();
        if( sourceDetailsActivationKey.compare( ACTIVATION_KEY__Name( ACTIVATION_KEY_GAME ) ) == 0 )
        {
            leds.set_game( RCS_PB_MSG::LedsRawMsg_t::SOURCE_LED_ON );
        }
        else if( sourceDetailsActivationKey.compare( ACTIVATION_KEY__Name( ACTIVATION_KEY_CBL_SAT ) ) == 0 )
        {
            leds.set_set_top_box( RCS_PB_MSG::LedsRawMsg_t::SOURCE_LED_ON );
        }
        else if( sourceDetailsActivationKey.compare( ACTIVATION_KEY__Name( ACTIVATION_KEY_BD_DVD ) ) == 0 )
        {
            leds.set_clapboard( RCS_PB_MSG::LedsRawMsg_t::SOURCE_LED_ON );
        }
    }

    if( !m_poweredOn )
    {
        leds.set_zone_09( RCS_PB_MSG::LedsRawMsg_t::ZONE_BACKLIGHT_ON );
        leds.set_zone_10( RCS_PB_MSG::LedsRawMsg_t::ZONE_BACKLIGHT_ON );
        m_RCSClient->Led_Set(
            leds.sound_touch(), leds.tv(), leds.bluetooth(), leds.game(), leds.clapboard(), leds.set_top_box(),
            leds.zone_01(), leds.zone_02(), leds.zone_03(), leds.zone_04(), leds.zone_05(),
            leds.zone_06(), leds.zone_07(), leds.zone_08(), leds.zone_09(), leds.zone_10(),
            leds.backlight_enable(), leds.demo_mode()
        );
        return;
    }

    // set the active source and associated zones
    A4VRemoteCommunication::A4VRemoteCommClientIF::ledSourceType_t sourceLED;
    bool visible;
    bool valid = GetSourceLED( sourceLED, visible );
    if( valid )
    {
        // zone selection here is from section 6.5.4 ("Zone Assignments per Device")
        // "Bose Kepler (Ginger-Cheevers) BLE Remote Product Specification" (v1.3)
        switch( sourceLED )
        {
        case LedsSourceTypeMsg_t::SOUND_TOUCH:
            leds.set_sound_touch( RCS_PB_MSG::LedsRawMsg_t::SOURCE_LED_ACTIVE );
            leds.set_zone_04( RCS_PB_MSG::LedsRawMsg_t::ZONE_BACKLIGHT_ON );
            leds.set_zone_05( RCS_PB_MSG::LedsRawMsg_t::ZONE_BACKLIGHT_ON );
            leds.set_zone_07( RCS_PB_MSG::LedsRawMsg_t::ZONE_BACKLIGHT_ON );
            leds.set_zone_09( RCS_PB_MSG::LedsRawMsg_t::ZONE_BACKLIGHT_ON );
            leds.set_zone_10( RCS_PB_MSG::LedsRawMsg_t::ZONE_BACKLIGHT_ON );
            break;
        case LedsSourceTypeMsg_t::TV:
            leds.set_tv( RCS_PB_MSG::LedsRawMsg_t::SOURCE_LED_ACTIVE );
            leds.set_zone_07( RCS_PB_MSG::LedsRawMsg_t::ZONE_BACKLIGHT_ON );
            leds.set_zone_09( RCS_PB_MSG::LedsRawMsg_t::ZONE_BACKLIGHT_ON );
            if( visible )
            {
                leds.set_zone_01( RCS_PB_MSG::LedsRawMsg_t::ZONE_BACKLIGHT_ON );
                leds.set_zone_02( RCS_PB_MSG::LedsRawMsg_t::ZONE_BACKLIGHT_ON );
                leds.set_zone_04( RCS_PB_MSG::LedsRawMsg_t::ZONE_BACKLIGHT_ON );
                leds.set_zone_05( RCS_PB_MSG::LedsRawMsg_t::ZONE_BACKLIGHT_ON );
                leds.set_zone_06( RCS_PB_MSG::LedsRawMsg_t::ZONE_BACKLIGHT_ON );
                leds.set_zone_08( RCS_PB_MSG::LedsRawMsg_t::ZONE_BACKLIGHT_ON );
                leds.set_zone_10( RCS_PB_MSG::LedsRawMsg_t::ZONE_BACKLIGHT_ON );
            }
            break;
        case LedsSourceTypeMsg_t::BLUETOOTH:
            leds.set_bluetooth( RCS_PB_MSG::LedsRawMsg_t::SOURCE_LED_ACTIVE );
            leds.set_zone_04( RCS_PB_MSG::LedsRawMsg_t::ZONE_BACKLIGHT_ON );
            leds.set_zone_07( RCS_PB_MSG::LedsRawMsg_t::ZONE_BACKLIGHT_ON );
            leds.set_zone_09( RCS_PB_MSG::LedsRawMsg_t::ZONE_BACKLIGHT_ON );
            leds.set_zone_10( RCS_PB_MSG::LedsRawMsg_t::ZONE_BACKLIGHT_ON );
            break;
        case LedsSourceTypeMsg_t::GAME:
            leds.set_game( RCS_PB_MSG::LedsRawMsg_t::SOURCE_LED_ACTIVE );
            GetZoneLEDs( leds );
            break;
        case LedsSourceTypeMsg_t::DVD:
            leds.set_clapboard( RCS_PB_MSG::LedsRawMsg_t::SOURCE_LED_ACTIVE );
            GetZoneLEDs( leds );
            break;
        case LedsSourceTypeMsg_t::SET_TOP_BOX:
            leds.set_set_top_box( RCS_PB_MSG::LedsRawMsg_t::SOURCE_LED_ACTIVE );
            GetZoneLEDs( leds );
            break;
        case LedsSourceTypeMsg_t::NOT_SETUP_COMPLETE:
            leds.set_zone_04( RCS_PB_MSG::LedsRawMsg_t::ZONE_BACKLIGHT_ON );
            leds.set_zone_07( RCS_PB_MSG::LedsRawMsg_t::ZONE_BACKLIGHT_ON );
            leds.set_zone_09( RCS_PB_MSG::LedsRawMsg_t::ZONE_BACKLIGHT_ON );
            break;
        }
        m_RCSClient->Led_Set(
            leds.sound_touch(), leds.tv(), leds.bluetooth(), leds.game(), leds.clapboard(), leds.set_top_box(),
            leds.zone_01(), leds.zone_02(), leds.zone_03(), leds.zone_04(), leds.zone_05(),
            leds.zone_06(), leds.zone_07(), leds.zone_08(), leds.zone_09(), leds.zone_10(),
            leds.backlight_enable(), leds.demo_mode()
        );
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief ProductBLERemoteManager::GetSourceLED
///
/// @param  sourceLED - reference to sourceLED to illuminate
///         visible - reference to flag indicating whether source is configured
///
/// @return This method does not return anything.
///
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool ProductBLERemoteManager::GetSourceLED(
    A4VRemoteCommunication::A4VRemoteCommClientIF::ledSourceType_t& sourceLED, bool& visible )
{
    using namespace ProductSTS;
    using namespace SystemSourcesProperties;

    visible = false;

    if( m_inSetup )
    {
        sourceLED = LedsSourceTypeMsg_t::NOT_SETUP_COMPLETE;
        return true;
    }

    if( !m_nowSelection.has_contentitem() )
    {
        return false;
    }

    auto sourceItem = m_ProductController.GetSourceInfo().FindSource( m_nowSelection.contentitem() );
    if( !sourceItem )
    {
        return false;
    }

    const auto& sourceName = sourceItem->sourcename();
    const auto& sourceAccountName = sourceItem->sourceaccountname();

    visible = sourceItem->visible();
    if( sourceName.compare( SHELBY_SOURCE::PRODUCT ) == 0 )
    {
        if( sourceAccountName.compare( ProductSourceSlot_Name( TV ) ) == 0 )
        {
            BOSE_INFO( s_logger, "update nowSelection TV" );
            // Check for TV explicitly for now, since I don't know if Madrid will set deviceType for the TV
            sourceLED = LedsSourceTypeMsg_t::TV;
        }
        else if( ( sourceAccountName.compare( 0, 4, ProductSourceSlot_Name( SLOT_0 ), 0, 4 ) == 0 ) and sourceItem->has_details() )
        {
            const auto& sourceDetailsActivationKey = sourceItem->details().activationkey();
            BOSE_INFO( s_logger, "(%s) update nowSelection with activationKey %s", __func__, sourceDetailsActivationKey.c_str() );

            if( sourceDetailsActivationKey.compare( ACTIVATION_KEY__Name( ACTIVATION_KEY_GAME ) ) == 0 )
            {
                sourceLED = LedsSourceTypeMsg_t::GAME;
            }
            else if( sourceDetailsActivationKey.compare( ACTIVATION_KEY__Name( ACTIVATION_KEY_CBL_SAT ) ) == 0 )
            {
                sourceLED = LedsSourceTypeMsg_t::SET_TOP_BOX;
            }
            else if( sourceDetailsActivationKey.compare( ACTIVATION_KEY__Name( ACTIVATION_KEY_BD_DVD ) ) == 0 )
            {
                sourceLED = LedsSourceTypeMsg_t::DVD;
            }
            else
            {
                BOSE_ERROR( s_logger, "%s product source with unknown activation key %s", __func__, sourceDetailsActivationKey.c_str() );
            }
        }
        else
        {
            BOSE_ERROR( s_logger, "%s PRODUCT source with missing details/devicetype", __func__ );
        }
    }
    else if( sourceName.compare( SHELBY_SOURCE::SETUP ) == 0 )
    {
        if( sourceAccountName.compare( SetupSourceSlot_Name( SETUP ) ) == 0 )
        {
            BOSE_INFO( s_logger, "update nowSelection SETUP" );
            sourceLED = LedsSourceTypeMsg_t::NOT_SETUP_COMPLETE;
        }
        else if( sourceAccountName.compare( SetupSourceSlot_Name( PAIRING ) ) == 0 )
        {
            BOSE_INFO( s_logger, "update nowSelection PAIRING No LED Available" );
        }
        else
        {
            BOSE_ERROR( s_logger, "%s SETUP source with missing details/devicetype", __func__ );
        }
    }
    else if( sourceName.compare( SHELBY_SOURCE::BLUETOOTH ) == 0 )
    {
        BOSE_INFO( s_logger, "update nowSelection BLUETOOTH" );
        sourceLED = LedsSourceTypeMsg_t::BLUETOOTH;
    }
    else if( sourceName.compare( SHELBY_SOURCE::INVALID_SOURCE ) != 0 )
    {
        sourceLED = LedsSourceTypeMsg_t::SOUND_TOUCH;
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief ProductBLERemoteManager::GetZoneLEDs
///
/// @param  leds - reference to LedsRawMsg_t
///
/// @return This method does not return anything.
///
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductBLERemoteManager::GetZoneLEDs( RCS_PB_MSG::LedsRawMsg_t& leds )
{
    using namespace ProductSTS;
    using namespace SystemSourcesProperties;

    if( !m_nowSelection.has_contentitem() )
    {
        return;
    }

    auto sourceItem = m_ProductController.GetSourceInfo().FindSource( m_nowSelection.contentitem() );
    if( !sourceItem )
    {
        return;
    }

    const auto& sourceName = sourceItem->sourcename();

    if( sourceName.compare( SHELBY_SOURCE::PRODUCT ) != 0 )
    {
        return;
    }

    if( not sourceItem->has_details() )
    {
        return;
    }

    const auto& sourceDetailsDeviceType = sourceItem->details().devicetype();
    if(
        ( sourceDetailsDeviceType.compare( DEVICE_TYPE__Name( DEVICE_TYPE_GAME ) ) == 0 ) or
        ( sourceDetailsDeviceType.compare( DEVICE_TYPE__Name( DEVICE_TYPE_STREAMING ) ) == 0 ) )
    {
        leds.set_zone_02( RCS_PB_MSG::LedsRawMsg_t::ZONE_BACKLIGHT_ON );
        leds.set_zone_04( RCS_PB_MSG::LedsRawMsg_t::ZONE_BACKLIGHT_ON );
        leds.set_zone_07( RCS_PB_MSG::LedsRawMsg_t::ZONE_BACKLIGHT_ON );
        leds.set_zone_09( RCS_PB_MSG::LedsRawMsg_t::ZONE_BACKLIGHT_ON );
        leds.set_zone_10( RCS_PB_MSG::LedsRawMsg_t::ZONE_BACKLIGHT_ON );
    }
    else if( sourceDetailsDeviceType.compare( DEVICE_TYPE__Name( DEVICE_TYPE_BD_DVD ) ) == 0 )
    {
        leds.set_zone_02( RCS_PB_MSG::LedsRawMsg_t::ZONE_BACKLIGHT_ON );
        leds.set_zone_04( RCS_PB_MSG::LedsRawMsg_t::ZONE_BACKLIGHT_ON );
        leds.set_zone_07( RCS_PB_MSG::LedsRawMsg_t::ZONE_BACKLIGHT_ON );
        leds.set_zone_09( RCS_PB_MSG::LedsRawMsg_t::ZONE_BACKLIGHT_ON );
        leds.set_zone_10( RCS_PB_MSG::LedsRawMsg_t::ZONE_BACKLIGHT_ON );
    }
    else if( sourceDetailsDeviceType.compare( DEVICE_TYPE__Name( DEVICE_TYPE_CBL_SAT ) ) == 0 )
    {
        leds.set_zone_01( RCS_PB_MSG::LedsRawMsg_t::ZONE_BACKLIGHT_ON );
        leds.set_zone_02( RCS_PB_MSG::LedsRawMsg_t::ZONE_BACKLIGHT_ON );
        leds.set_zone_03( RCS_PB_MSG::LedsRawMsg_t::ZONE_BACKLIGHT_ON );
        leds.set_zone_04( RCS_PB_MSG::LedsRawMsg_t::ZONE_BACKLIGHT_ON );
        leds.set_zone_05( RCS_PB_MSG::LedsRawMsg_t::ZONE_BACKLIGHT_ON );
        leds.set_zone_06( RCS_PB_MSG::LedsRawMsg_t::ZONE_BACKLIGHT_ON );
        leds.set_zone_07( RCS_PB_MSG::LedsRawMsg_t::ZONE_BACKLIGHT_ON );
        leds.set_zone_08( RCS_PB_MSG::LedsRawMsg_t::ZONE_BACKLIGHT_ON );
        leds.set_zone_09( RCS_PB_MSG::LedsRawMsg_t::ZONE_BACKLIGHT_ON );
        leds.set_zone_10( RCS_PB_MSG::LedsRawMsg_t::ZONE_BACKLIGHT_ON );
    }
    else
    {
        BOSE_ERROR( s_logger, "%s unhandled device type %s", __func__, sourceDetailsDeviceType.c_str() );
    }
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
