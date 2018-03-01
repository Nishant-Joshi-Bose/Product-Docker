///////////////////////////////////////////////////////////////////////////////////////////////////
/// @file   CustomProductAudioService.h
/// @brief   This file contains source code for Professor specific behavior for
///         communicating with APProduct Server and APProduct related FrontDoor interaction
/// Copyright 2017 Bose Corporation
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include "ProductAudioService.h"
#include "CustomProductLpmHardwareInterface.h"
#include "CustomAudioSettingsManager.h"
#include "ThermalMonitorTask.h"
#include "FrontDoorClient.h"

namespace ProductApp
{
class ProfessorProductController;
class CustomProductAudioService: public ProductAudioService
{
public:
    CustomProductAudioService( ProfessorProductController& ProductController,
                               const FrontDoorClientIF_t& frontDoorClient,
                               LpmClientIF::LpmClientPtr lpmClient );
    void SetThermalMonitorEnabled( bool enabled );

    //////////////////////////////////////////////////////////////////////////////////////////////
    /// m_MainStreamAudioSettings is the structure holding information that APProduct would like to know
    ///                             including audio settings and thermal data
    /// m_InputRoute is the current physical DSP input that should be used, based on current source info from contentItem
    /////////////////////////////////////////////////////////////////////////////////////////////
    LpmServiceMessages::AudioSettings_t m_MainStreamAudioSettings;
    uint32_t m_InputRoute;

private:
    std::shared_ptr<CustomProductLpmHardwareInterface>  m_ProductLpmHardwareInterface;
    std::unique_ptr<CustomAudioSettingsManager>         m_AudioSettingsMgr;
    std::unique_ptr<ThermalMonitorTask>                 m_ThermalTask;

    //////////////////////////////////////////////////////////////////////////////////////////////
    /// Front Door handlers
    /////////////////////////////////////////////////////////////////////////////////////////////
    std::unique_ptr<AudioSetting<ProductPb::AudioBassLevel>>        m_AudioBassSetting;
    std::unique_ptr<AudioSetting<ProductPb::AudioTrebleLevel>>      m_AudioTrebleSetting;
    std::unique_ptr<AudioSetting<ProductPb::AudioCenterLevel>>      m_AudioCenterSetting;
    std::unique_ptr<AudioSetting<ProductPb::AudioSurroundLevel>>    m_AudioSurroundSetting;
    std::unique_ptr<AudioSetting<ProductPb::AudioGainOffset>>       m_AudioGainOffsetSetting;
    std::unique_ptr<AudioSetting<ProductPb::AudioAvSync>>           m_AudioAvSyncsetSetting;
    std::unique_ptr<AudioSetting<ProductPb::AudioSubwooferGain>>    m_AudioSubwooferGainSetting;
    std::unique_ptr<AudioSetting<ProductPb::AudioMode>>             m_AudioModeSetting;
    std::unique_ptr<AudioSetting<ProductPb::AudioContentType>>      m_AudioContentTypeSetting;
    std::unique_ptr<AudioSetting<ProductPb::AudioDualMonoSelect>>   m_DualMonoSelectSetting;
    /////////////////////////////////////////////////////////////////////////////////////////////////
    /// APProduct handling functions
    /////////////////////////////////////////////////////////////////////////////////////////////////
    void RegisterAudioPathEvents() override;
    void GetMainStreamAudioSettingsCallback( std::string contentItem,  const Callback<std::string, std::string> cb );
    void SetStreamConfigCallback( std::vector<APProductCommon::ChannelParameters> channelParams, std::string serializedAudioSettings, std::string serializedInputRoute, const Callback<bool> cb );
    void SendMainStreamAudioSettingsEvent();
    void ThermalDataReceivedCb( const IpcSystemTemperatureData_t& data );

    /////////////////////////////////////////////////////////////////////////////////////////////////
    /// Helper functions to prepare m_MainStreamAudioSettings for APProduct to use
    /////////////////////////////////////////////////////////////////////////////////////////////////
    void FetchLatestAudioSettings();
    LpmServiceMessages::AudioSettingsAudioMode_t ModeNameToEnum( const std::string& modeName );
    LpmServiceMessages::AudioSettingsContent_t ContentTypeNameToEnum( const std::string& contentTypeName );
    LpmServiceMessages::AudioSettingsDualMonoMode_t DualMonoSelectNameToEnum( const std::string& dualMonoSelectName );

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// FrontDoor handling functions
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    void RegisterFrontDoorEvents() override;
};
}// namespace ProductApp
