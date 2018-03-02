////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file      ProductBLERemoteManager.cpp
///
/// @brief     This file implements audio volume management.
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
#include "DPrint.h"
#include "Utilities.h"
#include "ProfessorProductController.h"
#include "CustomProductLpmHardwareInterface.h"
#include "ProductBLERemoteManager.h"
#include "EndPointsError.pb.h"
#include "ProtoToMarkup.h"
#include "ProductSourceInfo.h"

using namespace ProductPb;
using namespace A4V_RemoteCommunicationServiceMessages;
using namespace A4VRemoteCommunication;

namespace
{
const std::string s_FrontDoorNowSelection = "/content/nowSelection";
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                             Start of Product Namespace                                       ///
////////////////////////////////////////////////////////////////////////////////////////////////////
namespace ProductApp
{

////////////////////////////////////////////////////////////////////////////////////////////////
///
/// The following constants define FrontDoor endpoints used by the SourceInfo
///
////////////////////////////////////////////////////////////////////////////////////////////////

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
    m_ProductController( ProductController )
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

    auto handleNowSelectionFail = [ this ]( const EndPointsError::Error & error )
    {
        BOSE_ERROR( s_logger, "Error %d %d <%s> while retrieving nowSelection",
                    error.code( ), error.subcode( ), error.message( ).c_str( ) );
    };

    auto handleNowSelectionReady = [ = ]( const std::list<std::string>& endPointList )
    {
        if( endPointList.empty( ) )
        {
            BOSE_ERROR( s_logger, "Endpoint not ready" );
            return;
        }

        BOSE_INFO( s_logger, "Registering for %s",  s_FrontDoorNowSelection.c_str() );
        m_FrontDoorClient->SendGet<SoundTouchInterface::NowSelectionInfo, EndPointsError::Error>( s_FrontDoorNowSelection, handleNowSelection, handleNowSelectionFail );
        m_FrontDoorClient->RegisterNotification<SoundTouchInterface::NowSelectionInfo>( s_FrontDoorNowSelection, handleNowSelection );
    };

    auto handleNowSelectionNotReady = [ this ]( const std::list<std::string>& endPointList )
    {
    };

    std::list<std::string> endPointList = { s_FrontDoorNowSelection };
    m_FrontDoorClient->RegisterEndpointsOfInterest( endPointList, handleNowSelectionReady, handleNowSelectionNotReady );
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

    auto disconnectCb = [this]( )
    {
    };

    auto connectCb = [this]( bool status )
    {
    };

    m_RCSClient->RegisterDisconnectCb( disconnectCb );
    m_RCSClient->Connect( connectCb );
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
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief ProductBLERemoteManager::Stop
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductBLERemoteManager::Stop( void )
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief ProductBLERemoteManager::UpdateNowSelection
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

    if( ( not source ) or ( not source->has_sourcename() ) )
    {
        return;
    }

    if( source->sourcename().compare( "PRODUCT" ) == 0 )
    {
        if( not source->has_sourceaccountname() )
        {
            return;
        }
        else if( source->sourceaccountname().compare( "TV" ) == 0 )
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
        // TODO verify that sourcename is "SOUNDTOUCH"
        else if( source->sourcename().compare( "SOUNDTOUCH" ) == 0 )
        {
            m_RCSClient->Led_Set( LedsSourceTypeMsg_t::SOUND_TOUCH );
        }
        else
        {
            BOSE_ERROR( s_logger, "%s non-product source with unknown name %s", __func__, source->sourcename().c_str() );
        }
    }

}


////////////////////////////////////////////////////////////////////////////////////////////////////
///                               End of ProductApp Namespace                                    ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
