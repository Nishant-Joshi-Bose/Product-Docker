////////////////////////////////////////////////////////////////////////////////
/// @file   ProductController.cpp
/// @brief  Generic Product controller class for Riviera based products.
///
/// @attention Copyright 2017 Bose Corporation, Framingham, MA
////////////////////////////////////////////////////////////////////////////////

#include "ProductController.h"
#include "APTaskFactory.h"
#include "AsyncCallback.h"
#include "ProtoToMarkup.h"

static DPrint s_logger( "ProductController" );

namespace ProductApp
{
ProductController::ProductController():
    m_ProductControllerTask( IL::CreateTask( "ProductControllerTask" ) ),
    m_ProductAppHsm( m_ProductControllerTask, "ProductApplicationHsm", *this ),
    m_FrontDoorClientInterface( "eddie", "localhost", m_ProductControllerTask ),
    m_ProductAppStateTop( m_ProductAppHsm, NULL,  *this ),
    m_ProductAppStateBooting( m_ProductAppHsm, &m_ProductAppStateTop, *this ),
    m_ProductAppStateStdOp( m_ProductAppHsm, &m_ProductAppStateTop, *this ),
    m_ProductAppStateSetup( m_ProductAppHsm, &m_ProductAppStateStdOp, *this ),
    m_ProductAppStateStandby( m_ProductAppHsm, &m_ProductAppStateStdOp, *this )
{
    BOSE_INFO( s_logger, __func__ );

    m_ProductAppHsm.AddState( &m_ProductAppStateTop );
    m_ProductAppHsm.AddState( &m_ProductAppStateBooting );
    m_ProductAppHsm.AddState( &m_ProductAppStateStdOp );
    m_ProductAppHsm.AddState( &m_ProductAppStateSetup );
    m_ProductAppHsm.AddState( &m_ProductAppStateStandby );

    m_ProductAppHsm.Init( PRODUCT_APP_STATE_BOOTING );

    RegisterCallbacks();

    /// This will make FrontDoorClientInterface task to start and accepting request from front door client.
    m_FrontDoorClientInterface.Start();
}

ProductController::~ProductController()
{
}

void ProductController::Initialize()
{
    m_productCliClient.Initialize( m_ProductControllerTask );
}

void ProductController::RegisterCallbacks()
{
    BOSE_INFO( s_logger, __func__ );
    /// Registration of endpoints to the frontdoor client.
    auto func = std::bind( &ProductController::HandleFrontDoorRequest, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3 );
    AsyncCallback<SoundTouchInterface::msg_Header const&, std::string const&, std::string const&>setupEndPointCB( func, m_ProductControllerTask );
    m_FrontDoorClientInterface.RegisterEndPoint( "setup", setupEndPointCB );
}

void ProductController::HandleFrontDoorRequest( SoundTouchInterface::msg_Header const& cookie, std::string const& body, std::string const& operation )
{
    BOSE_INFO( s_logger, __func__ );

    /// Forward the request to Product application Hsm.
    m_ProductAppHsm.Handle<SoundTouchInterface::msg_Header const&, std::string const&, std::string const&>( &ProductAppState::HandleSetupEndPoint, cookie, body, operation );

    ///Send Response to Front Door Client
    SoundTouchInterface::status rsp;
    rsp.set_text( "/" + operation );
    m_FrontDoorClientInterface.SendResponseForUnhandledRequest( cookie, ProtoToMarkup::ToXML( rsp ) );
}
} // namespace ProductApp
