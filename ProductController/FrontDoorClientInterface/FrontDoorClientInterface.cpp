////////////////////////////////////////////////////////////////////////////////
/// @file   FrontDoorClientInterface.cpp
/// @brief  Interface class to frontdoor client.
///
/// @attention Copyright 2017 Bose Corporation, Framingham, MA
////////////////////////////////////////////////////////////////////////////////

#include "FrontDoorClientInterface.h"
#include "APTaskFactory.h"
#include "BreakThread.h"
#include "DPrint.h"

static DPrint s_logger( "FrontDoorClientInterface" );

namespace ProductApp
{

FrontDoorClientInterface::FrontDoorClientInterface( const std::string productName, std::string host , NotifyTargetTaskIF* pTask ):
    SoundTouch::Client( productName, std::move( host ) ),
    APTask( "FrontDoorClientInterface" ),
    m_ProductControllerTask( pTask )
{
    BOSE_INFO( s_logger, __func__ );
}

FrontDoorClientInterface::~FrontDoorClientInterface()
{
    m_Running = false;
}

void FrontDoorClientInterface::OnOpen()
{
    BOSE_INFO( s_logger, __func__ );
}

void FrontDoorClientInterface::OnClose()
{
    BOSE_INFO( s_logger, __func__ );
}

void FrontDoorClientInterface::OnEntry()
{
    BOSE_INFO( s_logger, __func__ );
    FrontDoorClientRun();
}

void FrontDoorClientInterface::FrontDoorClientRun()
{
    BOSE_INFO( s_logger, __func__ );
    while( m_Running )
    {
        Run();
    }
}

void FrontDoorClientInterface::OnMessage( SoundTouchInterface::initializationCompleteRequest req,
                                          SoundTouchInterface::msg msg )
{
    BOSE_INFO( s_logger, __func__ );

    SoundTouchInterface::initializationCompleteResponse response;
    response.set_action( SoundTouchInterface::sourceAction::INIT_STANDBY );
    InitializationCompleteResponse( std::move( msg ), response );
    GetNetworkInfo();
}

void FrontDoorClientInterface::OnMessage( SoundTouchInterface::activateProductRequest req,
                                          SoundTouchInterface::msg msg )
{
    BOSE_INFO( s_logger, "%s(activateProductRequest)", __func__ );
    ActivateProductResponse( std::move( msg ) );
}

bool FrontDoorClientInterface::UnhandledRequest( SoundTouchInterface::msg_Header const& cookie, std::string const& body, std::string const& operation )
{
    BOSE_LOG( INFO, "url:'"  << cookie.url() << "' body: '" << DPrint::Limit( body ) << "' operation:'" << operation << "'" );
    auto it = m_EndPointRequestCallbackMap.find( operation );
    if( it == m_EndPointRequestCallbackMap.end() )
    {
        /// Unhandled by Product, let Frontdoor client handle the request.
        BOSE_INFO( s_logger, "%s Unhandled by Product..", __func__ );
        return false;
    }
    it->second( cookie, body, operation );
    return true;
}


void FrontDoorClientInterface::RegisterEndPoint( std::string endPoint, const EndPointRequestCallback cb )
{
    BOSE_INFO( s_logger, "%s(endPoint=%s)", __func__, endPoint.c_str() );
    auto ret = m_EndPointRequestCallbackMap.emplace( endPoint, cb );
    if( not ret.second )
    {
        BOSE_ERROR( s_logger, "%s(endPoint=%s) already registered", __func__, endPoint.c_str() );
    }
}

} /// namespace ProductApp
