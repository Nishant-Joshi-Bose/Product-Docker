////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file      ProductAdaptIQManager.cpp
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
#include "ProductAdaptIQManager.h"
#include "SharedProto.pb.h"
#include "ProductEndpointDefines.h"
#include "PGCErrorCodes.h"

using namespace ProductPb;

namespace
{
const std::string s_ActionEnter         = "ACTION_ENTER";
const std::string s_ActionCancel        = "ACTION_CANCEL";
const std::string s_ActionAdvance       = "ACTION_ADVANCE";
const std::string s_ActionPrevious      = "ACTION_PREVIOUS";

#if 0
// these have moved to a new endpoint, not sure if they will be implemented here
// or elsewhere
const std::string s_ModeNormal          = "Enabled Normal";
const std::string s_ModeRetail          = "Enabled Retail";
const std::string s_ModeDisabled        = "Enabled Disabled";
#endif

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
    m_status.set_smstate( "AIQ_STATE_NOT_RUNNING" );
    m_status.set_currentlocation( ADAPTIQ_LOCATION_FIRST );
    m_status.set_currentspeaker( IpcAiqMeasChannel_t_Name( LpmServiceMessages::AIQ_MEAS_CHANNEL_NONE ) );
    m_status.set_hpconnected( true );
    m_status.set_errorcode( "AIQ_ERROR_NONE" );
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

    auto lpmFunc = [ this ]( bool connected )
    {
        SetLpmConnectionState( connected );
    };

    m_ProductLpmHardwareInterface->RegisterForLpmConnection( Callback<bool>( lpmFunc ) );

    auto getFunc = [ this ]( const Callback< const AdaptIQStatus >& resp,
                             const Callback< FrontDoor::Error >&    errorRsp )
    {
        HandleGet( resp, errorRsp );
    };

    AsyncCallback< Callback< AdaptIQStatus >, Callback< FrontDoor::Error > >
    getCb( getFunc, m_ProductTask );

    m_GetConnection = m_FrontDoorClient->RegisterGet( FRONTDOOR_ADAPTIQ_API,
                                                      getCb,
                                                      FrontDoor::PUBLIC,
                                                      FRONTDOOR_PRODUCT_CONTROLLER_VERSION,
                                                      FRONTDOOR_PRODUCT_CONTROLLER_GROUP_NAME );

    auto putFunc = [ this ]( const AdaptIQReq                       req,
                             const Callback< const AdaptIQStatus >& resp,
                             const Callback< FrontDoor::Error >&    errorRsp )
    {
        HandlePut( req, resp, errorRsp );
    };

    AsyncCallback< const AdaptIQReq, Callback< AdaptIQStatus >, Callback< FrontDoor::Error > >
    putCb( putFunc, m_ProductTask );

