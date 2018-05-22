///////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file      CustomProductAudioService.cpp
///
/// @brief     This file contains source code for Professor specific behavior for communicating
///            with APProduct Server and APProduct related FrontDoor interaction
///
/// @attention Copyright (C) 2018 Bose Corporation All Rights Reserved
///
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "DPrint.h"
#include "AsyncCallback.h"
#include "APProductFactory.h"
#include "LpmClientFactory.h"
#include "Utilities.h"
#include "ProfessorProductController.h"
#include "CustomProductAudioService.h"
#include "ProtoToMarkup.h"
#include "SoundTouchInterface/ContentItem.pb.h"
#include "AutoLpmServiceMessages.pb.h"
#include "ProductEndpointDefines.h"
#include "ProductDataCollectionDefines.h"

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
    m_ThermalTask( std::unique_ptr<ThermalMonitorTask>( new ThermalMonitorTask( lpmClient, std::bind( &CustomProductAudioService::ThermalDataReceivedCb, this, std::placeholders::_1 ) ) ) ),
    m_DataCollectionClient( ProductController.GetDataCollectionClient() )
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
    // Initialize m_MainStreamAudioSettings with current audio settings value from AudioSettingsManager
    // thermalData will be updated by thermal task periodically in a separate route
    FetchLatestAudioSettings();

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Register for AudioPath requests and events
    /////////////////////////////////////////////////////////////////////////////////////////////////
    m_APPointer = APProductFactory::Create( "ProductAudioService-APProduct", m_ProductTask );
    RegisterCommonAudioPathEvents();

    {
        Callback< std::string, Callback< std::string, std::string > >
        callback( std::bind( &CustomProductAudioService::GetMainStreamAudioSettingsCallback,
                             this,
                             std::placeholders::_1,
                             std::placeholders::_2 ) );

        m_APPointer->RegisterForMainStreamAudioSettingsRequest( callback );
    }
    {
        Callback< std::vector< APProductCommon::ChannelParameters >,
                  std::string,
                  std::string,
                  Callback< bool > >
                  callback( std::bind( &CustomProductAudioService::SetStreamConfigCallback,
                                       this,
                                       std::placeholders::_1,
                                       std::placeholders::_2,
                                       std::placeholders::_3,
                                       std::placeholders::_4 ) );

        m_APPointer->RegisterForSetStreamConfig( callback );
    }
    {
        Callback< bool > callback( std::bind( &CustomProductAudioService::InternalMuteCallback,
                                              this,
                                              std::placeholders::_1 ) );

        m_APPointer->RegisterForInternalMute( callback );
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
    m_MainStreamAudioSettings.set_surrounddelay( m_AudioSettingsMgr->GetSurroundDelay( ).value() );
    m_MainStreamAudioSettings.set_gainoffset( m_AudioSettingsMgr->GetGainOffset( ).value() );
    m_MainStreamAudioSettings.set_targetlatencyms( m_AudioSettingsMgr->GetAvSync( ).value() );
    m_MainStreamAudioSettings.set_subwooferlevel( m_AudioSettingsMgr->GetSubwooferGain( ).value() );
    m_MainStreamAudioSettings.set_audiomode( ModeNameToEnum( m_AudioSettingsMgr->GetMode( ).value() ) );
    m_MainStreamAudioSettings.set_contenttype( ContentTypeNameToEnum( m_AudioSettingsMgr->GetContentType( ).value() ) );
    m_MainStreamAudioSettings.set_dualmonoselect( DualMonoSelectNameToEnum( m_AudioSettingsMgr->GetDualMonoSelect( ).value() ) );
    m_MainStreamAudioSettings.set_deltaeqselect( EqSelectNameToEnum( m_AudioSettingsMgr->GetEqSelect( ).mode() ) );
    m_MainStreamAudioSettings.set_subwooferpolarity( SubwooferPolarityNameToEnum( m_AudioSettingsMgr->GetSubwooferPolarity( ).value() ) );
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
void CustomProductAudioService::SetStreamConfigCallback( std::vector<APProductCommon::ChannelParameters> channelParams,
                                                         std::string serializedAudioSettings,
                                                         std::string serializedInputRoute,
                                                         const Callback<bool> cb )
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

    if( serializedInputRoute.empty() )
    {
        streamConfig.set_inputroute( m_InputRoute );
    }
    else
    {
        streamConfig.set_inputroute( std::stoi( serializedInputRoute ) );
    }

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
/// @name   CustomProductAudioService::InternalMuteCallback
///
/// @param  bool mute
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductAudioService::InternalMuteCallback( bool mute )
{
    BOSE_DEBUG( s_logger, "%s, mute = %s", __func__, mute ? "true" : "false" );
    m_ProductLpmHardwareInterface->SetInternalMute( mute );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   CustomProductAudioService::SendMainStreamAudioSettingsEvent
///
/// @brief  ProductController notifies APProduct about latest mainStreamAudioSettings by sending
///         this event.
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
    BOSE_VERBOSE( s_logger, __func__ );
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
/// @name   CustomProductAudioService::SetAiqInstalled
///
/// @param  bool installed
///
/// @brief  Info from DSP about whether AdaptIQ has installed on DSP
///
/////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductAudioService::SetAiqInstalled( bool installed )
{
    m_AudioSettingsMgr->UpdateEqSelectSupportedMode( "EQ_AIQ_A", installed );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief Helper functions to convert audio setting values from string format to enumuration
///        required from DSP
///
////////////////////////////////////////////////////////////////////////////////////////////////////
LpmServiceMessages::AudioSettingsAudioMode_t CustomProductAudioService::ModeNameToEnum( const std::string& modeName )
{
    if( modeName == "normal" )
    {
        return AUDIOSETTINGS_AUDIO_MODE_NORMAL;
    }
    else if( modeName == "dialog" )
    {
        return AUDIOSETTINGS_AUDIO_MODE_DIALOG;
    }
    return AUDIOSETTINGS_AUDIO_MODE_NORMAL;
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
    return AUDIOSETTINGS_CONTENT_UNSPECIFIED;
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
    return AUDIOSETTINGS_DUAL_MONO_BOTH;
}

LpmServiceMessages::AudioSettingsSubwooferPolarity_t CustomProductAudioService::SubwooferPolarityNameToEnum( const std::string& subwooferPolarityName )
{
    if( subwooferPolarityName == "AUDIOSETTINGS_SUBWOOFERPOLARITY_INPHASE" )
    {
        return AUDIOSETTINGS_SUBWOOFERPOLARITY_INPHASE;
    }
    else if( subwooferPolarityName == "AUDIOSETTINGS_SUBWOOFERPOLARITY_OUTOFPHASE" )
    {
        return AUDIOSETTINGS_SUBWOOFERPOLARITY_OUTOFPHASE;
    }
    return AUDIOSETTINGS_SUBWOOFERPOLARITY_INPHASE;
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

///////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   CustomProductAudioService::RegisterFrontDoorEvents
///
/// @brief  On Professor, it register for put/post/get FrontDoor request for bass, treble, center,
///         surround, surroundDelay, gainOffset, avSync, subwooferGain, mode, contentType
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductAudioService::RegisterFrontDoorEvents()
{
    BOSE_DEBUG( s_logger, __func__ );

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// Endpoint /audio/bass - register handlers for POST/PUT/GET requests
    ///////////////////////////////////////////////////////////////////////////////////////////////
    auto getBassAction = [this]( )
    {
        return m_AudioSettingsMgr->GetBass( );
    };
    auto setBassAction = [this]( ProductPb::AudioBassLevel val )
    {
        ResultCode_t error = m_AudioSettingsMgr->SetBass( val );

        if( error == ResultCode_t::NO_ERROR )
        {
            m_MainStreamAudioSettings.set_basslevel( m_AudioSettingsMgr->GetBass( ).value() );
            SendMainStreamAudioSettingsEvent();
            m_DataCollectionClient->SendData(
                std::make_shared< ProductPb::AudioBassLevel >( m_AudioSettingsMgr->GetBass( ) ),
                DATA_COLLECTION_BASS );
        }
        return error;
    };

    m_AudioBassSetting = std::unique_ptr< AudioSetting< ProductPb::AudioBassLevel > >
                         ( new AudioSetting< ProductPb::AudioBassLevel >
                           ( FRONTDOOR_AUDIO_BASS_API,
                             getBassAction,
                             setBassAction,
                             m_FrontDoorClientIF,
                             m_ProductTask,
                             FRONTDOOR_PRODUCT_CONTROLLER_VERSION,
                             FRONTDOOR_PRODUCT_CONTROLLER_GROUP_NAME ) );

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// Endpoint /audio/treble - register handlers for POST/PUT/GET requests
    ///////////////////////////////////////////////////////////////////////////////////////////////
    auto getTrebleAction = [ this ]( )
    {
        return m_AudioSettingsMgr->GetTreble( );
    };
    auto setTrebleAction = [ this ]( ProductPb::AudioTrebleLevel val )
    {
        ResultCode_t error = m_AudioSettingsMgr->SetTreble( val );

        if( error == ResultCode_t::NO_ERROR )
        {
            m_MainStreamAudioSettings.set_treblelevel( m_AudioSettingsMgr->GetTreble( ).value() );
            SendMainStreamAudioSettingsEvent();
            m_DataCollectionClient->SendData(
                std::make_shared< ProductPb::AudioTrebleLevel >( m_AudioSettingsMgr->GetTreble( ) ),
                DATA_COLLECTION_TREBLE );
        }
        return error;
    };
    m_AudioTrebleSetting = std::unique_ptr< AudioSetting< ProductPb::AudioTrebleLevel > >
                           ( new AudioSetting< ProductPb::AudioTrebleLevel >
                             ( FRONTDOOR_AUDIO_TREBLE_API,
                               getTrebleAction,
                               setTrebleAction,
                               m_FrontDoorClientIF,
                               m_ProductTask,
                               FRONTDOOR_PRODUCT_CONTROLLER_VERSION,
                               FRONTDOOR_PRODUCT_CONTROLLER_GROUP_NAME ) );

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// Endpoint /audio/center - register handlers for POST/PUT/GET requests
    ///////////////////////////////////////////////////////////////////////////////////////////////
    auto getCenterAction = [ this ]( )
    {
        return m_AudioSettingsMgr->GetCenter( );
    };
    auto setCenterAction = [ this ]( ProductPb::AudioCenterLevel val )
    {
        ResultCode_t error = m_AudioSettingsMgr->SetCenter( val );

        if( error == ResultCode_t::NO_ERROR )
        {
            m_MainStreamAudioSettings.set_centerlevel( m_AudioSettingsMgr->GetCenter( ).value() );
            SendMainStreamAudioSettingsEvent();
            m_DataCollectionClient->SendData(
                std::make_shared< ProductPb::AudioCenterLevel >( m_AudioSettingsMgr->GetCenter( ) ),
                DATA_COLLECTION_CENTER );
        }
        return error;
    };
    m_AudioCenterSetting = std::unique_ptr< AudioSetting< ProductPb::AudioCenterLevel > >
                           ( new AudioSetting<ProductPb::AudioCenterLevel>
                             ( FRONTDOOR_AUDIO_CENTER_API,
                               getCenterAction,
                               setCenterAction,
                               m_FrontDoorClientIF,
                               m_ProductTask,
                               FRONTDOOR_PRODUCT_CONTROLLER_VERSION,
                               FRONTDOOR_PRODUCT_CONTROLLER_GROUP_NAME ) );

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// Endpoint /audio/surround - register handlers for POST/PUT/GET requests
    ///////////////////////////////////////////////////////////////////////////////////////////////
    auto getSurroundAction = [ this ]( )
    {
        return m_AudioSettingsMgr->GetSurround( );
    };
    auto setSurroundAction = [ this ]( ProductPb::AudioSurroundLevel val )
    {
        ResultCode_t error = m_AudioSettingsMgr->SetSurround( val );

        if( error == ResultCode_t::NO_ERROR )
        {
            m_MainStreamAudioSettings.set_surroundlevel( m_AudioSettingsMgr->GetSurround( ).value() );
            SendMainStreamAudioSettingsEvent();
            m_DataCollectionClient->SendData(
                std::make_shared< ProductPb::AudioSurroundLevel >( m_AudioSettingsMgr->GetSurround( ) ),
                DATA_COLLECTION_SURROUND );
        }
        return error;
    };
    m_AudioSurroundSetting = std::unique_ptr< AudioSetting< ProductPb::AudioSurroundLevel > >
                             ( new AudioSetting< ProductPb::AudioSurroundLevel >
                               ( FRONTDOOR_AUDIO_SURROUND_API,
                                 getSurroundAction,
                                 setSurroundAction,
                                 m_FrontDoorClientIF,
                                 m_ProductTask,
                                 FRONTDOOR_PRODUCT_CONTROLLER_VERSION,
                                 FRONTDOOR_PRODUCT_CONTROLLER_GROUP_NAME ) );

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// Endpoint /audio/surroundDelay - register handlers for POST/PUT/GET requests
    ///////////////////////////////////////////////////////////////////////////////////////////////
    auto getSurroundDelayAction = [ this ]( )
    {
        return m_AudioSettingsMgr->GetSurroundDelay( );
    };
    auto setSurroundDelayAction = [ this ]( ProductPb::AudioSurroundDelay val )
    {
        ResultCode_t error = m_AudioSettingsMgr->SetSurroundDelay( val );
        if( error == ResultCode_t::NO_ERROR )
        {
            m_MainStreamAudioSettings.set_surrounddelay( m_AudioSettingsMgr->GetSurroundDelay( ).value() );
            SendMainStreamAudioSettingsEvent();
            m_DataCollectionClient->SendData(
                std::make_shared< ProductPb::AudioSurroundDelay >( m_AudioSettingsMgr->GetSurroundDelay( ) ),
                DATA_COLLECTION_SURROUND_DELAY );
        }
        return error;
    };
    m_AudioSurroundDelaySetting = std::unique_ptr< AudioSetting< ProductPb::AudioSurroundDelay > >
                                  ( new AudioSetting< ProductPb::AudioSurroundDelay >
                                    ( FRONTDOOR_AUDIO_SURROUNDDELAY_API,
                                      getSurroundDelayAction,
                                      setSurroundDelayAction,
                                      m_FrontDoorClientIF,
                                      m_ProductTask,
                                      FRONTDOOR_PRODUCT_CONTROLLER_VERSION,
                                      FRONTDOOR_PRODUCT_CONTROLLER_GROUP_NAME ) );

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// Endpoint /audio/gainOffset - register handlers for POST/PUT/GET requests
    ///////////////////////////////////////////////////////////////////////////////////////////////
    auto getGainOffsetAction = [ this ]( )
    {
        return m_AudioSettingsMgr->GetGainOffset( );
    };
    auto setGainOffsetAction = [ this ]( ProductPb::AudioGainOffset val )
    {
        ResultCode_t error = m_AudioSettingsMgr->SetGainOffset( val );

        if( error == ResultCode_t::NO_ERROR )
        {
            m_MainStreamAudioSettings.set_gainoffset( m_AudioSettingsMgr->GetGainOffset( ).value() );
            SendMainStreamAudioSettingsEvent();
            m_DataCollectionClient->SendData(
                std::make_shared< ProductPb::AudioGainOffset >( m_AudioSettingsMgr->GetGainOffset( ) ),
                DATA_COLLECTION_GAIN_OFFSET );
        }
        return error;
    };
    m_AudioGainOffsetSetting =  std::unique_ptr< AudioSetting< ProductPb::AudioGainOffset > >
                                ( new AudioSetting< ProductPb::AudioGainOffset >
                                  ( FRONTDOOR_AUDIO_GAINOFFSET_API,
                                    getGainOffsetAction,
                                    setGainOffsetAction,
                                    m_FrontDoorClientIF,
                                    m_ProductTask,
                                    FRONTDOOR_PRODUCT_CONTROLLER_VERSION,
                                    FRONTDOOR_PRODUCT_CONTROLLER_GROUP_NAME ) );

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// Endpoint /audio/avSync - register handlers for POST/PUT/GET requests
    ///////////////////////////////////////////////////////////////////////////////////////////////
    auto getAvSyncAction = [ this ]( )
    {
        return m_AudioSettingsMgr->GetAvSync( );
    };
    auto setAvSyncAction = [ this ]( ProductPb::AudioAvSync val )
    {
        ResultCode_t error = m_AudioSettingsMgr->SetAvSync( val );

        if( error == ResultCode_t::NO_ERROR )
        {
            m_MainStreamAudioSettings.set_targetlatencyms( m_AudioSettingsMgr->GetAvSync( ).value() );
            SendMainStreamAudioSettingsEvent();
            m_DataCollectionClient->SendData(
                std::make_shared< ProductPb::AudioAvSync >( m_AudioSettingsMgr->GetAvSync( ) ),
                DATA_COLLECTION_AVSYNC );
        }
        return error;
    };
    m_AudioAvSyncSetting = std::unique_ptr< AudioSetting< ProductPb::AudioAvSync > >
                           ( new AudioSetting< ProductPb::AudioAvSync >
                             ( FRONTDOOR_AUDIO_AVSYNC_API,
                               getAvSyncAction,
                               setAvSyncAction,
                               m_FrontDoorClientIF,
                               m_ProductTask,
                               FRONTDOOR_PRODUCT_CONTROLLER_VERSION,
                               FRONTDOOR_PRODUCT_CONTROLLER_GROUP_NAME ) );

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// Endpoint /audio/subWooferGain - register handlers for POST/PUT/GET requests
    ///////////////////////////////////////////////////////////////////////////////////////////////
    auto getSubwooferGainAction = [ this ]( )
    {
        return m_AudioSettingsMgr->GetSubwooferGain( );
    };
    auto setSubwooferGainAction = [ this ]( ProductPb::AudioSubwooferGain val )
    {
        ResultCode_t error = m_AudioSettingsMgr->SetSubwooferGain( val );

        if( error == ResultCode_t::NO_ERROR )
        {
            m_MainStreamAudioSettings.set_subwooferlevel(
                m_AudioSettingsMgr->GetSubwooferGain( ).value() );

            SendMainStreamAudioSettingsEvent();
            m_DataCollectionClient->SendData(
                std::make_shared< ProductPb::AudioSubwooferGain >( m_AudioSettingsMgr->GetSubwooferGain( ) ),
                DATA_COLLECTION_SUBWOOFER_GAIN );
        }
        return error;
    };
    m_AudioSubwooferGainSetting = std::unique_ptr< AudioSetting<ProductPb::AudioSubwooferGain > >
                                  ( new AudioSetting< ProductPb::AudioSubwooferGain >
                                    ( FRONTDOOR_AUDIO_SUBWOOFERGAIN_API,
                                      getSubwooferGainAction,
                                      setSubwooferGainAction,
                                      m_FrontDoorClientIF,
                                      m_ProductTask,
                                      FRONTDOOR_PRODUCT_CONTROLLER_VERSION,
                                      FRONTDOOR_PRODUCT_CONTROLLER_GROUP_NAME ) );

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// Endpoint /audio/mode - register handlers for POST/PUT/GET requests
    ///////////////////////////////////////////////////////////////////////////////////////////////
    auto getModeAction = [ this ]( )
    {
        return m_AudioSettingsMgr->GetMode( );
    };
    auto setModeAction = [ this ]( ProductPb::AudioMode val )
    {
        ResultCode_t error = m_AudioSettingsMgr->SetMode( val );

        if( error == ResultCode_t::NO_ERROR )
        {
            m_MainStreamAudioSettings.set_audiomode(
                ModeNameToEnum( m_AudioSettingsMgr->GetMode( ).value() ) );
            SendMainStreamAudioSettingsEvent();
            m_DataCollectionClient->SendData(
                std::make_shared< ProductPb::AudioMode >( m_AudioSettingsMgr->GetMode( ) ),
                DATA_COLLECTION_MODE );
        }
        return error;
    };
    m_AudioModeSetting = std::unique_ptr< AudioSetting<ProductPb::AudioMode > >
                         ( new AudioSetting< ProductPb::AudioMode >
                           ( FRONTDOOR_AUDIO_MODE_API,
                             getModeAction,
                             setModeAction,
                             m_FrontDoorClientIF,
                             m_ProductTask,
                             FRONTDOOR_PRODUCT_CONTROLLER_VERSION,
                             FRONTDOOR_PRODUCT_CONTROLLER_GROUP_NAME ) );

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// Endpoint /audio/contentType - register handlers for POST/PUT/GET requests
    ///////////////////////////////////////////////////////////////////////////////////////////////
    auto getContentTypeAction = [ this ]( )
    {
        return m_AudioSettingsMgr->GetContentType( );
    };
    auto setContentTypeAction = [ this ]( ProductPb::AudioContentType val )
    {
        ResultCode_t error = m_AudioSettingsMgr->SetContentType( val );

        if( error == ResultCode_t::NO_ERROR )
        {
            m_MainStreamAudioSettings.set_contenttype( ContentTypeNameToEnum(
                                                           m_AudioSettingsMgr->GetContentType( ).value() ) );
            SendMainStreamAudioSettingsEvent();
            m_DataCollectionClient->SendData(
                std::make_shared< ProductPb::AudioContentType >( m_AudioSettingsMgr->GetContentType( ) ),
                DATA_COLLECTION_CONTENT_TYPE );
        }
        return error;
    };
    m_AudioContentTypeSetting = std::unique_ptr< AudioSetting<ProductPb::AudioContentType > >
                                ( new AudioSetting< ProductPb::AudioContentType >
                                  ( FRONTDOOR_AUDIO_CONTENTTYPE_API,
                                    getContentTypeAction,
                                    setContentTypeAction,
                                    m_FrontDoorClientIF,
                                    m_ProductTask,
                                    FRONTDOOR_PRODUCT_CONTROLLER_VERSION,
                                    FRONTDOOR_PRODUCT_CONTROLLER_GROUP_NAME ) );

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// Endpoint /audio/dualMonoSelect - register handlers for POST/PUT/GET requests
    ///////////////////////////////////////////////////////////////////////////////////////////////
    auto getDualMonoSelectAction = [ this ]( )
    {
        return m_AudioSettingsMgr->GetDualMonoSelect( );
    };
    auto setDualMonoSelectAction = [ this ]( ProductPb::AudioDualMonoSelect val )
    {
        ResultCode_t error = m_AudioSettingsMgr->SetDualMonoSelect( val );

        if( error == ResultCode_t::NO_ERROR )
        {
            m_MainStreamAudioSettings.set_dualmonoselect( DualMonoSelectNameToEnum(
                                                              m_AudioSettingsMgr->GetDualMonoSelect( ).value() ) );
            SendMainStreamAudioSettingsEvent();
            m_DataCollectionClient->SendData(
                std::make_shared< ProductPb::AudioDualMonoSelect >( m_AudioSettingsMgr->GetDualMonoSelect( ) ),
                DATA_COLLECTION_DUALMONO_SELECT );
        }
        return error;
    };
    m_DualMonoSelectSetting = std::unique_ptr< AudioSetting<ProductPb::AudioDualMonoSelect > >
                              ( new AudioSetting< ProductPb::AudioDualMonoSelect >
                                ( FRONTDOOR_AUDIO_DUALMONOSELECT_API,
                                  getDualMonoSelectAction,
                                  setDualMonoSelectAction,
                                  m_FrontDoorClientIF,
                                  m_ProductTask,
                                  FRONTDOOR_PRODUCT_CONTROLLER_VERSION,
                                  FRONTDOOR_PRODUCT_CONTROLLER_GROUP_NAME ) );

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// Endpoint /audio/eqSelect - register handlers for POST/PUT/GET requests
    ///////////////////////////////////////////////////////////////////////////////////////////////
    auto getEqSelectAction = [ this ]( )
    {
        return m_AudioSettingsMgr->GetEqSelect( );
    };
    auto setEqSelectAction = [ this ]( ProductPb::AudioEqSelect val )
    {
        ResultCode_t error = m_AudioSettingsMgr->SetEqSelect( val );

        if( error == ResultCode_t::NO_ERROR )
        {
            m_MainStreamAudioSettings.set_deltaeqselect(
                EqSelectNameToEnum( m_AudioSettingsMgr->GetEqSelect( ).mode() ) );
            SendMainStreamAudioSettingsEvent();
            m_DataCollectionClient->SendData(
                std::make_shared< ProductPb::AudioEqSelect >( m_AudioSettingsMgr->GetEqSelect( ) ),
                DATA_COLLECTION_EQSELECT );
        }
        return error;
    };
    m_EqSelectSetting = std::unique_ptr< AudioSetting< ProductPb::AudioEqSelect > >
                        ( new AudioSetting< ProductPb::AudioEqSelect >
                          ( FRONTDOOR_AUDIO_EQSELECT_API,
                            getEqSelectAction,
                            setEqSelectAction,
                            m_FrontDoorClientIF,
                            m_ProductTask,
                            FRONTDOOR_PRODUCT_CONTROLLER_VERSION,
                            FRONTDOOR_PRODUCT_CONTROLLER_GROUP_NAME ) );

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// Endpoint /audio/SubwooferPolarity - register handlers for POST/PUT/GET requests
    ///////////////////////////////////////////////////////////////////////////////////////////////
    auto getSubwooferPolarityAction = [ this ]( )
    {
        return m_AudioSettingsMgr->GetSubwooferPolarity( );
    };
    auto setSubwooferPolarityAction = [ this ]( ProductPb::AudioSubwooferPolarity val )
    {
        ResultCode_t error = m_AudioSettingsMgr->SetSubwooferPolarity( val );
        if( error == ResultCode_t::NO_ERROR )
        {
            m_MainStreamAudioSettings.set_subwooferpolarity(
                SubwooferPolarityNameToEnum( m_AudioSettingsMgr->GetSubwooferPolarity( ).value() ) );
            SendMainStreamAudioSettingsEvent();
            m_DataCollectionClient->SendData(
                std::make_shared< ProductPb::AudioSubwooferPolarity >( m_AudioSettingsMgr->GetSubwooferPolarity( ) ),
                DATA_COLLECTION_SUBWOOFER_POLARITY );
        }
        return error;
    };
    m_SubwooferPolaritySetting = std::unique_ptr< AudioSetting< ProductPb::AudioSubwooferPolarity > >
                                 ( new AudioSetting< ProductPb::AudioSubwooferPolarity >
                                   ( FRONTDOOR_AUDIO_SUBWOOFERPOLARITY_API,
                                     getSubwooferPolarityAction,
                                     setSubwooferPolarityAction,
                                     m_FrontDoorClientIF,
                                     m_ProductTask,
                                     FRONTDOOR_PRODUCT_CONTROLLER_VERSION,
                                     FRONTDOOR_PRODUCT_CONTROLLER_GROUP_NAME ) );
}

}// namespace ProductApp
