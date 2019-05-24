///////////////////////////////////////////////////////////////////////////////////////////////////
/// @file   CustomProductAudioService.cpp
/// @brief  This file contains source code for product specific behavior for
///         communicating with APProduct Server and APProduct related FrontDoor
///         interaction
///
/// Copyright 2017 Bose Corporation
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "AsyncCallback.h"
#include "APProductFactory.h"
#include "DPrint.h"
#include "FrontDoorClient.h"
#include "CustomProductController.h"
#include "CustomProductAudioService.h"
#include "LpmClientFactory.h"
#include "SoundTouchInterface/ContentItem.pb.h"

using namespace std::placeholders;

static DPrint s_logger( "CustomProductAudioService" );

constexpr char kBassEndPoint            [] = "/audio/bass";
constexpr char kCenterEndPoint          [] = "/audio/center";
constexpr char kModeEndPoint            [] = "/audio/mode";
constexpr char kTrebleEndPoint          [] = "/audio/treble";

namespace ProductApp
{

CustomProductAudioService::CustomProductAudioService( CustomProductController& productController,
                                                      const FrontDoorClientIF_t& frontDoorClient,
                                                      LpmClientIF::LpmClientPtr lpmClient ):
    ProductAudioService( productController.GetTask( ),
                         productController.GetMessageHandler(),
                         frontDoorClient ),
    m_audioSettingsMgr( std::unique_ptr<CustomAudioSettingsManager>( new CustomAudioSettingsManager() ) ),
    m_thermalTask( lpmClient, productController.GetTask( ),
                   AsyncCallback<IpcSystemTemperatureData_t>(
                       std::bind( &CustomProductAudioService::ThermalDataReceivedCb, this, _1 ), productController.GetTask( ) ) ),
    m_dataCollectionClient( productController.GetDataCollectionClient() )
{
    BOSE_DEBUG( s_logger, __func__ );
}

CustomProductAudioService::~CustomProductAudioService()
{
}

void CustomProductAudioService::RegisterAudioPathEvents()
{
    BOSE_DEBUG( s_logger, __func__ );

    // Initialize m_MainStreamAudioSettings with current audio settings value from AudioSettingsManager
    // thermalData will be updated by thermal task periodically in a separate route
    FetchLatestAudioSettings();

    m_APPointer = APProductFactory::Create( "ProductAudioService-APProduct", m_ProductTask );
    RegisterCommonAudioPathEvents();

    {
        Callback< std::string, Callback< std::string, std::string > > callback( std::bind( &CustomProductAudioService::GetMainStreamAudioSettingsCallback,
                                                                                this, _1, _2 ) );
        m_APPointer->RegisterForMainStreamAudioSettingsRequest( callback );
    }
    ConnectToAudioPath();
}

void CustomProductAudioService::RegisterFrontDoorEvents()
{
    BOSE_DEBUG( s_logger, __func__ );

    //
    // Endpoint /audio/bass - register ProductController as handler for POST/PUT/GET requests
    //
    auto getBassAction = [this]( )
    {
        return m_audioSettingsMgr->GetBass( );
    };
    auto setBassAction = [this]( ProductPb::AudioBassLevel val )
    {
        AudioSettingResultCode::ResultCode_t error = m_audioSettingsMgr->SetBass( val );
        if( error == AudioSettingResultCode::ResultCode_t::NO_ERROR )
        {
            m_mainStreamAudioSettings.set_basslevel( m_audioSettingsMgr->GetBass( ).value() );
            SendMainStreamAudioSettingsEvent();
        }
        return error;
    };
    auto refreshBassAction = [this]( )
    {
        return m_audioSettingsMgr->RefreshBass( );
    };
    m_audioBassSetting = std::unique_ptr<AudioSetting<ProductPb::AudioBassLevel>>( new AudioSetting<ProductPb::AudioBassLevel>
                                                                                   ( kBassEndPoint,
                                                                                           getBassAction,
                                                                                           setBassAction,
                                                                                           refreshBassAction,
                                                                                           m_FrontDoorClientIF,
                                                                                           m_ProductTask,
                                                                                           FRONTDOOR_PRODUCT_CONTROLLER_VERSION,
                                                                                           FRONTDOOR_PRODUCT_CONTROLLER_GROUP_NAME,
                                                                                           "bass-changed",
                                                                                           m_dataCollectionClient ) );

    //
    // Endpoint /audio/center - register ProductController as handler for POST/PUT/GET requests
    //
    if( m_audioSettingsMgr->IsCenterLevelTestEnabled() ) // Future feature. Disabled Frontdoor by default.
    {
        auto getCenterAction = [this]( )
        {
            return m_audioSettingsMgr->GetCenter( );
        };
        auto setCenterAction = [this]( ProductPb::AudioCenterLevel val )
        {
            AudioSettingResultCode::ResultCode_t error = m_audioSettingsMgr->SetCenter( val );
            if( error == AudioSettingResultCode::ResultCode_t::NO_ERROR )
            {
                m_mainStreamAudioSettings.set_centerlevel( m_audioSettingsMgr->GetCenter( ).value() );
                SendMainStreamAudioSettingsEvent();
            }
            return error;
        };
        auto refreshCenterAction = [this]( )
        {
            return m_audioSettingsMgr->RefreshCenter( );
        };
        m_audioCenterSetting = std::unique_ptr<AudioSetting<ProductPb::AudioCenterLevel>>( new AudioSetting<ProductPb::AudioCenterLevel>
                               ( kCenterEndPoint,
                                 getCenterAction,
                                 setCenterAction,
                                 refreshCenterAction,
                                 m_FrontDoorClientIF,
                                 m_ProductTask,
                                 FRONTDOOR_PRODUCT_CONTROLLER_VERSION,
                                 FRONTDOOR_PRODUCT_CONTROLLER_GROUP_NAME,
                                 "center-level-changed",
                                 m_dataCollectionClient ) );
    }

    //
    // Endpoint /audio/mode - register ProductController as handler for POST/PUT/GET requests
    //
    if( m_audioSettingsMgr->IsModeTestEnabled() ) // Future feature. Disabled Frontdoor by default.
    {
        auto getModeAction = [ this ]( )
        {
            return m_audioSettingsMgr->GetMode( );
        };
        auto setModeAction = [ this ]( ProductPb::AudioMode val )
        {
            // /audio/mode's persistence level is default to "CONTENT_ITEM"
            if( !val.has_persistence() )
            {
                val.set_persistence( kPersistContentItem );
            }
            AudioSettingResultCode::ResultCode_t error = m_audioSettingsMgr->SetMode( val );
            if( error == AudioSettingResultCode::ResultCode_t::NO_ERROR )
            {
                m_mainStreamAudioSettings.set_audiomode( ModeNameToEnum( m_audioSettingsMgr->GetMode( ).value() ) );
                SendMainStreamAudioSettingsEvent();
            }
            return error;
        };
        auto refreshModeAction = [this]( )
        {
            return m_audioSettingsMgr->RefreshMode( );
        };
        m_audioModeSetting = std::unique_ptr<AudioSetting<ProductPb::AudioMode>>( new AudioSetting<ProductPb::AudioMode>
                                                                                  ( kModeEndPoint,
                                                                                    getModeAction,
                                                                                    setModeAction,
                                                                                    refreshModeAction,
                                                                                    m_FrontDoorClientIF,
                                                                                    m_ProductTask,
                                                                                    FRONTDOOR_PRODUCT_CONTROLLER_VERSION,
                                                                                    FRONTDOOR_PRODUCT_CONTROLLER_GROUP_NAME,
                                                                                    "mode-changed",
                                                                                    m_dataCollectionClient ) );
    }

    //
    // Endpoint /audio/treble - register ProductController as handler for POST/PUT/GET requests
    //
    auto getTrebleAction = [ this ]( )
    {
        return m_audioSettingsMgr->GetTreble( );
    };
    auto setTrebleAction = [ this ]( ProductPb::AudioTrebleLevel val )
    {
        AudioSettingResultCode::ResultCode_t error = m_audioSettingsMgr->SetTreble( val );
        if( error == AudioSettingResultCode::ResultCode_t::NO_ERROR )
        {
            m_mainStreamAudioSettings.set_treblelevel( m_audioSettingsMgr->GetTreble( ).value() );
            SendMainStreamAudioSettingsEvent();
        }
        return error;
    };
    auto refreshTrebleAction = [this]( )
    {
        return m_audioSettingsMgr->RefreshTreble( );
    };
    m_audioTrebleSetting = std::unique_ptr<AudioSetting<ProductPb::AudioTrebleLevel>>( new AudioSetting<ProductPb::AudioTrebleLevel>
                           ( kTrebleEndPoint,
                             getTrebleAction,
                             setTrebleAction,
                             refreshTrebleAction,
                             m_FrontDoorClientIF,
                             m_ProductTask,
                             FRONTDOOR_PRODUCT_CONTROLLER_VERSION,
                             FRONTDOOR_PRODUCT_CONTROLLER_GROUP_NAME,
                             "treble-changed",
                             m_dataCollectionClient ) );
}

void CustomProductAudioService::GetMainStreamAudioSettingsCallback( std::string contentItem, const Callback<std::string, std::string> cb )
{
    BOSE_DEBUG( s_logger, "%s - contentItem = %s", __func__, contentItem.c_str() );

    // Parse contentItem string received from APProduct
    bool error = false;
    SoundTouchInterface::ContentItem contentItemProto;

    try
    {
        ProtoToMarkup::FromJson( contentItem, &contentItemProto );
    }
    catch( const ProtoToMarkup::MarkupError &e )
    {
        BOSE_ERROR( s_logger, "Converting contentItem string from APProduct to ContentItem proto failed markup error - %s", e.what() );
        error = true;
    }
    // If no parsing error occured, update m_MainStreamAudioSettings and m_InputRoute with new contentItem
    if( !error && contentItemProto.has_source() && contentItemProto.has_sourceaccount() )
    {
        // Update audio settings
        BOSE_DEBUG( s_logger, "GetMainStreamAudioSettingsCallback, source = %s, sourecAccount = %s", contentItemProto.source().c_str(), contentItemProto.sourceaccount().c_str() );
        /// UpdateContentItem will re-populate the AudioSetting value and persistence level caused by content item change
        /// in m_AudioSetting Json and m_currentXX proto,
        /// and if value is changed, it will also send new values to frontdoor(notification) and data collection service
        bool isChanged = false;
        isChanged = ( m_audioSettingsMgr->UpdateContentItem( contentItemProto, kBassName,   m_audioBassSetting ) ) ? true : isChanged;
        isChanged = ( m_audioSettingsMgr->UpdateContentItem( contentItemProto, kTrebleName, m_audioTrebleSetting ) ) ? true : isChanged;
        if( m_audioSettingsMgr->IsCenterLevelTestEnabled() ) // Future feature. Disabled Frontdoor by default.
        {
            isChanged = ( m_audioSettingsMgr->UpdateContentItem( contentItemProto, kCenterName, m_audioCenterSetting ) ) ? true : isChanged;
        }
        if( m_audioSettingsMgr->IsModeTestEnabled() ) // Future feature. Disabled Frontdoor by default.
        {
            isChanged = ( m_audioSettingsMgr->UpdateContentItem( contentItemProto, kModeName,   m_audioModeSetting ) ) ? true : isChanged;
        }
        if( isChanged )
        {
            FetchLatestAudioSettings();
            m_audioSettingsMgr->PersistAudioSettings();
        }
    }
    else
    {
        BOSE_ERROR( s_logger, "ContentItem string from APProduct doesn't contain \"source\" or \"sourceAccount\" field" );
    }

    std::string mainStreamAudioSettings = ProtoToMarkup::ToJson( m_mainStreamAudioSettings, true );
    // DMR Using empty "input route" (second param) since Eddie does not use this.
    cb.Send( mainStreamAudioSettings, "" );
}

void CustomProductAudioService::SendMainStreamAudioSettingsEvent()
{
    std::string mainStreamAudioSettings = ProtoToMarkup::ToJson( m_mainStreamAudioSettings, true );
    m_APPointer->SetMainStreamAudioSettings( mainStreamAudioSettings );
    // DMR Print out the JSON being sent to AP.
    BOSE_INSANE( s_logger, "SendMainStreamAudioSettingsEvent %s", mainStreamAudioSettings.c_str() );
}

void CustomProductAudioService::FetchLatestAudioSettings( )
{
    m_mainStreamAudioSettings.set_basslevel( m_audioSettingsMgr->GetBass( ).value() );
    m_mainStreamAudioSettings.set_centerlevel( m_audioSettingsMgr->GetCenter( ).value() );
    m_mainStreamAudioSettings.set_audiomode( ModeNameToEnum( m_audioSettingsMgr->GetMode( ).value() ) );
    m_mainStreamAudioSettings.set_treblelevel( m_audioSettingsMgr->GetTreble( ).value() );
}

void CustomProductAudioService::ThermalDataReceivedCb( IpcSystemTemperatureData_t data )
{
    int16_t ampTemp = INT16_MAX;
    IpcThermalType_t thermalValueType;

    // DMR Data comes in as callback argument but it is easier to us the accessor
    // since it does all the safety checks.
    m_thermalTask.GetThermalValue( ampTemp, thermalValueType, IPC_THERMAL_LOCATION_CONSOLE_INTERNAL_AMP );

    // The DSP explictly only wants temperature values in valid ranges.
    // If we detect an invalid (non-existant) thermal reading, do not add it to the stream config.
    if( ampTemp == INT16_MAX )
    {
        m_mainStreamAudioSettings.clear_thermaldata();
    }
    else
    {
        if( m_mainStreamAudioSettings.thermaldata_size() == 0 )
        {
            m_mainStreamAudioSettings.add_thermaldata( ampTemp );
        }
        else
        {
            m_mainStreamAudioSettings.set_thermaldata( 0, ampTemp );
        }

        SendMainStreamAudioSettingsEvent();
    }
}

void CustomProductAudioService::SetThermalMonitorEnabled( bool enabled )
{
    if( enabled )
    {
        m_thermalTask.Start();
    }
    else
    {
        m_thermalTask.Stop();
    }
}

PRODUCT_AUDIO_SETTINGS( _AudioMode ) CustomProductAudioService::ModeNameToEnum( const std::string& modeName )
{
    if( modeName == "DIALOG" )
    {
        return PRODUCT_AUDIO_SETTINGS( _AudioMode_AUDIOSETTINGS_AUDIO_MODE_DIALOG );
    }
    else if( modeName == "DIRECT" )
    {
        return PRODUCT_AUDIO_SETTINGS( _AudioMode_AUDIOSETTINGS_AUDIO_MODE_DIRECT );
    }
    else if( modeName == "NIGHT" )
    {
        return PRODUCT_AUDIO_SETTINGS( _AudioMode_AUDIOSETTINGS_AUDIO_MODE_NIGHT );
    }
    else
    {
        return PRODUCT_AUDIO_SETTINGS( _AudioMode_AUDIOSETTINGS_AUDIO_MODE_NORMAL );
    }
}

}// namespace ProductApp
