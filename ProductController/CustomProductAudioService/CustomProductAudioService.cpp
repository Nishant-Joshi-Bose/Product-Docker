///////////////////////////////////////////////////////////////////////////////////////////////////
/// @file   CustomProductAudioService.cpp
/// @brief   This file contains source code for Eddie specific behavior for
///         communicating with APProduct Server and APProduct related FrontDoor interaction
/// Copyright 2017 Bose Corporation
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "AsyncCallback.h"
#include "APProductFactory.h"
#include "DPrint.h"
#include "FrontDoorClient.h"
#include "EddieProductController.h"
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
/*!
 */
CustomProductAudioService::CustomProductAudioService( EddieProductController& productController,
                                                      const FrontDoorClientIF_t& frontDoorClient,
                                                      LpmClientIF::LpmClientPtr lpmClient ):
    ProductAudioService( productController.GetTask( ),
                         productController.GetMessageHandler() ,
                         frontDoorClient ),
    m_audioSettingsMgr( std::unique_ptr<CustomAudioSettingsManager>( new CustomAudioSettingsManager() ) ),
    m_thermalTask( lpmClient, std::bind( &CustomProductAudioService::ThermalDataReceivedCb, this, _1 ) )
{
    BOSE_DEBUG( s_logger, __func__ );
}

/*!
 */
CustomProductAudioService::~CustomProductAudioService()
{

}

/*!
 */
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
                                                                                this,
                                                                                std::placeholders::_1,
                                                                                std::placeholders::_2 ) );
        m_APPointer->RegisterForMainStreamAudioSettingsRequest( callback );
    }
    ConnectToAudioPath();
}

/*!
 */
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
        ResultCode_t error = m_audioSettingsMgr->SetBass( val );
        if( error == ResultCode_t::NO_ERROR )
        {
            m_mainStreamAudioSettings.set_basslevel( m_audioSettingsMgr->GetBass( ).value() );
            SendMainStreamAudioSettingsEvent();
        }
        return error;
    };
    m_audioBassSetting = std::unique_ptr<AudioSetting<ProductPb::AudioBassLevel>>( new AudioSetting<ProductPb::AudioBassLevel>
                                                                                   ( kBassEndPoint,
                                                                                           getBassAction,
                                                                                           setBassAction,
                                                                                           m_FrontDoorClientIF,
                                                                                           m_ProductTask,
                                                                                           FRONTDOOR_PRODUCT_CONTROLLER_VERSION,
                                                                                           FRONTDOOR_PRODUCT_CONTROLLER_GROUP_NAME ) );

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
            ResultCode_t error = m_audioSettingsMgr->SetCenter( val );
            if( error == ResultCode_t::NO_ERROR )
            {
                m_mainStreamAudioSettings.set_centerlevel( m_audioSettingsMgr->GetCenter( ).value() );
                SendMainStreamAudioSettingsEvent();
            }
            return error;
        };
        m_audioCenterSetting = std::unique_ptr<AudioSetting<ProductPb::AudioCenterLevel>>( new AudioSetting<ProductPb::AudioCenterLevel>
                               ( kCenterEndPoint,
                                 getCenterAction,
                                 setCenterAction,
                                 m_FrontDoorClientIF,
                                 m_ProductTask,
                                 FRONTDOOR_PRODUCT_CONTROLLER_VERSION,
                                 FRONTDOOR_PRODUCT_CONTROLLER_GROUP_NAME ) );
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
            ResultCode_t error = m_audioSettingsMgr->SetMode( val );
            if( error == ResultCode_t::NO_ERROR )
            {
                m_mainStreamAudioSettings.set_audiomode( ModeNameToEnum( m_audioSettingsMgr->GetMode( ).value() ) );
                SendMainStreamAudioSettingsEvent();
            }
            return error;
        };
        m_audioModeSetting = std::unique_ptr<AudioSetting<ProductPb::AudioMode>>( new AudioSetting<ProductPb::AudioMode>
                                                                                  ( kModeEndPoint,
                                                                                    getModeAction,
                                                                                    setModeAction,
                                                                                    m_FrontDoorClientIF,
                                                                                    m_ProductTask,
                                                                                    FRONTDOOR_PRODUCT_CONTROLLER_VERSION,
                                                                                    FRONTDOOR_PRODUCT_CONTROLLER_GROUP_NAME ) );
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
        ResultCode_t error = m_audioSettingsMgr->SetTreble( val );
        if( error == ResultCode_t::NO_ERROR )
        {
            m_mainStreamAudioSettings.set_treblelevel( m_audioSettingsMgr->GetTreble( ).value() );
            SendMainStreamAudioSettingsEvent();
        }
        return error;
    };
    m_audioTrebleSetting = std::unique_ptr<AudioSetting<ProductPb::AudioTrebleLevel>>( new AudioSetting<ProductPb::AudioTrebleLevel>
                           ( kTrebleEndPoint,
                             getTrebleAction,
                             setTrebleAction,
                             m_FrontDoorClientIF,
                             m_ProductTask,
                             FRONTDOOR_PRODUCT_CONTROLLER_VERSION,
                             FRONTDOOR_PRODUCT_CONTROLLER_GROUP_NAME ) );
}

