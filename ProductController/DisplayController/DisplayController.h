////////////////////////////////////////////////////////////////////////////////
///// @file   DisplayController.h
///// @brief  LightBar controller declarations
/////
///// @attention Copyright 2017 Bose Corporation, Framingham, MA
//////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <iostream>
#include <memory>
#include <thread>
#include "APTask.h"
#include "APTimer.h"
#include "AsyncCallback.h"
#include "FrontDoorClientIF.h"
#include "DisplayController.pb.h"
#include "LpmClientIF.h"
#include "NotifyTargetTaskIF.h"

using namespace ::DisplayControllerPb;

namespace ProductApp
{
class ProductController;

class DisplayController
{
public:

    /*! \brief Set of paramaters that may be used to tune the behavior of DisplayController.
     */
    class Configuration
    {
    public:
        Configuration() :
            m_hasLightSensor( false ),
            m_hasLcd( false ),
            m_blackScreenDetectEnabled( false )
        {
        }

        bool m_hasLightSensor;              //!< Flag for device hase a light sensor.
        bool m_hasLcd;                      //!< Flag to enable LCD screen functionality.
        bool m_blackScreenDetectEnabled;    //!< Enable black screen detection (LCD only).
    };

    //! \brief State of screen contents used for blank detection.
    enum ScreenBlackState
    {
        ScreenBlackState_Invalid = -1,

        ScreenBlackState_Disabled,
        ScreenBlackState_Black,
        ScreenBlackState_NotBlack,

        ScreenBlackState_Count
    };

    /*! \brief Constructor.
     * \param config Tune how DisplayController will behave.
     * \param controller Reference to main Product Controller.
     * \param fdClient Frontdoor client reference.
     * \param clientPtr LpmClient reference.
     * \param uiConnectedCb Callback to be invoked when the UI as registered as connected.
     */
    DisplayController( const Configuration& config,
                       ProductController& controller,
                       const std::shared_ptr<FrontDoorClientIF>& fdClient,
                       LpmClientIF::LpmClientPtr clientPtr,
                       AsyncCallback<bool> uiConnectedCb );

    /*! \brief Destructor.
     */
    ~DisplayController();

///////////////////////////////////////////////////////////////////////////////
/// @name  Initialize()
/// @brief-
/// @return void
///////////////////////////////////////////////////////////////////////////////
    void Initialize();

    /*! \brief Register for LPM callback events.
     */
    void RegisterLpmEvents();

///////////////////////////////////////////////////////////////////////////////
/// @name  GetDisplay
/// @brief return currently active display and queued requests
/// @return void
///////////////////////////////////////////////////////////////////////////////
    Display GetDisplay();

///////////////////////////////////////////////////////////////////////////////
/// @name  GetConfig
/// @brief return config paramaters that used to tune the behavior of DisplayController
/// @return void
///////////////////////////////////////////////////////////////////////////////
    Configuration GetConfig();

    /*! \brief Put display in on or off state.
     * \param turnOn Pass true to turn the display on, and false to turn it off.
     * \param completedCb Optional callback to be invoked when the action is complete, default empty.
     *      If you don't want to use the callback, poss in a no-op one:
     *      AsyncCallback<void> emptyCb( [] {}, nullptr )
     */
    void RequestTurnDisplayOnOff( bool turnOn, AsyncCallback<void>& completedCb );

    /*! \brief Enables/disables brightness cap for LCD during a standby state (not low power).
     * \param enabled True to impose the cap and false to disable it.
     */
    void SetStandbyLcdBrightnessCapEnabled( bool enabled );

private:

    DisplayController( const DisplayController& ) = delete;
    DisplayController& operator=( const DisplayController& ) = delete;

    /*! \brief Register Frontdoor API's.
     */
    void RegisterFrontdoorEndPoints();

    /*!
     */
    void HandleLpmResponse( LBCSResponse_t response, const Callback<Display>& resp );

    /*!
     */
    bool HandleLpmNotificationBackLight( IpcBackLight_t   lpmBackLight );

    /*!
     */
    bool HandleLpmNotificationUIBrightness( IpcUIBrightness_t lpmBrightness );

