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
#include "CustomProductKeyInputManager.h"
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
#include "ProductSTSSilentStateFactory.h"
#include "CustomProductSTSAuxStateFactory.h"

static DPrint s_logger( "EddieProductController" );

using namespace DeviceManagerPb;

namespace ProductApp
{
const std::string g_ProductPersistenceDir = "product-persistence/";

EddieProductController::EddieProductController():
    m_ProductControllerStateTop( GetHsm(), nullptr ),
    m_ProductControllerStateBooting( GetHsm(), &m_ProductControllerStateTop, PRODUCT_CONTROLLER_STATE_BOOTING ),
    m_ProductControllerStateBooted( GetHsm(), &m_ProductControllerStateTop, PRODUCT_CONTROLLER_STATE_BOOTED ),
    m_CustomProductControllerStateOn( GetHsm(), &m_ProductControllerStateTop, CUSTOM_PRODUCT_CONTROLLER_STATE_ON ),
    m_CustomProductControllerStateLowPowerStandby( GetHsm(), &m_ProductControllerStateTop, CUSTOM_PRODUCT_CONTROLLER_STATE_LOW_POWER_STANDBY ),
    m_ProductControllerStateSwInstall( GetHsm(), &m_ProductControllerStateTop, PRODUCT_CONTROLLER_STATE_SOFTWARE_INSTALL ),
    m_ProductControllerStateCriticalError( GetHsm(), &m_ProductControllerStateTop, PRODUCT_CONTROLLER_STATE_CRITICAL_ERROR ),
    m_ProductControllerStatePlaying( GetHsm(), &m_CustomProductControllerStateOn, PRODUCT_CONTROLLER_STATE_PLAYING ),
    m_ProductControllerStatePlayable( GetHsm(), &m_CustomProductControllerStateOn, PRODUCT_CONTROLLER_STATE_PLAYABLE ),
    m_ProductControllerStateLowPowerStandbyTransition( GetHsm(), &m_CustomProductControllerStateLowPowerStandby, PRODUCT_CONTROLLER_STATE_LOW_POWER_STANDBY_TRANSITION ),
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
    m_ProductControllerStatePlayingSelectedSetupNetwork( GetHsm(), &m_ProductControllerStatePlayingSelectedSetup, PRODUCT_CONTROLLER_STATE_PLAYING_SELECTED_SETUP_NETWORK ),
    m_ProductControllerStatePlayingSelectedSetupNetworkTransition( GetHsm(), &m_ProductControllerStatePlayingSelectedSetup, PRODUCT_CONTROLLER_STATE_PLAYING_SELECTED_SETUP_NETWORK_TRANSITION ),
    m_ProductControllerStatePlayingSelectedSetupOther( GetHsm(), &m_ProductControllerStatePlayingSelectedSetup, PRODUCT_CONTROLLER_STATE_PLAYING_SELECTED_SETUP_OTHER ),
    m_ProductControllerStatePlayingSelectedSetupExiting( GetHsm(), &m_ProductControllerStatePlayingSelectedSetup, PRODUCT_CONTROLLER_STATE_PLAYING_SELECTED_SETUP_EXITING ),
    m_ProductControllerStatePlayingSelectedSetupExitingAP( m_ProductControllerHsm, &m_ProductControllerStatePlayingSelectedSetup, PRODUCT_CONTROLLER_STATE_PLAYING_SELECTED_SETUP_EXITING_AP ),
    m_ProductControllerStateStoppingStreams( GetHsm(), &m_ProductControllerStateTop, PRODUCT_CONTROLLER_STATE_STOPPING_STREAMS ),
    m_ProductControllerStatePlayableTransition( GetHsm(), &m_ProductControllerStateTop, PRODUCT_CONTROLLER_STATE_PLAYABLE_TRANSITION ),
    m_ProductControllerStatePlayableTransitionInternal( GetHsm(), &m_ProductControllerStatePlayableTransition, PRODUCT_CONTROLLER_STATE_PLAYABLE_TRANSITION_INTERNAL ),
    m_ProductControllerStatePlayableTransitionIdle( GetHsm(), &m_ProductControllerStatePlayableTransitionInternal, PRODUCT_CONTROLLER_STATE_PLAYABLE_TRANSITION_IDLE ),
    m_ProductControllerStatePlayableTransitionNetworkStandby( GetHsm(), &m_ProductControllerStatePlayableTransitionInternal, PRODUCT_CONTROLLER_STATE_PLAYABLE_TRANSITION_NETWORK_STANDBY ),
    m_ProductControllerStateSoftwareUpdateTransition( GetHsm(), &m_ProductControllerStateTop, PRODUCT_CONTROLLER_STATE_SOFTWARE_UPDATE_TRANSITION ),
    m_ProductControllerStatePlayingTransition( GetHsm(), &m_ProductControllerStateTop, PRODUCT_CONTROLLER_STATE_PLAYING_TRANSITION ),
    m_ProductControllerStateFirstBootGreeting( GetHsm(), &m_ProductControllerStateTop, PRODUCT_CONTROLLER_STATE_FIRST_BOOT_GREETING ),
    m_ProductControllerStateFirstBootGreetingTransition( GetHsm(), &m_ProductControllerStateTop, PRODUCT_CONTROLLER_STATE_FIRST_BOOT_GREETING_TRANSITION ),
    m_ProductControllerStateBootedTransition( GetHsm(), &m_ProductControllerStateTop, PRODUCT_CONTROLLER_STATE_BOOTED_TRANSITION ),
    m_ProductControllerStatePlayingTransitionSwitch( GetHsm(), &m_ProductControllerStatePlayingTransition, PRODUCT_CONTROLLER_STATE_PLAYING_TRANSITION_SWITCH ),
    m_ProductControllerStateStoppingStreamsDedicated( m_ProductControllerHsm, &m_ProductControllerStateTop, PRODUCT_CONTROLLER_STATE_STOPPING_STREAMS_DEDICATED ),
    m_ProductControllerStateStoppingStreamsDedicatedForFactoryDefault( m_ProductControllerHsm, &m_ProductControllerStateStoppingStreamsDedicated, PRODUCT_CONTROLLER_STATE_STOPPING_STREAMS_DEDICATED_FOR_FACTORY_DEFAULT ),
    m_ProductControllerStateStoppingStreamsDedicatedForSoftwareUpdate( m_ProductControllerHsm, &m_ProductControllerStateStoppingStreamsDedicated, PRODUCT_CONTROLLER_STATE_STOPPING_STREAMS_DEDICATED_FOR_SOFTWARE_UPDATE ),
    m_IntentHandler( *GetTask(), m_CliClientMT, m_FrontDoorClientIF, *this ),
    m_LpmInterface( std::make_shared< CustomProductLpmHardwareInterface >( *this ) ),
    m_dataCollectionClientInterface( m_FrontDoorClientIF, GetDataCollectionClient() )
{
    BOSE_INFO( s_logger, __func__ );
}

EddieProductController::~EddieProductController()
{
}

void EddieProductController::InitializeHsm()
{
    /// Add States to HSM object and initialize HSM before doing anything else.
    GetHsm().AddState( "", &m_ProductControllerStateTop );
    GetHsm().AddState( "", &m_CustomProductControllerStateLowPowerStandby );
    GetHsm().AddState( NotifiedNames_Name( NotifiedNames::UPDATING ), &m_ProductControllerStateSwInstall );
    GetHsm().AddState( NotifiedNames_Name( NotifiedNames::BOOTING ), &m_ProductControllerStateBooting );
    GetHsm().AddState( "", &m_ProductControllerStateBooted );
    GetHsm().AddState( "", &m_CustomProductControllerStateOn );
    GetHsm().AddState( NotifiedNames_Name( NotifiedNames::CRITICAL_ERROR ), &m_ProductControllerStateCriticalError );
    GetHsm().AddState( "", &m_ProductControllerStatePlaying );
    GetHsm().AddState( "", &m_ProductControllerStatePlayable );
    GetHsm().AddState( "", &m_ProductControllerStateLowPowerStandbyTransition );
    GetHsm().AddState( NotifiedNames_Name( NotifiedNames::IDLE ), &m_ProductControllerStateIdle );
    GetHsm().AddState( NotifiedNames_Name( NotifiedNames::NETWORK_STANDBY ), &m_ProductControllerStateNetworkStandby );
    GetHsm().AddState( NotifiedNames_Name( NotifiedNames::IDLE ), &m_ProductControllerStateVoiceConfigured );
    GetHsm().AddState( NotifiedNames_Name( NotifiedNames::IDLE ), &m_ProductControllerStateVoiceNotConfigured );
    GetHsm().AddState( NotifiedNames_Name( NotifiedNames::NETWORK_STANDBY ), &m_ProductControllerStateNetworkConfigured );
    GetHsm().AddState( NotifiedNames_Name( NotifiedNames::NETWORK_STANDBY ), &m_ProductControllerStateNetworkNotConfigured );
    GetHsm().AddState( NotifiedNames_Name( NotifiedNames::FACTORY_DEFAULT ), &m_ProductControllerStateFactoryDefault );
    GetHsm().AddState( NotifiedNames_Name( NotifiedNames::DESELECTED ), &m_ProductControllerStatePlayingDeselected );
    GetHsm().AddState( NotifiedNames_Name( NotifiedNames::SELECTED ), &m_ProductControllerStatePlayingSelected );
    GetHsm().AddState( NotifiedNames_Name( NotifiedNames::SELECTED ), &m_ProductControllerStatePlayingSelectedSilent );
    GetHsm().AddState( NotifiedNames_Name( NotifiedNames::SELECTED ), &m_ProductControllerStatePlayingSelectedNotSilent );
    GetHsm().AddState( NotifiedNames_Name( NotifiedNames::SELECTED ), &m_ProductControllerStatePlayingSelectedSetup );
    GetHsm().AddState( NotifiedNames_Name( NotifiedNames::SELECTED ), &m_ProductControllerStatePlayingSelectedSetupNetwork );
    GetHsm().AddState( NotifiedNames_Name( NotifiedNames::SELECTED ), &m_ProductControllerStatePlayingSelectedSetupNetworkTransition );
    GetHsm().AddState( NotifiedNames_Name( NotifiedNames::SELECTED ), &m_ProductControllerStatePlayingSelectedSetupOther );
    GetHsm().AddState( NotifiedNames_Name( NotifiedNames::SELECTED ), &m_ProductControllerStatePlayingSelectedSetupExiting );
    GetHsm().AddState( NotifiedNames_Name( NotifiedNames::SELECTED ), &m_ProductControllerStatePlayingSelectedSetupExitingAP );
    GetHsm().AddState( "", &m_ProductControllerStateStoppingStreams );
    GetHsm().AddState( "", &m_ProductControllerStatePlayableTransition );
    GetHsm().AddState( "", &m_ProductControllerStatePlayableTransitionInternal );
    GetHsm().AddState( "", &m_ProductControllerStatePlayableTransitionIdle );
    GetHsm().AddState( "", &m_ProductControllerStatePlayableTransitionNetworkStandby );
    GetHsm().AddState( "", &m_ProductControllerStateSoftwareUpdateTransition );
    GetHsm().AddState( "", &m_ProductControllerStatePlayingTransition );
    GetHsm().AddState( NotifiedNames_Name( NotifiedNames::FIRST_BOOT_GREETING ), &m_ProductControllerStateFirstBootGreeting );
    GetHsm().AddState( "", &m_ProductControllerStateFirstBootGreetingTransition );
    GetHsm().AddState( "", &m_ProductControllerStateBootedTransition );
    GetHsm().AddState( "", &m_ProductControllerStatePlayingTransitionSwitch );
    GetHsm().AddState( "", &m_ProductControllerStateStoppingStreamsDedicated );
    GetHsm().AddState( "", &m_ProductControllerStateStoppingStreamsDedicatedForFactoryDefault );
    GetHsm().AddState( "", &m_ProductControllerStateStoppingStreamsDedicatedForSoftwareUpdate );

    GetHsm().Init( this, PRODUCT_CONTROLLER_STATE_BOOTING );
}

void EddieProductController::InitializeAction()
{
    InitializeHsm( );
    CommonInitialize( );

    m_ConfigurationStatusPersistence = ProtoPersistenceFactory::Create( "ConfigurationStatus", g_ProductPersistenceDir );
    m_ConfigurationStatus.mutable_status()->set_language( IsLanguageSet() );
    ReadConfigurationStatusFromPersistence();
    AsyncCallback<bool> uiConnectedCb( std::bind( &EddieProductController::UpdateUiConnectedStatus,
                                                  this, std::placeholders::_1 ), GetTask() ) ;

    m_lightbarController = std::unique_ptr<LightBar::LightBarController>( new LightBar::LightBarController( GetTask(), m_FrontDoorClientIF,  m_LpmInterface->GetLpmClient() ) );
    m_displayController  = std::unique_ptr<DisplayController           >( new DisplayController( *this    , m_FrontDoorClientIF,  m_LpmInterface->GetLpmClient(), uiConnectedCb ) );
    SetupProductSTSController();

    // Start Eddie ProductAudioService
    m_ProductAudioService = std::make_shared< CustomProductAudioService >( *this, m_FrontDoorClientIF, m_LpmInterface->GetLpmClient() );
    m_ProductAudioService -> Run();

    // Start Eddie ProductKeyInputManager
    m_ProductKeyInputManager = std::make_shared< CustomProductKeyInputManager >( *this );
    m_ProductKeyInputManager -> Run();

    // Initialize and register Intents for the Product Controller
    m_IntentHandler.Initialize();

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
/// @name   EddieProductController::Initialize
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void EddieProductController::Initialize( void )
{
    BOSE_INFO( s_logger, __func__ );
    IL::BreakThread( std::bind( &EddieProductController::InitializeAction, this ), GetTask( ) );
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

std::string EddieProductController::GetDefaultProductName() const
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

    // Register lightbar controller LPM events
    m_lightbarController->RegisterLpmEvents();
}

void EddieProductController::RegisterEndPoints()
{
    BOSE_INFO( s_logger, __func__ );
    RegisterCommonEndPoints();

    AsyncCallback<Callback<ProductPb::ConfigurationStatus>, Callback<FrontDoor::Error>> getConfigurationStatusReqCb( std::bind( &EddieProductController::HandleConfigurationStatusRequest ,
            this, std::placeholders::_1 ) , GetTask() );

    /// Registration of endpoints to the frontdoor client.

    m_FrontDoorClientIF->RegisterGet( FRONTDOOR_SYSTEM_CONFIGURATION_STATUS_API , getConfigurationStatusReqCb );
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

void EddieProductController::CallbackError( const FrontDoor::Error &error )
{
    BOSE_WARNING( s_logger, "%s: Error = (%d-%d) %s", __func__, error.code(), error.subcode(), error.message().c_str() );
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

bool EddieProductController::IsAllModuleReady() const
{
    BOSE_INFO( s_logger,
               "%s:|CAPS Ready=%d|LPMReady=%d|AudioPathReady=%d|NetworkModuleReady=%d"
               "|m_isBluetoothReady=%d|STSReady=%d|IsSoftwareUpdateReady=%d|IsUiConnected=%d",
               __func__,
               IsCAPSReady() ,
               IsLpmReady(),
               IsAudioPathReady(),
               IsNetworkModuleReady(),
               IsBluetoothModuleReady(),
               IsSTSReady(),
               IsSoftwareUpdateReady(),
               IsUiConnected() );

    return ( IsCAPSReady() and
             IsLpmReady() and
             IsAudioPathReady() and
             IsNetworkModuleReady() and
             IsBluetoothModuleReady() and
             IsSTSReady() and
             IsSoftwareUpdateReady() and
             IsUiConnected() and
             IsSassReady() ) ;
}

bool EddieProductController::IsBtLeModuleReady() const
{
    BOSE_INFO( s_logger, "%s:|m_isBLEModuleReady[%d", __func__, m_isBLEModuleReady );
    return m_isBLEModuleReady;
}

bool EddieProductController::IsUiConnected() const
{
    BOSE_INFO( s_logger, "%s:m_isUiConnected-%d", __func__, m_isUiConnected );
    return m_isUiConnected;
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
                                             "command to set the display controller automatic mode",
                                             "setDisplayAutoMode auto|manual",
                                             GetTask(),
                                             cb,
                                             static_cast<int>( CLICmdKeys::SET_DISPLAY_AUTO_MODE ) );

    m_CliClientMT.RegisterCLIServerCommands( "product boot_status",
                                             "command to output the status of the boot up state.",
                                             "\t product boot_status \t\t\t",
                                             GetTask(),
                                             cb,
                                             static_cast<int>( CLICmdKeys::GET_BOOT_STATUS ) );
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
    case CLICmdKeys::GET_BOOT_STATUS:
    {
        HandleGetBootStatus( argList, response );
        break;
    }
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

void EddieProductController::HandleGetBootStatus( const std::list<std::string>& argList, std::string& response )
{
    std::string CapsInitialized( IsCAPSReady( )                 ? "true" : "false" );
    std::string LpmConnected( IsLpmReady( )                     ? "true" : "false" );
    std::string audioPathConnected( IsAudioPathReady( )         ? "true" : "false" );
    std::string networkModuleReady( IsNetworkModuleReady( )     ? "true" : "false" );

    std::string StsInitialized( IsSTSReady( )                   ? "true" : "false" );
    std::string bluetoothInitialized( IsBluetoothModuleReady( ) ? "true" : "false" );
    std::string UiConnected( IsUiConnected( )                   ? "true" : "false" );

    std::string SassInitialized( IsSassReady( )                 ? "true" : "false" );
    std::string SoftwareUpdateReady( IsSoftwareUpdateReady( )   ? "true" : "false" );

    response  = "------------- Eddie Product Controller Booting Status -------------\n";
    response += "\n";
    response += "CAPS Initialized      : ";
    response += CapsInitialized;
    response += "\n";
    response += "LPM Connected         : ";
    response += LpmConnected;
    response += "\n";
    response += "Audio Path Connected  : ";
    response += audioPathConnected;
    response += "\n";
    response += "Network Module Ready  : ";
    response += networkModuleReady;
    response += "\n";
    response += "STS Initialized       : ";
    response += StsInitialized;
    response += "\n";
    response += "Bluetooth Initialized : ";
    response += bluetoothInitialized;
    response += "\n";
    response += "UI Connected          : ";
    response += UiConnected;
    response += "\n";
    response += "Software Update Ready : ";
    response += SoftwareUpdateReady;
    response += "\n";
    response += "SASS Initialized      : ";
    response += SassInitialized;
    response += "\n";
    response += "\n";

    if( IsBooted( ) )
    {
        response += "The device has been successfully booted.";
    }
    else
    {
        response += "The device has not yet been booted.";
    }
}

void EddieProductController::UpdateUiConnectedStatus( bool status )
{
    BOSE_INFO( s_logger, "%s|status:%s", __func__ , status ? "true" : "false" );
    m_isUiConnected = status;
    GetHsm().Handle<bool>( &ProductControllerState::HandleUiConnectedUpdateState, status );
}

void EddieProductController::HandleProductMessage( const ProductMessage& productMessage )
{
    BOSE_INFO( s_logger, "%s", __func__ );

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// LPM status messages has both Common handling and Professor-specific handling
    ///////////////////////////////////////////////////////////////////////////////////////////////
    if( productMessage.has_lpmstatus( ) )
    {
        // First do the Eddie-specific stuff, i.e., register callbacks and thermal task control
        if( productMessage.lpmstatus( ).has_connected( ) && productMessage.lpmstatus( ).connected( ) )
        {
            RegisterLpmEvents();
        }
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
                break;
            case SYSTEM_STATE_UPDATE:
                break;
            case SYSTEM_STATE_SHUTDOWN:
                break;
            case SYSTEM_STATE_FACTORY_DEFAULT:
                break;
            case SYSTEM_STATE_IDLE:
                break;
            case SYSTEM_STATE_NUM_OF:
                break;
            case SYSTEM_STATE_ERROR:
                break;
            }
        }

        // Then (after registering for events above) do the common stuff
        ( void ) HandleCommonProductMessage( productMessage );
    }
    else if( productMessage.has_action() )
    {
        HandleIntents( productMessage.action() );
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// Key action intent messages are handled at this point, and passed to the HandleIntents
    /// method for processing. This messages are sent through the CustomProductKeyInputManager
    /// class that was instantiate and run when the product controller was constructed.
    ///////////////////////////////////////////////////////////////////////////////////////////////
    else if( productMessage.has_action( ) )
    {
        HandleIntents( productMessage.action( ) );
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// Common ProductMessage elements are handled last, any events with overrides to
    /// the Common elements will have been handled above and not get here
    ///////////////////////////////////////////////////////////////////////////////////////////////
    else if( !HandleCommonProductMessage( productMessage ) )
    {
        BOSE_ERROR( s_logger, "An unknown message type was received - %s.", ProtoToMarkup::ToJson( productMessage ).c_str() );
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
    CustomProductSTSAuxStateFactory    auxStateFactory;
    ProductSTSSilentStateFactory       silentStateFactory;

    std::vector<ProductSTSController::SourceDescriptor> sources;
    ProductSTSController::SourceDescriptor descriptor_AUX{ 0, "AUX", true, auxStateFactory };
    sources.push_back( descriptor_AUX );

    // 'SETUP' is a "fake" source used for setup state.
    ProductSTSController::SourceDescriptor descriptor_Setup{ 1, "SETUP", false, silentStateFactory };
    sources.push_back( descriptor_Setup );

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

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @name   IsLowPowerExited
/// @return This method returns a true or false value, based on a series of set member variables,
///         which all must be true to indicate that the device has exited low power and all modules
///         have come back
////////////////////////////////////////////////////////////////////////////////////////////////////
bool EddieProductController::IsLowPowerExited( ) const
{
    return IsAllModuleReady();
}
} /// namespace ProductApp
