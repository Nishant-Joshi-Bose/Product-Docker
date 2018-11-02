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

constexpr char kBassName                [] = "audioBassLevel";
constexpr char kTrebleName              [] = "audioTrebleLevel";
constexpr char kCenterName              [] = "audioCenterLevel";
constexpr char kSurroundName            [] = "audioSurroundLevel";
constexpr char kSurroundDelayName       [] = "audioSurroundDelay";
constexpr char kGainOffsetName          [] = "audioGainOffset";
constexpr char kAvSyncName              [] = "audioAvSync";
constexpr char kSubwooferGainName       [] = "audioSubwooferGain";
constexpr char kModeName                [] = "audioMode";
constexpr char kContentTypeName         [] = "audioContentType";
constexpr char kDualMonoSelectName      [] = "audioDualMonoSelect";
constexpr char kEqSelectName            [] = "audioEqSelect";
constexpr char kSubwooferPolarityName   [] = "audioSubwooferPolarity";

using std::string;

class CustomAudioSettingsManager: public AudioSettingsManager
{
public:
    CustomAudioSettingsManager();
    AudioSettingResultCode::ResultCode_t SetBass( const ProductPb::AudioBassLevel& bass );
    const ProductPb::AudioBassLevel& GetBass() const;
    void RefreshBass();

    AudioSettingResultCode::ResultCode_t SetTreble( const ProductPb::AudioTrebleLevel& treble );
    const ProductPb::AudioTrebleLevel& GetTreble() const;
    void RefreshTreble();

    AudioSettingResultCode::ResultCode_t SetCenter( const ProductPb::AudioCenterLevel& center );
    const ProductPb::AudioCenterLevel& GetCenter() const;
    void RefreshCenter();

    AudioSettingResultCode::ResultCode_t SetSurround( const ProductPb::AudioSurroundLevel& surround );
    const ProductPb::AudioSurroundLevel& GetSurround() const;
    void RefreshSurround();

    AudioSettingResultCode::ResultCode_t SetSurroundDelay( const ProductPb::AudioSurroundDelay& surroundDelay );
    const ProductPb::AudioSurroundDelay& GetSurroundDelay() const;
    void RefreshSurroundDelay();

    AudioSettingResultCode::ResultCode_t SetGainOffset( const ProductPb::AudioGainOffset& gainOffset );
    const ProductPb::AudioGainOffset& GetGainOffset() const;
    void RefreshGainOffset();

    AudioSettingResultCode::ResultCode_t SetAvSync( const ProductPb::AudioAvSync& avSync );
    const ProductPb::AudioAvSync& GetAvSync() const;
    void RefreshAvSync();

    AudioSettingResultCode::ResultCode_t SetSubwooferGain( const ProductPb::AudioSubwooferGain& subwooferGain );
    const ProductPb::AudioSubwooferGain& GetSubwooferGain() const;
    void RefreshSubwooferGain();

    AudioSettingResultCode::ResultCode_t SetMode( ProductPb::AudioMode& mode );
    const ProductPb::AudioMode& GetMode() const;
    void RefreshMode();

    AudioSettingResultCode::ResultCode_t SetContentType( const ProductPb::AudioContentType& contentType );
    const ProductPb::AudioContentType& GetContentType() const;
    void RefreshContentType();

    AudioSettingResultCode::ResultCode_t SetDualMonoSelect( const ProductPb::AudioDualMonoSelect& DualMonoSelect );
    const ProductPb::AudioDualMonoSelect& GetDualMonoSelect() const;
    void RefreshDualMonoSelect();

    AudioSettingResultCode::ResultCode_t SetEqSelect( const ProductPb::AudioEqSelect& EqSelect );
    const ProductPb::AudioEqSelect& GetEqSelect() const;
    AudioSettingResultCode::ResultCode_t UpdateEqSelectSupportedMode( string mode, bool supported );

    AudioSettingResultCode::ResultCode_t SetSubwooferPolarity( const ProductPb::AudioSubwooferPolarity& subwooferPolarity );
    const ProductPb::AudioSubwooferPolarity& GetSubwooferPolarity() const;

    void UpdateAllProtos() override;

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
    void InitializeAudioSettings();
};
}// namespace ProductApp

