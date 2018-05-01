////////////////////////////////////////////////////////////////////////////////
///// @file   DisplayController.cpp
///// @brief  Implements Eddie Display controller class.
/////
///// @attention Copyright 2017 Bose Corporation, Framingham, MA
//////////////////////////////////////////////////////////////////////////////////

#include <limits.h>
#include <float.h>
#include <functional>
#include <iostream>
#include <fstream>
#include <json/json.h>

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
using namespace ::DisplayController::Protobuf;

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
static constexpr char JSON_TOKEN_DISPLAY_CONTROLLER  [] = "DisplayController";
static constexpr char JSON_TOKEN_BACK_LIGHT_LEVELS   [] = "BackLightLevelsPercent";
static constexpr char JSON_TOKEN_LOWERING_THRESHOLDS [] = "LoweringThresholdLux";
static constexpr char JSON_TOKEN_RISING_THRESHOLDS   [] = "RisingThresholdLux";

static constexpr char UIBRIGHTNESS_MODE_NAME_AUTO[]     = "automatic";
static constexpr char UIBRIGHTNESS_MODE_NAME_DEFAULT[]  = "default";
static constexpr char UIBRIGHTNESS_MODE_NAME_MANUAL[]   = "manual";

static constexpr char FRONTDOOR_ENDPOINT_ALIVE[]        = "/ui/alive";
static constexpr char FRONTDOOR_ENDPOINT_BRIGHTNESS[]   = "/ui/lcd/brightness";

static constexpr int   UPDATE_SLEEP_MS                  = 1000;
static constexpr int   SEND_DEFAULTS_TO_LPM_RETRY_MS    = 2000;
static constexpr float SILVER_LUX_FACTOR                = 11.0f;
static constexpr uint8_t BRIGHTNESS_MAX                 = 100;
static constexpr uint8_t BRIGHTNESS_MIN                 = 0;

/*!
 */
DisplayController::DisplayController( ProductController& controller, const std::shared_ptr<FrontDoorClientIF>& fd_client,
                                      LpmClientIF::LpmClientPtr clientPtr, AsyncCallback<bool> uiConnectedCb ):
    m_productController( controller ),
    m_frontdoorClientPtr( fd_client ),
    m_lpmClient( clientPtr ),
    m_defaultsSentToLpm( false ),
    m_defaultsSentTime( 0 ),
    m_lcdStandbyBrightnessCap( 50 ), // Sensible default. Real default loaded from JSON.
    m_lcdBrightnessCapSystem( BRIGHTNESS_MAX ),
    m_lcdBrightnessCapFrontdoor( BRIGHTNESS_MAX ),
    m_timeToStop( false ),
    m_uiHeartBeat( ULLONG_MAX ),
    m_localHeartBeat( ULLONG_MAX ),
    m_ProductControllerUiConnectedCb( uiConnectedCb )
{

}

/*!
 */
DisplayController::~DisplayController()
{
    m_timeToStop = true;

    if( m_threadUpdateLoop )
    {
        m_threadUpdateLoop->join();
    }
}

/*!
 */
void DisplayController::Initialize()
{
    bool jsonReadSuccess = ParseJSONData();

    //ui/display end point registration with front door
    RegisterFrontdoorEndPoints();

    m_threadUpdateLoop = std::unique_ptr<std::thread>( new std::thread( [this] { UpdateLoop(); } ) );

    if( ! jsonReadSuccess )
    {
        m_defaultsSentToLpm = true; // Update loop will never send the defaults if this is true.

        BOSE_WARNING( s_logger, "JSON load from '%s' failed. Not sending settings to LPM.", DISPLAY_CONTROLLER_FILE_NAME );
    }
}

/*!
 */
