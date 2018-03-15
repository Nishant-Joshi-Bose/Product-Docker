///////////////////////////////////////////////////////////////////////////////////////////////////
/// @file   CustomAudioSettingsManager.cpp
/// @brief  This file contains source code for setting and getting AudioSettings
///         such as bass, treble, center, surround, gainOffset, avSync, subwooferGain, mode, contentType, dualMonoSelect
/// Copyright 2017 Bose Corporation
////////////////////////////////////////////////////////////////////////////////////////////////////
#include <fstream>
#include "DPrint.h"
#include "SystemUtils.h"
#include "Utilities.h"
#include "CustomAudioSettingsManager.h"
#include "ProtoToMarkup.h"

constexpr char  kDefaultConfigPath[] = "/opt/Bose/etc/DefaultAudioSettings.json";
constexpr uint32_t kConfigVersionMajor = 2;
constexpr uint32_t kConfigVersionMinor = 1;

constexpr char kBassName            [] = "audioBassLevel";
constexpr char kTrebleName          [] = "audioTrebleLevel";
constexpr char kCenterName          [] = "audioCenterLevel";
constexpr char kSurroundName        [] = "audioSurroundLevel";
constexpr char kGainOffsetName      [] = "audioGainOffset";
constexpr char kAvSyncName          [] = "audioAvSync";
constexpr char kSubwooferGainName   [] = "audioSubwooferGain";
constexpr char kModeName            [] = "audioMode";
constexpr char kContentTypeName     [] = "audioContentType";
constexpr char kDualMonoSelectName  [] = "audioDualMonoSelect";
constexpr char kEqSelectName        [] = "audioEqSelect";

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
///     setter returns a ErrorCode_t which indicates any error during applying the setting
///     getter returns a protobuf of current bass value
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
/// Treble setting setter/getter
///     setter returns a ErrorCode_t which indicates any error during applying the setting
///     getter returns a protobuf of current treble value
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

////////////////////////////////////////////////////////////////////////////////////////
/// Center setting setter/getter
///     setter returns a ErrorCode_t which indicates any error during applying the setting
///     getter returns a protobuf of current center value
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
/// Surround setting setter/getter
///     setter returns a ErrorCode_t which indicates any error during applying the setting
///     getter returns a protobuf of current surround value
///////////////////////////////////////////////////////////////////////////////////////
ErrorCode_t CustomAudioSettingsManager::SetSurround( const ProductPb::AudioSurroundLevel& surround )
{
    BOSE_DEBUG( s_logger, __func__ );
    if( !surround.has_value() )
    {
        return ErrorCode_t::MISSING_FIELDS;
    }
    if( surround.value() > m_audioSettings["configurations"][kSurroundName]["properties"]["max"].asInt()
        || surround.value() < m_audioSettings["configurations"][kSurroundName]["properties"]["min"].asInt() )
    {
        return ErrorCode_t::INVALID_VALUE;
    }
    return SetAudioProperties( surround, kSurroundName, m_currentSurround );
}

const ProductPb::AudioSurroundLevel& CustomAudioSettingsManager::GetSurround() const
{
    BOSE_DEBUG( s_logger, __func__ );
    return m_currentSurround;
}

