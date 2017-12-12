///////////////////////////////////////////////////////////////////////////////////////////////////
/// @file   CustomAudioSettingsManager.cpp
/// @brief  This file contains source code for setting and getting AudioSettings
///         such as bass, treble, center, surround, gainOffset, avSync, mode, contentType
/// Copyright 2017 Bose Corporation
////////////////////////////////////////////////////////////////////////////////////////////////////
#include <json/reader.h>
#include <fstream>
#include "DPrint.h"
#include "SystemUtils.h"
#include "CustomAudioSettingsManager.h"

static DPrint s_logger( "CustomAudioSettingsManager" );

constexpr char kDefaultConfigPath[] = "/opt/Bose/etc/DefaultAudioSettings.json";

constexpr uint32_t kConfigVersionMajor = 2;
constexpr uint32_t kConfigVersionMinor = 1;

constexpr char kBassName            [] = "audioBassLevel";
constexpr char kTrebleName          [] = "audioTrebleLevel";

namespace ProductApp
{
using std::string;

CustomAudioSettingsManager::CustomAudioSettingsManager()
{
    BOSE_DEBUG( s_logger, __func__ );
    //Reading persistence
    InitializeAudioSettings();
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Bass setting setter/getter
/////////////////////////////////////////////////////////////////////////////////////////
bool CustomAudioSettingsManager::SetBass( ProductPb::AudioBassLevel bass )
{
    BOSE_DEBUG( s_logger, __func__ );
    bool retVal;
    retVal = SetAudioProperties( bass, kBassName );
    string persistLevel = GetCurrentPersistLevel(kBassName);
    BOSE_DEBUG( s_logger, "Sisi - persistenceLevel = %s\n", persistLevel.c_str() );
    BOSE_DEBUG( s_logger, "Sisi - value = %d\n", GetCurrentSettingValue(kBassName, persistLevel).asInt() );

    m_currentBass.set_persistence(persistLevel);
    m_currentBass.set_value(GetCurrentSettingValue(kBassName, persistLevel).asInt());
    return retVal;
}

const ProductPb::AudioBassLevel& CustomAudioSettingsManager::GetBass()
{
    BOSE_DEBUG( s_logger, __func__ );
    return m_currentBass;
}

////////////////////////////////////////////////////////////////////////////////////////
/// Treble setting setter/getter
///////////////////////////////////////////////////////////////////////////////////////
bool CustomAudioSettingsManager::SetTreble( ProductPb::AudioTrebleLevel treble )
{
    bool retVal;
    BOSE_DEBUG( s_logger, __func__ );
    retVal = SetAudioProperties( treble, kTrebleName );
    string persistLevel = GetCurrentPersistLevel(kTrebleName);
    m_currentTreble.set_persistence(persistLevel);
    m_currentTreble.set_value(GetCurrentSettingValue(kBassName, persistLevel).asInt());
    return retVal;
}

const ProductPb::AudioTrebleLevel& CustomAudioSettingsManager::GetTreble()
{
    BOSE_DEBUG( s_logger, __func__ );
    return m_currentTreble;
}

////////////////////////////////////////////////////////////////////////////////////////
/// Load default audio settings
///////////////////////////////////////////////////////////////////////////////////////
void CustomAudioSettingsManager::InitializeAudioSettings()
{
    BOSE_DEBUG( s_logger, __func__ );
    //TODO: read from persistence, if failed or if major version doesn't match then read from Default file
    //m_audioSettings
    Json::Reader reader;
    std::ifstream in( kDefaultConfigPath );
    auto const& defaultAudioSettings = SystemUtils::ReadFile( kDefaultConfigPath );
    if( !reader.parse( *defaultAudioSettings, m_audioSettings ) )
    {
        BOSE_INFO( s_logger, reader.getFormattedErrorMessages().c_str() );
        return;
    }
    else if( m_audioSettings["version"]["major"].asInt() != kConfigVersionMajor )
    {
        BOSE_INFO( s_logger, "DefaultAudioSettings.json has version %d.%d, whereas Professor expects version %d.%d",
                   m_audioSettings["version"]["major"].asInt(), m_audioSettings["version"]["minor"].asInt(),
                   kConfigVersionMajor, kConfigVersionMinor );
    }

    // Initialize m_currentXX protobufs, only if the configuration file has this field specified
    if( m_audioSettings["configurations"].isMember( kBassName ) )
    {
        m_currentBass.set_value( m_audioSettings["defaultValues"][kBassName].asInt() );
        m_currentBass.set_persistence( m_audioSettings["configurations"][kBassName]["currentPersistenceLevel"].asString() );
        m_currentBass.mutable_properties()->set_min( m_audioSettings["configurations"][kBassName]["properties"]["min"].asInt() );
        m_currentBass.mutable_properties()->set_max( m_audioSettings["configurations"][kBassName]["properties"]["max"].asInt() );
        m_currentBass.mutable_properties()->set_step( m_audioSettings["configurations"][kBassName]["properties"]["step"].asInt() );
        for( uint32_t i = 0; i < m_audioSettings["configurations"][kBassName]["properties"]["supportedPersistence"].size(); i++ )
        {
            m_currentBass.mutable_properties()->add_supportedpersistence( m_audioSettings["configurations"][kBassName]["properties"]["supportedPersistence"][i].asString() );
        }
    }

    if( m_audioSettings["configurations"].isMember( kTrebleName ) )
    {
        m_currentTreble.set_value( m_audioSettings["defaultValues"][kTrebleName].asInt() );
        m_currentTreble.set_persistence( m_audioSettings["configurations"][kTrebleName]["currentPersistenceLevel"].asString() );
        m_currentTreble.mutable_properties()->set_min( m_audioSettings["configurations"][kTrebleName]["properties"]["min"].asInt() );
        m_currentTreble.mutable_properties()->set_max( m_audioSettings["configurations"][kTrebleName]["properties"]["max"].asInt() );
        m_currentTreble.mutable_properties()->set_step( m_audioSettings["configurations"][kTrebleName]["properties"]["step"].asInt() );
        for( uint32_t i = 0; i < m_audioSettings["configurations"][kTrebleName]["properties"]["supportedPersistence"].size(); i++ )
        {
            m_currentTreble.mutable_properties()->add_supportedpersistence( m_audioSettings["configurations"][kTrebleName]["properties"]["supportedPersistence"][i].asString() );
        }
    }
}

}// namespace ProductApp
