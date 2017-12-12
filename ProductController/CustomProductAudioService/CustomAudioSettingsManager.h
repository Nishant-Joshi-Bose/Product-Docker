////////////////////////////////////////////////////////////////////////////////
/// @file   CustomAudioSettingsManager.h
/// @brief  This file contains source code for setting and getting AudioSettings
///         such as bass, treble, center, surround, gainOffset, avSync, mode, contentType
/// Copyright 2017 Bose Corporation
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <json/value.h>
#include "AudioSettingsManager.h"
#include "AudioSettings.pb.h"
#include "SoundTouchInterface/ContentItem.pb.h"
#include "ProductMessage.pb.h"

namespace ProductApp
{
using std::string;

class CustomAudioSettingsManager:AudioSettingsManager
{
public:
    CustomAudioSettingsManager();
    bool SetBass( ProductPb::AudioBassLevel bass );
    const ProductPb::AudioBassLevel& GetBass();
    bool SetTreble( ProductPb::AudioTrebleLevel treble );
    const ProductPb::AudioTrebleLevel& GetTreble();

private:
    ProductPb::AudioBassLevel m_currentBass;
    ProductPb::AudioTrebleLevel m_currentTreble;

    void InitializeAudioSettings();

};
}// namespace ProductApp

