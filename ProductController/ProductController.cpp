////////////////////////////////////////////////////////////////////////////////
/// @file   ProductController.cpp
/// @brief  Generic Product controller class for Riviera based products.
///
/// @attention Copyright 2017 Bose Corporation, Framingham, MA
////////////////////////////////////////////////////////////////////////////////

#include "FrontDoorClient.h"
#include "ProductController.h"
#include "APTaskFactory.h"
#include "AsyncCallback.h"
#include "ProtoToMarkup.h"
#include "ProtoPersistenceFactory.h"
#include "LpmClientFactory.h"

static DPrint s_logger( "ProductController" );

using namespace FrontDoor;
namespace ProductApp
{
const std::string g_ProductPersistenceDir = "product-persistence/";

ProductController::ProductController():
    m_ProductControllerTask( IL::CreateTask( "ProductControllerTask" ) ),
    m_LpmClient(),
    m_ProductAppHsm( m_ProductControllerTask, "ProductApplicationHsm", *this ),
    m_ProductAppStateTop( m_ProductAppHsm, NULL,  *this ),
    m_ProductAppStateBooting( m_ProductAppHsm, &m_ProductAppStateTop, *this ),
    m_ProductAppStateStdOp( m_ProductAppHsm, &m_ProductAppStateTop, *this ),
    m_ProductAppStateSetup( m_ProductAppHsm, &m_ProductAppStateStdOp, *this ),
    m_ProductAppStateStandby( m_ProductAppHsm, &m_ProductAppStateStdOp, *this ),
    m_deviceManager( m_ProductControllerTask, *this )
{
    BOSE_INFO( s_logger, __func__ );
    InitializeLpmClient();
    m_LanguagePersistence = ProtoPersistenceFactory::Create( "ProductLanguage", g_ProductPersistenceDir );
    m_ConfigurationStatusPersistence = ProtoPersistenceFactory::Create( "ConfigurationStatus", g_ProductPersistenceDir );
    ReadSystemLanguageFromPersistence();
    m_ConfigurationStatus.mutable_status()->set_language( IsLanguageSet() );
    ReadConfigurationStatusFromPersistence();

    m_ProductAppHsm.AddState( &m_ProductAppStateTop );
    m_ProductAppHsm.AddState( &m_ProductAppStateBooting );
    m_ProductAppHsm.AddState( &m_ProductAppStateStdOp );
    m_ProductAppHsm.AddState( &m_ProductAppStateSetup );
    m_ProductAppHsm.AddState( &m_ProductAppStateStandby );

    m_ProductAppHsm.Init( PRODUCT_APP_STATE_BOOTING );


    /// Create an instance of the front door client, providing it with a unique name.
    m_FrontDoorClientIF = FrontDoorClient::Create( "eddie" );
}

ProductController::~ProductController()
{
}

void ProductController::Initialize()
{
    RegisterLpmEvents();
    m_productCliClient.Initialize( m_ProductControllerTask );
    RegisterEndPoints();
    SendInitialRequests();
}

void ProductController::InitializeLpmClient()
{
    BOSE_INFO( s_logger, __func__ );

    // Connect/Initialize the LPM Client
    m_LpmClient = LpmClientFactory::Create( "EddieLpmClient", GetTask() );

    auto func = std::bind( &ProductController::HandleLPMReady, this );
    AsyncCallback<bool> connectCb( func, GetTask() );
    m_LpmClient->Connect( connectCb );
}

void ProductController::RegisterLpmEvents()
{
    BOSE_INFO( s_logger, __func__ );

    // Register keys coming from the LPM.
    auto func = std::bind( &ProductController::HandleLpmKeyInformation, this, std::placeholders::_1 );
    AsyncCallback<IpcKeyInformation_t>response_cb( func, m_ProductControllerTask );
    m_LpmClient->RegisterEvent<IpcKeyInformation_t>( IPC_KEY, response_cb );
}


void ProductController::RegisterEndPoints()
{
    BOSE_INFO( s_logger, __func__ );

    AsyncCallback<Callback<ProductPb::Language>> getLanguageReqCb( std::bind( &ProductController::HandleGetLanguageRequest ,
                                                                              this, std::placeholders::_1 ) , m_ProductControllerTask );

    AsyncCallback<ProductPb::Language , Callback< ProductPb::Language>> postLanguageReqCb( std::bind( &ProductController::HandlePostLanguageRequest,
                                                                     this, std::placeholders::_1, std::placeholders::_2 ) , m_ProductControllerTask );

    AsyncCallback<Callback<ProductPb::ConfigurationStatus>> getConfigurationStatusReqCb( std::bind( &ProductController::HandleConfigurationStatusRequest ,
                                                         this, std::placeholders::_1 ) , m_ProductControllerTask );
    //DeviceInfo async callback
    AsyncCallback <Callback<::DeviceManager::Protobuf::DeviceInfo>> getDeviceInfoReqCb( std::bind( &ProductController :: HandleGetDeviceInfoRequest,
                                                                 this, std::placeholders::_1 ), m_ProductControllerTask );
    //Device State async callback
    AsyncCallback <Callback<::DeviceManager::Protobuf::DeviceState >> getDeviceStateReqCb( std::bind( &ProductController :: HandleGetDeviceStateRequest,
                                                                   this, std::placeholders::_1 ), m_ProductControllerTask );

    AsyncCallback<SoundTouchInterface::CapsInitializationStatus> capsInitializationCb( std::bind( &ProductController::HandleCapsInitializationUpdate ,
            this, std::placeholders::_1 ) , m_ProductControllerTask );

    /// Registration of endpoints to the frontdoor client.

    m_FrontDoorClientIF->RegisterNotification<SoundTouchInterface::CapsInitializationStatus>( "CapsInitializationUpdate", capsInitializationCb );
    m_FrontDoorClientIF->RegisterGet( "/system/language" , getLanguageReqCb );
    m_FrontDoorClientIF->RegisterGet( "/system/configuration/status" , getConfigurationStatusReqCb );

    m_FrontDoorClientIF->RegisterPost<ProductPb::Language>( "/system/language" , postLanguageReqCb );
    //Device info get request handler
    m_FrontDoorClientIF->RegisterGet( "/system/info", getDeviceInfoReqCb );
    //Device state get request handler
    m_FrontDoorClientIF->RegisterGet( "/system/state", getDeviceStateReqCb );
}

// This function will handle key information coming from LPM and give it to
// KeyHandler for repeat Manager to handle.
void ProductController::HandleLpmKeyInformation( IpcKeyInformation_t keyInformation )
{
    BOSE_DEBUG( s_logger, __func__ );

    if( keyInformation.has_keyorigin() && keyInformation.has_keystate() && keyInformation.has_keyid() )
    {
        BOSE_DEBUG( s_logger, "Received key Information : keyorigin:%d,"
                    " keystate:%d, keyid:%d",
                    keyInformation.keyorigin(),
                    keyInformation.keystate(), keyInformation.keyid() );
        // Feed it into the keyHandler : Coming soon.
    }
    else
    {
        BOSE_DEBUG( s_logger, "One or more of the parameters are not present"
                    " in the message: keyorigin_P:%d, keystate_P:%d, keyid_P:%d",
                    keyInformation.has_keyorigin(),
                    keyInformation.has_keystate(),
                    keyInformation.has_keyid() );
    }
}

void ProductController::SendInitialRequests()
{
    BOSE_INFO( s_logger, __func__ );
    AsyncCallback<FRONT_DOOR_CLIENT_ERRORS> errorCb( std::bind( &ProductController::CapsInitializationStatusCallbackError ,
                                                                this, std::placeholders::_1 ) , m_ProductControllerTask );

    AsyncCallback<SoundTouchInterface::CapsInitializationStatus> capsInitializationCb( std::bind( &ProductController::HandleCapsInitializationUpdate ,
            this, std::placeholders::_1 ) , m_ProductControllerTask );
    m_FrontDoorClientIF->SendGet<SoundTouchInterface::CapsInitializationStatus>( "/system/capsInitializationStatus", capsInitializationCb, errorCb );
}

void ProductController::CapsInitializationStatusCallbackError( const FRONT_DOOR_CLIENT_ERRORS errorCode )
{
    BOSE_ERROR( s_logger, "%s:error code- %d", __func__, errorCode );
}

void ProductController::HandleCapsInitializationUpdate( const SoundTouchInterface::CapsInitializationStatus &resp )
{
    BOSE_DEBUG( s_logger, "%s:notification: %s", __func__, ProtoToMarkup::ToJson( resp, false ).c_str() );
    HandleCAPSReady( resp.capsinitialized() );
}

void ProductController::HandleGetLanguageRequest( const Callback<ProductPb::Language> &resp )
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

void ProductController::HandlePostLanguageRequest( const ProductPb::Language &lang, const Callback<ProductPb::Language> &resp )
{
    m_systemLanguage.set_code( lang.code() );
    PersistSystemLanguageCode();
    BOSE_INFO( s_logger, "%s:Response: %s", __func__, ProtoToMarkup::ToJson( lang, false ).c_str() );
    resp.Send( lang );
}

void ProductController::HandleConfigurationStatusRequest( const Callback<ProductPb::ConfigurationStatus> &resp )
{
    BOSE_INFO( s_logger, "%s:Response: %s", __func__, ProtoToMarkup::ToJson( m_ConfigurationStatus, false ).c_str() );
    resp.Send( m_ConfigurationStatus );
}

void ProductController::HandleCAPSReady( bool capsReady )
{
    m_isCapsReady = capsReady;
    m_ProductAppHsm.Handle<>( &ProductAppState::HandleModulesReady );
}

void ProductController::HandleLPMReady()
{
    BOSE_INFO( s_logger, __func__ );
    m_isLPMReady = true;
    m_ProductAppHsm.Handle<>( &ProductAppState::HandleModulesReady );
}

bool ProductController::IsAllModuleReady()
{
    BOSE_INFO( s_logger, "%s:|CAPS Ready=%d|LPMReady=%d|", __func__, m_isCapsReady , m_isLPMReady );
    return ( m_isCapsReady and m_isLPMReady );
}

bool ProductController::IsLanguageSet()
{
    return not m_systemLanguage.code().empty();
}

bool ProductController::IsNetworkSetupDone()
{
    return m_ConfigurationStatus.status().network();
}

void ProductController::ReadConfigurationStatusFromPersistence()
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

void ProductController::ReadSystemLanguageFromPersistence()
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

std::string ProductController::GetSystemLanguageCode()
{
    return m_systemLanguage.code();
}

void ProductController::PersistSystemLanguageCode()
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

void ProductController::PersistSystemConfigurationStatus()
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

void ProductController::SendActivateAccessPointCmd()
{
    BOSE_INFO( s_logger, __func__ );
}

void ProductController::SendDeActivateAccessPointCmd()
{
    BOSE_INFO( s_logger, __func__ );
}

void ProductController :: HandleGetDeviceInfoRequest( const Callback<::DeviceManager::Protobuf::DeviceInfo>& resp )
{
    ::DeviceManager::Protobuf::DeviceInfo devInfo;

    devInfo = m_deviceManager.GetDeviceInfo();

    BOSE_INFO( s_logger, "%s:Reponse: %s", __func__, ProtoToMarkup::ToJson( devInfo, false ).c_str() );

    resp.Send( devInfo );
}

void ProductController :: HandleGetDeviceStateRequest( const Callback<::DeviceManager::Protobuf::DeviceState>& resp )
{
    ::DeviceManager::Protobuf::DeviceState currentState;

    int state_index = m_ProductAppHsm.GetCurrentStateId();
    currentState.set_state( m_ProductAppHsm.GetHsmStateName( state_index ) );
    BOSE_INFO( s_logger, "%s:Reponse: %s", __func__, ProtoToMarkup::ToJson( currentState, false ).c_str() );
    resp.Send( currentState );
}

} // namespace ProductApp
