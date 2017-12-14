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

namespace ProductApp
{
using std::string;

class CustomAudioSettingsManager: AudioSettingsManager
{
public:
    CustomAudioSettingsManager();
    bool SetBass( ProductPb::AudioBassLevel bass );
    ProductPb::AudioBassLevel GetBass() const;
    bool SetTreble( ProductPb::AudioTrebleLevel treble );
    ProductPb::AudioTrebleLevel GetTreble() const;
    bool SetCenter( ProductPb::AudioCenterLevel center );
    ProductPb::AudioCenterLevel GetCenter() const;
    bool SetSurround( ProductPb::AudioSurroundLevel surround );
    ProductPb::AudioSurroundLevel GetSurround() const;
    bool SetGainOffset( ProductPb::AudioGainOffset gainOffset );
    ProductPb::AudioGainOffset GetGainOffset() const;
    bool SetAvSync( ProductPb::AudioAvSync avSync );
    ProductPb::AudioAvSync GetAvSync() const;
    bool SetMode( ProductPb::AudioMode mode );
    ProductPb::AudioMode GetMode() const;
    bool SetContentType( ProductPb::AudioContentType contentType );
    ProductPb::AudioContentType GetContentType() const;
    bool SetDualMonoSelect( ProductPb::AudioDualMonoSelect DualMonoSelect );
    ProductPb::AudioDualMonoSelect GetDualMonoSelect() const;

private:
    ProductPb::AudioBassLevel m_currentBass;
    ProductPb::AudioTrebleLevel m_currentTreble;
    ProductPb::AudioCenterLevel m_currentCenter;
    ProductPb::AudioSurroundLevel m_currentSurround;
    ProductPb::AudioGainOffset m_currentGainOffset;
    ProductPb::AudioAvSync m_currentAvSync;
    ProductPb::AudioMode m_currentMode;
    ProductPb::AudioContentType m_currentContentType;
    ProductPb::AudioDualMonoSelect m_currentDualMonoSelect;

    ////////////////////////////////////////////////////////////////////////////////////////
    /// Helper functions to set contentItem sensitive audio settings
    //////////////////////////////////////////////////////////////////////////////////////
    void InitializeAudioSettings();
};
}// namespace ProductApp

