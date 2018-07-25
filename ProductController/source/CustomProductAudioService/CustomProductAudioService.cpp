///////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file      CustomProductAudioService.cpp
///
/// @brief     This file contains source code for custom behavior for communicating
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
#include "CustomProductController.h"
#include "CustomProductAudioService.h"
#include "ProtoToMarkup.h"
#include "SoundTouchInterface/ContentItem.pb.h"
#include "AutoLpmServiceMessages.pb.h"
#include "ProductEndpointDefines.h"
#include "ProductDataCollectionDefines.h"
// TODO - JCH - fix this as part of PGC-2476
//#include "RivieraLPM_IpcProtocol.h"
#define IPC_LATENCY_VALUE_UNKNOWN 0xffff
// end TODO

using namespace std::placeholders;

namespace ProductApp
{
using namespace ProductPb;
////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   CustomProductAudioService::CustomProductAudioService
///
/// @param  CustomProductController& ProductController
///
////////////////////////////////////////////////////////////////////////////////////////////////////
CustomProductAudioService::CustomProductAudioService( CustomProductController& ProductController,
                                                      const FrontDoorClientIF_t& frontDoorClient,
                                                      LpmClientIF::LpmClientPtr lpmClient ):
    ProductAudioService( ProductController.GetTask( ),
                         ProductController.GetMessageHandler(),
                         frontDoorClient ),
    m_ProductLpmHardwareInterface( ProductController.GetLpmHardwareInterface( ) ),
    m_AudioSettingsMgr( std::unique_ptr<CustomAudioSettingsManager>( new CustomAudioSettingsManager() ) ),
    m_ThermalTask( std::unique_ptr<ThermalMonitorTask>(
                       new ThermalMonitorTask( lpmClient, ProductController.GetTask( ),
                                               AsyncCallback<const IpcSystemTemperatureData_t&>(
                                                   std::bind( &CustomProductAudioService::ThermalDataReceivedCb, this, _1 ),
                                                   ProductController.GetTask( ) ) ) ) ),
    m_DataCollectionClient( ProductController.GetDataCollectionClient() ),
    m_currentMinimumLatency( IPC_LATENCY_VALUE_UNKNOWN )
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
    /// Register to handle DSP booted indications
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    auto bootedFunc = [ this ]( LpmServiceMessages::IpcDeviceBoot_t image )
    {
        // Verify that stream configuration is actually valid (it's possible that we get a "booted" indication
        // before first audio path select, since DSP booting and audio path selection are independent). This
        // is okay, since in that case audio path selection will still send stream configuration to the DSP)
        if( m_DspStreamConfig.has_audiosettings() )
        {
            BOSE_INFO( s_logger, "DSP booted, send stream config (%s)", ProtoToMarkup::ToJson( m_DspStreamConfig ).c_str() );
            m_ProductLpmHardwareInterface->SetStreamConfig( m_DspStreamConfig, m_StreamConfigResponseCb );
            // stream config callback is normally empty (gets set when we get a SetStreamConfig request that
            // we can't immediately forward to DSP because it's booting)
            m_StreamConfigResponseCb = {};
        }
        m_DspIsRebooting = false;
    };

    auto lpmConnectCb = [ this, bootedFunc ]( bool connected )
    {
        if( not connected )
        {
            return;
        }

        bool success =  m_ProductLpmHardwareInterface->RegisterForLpmEvents< LpmServiceMessages::IpcDeviceBoot_t >
                        ( LpmServiceMessages::IPC_DSP_BOOTED_EVENT, Callback<LpmServiceMessages::IpcDeviceBoot_t>( bootedFunc ) );
        if( not success )
        {
            BOSE_ERROR( s_logger, "%s error registering for DSP boot status", __func__ );
            return;
        }
    };
    m_ProductLpmHardwareInterface->RegisterForLpmConnection( lpmConnectCb );

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
        if( contentItemProto.source() == SHELBY_SOURCE::PRODUCT )
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
    BOSE_DEBUG( s_logger, "streamConfig.audioSettings = %s", streamConfig.audiosettings().DebugString().c_str() );

    if( serializedInputRoute.empty() )
    {
        streamConfig.set_inputroute( m_InputRoute );
    }
    else
    {
        streamConfig.set_inputroute( std::stoi( serializedInputRoute ) );
    }
    BOSE_DEBUG( s_logger, "streamConfig.inputRoute = %d", streamConfig.inputroute() );

