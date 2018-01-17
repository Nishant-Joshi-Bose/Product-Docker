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
//#include "CustomProductLpmHardwareInterface.h"
#include "ProtoToMarkup.h"
#include "SoundTouchInterface/ContentItem.pb.h"
#include "AutoLpmServiceMessages.pb.h"

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
    m_ProductLpmHardwareInterface( ProductController.GetLpmHardwareInterface( ) ),
    m_AudioSettingsMgr( std::unique_ptr<CustomAudioSettingsManager>( new CustomAudioSettingsManager() ) )
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
    //Initialize variables related to AudioPath
    m_InputRoute = 0x01;
    m_MainStreamAudioSettings.set_basslevel( m_AudioSettingsMgr->GetBass( ).value() );
    m_MainStreamAudioSettings.set_treblelevel( m_AudioSettingsMgr->GetTreble( ).value() );
    m_MainStreamAudioSettings.set_centerlevel( m_AudioSettingsMgr->GetCenter( ).value() );
    m_MainStreamAudioSettings.set_surroundlevel( m_AudioSettingsMgr->GetSurround( ).value() );
    m_MainStreamAudioSettings.set_gainoffsetdb( m_AudioSettingsMgr->GetGainOffset( ).value() );
    m_MainStreamAudioSettings.set_targetlatencyms( m_AudioSettingsMgr->GetAvSync( ).value() );
    m_MainStreamAudioSettings.set_audiomode( ModeNameToEnum( m_AudioSettingsMgr->GetMode( ).value() ) );
    m_MainStreamAudioSettings.set_contenttype( ContentTypeNameToEnum( m_AudioSettingsMgr->GetContentType( ).value() ) );
    m_MainStreamAudioSettings.set_dualmonoselect( DualMonoSelectNameToEnum( m_AudioSettingsMgr->GetDualMonoSelect( ).value() ) );

    m_APPointer = APProductFactory::Create( "ProductAudioService-APProduct", m_ProductTask );
    RegisterCommonAudioPathEvents();

    {
        Callback< std::string, Callback< std::string, std::string > > callback( std::bind( &CustomProductAudioService::GetMainStreamAudioSettingsCallback,
                                                                                this,
                                                                                std::placeholders::_1,
                                                                                std::placeholders::_2 ) );
        m_APPointer->RegisterForMainStreamAudioSettingsRequest( callback );
    }
    {
        Callback<std::string, std::string, Callback< bool > > callback( std::bind( &CustomProductAudioService::SetStreamConfigCallback,
                                                                                   this,
                                                                                   std::placeholders::_1,
                                                                                   std::placeholders::_2,
                                                                                   std::placeholders::_3 ) );
        m_APPointer->RegisterForSetStreamConfig( callback );
    }
    ConnectToAudioPath();
}


void CustomProductAudioService::GetMainStreamAudioSettingsCallback( std::string contentItem,  const Callback<std::string, std::string> cb )
{
    BOSE_DEBUG( s_logger, __func__ );
    // Update current contentItem
    SoundTouchInterface::ContentItem contentItemProto;
    ProtoToMarkup::FromJson( contentItem, &contentItemProto );
    m_AudioSettingsMgr->UpdateContentItem( contentItemProto );
    // Get latest mainStreamAudioSettings
    std::string mainStreamAudioSettings = ProtoToMarkup::ToJson( m_MainStreamAudioSettings );
    std::string inputRoute = std::to_string( m_InputRoute );
    cb.Send( mainStreamAudioSettings, inputRoute );
}

void CustomProductAudioService::SetStreamConfigCallback( std::string serializedAudioSettings, std::string serializedInputRoute, const Callback<bool> cb )
{
    BOSE_DEBUG( s_logger, __func__ );
    m_ProductLpmHardwareInterface->SendStreamConfig( serializedAudioSettings, serializedInputRoute, cb );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   CustomProductAudioService::SendMainStreamAudioSettingsEvent
///
/// @brief
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductAudioService::SendMainStreamAudioSettingsEvent()
{
    BOSE_DEBUG( s_logger, __func__ );
    std::string mainStreamAudioSettings = ProtoToMarkup::ToJson( m_MainStreamAudioSettings );
    m_APPointer -> SetMainStreamAudioSettings( mainStreamAudioSettings );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief Helper functions to convert audio setting values from string format to enumuration required from DSP
///
////////////////////////////////////////////////////////////////////////////////////////////////////
LpmServiceMessages::AudioSettingsAudioMode_t CustomProductAudioService::ModeNameToEnum( const std::string& modeName )
{
    BOSE_DEBUG( s_logger, __func__ );
    if( modeName == "night" )
    {
        return AUDIOSETTINGS_AUDIO_MODE_NIGHT;
    }
    else if( modeName == "direct" )
    {
        return AUDIOSETTINGS_AUDIO_MODE_DIRECT;
    }
    else if( modeName == "dialog" )
    {
        return AUDIOSETTINGS_AUDIO_MODE_DIALOG;
    }
    else if( modeName == "film" )
    {
        return AUDIOSETTINGS_AUDIO_MODE_NORMAL;
    }
    else
    {
        return AUDIOSETTINGS_AUDIO_MODE_UNSPECIFIED;
    }
}

LpmServiceMessages::AudioSettingsContent_t CustomProductAudioService::ContentTypeNameToEnum( const std::string& contentTypeName )
{
    BOSE_DEBUG( s_logger, __func__ );
    if( contentTypeName == "audio" )
    {
        return AUDIOSETTINGS_CONTENT_AUDIO;
    }
    else if( contentTypeName == "video" )
    {
        return AUDIOSETTINGS_CONTENT_VIDEO;
    }
    else
    {
        return AUDIOSETTINGS_CONTENT_UNSPECIFIED;
    }
}

LpmServiceMessages::AudioSettingsDualMonoMode_t CustomProductAudioService::DualMonoSelectNameToEnum( const std::string& dualMonoSelectName )
{
    BOSE_DEBUG( s_logger, __func__ );
    if( dualMonoSelectName == "left" )
    {
        return AUDIOSETTINGS_DUAL_MONO_LEFT;
    }
    else if( dualMonoSelectName == "video" )
    {
        return AUDIOSETTINGS_DUAL_MONO_RIGHT;
    }
    else
    {
        return AUDIOSETTINGS_DUAL_MONO_BOTH;
    }
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
    auto setBassAction = [this]( ProductPb::AudioBassLevel val )
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
    auto setTrebleAction = [ this ]( ProductPb::AudioTrebleLevel val )
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
    auto setCenterAction = [ this ]( ProductPb::AudioCenterLevel val )
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
    auto setSurroundAction = [ this ]( ProductPb::AudioSurroundLevel val )
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
    auto setGainOffsetAction = [ this ]( ProductPb::AudioGainOffset val )
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
    auto setAvSyncAction = [ this ]( ProductPb::AudioAvSync val )
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
    auto setModeAction = [ this ]( ProductPb::AudioMode val )
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
    auto setContentTypeAction = [ this ]( ProductPb::AudioContentType val )
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
    auto setDualMonoSelectAction = [ this ]( ProductPb::AudioDualMonoSelect val )
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
