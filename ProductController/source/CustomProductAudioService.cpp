///////////////////////////////////////////////////////////////////////////////////////////////////
/// @file   CustomProductAudioService.cpp
/// @brief   This file contains source code for communicating with APProduct Server
///          for controlling audio volume, muting, latency and audio settings
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

CustomProductAudioService* CustomProductAudioService::GetInstance( NotifyTargetTaskIF*        task,
                                                                   Callback< ProductMessage > ProductNotify )
{
    static CustomProductAudioService* instance = new CustomProductAudioService( task,
                                                                                ProductNotify );
    return instance;
}

CustomProductAudioService::CustomProductAudioService( NotifyTargetTaskIF*        task,
                                                      Callback< ProductMessage > ProductNotify )
    : ProductAudioService( task, ProductNotify )
{
    BOSE_DEBUG( s_logger, __func__ );
}

CustomProductAudioService::~CustomProductAudioService()
{
    DisconnectFrontDoor();
}

bool CustomProductAudioService::Run( )
{
    BOSE_DEBUG( s_logger, __func__ );
    RegisterAudioPathEvent();
    RegisterFrontDoorEvent();
    return true;
}

void CustomProductAudioService::RegisterAudioPathEvent()
{
    BOSE_DEBUG( s_logger, __func__ );
    m_APPointer = APProductFactory::Create( "ProductAudioService-APProduct", m_mainTask );
    RegisterCommonAudioPathEvent();
    ConnectToAudioPath();
}

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

///////////////////////////////////////////////////////////////////////////////////////////////
/// Endpoint /audio/bass - callback functions for POST/PUT/GET request
///////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductAudioService::BassGetHandler( const Callback<ProductPb::AudioBassLevel> &resp )
{
    BOSE_DEBUG( s_logger, __func__ );
    ProductPb::AudioBassLevel responseMsg = m_AudioSettingsMgr->GetBass();
    resp.Send( responseMsg );
}

void CustomProductAudioService::BassPostHandler( const ProductPb::AudioBassLevel &req, const Callback<ProductPb::AudioBassLevel> &resp )
{
    BOSE_DEBUG( s_logger, __func__ );
    m_AudioSettingsMgr->SetBass( req );
    ProductPb::AudioBassLevel responseMsg = m_AudioSettingsMgr->GetBass();
    resp.Send( responseMsg );
    m_FrontDoorClientIF->SendNotification( "/audio/bass", responseMsg );
}

void CustomProductAudioService::BassPutHandler( const ProductPb::AudioBassLevel &req, const Callback<ProductPb::AudioBassLevel> &resp )
{
    BOSE_DEBUG( s_logger, __func__ );
    m_AudioSettingsMgr->SetBass( req );
    ProductPb::AudioBassLevel responseMsg = m_AudioSettingsMgr->GetBass();
    resp.Send( responseMsg );
    m_FrontDoorClientIF->SendNotification( "/audio/bass", responseMsg );
}

///////////////////////////////////////////////////////////////////////////////////////////////
/// Endpoint /audio/treble - callback functions for POST/PUT/GET request
///////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductAudioService::TrebleGetHandler( const Callback<ProductPb::AudioTrebleLevel> &resp )
{
    BOSE_DEBUG( s_logger, __func__ );
    ProductPb::AudioTrebleLevel responseMsg = m_AudioSettingsMgr->GetTreble();
    resp.Send( responseMsg );
}

void CustomProductAudioService::TreblePostHandler( const ProductPb::AudioTrebleLevel &req, const Callback<ProductPb::AudioTrebleLevel> &resp )
{
    BOSE_DEBUG( s_logger, __func__ );
    m_AudioSettingsMgr->SetTreble( req );
    ProductPb::AudioTrebleLevel responseMsg = m_AudioSettingsMgr->GetTreble();
    resp.Send( responseMsg );
    m_FrontDoorClientIF->SendNotification( "/audio/treble", responseMsg );
}

void CustomProductAudioService::TreblePutHandler( const ProductPb::AudioTrebleLevel &req, const Callback<ProductPb::AudioTrebleLevel> &resp )
{
    BOSE_DEBUG( s_logger, __func__ );
    m_AudioSettingsMgr->SetTreble( req );
    ProductPb::AudioTrebleLevel responseMsg = m_AudioSettingsMgr->GetTreble();;
    resp.Send( responseMsg );
    m_FrontDoorClientIF->SendNotification( "/audio/treble", responseMsg );
}

