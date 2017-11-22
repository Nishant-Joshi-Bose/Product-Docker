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

typedef enum
{
    BACK_LIGHT_LEVEL_DIM           =   2,
    BACK_LIGHT_LEVEL_DIM_HIGH      =  10,
    BACK_LIGHT_LEVEL_MEDIUM_LOW    =  20,
    BACK_LIGHT_LEVEL_MEDIUM        =  25,
    BACK_LIGHT_LEVEL_MEDIUM_HIGH   =  40,
    BACK_LIGHT_LEVEL_BRIGHT        =  60,
    BACK_LIGHT_LEVEL_BRIGHT_HIGH   = 100
} e_backLightLevel;

typedef struct
{
    float            lux ;
    e_backLightLevel level;
} t_luxBacklightTuple;

// The LUX values are in real lux, withiout the light reading device attenuation
t_luxBacklightTuple lowering_lux_threshold[] =
{
    {430, BACK_LIGHT_LEVEL_BRIGHT_HIGH},  // BRIGHT HIGH TO BRIGHT
    {215, BACK_LIGHT_LEVEL_BRIGHT     },  // BRIGHT TO MEDIUM HIGH
    {100, BACK_LIGHT_LEVEL_MEDIUM_HIGH},  // MEDIUM HIGH TO MEDIUM
    { 50, BACK_LIGHT_LEVEL_MEDIUM     },  // MEDIUM TO DIM HIGH
    { 17, BACK_LIGHT_LEVEL_MEDIUM_LOW },
    {  5, BACK_LIGHT_LEVEL_DIM_HIGH   },  // DIM HIGH TO DIM
    {  0, BACK_LIGHT_LEVEL_DIM        }   // value must be 0
};


t_luxBacklightTuple rising_lux_threshold[] =
{
    {430, BACK_LIGHT_LEVEL_BRIGHT_HIGH},  // BRIGHT HIGH TO BRIGHT
    {215, BACK_LIGHT_LEVEL_BRIGHT     },  // BRIGHT TO MEDIUM HIGH
    {100, BACK_LIGHT_LEVEL_MEDIUM_HIGH},  // MEDIUM HIGH TO MEDIUM
    { 50, BACK_LIGHT_LEVEL_MEDIUM     },  // MEDIUM TO DIM HIGH
    { 17, BACK_LIGHT_LEVEL_MEDIUM_LOW },
    {  5, BACK_LIGHT_LEVEL_DIM_HIGH   },  // DIM HIGH TO DIM
    {  1, BACK_LIGHT_LEVEL_DIM        }
};


#if 0
t_luxBacklightTuple rising_lux_threshold  [] =
{
    {150.0, BACK_LIGHT_LEVEL_BRIGHT_HIGH},  // BRIGHT TO BRIGHT HIGH
    { 50.0, BACK_LIGHT_LEVEL_BRIGHT     },  // MEDIUM HIGH TO BRIGHT
    { 30.0, BACK_LIGHT_LEVEL_MEDIUM_HIGH},  // MEDIUM TO MEDIUM HIGH
    { 15.0, BACK_LIGHT_LEVEL_MEDIUM     },  // DIM HIGH TO MEDIUM
    { 10.0, BACK_LIGHT_LEVEL_DIM_HIGH   },  // DIM TO DIM HIGH
    {  0.0, BACK_LIGHT_LEVEL_DIM        }
};
#endif // 0

static const int   MONITOR_SENSOR_SLEEP_MS  = 1000;
static const int   CHANGING_LEVEL_SLEEP_MS  = 10;
static const int   BACKLIGHT_DIFF_THRESHOLD = ( BACK_LIGHT_LEVEL_DIM_HIGH - BACK_LIGHT_LEVEL_DIM - 1 );
static const float PLEXI_LUX_FACTOR         = 1.0f;
static const float SILVER_LUX_FACTOR        = 11.0f;
static const float BLACK_LUX_FACTOR         = 16.0f;
//static const int NB_LIGHT_SENSOR_VALUES   = 5;

DisplayController::DisplayController( ProductController& controller, const std::shared_ptr<FrontDoorClientIF>& fd_client, LpmClientIF::LpmClientPtr clientPtr ):
    m_productController( controller ),
    m_frontdoorClientPtr( fd_client ),
    m_lpmClient( clientPtr ),
    m_timeToStop( false ),
    m_autoMode( true )
{
    //s_logger.SetLogLevel( "DisplayController", DPrint::WARNING );
    s_logger.SetLogLevel( "DisplayController", DPrint::INFO );

    m_luxFactor = SILVER_LUX_FACTOR;
    //m_luxFactor = BLACK_LUX_FACTOR;
    //m_luxFactor = PLEXI_LUX_FACTOR;
}// constructor

DisplayController::~DisplayController()
{
    m_timeToStop = true;

    if( m_threadMonitorLightSensor )
    {
        m_threadMonitorLightSensor->join();
    }
}// destructor

int DisplayController::GetBackLightLevelFromLux( float lux, float lux_rising )
{
    t_luxBacklightTuple* lux_threshold = ( lux_rising > 0.0f ) ? rising_lux_threshold : lowering_lux_threshold;
    int i;

    for( i = 0; lux_threshold[i].lux > 0.0f; i++ )
    {
        if( lux >= lux_threshold[i].lux )
        {
            return ( lux_threshold[i].level );
        }
    }

    return  lux_threshold[i].level;
}// GetBackLightLevelFromLux

