///////////////////////////////////////////////////////////////////////////////////////////////////
/// @file   CustomProductAudioService.h
/// @brief   This file contains source code for Professor specific behavior for
///         communicating with APProduct Server and APProduct related FrontDoor interaction
/// Copyright 2017 Bose Corporation
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include "ProductAudioService.h"

namespace ProductApp
{
class CustomProductAudioService: public ProductAudioService
{
public:
    CustomProductAudioService( ProfessorProductController& ProductController );

private:
    Callback< ProductMessage >              m_ProductNotify = nullptr;

    //////////////////////////////////////////////////////////////////////////////////////////////
    /// Front Door handlers
    /////////////////////////////////////////////////////////////////////////////////////////////
    std::shared_ptr<AudioSetting<ProductPb::AudioBassLevel>> m_AudioBassSetting;
    std::shared_ptr<AudioSetting<ProductPb::AudioTrebleLevel>> m_AudioTrebleSetting;
    std::shared_ptr<AudioSetting<ProductPb::AudioCenterLevel>> m_AudioCenterSetting;
    std::shared_ptr<AudioSetting<ProductPb::AudioSurroundLevel>> m_AudioSurroundSetting;
    std::shared_ptr<AudioSetting<ProductPb::AudioGainOffset>> m_AudioGainOffsetSetting;
    std::shared_ptr<AudioSetting<ProductPb::AudioAvSync>> m_AudioAvSyncsetSetting;
    std::shared_ptr<AudioSetting<ProductPb::AudioMode>> m_AudioModeSetting;
    std::shared_ptr<AudioSetting<ProductPb::AudioContentType>> m_AudioContentTypeSetting;

    /////////////////////////////////////////////////////////////////////////////////////////////////
    /// APProduct handling functions
    /////////////////////////////////////////////////////////////////////////////////////////////////
    void RegisterAudioPathEvent() override;
    void ConnectCallback( bool connect );
    void DisconnectCallback();

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// FrontDoor handling functions
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    void RegisterFrontDoorEvent() override;
};
}// namespace ProductApp
