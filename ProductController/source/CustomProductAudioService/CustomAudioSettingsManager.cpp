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
constexpr uint32_t kConfigVersionMajor = 4;
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

constexpr char kAudioSettingValues      [] = "audioSettingValues";
constexpr char kProperties              [] = "properties";

namespace ProductApp
{
using std::string;
using namespace ProductPb;

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
ResultCode_t CustomAudioSettingsManager::SetBass( const AudioBassLevel& bass )
{
    BOSE_DEBUG( s_logger, __func__ );
    if( !bass.has_value() )
    {
        BOSE_INFO( s_logger, "Bass doesn't contain any value (%s)", bass.DebugString().c_str() );
        return ResultCode_t::MISSING_VALUE;
    }
    const Json::Value& properties = m_audioSettings[kAudioSettingValues][kBassName][kProperties];
    if( !isStepValueValid( bass.value(),
                           properties["min"].asInt(),
                           properties["max"].asInt(),
                           properties["step"].asInt() ) )
    {
        return ResultCode_t::INVALID_VALUE;
    }
    return SetAudioProperties( bass, kBassName, m_currentBass );
}

const AudioBassLevel& CustomAudioSettingsManager::GetBass() const
{
    BOSE_DEBUG( s_logger, __func__ );
    return m_currentBass;
}

////////////////////////////////////////////////////////////////////////////////////////
/// Treble setting setter/getter
///     setter returns a ResultCode_t which indicates any error during applying the setting
///     getter returns a protobuf of current treble value
///////////////////////////////////////////////////////////////////////////////////////
ResultCode_t CustomAudioSettingsManager::SetTreble( const AudioTrebleLevel& treble )
{
    BOSE_DEBUG( s_logger, __func__ );
    if( !treble.has_value() )
    {
        BOSE_INFO( s_logger, "Treble doesn't contain any value (%s)", treble.DebugString().c_str() );
        return ResultCode_t::MISSING_VALUE;
    }
    const Json::Value& properties = m_audioSettings[kAudioSettingValues][kTrebleName][kProperties];
    if( !isStepValueValid( treble.value(),
                           properties["min"].asInt(),
                           properties["max"].asInt(),
                           properties["step"].asInt() ) )
    {
        return ResultCode_t::INVALID_VALUE;
    }
    return SetAudioProperties( treble, kTrebleName, m_currentTreble );
}

const AudioTrebleLevel& CustomAudioSettingsManager::GetTreble() const
{
    BOSE_DEBUG( s_logger, __func__ );
    return m_currentTreble;
}

////////////////////////////////////////////////////////////////////////////////////////
/// Center setting setter/getter
///     setter returns a ResultCode_t which indicates any error during applying the setting
///     getter returns a protobuf of current center value
///////////////////////////////////////////////////////////////////////////////////////
ResultCode_t CustomAudioSettingsManager::SetCenter( const AudioCenterLevel& center )
{
    BOSE_DEBUG( s_logger, __func__ );
    if( !center.has_value() )
    {
        BOSE_INFO( s_logger, "Center doesn't contain any value (%s)", center.DebugString().c_str() );
        return ResultCode_t::MISSING_VALUE;
    }
    const Json::Value& properties = m_audioSettings[kAudioSettingValues][kCenterName][kProperties];
    if( !isStepValueValid( center.value(),
                           properties["min"].asInt(),
                           properties["max"].asInt(),
                           properties["step"].asInt() ) )
    {
        return ResultCode_t::INVALID_VALUE;
    }
    return SetAudioProperties( center, kCenterName, m_currentCenter );
}

const AudioCenterLevel& CustomAudioSettingsManager::GetCenter() const
{
    BOSE_DEBUG( s_logger, __func__ );
    return m_currentCenter;
}

////////////////////////////////////////////////////////////////////////////////////////
/// Surround setting setter/getter
///     setter returns a ResultCode_t which indicates any error during applying the setting
///     getter returns a protobuf of current surround value
///////////////////////////////////////////////////////////////////////////////////////
ResultCode_t CustomAudioSettingsManager::SetSurround( const AudioSurroundLevel& surround )
{
    BOSE_DEBUG( s_logger, __func__ );
    if( !surround.has_value() )
    {
        BOSE_INFO( s_logger, "Surround doesn't contain any value (%s)", surround.DebugString().c_str() );
        return ResultCode_t::MISSING_VALUE;
    }
    const Json::Value& properties = m_audioSettings[kAudioSettingValues][kSurroundName][kProperties];
    if( !isStepValueValid( surround.value(),
                           properties["min"].asInt(),
                           properties["max"].asInt(),
                           properties["step"].asInt() ) )
    {
        return ResultCode_t::INVALID_VALUE;
    }
    return SetAudioProperties( surround, kSurroundName, m_currentSurround );
}

const AudioSurroundLevel& CustomAudioSettingsManager::GetSurround() const
{
    BOSE_DEBUG( s_logger, __func__ );
    return m_currentSurround;
}

////////////////////////////////////////////////////////////////////////////////////////
/// SurroundDelay setting setter/getter
///     setter returns a boolean which indicates whether current surroundDelay value is changed by setter
///     getter returns a protobuf of current surroundDelay value
///////////////////////////////////////////////////////////////////////////////////////
ResultCode_t CustomAudioSettingsManager::SetSurroundDelay( const AudioSurroundDelay& surroundDelay )
{
    if( !surroundDelay.has_value() )
    {
        BOSE_INFO( s_logger, "SurroundDelay doesn't contain any value (%s)", surroundDelay.DebugString().c_str() );
        return ResultCode_t::MISSING_VALUE;
    }
    const Json::Value& properties = m_audioSettings[kAudioSettingValues][kSurroundDelayName][kProperties];
    if( !isStepValueValid( surroundDelay.value(),
                           properties["min"].asInt(),
                           properties["max"].asInt(),
                           properties["step"].asInt() ) )
    {
        return ResultCode_t::INVALID_VALUE;
    }
    return SetAudioProperties( surroundDelay, kSurroundDelayName, m_currentSurroundDelay );
}

const AudioSurroundDelay& CustomAudioSettingsManager::GetSurroundDelay() const
{
    BOSE_DEBUG( s_logger, __func__ );
    return m_currentSurroundDelay;
}

////////////////////////////////////////////////////////////////////////////////////////
/// GainOffset setting setter/getter
///     setter returns a ResultCode_t which indicates any error during applying the setting
///     getter returns a protobuf of current gainOffset value
//////////////////////////////////////////////////////////////////////////////////////
ResultCode_t CustomAudioSettingsManager::SetGainOffset( const AudioGainOffset& gainOffset )
{
    BOSE_DEBUG( s_logger, __func__ );
    if( !gainOffset.has_value() )
    {
        BOSE_INFO( s_logger, "GainOffset doesn't contain any value (%s)", gainOffset.DebugString().c_str() );
        return ResultCode_t::MISSING_VALUE;
    }
    const Json::Value& properties = m_audioSettings[kAudioSettingValues][kGainOffsetName][kProperties];
    if( !isStepValueValid( gainOffset.value(),
                           properties["min"].asInt(),
                           properties["max"].asInt(),
                           properties["step"].asInt() ) )
    {
        return ResultCode_t::INVALID_VALUE;
    }
    return SetAudioProperties( gainOffset, kGainOffsetName, m_currentGainOffset );
}

const AudioGainOffset& CustomAudioSettingsManager::GetGainOffset() const
{
    BOSE_DEBUG( s_logger, __func__ );
    return m_currentGainOffset;
}

////////////////////////////////////////////////////////////////////////////////////////
/// AvSync setting setter/getter
///     setter returns a ResultCode_t which indicates any error during applying the setting
///     getter returns a protobuf of current avSync value
//////////////////////////////////////////////////////////////////////////////////////
ResultCode_t CustomAudioSettingsManager::SetAvSync( const AudioAvSync& avSync )
{
    BOSE_DEBUG( s_logger, __func__ );
    if( !avSync.has_value() )
    {
        BOSE_INFO( s_logger, "AvSync doesn't contain any value (%s)", avSync.DebugString().c_str() );
        return ResultCode_t::MISSING_VALUE;
    }
    const Json::Value& properties = m_audioSettings[kAudioSettingValues][kAvSyncName][kProperties];
    if( !isStepValueValid( avSync.value(),
                           properties["min"].asInt(),
                           properties["max"].asInt(),
                           properties["step"].asInt() ) )
    {
        return ResultCode_t::INVALID_VALUE;
    }
    return SetAudioProperties( avSync, kAvSyncName, m_currentAvSync );
}

const AudioAvSync& CustomAudioSettingsManager::GetAvSync() const
{
    BOSE_DEBUG( s_logger, __func__ );
    return m_currentAvSync;
}

////////////////////////////////////////////////////////////////////////////////////////
/// SubwooferGain setting setter/getter
///     setter returns a ResultCode_t which indicates any error during applying the setting
///     getter returns a protobuf of current subwooferGain value
//////////////////////////////////////////////////////////////////////////////////////
ResultCode_t CustomAudioSettingsManager::SetSubwooferGain( const AudioSubwooferGain& subwooferGain )
{
    BOSE_DEBUG( s_logger, __func__ );
    if( !subwooferGain.has_value() )
    {
        BOSE_INFO( s_logger, "SubwooferGain doesn't contain any value (%s)", subwooferGain.DebugString().c_str() );
        return ResultCode_t::MISSING_VALUE;
    }
    const Json::Value& properties = m_audioSettings[kAudioSettingValues][kSubwooferGainName][kProperties];
    if( !isStepValueValid( subwooferGain.value(),
                           properties["min"].asInt(),
                           properties["max"].asInt(),
                           properties["step"].asInt() ) )
    {
        return ResultCode_t::INVALID_VALUE;
    }
    return SetAudioProperties( subwooferGain, kSubwooferGainName, m_currentSubwooferGain );
}

const AudioSubwooferGain& CustomAudioSettingsManager::GetSubwooferGain() const
{
    BOSE_DEBUG( s_logger, __func__ );
    return m_currentSubwooferGain;
}

////////////////////////////////////////////////////////////////////////////////////////
/// Mode setting setter/getter
///     setter returns a ResultCode_t which indicates any error during applying the setting
///     getter returns a protobuf of current mode value
//////////////////////////////////////////////////////////////////////////////////////
ResultCode_t CustomAudioSettingsManager::SetMode( AudioMode& mode )
{
    BOSE_DEBUG( s_logger, __func__ );
    if( !mode.has_value() )
    {
        BOSE_INFO( s_logger, "Mode doesn't contain any value (%s)", mode.DebugString().c_str() );
        return ResultCode_t::MISSING_VALUE;
    }
    if( !isValueInArray( mode.value(),
                         m_audioSettings[kAudioSettingValues][kModeName][kProperties]["supportedValues"] ) )
    {
        return ResultCode_t::INVALID_VALUE;
    }
    // /audio/mode's persistence level is default to "SESSION"
    if( !mode.has_persistence() )
    {
        mode.set_persistence( kPersistSession );
    }
    return SetAudioProperties( mode, kModeName, m_currentMode );
}
const AudioMode& CustomAudioSettingsManager::GetMode() const
{
    BOSE_DEBUG( s_logger, __func__ );
    return m_currentMode;
}

////////////////////////////////////////////////////////////////////////////////////////
/// ContentType setting setter/getter
///     setter returns a ResultCode_t which indicates any error during applying the setting
///     getter returns a protobuf of current contentType value
//////////////////////////////////////////////////////////////////////////////////////
ResultCode_t CustomAudioSettingsManager::SetContentType( const AudioContentType& contentType )
{
    BOSE_DEBUG( s_logger, __func__ );
    if( !contentType.has_value() )
    {
        BOSE_INFO( s_logger, "ContentType doesn't contain any value (%s)", contentType.DebugString().c_str() );
        return ResultCode_t::MISSING_VALUE;
    }
    if( !isValueInArray( contentType.value(),
                         m_audioSettings[kAudioSettingValues][kContentTypeName][kProperties]["supportedValues"] ) )
    {
        return ResultCode_t::INVALID_VALUE;
    }
    return SetAudioProperties( contentType, kContentTypeName, m_currentContentType );
}
const AudioContentType& CustomAudioSettingsManager::GetContentType() const
{
    BOSE_DEBUG( s_logger, __func__ );
    return m_currentContentType;
}

////////////////////////////////////////////////////////////////////////////////////////
/// DualMonoSelect setting setter/getter
///     setter returns a ResultCode_t which indicates any error during applying the setting
///     getter returns a protobuf of current dualMonoSelect value
//////////////////////////////////////////////////////////////////////////////////////
ResultCode_t CustomAudioSettingsManager::SetDualMonoSelect( const AudioDualMonoSelect& dualMonoSelect )
{
    BOSE_DEBUG( s_logger, __func__ );
    if( !dualMonoSelect.has_value() )
    {
        BOSE_INFO( s_logger, "DualMonoSelect doesn't contain any value (%s)", dualMonoSelect.DebugString().c_str() );
        return ResultCode_t::MISSING_VALUE;
    }

    // Check if new value is valid in supportedValue list
    if( !isValueInArray( dualMonoSelect.value(),
                         m_audioSettings[kAudioSettingValues][kDualMonoSelectName][kProperties]["supportedValues"] ) )
    {
        return ResultCode_t::INVALID_VALUE;
    }

    //Check if value changed from last time
    if( m_currentDualMonoSelect.value() == dualMonoSelect.value() )
    {
        BOSE_INFO( s_logger, "DualMonoSelect value doesn't change from last time (%s)", dualMonoSelect.DebugString().c_str() );
        return ResultCode_t::VALUE_UNCHANGED;
    }

    // /audio/dualMonoSelect doesn't allow user to set persistence, and the persistence is default to "SESSION"
    // Once content item change, the setting will be reverted to global setting, until next time user sets it
    m_audioSettings[kAudioSettingValues][kDualMonoSelectName]["persistenceLevel"]["persistenceSession"] = true;
    m_audioSettings[kAudioSettingValues][kDualMonoSelectName]["values"][kPersistSession] = dualMonoSelect.value();
    m_currentDualMonoSelect.set_value( dualMonoSelect.value() );
    PersistAudioSettings();
    return ResultCode_t::NO_ERROR;
}
const AudioDualMonoSelect& CustomAudioSettingsManager::GetDualMonoSelect() const
{
    BOSE_DEBUG( s_logger, __func__ );
    return m_currentDualMonoSelect;
}

////////////////////////////////////////////////////////////////////////////////////////
/// EqSelect setting setter/getter
///     setter returns a boolean which indicates whether current EqSelect value is changed by setter
///     getter returns a protobuf of current EqSelect value
//////////////////////////////////////////////////////////////////////////////////////
ResultCode_t CustomAudioSettingsManager::SetEqSelect( const AudioEqSelect& eqSelect )
{
    BOSE_DEBUG( s_logger, __func__ );
    if( !eqSelect.has_mode() )
    {
        BOSE_INFO( s_logger, "EqSelect doesn't contain any mode (%s)", eqSelect.DebugString().c_str() );
        return ResultCode_t::MISSING_VALUE;
    }

    // Check if new value is valid in supportedValue list
    if( !isValueInArray( eqSelect.mode(),
                         m_audioSettings[kAudioSettingValues][kEqSelectName][kProperties]["supportedModes"] ) )
    {
        return ResultCode_t::INVALID_VALUE;
    }

    //Check if value changed from last time
    if( m_currentEqSelect.mode() == eqSelect.mode() )
    {
        BOSE_INFO( s_logger, "EqSelect value doesn't change from last time (%s)", eqSelect.DebugString().c_str() );
        return ResultCode_t::VALUE_UNCHANGED;
    }
    m_audioSettings[kAudioSettingValues][kEqSelectName]["values"][kPersistGlobal] = eqSelect.mode();
    m_currentEqSelect.set_mode( eqSelect.mode() );
    PersistAudioSettings();
    return ResultCode_t::NO_ERROR;
}
const AudioEqSelect& CustomAudioSettingsManager::GetEqSelect() const
{
    BOSE_DEBUG( s_logger, __func__ );
    return m_currentEqSelect;
}
ResultCode_t CustomAudioSettingsManager::UpdateEqSelectSupportedMode( string mode, bool supported )
{
    BOSE_DEBUG( s_logger, __func__ );

    bool currSupported = isValueInArray( mode, m_audioSettings[kAudioSettingValues][kEqSelectName][kProperties]["supportedModes"] );
    // if new mode is supported, add it to the list
    // note: currently we don't have use case of dynamically removing modes (modes can only be reset by factory default).
    //       if use case does come up, rules have to be defined, and handling has to be added here
    if( supported && !currSupported )
    {
        m_audioSettings[kAudioSettingValues][kEqSelectName][kProperties]["supportedModes"].append( mode );
        m_currentEqSelect.mutable_properties()->add_supportedmodes( mode );
        if( mode == AudioEqSelect_supportedMode_Name( AudioEqSelect_supportedMode_EQ_AIQ_A ) )
        {
            m_currentEqSelect.set_storedeqvalid( true );
        }
        PersistAudioSettings();
        return ResultCode_t::NO_ERROR;
    }
    else
    {
        return ResultCode_t::VALUE_UNCHANGED;
    }
}

////////////////////////////////////////////////////////////////////////////////////////
/// SubwooferPolarity setting setter/getter
///     setter returns a boolean which indicates whether current subwooferPolarity value is changed by setter
///     getter returns a protobuf of current subwooferPolarity value
//////////////////////////////////////////////////////////////////////////////////////
ResultCode_t CustomAudioSettingsManager::SetSubwooferPolarity( const AudioSubwooferPolarity& subwooferPolarity )
{
    BOSE_DEBUG( s_logger, __func__ );
    if( !subwooferPolarity.has_value() )
    {
        BOSE_INFO( s_logger, "SubwooferPolarity doesn't contain any value (%s)", subwooferPolarity.DebugString().c_str() );
        return ResultCode_t::MISSING_VALUE;
    }
    // Check if new value is valid in supportedValue list
    if( !isValueInArray( subwooferPolarity.value(),
                         m_audioSettings[kAudioSettingValues][kSubwooferPolarityName][kProperties]["supportedPolarity"] ) )
    {
        return ResultCode_t::INVALID_VALUE;
    }
    //Check if value changed from last time
    if( m_currentSubwooferPolarity.value() == subwooferPolarity.value() )
    {
        BOSE_INFO( s_logger, "SubwooferPolarity value doesn't change from last time (%s)", subwooferPolarity.DebugString().c_str() );
        return ResultCode_t::VALUE_UNCHANGED;
    }
    m_audioSettings[kAudioSettingValues][kSubwooferPolarityName]["values"][kPersistGlobal] = subwooferPolarity.value();
    m_currentSubwooferPolarity.set_value( subwooferPolarity.value() );
    PersistAudioSettings();
    return ResultCode_t::NO_ERROR;
}
const AudioSubwooferPolarity& CustomAudioSettingsManager::GetSubwooferPolarity() const
{
    BOSE_DEBUG( s_logger, __func__ );
    return m_currentSubwooferPolarity;
}


///////////////////////////////////////////////////////////
/// CustomAudioSettingsManager::UpdateAllProtos
///     make sure protos are up to date with latest m_AudioSettings
/////////////////////////////////////////////////////////
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

