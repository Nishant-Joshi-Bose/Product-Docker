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
/// @name   CustomProductAudioService::CustomProductAudioService
///
/// @param  ProfessorProductController& ProductController
///
////////////////////////////////////////////////////////////////////////////////////////////////////
CustomProductAudioService::CustomProductAudioService( ProfessorProductController& ProductController )
    : ProductAudioService( ProductController.GetTask( ),
                           ProductController.GetMessageHandler() )
{
    BOSE_DEBUG( s_logger, __func__ );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   CustomProductAudioService::RegisterAudioPathEvents
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductAudioService::RegisterAudioPathEvents()
{
    BOSE_DEBUG( s_logger, __func__ );
    m_APPointer = APProductFactory::Create( "ProductAudioService-APProduct", m_ProductTask );
    RegisterCommonAudioPathEvents();
    ConnectToAudioPath();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   CustomProductAudioService::RegisterFrontDoorEvents
///
/// @brief  On Professor, it register for put/post/get FrontDoor request for
///         bass, treble, center, surround, gainOffset, avSync, mode, contentType
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductAudioService::RegisterFrontDoorEvents()
{
    BOSE_DEBUG( s_logger, __func__ );

    m_FrontDoorClientIF = FrontDoor::FrontDoorClient::Create( "ProductAudioService-FrontDoor" );

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// Endpoint /audio/bass - register ProductController as handler for POST/PUT/GET requests
    ///////////////////////////////////////////////////////////////////////////////////////////////
    auto getBassAction = [this]( )
    {
        return m_AudioSettingsMgr->GetBass( );
    };
    auto setBassAction = [this]( const ProductPb::AudioBassLevel val )
    {
        m_AudioSettingsMgr->SetBass( val );
    };
    m_AudioBassSetting = std::unique_ptr<AudioSetting<ProductPb::AudioBassLevel>>( new AudioSetting<ProductPb::AudioBassLevel>
                                                                                   ( "/audio/bass",
                                                                                           getBassAction,
                                                                                           setBassAction,
                                                                                           m_FrontDoorClientIF,
                                                                                           m_ProductTask ) );

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// Endpoint /audio/treble - register ProductController as handler for POST/PUT/GET requests
    ///////////////////////////////////////////////////////////////////////////////////////////////
    auto getTrebleAction = [ this ]( )
    {
        return m_AudioSettingsMgr->GetTreble( );
    };
    auto setTrebleAction = [ this ]( const ProductPb::AudioTrebleLevel val )
    {
        m_AudioSettingsMgr->SetTreble( val );
    };
    m_AudioTrebleSetting = std::unique_ptr<AudioSetting<ProductPb::AudioTrebleLevel>>( new AudioSetting<ProductPb::AudioTrebleLevel>
                           ( "/audio/treble",
                             getTrebleAction,
                             setTrebleAction,
                             m_FrontDoorClientIF,
                             m_ProductTask ) );

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// Endpoint /audio/center - register ProductController as handler for POST/PUT/GET requests
    ///////////////////////////////////////////////////////////////////////////////////////////////
    auto getCenterAction = [ this ]( )
    {
        return m_AudioSettingsMgr->GetCenter( );
    };
    auto setCenterAction = [ this ]( const ProductPb::AudioCenterLevel val )
    {
        m_AudioSettingsMgr->SetCenter( val );
    };
    m_AudioCenterSetting = std::unique_ptr<AudioSetting<ProductPb::AudioCenterLevel>>( new AudioSetting<ProductPb::AudioCenterLevel>
                           ( "/audio/center",
                             getCenterAction,
                             setCenterAction,
                             m_FrontDoorClientIF,
                             m_ProductTask ) );

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// Endpoint /audio/surround - register ProductController as handler for POST/PUT/GET requests
    ///////////////////////////////////////////////////////////////////////////////////////////////
    auto getSurroundAction = [ this ]( )
    {
        return m_AudioSettingsMgr->GetSurround( );
    };
    auto setSurroundAction = [ this ]( const ProductPb::AudioSurroundLevel val )
    {
        m_AudioSettingsMgr->SetSurround( val );
    };
    m_AudioSurroundSetting = std::unique_ptr<AudioSetting<ProductPb::AudioSurroundLevel>>( new AudioSetting<ProductPb::AudioSurroundLevel>
                             ( "/audio/surround",
                               getSurroundAction,
                               setSurroundAction,
                               m_FrontDoorClientIF,
                               m_ProductTask ) );

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// Endpoint /audio/gainOffset - register ProductController as handler for POST/PUT/GET requests
    ///////////////////////////////////////////////////////////////////////////////////////////////
    auto getGainOffsetAction = [ this ]( )
    {
        return m_AudioSettingsMgr->GetGainOffset( );
    };
    auto setGainOffsetAction = [ this ]( const ProductPb::AudioGainOffset val )
    {
        m_AudioSettingsMgr->SetGainOffset( val );
    };
    m_AudioGainOffsetSetting =  std::unique_ptr<AudioSetting<ProductPb::AudioGainOffset>>( new AudioSetting<ProductPb::AudioGainOffset>
                                ( "/audio/gainOffset",
                                  getGainOffsetAction,
                                  setGainOffsetAction,
                                  m_FrontDoorClientIF,
                                  m_ProductTask ) );

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// Endpoint /audio/avSync - register ProductController as handler for POST/PUT/GET requests
    //////////////////////////////////////////////////////////////////////////////////////////////
    auto getAvSyncAction = [ this ]( )
    {
        return m_AudioSettingsMgr->GetAvSync( );
    };
    auto setAvSyncAction = [ this ]( const ProductPb::AudioAvSync val )
    {
        m_AudioSettingsMgr->SetAvSync( val );
    };
    m_AudioAvSyncsetSetting = std::unique_ptr<AudioSetting<ProductPb::AudioAvSync>>( new AudioSetting<ProductPb::AudioAvSync>
                              ( "/audio/avSync",
                                getAvSyncAction,
                                setAvSyncAction,
                                m_FrontDoorClientIF,
                                m_ProductTask ) );

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// Endpoint /audio/mode - register ProductController as handler for POST/PUT/GET requests
    //////////////////////////////////////////////////////////////////////////////////////////////
    auto getModeAction = [ this ]( )
    {
        return m_AudioSettingsMgr->GetMode( );
    };
    auto setModeAction = [ this ]( const ProductPb::AudioMode val )
    {
        m_AudioSettingsMgr->SetMode( val );
    };
    m_AudioModeSetting = std::unique_ptr<AudioSetting<ProductPb::AudioMode>>( new AudioSetting<ProductPb::AudioMode>
                                                                              ( "/audio/mode",
                                                                                getModeAction,
                                                                                setModeAction,
                                                                                m_FrontDoorClientIF,
                                                                                m_ProductTask ) );

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// Endpoint /audio/contentType - register ProductController as handler for POST/PUT/GET requests
    //////////////////////////////////////////////////////////////////////////////////////////////
    auto getContentTypeAction = [ this ]( )
    {
        return m_AudioSettingsMgr->GetContentType( );
    };
    auto setContentTypeAction = [ this ]( const ProductPb::AudioContentType val )
    {
        m_AudioSettingsMgr->SetContentType( val );
    };
    m_AudioContentTypeSetting = std::unique_ptr<AudioSetting<ProductPb::AudioContentType>>( new AudioSetting<ProductPb::AudioContentType>
                                ( "/audio/contentType",
                                  getContentTypeAction,
                                  setContentTypeAction,
                                  m_FrontDoorClientIF,
                                  m_ProductTask ) );
}

}// namespace ProductApp
