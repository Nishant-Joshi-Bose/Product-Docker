////////////////////////////////////////////////////////////////////////////////
///// @file   DisplayController.cpp
///// @brief  Implements Eddie Display controller class.
/////
///// @attention Copyright 2017 Bose Corporation, Framingham, MA
//////////////////////////////////////////////////////////////////////////////////

#include <limits.h>
#include <functional>
#include "FrontDoorClient.h"
#include "DisplayController.h"
#include "ProductController.h"
#include "LpmClientFactory.h"
#include "SyncCallback.h"

static DPrint s_logger( "DisplayController" );
using namespace ::DisplayController::Protobuf;

namespace ProductApp
{

typedef enum {
    BACK_LIGHT_LEVEL_DIM          =  15,
    BACK_LIGHT_LEVEL_DIM_HIGH     =  30,
    BACK_LIGHT_LEVEL_MEDIUM       =  50,
    BACK_LIGHT_LEVEL_MEDIUM_HIGH  =  65,
    BACK_LIGHT_LEVEL_BRIGHT       =  70,
    BACK_LIGHT_LEVEL_BRIGHT_HIGH  = 100
} e_backLightLevel;

typedef enum {
    DIM_TO_MEDIUM    = 280,
    MEDIUM_TO_DIM    = 170,
    MEDIUM_TO_BRIGHT = 598,
    BRIGHT_TO_MEDIUM = 400
} e_luxThreshold;

static const int MONITOR_SENSOR_SLEEP_MS  = 1000;
static const int CHANGING_LEVEL_SLEEP_MS  = 10;
static const int BACKLIGHT_DIFF_THRESHOLD = (BACK_LIGHT_LEVEL_DIM_HIGH - BACK_LIGHT_LEVEL_DIM - 1);

DisplayController::DisplayController( ProductController& controller, const std::shared_ptr<FrontDoorClientIF>& fd_client, LpmClientIF::LpmClientPtr clientPtr ):
    m_productController ( controller ),
    m_frontdoorClientPtr( fd_client  ),
    m_lpmClient         ( clientPtr  ),
    m_timeToStop        ( false      ),
    m_autoMode          ( true       )
{
    s_logger.SetLogLevel( "DisplayController", DPrint::WARNING );
}// constructor

DisplayController::~DisplayController()
{
    m_timeToStop = true;

    if (m_threadMonitorLightSensor)
    {
        m_threadMonitorLightSensor->join();
    }
}// destructor

int DisplayController::GetBackLightLevelFromLux ( int lux, int lux_rising )
{
    if ( lux_rising > 1 )
    {
        if ( lux >= MEDIUM_TO_BRIGHT )
        {
            return BACK_LIGHT_LEVEL_BRIGHT_HIGH;
        }
        else if  ( lux >= DIM_TO_MEDIUM )
        {
            return BACK_LIGHT_LEVEL_MEDIUM;
        }

        return BACK_LIGHT_LEVEL_DIM;
    }

    if ( lux >= BRIGHT_TO_MEDIUM )
    {
        return BACK_LIGHT_LEVEL_BRIGHT_HIGH;
    }
    else if (  lux >= MEDIUM_TO_DIM )
    {
        return BACK_LIGHT_LEVEL_MEDIUM;
    }

    return BACK_LIGHT_LEVEL_DIM;
}// GetBackLightLevelFromLux

void DisplayController::SetBackLightLevel ( int actualLevel, int newLevel )
{
    int                steps          = abs(actualLevel - newLevel);
    int                levelIncrement = ((actualLevel > newLevel) ? -1 : 1);
    IpcBackLight_t     backlight;
    std::ostringstream ss;

    ss << "set actual level: " << actualLevel << " new level: " << newLevel;
    BOSE_LOG( INFO, ss.str() );
    ss.str("");

    for ( int i = 0; i < steps; i++ )
    {
        actualLevel += levelIncrement;

        ss << " level: " << actualLevel;
        BOSE_LOG( INFO, ss.str() );
        ss.str("");

        backlight.set_value( actualLevel );
        m_lpmClient->SetBackLight( backlight );
        usleep ( CHANGING_LEVEL_SLEEP_MS * 1000 );
    }// for all the level steps

    if ( actualLevel != newLevel )
    {
        ss << "Warning: adjusting actual level: " << actualLevel << ", new level: " << newLevel << "\n";
        BOSE_LOG( WARNING, ss.str() );

        backlight.set_value( newLevel );
        m_lpmClient->SetBackLight( backlight );
    }
}// SetBackLightLevel

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void DisplayController::MonitorLightSensor()
{
    int                previous_lux   = 0;
    int                targeted_level = 0;
    std::ostringstream ss;

    m_luxDecimal    = 0;
    m_luxFractional = 0;
    m_backLight     = 0;

    // This will activate the light sensor peripheral, the first reading
    // is always zero until the peripheral is activated in continuous
    // reading
    m_lpmClient->GetLightSensor( [this]( IpcLightSensor_t const & rsp ) {});

    while ( ! m_timeToStop )
    {
        m_lpmClient->GetLightSensor( [this]( IpcLightSensor_t const & rsp )
        {
            m_luxDecimal    = ( int )( be16toh( rsp.lux_decimal_value   () ) );
            m_luxFractional = ( int )( be16toh( rsp.lux_fractional_value() ) );
        } );

        m_lpmClient->GetBackLight( [this]( IpcBackLight_t const & rsp )
        {
            m_backLight = rsp.value();
        } );

        ss << "actual lux: " << m_luxDecimal << ", previous lux: " << previous_lux << ", back light: " << m_backLight << "\n";
        BOSE_LOG( INFO, ss.str() );
        ss.str("");

        if ( m_autoMode )
        {
            targeted_level = GetBackLightLevelFromLux(m_luxDecimal, m_luxDecimal - previous_lux );

            ss << "target level: " << targeted_level << ", actual level: " << m_backLight;
            BOSE_LOG( INFO, ss.str() );
            ss.str("");

            if ( abs (targeted_level - m_backLight ) >= BACKLIGHT_DIFF_THRESHOLD )
            {
                SetBackLightLevel( m_backLight , targeted_level );
                // dummy read of the back light, the IPC mechanism is caching a value
                m_lpmClient->GetBackLight( [this]( IpcBackLight_t const & rsp ) {});
                m_backLight = targeted_level;
            }

            previous_lux = m_luxDecimal;
        }// If we are in automatic mode

        usleep(MONITOR_SENSOR_SLEEP_MS * 1000);
    }// while it's not time to stop
}// MonitorLightSensor

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void DisplayController::Initialize()
{
    //ui/display end point registration with front door
    RegisterDisplayEndPoints();

    m_threadMonitorLightSensor = std::unique_ptr<std::thread>(new std::thread([this] { MonitorLightSensor(); } ));
}// Initialize

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void DisplayController::RegisterDisplayEndPoints()
{
    auto backLightCallBack   = std::bind( &DisplayController::HandleLpmNotificationBackLight  , this, std::placeholders::_1 );
    auto lightSensorCallBack = std::bind( &DisplayController::HandleLpmNotificationLightSensor, this, std::placeholders::_1 );

    AsyncCallback<IpcBackLight_t  > notification_cb_back_light  ( backLightCallBack  , m_productController.GetTask() );
    AsyncCallback<IpcLightSensor_t> notification_cb_light_sensor( lightSensorCallBack, m_productController.GetTask() );

    m_lpmClient->RegisterEvent<IpcBackLight_t  >( IPC_PER_GET_BACKLIGHT  , notification_cb_back_light   );
    m_lpmClient->RegisterEvent<IpcLightSensor_t>( IPC_PER_GET_LIGHTSENSOR, notification_cb_light_sensor );

    AsyncCallback<Display, Callback<Display>> putDisplayReqCb(
                                                    std::bind( &DisplayController::HandlePutDisplayRequest ,
                                                               this,
                                                               std::placeholders::_1,
                                                               std::placeholders::_2
                                                             ),
                                                    m_productController.GetTask() );
    m_frontdoorClientPtr->RegisterPut<Display>( "/ui/display", putDisplayReqCb );

    AsyncCallback< Callback<Display>> getDisplayReqCb(
                                         std::bind(
                                             &DisplayController::HandleGetDisplayRequest,
                                             this,
                                             std::placeholders::_1
                                         ),
                                         m_productController.GetTask() );
    m_frontdoorClientPtr->RegisterGet( "/ui/Display", getDisplayReqCb );

}// RegisterDisplayEndPoints

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void DisplayController::HandlePutDisplayRequest( const Display &req,
                                                 const Callback<Display>& resp )
{
}// HandlePutDisplayRequest

void  DisplayController::HandleGetDisplayRequest( const Callback<Display>& resp )
{
    resp.Send ( GetDisplay() );
}// HandleGetDisplayRequest

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
Display DisplayController::GetDisplay()
{
    return m_display;
}// GetDisplay

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
bool DisplayController::HandleLpmNotificationBackLight( IpcBackLight_t lpmBackLight )
{
    m_display.set_backlightprecentage( lpmBackLight.value() );

    m_frontdoorClientPtr->SendNotification( "/ui/display" , GetDisplay() );
    return true;
}// HandleLpmNotificationBackLight


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
bool DisplayController::HandleLpmNotificationLightSensor( IpcLightSensor_t lpmLightSensor )
{
    m_display.set_lightsensorlux( lpmLightSensor.lux_decimal_value() );

    m_frontdoorClientPtr->SendNotification( "/ui/display" , GetDisplay() );
    return true;
}// HandleLpmNotificationBackLight

} //namespace ProductApp
