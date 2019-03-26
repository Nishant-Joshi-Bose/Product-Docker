////////////////////////////////////////////////////////////////////////////////
///// @file   DisplayController.cpp
///// @brief  Implements produc controller DisplayController class.
/////
///// @attention Copyright 2017 Bose Corporation, Framingham, MA
//////////////////////////////////////////////////////////////////////////////////

#include <sys/stat.h>
#include <fcntl.h>

#include <limits.h>
#include <float.h>
#include <functional>
#include <iostream>
#include <fstream>
#include <json/json.h>

#include "APTaskFactory.h"
#include "BreakThread.h"
#include "DirUtils.h"
#include "SystemUtils.h"
#include "FrontDoorClient.h"
#include "DisplayController.h"
#include "ProductController.h"
#include "LpmClientFactory.h"
#include "SyncCallback.h"
#include "StringUtils.h"
#include "EndPointsDefines.h"
#include "MonotonicClock.h"

static DPrint s_logger( "DisplayController" );
using namespace ::DisplayControllerPb;

namespace ProductApp
{

/*! \brief Levels of intensity for the backlight.
 * These are triggered based off the lux values from the lowering and rising tables.
 */
static std::vector<uint8_t> s_backLightLevels =
{
    100,
    60,
    40,
    25,
    20,
    10,
    2
};

/*! \brief Lower thresholds for setting backlight level from lux.
 * The LUX values are in real lux, without the light reading device attenuation
 */
static std::vector<float> s_loweringLuxThreshold =
{
    387.00,
    195.50,
    90.00,
    45.00,
    15.30,
    4.50,
    0.00
};

/*! \brief Upper thresholds for setting backlight level from lux.
 * The LUX values are in real lux, without the light reading device attenuation
 */
static std::vector<float> s_risingLuxThreshold =
{
    430.00,
    215.00,
    100.00,
    50.00,
    17.00,
    5.00,
    0.00
};

static constexpr char DISPLAY_CONTROLLER_FILE_NAME   [] = "/opt/Bose/etc/display_controller.json";
static constexpr char FRAME_BUFFER_BLACK_SCREEN      [] = "black_screen";
static constexpr char EDDIE_LCD_FRAME_BUFFER_DIR     [] = "/sys/devices/soc/7af6000.spi/spi_master/spi6/spi6.1/graphics/fb1/";
static constexpr char JSON_TOKEN_DISPLAY_CONTROLLER  [] = "DisplayController";
static constexpr char JSON_TOKEN_BACK_LIGHT_LEVELS   [] = "BackLightLevelsPercent";
static constexpr char JSON_TOKEN_LOWERING_THRESHOLDS [] = "LoweringThresholdLux";
static constexpr char JSON_TOKEN_RISING_THRESHOLDS   [] = "RisingThresholdLux";

static constexpr char UIBRIGHTNESS_MODE_NAME_AUTO[]     = "automatic";
static constexpr char UIBRIGHTNESS_MODE_NAME_DEFAULT[]  = "default";
static constexpr char UIBRIGHTNESS_MODE_NAME_MANUAL[]   = "manual";

static constexpr char FRONTDOOR_ENDPOINT_ALIVE[]        = "/ui/alive";
static constexpr char FRONTDOOR_ENDPOINT_BRIGHTNESS[]   = "/ui/lcd/brightness";

static constexpr char UI_PID_FILE_NAME[]                = "/var/run/monaco.pid";

static constexpr int   UPDATE_SLEEP_MS                  = 100;
static constexpr int   SEND_DEFAULTS_TO_LPM_RETRY_MS    = 2000;
static constexpr uint8_t BRIGHTNESS_MAX                 = 100;
static constexpr uint8_t BRIGHTNESS_MIN                 = 0;

static constexpr uint16_t SCREEN_BLACK_RAMP_OFF_MS      = 5000;     //!< Duration (MS) of backlight turn off when screen is black.
static constexpr uint16_t SCREEN_BLACK_RAMP_ON_MS       = 500;      //!< Duration (MS) of backlight turn on when screen is not black.

//! Warning will be generated if received UI heartbeat counts are out of sync by this count.
static constexpr uint8_t UI_HEART_BEAT_MISSED_THRESHOLD = 2;

//! Number of ticks that must elapse without receiving a UI tick before we warn about it.
#define UI_HEART_BEAT_TIME_WARNING_TICKS_DEFAULT  300  // This * UPDATE_SLEEP_MS = time in MS.

//! Number of ticks that must elapse without receiving a UI tick before we consider UI disconnected.
#define UI_HEART_BEAT_TIME_ERROR_TICKS_DEFAULT 600  // This * UPDATE_SLEEP_MS = time in MS.

//! SysFS file for checking state of screen black.
const std::string BLACK_SCREEN_FILE_NAME = std::string( EDDIE_LCD_FRAME_BUFFER_DIR ) + FRAME_BUFFER_BLACK_SCREEN;

//! Number of update ticks after which to respond to a screen blank.
static constexpr uint64_t SCREEN_BLANK_DEBOUNCE_COUNTS[DisplayController::ScreenBlackState_Count] =
{
    200, // ScreenBlackState_Black
    1,  // ScreenBlackState_NotBlack
};

DisplayController::DisplayController( const Configuration& config,
                                      ProductController& controller,
                                      const std::shared_ptr<FrontDoorClientIF>& fdClient,
                                      LpmClientIF::LpmClientPtr clientPtr,
                                      AsyncCallback<bool> uiConnectedCb ):
    m_config( config ),
    m_productController( controller ),
    m_frontdoorClientPtr( fdClient ),
    m_lpmClient( clientPtr ),
    m_defaultsSentToLpm( false ),
    m_defaultsSentTime( 0 ),
    m_lcdStandbyBrightnessCap( 50 ), // Sensible default. Real default loaded from JSON.
    m_lcdBrightnessCap( BRIGHTNESS_MAX ),
    m_screenBlackState( ScreenBlackState_Disabled ),
    m_screenBlackChangeTo( ScreenBlackState_Invalid ),
    m_screenBlackInactivityTicks( 0 ),
    m_screenBlackChangeCounter( 0 ),
    m_currentTick( 0 ),
    m_task( IL::CreateTask( "DisplayControllerTask" ) ),
    m_uiHeartBeat( ULLONG_MAX ),
    m_lastUiHeartBeatTick( 0 ),
    m_uiHeartBeatLossWarnTicks( UI_HEART_BEAT_TIME_WARNING_TICKS_DEFAULT ),
    m_uiHeartBeatLossErrorTicks( UI_HEART_BEAT_TIME_ERROR_TICKS_DEFAULT ),
    m_uiConnected( false ),
    m_ProductControllerUiConnectedCb( uiConnectedCb )
{

}

DisplayController::~DisplayController()
{
    m_updateloopTimer->Stop();

    if( m_task != NULL )
    {
        IL::StopTask( m_task );
        IL::JoinTask( m_task );
    }
}

void DisplayController::Initialize()
{
    // Read JSON configuration.
    bool jsonReadSuccess = ParseJSONData();

    if( ! jsonReadSuccess )
    {
        m_defaultsSentToLpm = true; // Update loop will never send the defaults if this is true.

        BOSE_WARNING( s_logger, "JSON load from '%s' failed. Not sending settings to LPM.",
                      DISPLAY_CONTROLLER_FILE_NAME );
    }

    // ui/display end point registration with front door
    RegisterFrontdoorEndPoints();

    // Initial screen state.
    if( IsBlackScreenDetectSupported() )
    {
        if( DirUtils::DoesFileExist( BLACK_SCREEN_FILE_NAME ) == false )
        {
            BOSE_LOG( WARNING, "warning: can't find file: " + BLACK_SCREEN_FILE_NAME
                      + ", update your kernel for black screen detection" );
            m_config.m_blackScreenDetectEnabled = false;
        }

        // Initial poll of screen black state is set in UpdateUiConnected().
        // "Disabled" will keep the back light in boot state (off).
        m_screenBlackState = ScreenBlackState_Disabled;
    }

    // Background task initialization.
    m_updateloopTimer = APTimer::Create( m_task, "DisplayControllerUpdateTimer" );
    m_updateloopTimer->SetTimeouts( UPDATE_SLEEP_MS, 0 );
    m_updateloopTimer->Start( [ this ]( )
    {
        UpdateLoop();
    } );
}

bool DisplayController::ParseJSONData()
{
    BOSE_DEBUG( s_logger, "%s", __FUNCTION__ );

    auto f = SystemUtils::ReadFile( DISPLAY_CONTROLLER_FILE_NAME );

    if( ! f )
    {
        BOSE_LOG( WARNING, "Can't find file: " << DISPLAY_CONTROLLER_FILE_NAME );
        return false;
    }

    static Json::CharReaderBuilder readerBuilder;
    std::unique_ptr<Json::CharReader> json_reader( readerBuilder.newCharReader() );
    std::string errors;
    Json::Value  json_root;
    std::string s = *f;

    if( ! json_reader->parse( s.c_str(), s.c_str() + s.size(), &json_root, &errors ) )
    {
        BOSE_LOG( WARNING, "Error: failed to parse JSON File: " << DISPLAY_CONTROLLER_FILE_NAME
                  << " " << errors.c_str() );
        return false;
    }

    if( m_config.m_hasLightSensor && m_config.m_hasLcd )
    {
        if( ! json_root[JSON_TOKEN_DISPLAY_CONTROLLER].isMember( JSON_TOKEN_BACK_LIGHT_LEVELS ) )
        {
            BOSE_LOG( WARNING, "Error: " << JSON_TOKEN_BACK_LIGHT_LEVELS << " is not a member of: "
                      << JSON_TOKEN_DISPLAY_CONTROLLER );
            return false;
        }

        if( ! json_root[JSON_TOKEN_DISPLAY_CONTROLLER].isMember( JSON_TOKEN_LOWERING_THRESHOLDS ) )
        {
            BOSE_LOG( WARNING, "Error: " << JSON_TOKEN_LOWERING_THRESHOLDS << " is not a member of: "
                      << JSON_TOKEN_DISPLAY_CONTROLLER );
            return false;
        }

        if( ! json_root[JSON_TOKEN_DISPLAY_CONTROLLER].isMember( JSON_TOKEN_RISING_THRESHOLDS ) )
        {
            BOSE_LOG( WARNING, "Error: " << JSON_TOKEN_RISING_THRESHOLDS << " is not a member of: "
                      << JSON_TOKEN_DISPLAY_CONTROLLER );
            return false;
        }

        Json::Value  json_back_light_level   = json_root[JSON_TOKEN_DISPLAY_CONTROLLER][JSON_TOKEN_BACK_LIGHT_LEVELS  ];
        Json::Value  json_lowering_threshold = json_root[JSON_TOKEN_DISPLAY_CONTROLLER][JSON_TOKEN_LOWERING_THRESHOLDS];
        Json::Value  json_rising_threadhold  = json_root[JSON_TOKEN_DISPLAY_CONTROLLER][JSON_TOKEN_RISING_THRESHOLDS  ];
        unsigned int nb_threshold_levels     = json_back_light_level.size();

        if( json_lowering_threshold.size() != nb_threshold_levels
            || json_lowering_threshold.size() > IPC_MAX_LIGHT_SENSOR_THRESHOLD )
        {
            BOSE_LOG( WARNING, "Error: wrong # of elements in " << JSON_TOKEN_LOWERING_THRESHOLDS
                      << " expected: " << nb_threshold_levels
                      << " found: " << json_lowering_threshold.size()
                      << " max: " << IPC_MAX_LIGHT_SENSOR_THRESHOLD );
            return false;
        }

        if( json_rising_threadhold.size() != nb_threshold_levels
            || json_rising_threadhold.size() > IPC_MAX_LIGHT_SENSOR_THRESHOLD )
        {
            BOSE_LOG( WARNING, "Error: wrong # of elements in " << JSON_TOKEN_RISING_THRESHOLDS
                      << " expected: " << nb_threshold_levels
                      << " found: " << json_rising_threadhold.size()
                      << " max: " << IPC_MAX_LIGHT_SENSOR_THRESHOLD );
            return false;
        }

        for( unsigned int i = 0; i < nb_threshold_levels; i++ )
        {
            s_backLightLevels[i]        = json_back_light_level[i].asUInt();
            s_loweringLuxThreshold[i]   = json_lowering_threshold[i].asFloat();
            s_risingLuxThreshold[i]     = json_rising_threadhold[i].asFloat();
        }

        //
        // Print the levels for debugging.
        //

        for( unsigned int i = 0; i < nb_threshold_levels; i++ )
        {
            BOSE_DEBUG( s_logger, "%s level %i: backlight %d, lowering %f rising %f",
                        __FUNCTION__, i, s_backLightLevels[i], s_loweringLuxThreshold[i], s_risingLuxThreshold[i] );
        }
    }

    //
    // LCD and lightbar Brightness defaults.
    //

    if( m_config.m_hasLcd && json_root.isMember( "lcdStandbyBrightnessCap" ) )
    {
        m_lcdStandbyBrightnessCap = json_root["lcdStandbyBrightnessCap"].asUInt();
    }

    if( IsBlackScreenDetectSupported() && json_root.isMember( "lcdInactivityBacklightOffDebounceTime" ) )
    {
        m_screenBlackInactivityTicks = ( 1000 * json_root["lcdInactivityBacklightOffDebounceTime"].asUInt() ) / UPDATE_SLEEP_MS;
    }

    if( m_config.m_hasLcd && ! ParseBrightnessData( &m_lcdBrightness, json_root, "lcdBrightness" ) )
    {
        return false;
    }

    if( ! ParseBrightnessData( &m_lightbarBrightness, json_root, "lightbarBrightness" ) )
    {
        return false;
    }

    //
    // Others
    //

    if( json_root.isMember( "killUIOnHeartBeatLossPidFile" ) )
    {
        m_killUIOnHeartBeatLossPidFile = json_root["killUIOnHeartBeatLossPidFile"].asString();
    }

    if( json_root.isMember( "uiHeartBeatLossWarnMS" ) )
    {
        m_uiHeartBeatLossWarnTicks = json_root["uiHeartBeatLossWarnMS"].asUInt() / UPDATE_SLEEP_MS;
    }

    if( json_root.isMember( "uiHeartBeatLossErrorMS" ) )
    {
        m_uiHeartBeatLossErrorTicks = json_root["uiHeartBeatLossErrorMS"].asUInt() / UPDATE_SLEEP_MS;
    }

    BOSE_DEBUG( s_logger, "UI heartbeat ticks warning: %llu, error: %llu",
                m_uiHeartBeatLossWarnTicks, m_uiHeartBeatLossErrorTicks );

    return true;
}

bool DisplayController::ParseBrightnessData( Brightness* output, const Json::Value& rootNode, std::string nodeName )
{
    Json::Value brightnessRoot = rootNode[nodeName];

    if( brightnessRoot.isNull() )
    {
        BOSE_ERROR( s_logger, "No '%s' settings block found in '%s'.",
                    nodeName.c_str(), DISPLAY_CONTROLLER_FILE_NAME );
        return false;
    }

    try
    {
        Json::StreamWriterBuilder builder;
        builder.settings_["indentation"] = "";
        std::string nodeStr = Json::writeString( builder, brightnessRoot );
        ProtoToMarkup::FromJson( nodeStr, output, "Brightness" );
    }
    catch( const ProtoToMarkup::MarkupError &e )
    {
        BOSE_WARNING( s_logger, "Failed to read %s node from '%s'. Error: %s",
                      nodeName.c_str(), DISPLAY_CONTROLLER_FILE_NAME, e.what() );
        return false;
    }

    return true;
}

void DisplayController::PushDefaultsToLPM()
{
    static constexpr float FRACTIONAL_PRECISION = 1000.0f;

    IpcLightSensorParams_t defaults;
    IpcUIBrightness_t* lcdBrightness;
    IpcUIBrightness_t* lightbarBrightness;
    size_t entryCount = MIN( s_backLightLevels.size(), ( size_t ) IPC_MAX_LIGHT_SENSOR_THRESHOLD );

    // Lux to level tables.
    for( size_t i = 0; i < entryCount; ++i )
    {
        IpcLightSensorParamsValue_t* aValue;

        defaults.add_vec();
        aValue = defaults.mutable_vec( i );

        aValue->set_backlightlevel( s_backLightLevels[i] );
        aValue->set_loweringthresholddecimal( ( uint16_t ) s_loweringLuxThreshold[i] );
        aValue->set_loweringthresholdfractional( FRACTIONAL_PRECISION * ( s_loweringLuxThreshold[i] - aValue->loweringthresholddecimal() ) );
        aValue->set_risingthresholddecimal( ( uint16_t ) s_risingLuxThreshold[i] );
        aValue->set_risingthresholdfractional( FRACTIONAL_PRECISION * ( s_risingLuxThreshold[i] - aValue->risingthresholddecimal() ) );
    }

    // Human detection values.
    defaults.set_fastrisingluxdelta( 0 );
    defaults.set_fastfaillingluxdelta( 0 );
    defaults.set_slowrisinghysteresis( 0 );
    defaults.set_fastrisinghysteresis( 0 );
    defaults.set_slowfaillinghysteresis( 0 );
    defaults.set_fastfaillinghysteresis( 0 );

    // LCD UI Brightness.
    defaults.add_uibrightness();
    lcdBrightness = defaults.mutable_uibrightness( 0 );
    BuildLpmUIBrightnessStruct( lcdBrightness, UI_BRIGTHNESS_DEVICE_LCD );

    // Lightbar UI Brightness.
    defaults.add_uibrightness();
    lightbarBrightness = defaults.mutable_uibrightness( 1 );
    BuildLpmUIBrightnessStruct( lightbarBrightness, UI_BRIGTHNESS_DEVICE_LIGHTBAR );

    // Uncomment to print JSON string to log.
    //std::string defaultsString = ProtoToMarkup::ToJson( defaults, false );
    //BOSE_INFO( s_logger, "%s: %s", __FUNCTION__, defaultsString.c_str() );

    //
    // Send to LPM.
    //

    Callback<IpcLpmGenericResponse_t> setParamsCb( std::bind( &DisplayController::HandleLpmSetLightSensorParams, this, std::placeholders::_1 ) );
    AsyncCallback<const IpcLpmGenericResponse_t> setParamsAsync( setParamsCb, m_task );

    auto f = [this, defaults, setParamsAsync]()
    {
        IpcLightSensorParams_t params = defaults;
        m_lpmClient->SetLightSensorParams( params, setParamsAsync );
    };
    IL::BreakThread( f, m_productController.GetTask() );
}

void DisplayController::UpdateUiConnected( bool currentUiConnectedStatus )
{
    if( m_uiConnected != currentUiConnectedStatus )
    {
        BOSE_WARNING( s_logger, "UI status is now: %s", ( currentUiConnectedStatus ) ? "CONNECTED" : "DISCONNECTED" );
        m_uiConnected = currentUiConnectedStatus;
        m_ProductControllerUiConnectedCb( currentUiConnectedStatus );

        // Set initial black screen state. Black screen detection will be disabled until
        // the member variable is set to something other than ScreenBlackState_Disabled.
        // "Invalid" will force a re-detection.
        if( IsBlackScreenDetectSupported() )
        {
            m_screenBlackState = ScreenBlackState_Invalid;
        }
        // No black screen detection? Turn on the display now.
        else
        {
            SetDisplayBrightnessCap( m_lcdBrightnessCap, UI_BRIGHTNESS_TIME_DEFAULT );
        }

        PullUIBrightnessFromLpm( UI_BRIGTHNESS_DEVICE_LCD );
    }
}

void DisplayController::SetDisplayBrightnessCap( uint8_t capPercent, uint16_t time )
{
    BOSE_DEBUG( s_logger, "%s, hasLcd %i", __FUNCTION__, m_config.m_hasLcd );

    if( !m_config.m_hasLcd )
    {
        return;
    }

    auto f = [this, capPercent, time]()
    {
        IpcUIBrightness_t lpmBrightness;

        lpmBrightness.set_device( UI_BRIGTHNESS_DEVICE_LCD );
        lpmBrightness.set_value( capPercent );
        lpmBrightness.set_mode( UI_BRIGTHNESS_MODE_CAP_MAXIMUM );
        lpmBrightness.set_time( time );

        m_lpmClient->SetUIBrightness( lpmBrightness );
    };
    IL::BreakThread( f, m_productController.GetTask() );
}

/*!
 * Any value set through this function is meant to be authoritavite over a something
 * we get from Frontdoor. This is to ensure that if the embedded system needs to
 * conserve power (for regulation) the brightness will stay capped.
 */
void DisplayController::SetStandbyLcdBrightnessCapEnabled( bool enabled )
{
    BOSE_DEBUG( s_logger, "SetStandbyLcdBrightnessCapEnabled enabled %i, hasLcd %i", enabled, m_config.m_hasLcd );

    if( !m_config.m_hasLcd )
    {
        return;
    }

    auto f = [this, enabled]()
    {
        m_lcdBrightnessCap = enabled ? m_lcdStandbyBrightnessCap : BRIGHTNESS_MAX;

        // Cap (to 0) from screen black takes precedence.
        if( m_screenBlackState == ScreenBlackState_NotBlack )
        {
            SetDisplayBrightnessCap( m_lcdBrightnessCap, UI_BRIGHTNESS_TIME_DEFAULT );
        }
    };

    IL::BreakThread( f, m_task );
}

void DisplayController::UpdateLoop()
{
    m_currentTick++;

    //
    // Display/Monaco heartbeat.
    //

    ProcessUiHeartBeat();

    //
    // Blank screen detection.
    //

    if( IsBlackScreenDetectSupported() )
    {
        ProcessBlackScreenDetection();
    }

    //
    // Display settings send to LPM.
    //
    if( ( m_config.m_hasLightSensor && m_config.m_hasLcd )
        && ! m_defaultsSentToLpm
        // Transfer and processing on LPM can take time. Throttle this.
        && MonotonicClock::NowMs() - m_defaultsSentTime > SEND_DEFAULTS_TO_LPM_RETRY_MS )
    {
        m_defaultsSentTime = MonotonicClock::NowMs();

        PushDefaultsToLPM();
    }

    // Schedule the next update.
    m_updateloopTimer->SetTimeouts( UPDATE_SLEEP_MS, 0 );
    m_updateloopTimer->Start( [ this ]( )
    {
        UpdateLoop();
    } );
}

void DisplayController::ProcessUiHeartBeat()
{
    uint64_t deltaTicks = m_currentTick - m_lastUiHeartBeatTick;

    // If we have not heard from the UI in some time, consider it dead.
    if( deltaTicks >= m_uiHeartBeatLossErrorTicks )
    {
        BOSE_ERROR( s_logger, "UI has stopped. Last heartbeat received %0.2fs ago.",
                    ( float )( ( m_currentTick - m_lastUiHeartBeatTick ) * UPDATE_SLEEP_MS ) / 1000.0 );

        m_uiHeartBeat = ULLONG_MAX;
        m_lastUiHeartBeatTick = m_currentTick;  // Reset so we continue to monitor again.
        UpdateUiConnected( false );

        if( ! m_killUIOnHeartBeatLossPidFile.empty() )
        {
            KillUiProcess();
        }

        return;
    }
    else if( deltaTicks == m_uiHeartBeatLossWarnTicks )
    {
        BOSE_WARNING( s_logger, "UI heart beat warning. Last received %0.2fs ago.",
                      ( float )( ( m_currentTick - m_lastUiHeartBeatTick ) * UPDATE_SLEEP_MS ) / 1000.0 );
    }

    // If we have a heartbeat and not already flagged as connected, do so now.
    if( m_uiHeartBeat != ULLONG_MAX && ! m_uiConnected )
    {
        UpdateUiConnected( true );
    }
}

void DisplayController::ProcessBlackScreenDetection()
{
    ScreenBlackState screenState = ScreenBlackState_Invalid;
    screenState = ReadFrameBufferBlackState();

    // Not supported.
    if( screenState == ScreenBlackState_Invalid
        || m_screenBlackState == ScreenBlackState_Disabled )
    {
        return;
    }

    // Debouncing turning off the backlight.
    // Turning the backlight back on is handled immediatle in the next branch.
    if( m_screenBlackChangeCounter > 0
        && screenState == m_screenBlackChangeTo
        && m_screenBlackChangeTo ==  ScreenBlackState_Black )
    {
        m_screenBlackChangeCounter--;

        if( m_screenBlackChangeCounter == 0 )
        {
            BOSE_DEBUG( s_logger, "Screen is black, turning off backlight." );
            SetBlackScreenNowState( m_screenBlackChangeTo );
            SetDisplayBrightnessCap( BRIGHTNESS_MIN, SCREEN_BLACK_RAMP_OFF_MS );
        }
    }
    // Screen is now blank. Wait some time to be sure.
    else if( screenState != m_screenBlackState )
    {
        switch( screenState )
        {
        // When black is detected, turn off the backlight after some time as a debounce.
        case ScreenBlackState_Black:
        {
            if( m_screenBlackInactivityTicks > 0 )
            {
                m_screenBlackChangeTo = screenState;
                m_screenBlackChangeCounter = m_screenBlackInactivityTicks;

                BOSE_DEBUG( s_logger, "Screen black state changed to %i, waiting %llu MS.",
                            screenState, m_screenBlackChangeCounter * UPDATE_SLEEP_MS );
            }
            else
            {
                BOSE_DEBUG( s_logger, "Screen black state changed to %i, but screen blank response is disabled.",
                            screenState );
            }
            break;
        }

        // When the screen is no longer black, respond immediately.
        case ScreenBlackState_NotBlack:
        {
            BOSE_DEBUG( s_logger, "Screen is no longer black, turning on backlight." );
            SetBlackScreenNowState( ScreenBlackState_NotBlack );
            SetDisplayBrightnessCap( MIN( m_lcdBrightnessCap, BRIGHTNESS_MAX ), SCREEN_BLACK_RAMP_ON_MS );
            // Do not use LCD on/off since it boots into an "all white" state which
            // may be momentarily visible.
            break;
        }

        default:
            break;
        }
    }
    // No change.
    else if( m_screenBlackChangeTo != ScreenBlackState_Invalid )
    {
        BOSE_DEBUG( s_logger, "Aborting screen blank state change." );

        m_screenBlackChangeTo = ScreenBlackState_Invalid;
        m_screenBlackChangeCounter = 0;
    }
}

void DisplayController::SetBlackScreenNowState( ScreenBlackState s )
{
    m_screenBlackState = s;
    m_screenBlackChangeTo = ScreenBlackState_Invalid;
    m_screenBlackChangeCounter = 0;
}

void DisplayController::RegisterLpmEvents()
{
    BOSE_DEBUG( s_logger, "%s, hasLcd %i", __FUNCTION__, m_config.m_hasLcd );

    if( !m_config.m_hasLcd )
    {
        return;
    }

    // Register to receive IPC_PER_GET_BACKLIGHT messages from LPM.
    auto backLightCallBack = [this]( IpcBackLight_t arg )
    {
        HandleLpmNotificationBackLight( arg );
    };
    AsyncCallback<IpcBackLight_t> notificationCbBackLight( backLightCallBack, m_productController.GetTask() );
    m_lpmClient->RegisterEvent<IpcBackLight_t>( IPC_PER_GET_BACKLIGHT, notificationCbBackLight );

    // Register to receive IPC_PER_GET_UI_BRIGHTNESS messages from LPM.
    auto uiBrightnessCallBack = [this]( IpcUIBrightness_t arg )
    {
        HandleLpmNotificationUIBrightness( arg );
    };
    AsyncCallback<IpcUIBrightness_t> notificationCbUIBrightness( uiBrightnessCallBack, m_productController.GetTask() );
    m_lpmClient->RegisterEvent<IpcUIBrightness_t>( IPC_PER_GET_UI_BRIGHTNESS, notificationCbUIBrightness );
}

void DisplayController::RegisterFrontdoorEndPoints()
{

    // ==========================
    // HandlePostUIHeartBeat
    // ==========================
    AsyncCallback<UiHeartBeat, Callback<UiHeartBeat>, Callback<FrontDoor::Error>> uiAlivePostCb(
                                                                                   std::bind( &DisplayController::HandlePostUiHeartBeat,
                                                                                           this,
                                                                                           std::placeholders::_1,
                                                                                           std::placeholders::_2
                                                                                            ),
                                                                                   m_productController.GetTask() );
    m_frontdoorClientPtr->RegisterPost<UiHeartBeat>( FRONTDOOR_ENDPOINT_ALIVE, uiAlivePostCb,
                                                     FrontDoor::PUBLIC,
                                                     FRONTDOOR_PRODUCT_CONTROLLER_VERSION,
                                                     FRONTDOOR_PRODUCT_CONTROLLER_GROUP_NAME );

    // ==========================
    // HandlePutUIHeartBeat
    // ==========================
    AsyncCallback<UiHeartBeat, Callback<UiHeartBeat>, Callback<FrontDoor::Error>> uiAlivePutCb(
                                                                                   std::bind( &DisplayController::HandlePutUiHeartBeat,
                                                                                           this,
                                                                                           std::placeholders::_1,
                                                                                           std::placeholders::_2
                                                                                            ),
                                                                                   m_productController.GetTask() );
    m_frontdoorClientPtr->RegisterPut<UiHeartBeat>( FRONTDOOR_ENDPOINT_ALIVE, uiAlivePutCb,
                                                    FrontDoor::PUBLIC,
                                                    FRONTDOOR_PRODUCT_CONTROLLER_VERSION,
                                                    FRONTDOOR_PRODUCT_CONTROLLER_GROUP_NAME );

    // ==========================
    // HandleGetUIHeartBeat
    // ==========================
    AsyncCallback< Callback<UiHeartBeat>, Callback<FrontDoor::Error> > uiAliveGetCb(
        std::bind( &DisplayController::HandleGetUiHeartBeat,
                   this,
                   std::placeholders::_1
                 ),
        m_productController.GetTask() );

    m_frontdoorClientPtr->RegisterGet( FRONTDOOR_ENDPOINT_ALIVE, uiAliveGetCb,
                                       FrontDoor::PUBLIC,
                                       FRONTDOOR_PRODUCT_CONTROLLER_VERSION,
                                       FRONTDOOR_PRODUCT_CONTROLLER_GROUP_NAME );

    //
    // Frontdoor /ui/lcd/brightness
    //
    if( m_config.m_hasLcd )
    {
        // POST
        AsyncCallback<Brightness, Callback<Brightness>, Callback<FrontDoor::Error>> putLcdBrightnessCb(
                                                                                     std::bind( &DisplayController::HandlePutLcdBrightnessRequest, this, std::placeholders::_1, std::placeholders::_2 ),
                                                                                     m_productController.GetTask() );
        m_frontdoorClientPtr->RegisterPut<Brightness>( FRONTDOOR_ENDPOINT_BRIGHTNESS, putLcdBrightnessCb,
                                                       FrontDoor::PUBLIC,
                                                       FRONTDOOR_PRODUCT_CONTROLLER_VERSION,
                                                       FRONTDOOR_PRODUCT_CONTROLLER_GROUP_NAME );

        // GET
        AsyncCallback< Callback<Brightness>, Callback<FrontDoor::Error>> getLcdBrightnessCb(
                                                                          std::bind( &DisplayController::HandleGetLcdBrightnessRequest, this, std::placeholders::_1 ),
                                                                          m_productController.GetTask() );
        m_frontdoorClientPtr->RegisterGet( FRONTDOOR_ENDPOINT_BRIGHTNESS, getLcdBrightnessCb,
                                           FrontDoor::PUBLIC,
                                           FRONTDOOR_PRODUCT_CONTROLLER_VERSION,
                                           FRONTDOOR_PRODUCT_CONTROLLER_GROUP_NAME );
    }

}

void DisplayController::HandlePutDisplayRequest( const Display &req,
                                                 const Callback<Display>& resp )
{
}

void DisplayController::HandleGetDisplayRequest( Callback<Display> resp )
{
    resp.Send( GetDisplay() );
}

void DisplayController::PullUIBrightnessFromLpm( IpcUIBrightnessDevice_t deviceType )
{
    BOSE_DEBUG( s_logger, "%s, hasLcd %i", __FUNCTION__, m_config.m_hasLcd );

    if( !m_config.m_hasLcd )
    {
        return;
    }

    Callback<IpcUIBrightness_t> cb( std::bind( &DisplayController::HandleLpmGetUIBrightness, this, std::placeholders::_1 ) );
    AsyncCallback<const IpcUIBrightness_t> cbAsync( cb, m_task );

    auto f = [this, deviceType, cbAsync]()
    {
        IpcGetUIBrightnessParams_t params;
        params.set_device( deviceType );

        m_lpmClient->GetUIBrightness( params, cbAsync );
    };

    IL::BreakThread( f, m_productController.GetTask() );
}

bool DisplayController::HandleLpmNotificationBackLight( IpcBackLight_t lpmBackLight )
{
    m_display.set_backlightprecentage( lpmBackLight.value() );

    return true;
}

/*! \brief Parse data from incoming IPC_PER_GET_UI_BRIGHTNESS.
 * \param lpmBackLight Data of type IpcUIBrightness_t frm RivieraLPM_IpcProtocol.h.
 */
bool DisplayController::HandleLpmNotificationUIBrightness( IpcUIBrightness_t lpmBrightness )
{
    switch( lpmBrightness.device() )
    {
    case UI_BRIGTHNESS_DEVICE_LCD:
    {
        m_lcdBrightness.set_value( lpmBrightness.value() );
        m_lcdBrightness.set_mode( BrightnessIpcEnumToProtoEnum( ( IpcUIBrightnessMode_t ) lpmBrightness.mode() ) );
        m_frontdoorClientPtr->SendNotification( FRONTDOOR_ENDPOINT_BRIGHTNESS, m_lcdBrightness );
        break;
    }

    case UI_BRIGTHNESS_DEVICE_LIGHTBAR:
    {
        // Not currently supported.
        BOSE_INFO( s_logger, "Received IPC_PER_GET_UI_BRIGHTNESS from LPM which is not currently supported." );
        break;
    }
    }

    return true;
}

void DisplayController::HandlePostUiHeartBeat( const UiHeartBeat &req,
                                               Callback<UiHeartBeat> resp )
{
    BOSE_INFO( s_logger, "Received first heartbeat: %llu", req.count() );

    m_uiHeartBeat = req.count();
    m_lastUiHeartBeatTick = m_currentTick;

    UiHeartBeat response;
    response.set_count( m_uiHeartBeat );
    resp.Send( response );
}

void DisplayController::HandlePutUiHeartBeat( const UiHeartBeat &req,
                                              Callback<UiHeartBeat> resp )
{
    BOSE_VERBOSE( s_logger, "Received heartbeat: %llu, current %llu", req.count(), m_uiHeartBeat );

    if( ( m_uiHeartBeat != ULLONG_MAX )
        && ( abs( m_uiHeartBeat - req.count() ) >= UI_HEART_BEAT_MISSED_THRESHOLD ) )
    {
        BOSE_WARNING( s_logger, "UI is skipping heart beat, received: %llu, current: %llu",
                      req.count(), m_uiHeartBeat );
    }

    m_uiHeartBeat = req.count();
    m_lastUiHeartBeatTick = m_currentTick;

    UiHeartBeat response;
    response.set_count( m_uiHeartBeat );
    resp.Send( response );
}

void DisplayController::HandleGetUiHeartBeat( Callback<UiHeartBeat> resp )
{
    BOSE_LOG( VERBOSE, "received Get heartbeat: " << m_uiHeartBeat );
    UiHeartBeat response;
    response.set_count( m_uiHeartBeat );
    resp.Send( response );
}

Display DisplayController::GetDisplay()
{
    return m_display;
}

const DisplayController::Configuration& DisplayController::GetConfig() const
{
    return m_config;
}

void DisplayController::HandleGetLcdBrightnessRequest( const Callback<Brightness>& resp )
{
    BOSE_DEBUG( s_logger, "%s", __FUNCTION__ );

    resp.Send( m_lcdBrightness );
}

void DisplayController::HandleLpmSetLightSensorParams( IpcLpmGenericResponse_t response )
{
    if( response.code() == IPC_TRANSITION_COMPLETE )
    {
        m_defaultsSentToLpm = true;

        BOSE_DEBUG( s_logger, "IPC_PER_SET_LIGHTSENSOR_PARAMS success!" );
    }
}

void DisplayController::HandleLpmGetUIBrightness( IpcUIBrightness_t response )
{
    m_lcdBrightness.set_mode( BrightnessIpcEnumToProtoEnum( ( IpcUIBrightnessMode_t ) response.mode() ) );
    m_lcdBrightness.set_value( response.value() );

    m_frontdoorClientPtr->SendNotification( FRONTDOOR_ENDPOINT_BRIGHTNESS, m_lcdBrightness );
}

bool DisplayController::IsFrontdoorBrightnessDataValid( const Brightness& incoming,
                                                        const Brightness& spec, const char* endPoint )
{
    bool modeFound = false;

    if( incoming.value() < spec.properties().min()
        || incoming.value() > spec.properties().max() )
    {
        BOSE_WARNING( s_logger, "%s %s request 'value' (%d) is out of range [%d, %d].",
                      __FUNCTION__, endPoint, incoming.value(),
                      spec.properties().min(), spec.properties().max() );

        return false;
    }

    for( int i = 0; i < spec.properties().supportedmodes_size(); ++i )
    {
        if( incoming.mode() == spec.properties().supportedmodes( i ) )
        {
            modeFound = true;
            break;
        }
    }

    if( ! modeFound )
    {
        BOSE_WARNING( s_logger, "%s %s request 'mode' (%s) is not supported.",
                      __FUNCTION__, endPoint, Brightness_BrightnessMode_Name( incoming.mode() ).c_str() );

        return false;
    }

    return true;
}

/*! \brief Frontdoor POST request handler for /ui/lcd/brightness.
 */
void DisplayController::HandlePutLcdBrightnessRequest( Brightness req, const Callback<Brightness> resp )
{
    BOSE_DEBUG( s_logger, "%s", __FUNCTION__ );

    bool incomingIsValid = IsFrontdoorBrightnessDataValid( req, m_lcdBrightness, FRONTDOOR_ENDPOINT_BRIGHTNESS );

    if( incomingIsValid )
    {
        // User changes from Frontdoor will be persisted all modes except UI_BRIGTHNESS_MODE_CAP_MAXIMUM.

        // Persist locally.
        m_lcdBrightness.set_mode( req.mode() );
        m_lcdBrightness.set_value( req.value() );

        // Send to LPM.
        auto f = [this]()
        {
            IpcUIBrightness_t lpmBrightness;
            BuildLpmUIBrightnessStruct( &lpmBrightness, UI_BRIGTHNESS_DEVICE_LCD );

            m_lpmClient->SetUIBrightness( lpmBrightness );
        };
        IL::BreakThread( f, m_productController.GetTask() );

        m_frontdoorClientPtr->SendNotification( FRONTDOOR_ENDPOINT_BRIGHTNESS, m_lcdBrightness );
    }

    resp.Send( m_lcdBrightness );
}

IpcUIBrightnessMode_t DisplayController::BrightnessProtoEnumToIpcEnum( Brightness_BrightnessMode mode )
{
    switch( mode )
    {
    case Brightness_BrightnessMode_MANUAL:
        return UI_BRIGTHNESS_MODE_MANUAL;

    case Brightness_BrightnessMode_AUTOMATIC:
        return UI_BRIGTHNESS_MODE_AUTOMATIC;

    case Brightness_BrightnessMode_DEFAULT:
        return UI_BRIGTHNESS_MODE_DEFAULT;

    default:
        BOSE_WARNING( s_logger, "%s: No defined mapping from Brightness_BrightnessMode %d to IpcUIBrightnessMode_t",
                      __FUNCTION__, mode );
        return UI_BRIGTHNESS_MODE_DEFAULT;
    }
}

Brightness_BrightnessMode DisplayController::BrightnessIpcEnumToProtoEnum( IpcUIBrightnessMode_t mode )
{
    switch( mode )
    {
    case UI_BRIGTHNESS_MODE_MANUAL:
        return Brightness_BrightnessMode_MANUAL;

    case UI_BRIGTHNESS_MODE_AUTOMATIC:
        return Brightness_BrightnessMode_AUTOMATIC;

    case UI_BRIGTHNESS_MODE_DEFAULT:
        return Brightness_BrightnessMode_DEFAULT;

    default:
        BOSE_WARNING( s_logger, "%s: No defined mapping from IpcUIBrightnessMode_t %d to Brightness_BrightnessMode",
                      __FUNCTION__, mode );
        return Brightness_BrightnessMode_DEFAULT;
    }
}

void DisplayController::BuildLpmUIBrightnessStruct( IpcUIBrightness_t* out, IpcUIBrightnessDevice_t deviceType )
{
    out->set_device( deviceType );

    switch( deviceType )
    {
    case UI_BRIGTHNESS_DEVICE_LCD:
        out->set_mode( BrightnessProtoEnumToIpcEnum( m_lcdBrightness.mode() ) );
        out->set_value( m_lcdBrightness.value() );
        break;

    case UI_BRIGTHNESS_DEVICE_LIGHTBAR:
        out->set_mode( BrightnessProtoEnumToIpcEnum( m_lightbarBrightness.mode() ) );
        out->set_value( m_lightbarBrightness.value() );
        break;

    default:
        // UI_BRIGTHNESS_DEVICE_LIGHTBAR is not supported at this time. Set all values to 0/default.
        out->set_mode( UI_BRIGTHNESS_MODE_DEFAULT );
        out->set_value( 0 );
        break;
    }

    out->set_time( UI_BRIGHTNESS_TIME_DEFAULT );
}

void DisplayController::RequestTurnDisplayOnOff( bool turnOn, AsyncCallback<void>& completedCb )
{
    BOSE_DEBUG( s_logger, "%s, turnOn %i", __FUNCTION__, turnOn );

    // Need to use "mutable" so that completedCb gets value-copied.
    auto f = [this, turnOn, completedCb]() mutable
    {
        if( m_config.m_hasLcd )
        {
            if( turnOn )
            {
                // Re-enable black screen detection which will drive the back light back on
                // once there is content. "Invalid" will force a re-detection.
                if( IsBlackScreenDetectSupported() )
                {
                    m_screenBlackState = ScreenBlackState_Invalid;
                }
            }
            else
            {
                // Disable black screen detection while the screen is off.
                m_screenBlackState = ScreenBlackState_Disabled;

                // Turn the display off instantly because the LPM may be driving into a low
                // power state.
                SetDisplayBrightnessCap( BRIGHTNESS_MIN, UI_BRIGHTNESS_TIME_IMMEDIATE );
            }
        }

        completedCb();
    };

    IL::BreakThread( f, m_task );
}

DisplayController::ScreenBlackState DisplayController::ReadFrameBufferBlackState()
{
    if( ! m_config.m_blackScreenDetectEnabled )
    {
        return ScreenBlackState_Invalid;
    }

    char blackState = '9';
    // dr1037486 I am intentionally using open() instead of SystemUtils::ReadFile() because
    // it is about 25% faster and we don't need the extra overhead (only reading 1 character).
    // This gets called frequently.
    int blackScreenFp = open( BLACK_SCREEN_FILE_NAME.c_str(), O_RDONLY );

    if( blackScreenFp == -1
        || read( blackScreenFp, &blackState, 1 ) != 1 )
    {
        BOSE_DEBUG( s_logger, "Failed to read black screen file '%s'", BLACK_SCREEN_FILE_NAME.c_str() );

        m_config.m_blackScreenDetectEnabled = false;
        close( blackScreenFp );
        return ScreenBlackState_Invalid;
    }

    close( blackScreenFp );

    return ( blackState == '1' ) ? ScreenBlackState_Black : ScreenBlackState_NotBlack;
}

/*!
 * The "monaco.pid" file is created by run_wpe.sh from the CastleWebKit component.
 */
void DisplayController::KillUiProcess()
{
    if( m_killUIOnHeartBeatLossPidFile.empty() )
    {
        return;
    }

    pid_t pid = 0;

    if( auto fileData = SystemUtils::ReadFile( m_killUIOnHeartBeatLossPidFile ) )
    {
        char* endPointer = nullptr;
        pid = strtol( fileData->c_str(), &endPointer, 10 );
    }

    if( pid != 0 )
    {
        BOSE_ERROR( s_logger, "Killing UI process at pid %i", pid );
        kill( pid, SIGKILL );
    }
    else
    {
        BOSE_ERROR( s_logger, "UI pid file '%s' read failed or PID invalid. Restart aborted.",
                    m_killUIOnHeartBeatLossPidFile.c_str() );
    }
}

} //namespace ProductApp