bool DisplayController::ParseJSONData()
{
    BOSE_DEBUG( s_logger, "%s", __FUNCTION__ );

    auto f = SystemUtils::ReadFile( DISPLAY_CONTROLLER_FILE_NAME );

    m_luxFactor = SILVER_LUX_FACTOR;

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
        BOSE_LOG( WARNING, "Error: failed to parse JSON File: " << DISPLAY_CONTROLLER_FILE_NAME << " " << errors.c_str() );
        return false;
    }

    if( ! json_root[JSON_TOKEN_DISPLAY_CONTROLLER].isMember( JSON_TOKEN_BACK_LIGHT_LEVELS ) )
    {
        BOSE_LOG( WARNING, "Error: " << JSON_TOKEN_BACK_LIGHT_LEVELS << " is not a member of: " << JSON_TOKEN_DISPLAY_CONTROLLER );
        return false;
    }

    if( ! json_root[JSON_TOKEN_DISPLAY_CONTROLLER].isMember( JSON_TOKEN_LOWERING_THRESHOLDS ) )
    {
        BOSE_LOG( WARNING, "Error: " << JSON_TOKEN_LOWERING_THRESHOLDS << " is not a member of: " << JSON_TOKEN_DISPLAY_CONTROLLER );
        return false;
    }

    if( ! json_root[JSON_TOKEN_DISPLAY_CONTROLLER].isMember( JSON_TOKEN_RISING_THRESHOLDS ) )
    {
        BOSE_LOG( WARNING, "Error: " << JSON_TOKEN_RISING_THRESHOLDS << " is not a member of: " << JSON_TOKEN_DISPLAY_CONTROLLER );
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
    // LCD and lightbar Brightness defaults.
    //

    if( json_root.isMember( "lcdStandbyBrightnessCap" ) )
    {
        m_lcdStandbyBrightnessCap = json_root["lcdStandbyBrightnessCap"].asUInt();
    }

    if( ! ParseBrightnessData( &m_lcdBrightness, json_root, "lcdBrightness" ) )
    {
        return false;
    }

    if( ! ParseBrightnessData( &m_lightbarBrightness, json_root, "lightbarBrightness" ) )
    {
        return false;
    }

    //
    // Print the levels for debugging.
    //

    for( unsigned int i = 0; i < nb_threshold_levels; i++ )
    {
        BOSE_DEBUG( s_logger, "%s level %i: backlight %d, lowering %f rising %f",
                    __FUNCTION__, i, s_backLightLevels[i], s_loweringLuxThreshold[i], s_risingLuxThreshold[i] );
    }

    return true;
}

/*!
 */
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

/*!
 */
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

    m_lpmClient->SetLightSensorParams( defaults, [this]( const IpcLpmGenericResponse_t& response )
    {
        if( response.code() == IPC_TRANSITION_COMPLETE )
        {
            m_defaultsSentToLpm = true;

            BOSE_DEBUG( s_logger, "IPC_PER_SET_LIGHTSENSOR_PARAMS success!" );
        }
    } );
}

/*!
 */
void DisplayController::UpdateUiConnected( bool currentUiConnectedStatus )
{
    if( m_uiConnected != currentUiConnectedStatus )
    {
        BOSE_LOG( WARNING, "currentUiConnectedStatus: " << currentUiConnectedStatus );
        m_uiConnected = currentUiConnectedStatus;
        m_ProductControllerUiConnectedCb( currentUiConnectedStatus );

        // LPM boots with the cap at 0 to turn off the LCD until the display controller
        // on the APQ (us) is fully initialized. Remove that cap at this time.
        SetDisplayBrightnessCap( BRIGHTNESS_MAX );

        PullUIBrightnessFromLpm( UI_BRIGTHNESS_DEVICE_LCD );
    }
}

/*!
 */
void DisplayController::SetDisplayBrightnessCap( uint8_t capPercent, bool immediate )
{
    IpcUIBrightness_t lpmBrightness;

    lpmBrightness.set_device( UI_BRIGTHNESS_DEVICE_LCD );
    lpmBrightness.set_value( capPercent );
    lpmBrightness.set_mode( UI_BRIGTHNESS_MODE_CAP_MAXIMUM );
    lpmBrightness.set_immediate( immediate );

    m_lpmClient->SetUIBrightness( lpmBrightness );
}

