////////////////////////////////////////////////////////////////////////////////
/// @file   EddieProductController.cpp
/// @brief  Eddie product controller class.
///
/// @attention Copyright 2017 Bose Corporation, Framingham, MA
////////////////////////////////////////////////////////////////////////////////

#include "EddieProductController.h"
#include "CustomProductControllerStates.h"
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
#include "CustomProductLpmHardwareInterface.h"
#include "MfgData.h"
#include "BLESetupEndpoints.h"
#include "ButtonPress.pb.h"
#include "DataCollectionClientFactory.h"

static DPrint s_logger( "EddieProductController" );

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
    m_ProductControllerStateFactoryDefault( GetHsm(), &m_ProductControllerStateTop, PRODUCT_CONTROLLER_STATE_FACTORY_DEFAULT ),
    m_ProductControllerStatePlayingDeselected( GetHsm(), &m_ProductControllerStatePlaying, PRODUCT_CONTROLLER_STATE_PLAYING_DESELECTED ),
    m_ProductControllerStatePlayingSelected( GetHsm(), &m_ProductControllerStatePlaying, PRODUCT_CONTROLLER_STATE_PLAYING_SELECTED ),
    m_ProductControllerStatePlayingSelectedSilent( GetHsm(), &m_ProductControllerStatePlayingSelected, PRODUCT_CONTROLLER_STATE_PLAYING_SELECTED_SILENT ),
    m_ProductControllerStatePlayingSelectedNotSilent( GetHsm(), &m_ProductControllerStatePlayingSelected, PRODUCT_CONTROLLER_STATE_PLAYING_SELECTED_NOT_SILENT ),
    m_ProductControllerStatePlayingSelectedSetup( GetHsm(), &m_ProductControllerStatePlayingSelected, PRODUCT_CONTROLLER_STATE_PLAYING_SELECTED_SETUP ),
    m_ProductControllerStatePlayingSelectedSetupNetwork( GetHsm(), &m_ProductControllerStatePlayingSelectedSetup, PRODUCT_CONTROLLER_STATE_PLAYING_SELECTED_NETWORK_SETUP ),
    m_ProductControllerStatePlayingSelectedSetupOther( GetHsm(), &m_ProductControllerStatePlayingSelectedSetup, PRODUCT_CONTROLLER_STATE_PLAYING_SELECTED_SETUP_OTHER ),
    m_ProductControllerStateStoppingStreams( GetHsm(), &m_ProductControllerStateTop, PRODUCT_CONTROLLER_STATE_STOPPING_STREAMS ),
    m_ProductControllerStatePlayableTransition( GetHsm(), &m_ProductControllerStateTop, PRODUCT_CONTROLLER_STATE_PLAYABLE_TRANSITION ),
    m_ProductControllerStatePlayableTransitionIdle( GetHsm(), &m_ProductControllerStatePlayableTransition, PRODUCT_CONTROLLER_STATE_PLAYABLE_TRANSITION_IDLE ),
    m_ProductControllerStatePlayableTransitionNetworkStandby( GetHsm(), &m_ProductControllerStatePlayableTransition, PRODUCT_CONTROLLER_STATE_PLAYABLE_TRANSITION_NETWORK_STANDBY ),
    m_ProductControllerStateSoftwareUpdateTransition( GetHsm(), &m_ProductControllerStateTop, PRODUCT_CONTROLLER_STATE_SOFTWARE_UPDATE_TRANSITION ),
    m_ProductControllerStatePlayingTransition( GetHsm(), &m_ProductControllerStateTop, PRODUCT_CONTROLLER_STATE_PLAYING_TRANSITION ),
    m_ProductControllerStatePlayingTransitionSelected( GetHsm(), &m_ProductControllerStatePlayingTransition, PRODUCT_CONTROLLER_STATE_PLAYING_TRANSITION_SELECTED ),
    m_KeyHandler( *GetTask(), m_CliClientMT, KEY_CONFIG_FILE ),
    m_cachedStatus(),
    m_IntentHandler( *GetTask(), m_CliClientMT, m_FrontDoorClientIF, *this ),
    m_wifiProfilesCount(),
    m_fdErrorCb( AsyncCallback<EndPointsError::Error> ( std::bind( &EddieProductController::CallbackError,
                                                                   this, std::placeholders::_1 ), GetTask() ) ),
    m_voiceServiceClient( ProductName, m_FrontDoorClientIF ),
    m_LpmInterface( std::make_shared< CustomProductLpmHardwareInterface >( *this ) )
{
    BOSE_INFO( s_logger, __func__ );
    m_deviceManager.Initialize( this );
    m_ProductFrontDoorUtility.Initialize( this, &m_deviceManager );

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
    GetHsm().AddState( &m_ProductControllerStateFactoryDefault );
    GetHsm().AddState( &m_ProductControllerStatePlayingDeselected );
    GetHsm().AddState( &m_ProductControllerStatePlayingSelected );
    GetHsm().AddState( &m_ProductControllerStatePlayingSelectedSilent );
    GetHsm().AddState( &m_ProductControllerStatePlayingSelectedNotSilent );
    GetHsm().AddState( &m_ProductControllerStatePlayingSelectedSetup );
    GetHsm().AddState( &m_ProductControllerStatePlayingSelectedSetupNetwork );
    GetHsm().AddState( &m_ProductControllerStatePlayingSelectedSetupOther );
    GetHsm().AddState( &m_ProductControllerStateStoppingStreams );
    GetHsm().AddState( &m_ProductControllerStatePlayableTransition );
    GetHsm().AddState( &m_ProductControllerStatePlayableTransitionIdle );
    GetHsm().AddState( &m_ProductControllerStatePlayableTransitionNetworkStandby );
    GetHsm().AddState( &m_ProductControllerStateSoftwareUpdateTransition );
    GetHsm().AddState( &m_ProductControllerStatePlayingTransition );
    GetHsm().AddState( &m_ProductControllerStatePlayingTransitionSelected );


    GetHsm().Init( this, PRODUCT_CONTROLLER_STATE_BOOTING );

    m_ConfigurationStatusPersistence = ProtoPersistenceFactory::Create( "ConfigurationStatus", g_ProductPersistenceDir );
    m_ConfigurationStatus.mutable_status()->set_language( IsLanguageSet() );
    ReadConfigurationStatusFromPersistence();

    m_lightbarController = std::unique_ptr<LightBar::LightBarController>( new LightBar::LightBarController( GetTask(), m_FrontDoorClientIF,  m_LpmInterface->GetLpmClient() ) );
    m_displayController  = std::unique_ptr<DisplayController           >( new DisplayController( *this    , m_FrontDoorClientIF,  m_LpmInterface->GetLpmClient() ) );
    SetupProductSTSController();

    //Data Collection support
    m_DataCollectionClient =  DataCollectionClientFactory::CreateUDCService();

    // Start Eddie ProductAudioService
    m_ProductAudioService = std::make_shared< CustomProductAudioService>( *this, m_FrontDoorClientIF, m_LpmInterface->GetLpmClient() );
    m_ProductAudioService -> Run();

    // Initialize and register Intents for the Product Controller
    m_IntentHandler.Initialize();


}

