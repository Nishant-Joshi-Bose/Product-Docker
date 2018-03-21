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

class CustomAudioSettingsManager: public AudioSettingsManager
{
public:
    CustomAudioSettingsManager();
    ~CustomAudioSettingsManager() override {}

    ErrorCode_t SetBass( const ProductPb::AudioBassLevel& bass );
    const ProductPb::AudioBassLevel& GetBass() const;

    ErrorCode_t SetCenter( const ProductPb::AudioCenterLevel& center );
    const ProductPb::AudioCenterLevel& GetCenter() const;
    ErrorCode_t SetMode( const ProductPb::AudioMode& mode );
    const ProductPb::AudioMode& GetMode() const;
    ErrorCode_t SetTreble( const ProductPb::AudioTrebleLevel& treble );
    const ProductPb::AudioTrebleLevel& GetTreble() const;

    /*! \brief Returns state of testing flag for "center level" feature.
     */
    bool IsCenterLevelTestEnabled() const
    {
        return m_centerLevelTestEnabled;
    }

    /*! \brief Returns state of testing flag for "dialog mode" feature.
     */
    bool IsModeTestEnabled() const
    {
        return m_modeTestEnabled;
    }

private:

    void UpdateAllProtos() override;
    void InitializeAudioSettings();

    /*! \brief Attempts to read "test options" from the configuration file.
     * If no test options are present, that's OK.
     */
    void LoadTestOptions();

    ProductPb::AudioBassLevel m_currentBass;
    ProductPb::AudioCenterLevel m_currentCenter;
    ProductPb::AudioMode m_currentMode;
    ProductPb::AudioTrebleLevel m_currentTreble;

    bool m_centerLevelTestEnabled;                  //!< Testing flag for "center level".
    bool m_modeTestEnabled;                         //!< Testing flag for "mode".

};
}// namespace ProductApp

