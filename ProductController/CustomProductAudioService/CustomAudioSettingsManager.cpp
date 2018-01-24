///////////////////////////////////////////////////////////////////////////////////////////////////
/// @file   CustomAudioSettingsManager.cpp
/// @brief  This file contains source code for setting and getting AudioSettings
///         such as bass, treble
/// Copyright 2017 Bose Corporation
////////////////////////////////////////////////////////////////////////////////////////////////////
#include <json/reader.h>
#include <fstream>
#include "DPrint.h"
#include "SystemUtils.h"
#include "CustomAudioSettingsManager.h"

static DPrint s_logger( "CustomAudioSettingsManager" );

constexpr char  kDefaultConfigPath[] = "/opt/Bose/etc/DefaultAudioSettings.json";
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
    InitializeAudioSettings();
}

void CustomAudioSettingsManager::UpdateAllProtos()
{
    BOSE_DEBUG( s_logger, __func__ );
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Bass setting setter/getter
/////////////////////////////////////////////////////////////////////////////////////////
bool CustomAudioSettingsManager::SetBass( const ProductPb::AudioBassLevel& bass )
{
    BOSE_DEBUG( s_logger, __func__ );
    return SetAudioProperties( bass, kBassName, m_currentBass );
}

const ProductPb::AudioBassLevel& CustomAudioSettingsManager::GetBass() const
{
    BOSE_DEBUG( s_logger, __func__ );
    return m_currentBass;
}

////////////////////////////////////////////////////////////////////////////////////////
/// Treble setting setter/getter
///////////////////////////////////////////////////////////////////////////////////////
bool CustomAudioSettingsManager::SetTreble( const ProductPb::AudioTrebleLevel& treble )
{
    BOSE_DEBUG( s_logger, __func__ );
    return SetAudioProperties( treble, kTrebleName, m_currentTreble );
}

const ProductPb::AudioTrebleLevel& CustomAudioSettingsManager::GetTreble() const
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
    bool success = true; //successful reading from persistence, initialized to be true
    Json::Reader reader;
    try
    {
        std::string s = m_audioSettingsPersistence->Load();
        success = reader.parse( s, m_audioSettings );
        if( !success )
        {
            BOSE_DEBUG( s_logger, reader.getFormattedErrorMessages().c_str() );
        }
        else if( m_audioSettings.empty()
                 || ( !m_audioSettings.isMember( "configurations" ) )
                 || ( !m_audioSettings.isMember( "values" ) ) )
        {
            success = false;
            BOSE_ERROR( s_logger, "Persisted m_audioSettings doesn't contain valid data" );
        }
    }
    catch( ProtoPersistenceIF::ProtoPersistenceException& e )
    {
        success = false;
        BOSE_ERROR( s_logger, "Loading audioSettings from persistence failed - %s ", e.what() );
    }

    // If reading from persistence failed, read from default configuration file
    if( !success )
    {
        BOSE_DEBUG( s_logger, "Reading audio settings from persistence failed, let's read from default config file" );
        std::ifstream in( kDefaultConfigPath );
        auto const& defaultAudioSettings = SystemUtils::ReadFile( kDefaultConfigPath );
        if( !reader.parse( *defaultAudioSettings, m_audioSettings ) )
        {
            // If reading from default configuration file failed, there's something majorly wrong, have to return
            BOSE_ERROR( s_logger, "Reading from default config file also failed with error %s", reader.getFormattedErrorMessages().c_str() );
            return;
        }
        else if( m_audioSettings["version"]["major"].asInt() != kConfigVersionMajor )
        {
            // major version means major format or structure change in audio settings JSON
            // If the major version doesn't match between configuration file and code, there's mismatch during build procedure
            // force loading it will cause unknown error, should report error and stop
            BOSE_ERROR( s_logger, "Reading from default config file also failed. DefaultAudioSettings.json has version %d.%d, and Professor expects version %d.%d, check your build system",
                        m_audioSettings["version"]["major"].asInt(), m_audioSettings["version"]["minor"].asInt(),
                        kConfigVersionMajor, kConfigVersionMinor );
            return;
        }
    }

    // If it gets here, it means we successfully read from either persistence or default config file
    // Initialize ProtoBufs with m_audioSettings JSON values
    BOSE_DEBUG( s_logger, "Initialize current protos with m_audioSettings" );
    initializeProto( kBassName, m_currentBass );
    initializeProto( kTrebleName, m_currentTreble );
}

}// namespace ProductApp