/*!
 * Any value set through this function is meant to be authoritavite over a something
 * we get from Frontdoor. This is to ensure that if the embedded system needs to
 * conserve power (for regulation) the brightness will stay capped.
 */
void DisplayController::SetStandbyLcdBrightnessCapEnabled( bool enabled )
{
    m_lcdBrightnessCapSystem = ( enabled ) ? m_lcdStandbyBrightnessCap : BRIGHTNESS_MAX;
    // Actual cap sent to LPM is min of system and frontdoor caps.
    SetDisplayBrightnessCap( MIN( m_lcdBrightnessCapSystem, m_lcdBrightnessCapFrontdoor ) );
}

/*!
 */
void DisplayController::UpdateLoop()
{
    while( ! m_timeToStop )
    {
        if( m_uiHeartBeat != ULLONG_MAX )
        {
            if( m_localHeartBeat == ULLONG_MAX )
            {
                m_localHeartBeat = m_uiHeartBeat;
            }// if it's te first heart beat receive from the UI

            m_localHeartBeat++;
        }

        if( ! m_defaultsSentToLpm
            // Transfer and processing on LPM can take time. Throttle this.
            && MonotonicClock::NowMs() - m_defaultsSentTime > SEND_DEFAULTS_TO_LPM_RETRY_MS )
        {
            m_defaultsSentTime = MonotonicClock::NowMs();

            PushDefaultsToLPM();
        }

        usleep( UPDATE_SLEEP_MS * 1000 );
    }
}

/*!
 */
void DisplayController::RegisterLpmEvents()
{
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

/*!
 */
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

/*!
 */
void DisplayController::HandlePutDisplayRequest( const Display &req,
                                                 const Callback<Display>& resp )
{
}

/*!
 */
void DisplayController::HandleGetDisplayRequest( const Callback<Display>& resp )
{
    resp.Send( GetDisplay() );
}

/*!
 */
void DisplayController::PullUIBrightnessFromLpm( IpcUIBrightnessDevice_t deviceType )
{
    IpcGetUIBrightnessParams_t params;
    params.set_device( deviceType );

    m_lpmClient->GetUIBrightness( params, [this]( const IpcUIBrightness_t& response )
    {
        m_lcdBrightness.set_mode( BrightnessIpcEnumToProtoEnum( ( IpcUIBrightnessMode_t ) response.mode() ) );
        m_lcdBrightness.set_value( response.value() );
    } );
}

/*!
 */
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

/*!
 */
void DisplayController::HandlePostUiHeartBeat( const UiHeartBeat &req,
                                               Callback<UiHeartBeat> resp )
{
    BOSE_LOG( INFO, "received first heartbeat: " << req.count() );
    m_uiHeartBeat = req.count();
    if( !m_uiConnected )
    {
        UpdateUiConnected( true );
    }
    UiHeartBeat response;
    response.set_count( m_uiHeartBeat );
    resp.Send( response );
}

/*!
 */
void DisplayController::HandlePutUiHeartBeat( const UiHeartBeat &req,
                                              Callback<UiHeartBeat> resp )
{
    BOSE_LOG( VERBOSE, "received heartbeat: " << req.count() << ", current heat beat: " << m_uiHeartBeat );

    if( abs( m_uiHeartBeat - req.count() ) >= 2 )
    {
        BOSE_LOG( WARNING, "UI is skipping heart beat, received heartbeat: " << req.count() + ", current heat beat: " << m_uiHeartBeat );
    }

    m_uiHeartBeat = req.count();
    if( !m_uiConnected )
    {
        UpdateUiConnected( true );
    }
    UiHeartBeat response;
    response.set_count( m_uiHeartBeat );
    resp.Send( response );
}

/*!
 */
void DisplayController::HandleGetUiHeartBeat( Callback<UiHeartBeat> resp )
{
    BOSE_LOG( VERBOSE, "received Get heartbeat: " << m_uiHeartBeat );
    UiHeartBeat response;
    response.set_count( m_uiHeartBeat );
    resp.Send( response );
}

/*!
 */
Display DisplayController::GetDisplay()
{
    return m_display;
}

/*!
 */
void DisplayController::HandleGetLcdBrightnessRequest( const Callback<Brightness>& resp )
{
    BOSE_DEBUG( s_logger, "%s", __FUNCTION__ );

    resp.Send( m_lcdBrightness );
}

/*!
 */
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
void DisplayController::HandlePutLcdBrightnessRequest( const Brightness& req, const Callback<Brightness>& resp )
{
    BOSE_DEBUG( s_logger, "%s", __FUNCTION__ );

    bool incomingIsValid = IsFrontdoorBrightnessDataValid( req, m_lcdBrightness, FRONTDOOR_ENDPOINT_BRIGHTNESS );

    if( incomingIsValid )
    {
        IpcUIBrightness_t lpmBrightness;
        // User changes from Frontdoor will be persisted all modes except UI_BRIGTHNESS_MODE_CAP_MAXIMUM.

        // Persist locally.
        m_lcdBrightness.set_mode( req.mode() );
        m_lcdBrightness.set_value( req.value() );

        // Send to LPM.
        BuildLpmUIBrightnessStruct( &lpmBrightness, UI_BRIGTHNESS_DEVICE_LCD );
        m_lpmClient->SetUIBrightness( lpmBrightness );
    }

    resp.Send( m_lcdBrightness );
}

/*!
 */
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

/*!
*/
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

/*!
 */
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
}