    m_PutConnection = m_FrontDoorClient->RegisterPut< AdaptIQReq >(
                          FRONTDOOR_ADAPTIQ_API,
                          putCb,
                          FrontDoor::PUBLIC,
                          FRONTDOOR_PRODUCT_CONTROLLER_VERSION,
                          FRONTDOOR_PRODUCT_CONTROLLER_GROUP_NAME );
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
/// @brief ProductAdaptIQManager::SetDefaultProperties
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductAdaptIQManager::SetDefaultProperties( ProductPb::AdaptIQStatus& status )
{
    // fill in list of supported actions
    status.mutable_properties()->add_supportedactions( s_ActionEnter );
    status.mutable_properties()->add_supportedactions( s_ActionCancel );
    status.mutable_properties()->add_supportedactions( s_ActionAdvance );
    status.mutable_properties()->add_supportedactions( s_ActionPrevious );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief ProductAdaptIQManager::SetStatus
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductAdaptIQManager::DSPToFrontDoorStatus( ProductPb::AdaptIQStatus& frontDoorStatus, const ProductAdaptIQStatus& dspStatus )
{
    /*
     * We should fix this at some point, as casting away const-ness *feels* wrong
     * Presumably the issue here is that mutable_status() member generated by the protobuf class will allocate a new
     * object if the object doesn't exist, which should never be the case here
     */
    ProductAdaptIQStatus& status = const_cast<ProductAdaptIQStatus&>( dspStatus );

    frontDoorStatus.set_smstate( IpcAiqState_t_Name( status.mutable_status()->smstate() ) );
    frontDoorStatus.set_currentlocation( status.mutable_status()->currentlocation() );
    frontDoorStatus.set_currentspeaker( IpcAiqMeasChannel_t_Name( status.mutable_status()->currentchannel() ) );
    frontDoorStatus.set_hpconnected( status.mutable_status()->hpconnected() );
    frontDoorStatus.set_errorcode( IpcAiqError_t_Name( status.mutable_status()->errorcode() ) );

}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief ProductAdaptIQManager::SetStatus
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductAdaptIQManager::SetStatus( const ProductPb::AdaptIQStatus& status, bool force )
{
    if( ( m_status.SerializeAsString() != status.SerializeAsString() ) || force )
    {
        m_status = status;

        // make a copy and add fixed properties to it
        ProductPb::AdaptIQStatus msg = status;
        SetDefaultProperties( msg );
        m_FrontDoorClient->SendNotification( FRONTDOOR_ADAPTIQ_API, msg );
    }
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
void ProductAdaptIQManager::HandleGet( const Callback<const AdaptIQStatus> & resp, const Callback<FrontDoor::Error> & errorRsp )
{
    AdaptIQStatus status = m_status;
    SetDefaultProperties( status );
    resp.Send( status );
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

////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductAdaptIQManager::HandlePut( const AdaptIQReq req, const Callback<const AdaptIQStatus> & resp, const Callback<FrontDoor::Error> & errorRsp )
{
    ProductMessage msg;
    FrontDoor::Error error;

    if( !req.has_action() )
    {
        error.set_message( "The AiQ request had no action." );
    }
    else if( req.action() == s_ActionEnter )
    {
        msg.mutable_aiqcontrol()->set_action( ProductAdaptIQControl::Start );
    }
    else if( req.action() == s_ActionCancel )
    {
        msg.mutable_aiqcontrol()->set_action( ProductAdaptIQControl::Cancel );
    }
    else if( req.action() == s_ActionAdvance )
    {
        msg.mutable_aiqcontrol()->set_action( ProductAdaptIQControl::Advance );
    }
    else if( req.action() == s_ActionPrevious )
    {
        msg.mutable_aiqcontrol()->set_action( ProductAdaptIQControl::Previous );
    }
    else
    {
        error.set_message( "The AiQ request had an invalid action: " + req.action() );
    }

    if( msg.has_aiqcontrol() )
    {
        IL::BreakThread( [ = ]( )
        {
            m_ProductNotify( msg );
        }, m_ProductTask );

        HandleGet( resp, errorRsp );
    }
    else if( error.has_message() )
    {
        error.set_code( PGCErrorCodes::ERROR_CODE_PRODUCT_CONTROLLER_CUSTOM );
        error.set_subcode( PGCErrorCodes::ERROR_SUBCODE_AIQ );
        errorRsp.Send( error );
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductAdaptIQManager::SetLpmConnectionState
///
/// @brief  The following methods is called when an LPM connection event is received
///
/// @param  connected
///
/// @return
///
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductAdaptIQManager::SetLpmConnectionState( bool connected )
{
    if( connected )
    {
        RegisterLpmClientEvents();
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductAdaptIQManager::RegisterLpmClientEvents
///
/// @brief  Register to receive events from the LPM
///
/// @param  none
///
/// @return none
///
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductAdaptIQManager::RegisterLpmClientEvents( )
{
    auto aiqFunc = [ this ]( LpmServiceMessages::IpcAiqSetupStatus_t status )
    {
        HandleAdaptIQStatus( status );
    };
    bool success =  m_ProductLpmHardwareInterface->RegisterForLpmEvents< LpmServiceMessages::IpcAiqSetupStatus_t >
                    ( LpmServiceMessages::IPC_DSP_AIQ_SETUP_STATUS, Callback<LpmServiceMessages::IpcAiqSetupStatus_t >( aiqFunc ) );

    BOSE_INFO( s_logger, "%s registered for AdaptIQ status from the LPM hardware.",
               ( success ? "Successfully" : "Unsuccessfully" ) );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductAdaptIQManager::HandleAdaptIQStatus
///
/// @brief  Process an AdaptIQ status message from the LPM
///
/// @param  none
///
/// @return none
///
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductAdaptIQManager::HandleAdaptIQStatus( LpmServiceMessages::IpcAiqSetupStatus_t status )
{
    ProductMessage msg;

    BOSE_INFO( s_logger, "%s", __func__ );

    *( msg.mutable_aiqstatus()->mutable_status() ) = status;

    IL::BreakThread( [ = ]( )
    {
        m_ProductNotify( msg );
    }, m_ProductTask );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductAdaptIQManager::SendAdaptIQControl
///
/// @brief  Send a control message to AdaptIQ
///
/// @param  none
///
/// @return none
///
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductAdaptIQManager::SendAdaptIQControl( ProductAdaptIQControl::AdaptIQAction action )
{
    m_ProductLpmHardwareInterface->SendAdaptIQControl( action );
}




////////////////////////////////////////////////////////////////////////////////////////////////////
///                               End of ProductApp Namespace                                    ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