    uint32_t channel = 0;
    for( auto& itr : channelParams )
    {
        LpmServiceMessages::ChannelMix_t* channelMix;
        channelMix = streamConfig.add_channelmix();
        channelMix->set_volume( itr.m_volumLevel );
        channelMix->set_usermute( itr.m_userMuted );
        channelMix->set_location( static_cast<LpmServiceMessages::PresentationLocation_t>( itr.m_presentationLocation ) );
        channelMix->set_intent( static_cast<LpmServiceMessages::StreamIntent_t>( itr.m_streamIntent ) );
        BOSE_DEBUG( s_logger, "streamConfig.channelMix[%d] = %s", channel, channelMix->DebugString().c_str() );
        channel++;
    }

    m_DspStreamConfig = streamConfig;
    if( not m_DspIsRebooting )
    {
        // send it to the DSP
        m_ProductLpmHardwareInterface->SetStreamConfig( streamConfig, cb );
    }
    else
    {
        // wait until the DSP reboots to send it, and save the response callback
        m_StreamConfigResponseCb = cb;
    }
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
    std::string mainStreamAudioSettings = ProtoToMarkup::ToJson( m_MainStreamAudioSettings );
    BOSE_DEBUG( s_logger, "%s: %s", __func__, mainStreamAudioSettings.c_str() );
    m_APPointer -> SetMainStreamAudioSettings( mainStreamAudioSettings );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   CustomProductAudioService::SetMinimumOutputLatency
///
/// @param  int32_t latency
///
/// @brief  Send DSP minimumOutputLatency to AudioPath
///
/////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductAudioService::SetMinimumOutputLatency( int32_t latency )
{
    BOSE_VERBOSE( s_logger, __func__ );
    if( ( latency == IPC_LATENCY_VALUE_UNKNOWN ) or ( latency == m_currentMinimumLatency ) )
    {
        return;
    }
    m_currentMinimumLatency = latency;
    auto respCb = []( int32_t resp )
    {
        BOSE_VERBOSE( s_logger, "%s: received callback with latency(%d)", __func__, resp );
    };
    BOSE_INFO( s_logger, "%s: sending minimum output latency(%d) to AudioPath", __func__, m_currentMinimumLatency );
    m_APPointer -> SetOutputLatency( m_currentMinimumLatency, respCb );
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
/// @brief  Info from DSP about whether AdaptIQ EQ is available on DSP
///
/////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductAudioService::SetAiqInstalled( bool installed )
{
    m_AudioSettingsMgr->UpdateEqSelectSupportedMode( AudioEqSelect_supportedMode_Name( AudioEqSelect_supportedMode_EQ_AIQ_A ), installed );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief Helper functions to convert audio setting values from string format to enumuration
///        required from DSP
///
////////////////////////////////////////////////////////////////////////////////////////////////////
LpmServiceMessages::AudioSettingsAudioMode_t CustomProductAudioService::ModeNameToEnum( const std::string& modeName )
{
    if( modeName == AudioMode_supportedMode_Name( AudioMode_supportedMode_NORMAL ) )
    {
        return AUDIOSETTINGS_AUDIO_MODE_NORMAL;
    }
    else if( modeName == AudioMode_supportedMode_Name( AudioMode_supportedMode_DIALOG ) )
    {
        return AUDIOSETTINGS_AUDIO_MODE_DIALOG;
    }
    return AUDIOSETTINGS_AUDIO_MODE_NORMAL;
}

LpmServiceMessages::AudioSettingsContent_t CustomProductAudioService::ContentTypeNameToEnum( const std::string& contentTypeName )
{
    if( contentTypeName == AudioContentType_supportedValue_Name( AudioContentType_supportedValue_AUDIO ) )
    {
        return AUDIOSETTINGS_CONTENT_AUDIO;
    }
    else if( contentTypeName == AudioContentType_supportedValue_Name( AudioContentType_supportedValue_VIDEO ) )
    {
        return AUDIOSETTINGS_CONTENT_VIDEO;
    }
    return AUDIOSETTINGS_CONTENT_UNSPECIFIED;
}

LpmServiceMessages::AudioSettingsDualMonoMode_t CustomProductAudioService::DualMonoSelectNameToEnum( const std::string& dualMonoSelectName )
{
    if( dualMonoSelectName == AudioDualMonoSelect_supportedValue_Name( AudioDualMonoSelect_supportedValue_LEFT ) )
    {
        return AUDIOSETTINGS_DUAL_MONO_LEFT;
    }
    else if( dualMonoSelectName == AudioDualMonoSelect_supportedValue_Name( AudioDualMonoSelect_supportedValue_RIGHT ) )
    {
        return AUDIOSETTINGS_DUAL_MONO_RIGHT;
    }
    return AUDIOSETTINGS_DUAL_MONO_BOTH;
}

LpmServiceMessages::AudioSettingsSubwooferPolarity_t CustomProductAudioService::SubwooferPolarityNameToEnum( const std::string& subwooferPolarityName )
{
    if( subwooferPolarityName == AudioSubwooferPolarity_supportedValue_Name( AudioSubwooferPolarity_supportedValue_POLARITY_IN_PHASE ) )
    {
        return AUDIOSETTINGS_SUBWOOFERPOLARITY_INPHASE;
    }
    else if( subwooferPolarityName == AudioSubwooferPolarity_supportedValue_Name( AudioSubwooferPolarity_supportedValue_POLARITY_OUT_OF_PHASE ) )
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
    if( modeName == AudioEqSelect_supportedMode_Name( AudioEqSelect_supportedMode_EQ_AIQ_A ) )
    {
        return AUDIOSETTINGS_DELTAEQ_AIQ_A;
    }
    else if( modeName == AudioEqSelect_supportedMode_Name( AudioEqSelect_supportedMode_EQ_RETAIL_A ) )
    {
        return AUDIOSETTINGS_DELTAEQ_RETAIL_A;
    }
    return AUDIOSETTINGS_DELTAEQ_NONE;
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
    auto setBassAction = [this]( AudioBassLevel val )
    {
        ResultCode_t error = m_AudioSettingsMgr->SetBass( val );

        if( error == ResultCode_t::NO_ERROR )
        {
            m_MainStreamAudioSettings.set_basslevel( m_AudioSettingsMgr->GetBass( ).value() );
            SendMainStreamAudioSettingsEvent();
            m_DataCollectionClient->SendData(
                std::make_shared< AudioBassLevel >( m_AudioSettingsMgr->GetBass( ) ),
                DATA_COLLECTION_BASS );
        }
        return error;
    };

    m_AudioBassSetting = std::unique_ptr< AudioSetting< AudioBassLevel > >
                         ( new AudioSetting< AudioBassLevel >
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
    auto setTrebleAction = [ this ]( AudioTrebleLevel val )
    {
        ResultCode_t error = m_AudioSettingsMgr->SetTreble( val );

        if( error == ResultCode_t::NO_ERROR )
        {
            m_MainStreamAudioSettings.set_treblelevel( m_AudioSettingsMgr->GetTreble( ).value() );
            SendMainStreamAudioSettingsEvent();
            m_DataCollectionClient->SendData(
                std::make_shared< AudioTrebleLevel >( m_AudioSettingsMgr->GetTreble( ) ),
                DATA_COLLECTION_TREBLE );
        }
        return error;
    };
    m_AudioTrebleSetting = std::unique_ptr< AudioSetting< AudioTrebleLevel > >
                           ( new AudioSetting< AudioTrebleLevel >
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
    auto setCenterAction = [ this ]( AudioCenterLevel val )
    {
        ResultCode_t error = m_AudioSettingsMgr->SetCenter( val );

        if( error == ResultCode_t::NO_ERROR )
        {
            m_MainStreamAudioSettings.set_centerlevel( m_AudioSettingsMgr->GetCenter( ).value() );
            SendMainStreamAudioSettingsEvent();
            m_DataCollectionClient->SendData(
                std::make_shared< AudioCenterLevel >( m_AudioSettingsMgr->GetCenter( ) ),
                DATA_COLLECTION_CENTER );
        }
        return error;
    };
    m_AudioCenterSetting = std::unique_ptr< AudioSetting< AudioCenterLevel > >
                           ( new AudioSetting<AudioCenterLevel>
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
    auto setSurroundAction = [ this ]( AudioSurroundLevel val )
    {
        ResultCode_t error = m_AudioSettingsMgr->SetSurround( val );

        if( error == ResultCode_t::NO_ERROR )
        {
            m_MainStreamAudioSettings.set_surroundlevel( m_AudioSettingsMgr->GetSurround( ).value() );
            SendMainStreamAudioSettingsEvent();
            m_DataCollectionClient->SendData(
                std::make_shared< AudioSurroundLevel >( m_AudioSettingsMgr->GetSurround( ) ),
                DATA_COLLECTION_SURROUND );
        }
        return error;
    };
    m_AudioSurroundSetting = std::unique_ptr< AudioSetting< AudioSurroundLevel > >
                             ( new AudioSetting< AudioSurroundLevel >
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
    auto setSurroundDelayAction = [ this ]( AudioSurroundDelay val )
    {
        ResultCode_t error = m_AudioSettingsMgr->SetSurroundDelay( val );
        if( error == ResultCode_t::NO_ERROR )
        {
            m_MainStreamAudioSettings.set_surrounddelay( m_AudioSettingsMgr->GetSurroundDelay( ).value() );
            SendMainStreamAudioSettingsEvent();
            m_DataCollectionClient->SendData(
                std::make_shared< AudioSurroundDelay >( m_AudioSettingsMgr->GetSurroundDelay( ) ),
                DATA_COLLECTION_SURROUND_DELAY );
        }
        return error;
    };
    m_AudioSurroundDelaySetting = std::unique_ptr< AudioSetting< AudioSurroundDelay > >
                                  ( new AudioSetting< AudioSurroundDelay >
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
    auto setGainOffsetAction = [ this ]( AudioGainOffset val )
    {
        ResultCode_t error = m_AudioSettingsMgr->SetGainOffset( val );

        if( error == ResultCode_t::NO_ERROR )
        {
            m_MainStreamAudioSettings.set_gainoffset( m_AudioSettingsMgr->GetGainOffset( ).value() );
            SendMainStreamAudioSettingsEvent();
            m_DataCollectionClient->SendData(
                std::make_shared< AudioGainOffset >( m_AudioSettingsMgr->GetGainOffset( ) ),
                DATA_COLLECTION_GAIN_OFFSET );
        }
        return error;
    };
    m_AudioGainOffsetSetting =  std::unique_ptr< AudioSetting< AudioGainOffset > >
                                ( new AudioSetting< AudioGainOffset >
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
    auto setAvSyncAction = [ this ]( AudioAvSync val )
    {
        ResultCode_t error = m_AudioSettingsMgr->SetAvSync( val );

        if( error == ResultCode_t::NO_ERROR )
        {
            m_MainStreamAudioSettings.set_targetlatencyms( m_AudioSettingsMgr->GetAvSync( ).value() );
            SendMainStreamAudioSettingsEvent();
            m_DataCollectionClient->SendData(
                std::make_shared< AudioAvSync >( m_AudioSettingsMgr->GetAvSync( ) ),
                DATA_COLLECTION_AVSYNC );
        }
        return error;
    };
    m_AudioAvSyncSetting = std::unique_ptr< AudioSetting< AudioAvSync > >
                           ( new AudioSetting< AudioAvSync >
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
    auto setSubwooferGainAction = [ this ]( AudioSubwooferGain val )
    {
        ResultCode_t error = m_AudioSettingsMgr->SetSubwooferGain( val );

        if( error == ResultCode_t::NO_ERROR )
        {
            m_MainStreamAudioSettings.set_subwooferlevel(
                m_AudioSettingsMgr->GetSubwooferGain( ).value() );

            SendMainStreamAudioSettingsEvent();
            m_DataCollectionClient->SendData(
                std::make_shared< AudioSubwooferGain >( m_AudioSettingsMgr->GetSubwooferGain( ) ),
                DATA_COLLECTION_SUBWOOFER_GAIN );
        }
        return error;
    };
    m_AudioSubwooferGainSetting = std::unique_ptr< AudioSetting<AudioSubwooferGain > >
                                  ( new AudioSetting< AudioSubwooferGain >
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
    auto setModeAction = [ this ]( AudioMode val )
    {
        ResultCode_t error = m_AudioSettingsMgr->SetMode( val );

        if( error == ResultCode_t::NO_ERROR )
        {
            m_MainStreamAudioSettings.set_audiomode(
                ModeNameToEnum( m_AudioSettingsMgr->GetMode( ).value() ) );
            SendMainStreamAudioSettingsEvent();
            m_DataCollectionClient->SendData(
                std::make_shared< AudioMode >( m_AudioSettingsMgr->GetMode( ) ),
                DATA_COLLECTION_MODE );
        }
        return error;
    };
    m_AudioModeSetting = std::unique_ptr< AudioSetting<AudioMode > >
                         ( new AudioSetting< AudioMode >
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
    auto setContentTypeAction = [ this ]( AudioContentType val )
    {
        ResultCode_t error = m_AudioSettingsMgr->SetContentType( val );

        if( error == ResultCode_t::NO_ERROR )
        {
            m_MainStreamAudioSettings.set_contenttype( ContentTypeNameToEnum(
                                                           m_AudioSettingsMgr->GetContentType( ).value() ) );
            SendMainStreamAudioSettingsEvent();
            m_DataCollectionClient->SendData(
                std::make_shared< AudioContentType >( m_AudioSettingsMgr->GetContentType( ) ),
                DATA_COLLECTION_CONTENT_TYPE );
        }
        return error;
    };
    m_AudioContentTypeSetting = std::unique_ptr< AudioSetting<AudioContentType > >
                                ( new AudioSetting< AudioContentType >
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
    auto setDualMonoSelectAction = [ this ]( AudioDualMonoSelect val )
    {
        ResultCode_t error = m_AudioSettingsMgr->SetDualMonoSelect( val );

        if( error == ResultCode_t::NO_ERROR )
        {
            m_MainStreamAudioSettings.set_dualmonoselect( DualMonoSelectNameToEnum(
                                                              m_AudioSettingsMgr->GetDualMonoSelect( ).value() ) );
            SendMainStreamAudioSettingsEvent();
            m_DataCollectionClient->SendData(
                std::make_shared< AudioDualMonoSelect >( m_AudioSettingsMgr->GetDualMonoSelect( ) ),
                DATA_COLLECTION_DUALMONO_SELECT );
        }
        return error;
    };
    m_DualMonoSelectSetting = std::unique_ptr< AudioSetting<AudioDualMonoSelect > >
                              ( new AudioSetting< AudioDualMonoSelect >
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
    auto setEqSelectAction = [ this ]( AudioEqSelect val )
    {
        ResultCode_t error = m_AudioSettingsMgr->SetEqSelect( val );

        if( error == ResultCode_t::NO_ERROR )
        {
            m_MainStreamAudioSettings.set_deltaeqselect(
                EqSelectNameToEnum( m_AudioSettingsMgr->GetEqSelect( ).mode() ) );
            SendMainStreamAudioSettingsEvent();
            m_DataCollectionClient->SendData(
                std::make_shared< AudioEqSelect >( m_AudioSettingsMgr->GetEqSelect( ) ),
                DATA_COLLECTION_EQSELECT );
        }
        return error;
    };
    m_EqSelectSetting = std::unique_ptr< AudioSetting< AudioEqSelect > >
                        ( new AudioSetting< AudioEqSelect >
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
    auto setSubwooferPolarityAction = [ this ]( AudioSubwooferPolarity val )
    {
        ResultCode_t error = m_AudioSettingsMgr->SetSubwooferPolarity( val );
        if( error == ResultCode_t::NO_ERROR )
        {
            m_MainStreamAudioSettings.set_subwooferpolarity(
                SubwooferPolarityNameToEnum( m_AudioSettingsMgr->GetSubwooferPolarity( ).value() ) );
            SendMainStreamAudioSettingsEvent();
            m_DataCollectionClient->SendData(
                std::make_shared< AudioSubwooferPolarity >( m_AudioSettingsMgr->GetSubwooferPolarity( ) ),
                DATA_COLLECTION_SUBWOOFER_POLARITY );
        }
        return error;
    };
    m_SubwooferPolaritySetting = std::unique_ptr< AudioSetting< AudioSubwooferPolarity > >
                                 ( new AudioSetting< AudioSubwooferPolarity >
                                   ( FRONTDOOR_AUDIO_SUBWOOFERPOLARITY_API,
                                     getSubwooferPolarityAction,
                                     setSubwooferPolarityAction,
                                     m_FrontDoorClientIF,
                                     m_ProductTask,
                                     FRONTDOOR_PRODUCT_CONTROLLER_VERSION,
                                     FRONTDOOR_PRODUCT_CONTROLLER_GROUP_NAME ) );
}

}// namespace ProductApp
