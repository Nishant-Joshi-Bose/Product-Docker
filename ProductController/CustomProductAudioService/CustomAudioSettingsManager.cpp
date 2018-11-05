///////////////////////////////////////////////////////////////////////////////////////////////////
/// @file   CustomAudioSettingsManager.cpp
/// @brief  This file contains source code for setting and getting AudioSettings
///         such as bass, treble
/// Copyright 2017 Bose Corporation
////////////////////////////////////////////////////////////////////////////////////////////////////
#include <json/reader.h>
#include <fstream>
#include <iostream>
#include "DPrint.h"
#include "SystemUtils.h"
#include "CustomAudioSettingsManager.h"

static DPrint s_logger( "CustomAudioSettingsManager" );

constexpr char  kDefaultConfigPath[] = "/opt/Bose/etc/DefaultAudioSettings.json";
constexpr uint32_t kConfigVersionMajor = 3;
constexpr uint32_t kConfigVersionMinor = 0;

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
ResultCode_t CustomAudioSettingsManager::SetBass( const ProductPb::AudioBassLevel& bass )
{
    BOSE_DEBUG( s_logger, __func__ );
    if( !bass.has_value() )
    {
        return ResultCode_t::MISSING_VALUE;
    }
    if( !isStepValueValid( bass.value(),
                           m_audioSettings["audioSettingValues"][kBassName]["properties"]["min"].asInt(),
                           m_audioSettings["audioSettingValues"][kBassName]["properties"]["max"].asInt(),
                           m_audioSettings["audioSettingValues"][kBassName]["properties"]["step"].asInt() ) )
    {
        return ResultCode_t::INVALID_VALUE;
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
ResultCode_t CustomAudioSettingsManager::SetCenter( const ProductPb::AudioCenterLevel& center )
{
    BOSE_DEBUG( s_logger, __func__ );
    if( !center.has_value() )
    {
        return ResultCode_t::MISSING_VALUE;
    }
    if( !isStepValueValid( center.value(),
                           m_audioSettings["audioSettingValues"][kCenterName]["properties"]["min"].asInt(),
                           m_audioSettings["audioSettingValues"][kCenterName]["properties"]["max"].asInt(),
                           m_audioSettings["audioSettingValues"][kCenterName]["properties"]["step"].asInt() ) )
    {
        return ResultCode_t::INVALID_VALUE;
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
ResultCode_t CustomAudioSettingsManager::SetMode( const ProductPb::AudioMode& mode )
{
    BOSE_DEBUG( s_logger, __func__ );
    if( !mode.has_value() )
    {
        BOSE_INFO( s_logger, "Mode doesn't contain any value (%s)", mode.DebugString().c_str() );
        return ResultCode_t::MISSING_VALUE;
    }
    if( !isValueInArray( mode.value(),
                         m_audioSettings["audioSettingValues"][kModeName]["properties"]["supportedValues"] ) )
    {
        return ResultCode_t::INVALID_VALUE;
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
ResultCode_t CustomAudioSettingsManager::SetTreble( const ProductPb::AudioTrebleLevel& treble )
{
    BOSE_DEBUG( s_logger, __func__ );
    if( !treble.has_value() )
    {
        return ResultCode_t::MISSING_VALUE;
    }
    if( !isStepValueValid( treble.value(),
                           m_audioSettings["audioSettingValues"][kTrebleName]["properties"]["min"].asInt(),
                           m_audioSettings["audioSettingValues"][kTrebleName]["properties"]["max"].asInt(),
                           m_audioSettings["audioSettingValues"][kTrebleName]["properties"]["step"].asInt() ) )
    {
        return ResultCode_t::INVALID_VALUE;
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
    static Json::CharReaderBuilder readerBuilder;
    std::unique_ptr<Json::CharReader> reader( readerBuilder.newCharReader() );
    std::string errors;

    // Load m_AudioSettings with DefaultAudioSettings.json first
    auto const& defaultAudioSettings = SystemUtils::ReadFile( kDefaultConfigPath );
    if( !reader->parse( defaultAudioSettings->c_str(), defaultAudioSettings->c_str() + defaultAudioSettings->size(), &m_audioSettings, &errors ) )
    {
        // If reading from default configuration file failed, there's something majorly wrong
        string errorString = "Reading and parsing default config file failed with error (" + errors + ")";
        BOSE_DIE( errorString );
    }
    else if( m_audioSettings["version"]["major"].asInt() != kConfigVersionMajor )
    {
        // major version means major format or structure change in audio settings JSON
        // If the major version doesn't match between configuration file and code, there's mismatch during build procedure
        // force loading it will cause unknown error
        BOSE_DIE( "DefaultAudioSettings.json is not compatible with ProductController software" );
    }

    // If there's persisted AudioSettings.json, overwrite m_AudioSettings with persisted data
    try
    {
        Json::Value persistedAudioSettings;
        std::string persistedValue = m_audioSettingsPersistence->Load();
        if( !reader->parse( persistedValue.c_str(), persistedValue.c_str() + persistedValue.size(), &persistedAudioSettings, &errors ) )
        {
            BOSE_ERROR( s_logger, "Parsing persisted AudioSettings.json failed with error %s", errors.c_str() );
        }
        else if( persistedAudioSettings.empty() ||
                 !persistedAudioSettings.isMember( "version" ) ||
                 !persistedAudioSettings.isMember( "supportedAudioSettings" ) ||
                 !persistedAudioSettings.isMember( "audioSettingValues" ) )
        {
            BOSE_ERROR( s_logger, "Persisted m_audioSettings doesn't contain valid data" );
        }
        else if( persistedAudioSettings["version"].isMember( "major" ) &&
                 persistedAudioSettings["version"]["major"].asInt() != kConfigVersionMajor )
        {
            BOSE_ERROR( s_logger, "Persisted AudioSettings.json is not compatible with ProductController software: "
                        "AudioSettings.json has version %d.%d, and ProductController expects version %d.%d",
                        persistedAudioSettings["version"]["major"].asInt(),
                        persistedAudioSettings["version"]["minor"].asInt(),
                        kConfigVersionMajor, kConfigVersionMinor );
        }
        else
        {
            // Copy persisted data to m_AudioSettings,
            //      without overwriting those fields which exist in DefaultAudioSettings
            //      but not in persisted AudioSettings
            for( uint32_t i = 0; i < persistedAudioSettings["supportedAudioSettings"]["contentItemSensitive"].size(); i++ )
            {
                string propName = persistedAudioSettings["supportedAudioSettings"]["contentItemSensitive"][i].asString();
                m_audioSettings["audioSettingValues"][propName] = persistedAudioSettings["audioSettingValues"][propName];
            }
            for( uint32_t i = 0; i < persistedAudioSettings["supportedAudioSettings"]["contentItemNonSensitive"].size(); i++ )
            {
                string propName = persistedAudioSettings["supportedAudioSettings"]["contentItemNonSensitive"][i].asString();
                m_audioSettings["audioSettingValues"][propName] = persistedAudioSettings["audioSettingValues"][propName];
            }
        }
    }
    catch( ProtoPersistenceIF::ProtoPersistenceException& e )
    {
        BOSE_WARNING( s_logger, "Loading audioSettings from persistence failed - %s ", e.what() );
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

    ///
    /// initialize mode proto
    ///
    m_audioSettings["audioSettingValues"][kModeName]["persistenceLevel"]["persistenceSession"] = false;
    //Fill out protobuf
    std::string currPersistLvlMode = m_audioSettings["audioSettingValues"][kModeName]["persistenceLevel"]["currentPersistenceLevel"].asString();
    m_currentMode.set_persistence( currPersistLvlMode );
    if( currPersistLvlMode == kPersistContentItem )
    {
        m_currentMode.set_value( JsonToProtoField( m_currentMode.value(), m_audioSettings["audioSettingValues"][kModeName]["values"][kPersistContentItem][m_currentContentItem] ) );
    }
    else
    {
        m_currentMode.set_value( JsonToProtoField( m_currentMode.value(), m_audioSettings["audioSettingValues"][kModeName]["values"][currPersistLvlMode] ) );
    }
    for( uint32_t i = 0; i < m_audioSettings["audioSettingValues"][kModeName]["properties"]["supportedValues"].size(); i++ )
    {
        m_currentMode.mutable_properties()->add_supportedvalues( m_audioSettings["audioSettingValues"][kModeName]["properties"]["supportedValues"][i].asString() );
    }
    for( uint32_t i = 0; i < m_audioSettings["audioSettingValues"][kModeName]["properties"]["supportedPersistence"].size(); i++ )
    {
        m_currentMode.mutable_properties()->add_supportedpersistence( m_audioSettings["audioSettingValues"][kModeName]["properties"]["supportedPersistence"][i].asString() );
    }
}

}// namespace ProductApp
