////////////////////////////////////////////////////////////////////////////////
///// @file   DisplayController.cpp
///// @brief  Implements Eddie Display controller class.
/////
///// @attention Copyright 2017 Bose Corporation, Framingham, MA
//////////////////////////////////////////////////////////////////////////////////

#include <limits.h>
#include <float.h>
#include <functional>
#include <json/json.h>
#include "SystemUtils.h"
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

// The LUX values are in real lux, without the light reading device attenuation
static std::vector<t_luxBacklightTuple> lowering_lux_threshold =
{
    {387.00, BACK_LIGHT_LEVEL_BRIGHT_HIGH},// BRIGHT HIGH
    {195.50, BACK_LIGHT_LEVEL_BRIGHT     },
    { 90.00, BACK_LIGHT_LEVEL_MEDIUM_HIGH},
    { 45.00, BACK_LIGHT_LEVEL_MEDIUM     },
    { 15.30, BACK_LIGHT_LEVEL_MEDIUM_LOW },
    {  4.50, BACK_LIGHT_LEVEL_DIM_HIGH   },
    {  0.00, BACK_LIGHT_LEVEL_DIM        } // DIM LOW
};

static std::vector<t_luxBacklightTuple> rising_lux_threshold =
{
    {430.00, BACK_LIGHT_LEVEL_BRIGHT_HIGH},// BRIGHT HIGH
    {215.00, BACK_LIGHT_LEVEL_BRIGHT     },
    {100.00, BACK_LIGHT_LEVEL_MEDIUM_HIGH},
    { 50.00, BACK_LIGHT_LEVEL_MEDIUM     },
    { 17.00, BACK_LIGHT_LEVEL_MEDIUM_LOW },
    {  5.00, BACK_LIGHT_LEVEL_DIM_HIGH   },
    {  0.00, BACK_LIGHT_LEVEL_DIM        } // DIM LOW
};

static constexpr char  DISPLAY_CONTROLLER_FILE_NAME   [] = "/opt/Bose/etc/display_controller.json";
static constexpr char  JSON_TOKEN_DISPLAY_CONTROLLER  [] = "DisplayController"                    ;
static constexpr char  JSON_TOKEN_DEVICE_MODE         [] = "Mode"                                 ;
static constexpr char  JSON_TOKEN_DEVICE_ABSORTION_LUX[] = "DeviceAbsortionLux"                   ;
static constexpr char  JSON_TOKEN_BACK_LIGHT_LEVELS   [] = "BackLightLevelsPercent"               ;
static constexpr char  JSON_TOKEN_LOWERING_THRESHOLDS [] = "LoweringThresholdLux"                 ;
static constexpr char  JSON_TOKEN_RISING_THRESHOLDS   [] = "RisingThresholdLux"                   ;
static constexpr int   MONITOR_SENSOR_SLEEP_MS           = 1000 ;
static constexpr int   CHANGING_LEVEL_SLEEP_MS           = 10   ;
static constexpr float LUX_DIFF_THRESHOLD                = 2.0f ;
static constexpr float PLEXI_LUX_FACTOR                  = 1.0f ;
static constexpr float SILVER_LUX_FACTOR                 = 11.0f;
static constexpr float BLACK_LUX_FACTOR                  = 16.0f;

DisplayController::DisplayController( ProductController& controller, const std::shared_ptr<FrontDoorClientIF>& fd_client, LpmClientIF::LpmClientPtr clientPtr ):
    m_productController( controller ),
    m_frontdoorClientPtr( fd_client ),
    m_lpmClient( clientPtr ),
    m_timeToStop( false ),
    m_autoMode( true )
{
    ParseJSONData();
}// constructor

DisplayController::~DisplayController()
{
    m_timeToStop = true;

    if( m_threadMonitorLightSensor )
    {
        m_threadMonitorLightSensor->join();
    }
}// destructor

