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
                       new ThermalMonitorTask( lpmClient, m_ProductTask,
                                               AsyncCallback<IpcSystemTemperatureData_t>(
                                                   std::bind( &CustomProductAudioService::ThermalDataReceivedCb, this, _1 ),
                                                   m_ProductTask ) ) ) ),
    m_DataCollectionClient( ProductController.GetDataCollectionClient() ),
    m_currentNetworkSourceLatency( LpmServiceMessages::LATENCY_VALUE_UNKNOWN ),
    m_currentTVSourceLatency( LpmServiceMessages::LATENCY_VALUE_UNKNOWN ),
    m_deferredEqSelectResponse( []( AudioEqSelect eq ) {}, m_ProductTask )
{
    BOSE_DEBUG( s_logger, __func__ );

    auto func = [this]( bool enabled )
    {
        if( enabled )
        {
            SendAudioSettingsToDataCollection();
        }
    };
    m_DataCollectionClient->RegisterForEnabledNotifications( Callback<bool>( func ) );
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
        m_currentEqSelectUpdating = true;

        ProductMessage bootedMsg;
        *bootedMsg.mutable_dspbooted( ) = image;
        IL::BreakThread( [ this, bootedMsg ]( )
        {
            m_ProductNotify( bootedMsg );
        }, m_ProductTask );
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
    {
        Callback< uint32_t > callback( std::bind( &CustomProductAudioService::RebroadcastLatencyCallback,
                                                  this,
                                                  std::placeholders::_1 ) );
        m_APPointer->RegisterForRebroadcastLatency( callback );
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
    BOSE_DEBUG( s_logger, "%s - contentItem = %s", __func__, contentItem.c_str() );

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

        /// UpdateContentItem will re-populate the AudioSetting value and persistence level caused by content item change
        /// in m_AudioSetting Json and m_currentXX proto,
        /// and if value is changed, it will also send new values to frontdoor(notification) and data collection service
        bool isChanged = false;
        isChanged = ( m_AudioSettingsMgr->UpdateContentItem( contentItemProto, kBassName,              m_AudioBassSetting ) ) ? true : isChanged;
        isChanged = ( m_AudioSettingsMgr->UpdateContentItem( contentItemProto, kTrebleName,            m_AudioTrebleSetting ) ) ? true : isChanged;
        isChanged = ( m_AudioSettingsMgr->UpdateContentItem( contentItemProto, kCenterName,            m_AudioCenterSetting ) ) ? true : isChanged;
        isChanged = ( m_AudioSettingsMgr->UpdateContentItem( contentItemProto, kSurroundName,          m_AudioSurroundSetting ) ) ? true : isChanged;
        isChanged = ( m_AudioSettingsMgr->UpdateContentItem( contentItemProto, kSurroundDelayName,     m_AudioSurroundDelaySetting ) ) ? true : isChanged;
        isChanged = ( m_AudioSettingsMgr->UpdateContentItem( contentItemProto, kGainOffsetName,        m_AudioGainOffsetSetting ) ) ? true : isChanged;
        isChanged = ( m_AudioSettingsMgr->UpdateContentItem( contentItemProto, kAvSyncName,            m_AudioAvSyncSetting ) ) ? true : isChanged;
        isChanged = ( m_AudioSettingsMgr->UpdateContentItem( contentItemProto, kSubwooferGainName,     m_AudioSubwooferGainSetting ) ) ? true : isChanged;
        isChanged = ( m_AudioSettingsMgr->UpdateContentItem( contentItemProto, kModeName,              m_AudioModeSetting ) ) ? true : isChanged;
        isChanged = ( m_AudioSettingsMgr->UpdateContentItem( contentItemProto, kContentTypeName,       m_AudioContentTypeSetting ) ) ? true : isChanged;
        isChanged = ( m_AudioSettingsMgr->UpdateContentItem( contentItemProto, kDualMonoSelectName,    m_DualMonoSelectSetting ) ) ? true : isChanged;
        isChanged = ( m_AudioSettingsMgr->UpdateContentItem( contentItemProto, kEqSelectName,          m_EqSelectSetting ) ) ? true : isChanged;
        isChanged = ( m_AudioSettingsMgr->UpdateContentItem( contentItemProto, kSubwooferPolarityName, m_SubwooferPolaritySetting ) ) ? true : isChanged;

        if( isChanged )
        {
            FetchLatestAudioSettings();
            m_AudioSettingsMgr->PersistAudioSettings();
        }

        // Update input route
        if( contentItemProto.source() == SHELBY_SOURCE::PRODUCT )
        {
            m_InputRoute = ( 1 << AUDIO_INPUT_BIT_POSITION_SPDIF_OPTICAL ) |
                           ( 1 << AUDIO_INPUT_BIT_POSITION_SPDIF_ARC ) |
                           ( 1 << AUDIO_INPUT_BIT_POSITION_EARC );
        }
        else if( contentItemProto.source() == SHELBY_SOURCE::SETUP )
        {
            m_InputRoute = 0;
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
/// @name   CustomProductAudioService::SendAudioSettingsToDataCollection
///
/// @brief  Send all AudioSettings to DataCollectionService
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductAudioService::SendAudioSettingsToDataCollection( ) const
{
    BOSE_DEBUG( s_logger, __func__ );
    m_DataCollectionClient->SendData(
        std::make_shared< AudioBassLevel >( m_AudioSettingsMgr->GetBass( ) ),
        DATA_COLLECTION_BASS );
    m_DataCollectionClient->SendData(
        std::make_shared< AudioTrebleLevel >( m_AudioSettingsMgr->GetTreble( ) ),
        DATA_COLLECTION_TREBLE );
    m_DataCollectionClient->SendData(
        std::make_shared< AudioCenterLevel >( m_AudioSettingsMgr->GetCenter( ) ),
        DATA_COLLECTION_CENTER );
    m_DataCollectionClient->SendData(
        std::make_shared< AudioSurroundLevel >( m_AudioSettingsMgr->GetSurround( ) ),
        DATA_COLLECTION_SURROUND );
    m_DataCollectionClient->SendData(
        std::make_shared< AudioSurroundDelay >( m_AudioSettingsMgr->GetSurroundDelay( ) ),
        DATA_COLLECTION_SURROUND_DELAY );
    m_DataCollectionClient->SendData(
        std::make_shared< AudioGainOffset >( m_AudioSettingsMgr->GetGainOffset( ) ),
        DATA_COLLECTION_GAIN_OFFSET );
    m_DataCollectionClient->SendData(
        std::make_shared< AudioAvSync >( m_AudioSettingsMgr->GetAvSync( ) ),
        DATA_COLLECTION_AVSYNC );
    m_DataCollectionClient->SendData(
        std::make_shared< AudioSubwooferGain >( m_AudioSettingsMgr->GetSubwooferGain( ) ),
        DATA_COLLECTION_SUBWOOFER_GAIN );
    m_DataCollectionClient->SendData(
        std::make_shared< AudioMode >( m_AudioSettingsMgr->GetMode( ) ),
        DATA_COLLECTION_MODE );
    m_DataCollectionClient->SendData(
        std::make_shared< AudioContentType >( m_AudioSettingsMgr->GetContentType( ) ),
        DATA_COLLECTION_CONTENT_TYPE );
    m_DataCollectionClient->SendData(
        std::make_shared< AudioDualMonoSelect >( m_AudioSettingsMgr->GetDualMonoSelect( ) ),
        DATA_COLLECTION_DUALMONO_SELECT );
    m_DataCollectionClient->SendData(
        std::make_shared< AudioEqSelect >( m_AudioSettingsMgr->GetEqSelect( ) ),
        DATA_COLLECTION_EQSELECT );
    m_DataCollectionClient->SendData(
        std::make_shared< AudioSubwooferPolarity >( m_AudioSettingsMgr->GetSubwooferPolarity( ) ),
        DATA_COLLECTION_SUBWOOFER_POLARITY );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   CustomProductAudioService::RebroadcastLatencyCallback
///
/// @param  uint32_t latency
///
/// @brief  Callback function, when AudioPath wants to notify ProductController about the
///         rebroadcastLatency value
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductAudioService::RebroadcastLatencyCallback( uint32_t latency )
{
    BOSE_DEBUG( s_logger, "%s: latency = %d", __func__, latency );
    if( latency != m_MainStreamAudioSettings.additionallocallatencyms() )
    {
        m_MainStreamAudioSettings.set_additionallocallatencyms( latency );
        SendMainStreamAudioSettingsEvent();
    }
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
    m_MainStreamAudioSettings.set_appxvideolatencyms( m_AudioSettingsMgr->GetAvSync( ).value() );
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
/// @name   CustomProductAudioService::SetNetworkSourceLatency
///
/// @param  uint32_t latency
///
/// @brief  Send DSP networkSourceLatency to AudioPath
///
/////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductAudioService::SetNetworkSourceLatency( uint32_t latency )
{
    BOSE_VERBOSE( s_logger, __func__ );
    if( ( latency == LpmServiceMessages::LATENCY_VALUE_UNKNOWN ) or ( latency == m_currentNetworkSourceLatency ) )
    {
        return;
    }
    m_currentNetworkSourceLatency = latency;
    auto respCb = []( uint32_t resp )
    {
        BOSE_VERBOSE( s_logger, "%s: received callback with latency(%d)", __func__, resp );
    };
    BOSE_INFO( s_logger, "%s: sending network source latency(%d) to AudioPath", __func__, m_currentNetworkSourceLatency );
    m_APPointer -> SetNetworkSourceLatency( m_currentNetworkSourceLatency, respCb );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   CustomProductAudioService::SetTVSourceLatency
///
/// @param  uint32_t latency
///
/// @brief  Send DSP TVSourceLatency to AudioPath
///
/////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductAudioService::SetTVSourceLatency( uint32_t latency )
{
    BOSE_VERBOSE( s_logger, __func__ );
    if( ( latency == LpmServiceMessages::LATENCY_VALUE_UNKNOWN ) or ( latency == m_currentTVSourceLatency ) )
    {
        return;
    }
    m_currentTVSourceLatency = latency;
    auto respCb = []( uint32_t resp )
    {
        BOSE_VERBOSE( s_logger, "%s: received callback with latency(%d)", __func__, resp );
    };
    BOSE_INFO( s_logger, "%s: sending network source latency(%d) to AudioPath", __func__, m_currentTVSourceLatency );
    m_APPointer -> SetTVSourceLatency( m_currentTVSourceLatency, respCb );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   CustomProductAudioService::ThermalDataReceivedCb
///
/// @param  IpcSystemTemperatureData_t data
///
/// @brief  Callback function, when thermal task receives thermal data from LPM
///
/////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductAudioService::ThermalDataReceivedCb( IpcSystemTemperatureData_t data )
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
    BOSE_DEBUG( s_logger, "%s: installed = %s", __func__, installed ? "true" : "false" );
    AudioSettingResultCode::ResultCode_t ret = m_AudioSettingsMgr->UpdateEqSelectSupportedMode( AudioEqSelect_supportedMode_Name( AudioEqSelect_supportedMode_EQ_AIQ_A ), installed );
    if( ret == AudioSettingResultCode::ResultCode_t::NO_ERROR )
    {
        m_FrontDoorClientIF->SendNotification( FRONTDOOR_AUDIO_EQSELECT_API, m_AudioSettingsMgr->GetEqSelect( ) );
    }
    m_deferredEqSelectResponse( m_AudioSettingsMgr->GetEqSelect() );
    m_deferredEqSelectResponse = AsyncCallback<AudioEqSelect>( []( AudioEqSelect eq ) {}, m_ProductTask );
    m_currentEqSelectUpdating = false;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name CustomProductAudioService::ToggleAudioMode
///
/// @brief Toggles audio mode between DIALOG and NORMAL, updates FrontDoor and DataCollection accordingly.
///
//////////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductAudioService::ToggleAudioMode()
{
    AudioMode mode = m_AudioSettingsMgr->GetMode();

    if( mode.value() == "DIALOG" )
    {
        BOSE_INFO( s_logger, "Toggling AudioMode: Currently DIALOG" );
        mode.set_value( "NORMAL" );
        m_AudioSettingsMgr->SetMode( mode );
    }
    else if( mode.value() == "NORMAL" )
    {
        BOSE_INFO( s_logger, "Toggling AudioMode: Currently NORMAL" );
        mode.set_value( "DIALOG" );
        m_AudioSettingsMgr->SetMode( mode );
    }
    else
    {
        BOSE_ERROR( s_logger, "Unexpected AudioMode %s found in %s", mode.value().c_str(), __func__ );
    }

    m_AudioModeSetting->SendFrontDoorNotification();
    m_AudioModeSetting->SendToDataCollection();
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
        AudioSettingResultCode::ResultCode_t error = m_AudioSettingsMgr->SetBass( val );

        if( error == AudioSettingResultCode::ResultCode_t::NO_ERROR )
        {
            m_MainStreamAudioSettings.set_basslevel( m_AudioSettingsMgr->GetBass( ).value() );
            SendMainStreamAudioSettingsEvent();
        }
        return error;
    };
    auto refreshBassAction = [this]( )
    {
        return m_AudioSettingsMgr->RefreshBass( );
    };
    m_AudioBassSetting = std::unique_ptr< AudioSetting< AudioBassLevel > >
                         ( new AudioSetting< AudioBassLevel >
                           ( FRONTDOOR_AUDIO_BASS_API,
                             getBassAction,
                             setBassAction,
                             refreshBassAction,
                             m_FrontDoorClientIF,
                             m_ProductTask,
                             FRONTDOOR_PRODUCT_CONTROLLER_VERSION,
                             FRONTDOOR_PRODUCT_CONTROLLER_GROUP_NAME,
                             DATA_COLLECTION_BASS,
                             m_DataCollectionClient ) );

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// Endpoint /audio/treble - register handlers for POST/PUT/GET requests
    ///////////////////////////////////////////////////////////////////////////////////////////////
    auto getTrebleAction = [ this ]( )
    {
        return m_AudioSettingsMgr->GetTreble( );
    };
    auto setTrebleAction = [ this ]( AudioTrebleLevel val )
    {
        AudioSettingResultCode::ResultCode_t error = m_AudioSettingsMgr->SetTreble( val );

        if( error == AudioSettingResultCode::ResultCode_t::NO_ERROR )
        {
            m_MainStreamAudioSettings.set_treblelevel( m_AudioSettingsMgr->GetTreble( ).value() );
            SendMainStreamAudioSettingsEvent();
        }
        return error;
    };
    auto refreshTrebleAction = [this]( )
    {
        return m_AudioSettingsMgr->RefreshTreble( );
    };
    m_AudioTrebleSetting = std::unique_ptr< AudioSetting< AudioTrebleLevel > >
                           ( new AudioSetting< AudioTrebleLevel >
                             ( FRONTDOOR_AUDIO_TREBLE_API,
                               getTrebleAction,
                               setTrebleAction,
                               refreshTrebleAction,
                               m_FrontDoorClientIF,
                               m_ProductTask,
                               FRONTDOOR_PRODUCT_CONTROLLER_VERSION,
                               FRONTDOOR_PRODUCT_CONTROLLER_GROUP_NAME,
                               DATA_COLLECTION_TREBLE,
                               m_DataCollectionClient ) );

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// Endpoint /audio/center - register handlers for POST/PUT/GET requests
    ///////////////////////////////////////////////////////////////////////////////////////////////
    auto getCenterAction = [ this ]( )
    {
        return m_AudioSettingsMgr->GetCenter( );
    };
    auto setCenterAction = [ this ]( AudioCenterLevel val )
    {
        AudioSettingResultCode::ResultCode_t error = m_AudioSettingsMgr->SetCenter( val );

        if( error == AudioSettingResultCode::ResultCode_t::NO_ERROR )
        {
            m_MainStreamAudioSettings.set_centerlevel( m_AudioSettingsMgr->GetCenter( ).value() );
            SendMainStreamAudioSettingsEvent();
        }
        return error;
    };
    auto refreshCenterAction = [this]( )
    {
        return m_AudioSettingsMgr->RefreshCenter( );
    };
    m_AudioCenterSetting = std::unique_ptr< AudioSetting< AudioCenterLevel > >
                           ( new AudioSetting<AudioCenterLevel>
                             ( FRONTDOOR_AUDIO_CENTER_API,
                               getCenterAction,
                               setCenterAction,
                               refreshCenterAction,
                               m_FrontDoorClientIF,
                               m_ProductTask,
                               FRONTDOOR_PRODUCT_CONTROLLER_VERSION,
                               FRONTDOOR_PRODUCT_CONTROLLER_GROUP_NAME,
                               DATA_COLLECTION_CENTER,
                               m_DataCollectionClient ) );

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// Endpoint /audio/surround - register handlers for POST/PUT/GET requests
    ///////////////////////////////////////////////////////////////////////////////////////////////
    auto getSurroundAction = [ this ]( )
    {
        return m_AudioSettingsMgr->GetSurround( );
    };
    auto setSurroundAction = [ this ]( AudioSurroundLevel val )
    {
        AudioSettingResultCode::ResultCode_t error = m_AudioSettingsMgr->SetSurround( val );

        if( error == AudioSettingResultCode::ResultCode_t::NO_ERROR )
        {
            m_MainStreamAudioSettings.set_surroundlevel( m_AudioSettingsMgr->GetSurround( ).value() );
            SendMainStreamAudioSettingsEvent();
        }
        return error;
    };
    auto refreshSurroundAction = [this]( )
    {
        return m_AudioSettingsMgr->RefreshSurround( );
    };
    m_AudioSurroundSetting = std::unique_ptr< AudioSetting< AudioSurroundLevel > >
                             ( new AudioSetting< AudioSurroundLevel >
                               ( FRONTDOOR_AUDIO_SURROUND_API,
                                 getSurroundAction,
                                 setSurroundAction,
                                 refreshSurroundAction,
                                 m_FrontDoorClientIF,
                                 m_ProductTask,
                                 FRONTDOOR_PRODUCT_CONTROLLER_VERSION,
                                 FRONTDOOR_PRODUCT_CONTROLLER_GROUP_NAME,
                                 DATA_COLLECTION_SURROUND,
                                 m_DataCollectionClient ) );

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// Endpoint /audio/surroundDelay - register handlers for POST/PUT/GET requests
    ///////////////////////////////////////////////////////////////////////////////////////////////
    auto getSurroundDelayAction = [ this ]( )
    {
        return m_AudioSettingsMgr->GetSurroundDelay( );
    };
    auto setSurroundDelayAction = [ this ]( AudioSurroundDelay val )
    {
        AudioSettingResultCode::ResultCode_t error = m_AudioSettingsMgr->SetSurroundDelay( val );
        if( error == AudioSettingResultCode::ResultCode_t::NO_ERROR )
        {
            m_MainStreamAudioSettings.set_surrounddelay( m_AudioSettingsMgr->GetSurroundDelay( ).value() );
            SendMainStreamAudioSettingsEvent();
        }
        return error;
    };
    auto refreshSurroundDelayAction = [this]( )
    {
        return m_AudioSettingsMgr->RefreshSurroundDelay( );
    };
    m_AudioSurroundDelaySetting = std::unique_ptr< AudioSetting< AudioSurroundDelay > >
                                  ( new AudioSetting< AudioSurroundDelay >
                                    ( FRONTDOOR_AUDIO_SURROUNDDELAY_API,
                                      getSurroundDelayAction,
                                      setSurroundDelayAction,
                                      refreshSurroundDelayAction,
                                      m_FrontDoorClientIF,
                                      m_ProductTask,
                                      FRONTDOOR_PRODUCT_CONTROLLER_VERSION,
                                      FRONTDOOR_PRODUCT_CONTROLLER_GROUP_NAME,
                                      DATA_COLLECTION_SURROUND_DELAY,
                                      m_DataCollectionClient ) );

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// Endpoint /audio/gainOffset - register handlers for POST/PUT/GET requests
    ///////////////////////////////////////////////////////////////////////////////////////////////
    auto getGainOffsetAction = [ this ]( )
    {
        return m_AudioSettingsMgr->GetGainOffset( );
    };
    auto setGainOffsetAction = [ this ]( AudioGainOffset val )
    {
        AudioSettingResultCode::ResultCode_t error = m_AudioSettingsMgr->SetGainOffset( val );

        if( error == AudioSettingResultCode::ResultCode_t::NO_ERROR )
        {
            m_MainStreamAudioSettings.set_gainoffset( m_AudioSettingsMgr->GetGainOffset( ).value() );
            SendMainStreamAudioSettingsEvent();
        }
        return error;
    };
    auto refreshGainOffsetAction = [this]( )
    {
        return m_AudioSettingsMgr->RefreshGainOffset( );
    };
    m_AudioGainOffsetSetting =  std::unique_ptr< AudioSetting< AudioGainOffset > >
                                ( new AudioSetting< AudioGainOffset >
                                  ( FRONTDOOR_AUDIO_GAINOFFSET_API,
                                    getGainOffsetAction,
                                    setGainOffsetAction,
                                    refreshGainOffsetAction,
                                    m_FrontDoorClientIF,
                                    m_ProductTask,
                                    FRONTDOOR_PRODUCT_CONTROLLER_VERSION,
                                    FRONTDOOR_PRODUCT_CONTROLLER_GROUP_NAME,
                                    DATA_COLLECTION_GAIN_OFFSET,
                                    m_DataCollectionClient ) );

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// Endpoint /audio/avSync - register handlers for POST/PUT/GET requests
    ///////////////////////////////////////////////////////////////////////////////////////////////
    auto getAvSyncAction = [ this ]( )
    {
        return m_AudioSettingsMgr->GetAvSync( );
    };
    auto setAvSyncAction = [ this ]( AudioAvSync val )
    {
        AudioSettingResultCode::ResultCode_t error = m_AudioSettingsMgr->SetAvSync( val );

        if( error == AudioSettingResultCode::ResultCode_t::NO_ERROR )
        {
            m_MainStreamAudioSettings.set_appxvideolatencyms( m_AudioSettingsMgr->GetAvSync( ).value() );
            SendMainStreamAudioSettingsEvent();
        }
        return error;
    };
    auto refreshAvSyncAction = [this]( )
    {
        return m_AudioSettingsMgr->RefreshAvSync( );
    };
    m_AudioAvSyncSetting = std::unique_ptr< AudioSetting< AudioAvSync > >
                           ( new AudioSetting< AudioAvSync >
                             ( FRONTDOOR_AUDIO_AVSYNC_API,
                               getAvSyncAction,
                               setAvSyncAction,
                               refreshAvSyncAction,
                               m_FrontDoorClientIF,
                               m_ProductTask,
                               FRONTDOOR_PRODUCT_CONTROLLER_VERSION,
                               FRONTDOOR_PRODUCT_CONTROLLER_GROUP_NAME,
                               DATA_COLLECTION_AVSYNC,
                               m_DataCollectionClient ) );

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// Endpoint /audio/subWooferGain - register handlers for POST/PUT/GET requests
    ///////////////////////////////////////////////////////////////////////////////////////////////
    auto getSubwooferGainAction = [ this ]( )
    {
        return m_AudioSettingsMgr->GetSubwooferGain( );
    };
    auto setSubwooferGainAction = [ this ]( AudioSubwooferGain val )
    {
        AudioSettingResultCode::ResultCode_t error = m_AudioSettingsMgr->SetSubwooferGain( val );

        if( error == AudioSettingResultCode::ResultCode_t::NO_ERROR )
        {
            m_MainStreamAudioSettings.set_subwooferlevel(
                m_AudioSettingsMgr->GetSubwooferGain( ).value() );

            SendMainStreamAudioSettingsEvent();
        }
        return error;
    };
    auto refreshSubwooferGainAction = [this]( )
    {
        return m_AudioSettingsMgr->RefreshSubwooferGain( );
    };
    m_AudioSubwooferGainSetting = std::unique_ptr< AudioSetting<AudioSubwooferGain > >
                                  ( new AudioSetting< AudioSubwooferGain >
                                    ( FRONTDOOR_AUDIO_SUBWOOFERGAIN_API,
                                      getSubwooferGainAction,
                                      setSubwooferGainAction,
                                      refreshSubwooferGainAction,
                                      m_FrontDoorClientIF,
                                      m_ProductTask,
                                      FRONTDOOR_PRODUCT_CONTROLLER_VERSION,
                                      FRONTDOOR_PRODUCT_CONTROLLER_GROUP_NAME,
                                      DATA_COLLECTION_SUBWOOFER_GAIN,
                                      m_DataCollectionClient ) );

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// Endpoint /audio/mode - register handlers for POST/PUT/GET requests
    ///////////////////////////////////////////////////////////////////////////////////////////////
    auto getModeAction = [ this ]( )
    {
        return m_AudioSettingsMgr->GetMode( );
    };
    auto setModeAction = [ this ]( AudioMode val )
    {
        AudioSettingResultCode::ResultCode_t error = m_AudioSettingsMgr->SetMode( val );

        if( error == AudioSettingResultCode::ResultCode_t::NO_ERROR )
        {
            m_MainStreamAudioSettings.set_audiomode(
                ModeNameToEnum( m_AudioSettingsMgr->GetMode( ).value() ) );
            SendMainStreamAudioSettingsEvent();
        }
        return error;
    };
    auto refreshModeAction = [this]( )
    {
        return m_AudioSettingsMgr->RefreshMode( );
    };
    m_AudioModeSetting = std::unique_ptr< AudioSetting<AudioMode > >
                         ( new AudioSetting< AudioMode >
                           ( FRONTDOOR_AUDIO_MODE_API,
                             getModeAction,
                             setModeAction,
                             refreshModeAction,
                             m_FrontDoorClientIF,
                             m_ProductTask,
                             FRONTDOOR_PRODUCT_CONTROLLER_VERSION,
                             FRONTDOOR_PRODUCT_CONTROLLER_GROUP_NAME,
                             DATA_COLLECTION_MODE,
                             m_DataCollectionClient ) );

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// Endpoint /audio/contentType - register handlers for POST/PUT/GET requests
    ///////////////////////////////////////////////////////////////////////////////////////////////
    auto getContentTypeAction = [ this ]( )
    {
        return m_AudioSettingsMgr->GetContentType( );
    };
    auto setContentTypeAction = [ this ]( AudioContentType val )
    {
        AudioSettingResultCode::ResultCode_t error = m_AudioSettingsMgr->SetContentType( val );

        if( error == AudioSettingResultCode::ResultCode_t::NO_ERROR )
        {
            m_MainStreamAudioSettings.set_contenttype( ContentTypeNameToEnum(
                                                           m_AudioSettingsMgr->GetContentType( ).value() ) );
            SendMainStreamAudioSettingsEvent();
        }
        return error;
    };
    auto refreshContentTypeAction = [this]( )
    {
        return m_AudioSettingsMgr->RefreshContentType( );
    };
    m_AudioContentTypeSetting = std::unique_ptr< AudioSetting<AudioContentType > >
                                ( new AudioSetting< AudioContentType >
                                  ( FRONTDOOR_AUDIO_CONTENTTYPE_API,
                                    getContentTypeAction,
                                    setContentTypeAction,
                                    refreshContentTypeAction,
                                    m_FrontDoorClientIF,
                                    m_ProductTask,
                                    FRONTDOOR_PRODUCT_CONTROLLER_VERSION,
                                    FRONTDOOR_PRODUCT_CONTROLLER_GROUP_NAME,
                                    DATA_COLLECTION_CONTENT_TYPE,
                                    m_DataCollectionClient ) );

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// Endpoint /audio/dualMonoSelect - register handlers for POST/PUT/GET requests
    ///////////////////////////////////////////////////////////////////////////////////////////////
    auto getDualMonoSelectAction = [ this ]( )
    {
        return m_AudioSettingsMgr->GetDualMonoSelect( );
    };
    auto setDualMonoSelectAction = [ this ]( AudioDualMonoSelect val )
    {
        AudioSettingResultCode::ResultCode_t error = m_AudioSettingsMgr->SetDualMonoSelect( val );

        if( error == AudioSettingResultCode::ResultCode_t::NO_ERROR )
        {
            m_MainStreamAudioSettings.set_dualmonoselect( DualMonoSelectNameToEnum(
                                                              m_AudioSettingsMgr->GetDualMonoSelect( ).value() ) );
            SendMainStreamAudioSettingsEvent();
        }
        return error;
    };
    auto refreshDualMonoSelectAction = [this]( )
    {
        return m_AudioSettingsMgr->RefreshDualMonoSelect( );
    };
    m_DualMonoSelectSetting = std::unique_ptr< AudioSetting<AudioDualMonoSelect > >
                              ( new AudioSetting< AudioDualMonoSelect >
                                ( FRONTDOOR_AUDIO_DUALMONOSELECT_API,
                                  getDualMonoSelectAction,
                                  setDualMonoSelectAction,
                                  refreshDualMonoSelectAction,
                                  m_FrontDoorClientIF,
                                  m_ProductTask,
                                  FRONTDOOR_PRODUCT_CONTROLLER_VERSION,
                                  FRONTDOOR_PRODUCT_CONTROLLER_GROUP_NAME,
                                  DATA_COLLECTION_DUALMONO_SELECT,
                                  m_DataCollectionClient ) );

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// Endpoint /audio/eqSelect - register handlers for POST/PUT/GET requests
    ///////////////////////////////////////////////////////////////////////////////////////////////
    auto getEqSelectAction = [ this ]( )
    {
        return m_AudioSettingsMgr->GetEqSelect( );
    };
    auto setEqSelectAction = [ this ]( AudioEqSelect val )
    {
        AudioSettingResultCode::ResultCode_t error = m_AudioSettingsMgr->SetEqSelect( val );

        if( error == AudioSettingResultCode::ResultCode_t::NO_ERROR )
        {
            m_MainStreamAudioSettings.set_deltaeqselect(
                EqSelectNameToEnum( m_AudioSettingsMgr->GetEqSelect( ).mode() ) );
            SendMainStreamAudioSettingsEvent();
        }
        return error;
    };
    auto refreshEqSelectAction = []( ) {};

    auto getEqSelectDeferred = [ this ]( Callback<AudioEqSelect> respCb )
    {
        if( !m_currentEqSelectUpdating )
        {
            // If we know EQ select isn't in the process of updating, just answer with the current value
            respCb( m_AudioSettingsMgr->GetEqSelect() );
            return;
        }

        // Wait for the next status to arrive from the DSP before answering.  This is important
        // to eliminate a race condition in which AiQ has just completed, the DSP is in the process
        // of rebooting (so we haven't received the list of updated supported modes yet), and
        // Madrid queries this endpoint.  In this case, we need to defer the response until we get
        // the next update from the DSP.
        m_deferredEqSelectResponse = AsyncCallback<AudioEqSelect>( respCb, m_ProductTask );
    };
    m_EqSelectSetting = std::unique_ptr< AudioSetting< AudioEqSelect > >
                        ( new AudioSetting< AudioEqSelect >
                          ( FRONTDOOR_AUDIO_EQSELECT_API,
                            getEqSelectAction,
                            setEqSelectAction,
                            refreshEqSelectAction,
                            m_FrontDoorClientIF,
                            m_ProductTask,
                            FRONTDOOR_PRODUCT_CONTROLLER_VERSION,
                            FRONTDOOR_PRODUCT_CONTROLLER_GROUP_NAME,
                            DATA_COLLECTION_EQSELECT,
                            m_DataCollectionClient,
                            getEqSelectDeferred ) );

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// Endpoint /audio/SubwooferPolarity - register handlers for POST/PUT/GET requests
    ///////////////////////////////////////////////////////////////////////////////////////////////
    auto getSubwooferPolarityAction = [ this ]( )
    {
        return m_AudioSettingsMgr->GetSubwooferPolarity( );
    };
    auto setSubwooferPolarityAction = [ this ]( AudioSubwooferPolarity val )
    {
        AudioSettingResultCode::ResultCode_t error = m_AudioSettingsMgr->SetSubwooferPolarity( val );
        if( error == AudioSettingResultCode::ResultCode_t::NO_ERROR )
        {
            m_MainStreamAudioSettings.set_subwooferpolarity(
                SubwooferPolarityNameToEnum( m_AudioSettingsMgr->GetSubwooferPolarity( ).value() ) );
            SendMainStreamAudioSettingsEvent();
        }
        return error;
    };
    auto refreshSubwooferPolarityAction = []( ) {};
    m_SubwooferPolaritySetting = std::unique_ptr< AudioSetting< AudioSubwooferPolarity > >
                                 ( new AudioSetting< AudioSubwooferPolarity >
                                   ( FRONTDOOR_AUDIO_SUBWOOFERPOLARITY_API,
                                     getSubwooferPolarityAction,
                                     setSubwooferPolarityAction,
                                     refreshSubwooferPolarityAction,
                                     m_FrontDoorClientIF,
                                     m_ProductTask,
                                     FRONTDOOR_PRODUCT_CONTROLLER_VERSION,
                                     FRONTDOOR_PRODUCT_CONTROLLER_GROUP_NAME,
                                     DATA_COLLECTION_SUBWOOFER_POLARITY,
                                     m_DataCollectionClient ) );
}

}// namespace ProductApp
