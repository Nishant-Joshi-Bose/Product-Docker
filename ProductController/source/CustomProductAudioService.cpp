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

//static DPrint s_logger( "CustomProductAudioService" );

namespace ProductApp
{
////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   CustomProductAudioService::CustomProductAudioService
///
/// @param  NotifyTargetTaskIF* task
///
/// @param  Callback< ProductMessage > ProductNotify
///
////////////////////////////////////////////////////////////////////////////////////////////////////
CustomProductAudioService::CustomProductAudioService( ProfessorProductController& ProductController )
    : ProductAudioService( ProductController ),
      m_ProductNotify( ProductController.GetMessageHandler( ) )
{
    BOSE_DEBUG( s_logger, __func__ );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   CustomProductAudioService::RegisterAudioPathEvent
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductAudioService::RegisterAudioPathEvent()
{
    BOSE_DEBUG( s_logger, __func__ );
    m_APPointer = APProductFactory::Create( "ProductAudioService-APProduct", m_ProductTask );
    RegisterCommonAudioPathEvent();
    //TODO: register for custom AudioPath event
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
    m_AudioBassSetting = std::make_shared<AudioSetting<ProductPb::AudioBassLevel>> ( "/audio/bass",
                         [ this ]( )
    {
        BOSE_INFO( s_logger, "Sisi - bass value: %s", ProtoToMarkup::ToJson( m_AudioSettingsMgr->GetBass( ), false ).c_str() );
        return m_AudioSettingsMgr->GetBass( );
    },
    [ this ]( const ProductPb::AudioBassLevel & val )
    {
        m_AudioSettingsMgr->SetBass( val );
    },
    m_FrontDoorClientIF, m_ProductTask );

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// Endpoint /audio/treble - register ProductController as handler for POST/PUT/GET requests
    ///////////////////////////////////////////////////////////////////////////////////////////////
    m_AudioTrebleSetting = std::make_shared<AudioSetting<ProductPb::AudioTrebleLevel>>( "/audio/treble",
                           [ this ]( )
    {
        return m_AudioSettingsMgr->GetTreble( );
    },
    [ this ]( const ProductPb::AudioTrebleLevel & val )
    {
        m_AudioSettingsMgr->SetTreble( val );
    },
    m_FrontDoorClientIF, m_ProductTask );

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// Endpoint /audio/center - register ProductController as handler for POST/PUT/GET requests
    ///////////////////////////////////////////////////////////////////////////////////////////////
    m_AudioCenterSetting = std::make_shared<AudioSetting<ProductPb::AudioCenterLevel>>( "/audio/center",
                           [ this ]( )
    {
        return m_AudioSettingsMgr->GetCenter( );
    },
    [ this ]( const ProductPb::AudioCenterLevel & val )
    {
        m_AudioSettingsMgr->SetCenter( val );
    },
    m_FrontDoorClientIF, m_ProductTask );

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// Endpoint /audio/surround - register ProductController as handler for POST/PUT/GET requests
    ///////////////////////////////////////////////////////////////////////////////////////////////
    m_AudioSurroundSetting = std::make_shared<AudioSetting<ProductPb::AudioSurroundLevel>>( "/audio/surround",
                             [ this ]( )
    {
        return m_AudioSettingsMgr->GetSurround( );
    },
    [ this ]( const ProductPb::AudioSurroundLevel & val )
    {
        m_AudioSettingsMgr->SetSurround( val );
    },
    m_FrontDoorClientIF, m_ProductTask );

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// Endpoint /audio/gainOffset - register ProductController as handler for POST/PUT/GET requests
    ///////////////////////////////////////////////////////////////////////////////////////////////
    m_AudioGainOffsetSetting = std::make_shared<AudioSetting<ProductPb::AudioGainOffset>>( "/audio/gainOffset",
                               [ this ]( )
    {
        return m_AudioSettingsMgr->GetGainOffset( );
    },
    [ this ]( const ProductPb::AudioGainOffset & val )
    {
        m_AudioSettingsMgr->SetGainOffset( val );
    },
    m_FrontDoorClientIF, m_ProductTask );

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// Endpoint /audio/avSync - register ProductController as handler for POST/PUT/GET requests
    //////////////////////////////////////////////////////////////////////////////////////////////
    m_AudioAvSyncsetSetting = std::make_shared<AudioSetting<ProductPb::AudioAvSync>>( "/audio/avSync",
                              [ this ]( )
    {
        return m_AudioSettingsMgr->GetAvSync( );
    },
    [ this ]( const ProductPb::AudioAvSync & val )
    {
        m_AudioSettingsMgr->SetAvSync( val );
    },
    m_FrontDoorClientIF, m_ProductTask );

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// Endpoint /audio/mode - register ProductController as handler for POST/PUT/GET requests
    //////////////////////////////////////////////////////////////////////////////////////////////
    m_AudioModeSetting = std::make_shared<AudioSetting<ProductPb::AudioMode>>( "/audio/mode", [ this ]( )
    {
        return m_AudioSettingsMgr->GetMode( );
    },
    [ this ]( const ProductPb::AudioMode & val )
    {
        m_AudioSettingsMgr->SetMode( val );
    },
    m_FrontDoorClientIF, m_ProductTask );

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// Endpoint /audio/contentType - register ProductController as handler for POST/PUT/GET requests
    //////////////////////////////////////////////////////////////////////////////////////////////
    m_AudioContentTypeSetting = std::make_shared<AudioSetting<ProductPb::AudioContentType>>( "/audio/contentType",
                                [ this ]( )
    {
        return m_AudioSettingsMgr->GetContentType( );
    },
    [ this ]( const ProductPb::AudioContentType & val )
    {
        m_AudioSettingsMgr->SetContentType( val );
    },
    m_FrontDoorClientIF, m_ProductTask );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   CustomProductAudioService::ConnectCallback
///
/// @param  bool connect
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductAudioService::ConnectCallback( bool connect )
{
    BOSE_DEBUG( s_logger, "CustomProductAudioService::ConnectCallback: connect = %s", connect ? "true" : "false" );
    ProductMessage message;
    message.mutable_audiopathstatus( )->set_connected( connect );
    m_ProductNotify.Send( message );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductAudioService::ConnectCallback
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductAudioService::DisconnectCallback()
{
    BOSE_DEBUG( s_logger, "CustomProductAudioService::DisconnectCallback" );
    ProductMessage message;
    message.mutable_audiopathstatus( )->set_connected( false );
    m_ProductNotify.Send( message );
}


}// namespace ProductApp
