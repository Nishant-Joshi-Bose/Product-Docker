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
#include "EndPointsError.pb.h"
#include "ProtoToMarkup.h"
#include "ProductSourceInfo.h"
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
    m_FrontDoorClient->SendGet<SoundTouchInterface::NowSelectionInfo, EndPointsError::Error>( FRONTDOOR_CONTENT_NOWSELECTIONINFO_API, handleNowSelection, {} );
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
/// @brief ProductBLERemoteManager::UpdateNowSelection
///
/// @param  void This method does not take any arguments.
///
/// @return This method does not return anything.
///
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductBLERemoteManager::UpdateNowSelection( const SoundTouchInterface::NowSelectionInfo& nowSelection )
{
    m_nowSelection = nowSelection;
    BOSE_INFO( s_logger, "%s update nowSelection %s", __func__, ProtoToMarkup::ToJson( nowSelection ).c_str() );

    if( !nowSelection.has_contentitem() )
    {
        return;
    }

    auto ci = nowSelection.contentitem();
    auto source = m_ProductController.GetSourceInfo()->FindSource( ci );

    if( not source )
    {
        return;
    }

    /// TODO the following rat's nest of code will need some more work, but the basic idea is right
    if( source->sourcename().compare( "PRODUCT" ) == 0 )
    {
        if( source->sourceaccountname().compare( "TV" ) == 0 )
        {
            // Check for TV explicitly for now, since I don't know if Mardid will set deviceType for the TV
            m_RCSClient->Led_Set( LedsSourceTypeMsg_t::TV );
        }
        else if( ( source->sourceaccountname().compare( 0, 4, "SLOT" ) == 0 ) and source->has_details() and source->details().has_devicetype() )
        {
            if( source->details().devicetype().compare( "DEVICE_TYPE_GAME" ) )
            {
                m_RCSClient->Led_Set( LedsSourceTypeMsg_t::GAME );
            }
            else if( source->details().devicetype().compare( "DEVICE_TYPE_CBL_SAT" ) )
            {
                m_RCSClient->Led_Set( LedsSourceTypeMsg_t::SET_TOP_BOX );
            }
            else if( source->details().devicetype().compare( "DEVICE_TYPE_BD_DVD" ) )
            {
                m_RCSClient->Led_Set( LedsSourceTypeMsg_t::DVD );
            }
            else if( source->details().devicetype().compare( "DEVICE_TYPE_TV" ) or
                     source->details().devicetype().compare( "DEVICE_TYPE_SMART_TV" ) )
            {
                m_RCSClient->Led_Set( LedsSourceTypeMsg_t::TV );
            }
            else if( source->details().devicetype().compare( "DEVICE_TYPE_STREAMING" ) )
            {
                // TODO WHAT SHOULD THIS BE? (WHAT IS "DEVICE_TYPE_STREAMING")?
                m_RCSClient->Led_Set( LedsSourceTypeMsg_t::SET_TOP_BOX );
            }
            else
            {
                BOSE_ERROR( s_logger, "%s product source with unknown device type %s", __func__, source->details().devicetype().c_str() );
            }
        }
        else
        {
            BOSE_ERROR( s_logger, "%s product source with missing details/devicetype", __func__ );
        }
    }
    else
    {
        if( source->sourcename().compare( "BLUETOOTH" ) == 0 )
        {
            m_RCSClient->Led_Set( LedsSourceTypeMsg_t::BLUETOOTH );
        }
        else
        {
            m_RCSClient->Led_Set( LedsSourceTypeMsg_t::SOUND_TOUCH );
        }
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
