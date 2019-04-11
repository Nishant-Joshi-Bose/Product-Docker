////////////////////////////////////////////////////////////////////////////////
/// @file   CustomProductController.cpp
/// @brief  Product controller class.
///
/// @attention Copyright 2017 Bose Corporation, Framingham, MA
////////////////////////////////////////////////////////////////////////////////

#include "CustomProductController.h"
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
#include "ProductSTSStateFactory.h"
#include "ProductSTSStateTopSetup.h"
#include "CustomProductSTSStateTopAux.h"
#include "ProductSTS.pb.h"
#include "SystemUtils.h"
#include "ProductCommandLine.h"
#include "CommonProductCommandLine.h"
#include "IoTIPCClient.h"

static DPrint s_logger( "CustomProductController" );

static constexpr char PRODUCT_CONFIG_FILE_PATH[] = "/opt/Bose/etc/ProductConfig.json";

using namespace DeviceManagerPb;

namespace ProductApp
{
const std::string g_ProductPersistenceDir = "product-persistence/";
const std::string g_DefaultCAPSValuesStateFile  = "DefaultCAPSValuesDone";

CustomProductController::CustomProductController():
    m_ProductControllerStateTop( GetHsm(), nullptr ),
    m_ProductControllerStateBooting( GetHsm(), &m_ProductControllerStateTop, PRODUCT_CONTROLLER_STATE_BOOTING ),
    m_ProductControllerStateBooted( GetHsm(), &m_ProductControllerStateTop, PRODUCT_CONTROLLER_STATE_BOOTED ),
    m_CustomProductControllerStateOn( GetHsm(), &m_ProductControllerStateTop, CUSTOM_PRODUCT_CONTROLLER_STATE_ON ),
    m_ProductControllerStateLowPowerResume( GetHsm(), &m_ProductControllerStateTop, PRODUCT_CONTROLLER_STATE_LOW_POWER_RESUME ),
    m_CustomProductControllerStateLowPowerStandby( GetHsm(), &m_ProductControllerStateTop, CUSTOM_PRODUCT_CONTROLLER_STATE_LOW_POWER_STANDBY ),
    m_ProductControllerStateCriticalError( GetHsm(), &m_ProductControllerStateTop, PRODUCT_CONTROLLER_STATE_CRITICAL_ERROR ),
    m_ProductControllerStatePlaying( GetHsm(), &m_CustomProductControllerStateOn, PRODUCT_CONTROLLER_STATE_PLAYING ),
    m_ProductControllerStatePlayable( GetHsm(), &m_CustomProductControllerStateOn, PRODUCT_CONTROLLER_STATE_PLAYABLE ),
    m_CustomProductControllerStateLowPowerStandbyTransition( GetHsm(), &m_CustomProductControllerStateLowPowerStandby, PRODUCT_CONTROLLER_STATE_LOW_POWER_STANDBY_TRANSITION ),
    m_ProductControllerStateIdle( GetHsm(), &m_ProductControllerStatePlayable, PRODUCT_CONTROLLER_STATE_IDLE ),
    m_CustomProductControllerStateNetworkStandby( GetHsm(), &m_ProductControllerStatePlayable, PRODUCT_CONTROLLER_STATE_NETWORK_STANDBY ),
    m_ProductControllerStateVoiceConfigured( GetHsm(), &m_ProductControllerStateIdle, PRODUCT_CONTROLLER_STATE_IDLE_VOICE_CONFIGURED ),
    m_ProductControllerStateVoiceNotConfigured( GetHsm(), &m_ProductControllerStateIdle, PRODUCT_CONTROLLER_STATE_IDLE_VOICE_NOT_CONFIGURED ),
    m_ProductControllerStateNetworkConfigured( GetHsm(), &m_CustomProductControllerStateNetworkStandby, PRODUCT_CONTROLLER_STATE_NETWORK_STANDBY_CONFIGURED ),
    m_ProductControllerStateNetworkNotConfigured( GetHsm(), &m_CustomProductControllerStateNetworkStandby, PRODUCT_CONTROLLER_STATE_NETWORK_STANDBY_NOT_CONFIGURED ),
    m_ProductControllerStateFactoryDefault( GetHsm(), &m_ProductControllerStateTop, PRODUCT_CONTROLLER_STATE_FACTORY_DEFAULT ),
    m_ProductControllerStatePlayingDeselected( GetHsm(), &m_ProductControllerStatePlaying, PRODUCT_CONTROLLER_STATE_PLAYING_DESELECTED ),
    m_ProductControllerStatePlayingSelected( GetHsm(), &m_ProductControllerStatePlaying, PRODUCT_CONTROLLER_STATE_PLAYING_SELECTED ),
    m_ProductControllerStatePlayingSelectedSilent( GetHsm(), &m_ProductControllerStatePlayingSelected, PRODUCT_CONTROLLER_STATE_PLAYING_SELECTED_SILENT ),
    m_ProductControllerStatePlayingSelectedSilentSourceInvalid( GetHsm(), &m_ProductControllerStatePlayingSelectedSilent, PRODUCT_CONTROLLER_STATE_PLAYING_SELECTED_SILENT_SOURCE_INVALID ),
    m_ProductControllerStatePlayingSelectedSilentSourceValid( GetHsm(), &m_ProductControllerStatePlayingSelectedSilent, PRODUCT_CONTROLLER_STATE_PLAYING_SELECTED_SILENT_SOURCE_VALID ),
    m_ProductControllerStatePlayingSelectedNotSilent( GetHsm(), &m_ProductControllerStatePlayingSelected, PRODUCT_CONTROLLER_STATE_PLAYING_SELECTED_NOT_SILENT ),
    m_ProductControllerStatePlayingSelectedSetup( GetHsm(), &m_ProductControllerStatePlayingSelected, PRODUCT_CONTROLLER_STATE_PLAYING_SELECTED_SETUP ),
    m_ProductControllerStatePlayingSelectedSetupNetworkConfig( GetHsm(), &m_ProductControllerStatePlayingSelectedSetup, PRODUCT_CONTROLLER_STATE_PLAYING_SELECTED_SETUP_NETWORK_CONFIG ),
    m_ProductControllerStatePlayingSelectedSetupNetworkConfigWiFiConnection( GetHsm(), &m_ProductControllerStatePlayingSelectedSetupNetworkConfig, PRODUCT_CONTROLLER_STATE_PLAYING_SELECTED_SETUP_NETWORK_CONFIG_WIFI_CONNECTION ),
    m_ProductControllerStatePlayingSelectedSetupNetworkConfigWiFiTransition( GetHsm(), &m_ProductControllerStatePlayingSelectedSetupNetworkConfig, PRODUCT_CONTROLLER_STATE_PLAYING_SELECTED_SETUP_NETWORK_CONFIG_WIFI_TRANSITION ),
    m_ProductControllerStatePlayingSelectedSetupOther( GetHsm(), &m_ProductControllerStatePlayingSelectedSetup, PRODUCT_CONTROLLER_STATE_PLAYING_SELECTED_SETUP_OTHER ),
    m_ProductControllerStatePlayingSelectedSetupExiting( GetHsm(), &m_ProductControllerStatePlayingSelectedSetup, PRODUCT_CONTROLLER_STATE_PLAYING_SELECTED_SETUP_EXITING ),
    m_ProductControllerStatePlayingSelectedSetupNetworkConfigWiFiExiting( m_ProductControllerHsm, &m_ProductControllerStatePlayingSelectedSetupNetworkConfig, PRODUCT_CONTROLLER_STATE_PLAYING_SELECTED_SETUP_NETWORK_CONFIG_WIFI_EXITING ),
    m_ProductControllerStatePlayingSelectedSetupNetworkConfigWiFiAborting( m_ProductControllerHsm, &m_ProductControllerStatePlayingSelectedSetupNetworkConfig, PRODUCT_CONTROLLER_STATE_PLAYING_SELECTED_SETUP_NETWORK_CONFIG_WIFI_ABORTING ),
    m_ProductControllerStatePlayingSelectedStoppingStreams( GetHsm(), &m_ProductControllerStatePlayingSelected, PRODUCT_CONTROLLER_STATE_PLAYING_SELECTED_STOPPING_STREAMS ),
    m_ProductControllerStatePlayableTransition( GetHsm(), &m_ProductControllerStateTop, PRODUCT_CONTROLLER_STATE_PLAYABLE_TRANSITION ),
    m_ProductControllerStatePlayableTransitionInternal( GetHsm(), &m_ProductControllerStatePlayableTransition, PRODUCT_CONTROLLER_STATE_PLAYABLE_TRANSITION_INTERNAL ),
    m_ProductControllerStatePlayableTransitionIdle( GetHsm(), &m_ProductControllerStatePlayableTransitionInternal, PRODUCT_CONTROLLER_STATE_PLAYABLE_TRANSITION_IDLE ),
    m_ProductControllerStatePlayableTransitionNetworkStandby( GetHsm(), &m_ProductControllerStatePlayableTransitionInternal, PRODUCT_CONTROLLER_STATE_PLAYABLE_TRANSITION_NETWORK_STANDBY ),
    m_ProductControllerStatePlayingTransition( GetHsm(), &m_ProductControllerStateTop, PRODUCT_CONTROLLER_STATE_PLAYING_TRANSITION ),
    m_ProductControllerStateFirstBootGreeting( GetHsm(), &m_ProductControllerStateTop, PRODUCT_CONTROLLER_STATE_FIRST_BOOT_GREETING ),
    m_ProductControllerStateFirstBootGreetingTransition( GetHsm(), &m_ProductControllerStateTop, PRODUCT_CONTROLLER_STATE_FIRST_BOOT_GREETING_TRANSITION ),
    m_ProductControllerStatePlayingTransitionSwitch( GetHsm(), &m_ProductControllerStatePlayingTransition, PRODUCT_CONTROLLER_STATE_PLAYING_TRANSITION_SWITCH ),
    m_ProductControllerStateStoppingStreamsDedicated( m_ProductControllerHsm, &m_ProductControllerStateTop, PRODUCT_CONTROLLER_STATE_STOPPING_STREAMS_DEDICATED ),
    m_ProductControllerStateStoppingStreamsDedicatedForFactoryDefault( m_ProductControllerHsm, &m_ProductControllerStateStoppingStreamsDedicated, PRODUCT_CONTROLLER_STATE_STOPPING_STREAMS_DEDICATED_FOR_FACTORY_DEFAULT ),
    m_ProductControllerStateStoppingStreamsDedicatedForSoftwareUpdate( m_ProductControllerHsm, &m_ProductControllerStateStoppingStreamsDedicated, PRODUCT_CONTROLLER_STATE_STOPPING_STREAMS_DEDICATED_FOR_SOFTWARE_UPDATE ),
    m_ProductControllerStateSoftwareInstall( GetHsm(), &m_ProductControllerStateTop, PRODUCT_CONTROLLER_STATE_SOFTWARE_INSTALL ),
    m_ProductControllerStateSoftwareInstallTransition( GetHsm(), &m_ProductControllerStateTop, PRODUCT_CONTROLLER_STATE_SOFTWARE_INSTALL_TRANSITION ),
    m_ProductControllerStateSoftwareInstallManual( GetHsm(), &m_ProductControllerStateTop, PRODUCT_CONTROLLER_STATE_SOFTWARE_INSTALL_MANUAL ),
    m_ProductCommandLine( std::make_shared< ProductCommandLine >( *this ) ),
    m_CommonProductCommandLine( ),
    m_IntentHandler( *GetTask(), GetCommonCliClientMT(), m_FrontDoorClientIF, *this ),
    m_LpmInterface( std::make_shared< CustomProductLpmHardwareInterface >( *this ) ),
    m_ProductSTSController( *this ),
    m_ProductIotHandler( GetTask(),
                         "/opt/Bose/etc/ProductInputs.json",
                         m_FrontDoorClientIF,
                         std::make_shared<IoTIPCClient>( GetTask(), "" ) )
{
    BOSE_INFO( s_logger, __func__ );
}

CustomProductController::~CustomProductController()
{
}

void CustomProductController::InitializeHsm()
{
    /// Add States to HSM object and initialize HSM before doing anything else.
    using namespace SystemPowerPb;
    GetHsm().AddState( Device_State_Not_Notify,
                       SystemPowerControl_State_Not_Notify,
                       &m_ProductControllerStateTop );

    GetHsm().AddState( Device_State_Not_Notify,
                       SystemPowerControl_State_Not_Notify,
                       &m_ProductControllerStateLowPowerResume );

    GetHsm().AddState( Device_State_Not_Notify,
                       SystemPowerControl_State_OFF,
                       &m_CustomProductControllerStateLowPowerStandby );

    GetHsm().AddState( NotifiedNames::BOOTING,
                       SystemPowerControl_State_Not_Notify,
                       &m_ProductControllerStateBooting );

    GetHsm().AddState( Device_State_Not_Notify,
                       SystemPowerControl_State_Not_Notify,
                       &m_ProductControllerStateBooted );

    GetHsm().AddState( Device_State_Not_Notify,
                       SystemPowerControl_State_Not_Notify,
                       &m_CustomProductControllerStateOn );

    GetHsm().AddState( NotifiedNames::CRITICAL_ERROR,
                       SystemPowerControl_State_OFF,
                       &m_ProductControllerStateCriticalError );

    GetHsm().AddState( Device_State_Not_Notify,
                       SystemPowerControl_State_Not_Notify,
                       &m_ProductControllerStatePlaying );

    GetHsm().AddState( Device_State_Not_Notify,
                       SystemPowerControl_State_OFF,
                       &m_ProductControllerStatePlayable );

    GetHsm().AddState( Device_State_Not_Notify,
                       SystemPowerControl_State_Not_Notify,
                       &m_CustomProductControllerStateLowPowerStandbyTransition );

    GetHsm().AddState( NotifiedNames::IDLE,
                       SystemPowerControl_State_OFF,
                       &m_ProductControllerStateIdle );

    GetHsm().AddState( NotifiedNames::NETWORK_STANDBY,
                       SystemPowerControl_State_OFF,
                       &m_CustomProductControllerStateNetworkStandby );

    GetHsm().AddState( NotifiedNames::IDLE,
                       SystemPowerControl_State_OFF,
                       &m_ProductControllerStateVoiceConfigured );

    GetHsm().AddState( NotifiedNames::IDLE,
                       SystemPowerControl_State_OFF,
                       &m_ProductControllerStateVoiceNotConfigured );

    GetHsm().AddState( NotifiedNames::NETWORK_STANDBY,
                       SystemPowerControl_State_OFF,
                       &m_ProductControllerStateNetworkConfigured );

    GetHsm().AddState( NotifiedNames::NETWORK_STANDBY,
                       SystemPowerControl_State_OFF,
                       &m_ProductControllerStateNetworkNotConfigured );

    GetHsm().AddState( NotifiedNames::FACTORY_DEFAULT,
                       SystemPowerControl_State_Not_Notify,
                       &m_ProductControllerStateFactoryDefault );

    GetHsm().AddState( NotifiedNames::PLAYING_SOURCE_OFF,
                       SystemPowerControl_State_OFF,
                       &m_ProductControllerStatePlayingDeselected );

    GetHsm().AddState( NotifiedNames::SELECTED,
                       SystemPowerControl_State_Not_Notify,
                       &m_ProductControllerStatePlayingSelected );

    GetHsm().AddState( NotifiedNames::SELECTED,
                       SystemPowerControl_State_Not_Notify,
                       &m_ProductControllerStatePlayingSelectedSilent );

    GetHsm().AddState( NotifiedNames::PLAYING_SOURCE_OFF,
                       SystemPowerControl_State_OFF,
                       &m_ProductControllerStatePlayingSelectedSilentSourceInvalid );

    GetHsm().AddState( NotifiedNames::SELECTED,
                       SystemPowerControl_State_ON,
                       &m_ProductControllerStatePlayingSelectedSilentSourceValid );

    GetHsm().AddState( NotifiedNames::SELECTED,
                       SystemPowerControl_State_ON,
                       &m_ProductControllerStatePlayingSelectedNotSilent );

    GetHsm().AddState( NotifiedNames::SELECTED,
                       SystemPowerControl_State_ON,
                       &m_ProductControllerStatePlayingSelectedSetup );

    GetHsm().AddState( NotifiedNames::NETWORK_CONFIG,
                       SystemPowerControl_State_ON,
                       &m_ProductControllerStatePlayingSelectedSetupNetworkConfig );

    GetHsm().AddState( NotifiedNames::NETWORK_CONFIG,
                       SystemPowerControl_State_ON,
                       &m_ProductControllerStatePlayingSelectedSetupNetworkConfigWiFiConnection );

    GetHsm().AddState( NotifiedNames::NETWORK_CONFIG,
                       SystemPowerControl_State_Not_Notify,
                       &m_ProductControllerStatePlayingSelectedSetupNetworkConfigWiFiTransition );

    GetHsm().AddState( NotifiedNames::SELECTED,
                       SystemPowerControl_State_ON,
                       &m_ProductControllerStatePlayingSelectedSetupOther );

    GetHsm().AddState( NotifiedNames::SELECTED,
                       SystemPowerControl_State_Not_Notify,
                       &m_ProductControllerStatePlayingSelectedSetupExiting );

    GetHsm().AddState( NotifiedNames::NETWORK_CONFIG,
                       SystemPowerControl_State_Not_Notify,
                       &m_ProductControllerStatePlayingSelectedSetupNetworkConfigWiFiExiting );

    GetHsm().AddState( NotifiedNames::NETWORK_CONFIG,
                       SystemPowerControl_State_ON,
                       &m_ProductControllerStatePlayingSelectedSetupNetworkConfigWiFiAborting );

    GetHsm().AddState( Device_State_Not_Notify,
                       SystemPowerControl_State_Not_Notify,
                       &m_ProductControllerStatePlayingSelectedStoppingStreams );

    GetHsm().AddState( Device_State_Not_Notify,
                       SystemPowerControl_State_Not_Notify,
                       &m_ProductControllerStatePlayableTransition );

    GetHsm().AddState( Device_State_Not_Notify,
                       SystemPowerControl_State_Not_Notify,
                       &m_ProductControllerStatePlayableTransitionInternal );

    GetHsm().AddState( Device_State_Not_Notify,
                       SystemPowerControl_State_Not_Notify,
                       &m_ProductControllerStatePlayableTransitionIdle );

    GetHsm().AddState( Device_State_Not_Notify,
                       SystemPowerControl_State_Not_Notify,
                       &m_ProductControllerStatePlayableTransitionNetworkStandby );

    GetHsm().AddState( Device_State_Not_Notify,
                       SystemPowerControl_State_Not_Notify,
                       &m_ProductControllerStatePlayingTransition );

    GetHsm().AddState( NotifiedNames::FIRST_BOOT_GREETING,
                       SystemPowerControl_State_Not_Notify,
                       &m_ProductControllerStateFirstBootGreeting );

    GetHsm().AddState( Device_State_Not_Notify,
                       SystemPowerControl_State_Not_Notify,
                       &m_ProductControllerStateFirstBootGreetingTransition );

    GetHsm().AddState( Device_State_Not_Notify,
                       SystemPowerControl_State_Not_Notify,
                       &m_ProductControllerStatePlayingTransitionSwitch );

    GetHsm().AddState( Device_State_Not_Notify,
                       SystemPowerControl_State_Not_Notify,
                       &m_ProductControllerStateStoppingStreamsDedicated );

    GetHsm().AddState( Device_State_Not_Notify,
                       SystemPowerControl_State_Not_Notify,
                       &m_ProductControllerStateStoppingStreamsDedicatedForFactoryDefault );

    GetHsm().AddState( Device_State_Not_Notify,
                       SystemPowerControl_State_Not_Notify,
                       &m_ProductControllerStateStoppingStreamsDedicatedForSoftwareUpdate );

    GetHsm().AddState( NotifiedNames::UPDATING,
                       SystemPowerControl_State_Not_Notify,
                       &m_ProductControllerStateSoftwareInstall );

    GetHsm().AddState( Device_State_Not_Notify,
                       SystemPowerControl_State_Not_Notify,
                       &m_ProductControllerStateSoftwareInstallTransition );

    GetHsm().AddState( NotifiedNames::UPDATING_MANUAL,
                       SystemPowerControl_State_Not_Notify,
                       &m_ProductControllerStateSoftwareInstallManual );

    GetHsm().Init( this, PRODUCT_CONTROLLER_STATE_BOOTING );
}

void CustomProductController::InitializeAction()
{
    InitializeHsm( );
    CommonInitialize( );

    ProductDependentInitialize();

    LpmClientLiteIF::LpmClientLitePtr lpmLitePtr( std::static_pointer_cast<LpmClientLiteIF>( m_LpmInterface->GetLpmClient( ) ) );
    m_lightbarController = std::unique_ptr<LightBar::LightBarController>( new LightBar::LightBarController( GetTask(), m_FrontDoorClientIF,  lpmLitePtr ) );

    // Start ProductAudioService
    m_ProductAudioService = std::make_shared< CustomProductAudioService >( *this, m_FrontDoorClientIF, m_LpmInterface->GetLpmClient() );
    m_ProductAudioService -> Run();

    // Start ProductKeyInputManager
    m_ProductKeyInputManager = std::make_shared< CustomProductKeyInputManager >( *this );
    AsyncCallback<> cancelAlarmCb( std::bind( &ProductController::CancelAlarm, this ), GetTask( ) );

    m_ProductKeyInputManager -> Run( cancelAlarmCb );

    // Initialize and register Intents for the Product Controller
    m_IntentHandler.Initialize();

    ///Instantiate and run the hardware interface.
    m_LpmInterface->Run( );

    m_ProductCommandLine -> Run();
    RegisterCommonEndPoints();
    SendInitialRequests();

    ///Register lpm events that lightbar will handle
    m_lightbarController->RegisterLightBarEndPoints();
    m_displayController->Initialize();

    m_ProductIotHandler.Run();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   CustomProductController::Initialize
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductController::Initialize( void )
{
    BOSE_INFO( s_logger, __func__ );
    IL::BreakThread( std::bind( &CustomProductController::InitializeAction, this ), GetTask( ) );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   CustomProductController::ProductDependentInitialize
/// @brief  Function to handle product specific items that are located in a Product config file
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductController::ProductDependentInitialize()
{
    ProductPb::ProductConfig productConfig;

    LoadProductConfiguration( productConfig );

    const auto& thisProductConfig = productConfig.productdetails( FindThisProductConfig( productConfig ) );

    DisplayController::Configuration displayCtrlConfig;
    displayCtrlConfig.m_hasLightSensor = thisProductConfig.lightsensoravailable();
    displayCtrlConfig.m_hasLcd = thisProductConfig.lcdavailable();
    displayCtrlConfig.m_blackScreenDetectEnabled = thisProductConfig.blackscreendetectenabled();

    AsyncCallback<bool> uiConnectedCb( std::bind( &CustomProductController::UpdateUiConnectedStatus,
                                                  this, std::placeholders::_1 ), GetTask() ) ;

    m_displayController = std::make_shared<DisplayController>( displayCtrlConfig, *this, m_FrontDoorClientIF, m_LpmInterface->GetLpmClient(), uiConnectedCb );

    if( thisProductConfig.clockavailable() )
    {
        BOSE_INFO( s_logger, "%s: Product has a clock, initialize Clock", __func__ );
        m_clock = std::make_shared<Clock>( m_FrontDoorClientIF, GetTask(), GetProductGuid() );
        m_clock->Initialize( );
    }

    // Sanity check...
    if( m_clock && not displayCtrlConfig.m_hasLcd )
    {
        BOSE_DIE( "Product Config file specifies clock but no LCD!!!" );
    }
}

///////////////////////////////////////////////////////////////////////////////
/// @name  LoadProductConfiguration
/// @brief Function to load the Product Configuration Json from a predetermined location.
////////////////////////////////////////////////////////////////////////////////
void CustomProductController::LoadProductConfiguration( ProductPb::ProductConfig& productConfig )
{
    BOSE_INFO( s_logger, "%s: Load Product Controller's Product Configuration:", __func__ );

    BOptional<std::string> cfg = SystemUtils::ReadFile( PRODUCT_CONFIG_FILE_PATH );

    if( !cfg )
    {
        BOSE_DIE( "Product config file: " << PRODUCT_CONFIG_FILE_PATH << " NOT found" );
    }

    try
    {
        ProtoToMarkup::FromJson( *cfg, &productConfig );
    }
    catch( const ProtoToMarkup::MarkupError &e )
    {
        BOSE_DIE( "Product config from disk failed markup error - " << e.what() );
    }
}

int CustomProductController::FindThisProductConfig( ProductPb::ProductConfig& productConfig )
{
    auto productType = GetProductType();

    for( auto productIndex = 0; productIndex < productConfig.productdetails_size(); productIndex++ )
    {
        if( productConfig.productdetails( productIndex ).product() == productType )
        {
            BOSE_INFO( s_logger, "%s: Product Type %s, found in config file at index %d", __func__, productType.c_str(), productIndex );
            return productIndex;
        }
    }
    BOSE_DIE( "Product Type " << productType << " NOT found in config file:: " );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief  CustomProductController::GetMessageHandler
///
/// @return Callback < ProductMessage >
///
////////////////////////////////////////////////////////////////////////////////////////////////////
Callback < ProductMessage > CustomProductController::GetMessageHandler( )
{
    Callback < ProductMessage >
    ProductMessageHandler( std::bind( &CustomProductController::HandleProductMessage,
                                      this,
                                      std::placeholders::_1 ) );
    return ProductMessageHandler;
}

std::string CustomProductController::GetDefaultProductName() const
{
    std::string productName;

    // Ensure that the device has a valid marketing product name, based on the manufacturing
    // data, and assign this value to the default product name initially.
    if( auto productNameValue = MfgData::Get( "productName" ) )
    {
        productName = *productNameValue;
    }
    else
    {
        BOSE_DIE( __func__ << " Fatal Error: No Product Name " );
    }

    // Leave the default product name assigned to the marketing product name in the manufacturing
    // data for production non-development devices; otherwise, assign the default product name
    // based on its MAC address and product name which is extracted from the Product Config file
    // at startup.
    if( IsDevelopmentMode() )
    {
        std::string macAddress = MacAddressInfo::GetPrimaryMAC();
        try
        {
            productName = ( macAddress.substr( macAddress.length() - 6 ) );
        }
        catch( const std::out_of_range& error )
        {
            productName = macAddress;
            BOSE_WARNING( s_logger, "errorType = %s", error.what() );
        }

        std::string productType;

        if( auto productTypeValue = MfgData::Get( "productType" ) )
        {
            productType = *productTypeValue;
            if( productType.compare( "eddie" ) == 0 )
            {
                productName += " HS 500";
            }
            else if( productType.compare( "eddieclub" ) == 0 )
            {
                productName += " HS 450";
            }
            else
            {
                BOSE_DIE( __func__ << " Fatal Error: Invalid Product Type " <<  productType );
            }
        }
    }
    BOSE_INFO( s_logger, "%s productName=%s", __func__, productName.c_str() );
    return productName;
}

void CustomProductController::RegisterLpmEvents()
{
    BOSE_INFO( s_logger, __func__ );

    // Register lightbar controller LPM events
    m_lightbarController->RegisterLpmEvents();
    m_displayController->RegisterLpmEvents();
}

void CustomProductController::RequestAuxCableState()
{
    BOSE_INFO( s_logger, __func__ );

    if( m_AuxCableStateCb )
    {
        GetLpmHardwareInterface()->GetLpmClient()->IpcGetAuxState( m_AuxCableStateCb.get() );
    }
}

void CustomProductController::RegisterAuxEvents( AsyncCallback<LpmServiceMessages::IpcAuxState_t> &cb )
{
    BOSE_INFO( s_logger, __func__ );

    // Cache this callback (cb), so it can be used for getting Aux State from LPM during Low Power Resume.
    m_AuxCableStateCb = cb;

    GetLpmHardwareInterface()->GetLpmClient()->IpcGetAuxState( cb );

    GetLpmHardwareInterface()->RegisterForLpmEvents<LpmServiceMessages::IpcAuxState_t>( IPC_AUX_STATE_EVENT, cb );
}

void CustomProductController::SendInitialRequests()
{
    BOSE_INFO( s_logger, __func__ );

    SendCommonInitialRequests();

    {
        AsyncCallback<std::list<std::string> > poiReadyCb( std::bind( &CustomProductController::HandleBluetoothCapabilityReady, this, std::placeholders::_1 ), GetTask() );
        AsyncCallback<std::list<std::string> > poiNotReadyCb( std::bind( &CustomProductController::HandleBluetoothCapabilityNotReady, this, std::placeholders::_1 ), GetTask() );
        std::list<std::string> endPoints;

        endPoints.push_back( FRONTDOOR_BLUETOOTH_SINK_LIST_API );
        m_FrontDoorClientIF->RegisterEndpointsOfInterest( endPoints, poiReadyCb,  poiNotReadyCb );
    }

    {
        AsyncCallback<std::list<std::string> > poiReadyCb( std::bind( &CustomProductController::HandleBtLeCapabilityReady, this, std::placeholders::_1 ), GetTask() );
        AsyncCallback<std::list<std::string> > poiNotReadyCb( std::bind( &CustomProductController::HandleBtLeCapabilityNotReady, this, std::placeholders::_1 ), GetTask() );
        std::list<std::string> endPoints;

        endPoints.push_back( BLESetupEndpoints::STATUS_NOTIF );
        m_FrontDoorClientIF->RegisterEndpointsOfInterest( endPoints, poiReadyCb,  poiNotReadyCb );
    }
}

void CustomProductController::CallbackError( const FrontDoor::Error &error )
{
    BOSE_WARNING( s_logger, "%s: Error = (%d-%d) %s", __func__, error.code(), error.subcode(), error.message().c_str() );
}

void CustomProductController::HandleSTSReady( void )
{
    BOSE_DEBUG( s_logger, __func__ );
    m_isSTSReady = true;
    GetHsm().Handle<>( &CustomProductControllerState::HandleSTSSourcesInit );
}

bool CustomProductController::IsAllModuleReady() const
{
    BOSE_INFO( s_logger,
               "%s:|CAPS=%d|LPM=%d|AudioPath=%d|Network=%d|Bluetooth=%d"
               "|STS=%d|SWUpdate=%d|ProductUI=%d|SASS=%d|Voice=%d",
               __func__,
               IsCAPSReady(),
               IsLpmReady(),
               IsAudioPathReady(),
               IsNetworkModuleReady(),
               IsBluetoothModuleReady(),
               IsSTSReady(),
               IsSoftwareUpdateReady(),
               IsUiConnected(),
               IsSassReady(),
               IsVoiceModuleReady() );

    return ( IsCAPSReady() and
             IsLpmReady() and
             IsAudioPathReady() and
             IsNetworkModuleReady() and
             IsBluetoothModuleReady() and
             IsSTSReady() and
             IsSoftwareUpdateReady() and
             IsUiConnected() and
             IsSassReady() and
             IsVoiceModuleReady() ) ;
}

bool CustomProductController::IsBtLeModuleReady() const
{
    BOSE_INFO( s_logger, "%s:|m_isBLEModuleReady[%d", __func__, m_isBLEModuleReady );
    return m_isBLEModuleReady;
}

bool CustomProductController::IsUiConnected() const
{
    BOSE_INFO( s_logger, "%s:m_isUiConnected-%d", __func__, m_isUiConnected );
    return m_isUiConnected;
}

bool CustomProductController::IsSTSReady() const
{
    BOSE_INFO( s_logger, "%s:STS Ready=%d", __func__, m_isSTSReady );
    return m_isSTSReady;
}

bool CustomProductController::IsLanguageSet()
{
    return m_deviceManager.IsLanguageSet();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductController::SendInitialCapsData
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductController::SendInitialCapsData()
{
    BOSE_INFO( s_logger, __func__ );

    using namespace SoundTouchInterface;

    std::string DefaultCAPSValuesStateFile{ g_PersistenceRootDir };
    DefaultCAPSValuesStateFile += g_ProductPersistenceDir;
    DefaultCAPSValuesStateFile += g_DefaultCAPSValuesStateFile;
    const bool defaultCAPSValuesDone = SystemUtils::Exists( DefaultCAPSValuesStateFile );
    if( !defaultCAPSValuesDone )
    {
        // Do this only once, after factory default
        if( ! SystemUtils::WriteFile( "", DefaultCAPSValuesStateFile ) )
        {
            BOSE_CRITICAL( s_logger, "File write to %s Failed", DefaultCAPSValuesStateFile.c_str( ) );
        }

        // Populate status and visibility of PRODUCT sources.
        using namespace ProductSTS;

        Sources message;
        Sources_SourceItem* source = message.add_sources( );
        source->set_sourcename( SHELBY_SOURCE::PRODUCT );
        source->set_sourceaccountname( ProductSourceSlot_Name( AUX ) );
        source->set_accountid( ProductSourceSlot_Name( AUX ) );
        source->set_status( SourceStatus::AVAILABLE );
        source->set_visible( true );

        // Set the (in)visibility of SETUP sources.
        source = message.add_sources( );
        source->set_sourcename( SHELBY_SOURCE::SETUP );
        source->set_sourceaccountname( SetupSourceSlot_Name( SETUP ) );
        source->set_accountid( SetupSourceSlot_Name( SETUP ) );
        source->set_status( SourceStatus::UNAVAILABLE );
        source->set_visible( false );

        GetFrontDoorClient()->SendPut<Sources, FrontDoor::Error>(
            FRONTDOOR_SYSTEM_SOURCES_API,
            message,
            {},
            m_errorCb );
        BOSE_INFO( s_logger, "DefaultCAPSValuesStateFile didn't exist, sent %s", message.DebugString( ).c_str( ) );
    }

    // Do the Common stuff last, the PUT above must come first
    ProductController::SendInitialCapsData();
}

void CustomProductController::SendActivateAccessPointCmd()
{
    BOSE_INFO( s_logger, __func__ );
}

void CustomProductController::SendDeActivateAccessPointCmd()
{
    BOSE_INFO( s_logger, __func__ );
}

NetManager::Protobuf::OperationalMode CustomProductController::GetWiFiOperationalMode( ) const
{
    return GetNetworkServiceUtil().GetNetManagerOperationMode();
}

void CustomProductController::HandleIntents( KeyHandlerUtil::ActionType_t intent )
{
    BOSE_INFO( s_logger, "Translated Intent %s", CommonIntentHandler::GetIntentName( intent ).c_str( ) );
    GetCommonCliClientMT().SendAsyncResponse( "Translated intent = " + \
                                              CommonIntentHandler::GetIntentName( intent ) );

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

void CustomProductController::InitializeKeyIdToKeyNameMap()
{
    BOSE_INFO( s_logger, "CustomProductController::%s:", __func__ );

    // This provides a mapping of the hardware keys to a keyName
    // The protobuf's for Keys are not explicit in terms of name and the Product need to do the mapping.
    m_keyIdToKeyNameMap[std::make_pair( KeyOrigin_t::KEY_ORIGIN_CONSOLE_BUTTON, KeyNamesPB::keyid::BLUETOOTH_KEYID )]  = KeyNamesPB::keynames::BLUETOOTH;
    m_keyIdToKeyNameMap[std::make_pair( KeyOrigin_t::KEY_ORIGIN_CONSOLE_BUTTON, KeyNamesPB::keyid::AUX_KEYID )]  = KeyNamesPB::keynames::AUX;
    m_keyIdToKeyNameMap[std::make_pair( KeyOrigin_t::KEY_ORIGIN_CONSOLE_BUTTON, KeyNamesPB::keyid::VOLUME_UP_KEYID )]  = KeyNamesPB::keynames::VOLUME_UP;
    m_keyIdToKeyNameMap[std::make_pair( KeyOrigin_t::KEY_ORIGIN_CONSOLE_BUTTON, KeyNamesPB::keyid::MFB_KEYID )]  = KeyNamesPB::keynames::MFB;
    m_keyIdToKeyNameMap[std::make_pair( KeyOrigin_t::KEY_ORIGIN_CONSOLE_BUTTON, KeyNamesPB::keyid::VOLUME_DOWN_KEYID )]  = KeyNamesPB::keynames::VOLUME_DOWN;
    m_keyIdToKeyNameMap[std::make_pair( KeyOrigin_t::KEY_ORIGIN_CONSOLE_BUTTON, KeyNamesPB::keyid::ACTION_KEYID )]  = KeyNamesPB::keynames::ACTION;

    m_keyIdToKeyNameMap[std::make_pair( KeyOrigin_t::KEY_ORIGIN_CONSOLE_BUTTON, KeyNamesPB::keyid::PRESET_1_KEYID )]  = KeyNamesPB::keynames::PRESET_1;
    m_keyIdToKeyNameMap[std::make_pair( KeyOrigin_t::KEY_ORIGIN_CONSOLE_BUTTON, KeyNamesPB::keyid::PRESET_2_KEYID )]  = KeyNamesPB::keynames::PRESET_2;
    m_keyIdToKeyNameMap[std::make_pair( KeyOrigin_t::KEY_ORIGIN_CONSOLE_BUTTON, KeyNamesPB::keyid::PRESET_3_KEYID )]  = KeyNamesPB::keynames::PRESET_3;
    m_keyIdToKeyNameMap[std::make_pair( KeyOrigin_t::KEY_ORIGIN_CONSOLE_BUTTON, KeyNamesPB::keyid::PRESET_4_KEYID )]  = KeyNamesPB::keynames::PRESET_4;
    m_keyIdToKeyNameMap[std::make_pair( KeyOrigin_t::KEY_ORIGIN_CONSOLE_BUTTON, KeyNamesPB::keyid::PRESET_5_KEYID )]  = KeyNamesPB::keynames::PRESET_5;
    m_keyIdToKeyNameMap[std::make_pair( KeyOrigin_t::KEY_ORIGIN_CONSOLE_BUTTON, KeyNamesPB::keyid::PRESET_6_KEYID )]  = KeyNamesPB::keynames::PRESET_6;
    return;
}


void CustomProductController::HandleNetworkStandbyIntentCb( const KeyHandlerUtil::ActionType_t& intent )
{
    BOSE_INFO( s_logger, "%s: Translated Intent %s", __func__, CommonIntentHandler::GetIntentName( intent ).c_str( ) );

    GetHsm().Handle<> ( &CustomProductControllerState::HandleNetworkStandbyIntent );
    return;
}

void CustomProductController::UpdateUiConnectedStatus( bool status )
{
    BOSE_INFO( s_logger, "%s|status:%s", __func__, status ? "true" : "false" );
    m_isUiConnected = status;
    GetHsm().Handle<bool>( &ProductControllerState::HandleUiConnectedUpdateState, status );
}

void CustomProductController::HandleProductMessage( const ProductMessage& productMessage )
{
    BOSE_DEBUG( s_logger, "%s", __func__ );

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// LPM status messages require both product-specific and common handling.
    ///////////////////////////////////////////////////////////////////////////////////////////////
    if( productMessage.has_lpmstatus( ) )
    {
        ///
        /// First register for product-specific LPM events if connected. Common handling of the product message is
        /// then done.
        ///
        if( productMessage.lpmstatus( ).has_connected( ) && productMessage.lpmstatus( ).connected( ) )
        {
            RegisterLpmEvents();
        }

        ( void ) HandleCommonProductMessage( productMessage );
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
/// @name   CustomProductController::SetupProductSTSController
///
/// @brief  This method is called to perform the needed initialization of the ProductSTSController,
///         specifically, provide the set of sources to be created initially.
///
/// @param  void This method does not take any arguments.
///
/// @return This method does not return anything.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductController::SetupProductSTSController( void )
{
    using namespace ProductSTS;

    ProductSTSStateFactory<CustomProductSTSStateTopAux> auxStateFactory;
    ProductSTSStateFactory<ProductSTSStateTopSetup>    silentStateFactory;

    // 'AUX' is a product defined source used for the auxilary port.
    std::vector<ProductSTSController::SourceDescriptor> sources;
    ProductSTSController::SourceDescriptor descriptor_AUX{ AUX, ProductSourceSlot_Name( AUX ), true, auxStateFactory };
    sources.push_back( descriptor_AUX );

    // 'SETUP' is a "fake" source used for setup state.
    ProductSTSController::SourceDescriptor descriptor_Setup{ SETUP, SetupSourceSlot_Name( SETUP ), false, silentStateFactory };
    sources.push_back( descriptor_Setup );

    Callback<void> cb_STSInitWasComplete( std::bind( &CustomProductController::HandleSTSInitWasComplete, this ) );
    Callback<ProductSTSAccount::ProductSourceSlot> cb_HandleSelectSourceSlot( std::bind( &CustomProductController::HandleSelectSourceSlot, this, std::placeholders::_1 ) );
    m_ProductSTSController.Initialize( sources, cb_STSInitWasComplete, cb_HandleSelectSourceSlot );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   CustomProductController::HandleSTSInitWasComplete
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
void CustomProductController::HandleSTSInitWasComplete( void )
{
    BOSE_INFO( s_logger, __func__ );
    IL::BreakThread( std::bind( &CustomProductController::HandleSTSReady,
                                this ),
                     GetTask( ) );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   CustomProductController::HandleSelectSourceSlot
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
void CustomProductController::HandleSelectSourceSlot( ProductSTSAccount::ProductSourceSlot sourceSlot )
{
    BOSE_INFO( s_logger, "%s: slot: %d", __func__, sourceSlot );
}

void CustomProductController::HandleBtLeCapabilityReady( const std::list<std::string>& points )
{
    BOSE_INFO( s_logger, __func__ );
    HandleBtLeModuleReady( true );
}

void CustomProductController::HandleBtLeCapabilityNotReady( const std::list<std::string>& points )
{
    BOSE_INFO( s_logger, __func__ );
    HandleBtLeModuleReady( false );
}

void CustomProductController::HandleBtLeModuleReady( bool btLeModuleReady )
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
bool CustomProductController::IsBooted( ) const
{
    return IsAllModuleReady();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @name   IsLowPowerExited
/// @return This method returns a true or false value, based on a series of set member variables,
///         which all must be true to indicate that the device has exited low power and all modules
///         have come back
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductController::IsLowPowerExited( ) const
{
    return IsAllModuleReady();
}
} /// namespace ProductApp
