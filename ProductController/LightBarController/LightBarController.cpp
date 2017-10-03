////////////////////////////////////////////////////////////////////////////////
///// @file   LightBarController.cpp
///// @brief  Implements Eddie LightBar controller class.
/////
///// @attention Copyright 2017 Bose Corporation, Framingham, MA
//////////////////////////////////////////////////////////////////////////////////
#include <functional>
#include "FrontDoorClient.h"
#include "LightBarController.h"
#include "ProductController.h"
#include "LEDAnimation_Manifest.h"
#include "LpmClientFactory.h"
#include "SyncCallback.h"

static DPrint s_logger( "LightBarController" );
using namespace ::LightBarController::Protobuf;

namespace ProductApp
{
LightBarController :: LightBarController( ProductController& controller, const std::shared_ptr<FrontDoorClientIF>& fd_client, LpmClientIF::LpmClientPtr clientPtr ):
    m_productController( controller ),
    m_frontdoorClientPtr( fd_client ),
    m_lpmClient( clientPtr ),
    m_intentIdMap()
{
    BOSE_INFO( s_logger, __func__ );
    //initializing animation objects to no animation
    currentAnimation.set_value( "none" );
    nextAnimation.set_value( "none" );
}
LightBarController :: ~LightBarController()
{
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void LightBarController::Initialize()
{
    //ui/lightbar end point registration with front door
    RegisterLightBarEndPoints();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void LightBarController::RegisterLightBarEndPoints()
{
    auto func = std::bind( &LightBarController::HandleLpmNotification, this, std::placeholders::_1 );
    AsyncCallback<LBCSAnimationStateEvent_t>notification_cb( func, m_productController.GetTask() );

    m_lpmClient->RegisterEvent<LBCSAnimationStateEvent_t>( IPC_LBCS_ANIM_STATE_EVENT , notification_cb );

    //register lightbar put endpoint
    AsyncCallback<Animation , Callback<Animation>> putAnimationReqCb(
                                                    std::bind( &LightBarController::HandlePutAnimationRequest ,
                                                               this,
                                                               std::placeholders::_1,
                                                               std::placeholders::_2
                                                             ),
                                                    m_productController.GetTask() );
    m_frontdoorClientPtr->RegisterPut<Animation>( "/ui/lightbar", putAnimationReqCb );

    //register lightbar get endpoint
    AsyncCallback< Callback<Animation>> getAnimationReqCb(
                                         std::bind(
                                             &LightBarController::HandleGetAnimationRequest,
                                             this,
                                             std::placeholders::_1
                                         ),
                                         m_productController.GetTask() );
    m_frontdoorClientPtr->RegisterGet( "/ui/lightbar", getAnimationReqCb );

    //register lightbar delete endpoint
    AsyncCallback <Animation, Callback<Animation>> stopAnimationReqCb(
                                                    std::bind(
                                                        &LightBarController::HandleDeleteAnimationRequest,
                                                        this,
                                                        std::placeholders::_1,
                                                        std::placeholders::_2 ),
                                                    m_productController.GetTask() );
    m_frontdoorClientPtr->RegisterDelete<Animation> ( "/ui/lightbar", stopAnimationReqCb );
}
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
Animation LightBarController::GetLedActiveAnimation()
{
    Animation response;

    if( currentAnimation.value() != "none" )
        response.mutable_current()->CopyFrom( currentAnimation );

    if( nextAnimation.value() != "none" )
        response.mutable_next()->CopyFrom( nextAnimation );

    return response;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void LightBarController :: PlayLedAnimation( Animation & m_playAnimation, const Callback<Animation>& resp )
{
    BOSE_INFO( s_logger, __func__ );

    //All new requests goes to "next"
    if( m_playAnimation.has_next() )
        nextAnimation = m_playAnimation.next();
    else
        return;

    LBCSStartAnimation_t request;

    //fill request information

    request.set_animationid( m_intentIdMap.GetIdFromIntent( m_playAnimation.next().value() ) );
    if( m_playAnimation.next().transition() == "immediate" )
        request.set_immediateflag( true );
    else
        request.set_immediateflag( false );

    request.set_repeatflag( m_playAnimation.next().repeat() );

    //send animation to LPM client for playback
    //TBD - should move to registration...
    auto func = std::bind( &LightBarController::HandleLpmResponse, this, std::placeholders::_1, resp );
    AsyncCallback<LBCSResponse_t>response_cb( func, m_productController.GetTask() );
    BOSE_INFO( s_logger, "request id - %d", request.animationid() );
    m_lpmClient->LBCSStartAnim( request, response_cb, IPC_DEVICE_LPM );
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void LightBarController :: StopLedAnimation( bool stopImmediate, const Callback<Animation>& resp )
{
    BOSE_INFO( s_logger, __func__ );
    auto func = std::bind( &LightBarController::HandleLpmResponse, this, std::placeholders::_1, resp );

    AsyncCallback<LBCSResponse_t>response_cb( func, m_productController.GetTask() );
    if( !stopImmediate )
        m_lpmClient->LBCSStopAnimation( response_cb, IPC_DEVICE_LPM );
    else
        m_lpmClient->LBCSAbortAnimation( response_cb, IPC_DEVICE_LPM );
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void LightBarController :: HandleLpmResponse( LBCSResponse_t lpmResponse, const Callback<Animation>& resp )
{
    BOSE_INFO( s_logger, __func__ );

    //when success respond with response
    if( lpmResponse.lbcsresponsesuccess() )
        resp( GetLedActiveAnimation() );
    else
    {
        //TBD - Not sure what to do when response is failed. IMO a messag id will
        //help differentiate request response
        Animation respAnimation;
        respAnimation.mutable_error()->set_description( "Intent not found" );

        if( currentAnimation.value() != "none" )
            respAnimation.mutable_current()->CopyFrom( currentAnimation );

        nextAnimation.set_value( "none" );

        respAnimation.mutable_next()->CopyFrom( nextAnimation );
        resp( respAnimation );
    }
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
bool LightBarController :: HandleLpmNotification( LBCSAnimationStateEvent_t lpmAnimationEvent )
{
    BOSE_INFO( s_logger, __func__ );

    //Animation stop can only receive for "current" and start can only received
    //for "next"
    if( lpmAnimationEvent.state() == ANIMATION_STARTED )
    {
        //get animation id to intent matching
        {
            //ideally id and next should match
            //Note: There can be an issue where multiple next are received before start is received
            //in that case, the start could be of different id compared to next.
            //Even if id in response will give animation name from a map, the other
            //parameters like "repeat" and "transition" cannot be figured out, so
            //need to think how to ensure the whole response is correct, not just name
            //"next" is not "current"

            //currentAnimation = nextAnimation;
            currentAnimation.set_value( m_intentIdMap.GetIntentFromId( lpmAnimationEvent.animationid() ) );
            currentAnimation.set_transition( nextAnimation.transition() );
            currentAnimation.set_repeat( nextAnimation.repeat() );
            nextAnimation.set_value( "none" );
        }
    }
    else if( lpmAnimationEvent.state() == ANIMATION_STOPPED )
    {
        //get animation id to intent matching
        {
            //current animation stopped, set active to "none"
            //send notification with queued only
            currentAnimation.set_value( "none" );
            currentAnimation.set_transition( "smooth" );
            currentAnimation.set_repeat( false );
        }
    }
    else
        std::cout << "unknown state of animation" << std::endl;

    m_frontdoorClientPtr->SendNotification( "/ui/lightbar" , GetLedActiveAnimation() );
    return true;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void LightBarController :: HandlePutAnimationRequest( const Animation &req,
                                                      const Callback <Animation>& resp )
{
    BOSE_INFO( s_logger, __func__ );
    Animation lreq = req;
    PlayLedAnimation( lreq, resp );
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void LightBarController :: HandleGetAnimationRequest( const Callback<Animation>& resp )
{
    Animation animation = GetLedActiveAnimation();

    resp.Send( animation );
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void LightBarController :: HandleDeleteAnimationRequest( const Animation& req,
                                                         const Callback<Animation>& resp )
{
    BOSE_INFO( s_logger, __func__ );
    Animation lreq = req;
    bool stopImmediate = false;
    if( lreq.current().has_transition() )
    {
        if( lreq.current().transition() == "immediate" )
            stopImmediate = true;
    }

    StopLedAnimation( stopImmediate, resp );

}

} //namespace ProductApp


