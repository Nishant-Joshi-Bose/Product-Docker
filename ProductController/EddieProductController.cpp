////////////////////////////////////////////////////////////////////////////////
/// @file   EddieProductController.cpp
/// @brief  Eddie product controller class.
///
/// @attention Copyright 2017 Bose Corporation, Framingham, MA
////////////////////////////////////////////////////////////////////////////////

#include "EddieProductController.h"
#include "ProductControllerStates.h"
#include "CustomProductControllerState.h"
#include "CustomProductAudioService.h"
#include "APTaskFactory.h"
#include "AsyncCallback.h"
#include "ProtoToMarkup.h"
#include "ProtoPersistenceFactory.h"
#include "LpmClientFactory.h"
#include "CLICmdsKeys.h"
#include "BluetoothSinkEndpoints.h"
#include "EndPointsDefines.h"
#include "CustomProductHardwareInterface.h"

//#include "ButtonPress.pb.h" // @TODO Leela, re-enable this code

static DPrint s_logger( "EddieProductController" );

using namespace DemoApp;

namespace ProductApp
{
const std::string g_ProductPersistenceDir = "product-persistence/";
const std::string KEY_CONFIG_FILE = "/var/run/KeyConfiguration.json";

EddieProductController::EddieProductController( std::string const& ProductName ):
    ProductController( ProductName ),
    m_ProductControllerStateTop( GetHsm(), nullptr ),
    m_CustomProductControllerStateBooting( GetHsm(), &m_ProductControllerStateTop, CUSTOM_PRODUCT_CONTROLLER_STATE_BOOTING ),
    m_CustomProductControllerStateSetup( GetHsm(), &m_ProductControllerStateTop, CUSTOM_PRODUCT_CONTROLLER_STATE_SETUP ),
    m_CustomProductControllerStateOn( GetHsm(), &m_ProductControllerStateTop, CUSTOM_PRODUCT_CONTROLLER_STATE_ON ),
    m_ProductControllerStateLowPowerStandby( GetHsm(), &m_ProductControllerStateTop, PRODUCT_CONTROLLER_STATE_LOW_POWER_STANDBY ),
    m_ProductControllerStateSwUpdating( GetHsm(), &m_ProductControllerStateTop, PRODUCT_CONTROLLER_STATE_SOFTWARE_UPDATING ),
    m_ProductControllerStateCriticalError( GetHsm(), &m_ProductControllerStateTop, PRODUCT_CONTROLLER_STATE_CRITICAL_ERROR ),
    m_ProductControllerStateRebooting( GetHsm(), &m_ProductControllerStateTop, PRODUCT_CONTROLLER_STATE_REBOOTING ),
    m_ProductControllerStatePlaying( GetHsm(), &m_CustomProductControllerStateOn, PRODUCT_CONTROLLER_STATE_PLAYING ),
    m_ProductControllerStatePlayable( GetHsm(), &m_CustomProductControllerStateOn, PRODUCT_CONTROLLER_STATE_PLAYABLE ),
    m_ProductControllerStateLowPowerStandbyTransition( GetHsm(), &m_ProductControllerStateLowPowerStandby, PRODUCT_CONTROLLER_STATE_LOW_POWER_STANDBY_TRANSITION ),
    m_ProductControllerStatePlayingActive( GetHsm(), &m_ProductControllerStatePlaying, PRODUCT_CONTROLLER_STATE_PLAYING_ACTIVE ),
    m_ProductControllerStatePlayingInactive( GetHsm(), &m_ProductControllerStatePlaying, PRODUCT_CONTROLLER_STATE_PLAYING_INACTIVE ),
    m_ProductControllerStateIdle( GetHsm(), &m_ProductControllerStatePlayable, PRODUCT_CONTROLLER_STATE_IDLE ),
    m_ProductControllerStateNetworkStandby( GetHsm(), &m_ProductControllerStatePlayable, PRODUCT_CONTROLLER_STATE_NETWORK_STANDBY ),
    m_ProductControllerStateVoiceConfigured( GetHsm(), &m_ProductControllerStateIdle, PRODUCT_CONTROLLER_STATE_IDLE_VOICE_CONFIGURED ),
    m_ProductControllerStateVoiceNotConfigured( GetHsm(), &m_ProductControllerStateIdle, PRODUCT_CONTROLLER_STATE_IDLE_VOICE_NOT_CONFIGURED ),
    m_ProductControllerStateNetworkConfigured( GetHsm(), &m_ProductControllerStateNetworkStandby, PRODUCT_CONTROLLER_STATE_NETWORK_STANDBY_CONFIGURED ),
    m_ProductControllerStateNetworkNotConfigured( GetHsm(), &m_ProductControllerStateNetworkStandby, PRODUCT_CONTROLLER_STATE_NETWORK_STANDBY_NOT_CONFIGURED ),
    m_KeyHandler( *GetTask(), m_CliClientMT, KEY_CONFIG_FILE ),
    m_cachedStatus(),
    m_IntentHandler( *GetTask(), m_CliClientMT, m_FrontDoorClientIF, *this ),
    m_wifiProfilesCount(),
    errorCb( AsyncCallback<FRONT_DOOR_CLIENT_ERRORS> ( std::bind( &EddieProductController::CallbackError,
                                                                  this, std::placeholders::_1 ), GetTask() ) ),
    m_demoController( m_ProductControllerTask, m_KeyHandler, g_ProductPersistenceDir ),
    m_DataCollectionClient( "EddieProductController" ),
    m_voiceServiceClient( ProductName, m_FrontDoorClientIF ),
    m_LpmInterface( std::make_shared< CustomProductHardwareInterface >( *this ) )
{
    BOSE_INFO( s_logger, __func__ );
    m_deviceManager.Initialize( this );
    /// Add States to HSM object and initialize HSM before doing anything else.
    GetHsm().AddState( &m_ProductControllerStateTop );
    GetHsm().AddState( &m_ProductControllerStateLowPowerStandby );
    GetHsm().AddState( &m_ProductControllerStateSwUpdating );
    GetHsm().AddState( &m_CustomProductControllerStateBooting );
    GetHsm().AddState( &m_CustomProductControllerStateSetup );
    GetHsm().AddState( &m_CustomProductControllerStateOn );
    GetHsm().AddState( &m_ProductControllerStateCriticalError );
    GetHsm().AddState( &m_ProductControllerStateRebooting );
    GetHsm().AddState( &m_ProductControllerStatePlaying );
    GetHsm().AddState( &m_ProductControllerStatePlayable );
    GetHsm().AddState( &m_ProductControllerStateLowPowerStandbyTransition );
    GetHsm().AddState( &m_ProductControllerStatePlayingActive );
    GetHsm().AddState( &m_ProductControllerStatePlayingInactive );
    GetHsm().AddState( &m_ProductControllerStateIdle );
    GetHsm().AddState( &m_ProductControllerStateNetworkStandby );
    GetHsm().AddState( &m_ProductControllerStateVoiceConfigured );
    GetHsm().AddState( &m_ProductControllerStateVoiceNotConfigured );
    GetHsm().AddState( &m_ProductControllerStateNetworkConfigured );
    GetHsm().AddState( &m_ProductControllerStateNetworkNotConfigured );

    GetHsm().Init( this, PRODUCT_CONTROLLER_STATE_BOOTING );

    m_ConfigurationStatusPersistence = ProtoPersistenceFactory::Create( "ConfigurationStatus", g_ProductPersistenceDir );
    m_ConfigurationStatus.mutable_status()->set_language( IsLanguageSet() );
    ReadConfigurationStatusFromPersistence();

    m_lightbarController = std::unique_ptr<LightBar::LightBarController>( new LightBar::LightBarController( GetTask(), m_FrontDoorClientIF,  m_LpmInterface->GetClient() ) );
    m_displayController  = std::unique_ptr<DisplayController           >( new DisplayController( *this    , m_FrontDoorClientIF,  m_LpmInterface->GetClient() ) );
    SetupProductSTSController();

    // Start Eddie ProductAudioService
    m_ProductAudioService = std::make_shared< CustomProductAudioService>( *this, m_FrontDoorClientIF );
    m_ProductAudioService -> Run();

    // Initialize and register Intents for the Product Controller
    m_IntentHandler.Initialize();
}

EddieProductController::~EddieProductController()
{
}

void EddieProductController::Initialize()
{
    //Instantiate and run the hardware interface.
    m_LpmInterface->Run( );

    m_productCliClient.Initialize( GetTask() );
    RegisterCliClientCmds();
    RegisterEndPoints();
    SendInitialRequests();
    ///Register lpm events that lightbar will handle
    m_lightbarController->RegisterLightBarEndPoints();
    m_demoController.Initialize();
    m_displayController ->Initialize();
}


////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief  EddieProductController::GetMessageHandler
///
/// @return Callback < ProductMessage >
///
////////////////////////////////////////////////////////////////////////////////////////////////////
Callback < ProductMessage > EddieProductController::GetMessageHandler( )
{
    Callback < ProductMessage >
    ProductMessageHandler( std::bind( &EddieProductController::HandleProductMessage,
                                      this,
                                      std::placeholders::_1 ) );
    return ProductMessageHandler;
}

std::string const& EddieProductController::GetProductType() const
{
    static std::string productType = "SoundTouch 05";
    return productType;
}

//@TODO - Below value may be available through HSP APIs
std::string const& EddieProductController::GetProductVariant() const
{
    static std::string productType = "Eddie";
    return productType;
}

void EddieProductController::RegisterLpmEvents()
{
    BOSE_INFO( s_logger, __func__ );

    // Register keys coming from the LPM.
    auto func = std::bind( &EddieProductController::HandleLpmKeyInformation, this, std::placeholders::_1 );
    AsyncCallback<IpcKeyInformation_t>response_cb( func, GetTask() );
    m_LpmInterface->GetClient()->RegisterEvent<IpcKeyInformation_t>( IPC_KEY, response_cb );
    m_lightbarController->RegisterLpmEvents();
}

void EddieProductController::RegisterKeyHandler()
{
    auto func = [this]( KeyHandlerUtil::ActionType_t intent )
    {
        HandleIntents( intent );
    };
    auto cb = std::make_shared<AsyncCallback<KeyHandlerUtil::ActionType_t> > ( func, GetTask() );
    m_KeyHandler.RegisterKeyHandler( cb );
}

void EddieProductController::RegisterEndPoints()
{
    BOSE_INFO( s_logger, __func__ );
    RegisterCommonEndPoints();

    AsyncCallback<Callback<ProductPb::ConfigurationStatus>> getConfigurationStatusReqCb( std::bind( &EddieProductController::HandleConfigurationStatusRequest ,
                                                         this, std::placeholders::_1 ) , GetTask() );

    AsyncCallback<SoundTouchInterface::CapsInitializationStatus> capsInitializationCb( std::bind( &EddieProductController::HandleCapsInitializationUpdate,
            this, std::placeholders::_1 ) , GetTask() );

    /// Registration of endpoints to the frontdoor client.

    m_FrontDoorClientIF->RegisterNotification<SoundTouchInterface::CapsInitializationStatus>( "CapsInitializationUpdate", capsInitializationCb );

    m_FrontDoorClientIF->RegisterGet( FRONTDOOR_SYSTEM_CONFIGURATION_STATUS_API , getConfigurationStatusReqCb );

    AsyncCallback<NetManager::Protobuf::NetworkStatus> networkStatusCb( std::bind( &EddieProductController::HandleNetworkStatus ,
                                                                                   this, std::placeholders::_1 ), GetTask() );
    m_FrontDoorClientIF->RegisterNotification<NetManager::Protobuf::NetworkStatus>( FRONTDOOR_NETWORK_STATUS_API, networkStatusCb );

    AsyncCallback<NetManager::Protobuf::WiFiProfiles> networkWifiProfilesCb( std::bind( &EddieProductController::HandleWiFiProfileResponse ,
                                                                             this, std::placeholders::_1 ), GetTask() );
    m_FrontDoorClientIF->RegisterNotification<NetManager::Protobuf::WiFiProfiles>( FRONTDOOR_NETWORK_WIFI_PROFILE_API, networkWifiProfilesCb );
}

void EddieProductController::HandleNetworkStatus( const NetManager::Protobuf::NetworkStatus& networkStatus )
{
    BOSE_INFO( s_logger, "%s,N/w status- (%s)", __func__,  ProtoToMarkup::ToJson( networkStatus, false ).c_str() );

    if( networkStatus.has_isprimaryup() )
    {
        m_cachedStatus = networkStatus;
        GetHsm().Handle< bool, bool > ( &CustomProductControllerState::HandleNetworkState, IsNetworkConfigured() /*configured*/, IsNetworkConnected() /*connected*/ );
    }
}

bool EddieProductController::IsNetworkConfigured() const
{
    return ( m_bluetoothSinkList.get().devices_size() || m_wifiProfilesCount.get() || m_cachedStatus.get().isprimaryup() );
}

bool EddieProductController::IsNetworkConnected() const
{
    return m_cachedStatus.get().isprimaryup() ;
}

void EddieProductController::HandleWiFiProfileResponse( const NetManager::Protobuf::WiFiProfiles& profiles )
{
    m_wifiProfilesCount = profiles.profiles_size();
    BOSE_INFO( s_logger, "%s, m_wifiProfilesCount=%d", __func__, m_wifiProfilesCount.get() );
    GetHsm().Handle< bool, bool > ( &CustomProductControllerState::HandleNetworkState, IsNetworkConfigured() /*configured*/, IsNetworkConnected() /*connected*/ );
}

/// This function will handle key information coming from LPM and give it to
/// KeyHandler for repeat Manager to handle.
void EddieProductController::HandleLpmKeyInformation( IpcKeyInformation_t keyInformation )
{
    BOSE_DEBUG( s_logger, __func__ );

    if( keyInformation.has_keyorigin() &&
        keyInformation.has_keystate() &&
        keyInformation.has_keyid() )
    {
        BOSE_DEBUG( s_logger, "Received key Information : keyorigin:%d,"
                    " keystate:%d, keyid:%d",
                    keyInformation.keyorigin(),
                    keyInformation.keystate(), keyInformation.keyid() );
        m_CliClientMT.SendAsyncResponse( "Received from LPM, KeySource: CONSOLE, State " + \
                                         std::to_string( keyInformation.keystate() ) + " KeyId " + \
                                         std::to_string( keyInformation.keyid() ) );
        m_KeyHandler.HandleKeys( keyInformation.keyorigin(),
                                 keyInformation.keystate(),
                                 keyInformation.keyid() );
        if( keyInformation.keystate() == KEY_RELEASED )
        {
            SendDataCollection( keyInformation );
        }
    }
    else
    {
        BOSE_ERROR( s_logger, "One or more of the parameters are not present"
                    " in the message: keyorigin_P:%d, keystate_P:%d, keyid_P:%d",
                    keyInformation.has_keyorigin(),
                    keyInformation.has_keystate(),
                    keyInformation.has_keyid() );
    }
}

void EddieProductController::SendDataCollection( const IpcKeyInformation_t& keyInformation )
{
#if 0 // @TODO Leela, re-enable this code
    std::string currentButtonId;
    const auto currentKeyId = keyInformation.keyid();
    const auto currentOrigin = keyInformation.keyorigin();

    ProductPb::Protobuf::ButtonPress KeyPress;
    KeyPress.set_eventname( keyToEventName( currentKeyId ) );
    if( currentKeyId <= NUM_KEY_NAMES )
    {
        currentButtonId = KEY_NAMES[currentKeyId - 1];
    }
    else
    {
        BOSE_ERROR( s_logger, "%s, Invalid CurrentKeyID: %d", __func__, currentKeyId );
    }
    KeyPress.set_buttonid( currentButtonId ) ;
    KeyPress.set_origin( keyToOriginator( currentOrigin ) );
    m_DataCollectionClient.processKeyData( KeyPress );
#endif
}

std::string EddieProductController::keyToEventName( uint32_t e )
{
    const std::string emptystr;

    switch( e )
    {
    case 1 :
    {
        return "bluetooth" ;
    };
    case 2:
    {
        return "aux" ;
    };
    case 3:
    {
        return "volume-plus" ;
    };
    case 4:
    {
        return "play-pause" ;
    };
    case 5:
    {
        return "volume-minus" ;
    };
    case 6:
    {
        return "alexa" ;
    };

    }
    return emptystr;
}

std::string EddieProductController::keyToOriginator( enum KeyOrigin_t e )
{
    switch( e )
    {
    case KEY_ORIGIN_CONSOLE_BUTTON:
    {
        return "console" ;
    }

    case KEY_ORIGIN_CAPSENSE:
    {
        return "capsense";
    }
    case KEY_ORIGIN_IR:
    {
        return "ir-remote" ;
    }
    case KEY_ORIGIN_RF:
    {
        return "rf" ;
    }
    case KEY_ORIGIN_CEC:
    {
        return "cec" ;
    }
    case KEY_ORIGIN_NETWORK:
    {
        return "network" ;
    }
    case KEY_ORIGIN_TAP:
    {
        return "tap" ;
    }
    case KEY_ORIGIN_INVALID:
    {
        return "invalid" ;
    }
    }
    return "unknown" ;
}


void EddieProductController::SendInitialRequests()
{
    BOSE_INFO( s_logger, __func__ );

    SendCommonInitialRequests();

    {
        AsyncCallback<std::list<std::string> > poiReadyCb( std::bind( &EddieProductController::HandleNetworkCapabilityReady, this, std::placeholders::_1 ), GetTask() );
        AsyncCallback<std::list<std::string> > poiNotReadyCb( std::bind( &EddieProductController::HandleNetworkCapabilityNotReady, this, std::placeholders::_1 ), GetTask() );

        std::list<std::string> endPoints;

        endPoints.push_back( FRONTDOOR_NETWORK_STATUS_API );

        m_FrontDoorClientIF->RegisterEndpointsOfInterest( endPoints, poiReadyCb,  poiNotReadyCb );
    }

    {
        AsyncCallback<std::list<std::string> > poiReadyCb( std::bind( &EddieProductController::HandleCapsCapabilityReady, this, std::placeholders::_1 ), GetTask() );
        AsyncCallback<std::list<std::string> > poiNotReadyCb( std::bind( &EddieProductController::HandleCapsCapabilityNotReady, this, std::placeholders::_1 ), GetTask() );
        std::list<std::string> endPoints;

        endPoints.push_back( FRONTDOOR_SYSTEM_CAPSINIT_STATUS_API );
        m_FrontDoorClientIF->RegisterEndpointsOfInterest( endPoints, poiReadyCb,  poiNotReadyCb );
    }

    {
        AsyncCallback<std::list<std::string> > poiReadyCb( std::bind( &EddieProductController::HandleBluetoothCapabilityReady, this, std::placeholders::_1 ), GetTask() );
        AsyncCallback<std::list<std::string> > poiNotReadyCb( std::bind( &EddieProductController::HandleBluetoothCapabilityNotReady, this, std::placeholders::_1 ), GetTask() );
        std::list<std::string> endPoints;

        endPoints.push_back( FRONTDOOR_BLUETOOTH_SINK_LIST_API );
        m_FrontDoorClientIF->RegisterEndpointsOfInterest( endPoints, poiReadyCb,  poiNotReadyCb );
    }

    {
        AsyncCallback<std::list<std::string> > poiReadyCb( std::bind( &EddieProductController::HandleBtLeCapabilityReady, this, std::placeholders::_1 ), GetTask() );
        AsyncCallback<std::list<std::string> > poiNotReadyCb( std::bind( &EddieProductController::HandleBtLeCapabilityNotReady, this, std::placeholders::_1 ), GetTask() );
        std::list<std::string> endPoints;

        endPoints.push_back( FRONTDOOR_BLUETOOTH_BLESETUP_STATUS_API );
        m_FrontDoorClientIF->RegisterEndpointsOfInterest( endPoints, poiReadyCb,  poiNotReadyCb );
    }
}

void EddieProductController::CallbackError( const FRONT_DOOR_CLIENT_ERRORS errorCode )
{
    BOSE_ERROR( s_logger, "%s:error code- %d", __func__, errorCode );
}

void EddieProductController::HandleCapsInitializationUpdate( const SoundTouchInterface::CapsInitializationStatus &resp )
{
    BOSE_DEBUG( s_logger, "%s:notification: %s", __func__, ProtoToMarkup::ToJson( resp, false ).c_str() );
    HandleCAPSReady( resp.capsinitialized() );
}

void EddieProductController::HandleSTSReady( void )
{
    BOSE_DEBUG( s_logger, __func__ );
    m_isSTSReady = true;
    GetHsm().Handle<>( &CustomProductControllerState::HandleSTSSourcesInit );
}

void EddieProductController::HandleConfigurationStatusRequest( const Callback<ProductPb::ConfigurationStatus> &resp )
{
    BOSE_INFO( s_logger, "%s:Response: %s", __func__, ProtoToMarkup::ToJson( m_ConfigurationStatus, false ).c_str() );
    resp.Send( m_ConfigurationStatus );
}

void EddieProductController::HandleCAPSReady( bool capsReady )
{
    m_isCapsReady = capsReady;
    GetHsm().Handle<bool>( &CustomProductControllerState::HandleCapsState, capsReady );
}

void EddieProductController::HandleNetworkModuleReady( bool networkModuleReady )
{
    BOSE_INFO( s_logger, "%s:networkModuleReady=%d", __func__, networkModuleReady );

    if( networkModuleReady && !m_isNetworkModuleReady )
    {
        AsyncCallback<NetManager::Protobuf::WiFiProfiles> networkWifiProfilesCb( std::bind( &EddieProductController::HandleWiFiProfileResponse ,
                                                                                 this, std::placeholders::_1 ), GetTask() );
        m_FrontDoorClientIF->SendGet<NetManager::Protobuf::WiFiProfiles>( FRONTDOOR_NETWORK_WIFI_PROFILE_API, networkWifiProfilesCb, errorCb );

        AsyncCallback<NetManager::Protobuf::NetworkStatus> networkStatusCb( std::bind( &EddieProductController::HandleNetworkStatus ,
                                                                                       this, std::placeholders::_1 ), GetTask() );
        m_FrontDoorClientIF->SendGet<NetManager::Protobuf::NetworkStatus>( FRONTDOOR_NETWORK_STATUS_API, networkStatusCb, errorCb );
    }

    m_isNetworkModuleReady = networkModuleReady;
}

bool EddieProductController::IsAllModuleReady() const
{
    BOSE_INFO( s_logger, "%s:|CAPS Ready=%d|LPMReady=%d|NetworkModuleReady=%d|m_isBluetoothReady=%d|STSReady=%d", __func__,
               IsCAPSReady() , IsLpmReady(), IsNetworkModuleReady(), IsBluetoothModuleReady(), IsSTSReady() );

    return ( IsCAPSReady() and
             IsLpmReady() and
             IsNetworkModuleReady() and
             IsSTSReady() and
             IsBluetoothModuleReady() ) ;
}

bool EddieProductController::IsBtLeModuleReady() const
{
    BOSE_INFO( s_logger, "%s:|m_isBLEModuleReady[%d", __func__, m_isBLEModuleReady );
    return m_isBLEModuleReady;
}

bool EddieProductController::IsCAPSReady() const
{
    BOSE_DEBUG( s_logger, "%s:%s", __func__, m_isCapsReady ? "Yes" : "No" );
    return m_isCapsReady;
}

bool EddieProductController::IsLpmReady() const
{
    BOSE_DEBUG( s_logger, "%s:%s", __func__, m_isLpmReady ? "Yes" : "No" );
    return m_isLpmReady;
}

bool EddieProductController::IsNetworkModuleReady() const
{
    bool ready = m_isNetworkModuleReady and m_cachedStatus.is_initialized() and m_wifiProfilesCount.is_initialized();
    BOSE_DEBUG( s_logger, "%s:%s", __func__, ready ? "Yes" : "No" );
    return ready;
}

bool EddieProductController::IsSTSReady() const
{
    BOSE_INFO( s_logger, "%s:STS Ready=%d", __func__, m_isSTSReady );
    return m_isSTSReady;
}

bool EddieProductController::IsLanguageSet()
{
    return m_deviceManager.IsLanguageSet();
}

void EddieProductController::ReadConfigurationStatusFromPersistence()
{
    try
    {
        std::string s = m_ConfigurationStatusPersistence->Load();
        ProtoToMarkup::FromJson( s, &m_ConfigurationStatus );
    }
    catch( const ProtoToMarkup::MarkupError &e )
    {
        BOSE_LOG( ERROR, "Configuration status from persistence failed markup error - " << e.what() );
    }
    catch( ProtoPersistenceIF::ProtoPersistenceException& e )
    {
        BOSE_LOG( ERROR, "Loading configuration status from persistence failed - " << e.what() );
    }
    BOSE_INFO( s_logger, "%s: %s", __func__, ProtoToMarkup::ToJson( m_ConfigurationStatus ).c_str() );
}

void EddieProductController::PersistSystemConfigurationStatus()
{
    BOSE_INFO( s_logger, __func__ );
    ///Persist configuration status only if it changes.
    if( m_ConfigurationStatus.status().language() not_eq IsLanguageSet() )
        ///To_Do- add condition to Check for network and Account too
    {
        m_ConfigurationStatus.mutable_status()->set_language( IsLanguageSet() );

        try
        {
            m_ConfigurationStatusPersistence->Store( ProtoToMarkup::ToJson( m_ConfigurationStatus ) );
        }
        catch( const ProtoToMarkup::MarkupError &e )
        {
            BOSE_LOG( ERROR, "Configuration status from persistence failed markup error - " << e.what() );
        }
        catch( ProtoPersistenceIF::ProtoPersistenceException& e )
        {
            BOSE_LOG( ERROR, "Loading configuration status from persistence failed - " << e.what() );
        }
    }
}

void EddieProductController::SendActivateAccessPointCmd()
{
    BOSE_INFO( s_logger, __func__ );
}

void EddieProductController::SendDeActivateAccessPointCmd()
{
    BOSE_INFO( s_logger, __func__ );
}

void EddieProductController::HandleIntents( KeyHandlerUtil::ActionType_t intent )
{
    BOSE_INFO( s_logger, "Translated Intent %d", intent );
    m_CliClientMT.SendAsyncResponse( "Translated intent = " + \
                                     std::to_string( intent ) );

    if( HandleCommonIntents( intent ) )
    {
        return;
    }

    if( IntentHandler::IsIntentAuxIn( intent ) )
    {
        GetHsm().Handle<KeyHandlerUtil::ActionType_t>( &CustomProductControllerState::HandleIntentAuxIn, intent );
    }
    else if( IntentHandler::IsIntentVoice( intent ) )
    {
        GetHsm().Handle<KeyHandlerUtil::ActionType_t>( &CustomProductControllerState::HandleIntentVoice, intent );
    }
}

void EddieProductController::HandleNetworkStandbyIntentCb( const KeyHandlerUtil::ActionType_t& intent )
{
    BOSE_INFO( s_logger, "%s: Translated Intent %d", __func__, intent );

    GetHsm().Handle<> ( &CustomProductControllerState::HandleNetworkStandbyIntent );
    return;
}

void EddieProductController::RegisterCliClientCmds()
{
    BOSE_INFO( s_logger, __func__ );
    auto cb = [this]( uint16_t cmdKey, const std::list<std::string> & argList, AsyncCallback<std::string, int32_t> respCb, int32_t transact_id )
    {
        HandleCliCmd( cmdKey, argList, respCb, transact_id );
    };

    m_CliClientMT.RegisterCLIServerCommands( "allowSourceSelect",
                                             "command to send allow/disallow source selection by Caps", "allowSourceSelect yes|no",
                                             GetTask(), cb , static_cast<int>( CLICmdKeys::ALLOW_SOURCE_SELECT ) );

    m_CliClientMT.RegisterCLIServerCommands( "setDisplayAutoMode",
                                             "command to set the display controller automatic mode", "setDisplayAutoMode auto|manual",
                                             GetTask(), cb , static_cast<int>( CLICmdKeys::SET_DISPLAY_AUTO_MODE ) );

    m_CliClientMT.RegisterCLIServerCommands( "getProductState",
                                             "command to get Product Controller state", "getProductState",
                                             GetTask(), cb , static_cast<int>( CLICmdKeys::GET_PRODUCT_CONTROLLER_STATE ) );

    m_CliClientMT.RegisterCLIServerCommands( "raw_key",
                                             "command to simulate raw key events."
                                             "Usage: raw_key origin keyId state ; where origin is 0-6, keyId is 1-7, state 0-1 (press-release).",
                                             "raw_key origin keyId state",
                                             GetTask(), cb , static_cast<int>( CLICmdKeys::RAW_KEY ) );
}

void EddieProductController::HandleCliCmd( uint16_t cmdKey,
                                           const std::list<std::string> & argList,
                                           AsyncCallback<std::string, int32_t> respCb,
                                           int32_t transact_id )
{
    std::string response( "Success" );

    std::ostringstream ss;
    ss << "Received " << cmdKey << std::endl;
    switch( static_cast<CLICmdKeys>( cmdKey ) )
    {
    case CLICmdKeys::SET_DISPLAY_AUTO_MODE:
    {
        HandleSetDisplayAutoMode( argList, response );
        break;
    }
    case CLICmdKeys::ALLOW_SOURCE_SELECT:
    {
        HandleAllowSourceSelectCliCmd( argList, response );
    }
    break;
    case CLICmdKeys::GET_PRODUCT_CONTROLLER_STATE:
    {
        HandleGetProductControllerStateCliCmd( argList, response );
    }
    break;
    case CLICmdKeys::RAW_KEY:
    {
        HandleRawKeyCliCmd( argList, response );
    }
    break;
    default:
        response = "Command not found";
        break;
    }
    respCb( response, transact_id );
}

void EddieProductController::HandleSetDisplayAutoMode( const std::list<std::string>& argList, std::string& response )
{
    if( argList.size() != 1 )
    {
        response  = "command requires one argument\n" ;
        response += "Usage: SetDisplayAutoMode";
        return;
    }
    std::string arg = argList.front();
    if( arg == "auto" )
    {
        m_displayController->SetAutoMode( true );
    }
    else if( arg == "manual" )
    {
        m_displayController->SetAutoMode( false );
    }
    else
    {
        response = "Unknown argument.\n";
        response += "Usage: auto|manual";
    }
}// HandleSetDisplayAutoMode

void EddieProductController::HandleAllowSourceSelectCliCmd( const std::list<std::string>& argList, std::string& response )
{
    if( argList.size() != 1 )
    {
        response = "command requires one argument\n" ;
        response += "Usage: allowSourceSelect yes|no";
        return;
    }
    std::string arg = argList.front();
    if( arg == "yes" )
    {
        SendAllowSourceSelectMessage( true );
    }
    else if( arg == "no" )
    {
        SendAllowSourceSelectMessage( false );
    }
    else
    {
        response = "Unknown argument.\n";
        response += "Usage: allowSourceSelect yes|no";
    }
}

void EddieProductController::HandleRawKeyCliCmd( const std::list<std::string>& argList, std::string& response )
{
    if( argList.size() == 3 )
    {
        auto it = argList.begin();
        uint8_t origin = atoi( ( *it ).c_str() ) ;
        it++;
        uint32_t id = atoi( ( *it ).c_str() ) ;
        it++;
        uint8_t state = atoi( ( *it ).c_str() ) ;

        m_KeyHandler.HandleKeys( origin, state, id );
    }
    else
    {
        response = "Invalid command. use help to look at the raw_key usage";
    }
}

void EddieProductController::HandleGetProductControllerStateCliCmd( const std::list<std::string>& argList,
                                                                    std::string& response )
{
    if( argList.size() > 0 )
    {
        response = "Incorrect usage \nUsage: getProductState";
        return;
    }
    response = "-------------------------------------\n";
    response += "Product Controller State Information\n";
    response += "-------------------------------------\n";
    response += "Current State: " + GetHsm().GetCurrentState()->GetName();
}

void EddieProductController::HandleProductMessage( const ProductMessage& productMessage )
{
    BOSE_INFO( s_logger, "%s", __func__ );

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// LPM status messages are handled at this point.
    ///////////////////////////////////////////////////////////////////////////////////////////////
    if( productMessage.has_lpmstatus( ) )
    {
        if( productMessage.lpmstatus( ).has_connected( ) )
        {
            m_isLpmReady = productMessage.lpmstatus( ).connected( );
        }
        else
        {
            BOSE_ERROR( s_logger, "An invalid LPM status message was received." );
            return;
        }
        if( m_isLpmReady )
        {
            /// RegisterLpmEvents and RegisterKeyHandler
            RegisterLpmEvents();
            RegisterKeyHandler();
        }

        BOSE_DEBUG( s_logger, "An LPM Hardware %s message was received.",
                    m_isLpmReady ? "up" : "down" );

        GetHsm().Handle<bool>( &CustomProductControllerState::HandleLpmState, m_isLpmReady );

        ///
        /// @todo The system state is return here. Code to act on this event needs to be developed.
        ///
        if( productMessage.lpmstatus( ).has_systemstatus( ) )
        {
            BOSE_DEBUG( s_logger, "The LPM system state was set to %s",
                        IpcLpmSystemState_t_Name( productMessage.lpmstatus( ).systemstatus( ) ).c_str( ) );

            switch( productMessage.lpmstatus( ).systemstatus( ) )
            {
            case SYSTEM_STATE_ON:
                break;
            case SYSTEM_STATE_OFF:
                break;
            case SYSTEM_STATE_BOOTING:
                break;
            case SYSTEM_STATE_STANDBY:
                break;
            case SYSTEM_STATE_RECOVERY:
                break;
            case SYSTEM_STATE_LOW_POWER:
                break;
            case SYSTEM_STATE_UPDATE:
                break;
            case SYSTEM_STATE_SHUTDOWN:
                break;
            case SYSTEM_STATE_FACTORY_DEFAULT:
                break;
            case SYSTEM_STATE_IDLE:
                break;
            default:
                break;
            }
        }

        ///
        /// The power state if returned from the LPM hardware is used only for informational purposes.
        ///
        if( productMessage.lpmstatus( ).has_powerstatus( ) )
        {
            BOSE_DEBUG( s_logger, "The LPM power state was set to %s",
                        IpcLPMPowerState_t_Name( productMessage.lpmstatus( ).powerstatus( ) ).c_str( ) );
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   EddieProductController::SetupProductSTSController
///
/// @brief  This method is called to perform the needed initialization of the ProductSTSController,
///         specifically, provide the set of sources to be created initially.
///
/// @param  void This method does not take any arguments.
///
/// @return This method does not return anything.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void EddieProductController::SetupProductSTSController( void )
{
    std::vector<ProductSTSController::SourceDescriptor> sources;
    ProductSTSController::SourceDescriptor descriptor_AUX{ 0, "AUX", true }; // AUX is always available
    sources.push_back( descriptor_AUX );
    Callback<void> cb_STSInitWasComplete( std::bind( &EddieProductController::HandleSTSInitWasComplete, this ) );
    Callback<ProductSTSAccount::ProductSourceSlot> cb_HandleSelectSourceSlot( std::bind( &EddieProductController::HandleSelectSourceSlot, this, std::placeholders::_1 ) );
    m_ProductSTSController.Initialize( sources, cb_STSInitWasComplete, cb_HandleSelectSourceSlot );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   EddieProductController::HandleSTSInitWasComplete
///
/// @brief  This method is called from the ProductSTSController when all the initially-created
///         sources have been created with CAPS/STS
///
/// @note   THIS METHOD IS CALLED ON THE ProductSTSController THREAD
///
/// @param  void This method does not take any arguments.
///
/// @return This method does not return anything.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void EddieProductController::HandleSTSInitWasComplete( void )
{
    BOSE_INFO( s_logger, __func__ );
    IL::BreakThread( std::bind( &EddieProductController::HandleSTSReady,
                                this ),
                     GetTask( ) );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   EddieProductController::HandleSelectSourceSlot
///
/// @brief  This method is called from the ProductSTSController when one of our sources is
///         activated by CAPS/STS
///
/// @note   THIS METHOD IS CALLED ON THE ProductSTSController THREAD
///
/// @param  ProductSTSAccount::ProductSourceSlot sourceSlot - identifies the activated slot
///
/// @return This method does not return anything.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void EddieProductController::HandleSelectSourceSlot( ProductSTSAccount::ProductSourceSlot sourceSlot )
{
    BOSE_INFO( s_logger, "%s: slot: %d", __func__, sourceSlot );
}

void EddieProductController::HandleNetworkCapabilityReady( const std::list<std::string>& points )
{
    BOSE_INFO( s_logger, __func__ );
    HandleNetworkModuleReady( true );
}

void EddieProductController::HandleNetworkCapabilityNotReady( const std::list<std::string>& points )
{
    BOSE_INFO( s_logger, __func__ );
    HandleNetworkModuleReady( false );
}

void EddieProductController::HandleCapsCapabilityReady( const std::list<std::string>& points )
{
    BOSE_INFO( s_logger, __func__ );
    HandleCAPSReady( true );
}

void EddieProductController::HandleCapsCapabilityNotReady( const std::list<std::string>& points )
{
    BOSE_INFO( s_logger, __func__ );
    HandleCAPSReady( false );
}

void EddieProductController::HandleBtLeCapabilityReady( const std::list<std::string>& points )
{
    BOSE_INFO( s_logger, __func__ );
    HandleBtLeModuleReady( true );
}

void EddieProductController::HandleBtLeCapabilityNotReady( const std::list<std::string>& points )
{
    BOSE_INFO( s_logger, __func__ );
    HandleBtLeModuleReady( false );
}

void EddieProductController::HandleBtLeModuleReady( bool btLeModuleReady )
{
    BOSE_INFO( s_logger, __func__ );
    m_isBLEModuleReady = btLeModuleReady;
    if( m_isBLEModuleReady )
        GetHsm().Handle<>( &CustomProductControllerState::HandleBtLeModuleReady );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @name   IsBooted
/// @return This method returns a true or false value, based on a series of set member variables,
///         which all must be true to indicate that the device has booted.
////////////////////////////////////////////////////////////////////////////////////////////////////
bool EddieProductController::IsBooted( ) const
{
    return IsAllModuleReady();
}


} /// namespace ProductApp
