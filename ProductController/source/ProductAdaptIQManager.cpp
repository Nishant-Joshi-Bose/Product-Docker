////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file      ProductAdaptIQManager.cpp
///
/// @brief     This file implements audio volume management.
///
/// @author    Chris Houston
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
#include "ProductAdaptIQManager.h"

using namespace ProductPb;

namespace
{
const std::string s_ActionEnter         = "enter";
const std::string s_ActionCancel        = "cancel";
const std::string s_ActionAdvance       = "advance";

const std::string s_ModeNormal          = "Enabled Normal";
const std::string s_ModeRetail          = "Enabled Retail";
const std::string s_ModeDisabled        = "Enabled Disabled";
const std::string s_FrontDoorAdaptIQ    = "/adaptiq";
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                             Start of Product Namespace                                       ///
////////////////////////////////////////////////////////////////////////////////////////////////////
namespace ProductApp
{

////////////////////////////////////////////////////////////////////////////////////////////////
///
/// The following constants define FrontDoor endpoints used by the AdaptIQManager
///
////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductAdaptIQManager::ProductAdaptIQManager
///
/// @param ProductController
///
/// @return This method does not return anything.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
ProductAdaptIQManager::ProductAdaptIQManager( ProfessorProductController& ProductController ) :
    m_ProductTask( ProductController.GetTask( ) ),
    m_ProductNotify( ProductController.GetMessageHandler( ) ),
    m_ProductLpmHardwareInterface( ProductController.GetLpmHardwareInterface( ) )
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductAdaptIQManager::Run
///
/// @brief  This method starts the main task for the ProductAdaptIQManager class.
///
/// @param  void This method does not take any arguments.
///
/// @return This method does not return anything.
///
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductAdaptIQManager::Run( )
{
    m_FrontDoorClient = FrontDoor::FrontDoorClient::Create( "ProductAdaptIQManager" );

    auto getFunc = [ this ]( Callback<const AdaptIQStatus> resp )
    {
        AdaptIQStatus status;
        HandleGet( status );
        resp.Send( status );
    };
    AsyncCallback<Callback<AdaptIQStatus>> getCb( getFunc, m_ProductTask );
    m_GetConnection = m_FrontDoorClient->RegisterGet( s_FrontDoorAdaptIQ, getFunc );

    auto putFunc = [ this ]( const AdaptIQReq & req, Callback<const AdaptIQReq> resp )
    {
        AdaptIQReq respMsg;
        HandlePut( req, respMsg );
        resp.Send( respMsg );
    };
    AsyncCallback<const AdaptIQReq&, Callback<AdaptIQReq>> putCb( putFunc, m_ProductTask );
    m_PutConnection = m_FrontDoorClient->RegisterPut<AdaptIQReq>( s_FrontDoorAdaptIQ, putFunc );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief ProductAdaptIQManager::Stop
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductAdaptIQManager::Stop( void )
{
    m_PutConnection.Disconnect();
    m_GetConnection.Disconnect();
}


////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductAdaptIQManager::HandleGet
///
/// @brief  This method populates the supplied AdaptIQStatus argument
///
/// @param  AdaptIQStatus
///
/// @return
///
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductAdaptIQManager::HandleGet( AdaptIQStatus& status )
{
    // fill in list of supported actions
    status.mutable_properties()->add_supportedactions( s_ActionEnter );
    status.mutable_properties()->add_supportedactions( s_ActionCancel );
    status.mutable_properties()->add_supportedactions( s_ActionAdvance );

    // fill in list of supported modes
    status.mutable_properties()->add_supportedmodes( s_ModeNormal );
    status.mutable_properties()->add_supportedmodes( s_ModeRetail );
    status.mutable_properties()->add_supportedmodes( s_ModeDisabled );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductAdaptIQManager::HandlePut
///
/// @brief  This method handles an AdaptIQ request
///
/// @param  AdaptIQReq
///
/// @return
///
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductAdaptIQManager::HandlePut( const AdaptIQReq& req, ProductPb::AdaptIQReq& resp )
{
    // TODO : there's no response defined in the LAN API right now, so just mirror back the request
    resp = req;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                               End of ProductApp Namespace                                    ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
