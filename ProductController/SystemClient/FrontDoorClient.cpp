////////////////////////////////////////////////////////////////////////////////
/// @file   FrontDoorClient.cpp
/// @brief  Class for connection management to web socket server (front end).
///
/// @attention Copyright 2017 Bose Corporation, Framingham, MA
////////////////////////////////////////////////////////////////////////////////

#include "FrontDoorClient.h"
#include "APTaskFactory.h"
#include "DPrint.h"

static DPrint s_logger( "FrontDoorClient" );

namespace ProductApp
{

FrontDoorClient::FrontDoorClient( std::string host ): 
    SoundTouch::ClientSocket( "productapp", std::move( host ), 0 ),
    APTask( "FrontDoorClient" ),
    m_task( IL::CreateTask( "FrontDoorClientTimerTask" ) )
{
    BOSE_INFO( s_logger, __func__ );
}

FrontDoorClient::~FrontDoorClient()
{
}

void FrontDoorClient::OnOpen()
{
    BOSE_INFO( s_logger, __func__ );
}

void FrontDoorClient::OnClose()
{
    BOSE_INFO( s_logger, __func__ );
}

void FrontDoorClient::OnEntry()
{
    Run();
}

void FrontDoorClient::OnMessage( std::string msg )
{
    BOSE_LOG( INFO, "message" << msg );
}

} // namespace ProductApp
