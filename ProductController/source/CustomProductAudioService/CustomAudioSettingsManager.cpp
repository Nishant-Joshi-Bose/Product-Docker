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

constexpr char  kDefaultConfigPath[] = "/opt/Bose/etc/DefaultAudioSettings.json";
constexpr uint32_t kConfigVersionMajor = 2;
constexpr uint32_t kConfigVersionMinor = 1;

constexpr char kBassName            [] = "audioBassLevel";
constexpr char kTrebleName          [] = "audioTrebleLevel";
constexpr char kCenterName          [] = "audioCenterLevel";
constexpr char kSurroundName        [] = "audioSurroundLevel";
constexpr char kGainOffsetName      [] = "audioGainOffset";
constexpr char kAvSyncName          [] = "audioAvSync";
constexpr char kModeName            [] = "audioMode";
constexpr char kContentTypeName     [] = "audioContentType";
constexpr char kDualMonoSelectName  [] = "audioDualMonoSelect";

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
    return SetAudioProperties( bass, kBassName, m_currentBass );
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
    BOSE_DEBUG( s_logger, __func__ );
    return SetAudioProperties( treble, kTrebleName, m_currentTreble );
}

const ProductPb::AudioTrebleLevel& CustomAudioSettingsManager::GetTreble()
{
    BOSE_DEBUG( s_logger, __func__ );
    return m_currentTreble;
}

////////////////////////////////////////////////////////////////////////////////////////
/// Center setting setter/getter
///////////////////////////////////////////////////////////////////////////////////////
bool CustomAudioSettingsManager::SetCenter( ProductPb::AudioCenterLevel center )
{
    BOSE_DEBUG( s_logger, __func__ );
    return SetAudioProperties( center, kCenterName, m_currentCenter );
}

const ProductPb::AudioCenterLevel& CustomAudioSettingsManager::GetCenter()
{
    BOSE_DEBUG( s_logger, __func__ );
    return m_currentCenter;
}

////////////////////////////////////////////////////////////////////////////////////////
/// Center setting setter/getter
///////////////////////////////////////////////////////////////////////////////////////
bool CustomAudioSettingsManager::SetSurround( ProductPb::AudioSurroundLevel surround )
{
    BOSE_DEBUG( s_logger, __func__ );
    return SetAudioProperties( surround, kSurroundName, m_currentSurround );
}

const ProductPb::AudioSurroundLevel& CustomAudioSettingsManager::GetSurround()
{
    BOSE_DEBUG( s_logger, __func__ );
    return m_currentSurround;
}

////////////////////////////////////////////////////////////////////////////////////////
/// Center setting setter/getter
//////////////////////////////////////////////////////////////////////////////////////
bool CustomAudioSettingsManager::SetGainOffset( ProductPb::AudioGainOffset gainOffset )
{
    BOSE_DEBUG( s_logger, __func__ );
    return SetAudioProperties( gainOffset, kGainOffsetName, m_currentGainOffset );
}

const ProductPb::AudioGainOffset& CustomAudioSettingsManager::GetGainOffset()
{
    BOSE_DEBUG( s_logger, __func__ );
    return m_currentGainOffset;
}

////////////////////////////////////////////////////////////////////////////////////////
/// AvSync setting setter/getter
//////////////////////////////////////////////////////////////////////////////////////
bool CustomAudioSettingsManager::SetAvSync( ProductPb::AudioAvSync avSync )
{
    BOSE_DEBUG( s_logger, __func__ );
    return SetAudioProperties( avSync, kAvSyncName, m_currentAvSync );
}
const ProductPb::AudioAvSync& CustomAudioSettingsManager::GetAvSync()
{
    BOSE_DEBUG( s_logger, __func__ );
    return m_currentAvSync;
}

////////////////////////////////////////////////////////////////////////////////////////
/// Mode setting setter/getter
//////////////////////////////////////////////////////////////////////////////////////
bool CustomAudioSettingsManager::SetMode( ProductPb::AudioMode mode )
{
    BOSE_DEBUG( s_logger, __func__ );
    return SetAudioProperties( mode, kModeName, m_currentMode );
}
const ProductPb::AudioMode& CustomAudioSettingsManager::GetMode()
{
    BOSE_DEBUG( s_logger, __func__ );
    return m_currentMode;
}

