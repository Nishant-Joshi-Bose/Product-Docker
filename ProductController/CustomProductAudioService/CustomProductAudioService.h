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
#include "DeviceControllerClientIF.h"


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
        m_dspIsRebooting = true;
        m_productLpmHardwareInterface->BootDSPImage( image );
    }
    void ToggleAudioMode();

    //////////////////////////////////////////////////////////////////////////////////////////////
    /// m_MainStreamAudioSettings is the structure holding information that APProduct would like to know
    ///                             including audio settings and thermal data
    /// m_inputRoute is the current physical DSP input that should be used, based on current source info from contentItem
    /////////////////////////////////////////////////////////////////////////////////////////////
    LpmServiceMessages::AudioSettings_t m_mainStreamAudioSettings;
    uint32_t m_inputRoute = 0;

private:
    std::shared_ptr<CustomProductLpmHardwareInterface>  m_productLpmHardwareInterface;
    std::unique_ptr<CustomAudioSettingsManager>         m_audioSettingsMgr;
    std::unique_ptr<ThermalMonitorTask>                 m_thermalTask;
    std::shared_ptr< DataCollectionClientIF >           m_dataCollectionClient;
    DeviceController::DeviceControllerClientIF::DeviceControllerClientPtr m_deviceControllerPtr;
    bool                                                m_dspIsRebooting = false;
    bool                                                m_currentEqSelectUpdating = false;
    Callback<bool>                                      m_streamConfigResponseCb;

    //////////////////////////////////////////////////////////////////////////////////////////////
    /// Front Door handlers
    /////////////////////////////////////////////////////////////////////////////////////////////
    std::unique_ptr<AudioSetting<ProductPb::AudioBassLevel>>            m_audioBassSetting;
    std::unique_ptr<AudioSetting<ProductPb::AudioTrebleLevel>>          m_audioTrebleSetting;
    std::unique_ptr<AudioSetting<ProductPb::AudioCenterLevel>>          m_audioCenterSetting;
    std::unique_ptr<AudioSetting<ProductPb::AudioSurroundLevel>>        m_audioSurroundSetting;
    std::unique_ptr<AudioSetting<ProductPb::AudioSurroundDelay>>        m_audioSurroundDelaySetting;
    std::unique_ptr<AudioSetting<ProductPb::AudioGainOffset>>           m_audioGainOffsetSetting;
    std::unique_ptr<AudioSetting<ProductPb::AudioAvSync>>               m_audioAvSyncSetting;
    std::unique_ptr<AudioSetting<ProductPb::AudioSubwooferGain>>        m_audioSubwooferGainSetting;
    std::unique_ptr<AudioSetting<ProductPb::AudioMode>>                 m_audioModeSetting;
    std::unique_ptr<AudioSetting<ProductPb::AudioContentType>>          m_audioContentTypeSetting;
    std::unique_ptr<AudioSetting<ProductPb::AudioDualMonoSelect>>       m_dualMonoSelectSetting;
    std::unique_ptr<AudioSetting<ProductPb::AudioEqSelect>>             m_eqSelectSetting;
    std::unique_ptr<AudioSetting<ProductPb::AudioSubwooferPolarity>>    m_subwooferPolaritySetting;

    /////////////////////////////////////////////////////////////////////////////////////////////////
    /// APProduct handling functions
    /////////////////////////////////////////////////////////////////////////////////////////////////
    void RegisterAudioPathEvents() override;
    void GetMainStreamAudioSettingsCallback( const APProductCommon::MainStreamAudioSettingsParam_t& param,
                                             const Callback<std::string, std::string> cb );
    void SetStreamConfigCallback( std::vector<APProductCommon::ChannelParameters> channelParams, std::string serializedAudioSettings, std::string serializedInputRoute, const Callback<bool> cb );
    void PlaybackStatusCallback( const std::vector< APProductCommon::PlaybackStatus >& playbacks ) override;
    void InternalMuteCallback( bool mute );
    void RebroadcastLatencyCallback( uint32_t latency );
    void SendMainStreamAudioSettingsEvent();
    void ThermalDataReceivedCb( IpcSystemTemperatureData_t data );

    void SendAudioSettingsToDataCollection() const;

    /////////////////////////////////////////////////////////////////////////////////////////////////
    /// Helper functions to prepare m_mainStreamAudioSettings for APProduct to use
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

    AsyncCallback<ProductPb::AudioEqSelect> m_deferredEqSelectResponse;
};
}// namespace ProductApp