    /*!
     */
    void HandlePutDisplayRequest( const Display &req,
                                  const Callback<Display>& resp );
    /*!
     */
    void HandleGetDisplayRequest( Callback<Display> resp );

    /*!
     */
    void HandleDeleteDisplayRequest( const Display& req,
                                     const Callback<Display>& resp );

    /*!
     */
    void HandlePostUiHeartBeat( const UiHeartBeat &req,
                                Callback<UiHeartBeat> resp );

    /*!
     */
    void HandlePutUiHeartBeat( const UiHeartBeat &req,
                               Callback<UiHeartBeat> resp );

    /*!
     */
    void HandleGetUiHeartBeat( Callback<UiHeartBeat> resp );

    /*! \brief Frontdoor GET request handler for /ui/lcd/brightness.
     * \param resp Callback into which will be written current LCD brightess as a Brightness protobuf.
     */
    void HandleGetLcdBrightnessRequest( const Callback<Brightness>& resp );

    /*! \brief Frontdoor POST request handler for /ui/lcd/brightness.
     * \param req Incoming Brightness settings to apply.
     * \param resp Callback into which will be written current LCD brightess as a Brightness protobuf.
     */
    void HandlePutLcdBrightnessRequest( Brightness req, const Callback<Brightness> resp );

    /*! \brief Callback for LPMClient SetLightSensorParams response handling.
     */
    void HandleLpmSetLightSensorParams( IpcLpmGenericResponse_t response );

    /*! \brief Callback for LPMClient GetUIBrightness response handling.
     */
    void HandleLpmGetUIBrightness( IpcUIBrightness_t response );

    /*!
     */
    void UpdateUiConnected( bool currentStatus );

    /*! \brief Main update loop function.
     */
    void UpdateLoop( );

    /*! \brief Read display settings from from DISPLAY_CONTROLLER_FILE_NAME.
     * This includes light sensor default configuraiton.
     * \return True if parsing was successful.
     */
    bool ParseJSONData();

    /*! \brief Push all defaults to the LPM via IPC_PER_SET_LIGHTSENSOR_PARAMS.
     * ParseJSONData() must be invoked before.
     */
    void PushDefaultsToLPM();

    /*! \brief Parse through a JSON node to load Brightness data into the given output object.
     * \param output Output data.
     * \param rootNode Root of JSON structure.
     * \param nodeName Name of of block under rootNode containing brightness data.
     * \return Success or failure.
     */
    bool ParseBrightnessData( Brightness* output, const Json::Value& rootNode, std::string nodeName );

    /*! \brief Maps a Brightness enum value from a protobuf message to the corresponding IPC message value.
     * \param mode The mode type from protobuf messages.
     * \return IPC version of the enum.
     */
    static IpcUIBrightnessMode_t BrightnessProtoEnumToIpcEnum( Brightness_BrightnessMode mode );

    /*! \brief Maps a Brightness enum value from an IPC message to the corresponding protobuf message value.
     * \param mode The mode type from IPC messages.
     * \return Protobuf version of the enum.
     */
    static Brightness_BrightnessMode BrightnessIpcEnumToProtoEnum( IpcUIBrightnessMode_t mode );

    /*! \brief Builds a structure appropriate for sending UI Brightness settings to the LPM.
     * Data will be pulled from the appropriate local variable.
     * \param out Output structure reference.
     * \param deviceType The UI brightness device.
     */
    void BuildLpmUIBrightnessStruct( IpcUIBrightness_t* out, IpcUIBrightnessDevice_t deviceType );

    /*! \brief Retrieve current UI Brightness settings for the given device.
     * \param deviceType A valid IpcUIBrightnessDevice_t.
     */
    void PullUIBrightnessFromLpm( IpcUIBrightnessDevice_t deviceType );

    /*! \brief Sets the cap on brightness for a device. This is executed in the LightSensorTask on the LPM.
     * This cap is never persisted and will reset on boot.
     * Use this to, for example, drive the LCD to a reduced level during standby mode.
     * \param capPercent The intensity cap from 0 - 100.
     * \param time Transition time in MS. For example: UI_BRIGHTNESS_TIME_DEFAULT or UI_BRIGHTNESS_TIME_IMMEDIATE.
     */
    void SetDisplayBrightnessCap( uint8_t capPercent, uint16_t time );