///////////////////////////////////////////////////////////////////////////////////////////////
/// Endpoint /audio/center - callback functions for POST/PUT/GET request
///////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductAudioService::CenterGetHandler( const Callback<ProductPb::AudioCenterLevel> &resp )
{
    BOSE_DEBUG( s_logger, __func__ );
    ProductPb::AudioCenterLevel responseMsg = m_AudioSettingsMgr->GetCenter();
    resp.Send( responseMsg );
}

void CustomProductAudioService::CenterPostHandler( const ProductPb::AudioCenterLevel &req, const Callback<ProductPb::AudioCenterLevel> &resp )
{
    BOSE_DEBUG( s_logger, __func__ );
    m_AudioSettingsMgr->SetCenter( req );
    ProductPb::AudioCenterLevel responseMsg = m_AudioSettingsMgr->GetCenter();
    resp.Send( responseMsg );
    m_FrontDoorClientIF->SendNotification( "/audio/center", responseMsg );
}

void CustomProductAudioService::CenterPutHandler( const ProductPb::AudioCenterLevel &req, const Callback<ProductPb::AudioCenterLevel> &resp )
{
    BOSE_DEBUG( s_logger, __func__ );
    m_AudioSettingsMgr->SetCenter( req );
    ProductPb::AudioCenterLevel responseMsg = m_AudioSettingsMgr->GetCenter();
    resp.Send( responseMsg );
    m_FrontDoorClientIF->SendNotification( "/audio/center", responseMsg );
}

///////////////////////////////////////////////////////////////////////////////////////////////
/// Endpoint /audio/surround - callback functions for POST/PUT/GET request
///////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductAudioService::SurroundGetHandler( const Callback<ProductPb::AudioSurroundLevel> &resp )
{
    BOSE_DEBUG( s_logger, __func__ );
    ProductPb::AudioSurroundLevel responseMsg = m_AudioSettingsMgr->GetSurround();
    resp.Send( responseMsg );
}

void CustomProductAudioService::SurroundPostHandler( const ProductPb::AudioSurroundLevel &req, const Callback<ProductPb::AudioSurroundLevel> &resp )
{
    BOSE_DEBUG( s_logger, __func__ );
    m_AudioSettingsMgr->SetSurround( req );
    ProductPb::AudioSurroundLevel responseMsg = m_AudioSettingsMgr->GetSurround();
    resp.Send( responseMsg );
    m_FrontDoorClientIF->SendNotification( "/audio/surround", responseMsg );
}

void CustomProductAudioService::SurroundPutHandler( const ProductPb::AudioSurroundLevel &req, const Callback<ProductPb::AudioSurroundLevel> &resp )
{
    BOSE_DEBUG( s_logger, __func__ );
    m_AudioSettingsMgr->SetSurround( req );
    ProductPb::AudioSurroundLevel responseMsg = m_AudioSettingsMgr->GetSurround();
    resp.Send( responseMsg );
    m_FrontDoorClientIF->SendNotification( "/audio/surround", responseMsg );
}

///////////////////////////////////////////////////////////////////////////////////////////////
/// Endpoint /audio/gainOffset - callback functions for POST/PUT/GET request
///////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductAudioService::GainOffsetGetHandler( const Callback<ProductPb::AudioGainOffset> &resp )
{
    BOSE_DEBUG( s_logger, __func__ );
    ProductPb::AudioGainOffset responseMsg = m_AudioSettingsMgr->GetGainOffset();
    resp.Send( responseMsg );
}

void CustomProductAudioService::GainOffsetPostHandler( const ProductPb::AudioGainOffset &req, const Callback<ProductPb::AudioGainOffset> &resp )
{
    BOSE_DEBUG( s_logger, __func__ );
    m_AudioSettingsMgr->SetGainOffset( req );
    ProductPb::AudioGainOffset responseMsg = m_AudioSettingsMgr->GetGainOffset();
    resp.Send( responseMsg );
    m_FrontDoorClientIF->SendNotification( "/audio/gainOffset", responseMsg );
}

void CustomProductAudioService::GainOffsetPutHandler( const ProductPb::AudioGainOffset &req, const Callback<ProductPb::AudioGainOffset> &resp )
{
    BOSE_DEBUG( s_logger, __func__ );
    m_AudioSettingsMgr->SetGainOffset( req );
    ProductPb::AudioGainOffset responseMsg = m_AudioSettingsMgr->GetGainOffset();
    resp.Send( responseMsg );
    m_FrontDoorClientIF->SendNotification( "/audio/gainOffset", responseMsg );
}

