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
                       LpmClientIF::LpmClientPtr clientPtr );
    ~DisplayController();

///////////////////////////////////////////////////////////////////////////////
/// @name  Initialize()
/// @brief-
/// @return void
///////////////////////////////////////////////////////////////////////////////
    void Initialize();

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
/// @name   SetAutoMode
/// @brief-
/// @return void
///////////////////////////////////////////////////////////////////////////////
    void SetAutoMode( bool autoMode )
    {
        m_autoMode = autoMode;
    }

private:
    /// Disable copies
    DisplayController( const DisplayController& ) = delete;
    DisplayController& operator=( const DisplayController& ) = delete;

    void RegisterDisplayEndPoints();
    void HandleLpmResponse( LBCSResponse_t response, const Callback<Display>& resp );
    bool HandleLpmNotificationBackLight( IpcBackLight_t   lpmBackLight );
    bool HandleLpmNotificationLightSensor( IpcLightSensor_t lpmLightSensor );
    void MonitorLightSensor( );
    void SetBackLightLevel( int actualLevel, int newLevel );
    int  GetBackLightLevelFromLux( float lux, float lux_rising );

    ProductController&                 m_productController;
    std::shared_ptr<FrontDoorClientIF> m_frontdoorClientPtr;
    LpmClientIF::LpmClientPtr          m_lpmClient;
    Display                            m_display;
    std::unique_ptr<std::thread>       m_threadMonitorLightSensor;
    bool                               m_timeToStop;
    int                                m_backLight ;
    bool                               m_autoMode  ;
    float                              m_luxFactor ;
    float                              m_luxValue  ;
    int                                m_luxDecimal   ;
    int                                m_luxFractional;
};
} //namespace ProductApp

