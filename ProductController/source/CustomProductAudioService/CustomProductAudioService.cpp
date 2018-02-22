///////////////////////////////////////////////////////////////////////////////////////////////////
/// @file   CustomProductAudioService.cpp
/// @brief   This file contains source code for Professor specific behavior for
///         communicating with APProduct Server and APProduct related FrontDoor interaction
/// Copyright 2017 Bose Corporation
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "DPrint.h"
#include "AsyncCallback.h"
#include "APProductFactory.h"
#include "LpmClientFactory.h"
#include "CustomProductAudioService.h"
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
constexpr char kEqSelectEndPoint        [] = "/audio/eqSelect";

namespace ProductApp
{
////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   CustomProductAudioService::CustomProductAudioService
///
/// @param  ProfessorProductController& ProductController
///
////////////////////////////////////////////////////////////////////////////////////////////////////
CustomProductAudioService::CustomProductAudioService( ProfessorProductController& ProductController,
                                                      const FrontDoorClientIF_t& frontDoorClient,
                                                      LpmClientIF::LpmClientPtr lpmClient ):
    ProductAudioService( ProductController.GetTask( ),
                         ProductController.GetMessageHandler(),
                         frontDoorClient ),
    m_ProductLpmHardwareInterface( ProductController.GetLpmHardwareInterface( ) ),
    m_AudioSettingsMgr( std::unique_ptr<CustomAudioSettingsManager>( new CustomAudioSettingsManager() ) ),
    m_ThermalTask( std::unique_ptr<ThermalMonitorTask>( new ThermalMonitorTask( lpmClient, std::bind( &CustomProductAudioService::ThermalDataReceivedCb, this, std::placeholders::_1 ) ) ) )
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

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Initialize member variables related to AudioPath
    /////////////////////////////////////////////////////////////////////////////////////////////////
    // Initialize m_InputRoute
    m_InputRoute = 1 << AUDIO_INPUT_BIT_POSITION_NETWORK;
    // Initialize m_MainStreamAudioSettings with current audio settings value from AudioSettingsManager
    // thermalData will be updated by thermal task periodically in a separate route
    FetchLatestAudioSettings();

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Register for AudioPath requests and events
    /////////////////////////////////////////////////////////////////////////////////////////////////
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
        Callback<std::vector<APProductCommon::ChannelParameters>, std::string, std::string, Callback< bool > > callback( std::bind( &CustomProductAudioService::SetStreamConfigCallback,
                this,
                std::placeholders::_1,
                std::placeholders::_2,
                std::placeholders::_3,
                std::placeholders::_4 ) );
        m_APPointer->RegisterForSetStreamConfig( callback );
    }
    ConnectToAudioPath();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   CustomProductAudioService::GetMainStreamAudioSettingsCallback
