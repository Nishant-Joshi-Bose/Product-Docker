////////////////////////////////////////////////////////////////////////////////
///// @file   LightBarController.h
///// @brief  Eddie LightBar controller declaration
/////
///// @attention Copyright 2017 Bose Corporation, Framingham, MA
//////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <iostream>
#include <memory>
#include "FrontDoorClientIF.h"
#include "LightBarController.pb.h"
#include "LpmClientIF.h"
#include "AnimationIntentMap.h"

using namespace ::LightBarController::Protobuf;

namespace ProductApp
{
class ProductController;

class LightBarController
{
public:
    LightBarController( ProductController& m_controller,
                        const std::shared_ptr<FrontDoorClientIF>& fd_client,
                        LpmClientIF::LpmClientPtr clientPtr );
    ~LightBarController();

///////////////////////////////////////////////////////////////////////////////
/// @name  Initialize()
/// @brief LightBar specific initialization
/// @return void
///////////////////////////////////////////////////////////////////////////////
    void Initialize();

///////////////////////////////////////////////////////////////////////////////
/// @name  GetLedActiveAnimation
/// @brief return currently active and queued requests
/// @return void
///////////////////////////////////////////////////////////////////////////////
    Animation GetLedActiveAnimation();

    //POST ui/lb –{ “anim”: alexa_speaking, “type”: queue/immediate, “repeat”:T/F}
///////////////////////////////////////////////////////////////////////////////
/// @name  GetLedActiveAnimation
/// @brief return currently active and queued requests
/// @return void
///////////////////////////////////////////////////////////////////////////////
    void PlayLedAnimation( Animation & playAnimation, const Callback<Animation>& resp );

///////////////////////////////////////////////////////////////////////////////
/// @name   HandlePutAnimationRequest
/// @brief- Handles New animation Put request
/// @return void
///////////////////////////////////////////////////////////////////////////////
    void HandlePutAnimationRequest( const Animation &req,
                                    const Callback<Animation>& resp );

///////////////////////////////////////////////////////////////////////////////
/// @name   HandleGetAnimationRequest
/// @brief- Handles Get request to get current and queued animation requests
/// @return void
///////////////////////////////////////////////////////////////////////////////
    void HandleGetAnimationRequest( const Callback<Animation>& resp );


///////////////////////////////////////////////////////////////////////////////
/// @name   HandleDeleteAnimationRequest
/// @brief- Handles Get request to get current and queued animation requests
/// @return void
///////////////////////////////////////////////////////////////////////////////
    void HandleDeleteAnimationRequest( const Animation& req,
                                       const Callback<Animation>& resp );

private:
    /// Disable copies
    LightBarController( const LightBarController& ) = delete;
    LightBarController& operator=( const LightBarController& ) = delete;

    ///Register Lightbar Endpoints
    void RegisterLightBarEndPoints();

    void StopLedAnimation( bool stopImmediate, const Callback<Animation>& resp );

    void HandleLpmResponse( LBCSResponse_t response, const Callback<Animation>& resp );

    bool HandleLpmNotification( LBCSAnimationStateEvent_t notification );
private:
    ///product contorller reference, not sure it is needed
    ProductController& m_productController;

    std::shared_ptr<FrontDoorClientIF> m_frontdoorClientPtr;
    unsigned int currentAnimation_messageid;
    unsigned int nextAnimation_messageid;

    AnimationParams currentAnimation;
    AnimationParams nextAnimation;

    LpmClientIF::LpmClientPtr    m_lpmClient;

    AnimationIntentIdMapping    m_intentIdMap;
};
} //namespace ProductApp

