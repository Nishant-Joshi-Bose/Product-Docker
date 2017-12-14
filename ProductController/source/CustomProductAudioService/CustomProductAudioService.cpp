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

constexpr char kBassEndPoint            [] = "/audio/bass";
constexpr char kTrebleEndPoint          [] = "/audio/treble";
constexpr char kCenterEndPoint          [] = "/audio/center";
constexpr char kSurroundEndPoint        [] = "/audio/surround";
constexpr char kGainOffsetEndPoint      [] = "/audio/gainOffset";
constexpr char kAvSyncEndPoint          [] = "/audio/avSync";
constexpr char kModeEndPoint            [] = "/audio/mode";
constexpr char kContentTypeEndPoint     [] = "/audio/contentType";
constexpr char kDualMonoSelectEndPoint  [] = "/audio/dualMonoSelect";

namespace ProductApp
{
////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   CustomProductAudioService::CustomProductAudioService
///
/// @param  ProfessorProductController& ProductController
///
////////////////////////////////////////////////////////////////////////////////////////////////////
CustomProductAudioService::CustomProductAudioService( ProfessorProductController& ProductController ):
    ProductAudioService( ProductController.GetTask( ),
                         ProductController.GetMessageHandler() ),
    m_AudioSettingsMgr( std::make_shared<CustomAudioSettingsManager> () )
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
    auto setBassAction = [this]( const ProductPb::AudioBassLevel & val )
    {
        return m_AudioSettingsMgr->SetBass( val );
    };
    m_AudioBassSetting = std::unique_ptr<AudioSetting<ProductPb::AudioBassLevel>>( new AudioSetting<ProductPb::AudioBassLevel>
                                                                                   ( kBassEndPoint,
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
    auto setTrebleAction = [ this ]( const ProductPb::AudioTrebleLevel & val )
    {
        return m_AudioSettingsMgr->SetTreble( val );
    };
    m_AudioTrebleSetting = std::unique_ptr<AudioSetting<ProductPb::AudioTrebleLevel>>( new AudioSetting<ProductPb::AudioTrebleLevel>
                           ( kTrebleEndPoint,
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
    auto setCenterAction = [ this ]( const ProductPb::AudioCenterLevel & val )
    {
        return m_AudioSettingsMgr->SetCenter( val );
    };
    m_AudioCenterSetting = std::unique_ptr<AudioSetting<ProductPb::AudioCenterLevel>>( new AudioSetting<ProductPb::AudioCenterLevel>
                           ( kCenterEndPoint,
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
    auto setSurroundAction = [ this ]( const ProductPb::AudioSurroundLevel & val )
    {
        return m_AudioSettingsMgr->SetSurround( val );
    };
    m_AudioSurroundSetting = std::unique_ptr<AudioSetting<ProductPb::AudioSurroundLevel>>( new AudioSetting<ProductPb::AudioSurroundLevel>
                             ( kSurroundEndPoint,
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
    auto setGainOffsetAction = [ this ]( const ProductPb::AudioGainOffset & val )
    {
        return m_AudioSettingsMgr->SetGainOffset( val );
    };
    m_AudioGainOffsetSetting =  std::unique_ptr<AudioSetting<ProductPb::AudioGainOffset>>( new AudioSetting<ProductPb::AudioGainOffset>
                                ( kGainOffsetEndPoint,
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
    auto setAvSyncAction = [ this ]( const ProductPb::AudioAvSync & val )
    {
        return m_AudioSettingsMgr->SetAvSync( val );
    };
    m_AudioAvSyncsetSetting = std::unique_ptr<AudioSetting<ProductPb::AudioAvSync>>( new AudioSetting<ProductPb::AudioAvSync>
                              ( kAvSyncEndPoint,
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
    auto setModeAction = [ this ]( const ProductPb::AudioMode & val )
    {
        return m_AudioSettingsMgr->SetMode( val );
    };
    m_AudioModeSetting = std::unique_ptr<AudioSetting<ProductPb::AudioMode>>( new AudioSetting<ProductPb::AudioMode>
                                                                              ( kModeEndPoint,
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
    auto setContentTypeAction = [ this ]( const ProductPb::AudioContentType & val )
    {
        return m_AudioSettingsMgr->SetContentType( val );
    };
    m_AudioContentTypeSetting = std::unique_ptr<AudioSetting<ProductPb::AudioContentType>>( new AudioSetting<ProductPb::AudioContentType>
                                ( kContentTypeEndPoint,
                                  getContentTypeAction,
                                  setContentTypeAction,
                                  m_FrontDoorClientIF,
                                  m_ProductTask ) );

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// Endpoint /audio/dualMonoSelect - register ProductController as handler for POST/PUT/GET requests
    //////////////////////////////////////////////////////////////////////////////////////////////
    auto getDualMonoSelectAction = [ this ]( )
    {
        return m_AudioSettingsMgr->GetDualMonoSelect( );
    };
    auto setDualMonoSelectAction = [ this ]( const ProductPb::AudioDualMonoSelect & val )
    {
        return m_AudioSettingsMgr->SetDualMonoSelect( val );
    };
    m_DualMonoSelectSetting = std::unique_ptr<AudioSetting<ProductPb::AudioDualMonoSelect>>( new AudioSetting<ProductPb::AudioDualMonoSelect>
                              ( kDualMonoSelectEndPoint,
                                getDualMonoSelectAction,
                                setDualMonoSelectAction,
                                m_FrontDoorClientIF,
                                m_ProductTask ) );

}

}// namespace ProductApp