void DisplayController::ParseJSONData()
{
    auto f = SystemUtils::ReadFile( DISPLAY_CONTROLLER_FILE_NAME );

    m_luxFactor = SILVER_LUX_FACTOR;

    if( ! f )
    {
        BOSE_LOG( WARNING, "Warning: can't find file: " << DISPLAY_CONTROLLER_FILE_NAME );
        return;
    }

    Json::Reader json_reader;
    Json::Value  json_root;

    if( ! json_reader.parse( *f, json_root ) )
    {
        BOSE_LOG( ERROR, "Error: failed to parse JSON File: " << DISPLAY_CONTROLLER_FILE_NAME << " " << json_reader.getFormattedErrorMessages() );
        return;
    }

    if( ! json_root[JSON_TOKEN_DISPLAY_CONTROLLER].isMember( JSON_TOKEN_DEVICE_ABSORTION_LUX ) )
    {
        BOSE_LOG( ERROR, "Error: " << JSON_TOKEN_DEVICE_ABSORTION_LUX << " is not a member of: " << JSON_TOKEN_DISPLAY_CONTROLLER );
        return;
    }

    if( ! json_root[JSON_TOKEN_DISPLAY_CONTROLLER].isMember( JSON_TOKEN_DEVICE_MODE ) )
    {
        BOSE_LOG( ERROR, "Error: " << JSON_TOKEN_DEVICE_MODE << " is not a member of: " << JSON_TOKEN_DISPLAY_CONTROLLER );
        return;
    }

    if( ! json_root[JSON_TOKEN_DISPLAY_CONTROLLER].isMember( JSON_TOKEN_BACK_LIGHT_LEVELS ) )
    {
        BOSE_LOG( ERROR, "Error: " << JSON_TOKEN_BACK_LIGHT_LEVELS << " is not a member of: " << JSON_TOKEN_DISPLAY_CONTROLLER );
        return;
    }

    if( ! json_root[JSON_TOKEN_DISPLAY_CONTROLLER].isMember( JSON_TOKEN_LOWERING_THRESHOLDS ) )
    {
        BOSE_LOG( ERROR, "Error: " << JSON_TOKEN_LOWERING_THRESHOLDS << " is not a member of: " << JSON_TOKEN_DISPLAY_CONTROLLER );
        return;
    }

    if( ! json_root[JSON_TOKEN_DISPLAY_CONTROLLER].isMember( JSON_TOKEN_RISING_THRESHOLDS ) )
    {
        BOSE_LOG( ERROR, "Error: " << JSON_TOKEN_RISING_THRESHOLDS << " is not a member of: " << JSON_TOKEN_DISPLAY_CONTROLLER );
        return;
    }

    Json::Value  json_back_light_level   = json_root[JSON_TOKEN_DISPLAY_CONTROLLER][JSON_TOKEN_BACK_LIGHT_LEVELS  ];
    Json::Value  json_lowering_threshold = json_root[JSON_TOKEN_DISPLAY_CONTROLLER][JSON_TOKEN_LOWERING_THRESHOLDS];
    Json::Value  json_rising_threadhold  = json_root[JSON_TOKEN_DISPLAY_CONTROLLER][JSON_TOKEN_RISING_THRESHOLDS  ];
    unsigned int nb_threshold_levels     = lowering_lux_threshold.size();

    if( json_back_light_level.size() != nb_threshold_levels )
    {
        BOSE_LOG( ERROR, "Error: not enough elements in " << JSON_TOKEN_BACK_LIGHT_LEVELS << " expected: " << nb_threshold_levels << " found: " << json_back_light_level.size() );
        return;
    }

    if( json_lowering_threshold.size() != nb_threshold_levels )
    {
        BOSE_LOG( ERROR, "Error: not enough elements in " << JSON_TOKEN_LOWERING_THRESHOLDS << " expected: " << nb_threshold_levels << " found: " << json_lowering_threshold.size() );
        return;
    }

    if( json_rising_threadhold.size() != nb_threshold_levels )
    {
        BOSE_LOG( ERROR, "Error: not enough elements in " << JSON_TOKEN_RISING_THRESHOLDS << " expected: " << nb_threshold_levels << " found: " << json_rising_threadhold.size() );
        return;
    }

    for( unsigned int i = 0; i < nb_threshold_levels; i++ )
    {
        lowering_lux_threshold[i] = t_luxBacklightTuple{json_lowering_threshold[i].asFloat(), ( e_backLightLevel )json_back_light_level[i].asUInt()};
        rising_lux_threshold  [i] = t_luxBacklightTuple{json_rising_threadhold [i].asFloat(), ( e_backLightLevel )json_back_light_level[i].asUInt()};
    }

    m_luxFactor = json_root[JSON_TOKEN_DISPLAY_CONTROLLER][JSON_TOKEN_DEVICE_ABSORTION_LUX].asFloat();
    m_autoMode  = strcasecmp( json_root[JSON_TOKEN_DISPLAY_CONTROLLER][JSON_TOKEN_DEVICE_MODE].asString().c_str(), "Auto" ) == 0 ? true : false;

    std::stringstream lowering_ss;
    std::stringstream rising_ss;

    for( unsigned int i = 0; i < nb_threshold_levels; i++ )
    {
        lowering_ss << "{" << lowering_lux_threshold[i].lux << ", " <<  lowering_lux_threshold[i].level << "} ";
        rising_ss   << "{" << rising_lux_threshold  [i].lux << ", " <<  rising_lux_threshold  [i].level << "} ";
    }

    BOSE_LOG( INFO, "Device absortion factor: " << m_luxFactor                        <<
              " mode: "                   << ( m_autoMode ? "Auto" : "Manual" ) <<
              " lowering threshold: "     << lowering_ss.str()                  <<
              " rising threshold: "       << rising_ss.str() );

}// ParseJSONData

