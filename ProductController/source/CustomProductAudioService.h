///////////////////////////////////////////////////////////////////////////////////////////////////
/// @file   CustomProductAudioService.h
/// @brief   This file contains source code for Professor specific behavior for
///         communicating with APProduct Server and APProduct related FrontDoor interaction
/// Copyright 2017 Bose Corporation
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include "ProductAudioService.h"
#include "ProfessorProductController.h"

namespace ProductApp
{
class CustomProductAudioService: public ProductAudioService
{
public:
    CustomProductAudioService( ProfessorProductController& ProductController );

private:
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

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// FrontDoor handling functions
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    void RegisterFrontDoorEvents() override;
};
}// namespace ProductApp