////////////////////////////////////////////////////////////////////////////////////////
/// ContentType setting setter/getter
//////////////////////////////////////////////////////////////////////////////////////
bool CustomAudioSettingsManager::SetContentType( ProductPb::AudioContentType contentType )
{
    BOSE_DEBUG( s_logger, __func__ );
    return SetAudioProperties( contentType, kContentTypeName, m_currentContentType );
}
const ProductPb::AudioContentType& CustomAudioSettingsManager::GetContentType()
{
    BOSE_DEBUG( s_logger, __func__ );
    return m_currentContentType;
}

////////////////////////////////////////////////////////////////////////////////////////
/// DualMonoSelect setting setter/getter
//////////////////////////////////////////////////////////////////////////////////////
bool CustomAudioSettingsManager::SetDualMonoSelect( ProductPb::AudioDualMonoSelect DualMonoSelect )
{
    BOSE_DEBUG( s_logger, __func__ );
    if( !DualMonoSelect.has_value() )
    {
        BOSE_INFO( s_logger, "DualMonoSelect doesn't contain any value" );
        return false;
    }
    m_audioSettings["values"][kPersistGlobal][kDualMonoSelectName] = DualMonoSelect.value();
    if( m_currentDualMonoSelect.value() != DualMonoSelect.value() )
    {
        m_currentDualMonoSelect.set_value( DualMonoSelect.value() );
        return true;
    }
    return false;
}
const ProductPb::AudioDualMonoSelect& CustomAudioSettingsManager::GetDualMonoSelect()
{
    BOSE_DEBUG( s_logger, __func__ );
    return m_currentDualMonoSelect;
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

    if( m_audioSettings["configurations"].isMember( kCenterName ) )
    {
        m_currentCenter.set_value( m_audioSettings["defaultValues"][kCenterName].asInt() );
        m_currentCenter.set_persistence( m_audioSettings["configurations"][kCenterName]["currentPersistenceLevel"].asString() );
        m_currentCenter.mutable_properties()->set_min( m_audioSettings["configurations"][kCenterName]["properties"]["min"].asInt() );
        m_currentCenter.mutable_properties()->set_max( m_audioSettings["configurations"][kCenterName]["properties"]["max"].asInt() );
        m_currentCenter.mutable_properties()->set_step( m_audioSettings["configurations"][kCenterName]["properties"]["step"].asInt() );
        for( uint32_t i = 0; i < m_audioSettings["configurations"][kCenterName]["properties"]["supportedPersistence"].size(); i++ )
        {
            m_currentCenter.mutable_properties()->add_supportedpersistence( m_audioSettings["configurations"][kCenterName]["properties"]["supportedPersistence"][i].asString() );
        }
    }

    if( m_audioSettings["configurations"].isMember( kSurroundName ) )
    {
        m_currentSurround.set_value( m_audioSettings["defaultValues"][kSurroundName].asInt() );
        m_currentSurround.set_persistence( m_audioSettings["configurations"][kSurroundName]["currentPersistenceLevel"].asString() );
        m_currentSurround.mutable_properties()->set_min( m_audioSettings["configurations"][kSurroundName]["properties"]["min"].asInt() );
        m_currentSurround.mutable_properties()->set_max( m_audioSettings["configurations"][kSurroundName]["properties"]["max"].asInt() );
        m_currentSurround.mutable_properties()->set_step( m_audioSettings["configurations"][kSurroundName]["properties"]["step"].asInt() );
        for( uint32_t i = 0; i < m_audioSettings["configurations"][kSurroundName]["properties"]["supportedPersistence"].size(); i++ )
        {
            m_currentSurround.mutable_properties()->add_supportedpersistence( m_audioSettings["configurations"][kSurroundName]["properties"]["supportedPersistence"][i].asString() );
        }
    }

    if( m_audioSettings["configurations"].isMember( kGainOffsetName ) )
    {
        m_currentGainOffset.set_value( m_audioSettings["defaultValues"][kGainOffsetName].asInt() );
        m_currentGainOffset.set_persistence( m_audioSettings["configurations"][kGainOffsetName]["currentPersistenceLevel"].asString() );
        m_currentGainOffset.mutable_properties()->set_min( m_audioSettings["configurations"][kGainOffsetName]["properties"]["min"].asInt() );
        m_currentGainOffset.mutable_properties()->set_max( m_audioSettings["configurations"][kGainOffsetName]["properties"]["max"].asInt() );
        m_currentGainOffset.mutable_properties()->set_step( m_audioSettings["configurations"][kGainOffsetName]["properties"]["step"].asInt() );
        for( uint32_t i = 0; i < m_audioSettings["configurations"][kGainOffsetName]["properties"]["supportedPersistence"].size(); i++ )
        {
            m_currentGainOffset.mutable_properties()->add_supportedpersistence( m_audioSettings["configurations"][kGainOffsetName]["properties"]["supportedPersistence"][i].asString() );
        }
    }

    if( m_audioSettings["configurations"].isMember( kAvSyncName ) )
    {
        m_currentAvSync.set_value( m_audioSettings["defaultValues"][kAvSyncName].asInt() );
        m_currentAvSync.set_persistence( m_audioSettings["configurations"][kAvSyncName]["currentPersistenceLevel"].asString() );
        m_currentAvSync.mutable_properties()->set_min( m_audioSettings["configurations"][kAvSyncName]["properties"]["min"].asInt() );
        m_currentAvSync.mutable_properties()->set_max( m_audioSettings["configurations"][kAvSyncName]["properties"]["max"].asInt() );
        m_currentAvSync.mutable_properties()->set_step( m_audioSettings["configurations"][kAvSyncName]["properties"]["step"].asInt() );
        for( uint32_t i = 0; i < m_audioSettings["configurations"][kAvSyncName]["properties"]["supportedPersistence"].size(); i++ )
        {
            m_currentAvSync.mutable_properties()->add_supportedpersistence( m_audioSettings["configurations"][kAvSyncName]["properties"]["supportedPersistence"][i].asString() );
        }
    }

    if( m_audioSettings["configurations"].isMember( kModeName ) )
    {
        m_currentMode.set_value( m_audioSettings["defaultValues"][kModeName].asString() );
        m_currentMode.set_persistence( m_audioSettings["configurations"][kModeName]["currentPersistenceLevel"].asString() );
        for( uint32_t i = 0; i < m_audioSettings["configurations"][kModeName]["properties"]["supportedValues"].size(); i++ )
        {
            m_currentMode.mutable_properties()->add_supportedvalues( m_audioSettings["configurations"][kModeName]["properties"]["supportedValues"][i].asString() );
        }
        for( uint32_t i = 0; i < m_audioSettings["configurations"][kModeName]["properties"]["supportedPersistence"].size(); i++ )
        {
            m_currentMode.mutable_properties()->add_supportedpersistence( m_audioSettings["configurations"][kModeName]["properties"]["supportedPersistence"][i].asString() );
        }
    }

    if( m_audioSettings["configurations"].isMember( kContentTypeName ) )
    {
        m_currentContentType.set_value( m_audioSettings["defaultValues"][kContentTypeName].asString() );
        m_currentContentType.set_persistence( m_audioSettings["configurations"][kContentTypeName]["currentPersistenceLevel"].asString() );
        for( uint32_t i = 0; i < m_audioSettings["configurations"][kContentTypeName]["properties"]["supportedValues"].size(); i++ )
        {
            m_currentContentType.mutable_properties()->add_supportedvalues( m_audioSettings["configurations"][kContentTypeName]["properties"]["supportedValues"][i].asString() );
        }
        for( uint32_t i = 0; i < m_audioSettings["configurations"][kContentTypeName]["properties"]["supportedPersistence"].size(); i++ )
        {
            m_currentContentType.mutable_properties()->add_supportedpersistence( m_audioSettings["configurations"][kContentTypeName]["properties"]["supportedPersistence"][i].asString() );
        }
    }

    if( m_audioSettings["configurations"].isMember( kDualMonoSelectName ) )
    {
        m_currentDualMonoSelect.set_value( m_audioSettings["defaultValues"][kDualMonoSelectName].asString() );
        for( uint32_t i = 0; i < m_audioSettings["configurations"][kDualMonoSelectName]["properties"]["supportedValues"].size(); i++ )
        {
            m_currentDualMonoSelect.mutable_properties()->add_supportedvalues( m_audioSettings["configurations"][kDualMonoSelectName]["properties"]["supportedValues"][i].asString() );
        }
    }
}

}// namespace ProductApp
