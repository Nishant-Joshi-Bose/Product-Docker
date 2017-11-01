////////////////////////////////////////////////////////////////////////////////
/// @file   EddieProductController.cpp
/// @brief  Eddiec Product controller class.
///
/// @attention Copyright 2017 Bose Corporation, Framingham, MA
////////////////////////////////////////////////////////////////////////////////

#include "EddieProductController.h"
#include "ProductControllerStates.h"
#include "CustomProductControllerState.h"
#include "APTaskFactory.h"
#include "AsyncCallback.h"
#include "ProtoToMarkup.h"
#include "ProtoPersistenceFactory.h"
#include "LpmClientFactory.h"
#include "CLICmdsKeys.h"

static DPrint s_logger( "EddieProductController" );

using namespace DemoApp;

#define FRONTDOOR_SYSTEM_LANGUAGE_API               "/system/language"
#define FRONTDOOR_SYSTEM_CONFIGURATION_STATUS_API   "/system/configuration/status"
#define FRONTDOOR_CONTENT_ALLOWSOURCESELECT_API     "/content/allowSourceSelect"
#define FRONTDOOR_SYSTEM_INFO_API                   "/system/info"
#define FRONTDOOR_SYSTEM_STATE_API                  "/system/state"
#define FRONTDOOR_NETWORK_STATUS_API                "/network/status"
#define FRONTDOOR_NETWORK_WIFI_PROFILE_API          "/network/wifi/profile"
#define FRONTDOOR_CONTENT_NOWPLAYING_API            "/content/nowPlaying"
#define FRONTDOOR_SYSTEM_CAPSINIT_STATUS_API        "/system/capsInitializationStatus"
#define FRONTDOOR_BLUETOOTH_SINK_LIST_API           "/bluetooth/sink/list"
#define FRONTDOOR_BLUETOOTH_BLESETUP_STATUS_API     "/bluetooth/BLESetup/status"

