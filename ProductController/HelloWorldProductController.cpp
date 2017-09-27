////////////////////////////////////////////////////////////////////////////////
/// @file   HelloWorldProductController.cpp
/// @brief  Example HelloWorld Product controller class
///
/// @attention Copyright 2017 Bose Corporation, Framingham, MA
////////////////////////////////////////////////////////////////////////////////

#include "HelloWorldProductController.h"
#include "ProductControllerStates.h"
#include "FrontDoorClient.h"
#include "AsyncCallback.h"
#include "ProtoToMarkup.h"
#include "DPrint.h"
#include "LpmClientFactory.h"

static DPrint s_logger( "HelloWorldProductController" );
using namespace FrontDoor;

namespace ProductApp
{
/*
 If the code is common across products in a particular common product controller state and it is OK to do
 state transitions to the SAME state in common product controller framework, just instantiate a common
 product controller state. Below code is only deriving a custom product controller Booting state class, in
 reality it may be necessary to derive all custom product state classes for Booting, Setup, NetworkStandby,
 Idle and On states. It also instantiates product specific Software Updating state.
 It is optional to create a custom product controller HSM. Below code uses common product controller HSM.
 */
HelloWorldProductController::HelloWorldProductController( std::string const& ProductName ):
    ProductController( ProductName ),
    m_ProductControllerHsm( GetTask(), ProductName + "ProductHsm", *this ),
    m_ProductControllerStateTop( m_ProductControllerHsm, NULL,  *this ),
    m_CustomProductControllerStateBooting( m_ProductControllerHsm, &m_ProductControllerStateTop, *this ),
    m_ProductControllerStateSetup( m_ProductControllerHsm, &m_ProductControllerStateTop, *this ),
    m_ProductControllerStateNetworkStandby( m_ProductControllerHsm, &m_ProductControllerStateTop, *this ),
    m_ProductControllerStateOn( m_ProductControllerHsm, &m_ProductControllerStateTop, *this ),
    m_ProductControllerStateIdle( m_ProductControllerHsm, &m_ProductControllerStateTop, *this ),
    m_CustomProductControllerStateSwUpdating( m_ProductControllerHsm, &m_ProductControllerStateTop, *this ),
    m_LpmClient(),
    m_KeyHandler( *GetTask(), m_CliClientMT )
{
    BOSE_INFO( s_logger, __func__ );
    BOSE_INFO( s_logger, "Product name is %s", GetProductName().c_str() );

    //Add common and custom states here
    m_ProductControllerHsm.AddState( &m_ProductControllerStateTop );
    m_ProductControllerHsm.AddState( &m_CustomProductControllerStateBooting );
    m_ProductControllerHsm.AddState( &m_ProductControllerStateSetup );
    m_ProductControllerHsm.AddState( &m_ProductControllerStateNetworkStandby );
    m_ProductControllerHsm.AddState( &m_ProductControllerStateOn );
    m_ProductControllerHsm.AddState( &m_ProductControllerStateIdle );
    m_ProductControllerHsm.AddState( &m_CustomProductControllerStateSwUpdating );

    //Put the custom product controller in custom product controller Booting state.
    m_ProductControllerHsm.Init( CUSTOM_PRODUCT_CONTROLLER_STATE_BOOTING );

    /* Create an instance of the front door client, providing it with a unique name.
    Note: Creation of front door client code will go to common product controller framework.
    This is here just for demonstration.
    The argument "eddie" is per /opt/Bose/etc/HandCraftedFrontDoor-SoundTouch.xml file in Eddie. It will be different for Professor. */
    m_FrontDoorClientIF = FrontDoorClient::Create( "eddie" );

    //Register endpoints to the front door client.
    RegisterEndPoints();

    InitializeLpmClient();
}

HelloWorldProductController::~HelloWorldProductController()
{
}

void HelloWorldProductController::RegisterEndPoints()
{
    BOSE_INFO( s_logger, __func__ );

    // Register a callback with front door client that gets called after product controller gets a request for system/language.
    AsyncCallback<Callback<ProductPb::Language>> getLanguageReqCb( std::bind( &HelloWorldProductController::HandleGetLanguageRequest,
                                                                              this, std::placeholders::_1 ), GetTask() );

    // Registration of /system/language endpoint to the front door client.
    m_FrontDoorClientIF->RegisterGet( "/system/language", getLanguageReqCb );
}

void HelloWorldProductController::HandleGetLanguageRequest( const Callback<ProductPb::Language> &resp )
{
    BOSE_INFO( s_logger,  __func__ );
    //Pass it on to product controller HSM. The request will be dropped if HSM is not in right state to handle.
    m_ProductControllerHsm.Handle<>( &CustomProductControllerState::HandleLanguageRequest );

    ProductPb::Language lang;
    lang.set_code( "en" );

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

void HelloWorldProductController::InitializeLpmClient()
{
    BOSE_INFO( s_logger, __func__ );

    // Connect/Initialize the LPM Client
    m_LpmClient = LpmClientFactory::Create( "ProfessorLpmClient", GetTask() );

    auto func = std::bind( &HelloWorldProductController::HandleLPMReady, this );
    AsyncCallback<bool> connectCb( func, GetTask() );
    m_LpmClient->Connect( connectCb );
}


void HelloWorldProductController::RegisterLpmEvents()
{
    BOSE_INFO( s_logger, __func__ );

    // Register keys coming from the LPM.
    auto func = std::bind( &HelloWorldProductController::HandleLpmKeyInformation, this, std::placeholders::_1 );
    AsyncCallback<IpcKeyInformation_t>response_cb( func, GetTask() );
    m_LpmClient->RegisterEvent<IpcKeyInformation_t>( IPC_KEY, response_cb );
}

void HelloWorldProductController::RegisterKeyHandler()
{
    m_KeyHandler.RegisterKeyHandler( HelloWorldProductController::KeyInformationCallBack, this );
}


void HelloWorldProductController::HandleLpmKeyInformation( IpcKeyInformation_t keyInformation )
{
    BOSE_DEBUG( s_logger, __func__ );

    if( keyInformation.has_keyorigin() && keyInformation.has_keystate() && keyInformation.has_keyid() )
    {
        BOSE_DEBUG( s_logger, "Received key Information : keyorigin:%d,"
                    " keystate:%d, keyid:%d",
                    keyInformation.keyorigin(),
                    keyInformation.keystate(), keyInformation.keyid() );
        // Feed it into the keyHandler
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

void HelloWorldProductController::HandleLPMReady()
{
    BOSE_INFO( s_logger, __func__ );
    RegisterLpmEvents();
    RegisterKeyHandler();
}

void HelloWorldProductController:: KeyInformationCallBack( const int result, void *context )
{
    BOSE_INFO( s_logger, "%s %s %d\n", __FILE__, __func__, __LINE__ );
    s_logger.LogInfo( "Keys have been translated to intend = %d", result );
}



} // namespace ProductApp
