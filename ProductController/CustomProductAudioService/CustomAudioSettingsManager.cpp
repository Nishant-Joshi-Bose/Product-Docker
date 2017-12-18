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
    initializeProto( kBassName, m_currentBass );
    initializeProto( kTrebleName, m_currentTreble );
}

template<typename ProtoBuf>
void CustomAudioSettingsManager::initializeProto( string propName, ProtoBuf& proto )
{
    proto.set_value( m_audioSettings["defaultValues"][propName].asInt() );
    proto.set_persistence( m_audioSettings["configurations"][propName]["currentPersistenceLevel"].asString() );
    proto.mutable_properties()->set_min( m_audioSettings["configurations"][propName]["properties"]["min"].asInt() );
    proto.mutable_properties()->set_max( m_audioSettings["configurations"][propName]["properties"]["max"].asInt() );
    proto.mutable_properties()->set_step( m_audioSettings["configurations"][propName]["properties"]["step"].asInt() );
    for( uint32_t i = 0; i < m_audioSettings["configurations"][propName]["properties"]["supportedPersistence"].size(); i++ )
    {
        proto.mutable_properties()->add_supportedpersistence( m_audioSettings["configurations"][propName]["properties"]["supportedPersistence"][i].asString() );
    }
}

}// namespace ProductApp