/*!
 */
bool DisplayController::TurnDisplayOnOff( bool turnOn )
{
    const std::string displayControllerDir = "/sys/devices/soc/7af6000.spi/spi_master/spi6/spi6.1/graphics/fb1/";
    const std::string sendCommandFileName  = displayControllerDir + "send_command";
    const std::string teFileName           = displayControllerDir + "te";
    const char*       onOffCmdString       = turnOn ? "29" : "28" ;
    const char*       teString             = turnOn ? "1"  : "0"  ;

    BOSE_LOG( VERBOSE, "turning LCD: " << ( turnOn ? "on" : "off" ) );

    if( turnOn )
    {
        // dr1037486 When turning on the display, use a ramp. In addition to being more
        // pleasing, this will hide any pop that might result from multiple "brightness cap"
        // commands in quick succession. For example, right now Eddie resumes from low power
        // supmend into network standby. The latter has a brightness cap which will be
        // applied only after we resume into the previous brightness.
        SetDisplayBrightnessCap( BRIGHTNESS_MAX, false );
    }
    else
    {
        // Turn the display off instantly because the LPM may be driving into a low
        // power state.
        SetDisplayBrightnessCap( BRIGHTNESS_MIN, true );
    }

    if( DirUtils::DoesFileExist( sendCommandFileName ) == false )
    {
        BOSE_LOG( ERROR, "error: can't find file: " + sendCommandFileName + " - " + strerror( errno ) );
        return false;
    }

    if( DirUtils::DoesFileExist( teFileName ) == false )
    {
        BOSE_LOG( ERROR, "error: can't find file: " + teFileName + " - " + strerror( errno ) );
        return false;
    }

    std::ofstream displayControllerSendCmd( sendCommandFileName );
    std::ofstream displayControllerTe( teFileName );

    if( displayControllerSendCmd.is_open() == false )
    {
        BOSE_LOG( ERROR,  "error: failed to open file: " + sendCommandFileName + " - " + strerror( errno ) );
        return false;
    }

    if( displayControllerTe.is_open() == false )
    {
        BOSE_LOG( ERROR,  "error: failed to open file: " + teFileName + " - " + strerror( errno ) );
        return false;
    }

    if( turnOn == false )
    {
        displayControllerTe << teString;
        usleep( 25 * 1000 ); // wait a full te cycle, the slowest is 40Hz
    }

    displayControllerSendCmd << onOffCmdString; // see ST7789VI_SPEC_V1.4.pdf

    if( turnOn == true )
    {
        displayControllerTe << teString;
    }

    BOSE_LOG( VERBOSE, "LCD is now: " << ( turnOn ? "on" : "off" ) );
    return true;
}

} //namespace ProductApp
