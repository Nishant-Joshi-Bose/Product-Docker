////////////////////////////////////////////////////////////////////////////////
/// @file      Clock.cpp
///
/// @brief     Clock class.
///
/// @attention Copyright (C) 2018 Bose Corporation All Rights Reserved
////////////////////////////////////////////////////////////////////////////////
#include <fstream>
#include "ProductLogger.h"
#include "Clock.h"
#include "ProductController.h"
#include "AsyncCallback.h"
#include "EndPointsDefines.h"
#include "ProductConstants.h"
#include "SystemUtils.h"
#include "CommonCLICmdsKeys.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

////////////////////////////////////////////////////////////////////////////////
/// Class Name Declaration for Logging
////////////////////////////////////////////////////////////////////////////////
namespace
{
constexpr char CLASS_NAME[ ] = "Clock";
}


namespace ProductApp
{

Clock::Clock( std::shared_ptr<FrontDoorClientIF> FrontDoorClient, NotifyTargetTaskIF* task, std::string const& guid ):
    m_FrontDoorClientIF( FrontDoorClient ), m_Task( task ), m_ProductGuid( guid ),
    m_ClockPersist( ProtoPersistenceFactory :: Create( "Clock.json", g_ProductPersistenceDir ) )
{
}

Clock::~Clock()
{
}

void Clock::Initialize( )
{
    BOSE_INFO( s_logger, "%s::%s", CLASS_NAME, __func__ );
    LoadFromPersistence();
    RegisterEndPoints();
}

void Clock::RegisterEndPoints()
{
    BOSE_INFO( s_logger, "%s::%s", CLASS_NAME, __func__ );

    ///Register clock API GET request handler
    AsyncCallback< Callback< DeviceManagerPb::Clock >, Callback< EndPointsError::Error > >
    getClockReqCb( std::bind( &Clock::HandleGetClock,
                              this,
                              std::placeholders::_1,
                              std::placeholders::_2 ),
                   m_Task );

    m_FrontDoorClientIF->RegisterGet( FRONTDOOR_CLOCK_API ,
                                      getClockReqCb,
                                      FrontDoor::PUBLIC,
                                      FRONTDOOR_PRODUCT_CONTROLLER_VERSION,
                                      FRONTDOOR_PRODUCT_CONTROLLER_GROUP_NAME );

    ///Register clock API PUT request handler
    AsyncCallback< DeviceManagerPb::Clock,
                   Callback< DeviceManagerPb::Clock >,
                   Callback< EndPointsError::Error > >
                   putClockReqCb( std::bind( &Clock :: HandlePutClock,
                                             this,
                                             std::placeholders::_1,
                                             std::placeholders::_2,
                                             std::placeholders::_3 ),
                                  m_Task );

    m_FrontDoorClientIF->RegisterPut< DeviceManagerPb::Clock > ( FRONTDOOR_CLOCK_API,
                                                                 putClockReqCb,
                                                                 FrontDoor::PUBLIC,
                                                                 FRONTDOOR_PRODUCT_CONTROLLER_VERSION,
                                                                 FRONTDOOR_PRODUCT_CONTROLLER_GROUP_NAME );
}

void Clock::RegisterCliCmds( CliClientMT &cliClient )
{
    BOSE_INFO( s_logger, "%s::%s", CLASS_NAME, __func__ );

    auto callbackForCommands = [ this ]( uint16_t                            cmdKey,
                                         const std::list<std::string>&       argList,
                                         AsyncCallback<std::string, int32_t> respCb,
                                         int32_t                             transact_id )
    {
        HandleCliCmd( cmdKey, argList, respCb, transact_id );
    };
    cliClient.RegisterCLIServerCommands( "product get_clock",
                                         "command to know whether clock is enabled or not.",
                                         "\t get_clock \t",
                                         m_Task,
                                         callbackForCommands,
                                         static_cast<int>( CommonCLICmdKeys::GET_CLOCK ) );

    cliClient.RegisterCLIServerCommands( "product set_clock",
                                         "command to set clock as enabled or disabled.",
                                         "\t set_clock [true|false]\t",
                                         m_Task,
                                         callbackForCommands,
                                         static_cast<int>( CommonCLICmdKeys::SET_CLOCK ) );

}

void Clock::HandleGetClock( Callback<DeviceManagerPb::Clock> resp,
                            Callback<EndPointsError::Error> errResponse )
{
    BOSE_DEBUG( s_logger, "%s::%s - Response: %s", CLASS_NAME, __func__, ProtoToMarkup::ToJson( m_ClockInfo ).c_str() );
    resp.Send( m_ClockInfo );
}

void Clock::HandlePutClock( DeviceManagerPb::Clock req,
                            Callback<DeviceManagerPb::Clock> resp,
                            Callback<EndPointsError::Error> errResponse )
{
    EndPointsError::Error error;

    BOSE_DEBUG( s_logger, "%s::%s - Request: %s", CLASS_NAME, __func__, ProtoToMarkup::ToJson( req ).c_str() );

    if( req.has_enable() )
    {
        SetClockStatus( req.enable() );
        HandleGetClock( resp, errResponse );
    }
    else
    {
        error.set_guid( m_ProductGuid );
        error.set_code( 500 );
        error.set_message( "Invalid arguments" );
        BOSE_ERROR( s_logger, "%s::%s - Reponse: %s", CLASS_NAME, __func__, ProtoToMarkup::ToJson( error, false ).c_str() );
        errResponse.Send( error );
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Callback function for registered clock CLI commands.
////////////////////////////////////////////////////////////////////////////////////////////////////
void Clock::HandleCliCmd( uint16_t                              cmdKey,
                          const std::list< std::string >&       argList,
                          AsyncCallback< std::string, int32_t > respCb,
                          int32_t                               transact_id )
{
    BOSE_INFO( s_logger, "%s::%s - cmd: %d", CLASS_NAME, __func__, cmdKey );

    std::string response;

    auto CheckNoArgs = [&argList, &response]( uint32_t expected_no_args )
    {
        if( argList.size() == expected_no_args )
        {
            return true;
        }
        else
        {
            response = "Wrong number of Arguments : Expected ";
            response += std::to_string( expected_no_args );
            response += ( expected_no_args > 1 ) ? " args" : " argument";
            response += ". Got ";
            response += std::to_string( argList.size() );
            response += " arguments";
            return false;
        }
    };

    switch( static_cast< CommonCLICmdKeys >( cmdKey ) )
    {
    case CommonCLICmdKeys::GET_CLOCK:
        response = "Clock Enable : ";
        response += m_ClockInfo.enable() ? "true" : "false";
        break;

    case CommonCLICmdKeys::SET_CLOCK :
        if( CheckNoArgs( 1 ) )
        {
            const std::string  &arg = argList.front();
            if( "true" == arg )
            {
                SetClockStatus( true );
                response = "Successfully set clock to true";
            }
            else if( "false" == arg )
            {
                SetClockStatus( false );
                response = "Successfully set clock to false";
            }
            else
            {
                response = "ERROR: Use \"true\" or \"false\" as argument";
            }
        }
        break;

    default:
        response = "Command not found";
        BOSE_WARNING( s_logger, "%s::%s: Command %d not found", CLASS_NAME, __func__, cmdKey );
        break;
    }

    respCb( response, transact_id );
}

void Clock::SetClockStatus( bool status )
{
    if( status != m_ClockInfo.enable() )
    {
        m_ClockInfo.set_enable( status );
        SaveToPersistence();
        m_FrontDoorClientIF->SendNotification( FRONTDOOR_CLOCK_API, m_ClockInfo );
    }
}


void Clock::LoadFromPersistence()
{
    try
    {
        ProtoToMarkup::FromJson( m_ClockPersist->Load(), &m_ClockInfo );
        BOSE_INFO( s_logger, "%s::%s: %s", CLASS_NAME, __func__, ProtoToMarkup::ToJson( m_ClockInfo ).c_str() );
    }
    catch( const ProtoToMarkup::MarkupError &e )
    {
        BOSE_LOG( ERROR, "Clock setting from persistence failed markup error - " << e.what() );
        m_ClockInfo.set_enable( false ); //defaulting to false
    }
    catch( ProtoPersistenceIF::ProtoPersistenceException& e )
    {
        BOSE_LOG( ERROR, "Clock setting from persistence failed - " << e.what() );
        m_ClockInfo.set_enable( false ); //defaulting to false
    }
}

void Clock::SaveToPersistence()
{
    m_ClockPersist->Remove();
    m_ClockPersist->Store( ProtoToMarkup::ToJson( m_ClockInfo ) );
}

} ///namespace ProductApp