    //dualMonoSelect proto has different format than above, has to be specially handled
    string persistLevel;
    const Json::Value& dualMonoSelect = m_audioSettings["audioSettingValues"][kDualMonoSelectName];
    if( dualMonoSelect["persistenceLevel"]["persistenceSession"] == true )
    {
        persistLevel = kPersistSession;
    }
    else
    {
        persistLevel = dualMonoSelect["persistenceLevel"]["currentPersistenceLevel"].asString();
    }
    const Json::Value& currValue = GetCurrentSettingValue( kDualMonoSelectName, persistLevel );
    if( m_currentDualMonoSelect.value() != JsonToProtoField( m_currentDualMonoSelect.value(), currValue ) )
    {
        m_currentDualMonoSelect.set_value( JsonToProtoField( m_currentDualMonoSelect.value(), currValue ) );
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
                 !persistedAudioSettings.isMember( kAudioSettingValues ) )
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
                m_audioSettings[kAudioSettingValues][propName] = persistedAudioSettings[kAudioSettingValues][propName];
            }
            for( uint32_t i = 0; i < persistedAudioSettings["supportedAudioSettings"]["contentItemNonSensitive"].size(); i++ )
            {
                string propName = persistedAudioSettings["supportedAudioSettings"]["contentItemNonSensitive"][i].asString();
                m_audioSettings[kAudioSettingValues][propName] = persistedAudioSettings[kAudioSettingValues][propName];
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
    m_audioSettings[kAudioSettingValues][kModeName]["persistenceLevel"]["persistenceSession"] = false;
    //Fill out protobuf
    std::string currPersistLvlMode = m_audioSettings[kAudioSettingValues][kModeName]["persistenceLevel"]["currentPersistenceLevel"].asString();
    m_currentMode.set_persistence( currPersistLvlMode );
    if( currPersistLvlMode == kPersistContentItem )
    {
        m_currentMode.set_value( JsonToProtoField( m_currentMode.value(), m_audioSettings[kAudioSettingValues][kModeName]["values"][kPersistContentItem][m_currentContentItem] ) );
    }
    else
    {
        m_currentMode.set_value( JsonToProtoField( m_currentMode.value(), m_audioSettings[kAudioSettingValues][kModeName]["values"][currPersistLvlMode] ) );
    }
    for( uint32_t i = 0; i < m_audioSettings[kAudioSettingValues][kModeName][kProperties]["supportedValues"].size(); i++ )
    {
        m_currentMode.mutable_properties()->add_supportedvalues( m_audioSettings[kAudioSettingValues][kModeName][kProperties]["supportedValues"][i].asString() );
    }
    for( uint32_t i = 0; i < m_audioSettings[kAudioSettingValues][kModeName][kProperties]["supportedPersistence"].size(); i++ )
    {
        m_currentMode.mutable_properties()->add_supportedpersistence( m_audioSettings[kAudioSettingValues][kModeName][kProperties]["supportedPersistence"][i].asString() );
    }

    ///
    /// initialize contentType proto
    ///
    m_audioSettings[kAudioSettingValues][kContentTypeName]["persistenceLevel"]["persistenceSession"] = false;
    //Fill out protobuf
    std::string currPersistLvlContentType = m_audioSettings[kAudioSettingValues][kContentTypeName]["persistenceLevel"]["currentPersistenceLevel"].asString();
    m_currentContentType.set_persistence( currPersistLvlContentType );
    if( currPersistLvlContentType == kPersistContentItem )
    {
        m_currentContentType.set_value( JsonToProtoField( m_currentContentType.value(), m_audioSettings[kAudioSettingValues][kContentTypeName]["values"][kPersistContentItem][m_currentContentItem] ) );
    }
    else
    {
        m_currentContentType.set_value( JsonToProtoField( m_currentContentType.value(), m_audioSettings[kAudioSettingValues][kContentTypeName]["values"][currPersistLvlContentType] ) );
    }
    for( uint32_t i = 0; i < m_audioSettings[kAudioSettingValues][kContentTypeName][kProperties]["supportedValues"].size(); i++ )
    {
        m_currentContentType.mutable_properties()->add_supportedvalues( m_audioSettings[kAudioSettingValues][kContentTypeName][kProperties]["supportedValues"][i].asString() );
    }
    for( uint32_t i = 0; i < m_audioSettings[kAudioSettingValues][kContentTypeName][kProperties]["supportedPersistence"].size(); i++ )
    {
        m_currentContentType.mutable_properties()->add_supportedpersistence( m_audioSettings[kAudioSettingValues][kContentTypeName][kProperties]["supportedPersistence"][i].asString() );
    }

    ///
    /// initialize dualMonoSelect proto
    ///
    m_audioSettings[kAudioSettingValues][kDualMonoSelectName]["persistenceLevel"]["persistenceSession"] = false;
    //Fill out protobuf
    m_currentDualMonoSelect.set_value( JsonToProtoField( m_currentDualMonoSelect.value(), m_audioSettings[kAudioSettingValues][kDualMonoSelectName]["values"][kPersistGlobal] ) );
    for( uint32_t i = 0; i < m_audioSettings[kAudioSettingValues][kDualMonoSelectName][kProperties]["supportedValues"].size(); i++ )
    {
        m_currentDualMonoSelect.mutable_properties()->add_supportedvalues( m_audioSettings[kAudioSettingValues][kDualMonoSelectName][kProperties]["supportedValues"][i].asString() );
    }

    ///
    /// initialize eqSelect proto
    ///
    m_audioSettings[kAudioSettingValues][kEqSelectName]["persistenceLevel"]["persistenceSession"] = false;
    //Fill out protobuf
    m_currentEqSelect.set_mode( JsonToProtoField( m_currentEqSelect.mode(), m_audioSettings[kAudioSettingValues][kEqSelectName]["values"][kPersistGlobal] ) );
    m_currentEqSelect.set_storedeqvalid( false );
    for( uint32_t i = 0; i < m_audioSettings[kAudioSettingValues][kEqSelectName][kProperties]["supportedModes"].size(); i++ )
    {
        const string& supportedMode = m_audioSettings[kAudioSettingValues][kEqSelectName][kProperties]["supportedModes"][i].asString();
        if( supportedMode == AudioEqSelect_supportedMode_Name( AudioEqSelect_supportedMode_EQ_AIQ_A ) )
        {
            m_currentEqSelect.set_storedeqvalid( true );
        }
        m_currentEqSelect.mutable_properties()->add_supportedmodes( supportedMode );
    }

    ///
    /// initialize subwooferPolarity proto
    ///
    m_audioSettings[kAudioSettingValues][kSubwooferPolarityName]["persistenceLevel"]["persistenceSession"] = false;
    //Fill out protobuf
    m_currentSubwooferPolarity.set_value( JsonToProtoField( m_currentSubwooferPolarity.value(), m_audioSettings[kAudioSettingValues][kSubwooferPolarityName]["values"][kPersistGlobal] ) );
    for( uint32_t i = 0; i < m_audioSettings[kAudioSettingValues][kSubwooferPolarityName][kProperties]["supportedPolarity"].size(); i++ )
    {
        m_currentSubwooferPolarity.mutable_properties()->add_supportedpolarity( m_audioSettings[kAudioSettingValues][kSubwooferPolarityName][kProperties]["supportedPolarity"][i].asString() );
    }
}

}// namespace ProductApp