namespace ProductApp
{
const std::string g_ProductPersistenceDir = "product-persistence/";
const std::string KEY_CONFIG_FILE = "/var/run/shepherd/KeyConfiguration.json";

EddieProductController::EddieProductController( std::string const& ProductName ):
    ProductController( ProductName ),
    m_EddieProductControllerHsm( GetTask(), ProductName + "ProductHsm" ),
    m_EddieProductControllerStateTop( m_EddieProductControllerHsm, nullptr ),
    m_EddieProductControllerStateBooting( m_EddieProductControllerHsm, &m_EddieProductControllerStateTop ),
    m_EddieProductControllerStateSetup( m_EddieProductControllerHsm, &m_EddieProductControllerStateTop ),
    m_EddieProductControllerStateNetworkStandby( m_EddieProductControllerHsm, &m_EddieProductControllerStateTop ),
    m_LpmClient(),
    m_KeyHandler( *GetTask(), m_CliClientMT, KEY_CONFIG_FILE ),
    m_deviceManager( GetTask(), *this ),
    m_cachedStatus(),
    m_productSource( m_FrontDoorClientIF, *GetTask() ),
    m_IntentHandler( *GetTask(), m_CliClientMT, m_FrontDoorClientIF, *this ),
    m_LpmInterface( std::bind( &EddieProductController::HandleProductMessage,
                               this, std::placeholders::_1 ), GetTask() ),
    m_WiFiProfilesCount( 0 ),
    errorCb( AsyncCallback<FRONT_DOOR_CLIENT_ERRORS> ( std::bind( &EddieProductController::CallbackError,
                                                                  this, std::placeholders::_1 ), GetTask() ) ),
    m_demoController( m_ProductControllerTask )
{
    BOSE_INFO( s_logger, __func__ );
    /// Add States to HSM object and initialize HSM before doing anything else.
    m_EddieProductControllerHsm.AddState( &m_EddieProductControllerStateTop );
    m_EddieProductControllerHsm.AddState( &m_EddieProductControllerStateBooting );
    m_EddieProductControllerHsm.AddState( &m_EddieProductControllerStateSetup );
    m_EddieProductControllerHsm.AddState( &m_EddieProductControllerStateNetworkStandby );
    m_EddieProductControllerHsm.Init( this, PRODUCT_CONTROLLER_STATE_BOOTING );

    InitializeLpmClient();
    m_LanguagePersistence = ProtoPersistenceFactory::Create( "ProductLanguage", g_ProductPersistenceDir );
    m_ConfigurationStatusPersistence = ProtoPersistenceFactory::Create( "ConfigurationStatus", g_ProductPersistenceDir );
    m_nowPlayingPersistence = ProtoPersistenceFactory::Create( "NowPlaying", g_ProductPersistenceDir );
    ReadSystemLanguageFromPersistence();
    m_ConfigurationStatus.mutable_status()->set_language( IsLanguageSet() );
    ReadConfigurationStatusFromPersistence();
    ReadNowPlayingFromPersistence();

    m_lightbarController = std::unique_ptr<LightBarController>( new LightBarController( *this , m_FrontDoorClientIF, m_LpmClient ) );
}

EddieProductController::~EddieProductController()
{
}

void EddieProductController::Initialize()
{
    m_LpmInterface.Initialize();
    m_productCliClient.Initialize( GetTask() );
    RegisterCliClientCmds();
    RegisterEndPoints();
    SendInitialRequests();
    //Register lpm events that lightbar will handle
    m_lightbarController->Initialize();
    m_productSource.Initialize();
    m_demoController.RegisterEndPoints();
}

void EddieProductController::InitializeLpmClient()
{
    BOSE_INFO( s_logger, __func__ );
/// To_Do- will remove m_LpmClient from EddieProductController in separate commit.
    m_LpmClient = m_LpmInterface.GetLpmClient();
}

void EddieProductController::RegisterLpmEvents()
{
    BOSE_INFO( s_logger, __func__ );

    // Register keys coming from the LPM.
    auto func = std::bind( &EddieProductController::HandleLpmKeyInformation, this, std::placeholders::_1 );
    AsyncCallback<IpcKeyInformation_t>response_cb( func, GetTask() );
    m_LpmClient->RegisterEvent<IpcKeyInformation_t>( IPC_KEY, response_cb );
}

void EddieProductController::RegisterKeyHandler()
{
    auto func = [this]( KeyHandlerUtil::ActionType_t result )
    {
        HandleIntents( result );
    };
    auto cb = std::make_shared<AsyncCallback<KeyHandlerUtil::ActionType_t> > ( func, GetTask() );
    m_KeyHandler.RegisterKeyHandler( cb );
}

void EddieProductController::RegisterEndPoints()
{
    BOSE_INFO( s_logger, __func__ );

    AsyncCallback<Callback<ProductPb::Language>> getLanguageReqCb( std::bind( &EddieProductController::HandleGetLanguageRequest ,
                                                                              this, std::placeholders::_1 ) , GetTask() );

    AsyncCallback<ProductPb::Language , Callback< ProductPb::Language>> postLanguageReqCb( std::bind( &EddieProductController::HandlePostLanguageRequest,
                                                                     this, std::placeholders::_1, std::placeholders::_2 ) , GetTask() );

    AsyncCallback<Callback<ProductPb::ConfigurationStatus>> getConfigurationStatusReqCb( std::bind( &EddieProductController::HandleConfigurationStatusRequest ,
                                                         this, std::placeholders::_1 ) , GetTask() );
    //DeviceInfo async callback
    AsyncCallback <Callback<::DeviceManager::Protobuf::DeviceInfo>> getDeviceInfoReqCb( std::bind( &EddieProductController :: HandleGetDeviceInfoRequest,
                                                                 this, std::placeholders::_1 ), GetTask() );
    //Device State async callback
    AsyncCallback <Callback<::DeviceManager::Protobuf::DeviceState >> getDeviceStateReqCb( std::bind( &EddieProductController :: HandleGetDeviceStateRequest,
                                                                   this, std::placeholders::_1 ), GetTask() );

    AsyncCallback<SoundTouchInterface::CapsInitializationStatus> capsInitializationCb( std::bind( &EddieProductController::HandleCapsInitializationUpdate,
            this, std::placeholders::_1 ) , GetTask() );

    /// Registration of endpoints to the frontdoor client.

    m_FrontDoorClientIF->RegisterNotification<SoundTouchInterface::CapsInitializationStatus>( "CapsInitializationUpdate", capsInitializationCb );
    m_FrontDoorClientIF->RegisterGet( FRONTDOOR_SYSTEM_LANGUAGE_API , getLanguageReqCb );
    m_FrontDoorClientIF->RegisterGet( FRONTDOOR_SYSTEM_CONFIGURATION_STATUS_API , getConfigurationStatusReqCb );

    m_FrontDoorClientIF->RegisterPost<ProductPb::Language>( FRONTDOOR_SYSTEM_LANGUAGE_API , postLanguageReqCb );

    ///Device info get request handler
    m_FrontDoorClientIF->RegisterGet( FRONTDOOR_SYSTEM_INFO_API, getDeviceInfoReqCb );
    ///Device state get request handler
    m_FrontDoorClientIF->RegisterGet( FRONTDOOR_SYSTEM_STATE_API, getDeviceStateReqCb );
    AsyncCallback<NetManager::Protobuf::NetworkStatus> networkStatusCb( std::bind( &EddieProductController::HandleNetworkStatus ,
                                                                                   this, std::placeholders::_1 ), GetTask() );
    m_FrontDoorClientIF->RegisterNotification<NetManager::Protobuf::NetworkStatus>( FRONTDOOR_NETWORK_STATUS_API, networkStatusCb );

    AsyncCallback<NetManager::Protobuf::WiFiProfiles> networkWifiProfilesCb( std::bind( &EddieProductController::HandleWiFiProfileResponse ,
                                                                             this, std::placeholders::_1 ), GetTask() );
    m_FrontDoorClientIF->RegisterNotification<NetManager::Protobuf::WiFiProfiles>( FRONTDOOR_NETWORK_WIFI_PROFILE_API, networkWifiProfilesCb );

    AsyncCallback<SoundTouchInterface::NowPlayingJson> nowPlayingCb( std::bind( &EddieProductController::HandleCapsNowPlaying ,
                                                                                this, std::placeholders::_1 ), GetTask() );

    m_FrontDoorClientIF->RegisterNotification<SoundTouchInterface::NowPlayingJson>( FRONTDOOR_CONTENT_NOWPLAYING_API, nowPlayingCb );
}

void EddieProductController::HandleCapsNowPlaying( const SoundTouchInterface::NowPlayingJson& nowPlayingPb )
{
    BOSE_INFO( s_logger, "%s,np- (%s)", __func__,  ProtoToMarkup::ToJson( nowPlayingPb, false ).c_str() );
    PersistCapsNowPlaying( nowPlayingPb );
}

void EddieProductController::HandleNetworkStatus( const NetManager::Protobuf::NetworkStatus& networkStatus )
{
    BOSE_INFO( s_logger, "%s,N/w status- (%s)", __func__,  ProtoToMarkup::ToJson( networkStatus, false ).c_str() );

    if( networkStatus.has_isprimaryup() )
    {
        m_cachedStatus = networkStatus;
        m_EddieProductControllerHsm.Handle<const NetManager::Protobuf::NetworkStatus&, int>( &CustomProductControllerState::HandleNetworkConfigurationStatus, networkStatus, m_WiFiProfilesCount );
    }
}

void EddieProductController::HandleWiFiProfileResponse( const NetManager::Protobuf::WiFiProfiles& profiles )
{
    m_WiFiProfilesCount = profiles.profiles_size();
    BOSE_INFO( s_logger, "%s, m_WiFiProfilesCount=%d", __func__, m_WiFiProfilesCount );
    m_EddieProductControllerHsm.Handle<const NetManager::Protobuf::NetworkStatus&, int>( &CustomProductControllerState::HandleNetworkConfigurationStatus, m_cachedStatus, m_WiFiProfilesCount );
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
        //Work around code since we can't call non-static functions from within static EddieProductController::KeyInformationCallBack function.
        //This will be fixed in new KeyHandler component.
        if( ( keyInformation.keystate() == 1 ) &&
            ( keyInformation.keyid() == ProductSource::KEY_ID::SELECT_AUX_SOURCE ) )
        {
            BOSE_DEBUG( s_logger, "AUX Source key pressed..." );
            HandleAUXSourceKeyPress();
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

void EddieProductController::HandleAUXSourceKeyPress()
{
    BOSE_DEBUG( s_logger, __func__ );

    m_productSource.SendPostAUXPlaybackRequest();
}

void EddieProductController::SendInitialRequests()
{
    BOSE_INFO( s_logger, __func__ );

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

void EddieProductController::HandleGetLanguageRequest( const Callback<ProductPb::Language> &resp )
{
    ProductPb::Language lang;
    lang.set_code( GetSystemLanguageCode() );
    lang.mutable_properties()->add_supported_language_codes( "da" );   /// Danish
    lang.mutable_properties()->add_supported_language_codes( "de" );   /// German
    lang.mutable_properties()->add_supported_language_codes( "en" );   /// English
    lang.mutable_properties()->add_supported_language_codes( "es" );   /// Spanish
    lang.mutable_properties()->add_supported_language_codes( "fr" );   /// French
    lang.mutable_properties()->add_supported_language_codes( "it" );   /// Italian
    lang.mutable_properties()->add_supported_language_codes( "nl" );   /// Dutch
    lang.mutable_properties()->add_supported_language_codes( "sv" );   /// Swedish
    lang.mutable_properties()->add_supported_language_codes( "ja" );   /// Japanese
    lang.mutable_properties()->add_supported_language_codes( "zh" );   /// Chinese

    lang.mutable_properties()->add_supported_language_codes( "ko" );   /// Korean
    lang.mutable_properties()->add_supported_language_codes( "th" );   /// Thai
    lang.mutable_properties()->add_supported_language_codes( "cs" );   /// Czechoslovakian
    lang.mutable_properties()->add_supported_language_codes( "fi" );   /// Finnish
    lang.mutable_properties()->add_supported_language_codes( "el" );   /// Greek
    lang.mutable_properties()->add_supported_language_codes( "no" );   /// Norwegian
    lang.mutable_properties()->add_supported_language_codes( "pl" );   /// Polish
    lang.mutable_properties()->add_supported_language_codes( "pt" );   /// Portuguese
    lang.mutable_properties()->add_supported_language_codes( "ro" );   /// Romanian
    lang.mutable_properties()->add_supported_language_codes( "ru" );   /// Russian

    lang.mutable_properties()->add_supported_language_codes( "sl" );   /// Slovenian
    lang.mutable_properties()->add_supported_language_codes( "tr" );   /// Turkish
    lang.mutable_properties()->add_supported_language_codes( "hu" );   /// Hungarian

    BOSE_INFO( s_logger, "%s:Response: %s", __func__, ProtoToMarkup::ToJson( lang, false ).c_str() );
    resp.Send( lang );
}

void EddieProductController::HandlePostLanguageRequest( const ProductPb::Language &lang, const Callback<ProductPb::Language> &resp )
{
    m_systemLanguage.set_code( lang.code() );
    PersistSystemLanguageCode();
    BOSE_INFO( s_logger, "%s:Response: %s", __func__, ProtoToMarkup::ToJson( lang, false ).c_str() );
    resp.Send( lang );
}

void EddieProductController::HandleConfigurationStatusRequest( const Callback<ProductPb::ConfigurationStatus> &resp )
{
    BOSE_INFO( s_logger, "%s:Response: %s", __func__, ProtoToMarkup::ToJson( m_ConfigurationStatus, false ).c_str() );
    resp.Send( m_ConfigurationStatus );
}

void EddieProductController::HandleCAPSReady( bool capsReady )
{
    m_isCapsReady = capsReady;
    m_EddieProductControllerHsm.Handle<>( &CustomProductControllerState::HandleModulesReady );
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
    m_EddieProductControllerHsm.Handle<>( &CustomProductControllerState::HandleModulesReady );
}

void EddieProductController::HandleLPMReady()
{
    BOSE_INFO( s_logger, __func__ );
}

bool EddieProductController::IsAllModuleReady()
{
    BOSE_INFO( s_logger, "%s:|CAPS Ready=%d|LPMReady=%d|NetworkModuleReady=%d|m_isBluetoothReady=%d|m_isBLEModuleReady=%d|", __func__,
               m_isCapsReady , m_isLPMReady, m_isNetworkModuleReady, m_isBluetoothReady, m_isBLEModuleReady );
    return ( m_isCapsReady and
             m_isLPMReady and
             m_isNetworkModuleReady );
    // TODO m_isBLEModuleReady and m_isBluetoothReady should also be included to complete booting
}

bool EddieProductController::IsCAPSReady() const
{
    BOSE_INFO( s_logger, "%s:CAPS Ready=%d", __func__, m_isCapsReady );
    return m_isCapsReady;
}

bool EddieProductController::IsLanguageSet()
{
    return not m_systemLanguage.code().empty();
}

bool EddieProductController::IsNetworkConfigured()
{
    return ( m_WiFiProfilesCount || m_cachedStatus.isprimaryup() );
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
}

void EddieProductController::ReadNowPlayingFromPersistence()
{
    try
    {
        std::string s = m_nowPlayingPersistence->Load();
        ProtoToMarkup::FromJson( s, &m_nowPlaying );
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

void EddieProductController::ReadSystemLanguageFromPersistence()
{
    try
    {
        std::string s = m_LanguagePersistence->Load();
        ProtoToMarkup::FromJson( s, &m_systemLanguage );
    }
    catch( const ProtoToMarkup::MarkupError &e )
    {
        BOSE_LOG( ERROR, "ReadSystemLanguageFromPersistence- markup error - " << e.what() );
    }
    catch( ProtoPersistenceIF::ProtoPersistenceException& e )
    {
        BOSE_LOG( ERROR, "ReadSystemLanguageFromPersistence failed - " << e.what() );
    }
}

std::string EddieProductController::GetSystemLanguageCode()
{
    return m_systemLanguage.code();
}

void EddieProductController::PersistSystemLanguageCode()
{
    BOSE_INFO( s_logger, __func__ );
    try
    {
        m_LanguagePersistence->Remove();
        m_LanguagePersistence->Store( ProtoToMarkup::ToJson( m_systemLanguage ) );
        /// Persist configuration status everytime language gets
        /// changed.
        PersistSystemConfigurationStatus();
    }
    catch( ... )
    {
        BOSE_LOG( ERROR, "Storing language in persistence failed" );
    }
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

///
///Return true if nowPlaying protobuf has changed.
///
bool EddieProductController::IsNowPlayingChanged( const SoundTouchInterface::NowPlayingJson& nowPlayingPb )
{
    return ( ( m_nowPlaying.source().sourcedisplayname() not_eq nowPlayingPb.source().sourcedisplayname() )
             or ( m_nowPlaying.container().contentitem().sourceaccount() not_eq nowPlayingPb.container().contentitem().sourceaccount() )
             or ( m_nowPlaying.container().contentitem().source() not_eq nowPlayingPb.container().contentitem().source() ) );
}

void EddieProductController::PersistCapsNowPlaying( const SoundTouchInterface::NowPlayingJson& nowPlayingPb, bool forcePersist )
{
    BOSE_INFO( s_logger, __func__ );
    if( forcePersist or IsNowPlayingChanged( nowPlayingPb ) )
    {
        try
        {
            m_nowPlayingPersistence->Store( ProtoToMarkup::ToJson( nowPlayingPb ) );
        }
        catch( const ProtoToMarkup::MarkupError &e )
        {
            BOSE_LOG( ERROR, "Storing nowplaying failed markup error - " << e.what() );
        }
        catch( ProtoPersistenceIF::ProtoPersistenceException& e )
        {
            BOSE_LOG( ERROR, "Storing nowplaying in persistence failed - " << e.what() );
        }
    }
    m_nowPlaying.CopyFrom( nowPlayingPb );
}

void EddieProductController::SendActivateAccessPointCmd()
{
    BOSE_INFO( s_logger, __func__ );
}

void EddieProductController::SendDeActivateAccessPointCmd()
{
    BOSE_INFO( s_logger, __func__ );
}

void EddieProductController::HandleGetDeviceInfoRequest( const Callback<::DeviceManager::Protobuf::DeviceInfo>& resp )
{
    ::DeviceManager::Protobuf::DeviceInfo devInfo;

    devInfo = m_deviceManager.GetDeviceInfo();

    BOSE_INFO( s_logger, "%s:Reponse: %s", __func__, ProtoToMarkup::ToJson( devInfo, false ).c_str() );

    resp.Send( devInfo );
}

void EddieProductController::HandleGetDeviceStateRequest( const Callback<::DeviceManager::Protobuf::DeviceState>& resp )
{
    ::DeviceManager::Protobuf::DeviceState currentState;
    currentState.set_state( m_EddieProductControllerHsm.GetCurrentState()->GetName() );
    BOSE_INFO( s_logger, "%s:Reponse: %s", __func__, ProtoToMarkup::ToJson( currentState, false ).c_str() );
    resp.Send( currentState );
}

void EddieProductController::HandleIntents( KeyHandlerUtil::ActionType_t result )
{
    BOSE_INFO( s_logger, "Translated Intend %d", result );
    m_CliClientMT.SendAsyncResponse( "Translated intend = " + \
                                     std::to_string( result ) );

    m_EddieProductControllerHsm.Handle<KeyHandlerUtil::ActionType_t>( &CustomProductControllerState::HandleIntents, result );
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
                                             "allowSourceSelect yes|no", "command to send allow/disallow source selection by Caps",
                                             GetTask(), cb , static_cast<int>( CLICmdKeys::ALLOW_SOURCE_SELECT ) );
    m_CliClientMT.RegisterCLIServerCommands( "setProductState",
                                             "setProductState boot|standby|setup", "command to set Product Controller state",
                                             GetTask(), cb , static_cast<int>( CLICmdKeys::SET_PRODUCT_CONTROLLER_STATE ) );
    m_CliClientMT.RegisterCLIServerCommands( "getProductState",
                                             "getProductState", "command to get Product Controller state",
                                             GetTask(), cb , static_cast<int>( CLICmdKeys::GET_PRODUCT_CONTROLLER_STATE ) );
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
    case CLICmdKeys::ALLOW_SOURCE_SELECT:
    {
        HandleAllowSourceSelectCliCmd( argList, response );
    }
    break;
    case CLICmdKeys::SET_PRODUCT_CONTROLLER_STATE:
    {
        HandleSetProductControllerStateCliCmd( argList, response );
    }
    break;
    case CLICmdKeys::GET_PRODUCT_CONTROLLER_STATE:
    {
        HandleGetProductControllerStateCliCmd( argList, response );
    }
    break;
    default:
        response = "Command not found";
        break;
    }
    respCb( response, transact_id );
}

void EddieProductController::HandleAllowSourceSelectCliCmd( const std::list<std::string> & argList, std::string& response )
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

void EddieProductController::HandleSetProductControllerStateCliCmd( const std::list<std::string> & argList,
                                                                    std::string& response )
{
    std::string usage;
    usage = "Usage: setProductState boot|on|standby|setup|idle";

    if( argList.size() != 1 )
    {
        response = "Incorrect usage\n" + usage;
        return;
    }

    std::string arg = argList.front();

    if( arg == "boot" )
    {
        response = "Setting Product Controller state to BOOT";
        m_EddieProductControllerHsm.ChangeState( PRODUCT_CONTROLLER_STATE_BOOTING );
    }
    else if( arg == "on" )
    {
        response = "Will be implemented in future";
    }
    else if( arg == "standby" )
    {
        response = "Setting Product Controller state to NETWORK_STANDBY";
        m_EddieProductControllerHsm.ChangeState( CUSTOM_PRODUCT_CONTROLLER_STATE_NETWORK_STANDBY );
    }
    else if( arg == "setup" )
    {
        response = "Setting Product Controller state to SETUP";
        m_EddieProductControllerHsm.ChangeState( CUSTOM_PRODUCT_CONTROLLER_STATE_SETUP );
    }
    else if( arg == "idle" )
    {
        response = "Will be implemented in future";
    }
    else
    {
        response = "Unknown argument\n" + usage;
    }
}

void EddieProductController::HandleGetProductControllerStateCliCmd( const std::list<std::string> & argList,
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
    response += "Current State: " + m_EddieProductControllerHsm.GetCurrentState()->GetName();
}

void EddieProductController::HandleProductMessage( const ProductMessage& productMessage )
{
    BOSE_DEBUG( s_logger, "Product Controller Messages" );

    if( productMessage.has_id() )
    {
        switch( productMessage.id() )
        {
        case LPM_HARDWARE_DOWN:
        {
            BOSE_DEBUG( s_logger, "Received LPM Hardware Down message" );
            m_isLPMReady = false;
        }
        break;
        case LPM_HARDWARE_UP:
        {
            BOSE_DEBUG( s_logger, "Received LPM Hardware Up message" );
            m_isLPMReady = true;
            // RegisterLpmEvents and RegisterKeyHandler
            RegisterLpmEvents();
            RegisterKeyHandler();
            m_EddieProductControllerHsm.Handle<bool>( &CustomProductControllerState::HandleLpmState, true );
        }
        break;
        case LPM_INTERFACE_DOWN:
        {
            BOSE_DEBUG( s_logger, "Received LPM Interface Down message" );
            m_EddieProductControllerHsm.Handle<bool>(
                &CustomProductControllerState::HandleLpmInterfaceState, false );
        }
        break;
        case LPM_INTERFACE_UP:
        {
            BOSE_DEBUG( s_logger, "Received LPM Interface UP message" );
            m_EddieProductControllerHsm.Handle<bool>(
                &CustomProductControllerState::HandleLpmInterfaceState, true );
        }
        break;
        default:
            BOSE_ERROR( s_logger, "Unhandled product message" );
            break;
        }
        return;
    }
    else
    {
        BOSE_ERROR( s_logger, "productMessage doesn't have an Id" );
        return;
    }
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

void EddieProductController::HandleBluetoothCapabilityReady( const std::list<std::string>& points )
{
    BOSE_INFO( s_logger, __func__ );
    HandleBluetoothModuleReady( true );
}

void EddieProductController::HandleBluetoothCapabilityNotReady( const std::list<std::string>& points )
{
    BOSE_INFO( s_logger, __func__ );
    HandleBluetoothModuleReady( false );
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

void EddieProductController::HandleBluetoothModuleReady( bool bluetoothModuleReady )
{
    m_isBluetoothReady = bluetoothModuleReady;
    m_EddieProductControllerHsm.Handle<>( &CustomProductControllerState::HandleModulesReady );
}

void EddieProductController::HandleBtLeModuleReady( bool btLeModuleReady )
{
    m_isBLEModuleReady = btLeModuleReady;
    m_EddieProductControllerHsm.Handle<>( &CustomProductControllerState::HandleModulesReady );
}

} // namespace ProductApp
