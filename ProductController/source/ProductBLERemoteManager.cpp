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

using namespace ProductPb;
using namespace A4V_RemoteCommunicationServiceMessages;
using namespace A4VRemoteCommunication;

namespace
{
const std::string s_FrontDoorNowPlaying = "/content/nowPlaying";
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
    m_ProductNotify( ProductController.GetMessageHandler( ) )
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

    auto handleNowPlaying = [ this ]( const SoundTouchInterface::NowPlaying & nowPlaying )
    {
        UpdateNowPlaying( nowPlaying );
    };

    auto handleNowPlayingFail = [ this ]( const EndPointsError::Error & error )
    {
        BOSE_ERROR( s_logger, "Error %d %d <%s> while retrieving nowPlaying",
                    error.code( ), error.subcode( ), error.message( ).c_str( ) );
    };

    auto handleNowPlayingReady = [ = ]( const std::list<std::string>& endPointList )
    {
        if( endPointList.empty( ) )
        {
            BOSE_ERROR( s_logger, "Endpoint not ready" );
            return;
        }

        BOSE_INFO( s_logger, "Registering for %s",  s_FrontDoorNowPlaying.c_str() );
        m_FrontDoorClient->SendGet<SoundTouchInterface::NowPlaying, EndPointsError::Error>( s_FrontDoorNowPlaying, handleNowPlaying, handleNowPlayingFail );
        m_FrontDoorClient->RegisterNotification<SoundTouchInterface::NowPlaying>( s_FrontDoorNowPlaying, handleNowPlaying );
    };

    auto handleNowPlayingNotReady = [ this ]( const std::list<std::string>& endPointList )
    {
    };

    std::list<std::string> endPointList = { s_FrontDoorNowPlaying };
    m_FrontDoorClient->RegisterEndpointsOfInterest( endPointList, handleNowPlayingReady, handleNowPlayingNotReady );
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
/// @brief ProductBLERemoteManager::UpdateNowPlaying
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductBLERemoteManager::UpdateNowPlaying( const SoundTouchInterface::NowPlaying& nowPlaying )
{
    m_nowPlaying = nowPlaying;

    m_RCSClient->Led_Set( LedsSourceTypeMsg_t::SOUND_TOUCH );
}


////////////////////////////////////////////////////////////////////////////////////////////////////
///                               End of ProductApp Namespace                                    ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
