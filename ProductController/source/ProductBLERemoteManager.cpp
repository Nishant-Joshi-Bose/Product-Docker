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
            m_remoteConnected = n.has_status() && ( n.status() == RCS_PB_MSG::PairingNotify::PSTATE_BONDED );
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
    std::vector< A4VRemoteCommClientIF::ledSourceType_t > availableSources;

    availableSources.push_back( LedsSourceTypeMsg_t::BLUETOOTH );
    availableSources.push_back( LedsSourceTypeMsg_t::SOUND_TOUCH );
    availableSources.push_back( LedsSourceTypeMsg_t::TV );

    for( auto i = 0; i < m_sources.sources_size(); i++ )
    {
        const auto& source = m_sources.sources( i );

        // @TODO: this also needs to check "visible" once that flag works correctly; PGC-1169

        if( source.sourceaccountname().compare( "SLOT_0" ) == 0 )
        {
            availableSources.push_back( LedsSourceTypeMsg_t::GAME );
        }
        if( source.sourceaccountname().compare( "SLOT_1" ) == 0 )
        {
            availableSources.push_back( LedsSourceTypeMsg_t::DVD );
        }
        if( source.sourceaccountname().compare( "SLOT_2" ) == 0 )
        {
            availableSources.push_back( LedsSourceTypeMsg_t::SET_TOP_BOX );
        }
    }

    A4VRemoteCommunication::A4VRemoteCommClientIF::ledSourceType_t sourceLED;
    if( GetSourceLED( sourceLED ) )
    {
        m_RCSClient->Led_Set( sourceLED, availableSources );
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief ProductBLERemoteManager::GetSourceLED
///
/// @param  void This method does not take any arguments.
///
/// @return This method does not return anything.
///
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool ProductBLERemoteManager::GetSourceLED( A4VRemoteCommunication::A4VRemoteCommClientIF::ledSourceType_t& sourceLED )
{
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

    if( sourceName.compare( "PRODUCT" ) == 0 )
    {
        if( sourceAccountName.compare( "TV" ) == 0 )
        {
            BOSE_INFO( s_logger, "update nowSelection TV" );
            // Check for TV explicitly for now, since I don't know if Madrid will set deviceType for the TV
            sourceLED = LedsSourceTypeMsg_t::TV;
        }
        else if( sourceAccountName.compare( "SETUP" ) == 0 )
        {
            BOSE_INFO( s_logger, "update nowSelection SETUP" );
            sourceLED = LedsSourceTypeMsg_t::NOT_SETUP_COMPLETE;
        }
        else if( ( sourceAccountName.compare( 0, 4, "SLOT" ) == 0 ) and sourceItem->has_details() )
        {
            const auto& sourceDetailsDevicetype = sourceItem->details().devicetype();

            if( sourceDetailsDevicetype.compare( "DEVICE_TYPE_GAME" ) == 0 )
            {
                sourceLED = LedsSourceTypeMsg_t::GAME;
            }
            else if( sourceDetailsDevicetype.compare( "DEVICE_TYPE_CBL_SAT" ) == 0 )
            {
                sourceLED = LedsSourceTypeMsg_t::SET_TOP_BOX;
            }
            else if( sourceDetailsDevicetype.compare( "DEVICE_TYPE_BD_DVD" ) == 0 )
            {
                sourceLED = LedsSourceTypeMsg_t::DVD;
            }
            else if( ( sourceDetailsDevicetype.compare( "DEVICE_TYPE_TV" ) == 0 ) or
                     ( sourceDetailsDevicetype.compare( "DEVICE_TYPE_SMART_TV" ) == 0 ) )
            {
                sourceLED = LedsSourceTypeMsg_t::TV;
            }
            else if( sourceDetailsDevicetype.compare( "DEVICE_TYPE_STREAMING" ) == 0 )
            {
                // per Brian White, GAME is probably the most sensible choice here
                // I'm leaving this as an independent case from GAME above in case we change our minds
                sourceLED = LedsSourceTypeMsg_t::GAME;
            }
            else
            {
                BOSE_ERROR( s_logger, "%s product source with unknown device type %s", __func__, sourceDetailsDevicetype.c_str() );
            }
        }
        else
        {
            BOSE_ERROR( s_logger, "%s product source with missing details/devicetype", __func__ );
        }
    }
    else
    {
        if( sourceName.compare( "BLUETOOTH" ) == 0 )
        {
            BOSE_INFO( s_logger, "update nowSelection BLUETOOTH" );
            sourceLED = LedsSourceTypeMsg_t::BLUETOOTH;
        }
        else if( sourceName.compare( "INVALID_SOURCE" ) != 0 )
        {
            sourceLED = LedsSourceTypeMsg_t::SOUND_TOUCH;
        }
    }

    return true;
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
///                               End of ProductApp Namespace                                    ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
