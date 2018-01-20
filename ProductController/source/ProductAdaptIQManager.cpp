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

using namespace ProductPb;

namespace
{
const std::string s_ActionEnter         = "enter";
const std::string s_ActionCancel        = "cancel";
const std::string s_ActionAdvance       = "advance";
const std::string s_ActionPrevious      = "previous";

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
    m_status.set_smstate( "NA" );
    m_status.set_mode( "Booting" );
    m_status.set_currentlocation( ADAPTIQ_LOCATION_FIRST );
    m_status.set_currentspeaker( ADAPTIQ_SPEAKER_FIRST );
    m_status.set_hpconnected( true );
    m_status.set_errorcode( 0 );
    SetDefaultProperties( m_status );
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

    auto getFunc = [ this ]( Callback<const AdaptIQStatus> resp )
    {
        AdaptIQStatus status;
        HandleGet( status );
        resp.Send( status );
    };
    AsyncCallback<Callback<AdaptIQStatus>> getCb( getFunc, m_ProductTask );
    m_GetConnection = m_FrontDoorClient->RegisterGet( s_FrontDoorAdaptIQ, getCb );

    auto putFunc = [ this ]( const AdaptIQReq req, Callback<const AdaptIQReq> resp )
    {
        AdaptIQReq respMsg;
        HandlePut( req, respMsg );
        resp.Send( respMsg );
    };
    AsyncCallback<const AdaptIQReq, Callback<AdaptIQReq>> putCb( putFunc, m_ProductTask );
    m_PutConnection = m_FrontDoorClient->RegisterPut<AdaptIQReq>( s_FrontDoorAdaptIQ, putCb );
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

    // fill in list of supported modes
    status.mutable_properties()->add_supportedmodes( s_ModeNormal );
    status.mutable_properties()->add_supportedmodes( s_ModeRetail );
    status.mutable_properties()->add_supportedmodes( s_ModeDisabled );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief ProductAdaptIQManager::SetStatus
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductAdaptIQManager::DSPToFrontDoorStatus( ProductPb::AdaptIQStatus& frontDoorStatus, LpmServiceMessages::IpcAiqSetupStatus_t& dspStatus )
{
    static const char* smStateNames[] =
    {
        "SM_STATE_0",
        "SM_STATE_1",
        "SM_STATE_2",
        "SM_STATE_3",
        "SM_STATE_4",
        "SM_STATE_5",
        "SM_STATE_6",
        "SM_STATE_7",
        "SM_STATE_8",
        "SM_STATE_9",
    };

    static const char* modeNames[] =
    {
        "MODE_0",
        "MODE_1",
        "MODE_2",
        "MODE_3",
        "MODE_4",
        "MODE_5",
        "MODE_6",
        "MODE_7",
        "MODE_8",
        "MODE_9",
    };

    // set the translated fields ...
    if( dspStatus.smstate() < 10 )
    {
        frontDoorStatus.set_smstate( smStateNames[dspStatus.smstate()] );
    }
    else
    {
        frontDoorStatus.set_smstate( "SM_STATE_UNKNOWN" );
    }

    if( dspStatus.mode() < 10 )
    {
        frontDoorStatus.set_mode( modeNames[dspStatus.mode()] );
    }
    else
    {
        frontDoorStatus.set_mode( "MODE_UNKNOWN" );
    }

    // ... and set the direct-copy fields
    frontDoorStatus.set_currentlocation( dspStatus.currentlocation() );
    frontDoorStatus.set_currentspeaker( dspStatus.currentchannel() );
    frontDoorStatus.set_hpconnected( dspStatus.hpconnected() );
    frontDoorStatus.set_errorcode( dspStatus.errorcode() );

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
        m_FrontDoorClient->SendNotification( s_FrontDoorAdaptIQ, m_status );
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
void ProductAdaptIQManager::HandleGet( AdaptIQStatus& status )
{
    SetDefaultProperties( status );
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
void ProductAdaptIQManager::HandlePut( const AdaptIQReq req, ProductPb::AdaptIQReq& resp )
{
    ProductMessage msg;

    if( !req.has_action() )
    {
    }
    else if( req.action() == "enter" )
    {
        msg.mutable_aiqcontrol()->set_action( ProductAdaptIQControl::Start );
    }
    else if( req.action() == "cancel" )
    {
        msg.mutable_aiqcontrol()->set_action( ProductAdaptIQControl::Cancel );
    }
    else if( req.action() == "advance" )
    {
        msg.mutable_aiqcontrol()->set_action( ProductAdaptIQControl::Advance );
    }
    else if( req.action() == "previous" )
    {
        msg.mutable_aiqcontrol()->set_action( ProductAdaptIQControl::Previous );
    }
    else
    {
    }

    if( msg.has_aiqcontrol() )
    {
        IL::BreakThread( [ = ]( )
        {
            m_ProductNotify( msg );
        }, m_ProductTask );
    }

    // TODO : there's no response defined in the LAN API right now, so just mirror back the request
    resp = req;
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