///
/// @param  std::string contentItem
///
/// @param  const Callback<std::string, std::string> cb
///
/// @brief  Callback function, when AudioPath wants to notify ProductController about the new contentItem
///         and get latest mainStreamAudioSettings and inputRoute back
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductAudioService::GetMainStreamAudioSettingsCallback( std::string contentItem,  const Callback<std::string, std::string> cb )
{
    BOSE_DEBUG( s_logger, __func__ );
    BOSE_DEBUG( s_logger, "GetMainStreamAudioSettingsCallback - contentItem = %s", contentItem.c_str() );

    // Parse contentItem string received from APProduct
    bool error = false;
    SoundTouchInterface::ContentItem contentItemProto;
    try
    {
        ProtoToMarkup::FromJson( contentItem, &contentItemProto );
    }
    catch( const ProtoToMarkup::MarkupError &e )
    {
        BOSE_ERROR( s_logger, "Converting contentItem string from APProduct to ContentItem proto failed markup error - %s", e.what() );
        error = true;
    }
    // If no parsing error occured, update m_MainStreamAudioSettings and m_InputRoute with new contentItem
    if( !error && contentItemProto.has_source() && contentItemProto.has_sourceaccount() )
    {
        // Update audio settings
        BOSE_DEBUG( s_logger, "GetMainStreamAudioSettingsCallback, source = %s, sourecAccount = %s", contentItemProto.source().c_str(), contentItemProto.sourceaccount().c_str() );

        m_AudioSettingsMgr->UpdateContentItem( contentItemProto );
        FetchLatestAudioSettings();
        // Update input route
        if( contentItemProto.source() == "PRODUCT" )
        {
            m_InputRoute = ( 1 << AUDIO_INPUT_BIT_POSITION_SPDIF_OPTICAL ) |
                           ( 1 << AUDIO_INPUT_BIT_POSITION_SPDIF_ARC ) |
                           ( 1 << AUDIO_INPUT_BIT_POSITION_EARC );
        }
        else
        {
            m_InputRoute = 1 << AUDIO_INPUT_BIT_POSITION_NETWORK;
        }
    }
    else
    {
        BOSE_ERROR( s_logger, "ContentItem string from APProduct doesn't contain \"source\" or \"sourceAccount\" field" );
    }
    // Reply APProduct with the current m_MainStreamAudioSettings and m_InputRoute
    std::string mainStreamAudioSettings = ProtoToMarkup::ToJson( m_MainStreamAudioSettings );
    std::string inputRoute = std::to_string( m_InputRoute );
    cb.Send( mainStreamAudioSettings, inputRoute );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   CustomProductAudioService::FetchLatestAudioSettings
///
/// @brief  update m_MainStreamAudioSettings with the latest audio settings from AudioSettingsManager
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductAudioService::FetchLatestAudioSettings( )
{
    m_MainStreamAudioSettings.set_basslevel( m_AudioSettingsMgr->GetBass( ).value() );
    m_MainStreamAudioSettings.set_treblelevel( m_AudioSettingsMgr->GetTreble( ).value() );
    m_MainStreamAudioSettings.set_centerlevel( m_AudioSettingsMgr->GetCenter( ).value() );
    m_MainStreamAudioSettings.set_surroundlevel( m_AudioSettingsMgr->GetSurround( ).value() );
    m_MainStreamAudioSettings.set_gainoffset( m_AudioSettingsMgr->GetGainOffset( ).value() );
    m_MainStreamAudioSettings.set_targetlatencyms( m_AudioSettingsMgr->GetAvSync( ).value() );
    m_MainStreamAudioSettings.set_audiomode( ModeNameToEnum( m_AudioSettingsMgr->GetMode( ).value() ) );
    m_MainStreamAudioSettings.set_contenttype( ContentTypeNameToEnum( m_AudioSettingsMgr->GetContentType( ).value() ) );
    m_MainStreamAudioSettings.set_dualmonoselect( DualMonoSelectNameToEnum( m_AudioSettingsMgr->GetDualMonoSelect( ).value() ) );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   CustomProductAudioService::SetStreamConfigCallback
///
/// @param  std::vector<APProductCommon::ChannelParameters> channelParams
///
/// @param  std::string serializedAudioSettings
///
/// @param  std::string serializedInputRoute
///
/// @param  const Callback<bool> cb
///
/// @brief  Callback function, when AudioPath wants to send streamConfig structure to change DSP settings
///         serializedAudioSettings contains audio settings, thermal data, and stream mix
///         serializedInputRoute contains input route info
///
/////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductAudioService::SetStreamConfigCallback( std::vector<APProductCommon::ChannelParameters> channelParams, std::string serializedAudioSettings, std::string serializedInputRoute, const Callback<bool> cb )
{
    BOSE_DEBUG( s_logger, __func__ );
    LpmServiceMessages::IpcDspStreamConfigReqPayload_t streamConfig;
    LpmServiceMessages::AudioSettings_t audioSettingsProto;
    try
    {
        ProtoToMarkup::FromJson( serializedAudioSettings, &audioSettingsProto );
    }
    catch( const ProtoToMarkup::MarkupError &e )
    {
        BOSE_ERROR( s_logger, "Converting serializedAudioSettings to proto failed markup error - %s", e.what() );
        cb.Send( false );
        return;
    }

    streamConfig.mutable_audiosettings()->CopyFrom( audioSettingsProto );
    streamConfig.set_inputroute( std::stoi( serializedInputRoute ) );
    for( auto& itr : channelParams )
    {
        LpmServiceMessages::ChannelMix_t* channelMix;
        channelMix = streamConfig.add_channelmix();
        channelMix->set_volume( itr.m_volumLevel );
        channelMix->set_usermute( itr.m_userMuted );
        channelMix->set_location( static_cast<LpmServiceMessages::PresentationLocation_t>( itr.m_presentationLocation ) );
        channelMix->set_intent( static_cast<LpmServiceMessages::StreamIntent_t>( itr.m_streamIntent ) );
    }
    m_ProductLpmHardwareInterface->SetStreamConfig( streamConfig, cb );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   CustomProductAudioService::SendMainStreamAudioSettingsEvent
///
/// @brief  ProductController notifies APProduct about latest mainStreamAudioSettings by sending this event
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
/// @name   CustomProductAudioService::ThermalDataReceivedCb
///
/// @param  const IpcSystemTemperatureData_t& data
///
/// @brief  Callback function, when thermal task receives thermal data from LPM
///
/////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductAudioService::ThermalDataReceivedCb( const IpcSystemTemperatureData_t& data )
{
    BOSE_DEBUG( s_logger, __func__ );
    m_MainStreamAudioSettings.mutable_temperaturedata()->CopyFrom( data );
    SendMainStreamAudioSettingsEvent();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   CustomProductAudioService::SetThermalMonitorEnabled
///
/// @param  bool enabled
///
/////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductAudioService::SetThermalMonitorEnabled( bool enabled )
{
    BOSE_DEBUG( s_logger, __func__ );
    if( enabled )
    {
        m_ThermalTask->Start();
    }
    else
    {
        m_ThermalTask->Stop();
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief Helper functions to convert audio setting values from string format to enumuration required from DSP
///
////////////////////////////////////////////////////////////////////////////////////////////////////
LpmServiceMessages::AudioSettingsAudioMode_t CustomProductAudioService::ModeNameToEnum( const std::string& modeName )
{
    if( modeName == "direct" )
    {
        return AUDIOSETTINGS_AUDIO_MODE_DIRECT;
    }
    else if( modeName == "normal" )
    {
        return AUDIOSETTINGS_AUDIO_MODE_NORMAL;
    }
    else if( modeName == "dialog" )
    {
        return AUDIOSETTINGS_AUDIO_MODE_DIALOG;
    }
    else if( modeName == "night" )
    {
        return AUDIOSETTINGS_AUDIO_MODE_NIGHT;
    }
    return AUDIOSETTINGS_AUDIO_MODE_DIRECT;
}

LpmServiceMessages::AudioSettingsContent_t CustomProductAudioService::ContentTypeNameToEnum( const std::string& contentTypeName )
{
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
/// @brief Helper functions to convert eq select values from string format to enumuration required by DSP
///
////////////////////////////////////////////////////////////////////////////////////////////////////
LpmServiceMessages::AudioSettingsDeltaEqSelect_t CustomProductAudioService::EqSelectNameToEnum( const std::string& modeName )
{
    static const std::map<std::string, LpmServiceMessages::AudioSettingsDeltaEqSelect_t> map =
    {
        {"EQ_OFF",      AUDIOSETTINGS_DELTAEQ_NONE},
        {"EQ_AIQ_A",    AUDIOSETTINGS_DELTAEQ_AIQ_A},
        {"EQ_AIQ_B",    AUDIOSETTINGS_DELTAEQ_AIQ_B},
        {"EQ_RETAIL_A", AUDIOSETTINGS_DELTAEQ_RETAIL_A},
        {"EQ_RETAIL_B", AUDIOSETTINGS_DELTAEQ_RETAIL_B},
        {"EQ_RETAIL_C", AUDIOSETTINGS_DELTAEQ_RETAIL_C},
    };

    auto ret = map.find( modeName );

    if( ret == map.end() )
    {
        return AUDIOSETTINGS_DELTAEQ_NONE;
    }

    return ret->second;
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

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// Endpoint /audio/bass - register ProductController as handler for POST/PUT/GET requests
    ///////////////////////////////////////////////////////////////////////////////////////////////
    auto getBassAction = [this]( )
    {
        return m_AudioSettingsMgr->GetBass( );
    };
    auto setBassAction = [this]( ProductPb::AudioBassLevel val )
    {
        bool ret = m_AudioSettingsMgr->SetBass( val );
        if( ret )
        {
            m_MainStreamAudioSettings.set_basslevel( m_AudioSettingsMgr->GetBass( ).value() );
            SendMainStreamAudioSettingsEvent();
        }
        return ret;
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
        bool ret = m_AudioSettingsMgr->SetTreble( val );
        if( ret )
        {
            m_MainStreamAudioSettings.set_treblelevel( m_AudioSettingsMgr->GetTreble( ).value() );
            SendMainStreamAudioSettingsEvent();
        }
        return ret;
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
        bool ret = m_AudioSettingsMgr->SetCenter( val );
        if( ret )
        {
            m_MainStreamAudioSettings.set_centerlevel( m_AudioSettingsMgr->GetCenter( ).value() );
            SendMainStreamAudioSettingsEvent();
        }
        return ret;
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
        bool ret = m_AudioSettingsMgr->SetSurround( val );
        if( ret )
        {
            m_MainStreamAudioSettings.set_surroundlevel( m_AudioSettingsMgr->GetSurround( ).value() );
            SendMainStreamAudioSettingsEvent();
        }
        return ret;
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
        bool ret = m_AudioSettingsMgr->SetGainOffset( val );
        if( ret )
        {
            m_MainStreamAudioSettings.set_gainoffset( m_AudioSettingsMgr->GetGainOffset( ).value() );
            SendMainStreamAudioSettingsEvent();
        }
        return ret;
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
        bool ret = m_AudioSettingsMgr->SetAvSync( val );
        if( ret )
        {
            m_MainStreamAudioSettings.set_targetlatencyms( m_AudioSettingsMgr->GetAvSync( ).value() );
            SendMainStreamAudioSettingsEvent();
        }
        return ret;
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
        bool ret = m_AudioSettingsMgr->SetMode( val );
        if( ret )
        {
            m_MainStreamAudioSettings.set_audiomode( ModeNameToEnum( m_AudioSettingsMgr->GetMode( ).value() ) );
            SendMainStreamAudioSettingsEvent();
        }
        return ret;
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
        bool ret = m_AudioSettingsMgr->SetContentType( val );
        if( ret )
        {
            m_MainStreamAudioSettings.set_contenttype( ContentTypeNameToEnum( m_AudioSettingsMgr->GetContentType( ).value() ) );
            SendMainStreamAudioSettingsEvent();
        }
        return ret;
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
        bool ret = m_AudioSettingsMgr->SetDualMonoSelect( val );
        if( ret )
        {
            m_MainStreamAudioSettings.set_dualmonoselect( DualMonoSelectNameToEnum( m_AudioSettingsMgr->GetDualMonoSelect( ).value() ) );
            SendMainStreamAudioSettingsEvent();
        }
        return ret;
    };
    m_DualMonoSelectSetting = std::unique_ptr<AudioSetting<ProductPb::AudioDualMonoSelect>>( new AudioSetting<ProductPb::AudioDualMonoSelect>
                              ( kDualMonoSelectEndPoint,
                                getDualMonoSelectAction,
                                setDualMonoSelectAction,
                                m_FrontDoorClientIF,
                                m_ProductTask ) );

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// Endpoint /audio/eqSelect - register ProductController as handler for POST/PUT/GET requests
    //////////////////////////////////////////////////////////////////////////////////////////////
    auto getEqSelectAction = [ this ]( )
    {
        return m_AudioSettingsMgr->GetEqSelect( );
    };
    auto setEqSelectAction = [ this ]( ProductPb::AudioEqSelect val )
    {
        bool ret = m_AudioSettingsMgr->SetEqSelect( val );
        if( ret )
        {
            m_MainStreamAudioSettings.set_deltaeqselect( EqSelectNameToEnum( m_AudioSettingsMgr->GetEqSelect( ).mode() ) );
            SendMainStreamAudioSettingsEvent();
        }
        return ret;
    };
    m_EqSelectSetting = std::unique_ptr<AudioSetting<ProductPb::AudioEqSelect>>( new AudioSetting<ProductPb::AudioEqSelect>
                                                                                 ( kEqSelectEndPoint,
                                                                                   getEqSelectAction,
                                                                                   setEqSelectAction,
                                                                                   m_FrontDoorClientIF,
                                                                                   m_ProductTask ) );


}

}// namespace ProductApp
