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
#include "EndPointsError.pb.h"

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
    m_status.set_smstate( "AIQ_STATE_NOT_RUNNING" );
    m_status.set_currentlocation( ADAPTIQ_LOCATION_FIRST );
    m_status.set_currentspeaker( ADAPTIQ_SPEAKER_FIRST );
    m_status.set_hpconnected( true );
    m_status.set_errorcode( 0 );
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

    auto getFunc = [ this ]( const Callback<const AdaptIQStatus>& resp, const Callback<EndPointsError::Error>& errorRsp )
    {
        AdaptIQStatus status;
        HandleGet( status );
        resp.Send( status );
    };
    AsyncCallback<Callback<AdaptIQStatus>, Callback<EndPointsError::Error> > getCb( getFunc, m_ProductTask );
    m_GetConnection = m_FrontDoorClient->RegisterGet( s_FrontDoorAdaptIQ, getCb );

    auto putFunc = [ this ]( const AdaptIQReq req, const Callback<const AdaptIQReq>& resp, const Callback<EndPointsError::Error>& errorRsp )
    {
        AdaptIQReq respMsg;
        HandlePut( req, respMsg );
        resp.Send( respMsg );
    };
    AsyncCallback<const AdaptIQReq, Callback<AdaptIQReq>, Callback<EndPointsError::Error>> putCb( putFunc, m_ProductTask );
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

    /*
     * none of these definitions belong here, and they will be removed from here once aiq definition is finished,
     * but for now with the definition in flux this keeps us from having to change a bunch of other files
     */
    // https://wiki.bose.com/pages/viewpage.action?spaceKey=A4V&title=Professor+AdaptIQ#?lucidIFH-viewer-73845d7d=1
    // https://svn.bose.com/hepd/RivieraSharc/trunk/modules/IPC/IpcProtocol.h
    enum IpcAiqStates
    {
        AIQ_STATE_NONE,
        AIQ_STATE_NOT_RUNNING,
        AIQ_STATE_INTRO_3_COMFY,
        AIQ_STATE_INTRO_4_READY,
        AIQ_STATE_MEASURING,
        AIQ_STATE_TRANSITION_FIRST,
        AIQ_STATE_TRANSITION_NEXT,
        AIQ_STATE_TRANSITION_LAST,
        AIQ_STATE_SUCCESS,
        AIQ_STATE_ERR_NOISE,
        AIQ_STATE_ERR_TOO_CLOSE,
        AIQ_STATE_ERR_MICS_MOVED,
        AIQ_STATE_ERR_NO_SOUND,
        AIQ_STATE_ERR_PREVIOUS,
        AIQ_STATE_ERR_FAIL,
        AIQ_STATE_ERR_HS_DETECT,
        AIQ_STATE_ERR_HS_LOST,
        AIQ_STATE_ERR_HS_FAIL,
    };

#define MAP_ENUM_STRING(e) {e, #e}
    static std::map<IpcAiqStates, const char *> stateIdToName =
    {
        MAP_ENUM_STRING( AIQ_STATE_NONE ),
        MAP_ENUM_STRING( AIQ_STATE_NOT_RUNNING ),
        MAP_ENUM_STRING( AIQ_STATE_INTRO_3_COMFY ),
        MAP_ENUM_STRING( AIQ_STATE_INTRO_4_READY ),
        MAP_ENUM_STRING( AIQ_STATE_MEASURING ),
        MAP_ENUM_STRING( AIQ_STATE_TRANSITION_FIRST ),
        MAP_ENUM_STRING( AIQ_STATE_TRANSITION_NEXT ),
        MAP_ENUM_STRING( AIQ_STATE_TRANSITION_LAST ),
        MAP_ENUM_STRING( AIQ_STATE_SUCCESS ),
        MAP_ENUM_STRING( AIQ_STATE_ERR_NOISE ),
        MAP_ENUM_STRING( AIQ_STATE_ERR_TOO_CLOSE ),
        MAP_ENUM_STRING( AIQ_STATE_ERR_MICS_MOVED ),
        MAP_ENUM_STRING( AIQ_STATE_ERR_NO_SOUND ),
        MAP_ENUM_STRING( AIQ_STATE_ERR_PREVIOUS ),
        MAP_ENUM_STRING( AIQ_STATE_ERR_FAIL ),
        MAP_ENUM_STRING( AIQ_STATE_ERR_HS_DETECT ),
        MAP_ENUM_STRING( AIQ_STATE_ERR_HS_LOST ),
        MAP_ENUM_STRING( AIQ_STATE_ERR_HS_FAIL ),
    };

    // set the translated fields ...
    IpcAiqStates smState = static_cast<IpcAiqStates>( status.mutable_status()->smstate() );
    if( stateIdToName.count( smState ) )
    {
        frontDoorStatus.set_smstate( stateIdToName[smState] );
    }
    else
    {
        frontDoorStatus.set_smstate( "AIQ_STATE_UNKNOWN" );
    }

    // ... and set the direct-copy fields
    frontDoorStatus.set_currentlocation( status.mutable_status()->currentlocation() );
    frontDoorStatus.set_currentspeaker( status.mutable_status()->currentchannel() );
    frontDoorStatus.set_hpconnected( status.mutable_status()->hpconnected() );
    frontDoorStatus.set_errorcode( status.mutable_status()->errorcode() );

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
        m_FrontDoorClient->SendNotification( s_FrontDoorAdaptIQ, msg );
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
    status = m_status;
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
