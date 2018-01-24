////////////////////////////////////////////////////////////////////////////////
/// @file   CustomAudioSettingsManager.h
/// @brief  This file contains source code for setting and getting AudioSettings
///         such as bass, treble
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
    ~CustomAudioSettingsManager() override {}

    bool SetBass( const ProductPb::AudioBassLevel& bass );
    const ProductPb::AudioBassLevel& GetBass() const;
    bool SetTreble( const ProductPb::AudioTrebleLevel& treble );
    const ProductPb::AudioTrebleLevel& GetTreble() const;

private:
    ProductPb::AudioBassLevel m_currentBass;
    ProductPb::AudioTrebleLevel m_currentTreble;

    ////////////////////////////////////////////////////////////////////////////////////////
    /// Helper functions to set contentItem sensitive audio settings
    //////////////////////////////////////////////////////////////////////////////////////
    void UpdateAllProtos() override;
    void InitializeAudioSettings();
};
}// namespace ProductApp