/*!
 */
void CustomProductAudioService::GetMainStreamAudioSettingsCallback( std::string contentItem, const Callback<std::string, std::string> cb )
{
    //BOSE_DEBUG( s_logger, __func__ );
    //BOSE_DEBUG( s_logger, "GetMainStreamAudioSettingsCallback - contentItem = %s", contentItem.c_str() );

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

        m_audioSettingsMgr->UpdateContentItem( contentItemProto );
        FetchLatestAudioSettings();
    }
    else
    {
        BOSE_ERROR( s_logger, "ContentItem string from APProduct doesn't contain \"source\" or \"sourceAccount\" field" );
    }

    std::string mainStreamAudioSettings = ProtoToMarkup::ToJson( m_mainStreamAudioSettings, true );
    // DMR Using empty "input route" (second param) since Eddie does not use this.
    cb.Send( mainStreamAudioSettings, "" );
}

/*!
 */
void CustomProductAudioService::SendMainStreamAudioSettingsEvent()
{
    std::string mainStreamAudioSettings = ProtoToMarkup::ToJson( m_mainStreamAudioSettings, true );
    m_APPointer->SetMainStreamAudioSettings( mainStreamAudioSettings );
    // DMR Print out the JSON being sent to AP.
    //BOSE_INFO( s_logger, "SendMainStreamAudioSettingsEvent %s", mainStreamAudioSettings.c_str() );
}

/*!
 */
void CustomProductAudioService::FetchLatestAudioSettings( )
{
    m_mainStreamAudioSettings.set_basslevel( m_audioSettingsMgr->GetBass( ).value() );
    m_mainStreamAudioSettings.set_centerlevel( m_audioSettingsMgr->GetCenter( ).value() );
    m_mainStreamAudioSettings.set_audiomode( ModeNameToEnum( m_audioSettingsMgr->GetMode( ).value() ) );
    m_mainStreamAudioSettings.set_treblelevel( m_audioSettingsMgr->GetTreble( ).value() );
}

/*!
 */
void CustomProductAudioService::ThermalDataReceivedCb( const IpcSystemTemperatureData_t& data )
{
    int16_t ampTemp = INT16_MAX;
    IpcThermalType_t thermalValueType;

    // DMR Data comes in as callback argument but it is easier to us the accessor
    // since it does all the safety checks.
    m_thermalTask.GetThermalValue( ampTemp, thermalValueType, IPC_THERMAL_LOCATION_CONSOLE_INTERNAL_AMP );

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

/*!
 */
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

/*!
 */
EddieAudioSettings_t_AudioMode CustomProductAudioService::ModeNameToEnum( const std::string& modeName )
{
    if( modeName == "dialog" )
    {
        return EddieAudioSettings_t_AudioMode_AUDIOSETTINGS_AUDIO_MODE_DIALOG;
    }
    else if( modeName == "direct" )
    {
        return EddieAudioSettings_t_AudioMode_AUDIOSETTINGS_AUDIO_MODE_DIRECT;
    }
    else if( modeName == "night" )
    {
        return EddieAudioSettings_t_AudioMode_AUDIOSETTINGS_AUDIO_MODE_NIGHT;
    }
    else
    {
        return EddieAudioSettings_t_AudioMode_AUDIOSETTINGS_AUDIO_MODE_NORMAL;
    }
}

}// namespace ProductApp
