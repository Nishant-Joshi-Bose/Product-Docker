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
/// Center setting setter/getter
///////////////////////////////////////////////////////////////////////////////////////
bool CustomAudioSettingsManager::SetCenter( const ProductPb::AudioCenterLevel& center )
{
    BOSE_DEBUG( s_logger, __func__ );
    return SetAudioProperties( center, kCenterName, m_currentCenter );
}

const ProductPb::AudioCenterLevel& CustomAudioSettingsManager::GetCenter() const
{
    BOSE_DEBUG( s_logger, __func__ );
    return m_currentCenter;
}

////////////////////////////////////////////////////////////////////////////////////////
/// Surround setting setter/getter
///////////////////////////////////////////////////////////////////////////////////////
bool CustomAudioSettingsManager::SetSurround( const ProductPb::AudioSurroundLevel& surround )
{
    BOSE_DEBUG( s_logger, __func__ );
    return SetAudioProperties( surround, kSurroundName, m_currentSurround );
}

const ProductPb::AudioSurroundLevel& CustomAudioSettingsManager::GetSurround() const
{
    BOSE_DEBUG( s_logger, __func__ );
    return m_currentSurround;
}

////////////////////////////////////////////////////////////////////////////////////////
/// GainOffset setting setter/getter
//////////////////////////////////////////////////////////////////////////////////////
bool CustomAudioSettingsManager::SetGainOffset( const ProductPb::AudioGainOffset& gainOffset )
{
    BOSE_DEBUG( s_logger, __func__ );
    return SetAudioProperties( gainOffset, kGainOffsetName, m_currentGainOffset );
}

const ProductPb::AudioGainOffset& CustomAudioSettingsManager::GetGainOffset() const
{
    BOSE_DEBUG( s_logger, __func__ );
    return m_currentGainOffset;
}

////////////////////////////////////////////////////////////////////////////////////////
/// AvSync setting setter/getter
//////////////////////////////////////////////////////////////////////////////////////
bool CustomAudioSettingsManager::SetAvSync( const ProductPb::AudioAvSync& avSync )
{
    BOSE_DEBUG( s_logger, __func__ );
    return SetAudioProperties( avSync, kAvSyncName, m_currentAvSync );
}

const ProductPb::AudioAvSync& CustomAudioSettingsManager::GetAvSync() const
{
    BOSE_DEBUG( s_logger, __func__ );
    return m_currentAvSync;
}

////////////////////////////////////////////////////////////////////////////////////////
/// Mode setting setter/getter
//////////////////////////////////////////////////////////////////////////////////////
bool CustomAudioSettingsManager::SetMode( const ProductPb::AudioMode& mode )
{
    BOSE_DEBUG( s_logger, __func__ );
    return SetAudioProperties( mode, kModeName, m_currentMode );
}
const ProductPb::AudioMode& CustomAudioSettingsManager::GetMode() const
{
    BOSE_DEBUG( s_logger, __func__ );
    return m_currentMode;
}

////////////////////////////////////////////////////////////////////////////////////////
/// ContentType setting setter/getter
//////////////////////////////////////////////////////////////////////////////////////
bool CustomAudioSettingsManager::SetContentType( const ProductPb::AudioContentType& contentType )
{
    BOSE_DEBUG( s_logger, __func__ );
    return SetAudioProperties( contentType, kContentTypeName, m_currentContentType );
}
const ProductPb::AudioContentType& CustomAudioSettingsManager::GetContentType() const
{
    BOSE_DEBUG( s_logger, __func__ );
    return m_currentContentType;
}