////////////////////////////////////////////////////////////////////////////////////////
/// GainOffset setting setter/getter
///     setter returns a ErrorCode_t which indicates any error during applying the setting
///     getter returns a protobuf of current gainOffset value
//////////////////////////////////////////////////////////////////////////////////////
ErrorCode_t CustomAudioSettingsManager::SetGainOffset( const ProductPb::AudioGainOffset& gainOffset )
{
    BOSE_DEBUG( s_logger, __func__ );
    if( !gainOffset.has_value() )
    {
        return ErrorCode_t::MISSING_FIELDS;
    }
    if( gainOffset.value() > m_audioSettings["configurations"][kGainOffsetName]["properties"]["max"].asInt()
        || gainOffset.value() < m_audioSettings["configurations"][kGainOffsetName]["properties"]["min"].asInt() )
    {
        return ErrorCode_t::INVALID_VALUE;
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
///     setter returns a ErrorCode_t which indicates any error during applying the setting
///     getter returns a protobuf of current avSync value
//////////////////////////////////////////////////////////////////////////////////////
ErrorCode_t CustomAudioSettingsManager::SetAvSync( const ProductPb::AudioAvSync& avSync )
{
    BOSE_DEBUG( s_logger, __func__ );
    if( !avSync.has_value() )
    {
        return ErrorCode_t::MISSING_FIELDS;
    }
    if( avSync.value() > m_audioSettings["configurations"][kAvSyncName]["properties"]["max"].asInt()
        || avSync.value() < m_audioSettings["configurations"][kAvSyncName]["properties"]["min"].asInt() )
    {
        return ErrorCode_t::INVALID_VALUE;
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
///     setter returns a ErrorCode_t which indicates any error during applying the setting
///     getter returns a protobuf of current subwooferGain value
//////////////////////////////////////////////////////////////////////////////////////
ErrorCode_t CustomAudioSettingsManager::SetSubwooferGain( const ProductPb::AudioSubwooferGain& subwooferGain )
{
    BOSE_DEBUG( s_logger, __func__ );
    if( !subwooferGain.has_value() )
    {
        return ErrorCode_t::MISSING_FIELDS;
    }
    if( subwooferGain.value() > m_audioSettings["configurations"][kSubwooferGainName]["properties"]["max"].asInt()
        || subwooferGain.value() < m_audioSettings["configurations"][kSubwooferGainName]["properties"]["min"].asInt() )
    {
        return ErrorCode_t::INVALID_VALUE;
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
///     setter returns a ErrorCode_t which indicates any error during applying the setting
///     getter returns a protobuf of current mode value
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
/// ContentType setting setter/getter
///     setter returns a ErrorCode_t which indicates any error during applying the setting
///     getter returns a protobuf of current contentType value
//////////////////////////////////////////////////////////////////////////////////////
ErrorCode_t CustomAudioSettingsManager::SetContentType( const ProductPb::AudioContentType& contentType )
{
    BOSE_DEBUG( s_logger, __func__ );
    if( !contentType.has_value() )
    {
        return ErrorCode_t::MISSING_FIELDS;
    }
    if( !isValueInArray( contentType.value(),
                         m_audioSettings["configurations"][kContentTypeName]["properties"]["supportedValues"] ) )
    {
        return ErrorCode_t::INVALID_VALUE;
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
///     setter returns a ErrorCode_t which indicates any error during applying the setting
///     getter returns a protobuf of current dualMonoSelect value
//////////////////////////////////////////////////////////////////////////////////////
ErrorCode_t CustomAudioSettingsManager::SetDualMonoSelect( const ProductPb::AudioDualMonoSelect& dualMonoSelect )
{
    BOSE_DEBUG( s_logger, __func__ );
    if( !dualMonoSelect.has_value() )
    {
        BOSE_INFO( s_logger, "DualMonoSelect doesn't contain any value (%s)", ProtoToMarkup::ToJson( dualMonoSelect ).c_str() );
        return ErrorCode_t::MISSING_FIELDS;
    }

    // Check if new value is valid in supportedValue list
    if( !isValueInArray( dualMonoSelect.value(),
                         m_audioSettings["configurations"][kDualMonoSelectName]["properties"]["supportedValues"] ) )
    {
        return ErrorCode_t::INVALID_VALUE;
    }

    //Check if value changed from last time
    if( m_currentDualMonoSelect.value() == dualMonoSelect.value() )
    {
        BOSE_INFO( s_logger, "DualMonoSelect value doesn't change from last time (%s)", ProtoToMarkup::ToJson( dualMonoSelect ).c_str() );
        return ErrorCode_t::VALUE_UNCHANGED;
    }
    m_audioSettings["values"][kPersistGlobal][kDualMonoSelectName] = dualMonoSelect.value();
    m_currentDualMonoSelect.set_value( dualMonoSelect.value() );
    PersistAudioSettings();
    return ErrorCode_t::NO_ERROR;
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
ErrorCode_t CustomAudioSettingsManager::SetEqSelect( const ProductPb::AudioEqSelect& eqSelect )
{
    BOSE_DEBUG( s_logger, __func__ );
    if( !eqSelect.has_mode() )
    {
        BOSE_INFO( s_logger, "EqSelect doesn't contain any value (%s)", ProtoToMarkup::ToJson( eqSelect ).c_str() );
        return ErrorCode_t::MISSING_FIELDS;
    }

    // Check if new value is valid in supportedValue list
    if( !isValueInArray( eqSelect.mode(),
                         m_audioSettings["configurations"][kEqSelectName]["properties"]["supportedModes"] ) )
    {
        return ErrorCode_t::INVALID_VALUE;
    }

    //Check if value changed from last time
    if( m_currentEqSelect.mode() == eqSelect.mode() )
    {
        BOSE_INFO( s_logger, "EqSelect value doesn't change from last time (%s)", ProtoToMarkup::ToJson( eqSelect ).c_str() );
        return ErrorCode_t::VALUE_UNCHANGED;
    }
    m_audioSettings["values"][kPersistGlobal][kEqSelectName] = eqSelect.mode();
    m_currentEqSelect.set_mode( eqSelect.mode() );
    PersistAudioSettings();
    return ErrorCode_t::NO_ERROR;
}
const ProductPb::AudioEqSelect& CustomAudioSettingsManager::GetEqSelect() const
{
    BOSE_DEBUG( s_logger, __func__ );
    return m_currentEqSelect;
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
    initializeProto( kSubwooferGainName, m_currentSubwooferGain );
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

    m_currentEqSelect.set_mode( m_audioSettings["defaultValues"][kEqSelectName].asString() );
    for( uint32_t i = 0; i < m_audioSettings["configurations"][kEqSelectName]["properties"]["supportedModes"].size(); i++ )
    {
        m_currentEqSelect.mutable_properties()->add_supportedmodes( m_audioSettings["configurations"][kEqSelectName]["properties"]["supportedModes"][i].asString() );
    }
}

}// namespace ProductApp