int DisplayController::GetBackLightLevelFromLux( float lux, float lux_rising )
{
    std::vector <t_luxBacklightTuple> lux_threshold = ( lux_rising > 0.0f ) ? rising_lux_threshold : lowering_lux_threshold;
    int                               nb_threshold  = lux_threshold.size();

    for( int i = 0; i < nb_threshold; i++ )
    {
        if( lux >= lux_threshold[i].lux )
        {
            return ( lux_threshold[i].level );
        }
    }

    return  lux_threshold.back().level;
}// GetBackLightLevelFromLux

void DisplayController::SetBackLightLevel( int actualLevel, int newLevel )
{
    int            steps          = abs( actualLevel - newLevel );
    int            levelIncrement = ( ( actualLevel > newLevel ) ? -1 : 1 );
    IpcBackLight_t backlight;

    BOSE_LOG( INFO, "set actual level: " << actualLevel << " new level: " << newLevel );

    if( ( actualLevel < 0 ) || ( actualLevel > 100 ) )
    {
        BOSE_LOG( ERROR, "invalid actual back light level: "  << actualLevel );
        return;
    }

    if( ( newLevel < 0 ) || ( newLevel > 100 ) )
    {
        BOSE_LOG( ERROR, "invalid new back light level: "  << newLevel );
        return;
    }

    for( int i = 0; i < steps; i++ )
    {
        actualLevel += levelIncrement;

        //BOSE_LOG( INFO, " level: " << actualLevel );

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
    float previous_lux   = FLT_MAX;
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

        if( ( m_luxValue != 0.0 ) && ( previous_lux == FLT_MAX ) )
        {
            previous_lux = m_luxValue;
        }

        m_lpmClient->GetBackLight( [this]( IpcBackLight_t const & rsp )
        {
            m_backLight = rsp.value();
        } );

        if( ( m_backLight < 0 )  || ( m_backLight > 100 ) )
        {
            BOSE_LOG( WARNING, "invalid back light level read: " << m_backLight );
            SetBackLightLevel( 50, 49 );
        }

        float lux_diff = m_luxValue - previous_lux;
        BOSE_LOG( INFO,  "lux(raw, adj, prev): ("
                  << m_luxDecimal    << "."
                  << m_luxFractional << ", "
                  << m_luxValue      << ", "
                  << previous_lux    << ") bl: "
                  << m_backLight     << ( ( lux_diff == 0.0f ) ? " level" : ( lux_diff  < 0.0f ?  " lowering" : " rising" ) ) );

        if( m_autoMode )
        {
            targeted_level = GetBackLightLevelFromLux( m_luxValue, m_luxValue - previous_lux );

            BOSE_LOG( INFO, "target level: " << targeted_level << ", actual level: " << m_backLight );

            if( fabs( previous_lux - m_luxValue ) >= LUX_DIFF_THRESHOLD )
            {
                SetBackLightLevel( m_backLight , targeted_level );
                // dummy read of the back light, the IPC mechanism is caching a value
                m_lpmClient->GetBackLight( [this]( IpcBackLight_t const & rsp ) {} );
                m_backLight  = targeted_level;
                previous_lux = m_luxValue;
            }

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
