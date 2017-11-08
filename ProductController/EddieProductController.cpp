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
    m_EddieProductControllerStateTop( GetEddieHsm(), nullptr ),
    m_EddieProductControllerStateBooting( GetEddieHsm(), &m_EddieProductControllerStateTop ),
    m_EddieProductControllerStateSetup( GetEddieHsm(), &m_EddieProductControllerStateTop ),
    m_EddieProductControllerStateNetworkStandby( GetEddieHsm(), &m_EddieProductControllerStateTop ),
    m_EddieProductControllerStateAudioOn( GetEddieHsm(), &m_EddieProductControllerStateTop ),
    m_LpmClient(),
    m_KeyHandler( *GetTask(), m_CliClientMT, KEY_CONFIG_FILE ),
    m_deviceManager( GetTask(), *this ),
    m_cachedStatus(),
    m_IntentHandler( *GetTask(), m_CliClientMT, m_FrontDoorClientIF, *this ),
    m_LpmInterface( std::bind( &EddieProductController::HandleProductMessage,
                               this, std::placeholders::_1 ), GetTask() ),
    m_wifiProfilesCount( 0 ),
    m_bluetoothSinkList(),
    errorCb( AsyncCallback<FRONT_DOOR_CLIENT_ERRORS> ( std::bind( &EddieProductController::CallbackError,
                                                                  this, std::placeholders::_1 ), GetTask() ) ),
    m_demoController( m_ProductControllerTask, m_KeyHandler )
{
    BOSE_INFO( s_logger, __func__ );
    /// Add States to HSM object and initialize HSM before doing anything else.
    GetEddieHsm().AddState( &m_EddieProductControllerStateTop );
    GetEddieHsm().AddState( &m_EddieProductControllerStateBooting );
    GetEddieHsm().AddState( &m_EddieProductControllerStateSetup );
    GetEddieHsm().AddState( &m_EddieProductControllerStateNetworkStandby );
    GetEddieHsm().AddState( &m_EddieProductControllerStateAudioOn );
    GetEddieHsm().Init( this, PRODUCT_CONTROLLER_STATE_BOOTING );

    InitializeLpmClient();
    m_LanguagePersistence = ProtoPersistenceFactory::Create( "ProductLanguage", g_ProductPersistenceDir );
    m_ConfigurationStatusPersistence = ProtoPersistenceFactory::Create( "ConfigurationStatus", g_ProductPersistenceDir );
    m_nowPlayingPersistence = ProtoPersistenceFactory::Create( "NowPlaying", g_ProductPersistenceDir );
    ReadSystemLanguageFromPersistence();
    m_ConfigurationStatus.mutable_status()->set_language( IsLanguageSet() );
    ReadConfigurationStatusFromPersistence();
    ReadNowPlayingFromPersistence();

    m_lightbarController = std::unique_ptr<LightBar::LightBarController>( new LightBar::LightBarController( GetTask(), m_FrontDoorClientIF, m_LpmClient ) );
    m_displayController  = std::unique_ptr<DisplayController           >( new DisplayController( *this    , m_FrontDoorClientIF, m_LpmClient ) );
    SetupProductSTSController();
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
    m_lightbarController->RegisterLightBarEndPoints();
    m_demoController.RegisterEndPoints();
    m_displayController ->Initialize();
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

    AsyncCallback<BluetoothSinkService::ListResponse> bluetoothSinkListCb( std::bind( &EddieProductController::HandleBluetoothSinkListResponse ,
                                                                           this, std::placeholders::_1 ), GetTask() );
    m_FrontDoorClientIF->RegisterNotification<BluetoothSinkService::ListResponse>( FRONTDOOR_BLUETOOTH_SINK_LIST_API, bluetoothSinkListCb );

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
        GetEddieHsm().Handle<>( &CustomProductControllerState::HandleNetworkConfigurationStatus );
    }
}

