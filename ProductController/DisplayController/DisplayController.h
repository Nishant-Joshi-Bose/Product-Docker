////////////////////////////////////////////////////////////////////////////////
///// @file   DisplayController.h
///// @brief  Eddie LightBar controller declaration
/////
///// @attention Copyright 2017 Bose Corporation, Framingham, MA
//////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <iostream>
#include <memory>
#include <thread>
#include "APTask.h"
#include "FrontDoorClientIF.h"
#include "DisplayController.pb.h"
#include "LpmClientIF.h"

using namespace ::DisplayController::Protobuf;

namespace ProductApp
{
class ProductController;

class DisplayController
{
public:
    DisplayController( ProductController& m_controller,
                       const std::shared_ptr<FrontDoorClientIF>& fd_client,
                       LpmClientIF::LpmClientPtr clientPtr, AsyncCallback<bool> uiConnectedCb );
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
/// @name   HandlePutDisplayRequest
/// @brief-
/// @return void
///////////////////////////////////////////////////////////////////////////////
    void HandlePutDisplayRequest( const Display &req,
                                  const Callback<Display>& resp );

///////////////////////////////////////////////////////////////////////////////
/// @name   HandleGetDisplayRequest
/// @brief-
/// @return void
///////////////////////////////////////////////////////////////////////////////
    void HandleGetDisplayRequest( const Callback<Display>& resp );


///////////////////////////////////////////////////////////////////////////////
/// @name   HandleDeleteDisplayRequest
/// @brief-
/// @return void
///////////////////////////////////////////////////////////////////////////////
    void HandleDeleteDisplayRequest( const Display& req,
                                     const Callback<Display>& resp );

///////////////////////////////////////////////////////////////////////////////
/// @name   HandlePostUiHeartBeat
/// @brief-
/// @return void
///////////////////////////////////////////////////////////////////////////////
    void HandlePostUiHeartBeat( const UiHeartBeat &req,
                                Callback<UiHeartBeat> resp );

///////////////////////////////////////////////////////////////////////////////
/// @name   HandlePutUiHeartBeat
/// @brief-
/// @return void
///////////////////////////////////////////////////////////////////////////////
    void HandlePutUiHeartBeat( const UiHeartBeat &req,
                               Callback<UiHeartBeat> resp );

///////////////////////////////////////////////////////////////////////////////
/// @name   HandleGetUiHeartBeat
/// @brief-
/// @return void
///////////////////////////////////////////////////////////////////////////////
    void HandleGetUiHeartBeat( Callback<UiHeartBeat> resp );

    /*! \brief Frontdoor GET request handler for /ui/lcd/brightness.
     * \param resp Callback into which will be written current LCD brightess as a Brightness protobuf.
     */
    void HandleGetLcdBrightnessRequest( const Callback<Brightness>& resp );

    /*! \brief Frontdoor POST request handler for /ui/lcd/brightness.
     * \param req Incoming Brightness settings to apply.
     * \param resp Callback into which will be written current LCD brightess as a Brightness protobuf.
     */
    void HandlePutLcdBrightnessRequest( const Brightness &req, const Callback<Brightness>& resp );

    /*! \param Put display in on or off state.
     */
    bool TurnDisplayOnOff( bool turnOn );

    /*! \brief Enables/disables brightness cap for LCD during a standby state (not low power).
     * \param enabled True to impose the cap and false to disable it.
     */
    void SetStandbyLcdBrightnessCapEnabled( bool enabled );

private:
    /// Disable copies
    DisplayController( const DisplayController& ) = delete;
    DisplayController& operator=( const DisplayController& ) = delete;

    /*! \brief Register Frontdoor API's.
     */
    void RegisterFrontdoorEndPoints();

    void HandleLpmResponse( LBCSResponse_t response, const Callback<Display>& resp );
    bool HandleLpmNotificationBackLight( IpcBackLight_t   lpmBackLight );
    bool HandleLpmNotificationUIBrightness( IpcUIBrightness_t lpmBrightness );
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
     * \param immediate True to apply immediately w/o smoothing. Default false.
     */
    void SetDisplayBrightnessCap( uint8_t capPercent, bool immediate = false );

    /*! \brief Validate incoming data to the /ui/XXX/brightness end point.
     * \param incoming The data being received.
     * \param spec The specification data that will be used for validation.
     * \param endPoint Name of the end point (for log messages).
     * \return True if data is OK, false otherwise.
     */
    bool IsFrontdoorBrightnessDataValid( const Brightness& incoming,  const Brightness& spec, const char* endPoint );

    ProductController&                 m_productController;
    std::shared_ptr<FrontDoorClientIF> m_frontdoorClientPtr;
    LpmClientIF::LpmClientPtr          m_lpmClient;
    bool                               m_defaultsSentToLpm;         //!< Successfully send default values to LPM.
    int64_t                            m_defaultsSentTime;          //!< Time defaults were last sent to LPM.
    Display                            m_display;
    Brightness                         m_lcdBrightness;             //!< Current brightness settings for the LCD display.
    Brightness                         m_lightbarBrightness;        //!< Current brightness settings for the lightbar display.
    uint8_t                            m_lcdStandbyBrightnessCap;   //!< Value to use for LCD brightness cap when in standby.
    uint8_t                            m_lcdBrightnessCapSystem;    //!< System designated brightness cap.
    uint8_t                            m_lcdBrightnessCapFrontdoor; //!< Frontdoor designated brightness cap.
    std::unique_ptr<std::thread>       m_threadUpdateLoop;          //!< Thread that invokes UpdateLoop.
    bool                               m_timeToStop;
    int                                m_backLight;
    float                              m_luxFactor;
    uint64_t                           m_uiHeartBeat;
    uint64_t                           m_localHeartBeat;
    bool                               m_uiConnected = false;
    AsyncCallback<bool>                m_ProductControllerUiConnectedCb;
};
} //namespace ProductApp

