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

static DPrint s_logger( "ProductController" );

using namespace FrontDoor;
namespace ProductApp
{
const std::string g_ProductPersistenceDir = "product-persistence/";

ProductController::ProductController():
    m_ProductControllerTask( IL::CreateTask( "ProductControllerTask" ) ),
    m_ProductAppHsm( m_ProductControllerTask, "ProductApplicationHsm", *this ),
    m_ProductAppStateTop( m_ProductAppHsm, NULL,  *this ),
    m_ProductAppStateBooting( m_ProductAppHsm, &m_ProductAppStateTop, *this ),
    m_ProductAppStateStdOp( m_ProductAppHsm, &m_ProductAppStateTop, *this ),
    m_ProductAppStateSetup( m_ProductAppHsm, &m_ProductAppStateStdOp, *this ),
    m_ProductAppStateStandby( m_ProductAppHsm, &m_ProductAppStateStdOp, *this )
{
    BOSE_INFO( s_logger, __func__ );
    m_LanguagePersistence = ProtoPersistenceFactory::Create( "ProductLanguage", g_ProductPersistenceDir );
    ReadSystemLanguageFromPersistence();
    m_ConfigurationStatusPersistence = ProtoPersistenceFactory::Create( "ConfigurationStatus", g_ProductPersistenceDir );
    try
    {
        std::string s = m_ConfigurationStatusPersistence->Load();
        ProtoToMarkup::FromJson( s, &m_ConfigurationStatus );
    }
    catch( ... )
    {
        try
        {
            BOSE_LOG( ERROR, "Loading configuration status from persistence failed" );
            m_ConfigurationStatus.mutable_status()->set_language( IsLanguageSet() );
            m_ConfigurationStatusPersistence->Remove();
            m_ConfigurationStatusPersistence->Store( ProtoToMarkup::ToJson( m_ConfigurationStatus, false ) );
        }
        catch( ... )
        {
            BOSE_LOG( ERROR, "Storing configuration status from persistence failed" );
        }
    }

    m_ProductAppHsm.AddState( &m_ProductAppStateTop );
    m_ProductAppHsm.AddState( &m_ProductAppStateBooting );
    m_ProductAppHsm.AddState( &m_ProductAppStateStdOp );
    m_ProductAppHsm.AddState( &m_ProductAppStateSetup );
    m_ProductAppHsm.AddState( &m_ProductAppStateStandby );

    m_ProductAppHsm.Init( PRODUCT_APP_STATE_BOOTING );
    /// Create an instance of the front door client, providing it with a unique name.
    m_FrontDoorClientIF = FrontDoorClient::Create( "eddie" );

    RegisterEndPoints();
}

ProductController::~ProductController()
{
}

void ProductController::Initialize()
{
    m_productCliClient.Initialize( m_ProductControllerTask );
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
    /// Registration of endpoints to the frontdoor client.
    m_FrontDoorClientIF->RegisterGet( "/system/language" , getLanguageReqCb );
    m_FrontDoorClientIF->RegisterGet( "/system/configuration/status" , getConfigurationStatusReqCb );

    m_FrontDoorClientIF->RegisterPost<ProductPb::Language>( "/system/language" , postLanguageReqCb );
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

void ProductController::HandleCAPSReady()
{
    BOSE_INFO( s_logger, __func__ );
    m_isCapsReady = true;
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
    BOSE_INFO( s_logger, __func__ );
    return not m_systemLanguage.code().empty();
}

bool ProductController::IsNetworkSetupDone()
{
    BOSE_INFO( s_logger, __func__ );
    return m_ConfigurationStatus.status().network();
}

void ProductController::ReadSystemLanguageFromPersistence()
{
    try
    {
        std::string s = m_LanguagePersistence->Load();
        ProtoToMarkup::FromJson( s, &m_systemLanguage );
    }
    catch( ... )
    {
        BOSE_LOG( ERROR, "Loading system language from persistence failed" );
        /// Store English as default language.
        m_systemLanguage.set_code( "en" );
        PersistSystemLanguageCode();
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
        m_LanguagePersistence->Store( ProtoToMarkup::ToJson( m_systemLanguage, false ) );
    }
    catch( ... )
    {
        BOSE_LOG( ERROR, "Storing language in persistence failed" );
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
} // namespace ProductApp
