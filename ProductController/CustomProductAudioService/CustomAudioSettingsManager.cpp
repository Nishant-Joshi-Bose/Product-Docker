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

constexpr char kBassName                [] = "audioBassLevel";
constexpr char kCenterName              [] = "audioCenterLevel";
constexpr char kModeName                [] = "audioMode";
constexpr char kTrebleName              [] = "audioTrebleLevel";

// Some flags used to enable on configure testing features.
constexpr char kTestOptionsName         [] = "testOptions";
constexpr char kTestCenterEnabledName   [] = "centerEnabled";
constexpr char kTestModeEnabledName     [] = "modeEnabled";

namespace ProductApp
{
using std::string;

CustomAudioSettingsManager::CustomAudioSettingsManager() :
    AudioSettingsManager(),
    m_centerLevelTestEnabled( false ),
    m_modeTestEnabled( false )
{
    BOSE_DEBUG( s_logger, __func__ );
    InitializeAudioSettings();
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Bass setting setter/getter
/////////////////////////////////////////////////////////////////////////////////////////
ErrorCode_t CustomAudioSettingsManager::SetBass( const ProductPb::AudioBassLevel& bass )
{
    BOSE_DEBUG( s_logger, __func__ );
    if( !bass.has_value() )
    {
        return ErrorCode_t::MISSING_FIELDS;
    }
    if( bass.value() > m_audioSettings["configurations"][kBassName]["properties"]["max"].asInt()
        || bass.value() < m_audioSettings["configurations"][kBassName]["properties"]["min"].asInt() )
    {
        return ErrorCode_t::INVALID_VALUE;
    }
    return SetAudioProperties( bass, kBassName, m_currentBass );
}

const ProductPb::AudioBassLevel& CustomAudioSettingsManager::GetBass() const
{
    BOSE_DEBUG( s_logger, __func__ );
    return m_currentBass;
}

////////////////////////////////////////////////////////////////////////////////////////
/// Center setting setter/getter
///////////////////////////////////////////////////////////////////////////////////////
ErrorCode_t CustomAudioSettingsManager::SetCenter( const ProductPb::AudioCenterLevel& center )
{
    BOSE_DEBUG( s_logger, __func__ );
    if( !center.has_value() )
    {
        return ErrorCode_t::MISSING_FIELDS;
    }
    if( center.value() > m_audioSettings["configurations"][kCenterName]["properties"]["max"].asInt()
        || center.value() < m_audioSettings["configurations"][kCenterName]["properties"]["min"].asInt() )
    {
        return ErrorCode_t::INVALID_VALUE;
    }
    return SetAudioProperties( center, kCenterName, m_currentCenter );
}

const ProductPb::AudioCenterLevel& CustomAudioSettingsManager::GetCenter() const
{
    BOSE_DEBUG( s_logger, __func__ );
    return m_currentCenter;
}

////////////////////////////////////////////////////////////////////////////////////////
/// Mode setting setter/getter
//////////////////////////////////////////////////////////////////////////////////////
ErrorCode_t CustomAudioSettingsManager::SetMode( const ProductPb::AudioMode& mode )
{
    BOSE_DEBUG( s_logger, __func__ );
    if( !mode.has_value() )
    {
        return ErrorCode_t::MISSING_FIELDS;
    }
    if( !isValueInArray( mode.value(),
                         m_audioSettings["configurations"][kModeName]["properties"]["supportedValues"] ) )
    {
        return ErrorCode_t::INVALID_VALUE;
    }
    return SetAudioProperties( mode, kModeName, m_currentMode );
}

const ProductPb::AudioMode& CustomAudioSettingsManager::GetMode() const
{
    BOSE_DEBUG( s_logger, __func__ );
    return m_currentMode;
}

////////////////////////////////////////////////////////////////////////////////////////
/// Treble setting setter/getter
///////////////////////////////////////////////////////////////////////////////////////
ErrorCode_t CustomAudioSettingsManager::SetTreble( const ProductPb::AudioTrebleLevel& treble )
{
    BOSE_DEBUG( s_logger, __func__ );
    if( !treble.has_value() )
    {
        return ErrorCode_t::MISSING_FIELDS;
    }
    if( treble.value() > m_audioSettings["configurations"][kTrebleName]["properties"]["max"].asInt()
        || treble.value() < m_audioSettings["configurations"][kTrebleName]["properties"]["min"].asInt() )
    {
        return ErrorCode_t::INVALID_VALUE;
    }
    return SetAudioProperties( treble, kTrebleName, m_currentTreble );
}

const ProductPb::AudioTrebleLevel& CustomAudioSettingsManager::GetTreble() const
{
    BOSE_DEBUG( s_logger, __func__ );
    return m_currentTreble;
}

void CustomAudioSettingsManager::UpdateAllProtos()
{
    BOSE_DEBUG( s_logger, __func__ );

    // Default functionality using super class functionality.
    UpdateCurrentProto( kBassName, m_currentBass );
    UpdateCurrentProto( kCenterName, m_currentCenter );
    UpdateCurrentProto( kModeName, m_currentMode );
    UpdateCurrentProto( kTrebleName, m_currentTreble );
}

/*!
 */
void CustomAudioSettingsManager::LoadTestOptions()
{
    if( m_audioSettings.isMember( kTestOptionsName ) )
    {
        if( m_audioSettings[kTestOptionsName].isMember( kTestCenterEnabledName ) )
        {
            m_centerLevelTestEnabled = m_audioSettings[kTestOptionsName][kTestCenterEnabledName].asBool();
        }

        if( m_audioSettings[kTestOptionsName].isMember( kTestModeEnabledName ) )
        {
            m_modeTestEnabled = m_audioSettings[kTestOptionsName][kTestModeEnabledName].asBool();
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////
/// Load default audio settings
///////////////////////////////////////////////////////////////////////////////////////
void CustomAudioSettingsManager::InitializeAudioSettings()
{
    BOSE_DEBUG( s_logger, __func__ );
    bool success = true; //successful reading from persistence, initialized to be true
    static Json::CharReaderBuilder readerBuilder;
    std::unique_ptr<Json::CharReader> reader( readerBuilder.newCharReader() );
    std::string errors;

    try
    {
        std::string s = m_audioSettingsPersistence->Load();
        success = reader->parse( s.c_str(), s.c_str() + s.size(), &m_audioSettings, &errors );
        if( !success )
        {
            BOSE_DEBUG( s_logger, errors.c_str() );
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

        if( !reader->parse( defaultAudioSettings->c_str(), defaultAudioSettings->c_str() + defaultAudioSettings->size(), &m_audioSettings, &errors ) )
        {
            // If reading from default configuration file failed, there's something majorly wrong, have to return
            BOSE_ERROR( s_logger, "Reading from default config file also failed with error %s", errors.c_str() );
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

    LoadTestOptions();

    // If it gets here, it means we successfully read from either persistence or default config file
    // Initialize ProtoBufs with m_audioSettings JSON values
    BOSE_DEBUG( s_logger, "Initialize current protos with m_audioSettings" );

    //
    // Default initializations using super class functionality.
    //

    initializeProto( kBassName, m_currentBass );
    initializeProto( kCenterName, m_currentCenter );
    initializeProto( kTrebleName, m_currentTreble );

    //
    // Custom initializations for properies that do not follow default paradigm.
    //

    // Mode.
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
}

}// namespace ProductApp