    /*! \brief Validate incoming data to the /ui/XXX/brightness end point.
     * \param incoming The data being received.
     * \param spec The specification data that will be used for validation.
     * \param endPoint Name of the end point (for log messages).
     * \return True if data is OK, false otherwise.
     */
    bool IsFrontdoorBrightnessDataValid( const Brightness& incoming,  const Brightness& spec, const char* endPoint );

    /*! \brief Reads from sysfs to determine if the screen is blank or not.
     * Requires a compatible frame buffer driver (as of HSP 3.10) but is safe to call
     * even if not supported.
     * \return State of frame buffer being blank or ScreenBlackState_Invalid if not supported.
     */
    ScreenBlackState ReadFrameBufferBlackState();

    /*! \brief Manage heart beats from the UI system.
     * If we have not received a heart beat in a while, turn off the screen, etc.
     */
    void ProcessUiHeartBeat();

    /*! \brief Do screen blank detection and respons.
     */
    void ProcessBlackScreenDetection();

    /*! \brief Sets the current screen blank state and clear transient flags.
     * \param s New "now" screen blank state.
     */
    void SetBlackScreenNowState( ScreenBlackState s );

    /*! \brief Kills the process that drives the UI.
     * We do this when we lose hearbeat for so long. Shepherd will then restart the
     * dead process automatically. Note that the restarting is not done when the system
     * is running in a development environment. See /opt/Bose/bin/Soundtouch.
     */
    void KillUiProcess();

    /*! \brief Convenience function for checking if black screen detection is enabled.
     * \return Black screen detect state.
     */
    bool IsBlackScreenDetectSupported() const
    {
        return ( m_config.m_hasLcd && m_config.m_blackScreenDetectEnabled );
    }


    Configuration                      m_config;                    //!< Some configuration parameters.

    ProductController&                 m_productController;
    std::shared_ptr<FrontDoorClientIF> m_frontdoorClientPtr;
    LpmClientIF::LpmClientPtr          m_lpmClient;

    bool                               m_defaultsSentToLpm;         //!< Successfully send default values to LPM.
    int64_t                            m_defaultsSentTime;          //!< Time defaults were last sent to LPM.
    Display                            m_display;
    Brightness                         m_lcdBrightness;             //!< Current brightness settings for the LCD display.
    Brightness                         m_lightbarBrightness;        //!< Current brightness settings for the lightbar display.
    uint8_t                            m_lcdStandbyBrightnessCap;   //!< Value to use for LCD brightness cap when in standby.
    uint8_t                            m_lcdBrightnessCap;          //!< System designated brightness cap.

    ScreenBlackState                   m_screenBlackState;          //!< Current screen blank state.
    ScreenBlackState                   m_screenBlackChangeTo;       //!< Desired future screen blank state (for debouncing).
    uint64_t                           m_screenBlackInactivityTicks;//!< Ticks to wait after black screen before turning off backlight.
    uint64_t                           m_screenBlackChangeCounter;  //!< Counter for debouncing screen blank changes.

    uint64_t                           m_currentTick;               //!< Current update loop tick count.
    NotifyTargetTaskIF*                m_task;                      //!< Background task for periodically polling.
    APTimerPtr                         m_updateloopTimer;           //!< Trigger subsequent update loop function calls.

    uint64_t                           m_uiHeartBeat;               //!< Last received UI heartbeat count.
    uint64_t                           m_lastUiHeartBeatTick;       //!< Tick at which last UI heartbeat was received.
    uint64_t                           m_uiHeartBeatLossWarnTicks;  //!< Warn about UI heart beat loss after this many ticks.
    uint64_t                           m_uiHeartBeatLossErrorTicks; //!< Error and disconnect if UI heart beat is lost for this many ticks.
    std::string                        m_killUIOnHeartBeatLossPidFile;     //!< Name of file containing UI PID. If empty, function is disabled.
    bool                               m_uiConnected;
    AsyncCallback<bool>                m_ProductControllerUiConnectedCb;
};
} //namespace ProductApp

