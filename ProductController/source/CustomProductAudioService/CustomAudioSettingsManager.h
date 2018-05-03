////////////////////////////////////////////////////////////////////////////////
/// @file   CustomAudioSettingsManager.h
/// @brief  This file contains source code for setting and getting AudioSettings
///         such as bass, treble, center, surround, surroundDelay, gainOffset, avSync, subwooferGain, mode, contentType, dualMonoSelect
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
    ResultCode_t SetBass( const ProductPb::AudioBassLevel& bass );
    const ProductPb::AudioBassLevel& GetBass() const;
    ResultCode_t SetTreble( const ProductPb::AudioTrebleLevel& treble );
    const ProductPb::AudioTrebleLevel& GetTreble() const;
    ResultCode_t SetCenter( const ProductPb::AudioCenterLevel& center );
    const ProductPb::AudioCenterLevel& GetCenter() const;
    ResultCode_t SetSurround( const ProductPb::AudioSurroundLevel& surround );
    const ProductPb::AudioSurroundLevel& GetSurround() const;
    ResultCode_t SetSurroundDelay( const ProductPb::AudioSurroundDelay& surroundDelay );
    const ProductPb::AudioSurroundDelay& GetSurroundDelay() const;
    ResultCode_t SetGainOffset( const ProductPb::AudioGainOffset& gainOffset );
    const ProductPb::AudioGainOffset& GetGainOffset() const;
    ResultCode_t SetAvSync( const ProductPb::AudioAvSync& avSync );
    const ProductPb::AudioAvSync& GetAvSync() const;
    ResultCode_t SetSubwooferGain( const ProductPb::AudioSubwooferGain& subwooferGain );
    const ProductPb::AudioSubwooferGain& GetSubwooferGain() const;
    ResultCode_t SetMode( const ProductPb::AudioMode& mode );
    const ProductPb::AudioMode& GetMode() const;
    ResultCode_t SetContentType( const ProductPb::AudioContentType& contentType );
    const ProductPb::AudioContentType& GetContentType() const;
    ResultCode_t SetDualMonoSelect( const ProductPb::AudioDualMonoSelect& DualMonoSelect );
    const ProductPb::AudioDualMonoSelect& GetDualMonoSelect() const;
    ResultCode_t SetEqSelect( const ProductPb::AudioEqSelect& EqSelect );
    const ProductPb::AudioEqSelect& GetEqSelect() const;
    ResultCode_t SetSubwooferPolarity( const ProductPb::AudioSubwooferPolarity& subwooferPolarity );
    const ProductPb::AudioSubwooferPolarity& GetSubwooferPolarity() const;

private:
    ////////////////////////////////////////////////////////////////////////////////////////
    /// Protos containing current audio settings values
    //////////////////////////////////////////////////////////////////////////////////////
    ProductPb::AudioBassLevel m_currentBass;
    ProductPb::AudioTrebleLevel m_currentTreble;
    ProductPb::AudioCenterLevel m_currentCenter;
    ProductPb::AudioSurroundLevel m_currentSurround;
    ProductPb::AudioSurroundDelay m_currentSurroundDelay;
    ProductPb::AudioGainOffset m_currentGainOffset;
    ProductPb::AudioAvSync m_currentAvSync;
    ProductPb::AudioSubwooferGain m_currentSubwooferGain;
    ProductPb::AudioMode m_currentMode;
    ProductPb::AudioContentType m_currentContentType;
    ProductPb::AudioDualMonoSelect m_currentDualMonoSelect;
    ProductPb::AudioEqSelect m_currentEqSelect;
    ProductPb::AudioSubwooferPolarity m_currentSubwooferPolarity;

    ////////////////////////////////////////////////////////////////////////////////////////
    /// Helper functions to set contentItem sensitive audio settings
    //////////////////////////////////////////////////////////////////////////////////////
    void UpdateAllProtos() override;
    void InitializeAudioSettings();
};
}// namespace ProductApp