EddieProductController::~EddieProductController()
{
}

void EddieProductController::Initialize()
{
    ///Instantiate and run the hardware interface.
    m_LpmInterface->Run( );

    m_productCliClient.Initialize( GetTask() );
    RegisterCliClientCmds();
    RegisterEndPoints();
    SendInitialRequests();
    ///Register lpm events that lightbar will handle
    m_lightbarController->RegisterLightBarEndPoints();
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

std::string const& EddieProductController::GetProductModel() const
{
    static std::string productModel = "SoundTouch 20";

    if( auto model = MfgData::Get( "model" ) )
    {
        productModel =  *model;
    }

    return productModel;
}

std::string const& EddieProductController::GetProductVariant() const
{
    static std::string productVariant = "SoundTouch ECO2";
    return productVariant;
}

std::string const& EddieProductController::GetProductDescription() const
{
    static std::string productDescription = "SoundTouch";

    if( auto description = MfgData::Get( "description" ) )
    {
        productDescription = *description;
    }

    return productDescription;
}

std::string const& EddieProductController::GetDefaultProductName() const
{
    static std::string productName = "Bose ";
    std::string macAddress = MacAddressInfo::GetPrimaryMAC();

    productName = "Bose ";
    try
    {
        productName += ( macAddress.substr( macAddress.length() - 6 ) );
    }
    catch( const std::out_of_range& error )
    {
        productName += macAddress;
        BOSE_WARNING( s_logger, "errorType = %s", error.what() );
    }

    BOSE_INFO( s_logger, "%s productName=%s", __func__, productName.c_str() );
    return productName;
}

void EddieProductController::RegisterLpmEvents()
{
    BOSE_INFO( s_logger, __func__ );

    // Register keys coming from the LPM.
    auto func = std::bind( &EddieProductController::HandleLpmKeyInformation, this, std::placeholders::_1 );
    AsyncCallback<IpcKeyInformation_t>response_cb( func, GetTask() );
    m_LpmInterface->GetLpmClient()->RegisterEvent<IpcKeyInformation_t>( IPC_KEY, response_cb );
    m_lightbarController->RegisterLpmEvents();

    // Register mic mute from LPM.
    AsyncCallback<IpcVPAMicState_t>micmuteresponse_cb( std::bind( &EddieProductController::HandleLpmMicEvents, this, std::placeholders::_1 ), GetTask() );
    m_LpmInterface->GetLpmClient()->RegisterEvent<IpcVPAMicState_t>( IPC_VPA_MIC_STATE_EVENT, micmuteresponse_cb );
    // Get mic mute state from LPM.
    m_LpmInterface->GetLpmClient()->IpcGetVpaMicState( micmuteresponse_cb, IPC_DEVICE_LPM );
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

    AsyncCallback<Callback<ProductPb::ConfigurationStatus>, Callback<EndPointsError::Error>> getConfigurationStatusReqCb( std::bind( &EddieProductController::HandleConfigurationStatusRequest ,
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

uint32_t EddieProductController::GetWifiProfileCount() const
{
    if( m_wifiProfilesCount.is_initialized() )
    {
        return m_wifiProfilesCount.get();
    }
    else
    {
        return 0;
    }
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
    BOSE_DEBUG( s_logger, __func__ );

    std::string currentButtonId;
    const auto currentKeyId = keyInformation.keyid();
    const auto currentOrigin = keyInformation.keyorigin();

    if( currentKeyId <= NUM_KEY_NAMES )
    {
        currentButtonId = KEY_NAMES[currentKeyId - 1];
    }
    else
    {
        BOSE_ERROR( s_logger, "%s, Invalid CurrentKeyID: %d", __func__, currentKeyId );
    }

    auto keyPress  = std::make_shared<DataCollection::ButtonPress>();
    keyPress->set_buttonid( static_cast<DataCollection::ButtonId >( currentKeyId ) ) ;
    keyPress->set_origin( static_cast<DataCollection::Origin >( currentOrigin ) );

    m_DataCollectionClient->SendData( keyPress, "button-pressed" );
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

        endPoints.push_back( BLESetupEndpoints::STATUS_NOTIF );
        m_FrontDoorClientIF->RegisterEndpointsOfInterest( endPoints, poiReadyCb,  poiNotReadyCb );
    }
}

void EddieProductController::CallbackError( const EndPointsError::Error &error )
{
    BOSE_WARNING( s_logger, "%s: Error = (%d-%d) %s", __func__, error.code(), error.subcode(), error.message().c_str() );
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
        m_FrontDoorClientIF->SendGet<NetManager::Protobuf::WiFiProfiles, EndPointsError::Error>( FRONTDOOR_NETWORK_WIFI_PROFILE_API, networkWifiProfilesCb, m_fdErrorCb );

        AsyncCallback<NetManager::Protobuf::NetworkStatus> networkStatusCb( std::bind( &EddieProductController::HandleNetworkStatus ,
                                                                                       this, std::placeholders::_1 ), GetTask() );
        m_FrontDoorClientIF->SendGet<NetManager::Protobuf::NetworkStatus, EndPointsError::Error>( FRONTDOOR_NETWORK_STATUS_API, networkStatusCb, m_fdErrorCb );
    }

    m_isNetworkModuleReady = networkModuleReady;
}

bool EddieProductController::IsAllModuleReady() const
{
    BOSE_INFO( s_logger, "%s:|CAPS Ready=%d|LPMReady=%d|NetworkModuleReady=%d|m_isBluetoothReady=%d|"
               "STSReady=%d|IsSoftwareUpdateReady=%d", __func__, IsCAPSReady() , IsLpmReady(),
               IsNetworkModuleReady(), IsBluetoothModuleReady(), IsSTSReady(), IsSoftwareUpdateReady() );

    return ( IsCAPSReady() and
             IsLpmReady() and
             IsNetworkModuleReady() and
             IsSTSReady() and
             IsBluetoothModuleReady() and
             IsSoftwareUpdateReady() ) ;
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

NetManager::Protobuf::OperationalMode EddieProductController::GetWiFiOperationalMode( )
{
    return GetNetworkServiceUtil().GetNetManagerOperationMode();
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
    else if( IntentHandler::IsIntentCountDown( intent ) )
    {
        GetHsm().Handle<KeyHandlerUtil::ActionType_t>( &CustomProductControllerState::HandleIntentCountDown, intent );
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

    m_CliClientMT.RegisterCLIServerCommands( "setDisplayAutoMode",
                                             "command to set the display controller automatic mode", "setDisplayAutoMode auto|manual",
                                             GetTask(), cb , static_cast<int>( CLICmdKeys::SET_DISPLAY_AUTO_MODE ) );

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

    BOSE_INFO( s_logger, "%s - cmd: %d", __func__, cmdKey );
    switch( static_cast<CLICmdKeys>( cmdKey ) )
    {
    case CLICmdKeys::SET_DISPLAY_AUTO_MODE:
    {
        HandleSetDisplayAutoMode( argList, response );
        break;
    }
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
        response = "Invalid arguments. use help to look at the raw_key usage";
    }
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

            if( m_isLpmReady )
            {
                /// RegisterLpmEvents and RegisterKeyHandler
                RegisterLpmEvents();
                RegisterKeyHandler();
            }

            BOSE_DEBUG( s_logger, "An LPM Hardware %s message was received.",
                        m_isLpmReady ? "up" : "down" );

            GetHsm().Handle<bool>( &CustomProductControllerState::HandleLpmState, m_isLpmReady );
        }


        ///
        /// @todo The system state is return here. Code to act on this event needs to be developed.
        ///
        if( productMessage.lpmstatus( ).has_systemstate( ) )
        {
            BOSE_DEBUG( s_logger, "%s-The LPM system state was set to %s", __func__,
                        IpcLpmSystemState_t_Name( productMessage.lpmstatus( ).systemstate( ) ).c_str( ) );

            switch( productMessage.lpmstatus( ).systemstate( ) )
            {
            case SYSTEM_STATE_ON:
                m_ProductAudioService->SetThermalMonitorEnabled( true );
                break;
            case SYSTEM_STATE_OFF:
                m_ProductAudioService->SetThermalMonitorEnabled( false );
                break;
            case SYSTEM_STATE_BOOTING:
                break;
            case SYSTEM_STATE_STANDBY:
                m_ProductAudioService->SetThermalMonitorEnabled( false );
                break;
            case SYSTEM_STATE_RECOVERY:
                break;
            case SYSTEM_STATE_LOW_POWER:
            {
                GetHsm( ).Handle< >( &CustomProductControllerState::HandleLpmLowpowerSystemState );
            }
            break;
            case SYSTEM_STATE_UPDATE:
                break;
            case SYSTEM_STATE_SHUTDOWN:
                break;
            case SYSTEM_STATE_FACTORY_DEFAULT:
                BOSE_INFO( s_logger, "SYSTEM_STATE_FACTORY_DEFAULT was received." );
                GetHsm().Handle<>( &CustomProductControllerState::HandleFactoryDefault );
                break;
            case SYSTEM_STATE_IDLE:
                break;
            case SYSTEM_STATE_NUM_OF:
                break;
            case SYSTEM_STATE_ERROR:
                break;
            }
        }

        ///
        /// The power state if returned from the LPM hardware is used only for informational purposes.
        ///
        if( productMessage.lpmstatus( ).has_powerstate( ) )
        {
            BOSE_DEBUG( s_logger, "%s-The LPM power state was set to %s", __func__,
                        IpcLPMPowerState_t_Name( productMessage.lpmstatus( ).powerstate( ) ).c_str( ) );
        }
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// AudioPath Select or Deselect messages are handled at this point.
    ///////////////////////////////////////////////////////////////////////////////////////////////
    else if( productMessage.has_audiopathselect( ) )
    {
        if( productMessage.audiopathselect() == true )
        {
            BOSE_DEBUG( s_logger, "AudioPath Select event received" );
            GetHsm( ).Handle< > ( &CustomProductControllerState::HandleAudioPathSelect );
        }
        else
        {
            BOSE_DEBUG( s_logger, "AudioPath Deselect event received" );
            GetHsm( ).Handle< > ( &CustomProductControllerState::HandleAudioPathDeselect );
        }
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// LPM low power status messages are handled at this point.
    /////////////////////////////////////////////////////////////////////////////////////////////
    else if( productMessage.has_lpmlowpowerstatus( ) )
    {
        GetHsm( ).Handle<const ProductLpmLowPowerStatus& >( &CustomProductControllerState::HandleLpmLowPowerStatus, productMessage.lpmlowpowerstatus( ) );
    }
    //
    // An amp fault has been detected on the LPM. Enter the CriticalError state.
    //
    else if( productMessage.has_ampfaultdetected() )
    {
        GetHsm( ).Handle<>( &CustomProductControllerState::HandleAmpFaultDetected );
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// Unknown message types are handled at this point.
    ///////////////////////////////////////////////////////////////////////////////////////////////
    else
    {
        BOSE_ERROR( s_logger, "An unknown message type was received." );
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

std::string EddieProductController::GetProductColor() const
{
    if( auto color = MfgData::GetColor() )
    {
        if( *color == "luxGray" )
        {
            return "SILVER";
        }
        else if( *color == "tripleBlack" )
        {
            return "BLACK";
        }
        else
        {
            BOSE_LOG( WARNING, "Unexpected color value in manufacturing data: " << *color );
        }
    }
    else
    {
        BOSE_DIE( "No 'productColor' in mfgdata" );
    }

    return "UNKNOWN";
}

BLESetupService::VariantId EddieProductController::GetVariantId() const
{
    BLESetupService::VariantId varintId = BLESetupService::VariantId::NONE;

    if( auto color = MfgData::GetColor() )
    {
        if( *color == "luxGray" )
        {
            varintId = BLESetupService::VariantId::SILVER;
        }
        else if( *color == "tripleBlack" )
        {
            varintId = BLESetupService::VariantId::BLACK;
        }
        else
        {
            varintId = BLESetupService::VariantId::WHITE;
        }
    }
    else
    {
        BOSE_DIE( "No 'productColor' in mfgdata" );
    }

    return varintId;
}

} /// namespace ProductApp