void EddieProductController::HandleWiFiProfileResponse( const NetManager::Protobuf::WiFiProfiles& profiles )
{
    m_wifiProfilesCount = profiles.profiles_size();
    BOSE_INFO( s_logger, "%s, m_wifiProfilesCount=%d", __func__, m_wifiProfilesCount );
    GetEddieHsm().Handle<>( &CustomProductControllerState::HandleNetworkConfigurationStatus );
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

void EddieProductController::HandleSTSReady( void )
{
    BOSE_DEBUG( s_logger, __func__ );
    m_isSTSReady = true;
    GetEddieHsm().Handle<>( &CustomProductControllerState::HandleModulesReady );
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
    GetEddieHsm().Handle<>( &CustomProductControllerState::HandleModulesReady );
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
    GetEddieHsm().Handle<>( &CustomProductControllerState::HandleModulesReady );
}

void EddieProductController::HandleLPMReady()
{
    BOSE_INFO( s_logger, __func__ );
}

bool EddieProductController::IsAllModuleReady()
{
    BOSE_INFO( s_logger, "%s:|CAPS Ready=%d|LPMReady=%d|NetworkModuleReady=%d|m_isBluetoothReady=%d|STSReady=%d", __func__,
               m_isCapsReady , m_isLPMReady, m_isNetworkModuleReady, m_isBluetoothReady, m_isSTSReady );
    return ( m_isCapsReady and
             m_isLPMReady and
             m_isNetworkModuleReady and
             m_isSTSReady and
             m_isBluetoothReady );
}

bool EddieProductController::IsBtLeModuleReady() const
{
    BOSE_INFO( s_logger, "%s:|m_isBLEModuleReady[%d", __func__, m_isBLEModuleReady );
    return m_isBLEModuleReady;
}


bool EddieProductController::IsCAPSReady() const
{
    BOSE_INFO( s_logger, "%s:CAPS Ready=%d", __func__, m_isCapsReady );
    return m_isCapsReady;
}

bool EddieProductController::IsSTSReady() const
{
    BOSE_INFO( s_logger, "%s:STS Ready=%d", __func__, m_isSTSReady );
    return m_isSTSReady;
}

bool EddieProductController::IsLanguageSet()
{
    return not m_systemLanguage.code().empty();
}

bool EddieProductController::IsNetworkConfigured()
{
    return ( m_bluetoothSinkList.devices_size() || m_wifiProfilesCount || m_cachedStatus.isprimaryup() );
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
        m_nowPlaying.clear_state(); // Initialize the status
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
    currentState.set_state( GetEddieHsm().GetCurrentState()->GetName() );
    BOSE_INFO( s_logger, "%s:Reponse: %s", __func__, ProtoToMarkup::ToJson( currentState, false ).c_str() );
    resp.Send( currentState );
}

void EddieProductController::HandleIntents( KeyHandlerUtil::ActionType_t intent )
{
    BOSE_INFO( s_logger, "Translated Intent %d", intent );
    m_CliClientMT.SendAsyncResponse( "Translated intent = " + \
                                     std::to_string( intent ) );

    GetEddieHsm().Handle<KeyHandlerUtil::ActionType_t>( &CustomProductControllerState::HandleIntents, intent );
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
    m_CliClientMT.RegisterCLIServerCommands( "setProductState",
                                             "command to set Product Controller state", "setProductState boot|standby|setup",
                                             GetTask(), cb , static_cast<int>( CLICmdKeys::SET_PRODUCT_CONTROLLER_STATE ) );
    m_CliClientMT.RegisterCLIServerCommands( "getProductState",
                                             "command to get Product Controller state", "getProductState",
                                             GetTask(), cb , static_cast<int>( CLICmdKeys::GET_PRODUCT_CONTROLLER_STATE ) );
    m_CliClientMT.RegisterCLIServerCommands( "getProductState",
                                             "command to get Product Controller state",  "getProductState",
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

void EddieProductController::HandleSetProductControllerStateCliCmd( const std::list<std::string>& argList,
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
        GetEddieHsm().ChangeState( PRODUCT_CONTROLLER_STATE_BOOTING );
    }
    else if( arg == "on" )
    {
        response = "Setting Product Controller state to AUDIO_ON";
        SoundTouchInterface::NowSelectionInfo nowSelectionInfo;
        GetEddieHsm().Handle<const SoundTouchInterface::NowSelectionInfo&>( &CustomProductControllerState::HandleNowSelectionInfo, nowSelectionInfo );
    }
    else if( arg == "standby" )
    {
        response = "Setting Product Controller state to NETWORK_STANDBY";
        GetEddieHsm().ChangeState( CUSTOM_PRODUCT_CONTROLLER_STATE_NETWORK_STANDBY );
    }
    else if( arg == "setup" )
    {
        response = "Setting Product Controller state to SETUP";
        GetEddieHsm().ChangeState( CUSTOM_PRODUCT_CONTROLLER_STATE_SETUP );
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
    response += "Current State: " + GetEddieHsm().GetCurrentState()->GetName();
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
            GetEddieHsm().Handle<bool>( &CustomProductControllerState::HandleLpmState, true );
        }
        break;
        case LPM_INTERFACE_DOWN:
        {
            BOSE_DEBUG( s_logger, "Received LPM Interface Down message" );
            GetEddieHsm().Handle<bool>(
                &CustomProductControllerState::HandleLpmInterfaceState, false );
        }
        break;
        case LPM_INTERFACE_UP:
        {
            BOSE_DEBUG( s_logger, "Received LPM Interface UP message" );
            GetEddieHsm().Handle<bool>(
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
    BOSE_INFO( s_logger, __func__ );
    if( bluetoothModuleReady && !m_isBluetoothReady )
    {
        AsyncCallback<BluetoothSinkService::ListResponse> bluetoothSinkListCb( std::bind( &EddieProductController::HandleBluetoothSinkListResponse ,
                                                                               this, std::placeholders::_1 ), GetTask() );
        m_FrontDoorClientIF->SendGet<BluetoothSinkService::ListResponse>( FRONTDOOR_BLUETOOTH_SINK_LIST_API, bluetoothSinkListCb, errorCb );
    }
    m_isBluetoothReady = bluetoothModuleReady;
    GetEddieHsm().Handle<>( &CustomProductControllerState::HandleModulesReady );
}

void EddieProductController::HandleBtLeModuleReady( bool btLeModuleReady )
{
    BOSE_INFO( s_logger, __func__ );
    m_isBLEModuleReady = btLeModuleReady;
    if( m_isBLEModuleReady )
        GetEddieHsm().Handle<>( &CustomProductControllerState::HandleBtLeModuleReady );
}

void EddieProductController::HandleBluetoothSinkListResponse( const BluetoothSinkService::ListResponse &list )
{
    m_bluetoothSinkList = list;
    BOSE_INFO( s_logger, "%s Bluetooth sink list count [%d]", __func__, m_bluetoothSinkList.devices_size() );
    GetEddieHsm().Handle<>( &CustomProductControllerState::HandleNetworkConfigurationStatus );
}

} // namespace ProductApp