void DisplayController::SetBackLightLevel( int actualLevel, int newLevel )
{
    int            steps          = abs( actualLevel - newLevel );
    int            levelIncrement = ( ( actualLevel > newLevel ) ? -1 : 1 );
    IpcBackLight_t backlight;

    BOSE_LOG( INFO, "set actual level: " << actualLevel << " new level: " << newLevel );

    if ( ( actualLevel < 0 ) || ( actualLevel > 100 ) )
    {
        BOSE_LOG( ERROR, "invalid actual back light level: "  << actualLevel );
        return;
    }

    if ( (newLevel < 0) || (newLevel > 100) )
    {
        BOSE_LOG( ERROR, "invalid new back light level: "  << newLevel );
        return;
    }

    for( int i = 0; i < steps; i++ )
    {
        actualLevel += levelIncrement;

        BOSE_LOG( INFO, " level: " << actualLevel );

        backlight.set_value( actualLevel );
        m_lpmClient->SetBackLight( backlight );
        usleep( CHANGING_LEVEL_SLEEP_MS * 1000 );
    }// for all the level steps

    if( actualLevel != newLevel )
    {
        BOSE_LOG( WARNING, "Warning: adjusting actual level: " << actualLevel << ", new level: " << newLevel );

        backlight.set_value( newLevel );
        m_lpmClient->SetBackLight( backlight );
    }
}// SetBackLightLevel

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void DisplayController::MonitorLightSensor()
{
    float previous_lux   = 0;
    int   targeted_level = 0;

    m_luxValue      = 0.0f;
    m_luxDecimal    = 0;
    m_luxFractional = 0;
    m_backLight     = 0;

    // This will activate the light sensor peripheral, the first reading
    // is always zero until the peripheral is activated in continuous
    // reading
    m_lpmClient->GetLightSensor( [this]( IpcLightSensor_t const & rsp ) {} );

    while( ! m_timeToStop )
    {
        m_lpmClient->GetLightSensor( [this]( IpcLightSensor_t const & rsp )
        {
            m_luxDecimal    = ( int )( be16toh( rsp.lux_decimal_value() ) );
            m_luxFractional = ( int )( be16toh( rsp.lux_fractional_value() ) );
        } );

        m_luxValue = ( ( ( float ) m_luxDecimal ) + ( ( ( float )m_luxFractional ) * 0.001f ) ) * m_luxFactor;

        m_lpmClient->GetBackLight( [this]( IpcBackLight_t const & rsp )
        {
            m_backLight = rsp.value();
        } );

        if ( ( m_backLight < 0 )  || ( m_backLight > 100 ) )
        {
            BOSE_LOG ( WARNING, "invalid back light level read: " << m_backLight );
            SetBackLightLevel ( 50, 49 );
        }

        BOSE_LOG( INFO,  "lux(raw, adj, prev): (" << m_luxDecimal    << "."
                  << m_luxFractional << ", "
                  << m_luxValue      << ", "
                  << previous_lux    << ") bl: "
                  << m_backLight     << ( ( m_luxValue - previous_lux ) < 0.0f ?  " lowering" : " rising" ) );

        if( m_autoMode )
        {
            targeted_level = GetBackLightLevelFromLux( m_luxValue, m_luxValue - previous_lux );

            BOSE_LOG( INFO, "target level: " << targeted_level << ", actual level: " << m_backLight );

            if( abs( targeted_level - m_backLight ) >= BACKLIGHT_DIFF_THRESHOLD )
            {
                SetBackLightLevel( m_backLight , targeted_level );
                // dummy read of the back light, the IPC mechanism is caching a value
                m_lpmClient->GetBackLight( [this]( IpcBackLight_t const & rsp ) {} );
                m_backLight = targeted_level;
            }

            previous_lux = m_luxValue;
        }// If we are in automatic mode

        usleep( MONITOR_SENSOR_SLEEP_MS * 1000 );
    }// while it's not time to stop
}// MonitorLightSensor

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void DisplayController::Initialize()
{
    //ui/display end point registration with front door
    RegisterDisplayEndPoints();

    m_threadMonitorLightSensor = std::unique_ptr<std::thread>( new std::thread( [this] { MonitorLightSensor(); } ) );
}// Initialize

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void DisplayController::RegisterDisplayEndPoints()
{
    auto backLightCallBack = [this]( IpcBackLight_t arg )
    {
        HandleLpmNotificationBackLight( arg );
    };

    auto lightSensorCallBack = [this]( IpcLightSensor_t arg )
    {
        HandleLpmNotificationLightSensor( arg );
    };

    AsyncCallback<IpcBackLight_t  > notification_cb_back_light( backLightCallBack  , m_productController.GetTask() );
    AsyncCallback<IpcLightSensor_t> notification_cb_light_sensor( lightSensorCallBack, m_productController.GetTask() );

    m_lpmClient->RegisterEvent<IpcBackLight_t  >( IPC_PER_GET_BACKLIGHT  , notification_cb_back_light );
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
    resp.Send( GetDisplay() );
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
