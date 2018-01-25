////////////////////////////////////////////////////////////////////////////////
/// @file   CustomAudioSettingsManager.h
/// @brief  This file contains source code for setting and getting AudioSettings
///         such as bass, treble, center, surround, gainOffset, avSync, mode, contentType
/// Copyright 2017 Bose Corporation
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <json/value.h>
#include <json/reader.h>
#include "AudioSettingsManager.h"
#include "AudioSettings.pb.h"

namespace ProductApp
{
using std::string;

class CustomAudioSettingsManager: public AudioSettingsManager
{
public:
    CustomAudioSettingsManager();
    bool SetBass( const ProductPb::AudioBassLevel& bass );
    const ProductPb::AudioBassLevel& GetBass() const;
    bool SetTreble( const ProductPb::AudioTrebleLevel& treble );
    const ProductPb::AudioTrebleLevel& GetTreble() const;
    bool SetCenter( const ProductPb::AudioCenterLevel& center );
    const ProductPb::AudioCenterLevel& GetCenter() const;
    bool SetSurround( const ProductPb::AudioSurroundLevel& surround );
    const ProductPb::AudioSurroundLevel& GetSurround() const;
    bool SetGainOffset( const ProductPb::AudioGainOffset& gainOffset );
    const ProductPb::AudioGainOffset& GetGainOffset() const;
    bool SetAvSync( const ProductPb::AudioAvSync& avSync );
    const ProductPb::AudioAvSync& GetAvSync() const;
    bool SetMode( const ProductPb::AudioMode& mode );
    const ProductPb::AudioMode& GetMode() const;
    bool SetContentType( const ProductPb::AudioContentType& contentType );
    const ProductPb::AudioContentType& GetContentType() const;
    bool SetDualMonoSelect( const ProductPb::AudioDualMonoSelect& DualMonoSelect );
    const ProductPb::AudioDualMonoSelect& GetDualMonoSelect() const;

private:
    ////////////////////////////////////////////////////////////////////////////////////////
    /// Protos containing current audio settings values
    //////////////////////////////////////////////////////////////////////////////////////
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
    void UpdateAllProtos() override;
    void InitializeAudioSettings();
};
}// namespace ProductApp

