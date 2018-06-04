///////////////////////////////////////////////////////////////////////////////////////////////////
/// @file   CustomAudioSettingsManager.cpp
/// @brief  This file contains source code for setting and getting AudioSettings
///         such as bass, treble, center, surround, gainOffset, avSync, subwooferGain, mode, contentType, dualMonoSelect
/// Copyright 2017 Bose Corporation
////////////////////////////////////////////////////////////////////////////////////////////////////
#include <fstream>
#include <iostream>
#include "DPrint.h"
#include "SystemUtils.h"
#include "Utilities.h"
#include "CustomAudioSettingsManager.h"
#include "ProtoToMarkup.h"

constexpr char  kDefaultConfigPath[] = "/opt/Bose/etc/DefaultAudioSettings.json";
constexpr uint32_t kConfigVersionMajor = 3;
constexpr uint32_t kConfigVersionMinor = 0;

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
///     setter returns a ResultCode_t which indicates any error during applying the setting
///     getter returns a protobuf of current bass value
/////////////////////////////////////////////////////////////////////////////////////////
ResultCode_t CustomAudioSettingsManager::SetBass( const ProductPb::AudioBassLevel& bass )
{
    BOSE_DEBUG( s_logger, __func__ );
    if( !bass.has_value() )
    {
        BOSE_INFO( s_logger, "Bass doesn't contain any value (%s)", bass.DebugString().c_str() );
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
/// Treble setting setter/getter
///     setter returns a ResultCode_t which indicates any error during applying the setting
///     getter returns a protobuf of current treble value
///////////////////////////////////////////////////////////////////////////////////////
ResultCode_t CustomAudioSettingsManager::SetTreble( const ProductPb::AudioTrebleLevel& treble )
{
    BOSE_DEBUG( s_logger, __func__ );
    if( !treble.has_value() )
    {
        BOSE_INFO( s_logger, "Treble doesn't contain any value (%s)", treble.DebugString().c_str() );
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

////////////////////////////////////////////////////////////////////////////////////////
/// Center setting setter/getter
///     setter returns a ResultCode_t which indicates any error during applying the setting
///     getter returns a protobuf of current center value
///////////////////////////////////////////////////////////////////////////////////////
ResultCode_t CustomAudioSettingsManager::SetCenter( const ProductPb::AudioCenterLevel& center )
{
    BOSE_DEBUG( s_logger, __func__ );
    if( !center.has_value() )
    {
        BOSE_INFO( s_logger, "Center doesn't contain any value (%s)", center.DebugString().c_str() );
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
/// Surround setting setter/getter
///     setter returns a ResultCode_t which indicates any error during applying the setting
///     getter returns a protobuf of current surround value
///////////////////////////////////////////////////////////////////////////////////////
ResultCode_t CustomAudioSettingsManager::SetSurround( const ProductPb::AudioSurroundLevel& surround )
{
    BOSE_DEBUG( s_logger, __func__ );
    if( !surround.has_value() )
    {
        BOSE_INFO( s_logger, "Surround doesn't contain any value (%s)", surround.DebugString().c_str() );
        return ResultCode_t::MISSING_VALUE;
    }
    if( !isStepValueValid( surround.value(),
                           m_audioSettings["audioSettingValues"][kSurroundName]["properties"]["min"].asInt(),
                           m_audioSettings["audioSettingValues"][kSurroundName]["properties"]["max"].asInt(),
                           m_audioSettings["audioSettingValues"][kSurroundName]["properties"]["step"].asInt() ) )
    {
        return ResultCode_t::INVALID_VALUE;
    }
    return SetAudioProperties( surround, kSurroundName, m_currentSurround );
}

const ProductPb::AudioSurroundLevel& CustomAudioSettingsManager::GetSurround() const
{
    BOSE_DEBUG( s_logger, __func__ );
    return m_currentSurround;
}

////////////////////////////////////////////////////////////////////////////////////////
/// SurroundDelay setting setter/getter
///     setter returns a boolean which indicates whether current surroundDelay value is changed by setter
///     getter returns a protobuf of current surroundDelay value
///////////////////////////////////////////////////////////////////////////////////////
ResultCode_t CustomAudioSettingsManager::SetSurroundDelay( const ProductPb::AudioSurroundDelay& surroundDelay )
{
    if( !surroundDelay.has_value() )
    {
        BOSE_INFO( s_logger, "SurroundDelay doesn't contain any value (%s)", surroundDelay.DebugString().c_str() );
        return ResultCode_t::MISSING_VALUE;
    }
    if( !isStepValueValid( surroundDelay.value(),
                           m_audioSettings["audioSettingValues"][kSurroundDelayName]["properties"]["min"].asInt(),
                           m_audioSettings["audioSettingValues"][kSurroundDelayName]["properties"]["max"].asInt(),
                           m_audioSettings["audioSettingValues"][kSurroundDelayName]["properties"]["step"].asInt() ) )
    {
        return ResultCode_t::INVALID_VALUE;
    }
    return SetAudioProperties( surroundDelay, kSurroundDelayName, m_currentSurroundDelay );
}

const ProductPb::AudioSurroundDelay& CustomAudioSettingsManager::GetSurroundDelay() const
{
    BOSE_DEBUG( s_logger, __func__ );
    return m_currentSurroundDelay;
}

////////////////////////////////////////////////////////////////////////////////////////
/// GainOffset setting setter/getter
///     setter returns a ResultCode_t which indicates any error during applying the setting
///     getter returns a protobuf of current gainOffset value
//////////////////////////////////////////////////////////////////////////////////////
ResultCode_t CustomAudioSettingsManager::SetGainOffset( const ProductPb::AudioGainOffset& gainOffset )
{
    BOSE_DEBUG( s_logger, __func__ );
    if( !gainOffset.has_value() )
    {
        BOSE_INFO( s_logger, "GainOffset doesn't contain any value (%s)", gainOffset.DebugString().c_str() );
        return ResultCode_t::MISSING_VALUE;
    }
    if( !isStepValueValid( gainOffset.value(),
                           m_audioSettings["audioSettingValues"][kGainOffsetName]["properties"]["min"].asInt(),
                           m_audioSettings["audioSettingValues"][kGainOffsetName]["properties"]["max"].asInt(),
                           m_audioSettings["audioSettingValues"][kGainOffsetName]["properties"]["step"].asInt() ) )
    {
        return ResultCode_t::INVALID_VALUE;
    }
    return SetAudioProperties( gainOffset, kGainOffsetName, m_currentGainOffset );
}

const ProductPb::AudioGainOffset& CustomAudioSettingsManager::GetGainOffset() const
{
    BOSE_DEBUG( s_logger, __func__ );
    return m_currentGainOffset;
}

////////////////////////////////////////////////////////////////////////////////////////
/// AvSync setting setter/getter
///     setter returns a ResultCode_t which indicates any error during applying the setting
///     getter returns a protobuf of current avSync value
//////////////////////////////////////////////////////////////////////////////////////
ResultCode_t CustomAudioSettingsManager::SetAvSync( const ProductPb::AudioAvSync& avSync )
{
    BOSE_DEBUG( s_logger, __func__ );
    if( !avSync.has_value() )
    {
        BOSE_INFO( s_logger, "AvSync doesn't contain any value (%s)", avSync.DebugString().c_str() );
        return ResultCode_t::MISSING_VALUE;
    }
    if( !isStepValueValid( avSync.value(),
                           m_audioSettings["audioSettingValues"][kAvSyncName]["properties"]["min"].asInt(),
                           m_audioSettings["audioSettingValues"][kAvSyncName]["properties"]["max"].asInt(),
                           m_audioSettings["audioSettingValues"][kAvSyncName]["properties"]["step"].asInt() ) )
    {
        return ResultCode_t::INVALID_VALUE;
    }
    return SetAudioProperties( avSync, kAvSyncName, m_currentAvSync );
}

const ProductPb::AudioAvSync& CustomAudioSettingsManager::GetAvSync() const
{
    BOSE_DEBUG( s_logger, __func__ );
    return m_currentAvSync;
}

////////////////////////////////////////////////////////////////////////////////////////
/// SubwooferGain setting setter/getter
///     setter returns a ResultCode_t which indicates any error during applying the setting
///     getter returns a protobuf of current subwooferGain value
//////////////////////////////////////////////////////////////////////////////////////
ResultCode_t CustomAudioSettingsManager::SetSubwooferGain( const ProductPb::AudioSubwooferGain& subwooferGain )
{
    BOSE_DEBUG( s_logger, __func__ );
    if( !subwooferGain.has_value() )
    {
        BOSE_INFO( s_logger, "SubwooferGain doesn't contain any value (%s)", subwooferGain.DebugString().c_str() );
        return ResultCode_t::MISSING_VALUE;
    }
    if( !isStepValueValid( subwooferGain.value(),
                           m_audioSettings["audioSettingValues"][kSubwooferGainName]["properties"]["min"].asInt(),
                           m_audioSettings["audioSettingValues"][kSubwooferGainName]["properties"]["max"].asInt(),
                           m_audioSettings["audioSettingValues"][kSubwooferGainName]["properties"]["step"].asInt() ) )
    {
        return ResultCode_t::INVALID_VALUE;
    }
    return SetAudioProperties( subwooferGain, kSubwooferGainName, m_currentSubwooferGain );
}

const ProductPb::AudioSubwooferGain& CustomAudioSettingsManager::GetSubwooferGain() const
{
    BOSE_DEBUG( s_logger, __func__ );
    return m_currentSubwooferGain;
}

////////////////////////////////////////////////////////////////////////////////////////
/// Mode setting setter/getter
///     setter returns a ResultCode_t which indicates any error during applying the setting
///     getter returns a protobuf of current mode value
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
/// ContentType setting setter/getter
///     setter returns a ResultCode_t which indicates any error during applying the setting
///     getter returns a protobuf of current contentType value
//////////////////////////////////////////////////////////////////////////////////////
ResultCode_t CustomAudioSettingsManager::SetContentType( const ProductPb::AudioContentType& contentType )
{
    BOSE_DEBUG( s_logger, __func__ );
    if( !contentType.has_value() )
    {
        BOSE_INFO( s_logger, "ContentType doesn't contain any value (%s)", contentType.DebugString().c_str() );
        return ResultCode_t::MISSING_VALUE;
    }
    if( !isValueInArray( contentType.value(),
                         m_audioSettings["audioSettingValues"][kContentTypeName]["properties"]["supportedValues"] ) )
    {
        return ResultCode_t::INVALID_VALUE;
    }
    return SetAudioProperties( contentType, kContentTypeName, m_currentContentType );
}
const ProductPb::AudioContentType& CustomAudioSettingsManager::GetContentType() const
{
    BOSE_DEBUG( s_logger, __func__ );
    return m_currentContentType;
}

////////////////////////////////////////////////////////////////////////////////////////
/// DualMonoSelect setting setter/getter
///     setter returns a ResultCode_t which indicates any error during applying the setting
///     getter returns a protobuf of current dualMonoSelect value
//////////////////////////////////////////////////////////////////////////////////////
ResultCode_t CustomAudioSettingsManager::SetDualMonoSelect( const ProductPb::AudioDualMonoSelect& dualMonoSelect )
{
    BOSE_DEBUG( s_logger, __func__ );
    if( !dualMonoSelect.has_value() )
    {
        BOSE_INFO( s_logger, "DualMonoSelect doesn't contain any value (%s)", dualMonoSelect.DebugString().c_str() );
        return ResultCode_t::MISSING_VALUE;
    }

    // Check if new value is valid in supportedValue list
    if( !isValueInArray( dualMonoSelect.value(),
                         m_audioSettings["audioSettingValues"][kDualMonoSelectName]["properties"]["supportedValues"] ) )
    {
        return ResultCode_t::INVALID_VALUE;
    }

    //Check if value changed from last time
    if( m_currentDualMonoSelect.value() == dualMonoSelect.value() )
    {
        BOSE_INFO( s_logger, "DualMonoSelect value doesn't change from last time (%s)", dualMonoSelect.DebugString().c_str() );
        return ResultCode_t::VALUE_UNCHANGED;
    }
    m_audioSettings["audioSettingValues"][kDualMonoSelectName]["values"][kPersistGlobal] = dualMonoSelect.value();
    m_currentDualMonoSelect.set_value( dualMonoSelect.value() );
    PersistAudioSettings();
    return ResultCode_t::NO_ERROR;
}
const ProductPb::AudioDualMonoSelect& CustomAudioSettingsManager::GetDualMonoSelect() const
{
    BOSE_DEBUG( s_logger, __func__ );
    return m_currentDualMonoSelect;
}

////////////////////////////////////////////////////////////////////////////////////////
/// EqSelect setting setter/getter
///     setter returns a boolean which indicates whether current EqSelect value is changed by setter
///     getter returns a protobuf of current EqSelect value
//////////////////////////////////////////////////////////////////////////////////////
ResultCode_t CustomAudioSettingsManager::SetEqSelect( const ProductPb::AudioEqSelect& eqSelect )
{
    BOSE_DEBUG( s_logger, __func__ );
    if( !eqSelect.has_mode() )
    {
        BOSE_INFO( s_logger, "EqSelect doesn't contain any mode (%s)", eqSelect.DebugString().c_str() );
        return ResultCode_t::MISSING_VALUE;
    }

    // Check if new value is valid in supportedValue list
    if( !isValueInArray( eqSelect.mode(),
                         m_audioSettings["audioSettingValues"][kEqSelectName]["properties"]["supportedModes"] ) )
    {
        return ResultCode_t::INVALID_VALUE;
    }

    //Check if value changed from last time
    if( m_currentEqSelect.mode() == eqSelect.mode() )
    {
        BOSE_INFO( s_logger, "EqSelect value doesn't change from last time (%s)", eqSelect.DebugString().c_str() );
        return ResultCode_t::VALUE_UNCHANGED;
    }
    m_audioSettings["audioSettingValues"][kEqSelectName]["values"][kPersistGlobal] = eqSelect.mode();
    m_currentEqSelect.set_mode( eqSelect.mode() );
    PersistAudioSettings();
    return ResultCode_t::NO_ERROR;
}
const ProductPb::AudioEqSelect& CustomAudioSettingsManager::GetEqSelect() const
{
    BOSE_DEBUG( s_logger, __func__ );
    return m_currentEqSelect;
}
void CustomAudioSettingsManager::UpdateEqSelectSupportedMode( string mode, bool supported )
{
    BOSE_DEBUG( s_logger, __func__ );
    bool currSupported = isValueInArray( mode, m_audioSettings["audioSettingValues"]["audioEqSelect"]["properties"]["supportedModes"] );
    // if new mode is supported, add it to the list
    // note: currently we don't have use case of dynamically removing modes (modes can only be reset by factory default).
    //       if use case does come up, rules have to be defined, and handling has to be added here
    if( supported && !currSupported )
    {
        m_audioSettings["audioSettingValues"]["audioEqSelect"]["properties"]["supportedModes"].append( mode );
        m_currentEqSelect.mutable_properties()->add_supportedmodes( mode );
        PersistAudioSettings();
    }
}

////////////////////////////////////////////////////////////////////////////////////////
/// SubwooferPolarity setting setter/getter
///     setter returns a boolean which indicates whether current subwooferPolarity value is changed by setter
///     getter returns a protobuf of current subwooferPolarity value
//////////////////////////////////////////////////////////////////////////////////////
ResultCode_t CustomAudioSettingsManager::SetSubwooferPolarity( const ProductPb::AudioSubwooferPolarity& subwooferPolarity )
{
    BOSE_DEBUG( s_logger, __func__ );
    if( !subwooferPolarity.has_value() )
    {
        BOSE_INFO( s_logger, "SubwooferPolarity doesn't contain any value (%s)", subwooferPolarity.DebugString().c_str() );
        return ResultCode_t::MISSING_VALUE;
    }
    // Check if new value is valid in supportedValue list
    if( !isValueInArray( subwooferPolarity.value(),
                         m_audioSettings["audioSettingValues"][kSubwooferPolarityName]["properties"]["supportedPolarity"] ) )
    {
        return ResultCode_t::INVALID_VALUE;
    }
    //Check if value changed from last time
    if( m_currentSubwooferPolarity.value() == subwooferPolarity.value() )
    {
        BOSE_INFO( s_logger, "SubwooferPolarity value doesn't change from last time (%s)", subwooferPolarity.DebugString().c_str() );
        return ResultCode_t::VALUE_UNCHANGED;
    }
    m_audioSettings["audioSettingValues"][kSubwooferPolarityName]["values"][kPersistGlobal] = subwooferPolarity.value();
    m_currentSubwooferPolarity.set_value( subwooferPolarity.value() );
    PersistAudioSettings();
    return ResultCode_t::NO_ERROR;
}
const ProductPb::AudioSubwooferPolarity& CustomAudioSettingsManager::GetSubwooferPolarity() const
{
    BOSE_DEBUG( s_logger, __func__ );
    return m_currentSubwooferPolarity;
}


void CustomAudioSettingsManager::UpdateAllProtos()
{
    BOSE_DEBUG( s_logger, __func__ );
    // Only required for contentItem sensitive audio settings
    // contentItem non-sensitive audio settings will only be set through setters, no other ways
    UpdateCurrentProto( kBassName,          m_currentBass );
    UpdateCurrentProto( kTrebleName,        m_currentTreble );
    UpdateCurrentProto( kCenterName,        m_currentCenter );
    UpdateCurrentProto( kSurroundName,      m_currentSurround );
    UpdateCurrentProto( kSurroundDelayName, m_currentSurroundDelay );
    UpdateCurrentProto( kGainOffsetName,    m_currentGainOffset );
    UpdateCurrentProto( kAvSyncName,        m_currentAvSync );
    UpdateCurrentProto( kSubwooferGainName, m_currentSubwooferGain );
    UpdateCurrentProto( kModeName,          m_currentMode );
    UpdateCurrentProto( kContentTypeName,   m_currentContentType );
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

    // Initialize ProtoBufs with m_audioSettings JSON values
    BOSE_DEBUG( s_logger, "Initialize current protos with m_audioSettings" );
    initializeProto( kBassName, m_currentBass );
    initializeProto( kTrebleName, m_currentTreble );
    initializeProto( kCenterName, m_currentCenter );
    initializeProto( kSurroundName, m_currentSurround );
    initializeProto( kSurroundDelayName, m_currentSurroundDelay );
    initializeProto( kGainOffsetName, m_currentGainOffset );
    initializeProto( kSubwooferGainName, m_currentSubwooferGain );
    initializeProto( kAvSyncName, m_currentAvSync );

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

    ///
    /// initialize contentType proto
    ///
    m_audioSettings["audioSettingValues"][kContentTypeName]["persistenceLevel"]["persistenceSession"] = false;
    //Fill out protobuf
    std::string currPersistLvlContentType = m_audioSettings["audioSettingValues"][kContentTypeName]["persistenceLevel"]["currentPersistenceLevel"].asString();
    m_currentContentType.set_persistence( currPersistLvlContentType );
    if( currPersistLvlContentType == kPersistContentItem )
    {
        m_currentContentType.set_value( JsonToProtoField( m_currentContentType.value(), m_audioSettings["audioSettingValues"][kContentTypeName]["values"][kPersistContentItem][m_currentContentItem] ) );
    }
    else
    {
        m_currentContentType.set_value( JsonToProtoField( m_currentContentType.value(), m_audioSettings["audioSettingValues"][kContentTypeName]["values"][currPersistLvlContentType] ) );
    }
    for( uint32_t i = 0; i < m_audioSettings["audioSettingValues"][kContentTypeName]["properties"]["supportedValues"].size(); i++ )
    {
        m_currentContentType.mutable_properties()->add_supportedvalues( m_audioSettings["audioSettingValues"][kContentTypeName]["properties"]["supportedValues"][i].asString() );
    }
    for( uint32_t i = 0; i < m_audioSettings["audioSettingValues"][kContentTypeName]["properties"]["supportedPersistence"].size(); i++ )
    {
        m_currentContentType.mutable_properties()->add_supportedpersistence( m_audioSettings["audioSettingValues"][kContentTypeName]["properties"]["supportedPersistence"][i].asString() );
    }

    ///
    /// initialize dualMonoSelect proto
    ///
    m_audioSettings["audioSettingValues"][kDualMonoSelectName]["persistenceLevel"]["persistenceSession"] = false;
    //Fill out protobuf
    m_currentDualMonoSelect.set_value( JsonToProtoField( m_currentDualMonoSelect.value(), m_audioSettings["audioSettingValues"][kDualMonoSelectName]["values"][kPersistGlobal] ) );
    for( uint32_t i = 0; i < m_audioSettings["audioSettingValues"][kDualMonoSelectName]["properties"]["supportedValues"].size(); i++ )
    {
        m_currentDualMonoSelect.mutable_properties()->add_supportedvalues( m_audioSettings["audioSettingValues"][kDualMonoSelectName]["properties"]["supportedValues"][i].asString() );
    }

    ///
    /// initialize eqSelect proto
    ///
    m_audioSettings["audioSettingValues"][kEqSelectName]["persistenceLevel"]["persistenceSession"] = false;
    //Fill out protobuf
    m_currentEqSelect.set_mode( JsonToProtoField( m_currentEqSelect.mode(), m_audioSettings["audioSettingValues"][kEqSelectName]["values"][kPersistGlobal] ) );
    for( uint32_t i = 0; i < m_audioSettings["audioSettingValues"][kEqSelectName]["properties"]["supportedModes"].size(); i++ )
    {
        m_currentEqSelect.mutable_properties()->add_supportedmodes( m_audioSettings["audioSettingValues"][kEqSelectName]["properties"]["supportedModes"][i].asString() );
    }

    ///
    /// initialize subwooferPolarity proto
    ///
    m_audioSettings["audioSettingValues"][kSubwooferPolarityName]["persistenceLevel"]["persistenceSession"] = false;
    //Fill out protobuf
    m_currentSubwooferPolarity.set_value( JsonToProtoField( m_currentSubwooferPolarity.value(), m_audioSettings["audioSettingValues"][kSubwooferPolarityName]["values"][kPersistGlobal] ) );
    for( uint32_t i = 0; i < m_audioSettings["audioSettingValues"][kSubwooferPolarityName]["properties"]["supportedPolarity"].size(); i++ )
    {
        m_currentSubwooferPolarity.mutable_properties()->add_supportedpolarity( m_audioSettings["audioSettingValues"][kSubwooferPolarityName]["properties"]["supportedPolarity"][i].asString() );
    }
}

}// namespace ProductApp
