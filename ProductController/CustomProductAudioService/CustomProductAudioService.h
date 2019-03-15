///////////////////////////////////////////////////////////////////////////////////////////////////
/// @file   CustomProductAudioService.h
/// @brief   This file contains source code for custom behavior for
///         communicating with APProduct Server and APProduct related FrontDoor interaction
/// Copyright 2017 Bose Corporation
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include "ProductAudioService.h"
#include "AudioSetting.h"
#include "CustomProductLpmHardwareInterface.h"
#include "CustomAudioSettingsManager.h"
#include "ThermalMonitorTask.h"
#include "FrontDoorClient.h"
#include "AudioSettings.pb.h"
#include "DataCollectionClientIF.h"

namespace ProductApp
{
class CustomProductController;
class CustomProductAudioService: public ProductAudioService
{
public:
    CustomProductAudioService( CustomProductController& ProductController,
                               const FrontDoorClientIF_t& frontDoorClient,
                               LpmClientIF::LpmClientPtr lpmClient );
    void SetThermalMonitorEnabled( bool enabled );
    void SetAiqInstalled( bool installed );
    void SetNetworkSourceLatency( uint32_t latency );
    void SetTVSourceLatency( uint32_t latency );
    void BootDSPImage( LpmServiceMessages::IpcImage_t image )
    {
        m_DspIsRebooting = true;
        m_ProductLpmHardwareInterface->BootDSPImage( image );
    }
    void ToggleAudioMode();

    //////////////////////////////////////////////////////////////////////////////////////////////
    /// m_MainStreamAudioSettings is the structure holding information that APProduct would like to know
    ///                             including audio settings and thermal data
    /// m_InputRoute is the current physical DSP input that should be used, based on current source info from contentItem
    /////////////////////////////////////////////////////////////////////////////////////////////
    LpmServiceMessages::AudioSettings_t m_MainStreamAudioSettings;
    uint32_t m_InputRoute = 0;

private:
    std::shared_ptr<CustomProductLpmHardwareInterface>  m_ProductLpmHardwareInterface;
    std::unique_ptr<CustomAudioSettingsManager>         m_AudioSettingsMgr;
    std::unique_ptr<ThermalMonitorTask>                 m_ThermalTask;
    std::shared_ptr< DataCollectionClientIF >           m_DataCollectionClient;
    bool                                                m_DspIsRebooting = false;
    Callback<bool>                                      m_StreamConfigResponseCb;

    //////////////////////////////////////////////////////////////////////////////////////////////
    /// Front Door handlers
    /////////////////////////////////////////////////////////////////////////////////////////////
    std::unique_ptr<AudioSetting<ProductPb::AudioBassLevel>>            m_AudioBassSetting;
    std::unique_ptr<AudioSetting<ProductPb::AudioTrebleLevel>>          m_AudioTrebleSetting;
    std::unique_ptr<AudioSetting<ProductPb::AudioCenterLevel>>          m_AudioCenterSetting;
    std::unique_ptr<AudioSetting<ProductPb::AudioSurroundLevel>>        m_AudioSurroundSetting;
    std::unique_ptr<AudioSetting<ProductPb::AudioSurroundDelay>>        m_AudioSurroundDelaySetting;
    std::unique_ptr<AudioSetting<ProductPb::AudioGainOffset>>           m_AudioGainOffsetSetting;
    std::unique_ptr<AudioSetting<ProductPb::AudioAvSync>>               m_AudioAvSyncSetting;
    std::unique_ptr<AudioSetting<ProductPb::AudioSubwooferGain>>        m_AudioSubwooferGainSetting;
    std::unique_ptr<AudioSetting<ProductPb::AudioMode>>                 m_AudioModeSetting;
    std::unique_ptr<AudioSetting<ProductPb::AudioContentType>>          m_AudioContentTypeSetting;
    std::unique_ptr<AudioSetting<ProductPb::AudioDualMonoSelect>>       m_DualMonoSelectSetting;
    std::unique_ptr<AudioSetting<ProductPb::AudioEqSelect>>             m_EqSelectSetting;
    std::unique_ptr<AudioSetting<ProductPb::AudioSubwooferPolarity>>    m_SubwooferPolaritySetting;

    /////////////////////////////////////////////////////////////////////////////////////////////////
    /// APProduct handling functions
    /////////////////////////////////////////////////////////////////////////////////////////////////
    void RegisterAudioPathEvents() override;
    void GetMainStreamAudioSettingsCallback( std::string contentItem,  const Callback<std::string, std::string> cb );
    void SetStreamConfigCallback( std::vector<APProductCommon::ChannelParameters> channelParams, std::string serializedAudioSettings, std::string serializedInputRoute, const Callback<bool> cb );
    void InternalMuteCallback( bool mute );
    void RebroadcastLatencyCallback( uint32_t latency );
    void SendMainStreamAudioSettingsEvent();
    void ThermalDataReceivedCb( IpcSystemTemperatureData_t data );

    void SendAudioSettingsToDataCollection() const;

    /////////////////////////////////////////////////////////////////////////////////////////////////
    /// Helper functions to prepare m_MainStreamAudioSettings for APProduct to use
    /////////////////////////////////////////////////////////////////////////////////////////////////
    void FetchLatestAudioSettings();
    LpmServiceMessages::AudioSettingsAudioMode_t ModeNameToEnum( const std::string& modeName );
    LpmServiceMessages::AudioSettingsContent_t ContentTypeNameToEnum( const std::string& contentTypeName );
    LpmServiceMessages::AudioSettingsDualMonoMode_t DualMonoSelectNameToEnum( const std::string& dualMonoSelectName );
    LpmServiceMessages::AudioSettingsDeltaEqSelect_t EqSelectNameToEnum( const std::string& modeName );
    LpmServiceMessages::AudioSettingsSubwooferPolarity_t SubwooferPolarityNameToEnum( const std::string& subwooferPolarityName );

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// FrontDoor handling functions
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    void RegisterFrontDoorEvents() override;

    LpmServiceMessages::IpcDspStreamConfigReqPayload_t m_DspStreamConfig;
    uint32_t m_currentNetworkSourceLatency;
    uint32_t m_currentTVSourceLatency;

    Callback<ProductPb::AudioEqSelect> m_deferredEqSelectResponse;
};
}// namespace ProductApp
