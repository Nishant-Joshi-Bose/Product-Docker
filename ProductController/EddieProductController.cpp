////////////////////////////////////////////////////////////////////////////////
/// @file   EddieProductController.cpp
/// @brief  Eddiec Product controller class.
///
/// @attention Copyright 2017 Bose Corporation, Framingham, MA
////////////////////////////////////////////////////////////////////////////////

#include "EddieProductController.h"
#include "ProductControllerStates.h"
#include "CustomProductControllerState.h"
#include "FrontDoorClient.h"
#include "APTaskFactory.h"
#include "AsyncCallback.h"
#include "ProtoToMarkup.h"
#include "ProtoPersistenceFactory.h"
#include "LpmClientFactory.h"
#include "CLICmdsKeys.h"

static DPrint s_logger( "EddieProductController" );

using namespace FrontDoor;
namespace ProductApp
{
const std::string g_ProductPersistenceDir = "product-persistence/";

EddieProductController::EddieProductController( std::string const& ProductName ):
    ProductController( ProductName ),
    m_EddieProductControllerHsm( GetTask(), ProductName + "ProductHsm", *this ),
    m_EddieProductControllerStateTop( m_EddieProductControllerHsm, nullptr,  *this ),
    m_EddieProductControllerStateBooting( m_EddieProductControllerHsm, &m_EddieProductControllerStateTop, *this ),
    m_EddieProductControllerStateSetup( m_EddieProductControllerHsm, &m_EddieProductControllerStateTop, *this ),
    m_EddieProductControllerStateNetworkStandby( m_EddieProductControllerHsm, &m_EddieProductControllerStateTop, *this ),
    m_LpmClient(),
    m_KeyHandler( *GetTask(), m_CliClientMT ),
    m_deviceManager( GetTask(), *this )
{
    BOSE_INFO( s_logger, __func__ );
    /// Add States to HSM object and initialize HSM before doing anything else.
    m_EddieProductControllerHsm.AddState( &m_EddieProductControllerStateTop );
    m_EddieProductControllerHsm.AddState( &m_EddieProductControllerStateBooting );
    m_EddieProductControllerHsm.AddState( &m_EddieProductControllerStateSetup );
    m_EddieProductControllerHsm.AddState( &m_EddieProductControllerStateNetworkStandby );
    m_EddieProductControllerHsm.Init( CUSTOM_PRODUCT_CONTROLLER_STATE_BOOTING );

    InitializeLpmClient();
    m_LanguagePersistence = ProtoPersistenceFactory::Create( "ProductLanguage", g_ProductPersistenceDir );
    m_ConfigurationStatusPersistence = ProtoPersistenceFactory::Create( "ConfigurationStatus", g_ProductPersistenceDir );
    ReadSystemLanguageFromPersistence();
    m_ConfigurationStatus.mutable_status()->set_language( IsLanguageSet() );
    ReadConfigurationStatusFromPersistence();

    /// Create an instance of the front door client, providing it with a unique name.
    m_FrontDoorClientIF = FrontDoorClient::Create( ProductName );
}

EddieProductController::~EddieProductController()
{
}

void EddieProductController::Initialize()
{
    m_productCliClient.Initialize( GetTask() );
    RegisterCliClientCmds();
    RegisterEndPoints();
    SendInitialRequests();
}

void EddieProductController::InitializeLpmClient()
{
    BOSE_INFO( s_logger, __func__ );

    // Connect/Initialize the LPM Client
    m_LpmClient = LpmClientFactory::Create( "EddieLpmClient", GetTask() );

    auto func = std::bind( &EddieProductController::HandleLPMReady, this );
    AsyncCallback<bool> connectCb( func, GetTask() );
    m_LpmClient->Connect( connectCb );
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
    m_KeyHandler.RegisterKeyHandler( EddieProductController::KeyInformationCallBack, this );
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

    AsyncCallback<SoundTouchInterface::CapsInitializationStatus> capsInitializationCb( std::bind( &EddieProductController::HandleCapsInitializationUpdate ,
            this, std::placeholders::_1 ) , GetTask() );

    AsyncCallback<Callback<SoundTouchInterface::AllowSourceSelect>> getallowSourceSelectReqCb( std::bind( &EddieProductController::HandleAllowSourceSelectRequest ,
                                                                 this, std::placeholders::_1 ) , GetTask() );
    /// Registration of endpoints to the frontdoor client.

    m_FrontDoorClientIF->RegisterNotification<SoundTouchInterface::CapsInitializationStatus>( "CapsInitializationUpdate", capsInitializationCb );
    m_FrontDoorClientIF->RegisterGet( "/system/language" , getLanguageReqCb );
    m_FrontDoorClientIF->RegisterGet( "/system/configuration/status" , getConfigurationStatusReqCb );

    m_FrontDoorClientIF->RegisterGet( "/content/allowSourceSelect" , getallowSourceSelectReqCb );

    m_FrontDoorClientIF->RegisterPost<ProductPb::Language>( "/system/language" , postLanguageReqCb );
    //Device info get request handler
    m_FrontDoorClientIF->RegisterGet( "/system/info", getDeviceInfoReqCb );
    //Device state get request handler
    m_FrontDoorClientIF->RegisterGet( "/system/state", getDeviceStateReqCb );
    SendAllowSourceSelectNotification( true );
}

void EddieProductController::SendAllowSourceSelectNotification( bool isSourceSelectAllowed )
{
    BOSE_INFO( s_logger, __func__ );
    SoundTouchInterface::AllowSourceSelect pb;
    pb.set_sourceselectallowed( isSourceSelectAllowed );
    m_FrontDoorClientIF->SendNotification( "allowSourceSelectUpdate", pb );
}

void EddieProductController::HandleAllowSourceSelectRequest( const Callback<SoundTouchInterface::AllowSourceSelect> &resp )
{
    BOSE_DEBUG( s_logger, __func__ );
    SoundTouchInterface::AllowSourceSelect pb;
    pb.set_sourceselectallowed( true );
    resp.Send( pb );
}

// This function will handle key information coming from LPM and give it to
// KeyHandler for repeat Manager to handle.
void EddieProductController::HandleLpmKeyInformation( IpcKeyInformation_t keyInformation )
{
    BOSE_DEBUG( s_logger, __func__ );

    if( keyInformation.has_keyorigin() && keyInformation.has_keystate() && keyInformation.has_keyid() )
    {
        BOSE_DEBUG( s_logger, "Received key Information : keyorigin:%d,"
                    " keystate:%d, keyid:%d",
                    keyInformation.keyorigin(),
                    keyInformation.keystate(), keyInformation.keyid() );
        // Feed it into the keyHandler : Coming soon.
        if( KeyHandlerUtil::KeyRepeatManager *ptrRepeatMgr = m_KeyHandler.RepeatMgr( keyInformation.keyorigin() ) )
        {
            m_CliClientMT.SendAsyncResponse( "Received from LPM, KeySource: CONSOLE, State " + \
                                             std::to_string( keyInformation.keystate() ) + " KeyId " + \
                                             std::to_string( keyInformation.keyid() ) );
            ptrRepeatMgr->HandleKeys( keyInformation.keyorigin(),
                                      keyInformation.keystate(), keyInformation.keyid() );
        }
        else
        {
            s_logger.LogError( "Source %d not registered", keyInformation.has_keyorigin() );
        }
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

void EddieProductController::SendInitialRequests()
{
    BOSE_INFO( s_logger, __func__ );
    AsyncCallback<FRONT_DOOR_CLIENT_ERRORS> errorCb( std::bind( &EddieProductController::CapsInitializationStatusCallbackError ,
                                                                this, std::placeholders::_1 ) , GetTask() );

    AsyncCallback<SoundTouchInterface::CapsInitializationStatus> capsInitializationCb( std::bind( &EddieProductController::HandleCapsInitializationUpdate ,
            this, std::placeholders::_1 ) , GetTask() );
    m_FrontDoorClientIF->SendGet<SoundTouchInterface::CapsInitializationStatus>( "/system/capsInitializationStatus", capsInitializationCb, errorCb );
}

void EddieProductController::CapsInitializationStatusCallbackError( const FRONT_DOOR_CLIENT_ERRORS errorCode )
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

void EddieProductController::HandleLPMReady()
{
    BOSE_INFO( s_logger, __func__ );
    RegisterLpmEvents();
    RegisterKeyHandler();
    m_isLPMReady = true;
    m_EddieProductControllerHsm.Handle<>( &CustomProductControllerState::HandleModulesReady );
}

bool EddieProductController::IsAllModuleReady()
{
    BOSE_INFO( s_logger, "%s:|CAPS Ready=%d|LPMReady=%d|", __func__, m_isCapsReady , m_isLPMReady );
    return ( m_isCapsReady and m_isLPMReady );
}

bool EddieProductController::IsLanguageSet()
{
    return not m_systemLanguage.code().empty();
}

bool EddieProductController::IsNetworkSetupDone()
{
    return m_ConfigurationStatus.status().network();
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

void EddieProductController::KeyInformationCallBack( const int result, void *context )
{
    s_logger.LogInfo( "Keys have been translated to intend = %d", result );
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
        SendAllowSourceSelectNotification( true );
    }
    else if( arg == "no" )
    {
        SendAllowSourceSelectNotification( false );
    }
    else
    {
        response = "Unknown argument.\n";
        response += "Usage: allowSourceSelect yes|no";
    }
}

} // namespace ProductApp
