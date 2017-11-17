///////////////////////////////////////////////////////////////////////////////////////////////////
/// @file   CustomProductAudioService.cpp
/// @brief   This file contains source code for Professor specific behavior for
///         communicating with APProduct Server and APProduct related FrontDoor interaction
/// Copyright 2017 Bose Corporation
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "DPrint.h"
#include "AsyncCallback.h"
#include "APProductFactory.h"
#include "FrontDoorClient.h"
#include "CustomProductAudioService.h"

static DPrint s_logger( "CustomProductAudioService" );

namespace ProductApp
{

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   CustomProductAudioService::GetInstance
///
/// @param  NotifyTargetTaskIF* task
///
/// @param  Callback< ProductMessage > ProductNotify
///
/// @return This method returns a pointer to a CustomProductAudioService object.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
CustomProductAudioService* CustomProductAudioService::GetInstance( NotifyTargetTaskIF*        task,
                                                                   Callback< ProductMessage > ProductNotify )
{
    static CustomProductAudioService* instance = new CustomProductAudioService( task,
                                                                                ProductNotify );
    return instance;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   CustomProductAudioService::CustomProductAudioService
///
/// @param  NotifyTargetTaskIF* task
///
/// @param  Callback< ProductMessage > ProductNotify
///
////////////////////////////////////////////////////////////////////////////////////////////////////
CustomProductAudioService::CustomProductAudioService( NotifyTargetTaskIF*        task,
                                                      Callback< ProductMessage > ProductNotify )
    : ProductAudioService( task, ProductNotify )
{
    BOSE_DEBUG( s_logger, __func__ );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   CustomProductAudioService::~CustomProductAudioService
///
/// @brief  CustomProductAudioService class destructor
///
////////////////////////////////////////////////////////////////////////////////////////////////////
CustomProductAudioService::~CustomProductAudioService()
{
    DisconnectFrontDoor();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   CustomProductAudioService::Run
///
/// @brief  Register for APProduct event, and FrontDoor endpoints
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductAudioService::Run( )
{
    BOSE_DEBUG( s_logger, __func__ );
    RegisterAudioPathEvent();
    RegisterFrontDoorEvent();
    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   CustomProductAudioService::RegisterAudioPathEvent
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductAudioService::RegisterAudioPathEvent()
{
    BOSE_DEBUG( s_logger, __func__ );
    m_APPointer = APProductFactory::Create( "ProductAudioService-APProduct", m_mainTask );
    RegisterCommonAudioPathEvent();
    ConnectToAudioPath();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   CustomProductAudioService::RegisterFrontDoorEvent
///
/// @brief  On Professor, it register for put/post/get FrontDoor request for
///         bass, treble, center, surround, gainOffset, avSync, mode, contentType
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductAudioService::RegisterFrontDoorEvent()
{
    BOSE_DEBUG( s_logger, __func__ );

    m_FrontDoorClientIF = FrontDoor::FrontDoorClient::Create( "ProductAudioService-FrontDoor" );
    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// Endpoint /audio/bass - register ProductController as handler for POST/PUT/GET requests
    ///////////////////////////////////////////////////////////////////////////////////////////////
    {
        AsyncCallback<Callback<ProductPb::AudioBassLevel>> getBassCb( std::bind( &CustomProductAudioService::BassGetHandler,
                                                                                 this, std::placeholders::_1 ) , m_mainTask );
        m_registerGetBassCb = m_FrontDoorClientIF->RegisterGet( "/audio/bass", getBassCb );
    }
    {
        AsyncCallback<ProductPb::AudioBassLevel, Callback<ProductPb::AudioBassLevel>> postBassCb( std::bind( &CustomProductAudioService::BassPostHandler,
                                                                                   this, std::placeholders::_1, std::placeholders::_2 ) , m_mainTask );
        m_registerPostBassCb = m_FrontDoorClientIF->RegisterPost<ProductPb::AudioBassLevel>( "/audio/bass" , postBassCb );
    }
    {
        AsyncCallback<ProductPb::AudioBassLevel, Callback<ProductPb::AudioBassLevel>> putBassCb( std::bind( &CustomProductAudioService::BassPutHandler,
                                                                                   this, std::placeholders::_1, std::placeholders::_2 ) , m_mainTask );
        m_registerPutBassCb = m_FrontDoorClientIF->RegisterPut<ProductPb::AudioBassLevel>( "/audio/bass" , putBassCb );
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// Endpoint /audio/treble - register ProductController as handler for POST/PUT/GET requests
    ///////////////////////////////////////////////////////////////////////////////////////////////
    // Register the GET request callback with the FrontDoorClient
    {
        AsyncCallback<Callback<ProductPb::AudioTrebleLevel>> getTrebleCb( std::bind( &CustomProductAudioService::TrebleGetHandler,
                                                                                     this, std::placeholders::_1 ) , m_mainTask );
        m_registerGetTrebleCb = m_FrontDoorClientIF->RegisterGet( "/audio/treble", getTrebleCb );
    }
    {
        AsyncCallback<ProductPb::AudioTrebleLevel, Callback<ProductPb::AudioTrebleLevel>> postTrebleCb( std::bind( &CustomProductAudioService::TreblePostHandler,
                                                                                       this, std::placeholders::_1, std::placeholders::_2 ) , m_mainTask );
        m_registerPostTrebleCb = m_FrontDoorClientIF->RegisterPost<ProductPb::AudioTrebleLevel>( "/audio/treble" , postTrebleCb );
    }
    {
        AsyncCallback<ProductPb::AudioTrebleLevel, Callback<ProductPb::AudioTrebleLevel>> putTrebleCb( std::bind( &CustomProductAudioService::TreblePutHandler,
                                                                                       this, std::placeholders::_1, std::placeholders::_2 ) , m_mainTask );
        m_registerPutTrebleCb = m_FrontDoorClientIF->RegisterPut<ProductPb::AudioTrebleLevel>( "/audio/treble" , putTrebleCb );
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// Endpoint /audio/center - register ProductController as handler for POST/PUT/GET requests
    ///////////////////////////////////////////////////////////////////////////////////////////////
    {
        AsyncCallback<Callback<ProductPb::AudioCenterLevel>> getCenterCb( std::bind( &CustomProductAudioService::CenterGetHandler,
                                                                                     this, std::placeholders::_1 ) , m_mainTask );
        m_registerGetCenterCb = m_FrontDoorClientIF->RegisterGet( "/audio/center", getCenterCb );
    }
    {
        AsyncCallback<ProductPb::AudioCenterLevel, Callback<ProductPb::AudioCenterLevel>> postCenterCb( std::bind( &CustomProductAudioService::CenterPostHandler,
                                                                                       this, std::placeholders::_1, std::placeholders::_2 ) , m_mainTask );
        m_registerPostCenterCb = m_FrontDoorClientIF->RegisterPost<ProductPb::AudioCenterLevel>( "/audio/center" , postCenterCb );
    }
    {
        AsyncCallback<ProductPb::AudioCenterLevel, Callback<ProductPb::AudioCenterLevel>> putCenterCb( std::bind( &CustomProductAudioService::CenterPutHandler,
                                                                                       this, std::placeholders::_1, std::placeholders::_2 ) , m_mainTask );
        m_registerPutCenterCb = m_FrontDoorClientIF->RegisterPut<ProductPb::AudioCenterLevel>( "/audio/center" , putCenterCb );
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// Endpoint /audio/surround - register ProductController as handler for POST/PUT/GET requests
    ///////////////////////////////////////////////////////////////////////////////////////////////
    // Register the GET request callback with the FrontDoorClient
    {
        AsyncCallback<Callback<ProductPb::AudioSurroundLevel>> getSurroundCb( std::bind( &CustomProductAudioService::SurroundGetHandler,
                                                                              this, std::placeholders::_1 ) , m_mainTask );
        m_registerGetSurroundCb = m_FrontDoorClientIF->RegisterGet( "/audio/surround", getSurroundCb );
    }
    {
        AsyncCallback<ProductPb::AudioSurroundLevel, Callback<ProductPb::AudioSurroundLevel>> postSurroundCb( std::bind( &CustomProductAudioService::SurroundPostHandler,
                this, std::placeholders::_1, std::placeholders::_2 ) , m_mainTask );
        m_registerPostSurroundCb = m_FrontDoorClientIF->RegisterPost<ProductPb::AudioSurroundLevel>( "/audio/surround" , postSurroundCb );
    }
    {
        AsyncCallback<ProductPb::AudioSurroundLevel, Callback<ProductPb::AudioSurroundLevel>> putSurroundCb( std::bind( &CustomProductAudioService::SurroundPutHandler,
                this, std::placeholders::_1, std::placeholders::_2 ) , m_mainTask );
        m_registerPutSurroundCb = m_FrontDoorClientIF->RegisterPut<ProductPb::AudioSurroundLevel>( "/audio/surround" , putSurroundCb );
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// Endpoint /audio/gainOffset - register ProductController as handler for POST/PUT/GET requests
    ///////////////////////////////////////////////////////////////////////////////////////////////
    // Register the GET request callback with the FrontDoorClient
    {
        AsyncCallback<Callback<ProductPb::AudioGainOffset>> getGainOffsetCb( std::bind( &CustomProductAudioService::GainOffsetGetHandler,
                                                                                        this, std::placeholders::_1 ) , m_mainTask );
        m_registerGetGainOffsetCb = m_FrontDoorClientIF->RegisterGet( "/audio/gainOffset", getGainOffsetCb );
    }
    {
        AsyncCallback<ProductPb::AudioGainOffset, Callback<ProductPb::AudioGainOffset>> postGainOffsetCb( std::bind( &CustomProductAudioService::GainOffsetPostHandler,
                                                                                     this, std::placeholders::_1, std::placeholders::_2 ) , m_mainTask );
        m_registerPostGainOffsetCb = m_FrontDoorClientIF->RegisterPost<ProductPb::AudioGainOffset>( "/audio/gainOffset" , postGainOffsetCb );
    }
    {
        AsyncCallback<ProductPb::AudioGainOffset, Callback<ProductPb::AudioGainOffset>> putGainOffsetCb( std::bind( &CustomProductAudioService::GainOffsetPutHandler,
                                                                                     this, std::placeholders::_1, std::placeholders::_2 ) , m_mainTask );
        m_registerPutGainOffsetCb = m_FrontDoorClientIF->RegisterPut<ProductPb::AudioGainOffset>( "/audio/gainOffset" , putGainOffsetCb );
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// Endpoint /audio/avSync - register ProductController as handler for POST/PUT/GET requests
    ///////////////////////////////////////////////////////////////////////////////////////////////
    // Register the GET request callback with the FrontDoorClient
    {
        AsyncCallback<Callback<ProductPb::AudioAvSync>> getAvSyncCb( std::bind( &CustomProductAudioService::AvSyncGetHandler,
                                                                                this, std::placeholders::_1 ) , m_mainTask );
        m_registerGetAvSyncCb = m_FrontDoorClientIF->RegisterGet( "/audio/avSync", getAvSyncCb );
    }
    {
        AsyncCallback<ProductPb::AudioAvSync, Callback<ProductPb::AudioAvSync>> postAvSyncCb( std::bind( &CustomProductAudioService::AvSyncPostHandler,
                                                                             this, std::placeholders::_1, std::placeholders::_2 ) , m_mainTask );
        m_registerPostAvSyncCb = m_FrontDoorClientIF->RegisterPost<ProductPb::AudioAvSync>( "/audio/avSync" , postAvSyncCb );
    }
    {
        AsyncCallback<ProductPb::AudioAvSync, Callback<ProductPb::AudioAvSync>> putAvSyncCb( std::bind( &CustomProductAudioService::AvSyncPutHandler,
                                                                             this, std::placeholders::_1, std::placeholders::_2 ) , m_mainTask );
        m_registerPutAvSyncCb = m_FrontDoorClientIF->RegisterPut<ProductPb::AudioAvSync>( "/audio/avSync" , putAvSyncCb );
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// Endpoint /audio/mode - register ProductController as handler for POST/PUT/GET requests
    ///////////////////////////////////////////////////////////////////////////////////////////////
    // Register the GET request callback with the FrontDoorClient
    {
        AsyncCallback<Callback<ProductPb::AudioMode>> getModeCb( std::bind( &CustomProductAudioService::ModeGetHandler,
                                                                            this, std::placeholders::_1 ) , m_mainTask );
        m_registerGetModeCb = m_FrontDoorClientIF->RegisterGet( "/audio/mode", getModeCb );
    }
    {
        AsyncCallback<ProductPb::AudioMode, Callback<ProductPb::AudioMode>> postModeCb( std::bind( &CustomProductAudioService::ModePostHandler,
                                                                                        this, std::placeholders::_1, std::placeholders::_2 ) , m_mainTask );
        m_registerPostModeCb = m_FrontDoorClientIF->RegisterPost<ProductPb::AudioMode>( "/audio/mode" , postModeCb );
    }
    {
        AsyncCallback<ProductPb::AudioMode, Callback<ProductPb::AudioMode>> putModeCb( std::bind( &CustomProductAudioService::ModePutHandler,
                                                                                       this, std::placeholders::_1, std::placeholders::_2 ) , m_mainTask );
        m_registerPutModeCb = m_FrontDoorClientIF->RegisterPut<ProductPb::AudioMode>( "/audio/mode" , putModeCb );
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// Endpoint /audio/contentType - register ProductController as handler for POST/PUT/GET requests
    ///////////////////////////////////////////////////////////////////////////////////////////////
    // Register the GET request callback with the FrontDoorClient
    {
        AsyncCallback<Callback<ProductPb::AudioContentType>> getContentTypeCb( std::bind( &CustomProductAudioService::ContentTypeGetHandler,
                                                                               this, std::placeholders::_1 ) , m_mainTask );
        m_registerGetContentTypeCb = m_FrontDoorClientIF->RegisterGet( "/audio/contentType", getContentTypeCb );
    }
    {
        AsyncCallback<ProductPb::AudioContentType, Callback<ProductPb::AudioContentType>> postContentTypeCb( std::bind( &CustomProductAudioService::ContentTypePostHandler,
                                                                                       this, std::placeholders::_1, std::placeholders::_2 ) , m_mainTask );
        m_registerPostContentTypeCb = m_FrontDoorClientIF->RegisterPost<ProductPb::AudioContentType>( "/audio/contentType" , postContentTypeCb );
    }
    {
        AsyncCallback<ProductPb::AudioContentType, Callback<ProductPb::AudioContentType>> putContentTypeCb( std::bind( &CustomProductAudioService::ContentTypePutHandler,
                                                                                       this, std::placeholders::_1, std::placeholders::_2 ) , m_mainTask );
        m_registerPutContentTypeCb = m_FrontDoorClientIF->RegisterPut<ProductPb::AudioContentType>( "/audio/contentType" , putContentTypeCb );
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   CustomProductAudioService::BassGetHandler
///
/// @param  const Callback<ProductPb::AudioBassLevel> &resp
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductAudioService::BassGetHandler( const Callback<ProductPb::AudioBassLevel> &resp )
{
    BOSE_DEBUG( s_logger, __func__ );
    ProductPb::AudioBassLevel responseMsg = m_AudioSettingsMgr->GetBass();
    resp.Send( responseMsg );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   CustomProductAudioService::BassPostHandler
///
/// @param  const ProductPb::AudioBassLevel &req
///
/// @param  const Callback<ProductPb::AudioBassLevel> &resp
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductAudioService::BassPostHandler( const ProductPb::AudioBassLevel &req, const Callback<ProductPb::AudioBassLevel> &resp )
{
    BOSE_DEBUG( s_logger, __func__ );
    m_AudioSettingsMgr->SetBass( req );
    ProductPb::AudioBassLevel responseMsg = m_AudioSettingsMgr->GetBass();
    resp.Send( responseMsg );
    m_FrontDoorClientIF->SendNotification( "/audio/bass", responseMsg );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   CustomProductAudioService::BassPutHandler
///
/// @param  const ProductPb::AudioBassLevel &req
///
/// @param  const Callback<ProductPb::AudioBassLevel> &resp
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductAudioService::BassPutHandler( const ProductPb::AudioBassLevel &req, const Callback<ProductPb::AudioBassLevel> &resp )
{
    BOSE_DEBUG( s_logger, __func__ );
    m_AudioSettingsMgr->SetBass( req );
    ProductPb::AudioBassLevel responseMsg = m_AudioSettingsMgr->GetBass();
    resp.Send( responseMsg );
    m_FrontDoorClientIF->SendNotification( "/audio/bass", responseMsg );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   CustomProductAudioService::TrebleGetHandler
///
/// @param  const Callback<ProductPb::AudioTrebleLevel> &resp
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductAudioService::TrebleGetHandler( const Callback<ProductPb::AudioTrebleLevel> &resp )
{
    BOSE_DEBUG( s_logger, __func__ );
    ProductPb::AudioTrebleLevel responseMsg = m_AudioSettingsMgr->GetTreble();
    resp.Send( responseMsg );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   CustomProductAudioService::TreblePostHandler
///
/// @param  const ProductPb::AudioTrebleLevel &req
///
/// @param  const Callback<ProductPb::AudioTrebleLevel> &resp
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductAudioService::TreblePostHandler( const ProductPb::AudioTrebleLevel &req, const Callback<ProductPb::AudioTrebleLevel> &resp )
{
    BOSE_DEBUG( s_logger, __func__ );
    m_AudioSettingsMgr->SetTreble( req );
    ProductPb::AudioTrebleLevel responseMsg = m_AudioSettingsMgr->GetTreble();
    resp.Send( responseMsg );
    m_FrontDoorClientIF->SendNotification( "/audio/treble", responseMsg );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   CustomProductAudioService::TreblePutHandler
///
/// @param  const ProductPb::AudioTrebleLevel &req
///
/// @param  const Callback<ProductPb::AudioTrebleLevel> &resp
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductAudioService::TreblePutHandler( const ProductPb::AudioTrebleLevel &req, const Callback<ProductPb::AudioTrebleLevel> &resp )
{
    BOSE_DEBUG( s_logger, __func__ );
    m_AudioSettingsMgr->SetTreble( req );
    ProductPb::AudioTrebleLevel responseMsg = m_AudioSettingsMgr->GetTreble();;
    resp.Send( responseMsg );
    m_FrontDoorClientIF->SendNotification( "/audio/treble", responseMsg );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   CustomProductAudioService::CenterGetHandler
///
/// @param  const Callback<ProductPb::AudioCenterLevel> &resp
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductAudioService::CenterGetHandler( const Callback<ProductPb::AudioCenterLevel> &resp )
{
    BOSE_DEBUG( s_logger, __func__ );
    ProductPb::AudioCenterLevel responseMsg = m_AudioSettingsMgr->GetCenter();
    resp.Send( responseMsg );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   CustomProductAudioService::CenterPostHandler
///
/// @param  const ProductPb::AudioCenterLevel &req
///
/// @param  const Callback<ProductPb::AudioCenterLevel> &resp
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductAudioService::CenterPostHandler( const ProductPb::AudioCenterLevel &req, const Callback<ProductPb::AudioCenterLevel> &resp )
{
    BOSE_DEBUG( s_logger, __func__ );
    m_AudioSettingsMgr->SetCenter( req );
    ProductPb::AudioCenterLevel responseMsg = m_AudioSettingsMgr->GetCenter();
    resp.Send( responseMsg );
    m_FrontDoorClientIF->SendNotification( "/audio/center", responseMsg );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   CustomProductAudioService::CenterPutHandler
///
/// @param  const ProductPb::AudioCenterLevel &req
///
/// @param  const Callback<ProductPb::AudioCenterLevel> &resp
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductAudioService::CenterPutHandler( const ProductPb::AudioCenterLevel &req, const Callback<ProductPb::AudioCenterLevel> &resp )
{
    BOSE_DEBUG( s_logger, __func__ );
    m_AudioSettingsMgr->SetCenter( req );
    ProductPb::AudioCenterLevel responseMsg = m_AudioSettingsMgr->GetCenter();
    resp.Send( responseMsg );
    m_FrontDoorClientIF->SendNotification( "/audio/center", responseMsg );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   CustomProductAudioService::SurroundGetHandler
///
/// @param  const Callback<ProductPb::AudioSurroundLevel> &resp
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductAudioService::SurroundGetHandler( const Callback<ProductPb::AudioSurroundLevel> &resp )
{
    BOSE_DEBUG( s_logger, __func__ );
    ProductPb::AudioSurroundLevel responseMsg = m_AudioSettingsMgr->GetSurround();
    resp.Send( responseMsg );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   CustomProductAudioService::SurroundPostHandler
///
/// @param  const ProductPb::AudioSurroundLevel &req
///
/// @param  const Callback<ProductPb::AudioSurroundLevel> &resp
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductAudioService::SurroundPostHandler( const ProductPb::AudioSurroundLevel &req, const Callback<ProductPb::AudioSurroundLevel> &resp )
{
    BOSE_DEBUG( s_logger, __func__ );
    m_AudioSettingsMgr->SetSurround( req );
    ProductPb::AudioSurroundLevel responseMsg = m_AudioSettingsMgr->GetSurround();
    resp.Send( responseMsg );
    m_FrontDoorClientIF->SendNotification( "/audio/surround", responseMsg );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   CustomProductAudioService::SurroundPutHandler
///
/// @param  const ProductPb::AudioSurroundLevel &req
///
/// @param  const Callback<ProductPb::AudioSurroundLevel> &resp
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductAudioService::SurroundPutHandler( const ProductPb::AudioSurroundLevel &req, const Callback<ProductPb::AudioSurroundLevel> &resp )
{
    BOSE_DEBUG( s_logger, __func__ );
    m_AudioSettingsMgr->SetSurround( req );
    ProductPb::AudioSurroundLevel responseMsg = m_AudioSettingsMgr->GetSurround();
    resp.Send( responseMsg );
    m_FrontDoorClientIF->SendNotification( "/audio/surround", responseMsg );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   CustomProductAudioService::GainOffsetGetHandler
///
/// @param  const Callback<ProductPb::AudioGainOffset> &resp
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductAudioService::GainOffsetGetHandler( const Callback<ProductPb::AudioGainOffset> &resp )
{
    BOSE_DEBUG( s_logger, __func__ );
    ProductPb::AudioGainOffset responseMsg = m_AudioSettingsMgr->GetGainOffset();
    resp.Send( responseMsg );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   CustomProductAudioService::GainOffsetPostHandler
///
/// @param  const ProductPb::AudioGainOffset &req
///
/// @param  const Callback<ProductPb::AudioGainOffset> &resp
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductAudioService::GainOffsetPostHandler( const ProductPb::AudioGainOffset &req, const Callback<ProductPb::AudioGainOffset> &resp )
{
    BOSE_DEBUG( s_logger, __func__ );
    m_AudioSettingsMgr->SetGainOffset( req );
    ProductPb::AudioGainOffset responseMsg = m_AudioSettingsMgr->GetGainOffset();
    resp.Send( responseMsg );
    m_FrontDoorClientIF->SendNotification( "/audio/gainOffset", responseMsg );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   CustomProductAudioService::GainOffsetPutHandler
///
/// @param  const ProductPb::AudioGainOffset &req
///
/// @param  const Callback<ProductPb::AudioGainOffset> &resp
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductAudioService::GainOffsetPutHandler( const ProductPb::AudioGainOffset &req, const Callback<ProductPb::AudioGainOffset> &resp )
{
    BOSE_DEBUG( s_logger, __func__ );
    m_AudioSettingsMgr->SetGainOffset( req );
    ProductPb::AudioGainOffset responseMsg = m_AudioSettingsMgr->GetGainOffset();
    resp.Send( responseMsg );
    m_FrontDoorClientIF->SendNotification( "/audio/gainOffset", responseMsg );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   CustomProductAudioService::AvSyncGetHandler
///
/// @param  const Callback<ProductPb::AvSyncGetHandler> &resp
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductAudioService::AvSyncGetHandler( const Callback<ProductPb::AudioAvSync> &resp )
{
    BOSE_DEBUG( s_logger, __func__ );
    ProductPb::AudioAvSync responseMsg = m_AudioSettingsMgr->GetAvSync();
    resp.Send( responseMsg );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   CustomProductAudioService::AvSyncPostHandler
///
/// @param  const ProductPb::AudioAvSync &req
///
/// @param  const Callback<ProductPb::AudioAvSync> &resp
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductAudioService::AvSyncPostHandler( const ProductPb::AudioAvSync &req, const Callback<ProductPb::AudioAvSync> &resp )
{
    BOSE_DEBUG( s_logger, __func__ );
    m_AudioSettingsMgr->SetAvSync( req );
    ProductPb::AudioAvSync responseMsg = m_AudioSettingsMgr->GetAvSync();
    resp.Send( responseMsg );
    m_FrontDoorClientIF->SendNotification( "/audio/avSync", responseMsg );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   CustomProductAudioService::AvSyncPutHandler
///
/// @param  const ProductPb::AudioAvSync &req
///
/// @param  const Callback<ProductPb::AudioAvSync> &resp
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductAudioService::AvSyncPutHandler( const ProductPb::AudioAvSync &req, const Callback<ProductPb::AudioAvSync> &resp )
{
    BOSE_DEBUG( s_logger, __func__ );
    m_AudioSettingsMgr->SetAvSync( req );
    ProductPb::AudioAvSync responseMsg = m_AudioSettingsMgr->GetAvSync();
    resp.Send( responseMsg );
    m_FrontDoorClientIF->SendNotification( "/audio/avSync", responseMsg );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   CustomProductAudioService::ModeGetHandler
///
/// @param  const Callback<ProductPb::AudioMode> &resp
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductAudioService::ModeGetHandler( const Callback<ProductPb::AudioMode> &resp )
{
    BOSE_DEBUG( s_logger, __func__ );
    ProductPb::AudioMode responseMsg = m_AudioSettingsMgr->GetMode();
    resp.Send( responseMsg );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   CustomProductAudioService::ModePostHandler
///
/// @param  const ProductPb::AudioMode &req
///
/// @param  const Callback<ProductPb::AudioMode> &resp
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductAudioService::ModePostHandler( const ProductPb::AudioMode &req, const Callback<ProductPb::AudioMode> &resp )
{
    BOSE_DEBUG( s_logger, __func__ );
    m_AudioSettingsMgr->SetMode( req );
    ProductPb::AudioMode responseMsg = m_AudioSettingsMgr->GetMode();
    resp.Send( responseMsg );
    m_FrontDoorClientIF->SendNotification( "/audio/mode", responseMsg );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   CustomProductAudioService::ModePutHandler
///
/// @param  const ProductPb::AudioMode &req
///
/// @param  const Callback<ProductPb::AudioMode> &resp
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductAudioService::ModePutHandler( const ProductPb::AudioMode &req, const Callback<ProductPb::AudioMode> &resp )
{
    BOSE_DEBUG( s_logger, __func__ );
    m_AudioSettingsMgr->SetMode( req );
    ProductPb::AudioMode responseMsg = m_AudioSettingsMgr->GetMode();
    resp.Send( responseMsg );
    m_FrontDoorClientIF->SendNotification( "/audio/mode", responseMsg );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   CustomProductAudioService::ContentTypeGetHandler
///
/// @param  const Callback<ProductPb::AudioContentType> &resp
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductAudioService::ContentTypeGetHandler( const Callback<ProductPb::AudioContentType> &resp )
{
    BOSE_DEBUG( s_logger, __func__ );
    ProductPb::AudioContentType responseMsg = m_AudioSettingsMgr->GetContentType();
    resp.Send( responseMsg );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   CustomProductAudioService::ContentTypePostHandler
///
/// @param  const ProductPb::AudioContentType &req
///
/// @param  const Callback<ProductPb::AudioContentType> &resp
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductAudioService::ContentTypePostHandler( const ProductPb::AudioContentType &req, const Callback<ProductPb::AudioContentType> &resp )
{
    BOSE_DEBUG( s_logger, __func__ );
    m_AudioSettingsMgr->SetContentType( req );
    ProductPb::AudioContentType responseMsg = m_AudioSettingsMgr->GetContentType();
    resp.Send( responseMsg );
    m_FrontDoorClientIF->SendNotification( "/audio/contentType", responseMsg );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   CustomProductAudioService::ContentTypePutHandler
///
/// @param  const ProductPb::AudioContentType &req
///
/// @param  const Callback<ProductPb::AudioContentType> &resp
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductAudioService::ContentTypePutHandler( const ProductPb::AudioContentType &req, const Callback<ProductPb::AudioContentType> &resp )
{
    BOSE_DEBUG( s_logger, __func__ );
    m_AudioSettingsMgr->SetContentType( req );
    ProductPb::AudioContentType responseMsg = m_AudioSettingsMgr->GetContentType();
    resp.Send( responseMsg );
    m_FrontDoorClientIF->SendNotification( "/audio/contentType", responseMsg );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   CustomProductAudioService::DisconnectFrontDoor
///
/// @brief  Disconnect FrontDoor registrations
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductAudioService::DisconnectFrontDoor()
{
    BOSE_DEBUG( s_logger, __func__ );
    m_registerGetBassCb.Disconnect();
    m_registerPostBassCb.Disconnect();
    m_registerPutBassCb.Disconnect();
    m_registerGetTrebleCb.Disconnect();
    m_registerPostTrebleCb.Disconnect();
    m_registerPutTrebleCb.Disconnect();
    m_registerGetCenterCb.Disconnect();
    m_registerPostCenterCb.Disconnect();
    m_registerPutCenterCb.Disconnect();
    m_registerGetSurroundCb.Disconnect();
    m_registerPostSurroundCb.Disconnect();
    m_registerPutSurroundCb.Disconnect();
    m_registerGetGainOffsetCb.Disconnect();
    m_registerPostGainOffsetCb.Disconnect();
    m_registerPutGainOffsetCb.Disconnect();
    m_registerGetAvSyncCb.Disconnect();
    m_registerPostAvSyncCb.Disconnect();
    m_registerPutAvSyncCb.Disconnect();
    m_registerGetModeCb.Disconnect();
    m_registerPostModeCb.Disconnect();
    m_registerPutModeCb.Disconnect();
    m_registerGetContentTypeCb.Disconnect();
    m_registerPostContentTypeCb.Disconnect();
    m_registerPutContentTypeCb.Disconnect();
}

}// namespace ProductApp
