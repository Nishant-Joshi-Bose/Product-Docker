///////////////////////////////////////////////////////////////////////////////////////////////////
/// @file   CustomProductAudioService.h
/// @brief   This file contains source code for Professor specific behavior for
///         communicating with APProduct Server and APProduct related FrontDoor interaction
/// Copyright 2017 Bose Corporation
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include "ProductAudioService.h"
#include "ProfessorProductController.h"
#include "CustomProductLpmHardwareInterface.h"
#include "CustomAudioSettingsManager.h"

namespace ProductApp
{
//class CustomProductLpmHardwareInterface;

class CustomProductAudioService: public ProductAudioService
{
public:
    CustomProductAudioService( ProfessorProductController& ProductController );
    //////////////////////////////////////////////////////////////////////////////////////////////
    /// m_MainStreamAudioSettings is the structure holding audio settings info that APProduct and DSP would like to know
    /// m_InputRoute is the current physical DSP input that should be used, based on current source info from contentItem
    /////////////////////////////////////////////////////////////////////////////////////////////
    LpmServiceMessages::AudioSettings_t m_MainStreamAudioSettings;
    uint32_t m_InputRoute;

private:
    std::shared_ptr<CustomProductLpmHardwareInterface> m_ProductLpmHardwareInterface;
    std::unique_ptr<CustomAudioSettingsManager>   m_AudioSettingsMgr;

    //////////////////////////////////////////////////////////////////////////////////////////////
    /// Front Door handlers
    /////////////////////////////////////////////////////////////////////////////////////////////
    std::unique_ptr<AudioSetting<ProductPb::AudioBassLevel>>        m_AudioBassSetting;
    std::unique_ptr<AudioSetting<ProductPb::AudioTrebleLevel>>      m_AudioTrebleSetting;
    std::unique_ptr<AudioSetting<ProductPb::AudioCenterLevel>>      m_AudioCenterSetting;
    std::unique_ptr<AudioSetting<ProductPb::AudioSurroundLevel>>    m_AudioSurroundSetting;
    std::unique_ptr<AudioSetting<ProductPb::AudioGainOffset>>       m_AudioGainOffsetSetting;
    std::unique_ptr<AudioSetting<ProductPb::AudioAvSync>>           m_AudioAvSyncsetSetting;
    std::unique_ptr<AudioSetting<ProductPb::AudioMode>>             m_AudioModeSetting;
    std::unique_ptr<AudioSetting<ProductPb::AudioContentType>>      m_AudioContentTypeSetting;
    std::unique_ptr<AudioSetting<ProductPb::AudioDualMonoSelect>>   m_DualMonoSelectSetting;
    /////////////////////////////////////////////////////////////////////////////////////////////////
    /// APProduct handling functions
    /////////////////////////////////////////////////////////////////////////////////////////////////
    void RegisterAudioPathEvents() override;

    void GetMainStreamAudioSettingsCallback( std::string contentItem,  const Callback<std::string, std::string> cb );
    void SetStreamConfigCallback( std::string serializedAudioSettings, std::string serializedInputRoute, const Callback<bool> cb );
    void SendMainStreamAudioSettingsEvent();

    /////////////////////////////////////////////////////////////////////////////////////////////////
    /// Helper functions to convert audio setting values from string format to enumuration required from DSP
    /////////////////////////////////////////////////////////////////////////////////////////////////
    LpmServiceMessages::AudioSettingsAudioMode_t ModeNameToEnum( const std::string& modeName );
    LpmServiceMessages::AudioSettingsContent_t ContentTypeNameToEnum( const std::string& contentTypeName );
    LpmServiceMessages::AudioSettingsDualMonoMode_t DualMonoSelectNameToEnum( const std::string& dualMonoSelectName );

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// FrontDoor handling functions
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    void RegisterFrontDoorEvents() override;
};
}// namespace ProductApp