////////////////////////////////////////////////////////////////////////////////////////
/// DualMonoSelect setting setter/getter
//////////////////////////////////////////////////////////////////////////////////////
bool CustomAudioSettingsManager::SetDualMonoSelect( const ProductPb::AudioDualMonoSelect& DualMonoSelect )
{
    BOSE_DEBUG( s_logger, __func__ );
    if( !DualMonoSelect.has_value() )
    {
        BOSE_INFO( s_logger, "DualMonoSelect doesn't contain any value" );
        return false;
    }
    if( m_currentDualMonoSelect.value() != DualMonoSelect.value() )
    {
        m_audioSettings["values"][kPersistGlobal][kDualMonoSelectName] = DualMonoSelect.value();
        m_currentDualMonoSelect.set_value( DualMonoSelect.value() );
        PersistAudioSettings();
        return true;
    }
    return false;
}
const ProductPb::AudioDualMonoSelect& CustomAudioSettingsManager::GetDualMonoSelect() const
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
    initializeProto( kCenterName, m_currentCenter );
    initializeProto( kSurroundName, m_currentSurround );
    initializeProto( kGainOffsetName, m_currentGainOffset );
    initializeProto( kAvSyncName, m_currentAvSync );

    m_audioSettings["configurations"][kModeName]["persistenceSession"] = false;
    std::string currPersistLvlMode = m_audioSettings["configurations"][kModeName]["currentPersistenceLevel"].asString();
    m_currentMode.set_persistence( currPersistLvlMode );
    if( currPersistLvlMode == kPersistContentItem )
    {
        m_currentMode.set_value( m_audioSettings["values"][currPersistLvlMode][m_currentContentItem][kModeName].asString() );
    }
    else
    {
        m_currentMode.set_value( m_audioSettings["values"][currPersistLvlMode][kModeName].asString() );
    }
    for( uint32_t i = 0; i < m_audioSettings["configurations"][kModeName]["properties"]["supportedValues"].size(); i++ )
    {
        m_currentMode.mutable_properties()->add_supportedvalues( m_audioSettings["configurations"][kModeName]["properties"]["supportedValues"][i].asString() );
    }
    for( uint32_t i = 0; i < m_audioSettings["configurations"][kModeName]["properties"]["supportedPersistence"].size(); i++ )
    {
        m_currentMode.mutable_properties()->add_supportedpersistence( m_audioSettings["configurations"][kModeName]["properties"]["supportedPersistence"][i].asString() );
    }

    m_audioSettings["configurations"][kContentTypeName]["persistenceSession"] = false;
    std::string currPersistLvlContentType = m_audioSettings["configurations"][kContentTypeName]["currentPersistenceLevel"].asString();
    m_currentContentType.set_persistence( currPersistLvlContentType );
    if( currPersistLvlContentType == kPersistContentItem )
    {
        m_currentContentType.set_value( m_audioSettings["values"][currPersistLvlContentType][m_currentContentItem][kContentTypeName].asString() );
    }
    else
    {
        m_currentContentType.set_value( m_audioSettings["values"][currPersistLvlContentType][kContentTypeName].asString() );
    }
    for( uint32_t i = 0; i < m_audioSettings["configurations"][kContentTypeName]["properties"]["supportedValues"].size(); i++ )
    {
        m_currentContentType.mutable_properties()->add_supportedvalues( m_audioSettings["configurations"][kContentTypeName]["properties"]["supportedValues"][i].asString() );
    }
    for( uint32_t i = 0; i < m_audioSettings["configurations"][kContentTypeName]["properties"]["supportedPersistence"].size(); i++ )
    {
        m_currentContentType.mutable_properties()->add_supportedpersistence( m_audioSettings["configurations"][kContentTypeName]["properties"]["supportedPersistence"][i].asString() );
    }

    m_currentDualMonoSelect.set_value( m_audioSettings["defaultValues"][kDualMonoSelectName].asString() );
    for( uint32_t i = 0; i < m_audioSettings["configurations"][kDualMonoSelectName]["properties"]["supportedValues"].size(); i++ )
    {
        m_currentDualMonoSelect.mutable_properties()->add_supportedvalues( m_audioSettings["configurations"][kDualMonoSelectName]["properties"]["supportedValues"][i].asString() );
    }
}

}// namespace ProductApp