///////////////////////////////////////////////////////////////////////////////////////////////
/// Endpoint /audio/avSync - callback functions for POST/PUT/GET request
///////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductAudioService::AvSyncGetHandler( const Callback<ProductPb::AudioAvSync> &resp )
{
    BOSE_DEBUG( s_logger, __func__ );
    ProductPb::AudioAvSync responseMsg = m_AudioSettingsMgr->GetAvSync();
    resp.Send( responseMsg );
}

void CustomProductAudioService::AvSyncPostHandler( const ProductPb::AudioAvSync &req, const Callback<ProductPb::AudioAvSync> &resp )
{
    BOSE_DEBUG( s_logger, __func__ );
    m_AudioSettingsMgr->SetAvSync( req );
    ProductPb::AudioAvSync responseMsg = m_AudioSettingsMgr->GetAvSync();
    resp.Send( responseMsg );
    m_FrontDoorClientIF->SendNotification( "/audio/avSync", responseMsg );
}

void CustomProductAudioService::AvSyncPutHandler( const ProductPb::AudioAvSync &req, const Callback<ProductPb::AudioAvSync> &resp )
{
    BOSE_DEBUG( s_logger, __func__ );
    m_AudioSettingsMgr->SetAvSync( req );
    ProductPb::AudioAvSync responseMsg = m_AudioSettingsMgr->GetAvSync();
    resp.Send( responseMsg );
    m_FrontDoorClientIF->SendNotification( "/audio/avSync", responseMsg );
}

///////////////////////////////////////////////////////////////////////////////////////////////
/// Endpoint /audio/mode - callback functions for POST/PUT/GET request
///////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductAudioService::ModeGetHandler( const Callback<ProductPb::AudioMode> &resp )
{
    BOSE_DEBUG( s_logger, __func__ );
    ProductPb::AudioMode responseMsg = m_AudioSettingsMgr->GetMode();
    resp.Send( responseMsg );
}

void CustomProductAudioService::ModePostHandler( const ProductPb::AudioMode &req, const Callback<ProductPb::AudioMode> &resp )
{
    BOSE_DEBUG( s_logger, __func__ );
    m_AudioSettingsMgr->SetMode( req );
    ProductPb::AudioMode responseMsg = m_AudioSettingsMgr->GetMode();
    resp.Send( responseMsg );
    m_FrontDoorClientIF->SendNotification( "/audio/mode", responseMsg );
}

void CustomProductAudioService::ModePutHandler( const ProductPb::AudioMode &req, const Callback<ProductPb::AudioMode> &resp )
{
    BOSE_DEBUG( s_logger, __func__ );
    m_AudioSettingsMgr->SetMode( req );
    ProductPb::AudioMode responseMsg = m_AudioSettingsMgr->GetMode();
    resp.Send( responseMsg );
    m_FrontDoorClientIF->SendNotification( "/audio/mode", responseMsg );
}

///////////////////////////////////////////////////////////////////////////////////////////////
/// Endpoint /audio/contentType - callback functions for POST/PUT/GET request
///////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductAudioService::ContentTypeGetHandler( const Callback<ProductPb::AudioContentType> &resp )
{
    BOSE_DEBUG( s_logger, __func__ );
    ProductPb::AudioContentType responseMsg = m_AudioSettingsMgr->GetContentType();
    resp.Send( responseMsg );
}

void CustomProductAudioService::ContentTypePostHandler( const ProductPb::AudioContentType &req, const Callback<ProductPb::AudioContentType> &resp )
{
    BOSE_DEBUG( s_logger, __func__ );
    m_AudioSettingsMgr->SetContentType( req );
    ProductPb::AudioContentType responseMsg = m_AudioSettingsMgr->GetContentType();
    resp.Send( responseMsg );
    m_FrontDoorClientIF->SendNotification( "/audio/contentType", responseMsg );
}

void CustomProductAudioService::ContentTypePutHandler( const ProductPb::AudioContentType &req, const Callback<ProductPb::AudioContentType> &resp )
{
    BOSE_DEBUG( s_logger, __func__ );
    m_AudioSettingsMgr->SetContentType( req );
    ProductPb::AudioContentType responseMsg = m_AudioSettingsMgr->GetContentType();
    resp.Send( responseMsg );
    m_FrontDoorClientIF->SendNotification( "/audio/contentType", responseMsg );
}

///////////////////////////////////////////////////////////////////////////////////////////////
/// Cleanup functions
///////////////////////////////////////////////////////////////////////////////////////////////
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
