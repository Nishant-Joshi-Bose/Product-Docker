////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file      CustomProductController.cpp
///
/// @brief     This file contains source code that implements the CustomProductController class
///            that acts as a container to handle all the main functionality related to this program
///            that is product specific. In these regards, this class is used as a container to
///            control the product states, as well as to instantiate modules to manage the device
///            and lower level hardware, and interface with the user and system level applications.
///
/// @author    Stuart J. Lumby
///
/// @attention Copyright (C) 2017 Bose Corporation All Rights Reserved
///
///            Bose Corporation
///            The Mountain Road,
///            Framingham, MA 01701-9168
///            U.S.A.
///
///            This program may not be reproduced, in whole or in part, in any form by any means
///            whatsoever without the written permission of Bose Corporation.
///
////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////
///
///            Included Header Files
///
////////////////////////////////////////////////////////////////////////////////////////////////////
#include "unistd.h"
#include "CustomProductController.h"
#include "CustomProductLpmHardwareInterface.h"
#include "CustomProductAudioService.h"
#include "CustomAudioSettingsManager.h"
#include "CustomProductKeyInputManager.h"
#include "ProductCommandLine.h"
#include "CommonProductCommandLine.h"
#include "ProductAdaptIQManager.h"
#include "IntentHandler.h"
#include "ProductSTS.pb.h"
#include "ProductSTSStateFactory.h"
#include "ProductSTSStateTop.h"
#include "ProductSTSStateTopSilent.h"
#include "CustomSTSController/ProductSTSStateTopAiQ.h"
#include "CustomSTSController/ProductSTSStateDeviceControl.h"
#include "SystemSourcesProperties.pb.h"
#include "ProductControllerHsm.h"
#include "CustomProductControllerStates.h"
#include "CustomProductControllerState.h"
#include "ProductControllerStates.h"
#include "ProductControllerState.h"
#include "ProductControllerStateCriticalError.h"
#include "ProductControllerStateFactoryDefault.h"
#include "ProductControllerStateFirstBootGreeting.h"
#include "ProductControllerStateFirstBootGreetingTransition.h"
#include "ProductControllerStateIdleVoiceConfigured.h"
#include "ProductControllerStateIdleVoiceNotConfigured.h"
#include "ProductControllerStateLowPowerStandby.h"
#include "ProductControllerStateNetworkStandbyConfigured.h"
#include "ProductControllerStateNetworkStandby.h"
#include "ProductControllerStateNetworkStandbyNotConfigured.h"
#include "ProductControllerStatePlayableTransitionIdle.h"
#include "ProductControllerStatePlayableTransitionInternal.h"
#include "ProductControllerStatePlayingDeselected.h"
#include "ProductControllerStatePlaying.h"
#include "ProductControllerStatePlayingSelectedNotSilent.h"
#include "ProductControllerStatePlayingSelectedNotSilentSourceInvalid.h"
#include "ProductControllerStatePlayingSelectedNotSilentSourceValid.h"
#include "ProductControllerStatePlayingSelectedSetupExiting.h"
#include "ProductControllerStatePlayingSelectedSetupNetworkConfigWiFiConnection.h"
#include "ProductControllerStatePlayingSelectedSetupNetworkConfigWiFiTransition.h"
#include "ProductControllerStatePlayingSelectedSetupNetworkConfigWiFiExiting.h"
#include "ProductControllerStatePlayingSelectedSetupNetworkConfigWiFiAborting.h"
#include "ProductControllerStatePlayingSelectedSetupOther.h"
#include "ProductControllerStatePlayingSelectedSilent.h"
#include "ProductControllerStatePlayingSelectedSilentSourceInvalid.h"
#include "ProductControllerStatePlayingSelectedSilentSourceValid.h"
#include "ProductControllerStatePlayingSelectedStoppingStreams.h"
#include "ProductControllerStatePlayingTransitionSwitch.h"
#include "ProductControllerStateSoftwareInstall.h"
#include "ProductControllerStateSoftwareInstallTransition.h"
#include "ProductControllerStateStoppingStreamsDedicatedForFactoryDefault.h"
#include "ProductControllerStateStoppingStreamsDedicatedForSoftwareUpdate.h"
#include "ProductControllerStateStoppingStreamsDedicated.h"
#include "CustomProductControllerStateAccessoryPairing.h"
#include "CustomProductControllerStateAccessoryPairingCancelling.h"
#include "CustomProductControllerStateAdaptIQCancelling.h"
#include "CustomProductControllerStateAdaptIQ.h"
#include "CustomProductControllerStateBooted.h"
#include "CustomProductControllerStateBooting.h"
#include "CustomProductControllerStateFirstBootGreetingTransition.h"
#include "CustomProductControllerStateIdle.h"
#include "CustomProductControllerStateLowPowerResume.h"
#include "CustomProductControllerStateLowPowerStandbyTransition.h"
#include "CustomProductControllerStateNetworkStandby.h"
#include "CustomProductControllerStateOn.h"
#include "CustomProductControllerStatePlayable.h"
#include "CustomProductControllerStatePlayableTransition.h"
#include "CustomProductControllerStatePlayableTransitionNetworkStandby.h"
#include "CustomProductControllerStatePlaying.h"
#include "CustomProductControllerStatePlayingSelected.h"
#include "CustomProductControllerStatePlayingSelectedSetup.h"
#include "CustomProductControllerStatePlayingSelectedSetupNetworkConfig.h"
#include "CustomProductControllerStatePlayingTransition.h"
#include "CustomProductControllerStateTop.h"
#include "MfgData.h"
#include "DeviceManager.pb.h"
#include "ProductBLERemoteManager.h"
#include "ProductEndpointDefines.h"
#include "ProtoPersistenceFactory.h"
#include "PGCErrorCodes.h"
#include "SystemUtils.h"
#include "CustomChimeEvents.h"
#include "LpmClientLiteIF.h"
#include "AudioService.pb.h"
#include "AudioPathControl.pb.h"
#include "ProductDataCollectionDefines.h"
#include "SystemSourcesFriendlyNames.pb.h"

///
/// Class Name Declaration for Logging
///
namespace
{
constexpr char CLASS_NAME[ ] = "CustomProductController";
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                          Start of the Product Application Namespace                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
namespace ProductApp
{

namespace
{
////////////////////////////////////////////////////////////////////////////////////////////////////
///
///            Constant Definitions
///
////////////////////////////////////////////////////////////////////////////////////////////////////
constexpr uint32_t  PRODUCT_CONTROLLER_RUNNING_CHECK_IN_SECONDS = 4;
constexpr int32_t   VOLUME_MIN_THRESHOLD = 10;
constexpr int32_t   VOLUME_MAX_THRESHOLD = 70;
constexpr auto      g_DefaultCAPSValuesStateFile        = "DefaultCAPSValuesDone";
constexpr auto      g_DefaultRebroadcastLatencyModeFile = "DefaultRebroadcastLatencyModeDone";

constexpr const char BLAST_CONFIGURATION_FILE_NAME[ ] = BOSE_CONF_DIR "BlastConfiguration.json";
constexpr char       UI_KILL_PID_FILE[] = "/var/run/local-ui.pid";
constexpr uint32_t   UI_ALIVE_TIMEOUT = 60 * 1000;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   CustomProductController::CustomProductController
///
/// @brief  This method is the CustomProductController constructor, which is used to initialize
///         its corresponding module classes and member variables.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
CustomProductController::CustomProductController( ) :

    ///
    /// Construction of the Product Controller Modules
    ///
    m_ProductLpmHardwareInterface( nullptr ),
    m_ProductCommandLine( nullptr ),
    m_CommonProductCommandLine( ),
    m_ProductKeyInputManager( nullptr ),
    m_ProductFrontDoorKeyInjectIF( nullptr ),
    m_ProductCecHelper( nullptr ),
    m_ProductDspHelper( nullptr ),
    m_ProductAdaptIQManager( nullptr ),
    m_ProductAudioService( nullptr ),
    m_ProductBLERemoteManager( nullptr ),

    ///
    /// Member Variable Initialization
    ///
    m_IsAutoWakeEnabled( false ),
    m_Running( false ),
    m_networkOperationalMode( NetManager::Protobuf::wifiOff ),
    m_isNetworkWired( false ),
    m_ethernetEnabled( true ),
    ///
    /// Initialization of STS controller.
    ///
    m_ProductSTSController( *this ),

    ///
    /// Intent Handler Initialization
    ///
    m_IntentHandler( *GetTask(),
                     GetCommonCliClientMT(),
                     m_FrontDoorClientIF,
                     *this ),

    ///
    /// Intitialization for the Product Message Handler Reference
    ///
    m_ProductMessageHandler( static_cast< Callback < ProductMessage > >
                             ( std::bind( &CustomProductController::HandleMessage,
                                          this,
                                          std::placeholders::_1 ) ) ),
    m_AccessorySoftwareInstallManager( GetTask( ),
                                       GetProductSoftwareInstallManager( ),
                                       GetProductSoftwareInstallScheduler( ) )
{
    m_radioStatus.set_status( IPC_SOC_NETWORKSTATUS_OFF );
    m_radioStatus.set_band( IPC_SOC_RADIO_BAND_INVALID );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name CustomProductController::Start
///
/// @brief This method starts the product controller by dispatching its Run method inside the
///        product task. The Run method initializes the product controller state machine and all
///        of its associated modules, including the registration of callbacks for internal and state
///        machine messaging, IPC, Frontdoor end-points, and so forth. Since these initializations
///        take place first inside the product task, and all callbacks are processed inside the same
///        product task after the initialization, no callback can be invoked from a non-existent
///        state or module. This method was put in place based on the JIRA Story PGC-2052.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductController::Start( )
{
    m_Running = true;

    IL::BreakThread( std::bind( &CustomProductController::Run, this ), GetTask( ) );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name CustomProductController::Run
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductController::Run( )
{
    BOSE_DEBUG( s_logger, "----------- Product Controller Initialization Start ------------" );
    BOSE_DEBUG( s_logger, "The Custom Product Controller is setting up the state machine." );

    ///
    /// Construction of the Common and Custom States, and insertion of the states into the HSM
    ///
    using namespace SystemPowerPb;
    ///
    /// Top State
    ///
    auto* stateTop =
        GetHsm( ).AddState <CustomProductControllerStateTop> ( nullptr, CUSTOM_PRODUCT_CONTROLLER_STATE_TOP, SYSTEM_STATE_NOTIFIED_NOT_NOTIFY,
                                                               SystemPowerControl_State_Not_Notify );
    ///
    /// Booting States and Various System Level States
    ///
    ( void )
    GetHsm( ).AddState <CustomProductControllerStateBooting> ( stateTop, CUSTOM_PRODUCT_CONTROLLER_STATE_BOOTING, SYSTEM_STATE_NOTIFIED_NAME_BOOTING,
                                                               SystemPowerControl_State_Not_Notify );

    ( void )
    GetHsm( ).AddState <CustomProductControllerStateBooted> ( stateTop, CUSTOM_PRODUCT_CONTROLLER_STATE_BOOTED, SYSTEM_STATE_NOTIFIED_NOT_NOTIFY,
                                                              SystemPowerControl_State_Not_Notify );

    ( void )
    GetHsm( ).AddState <ProductControllerStateFirstBootGreeting> ( stateTop, PRODUCT_CONTROLLER_STATE_FIRST_BOOT_GREETING, SYSTEM_STATE_NOTIFIED_NAME_FIRST_BOOT_GREETING,
                                                                   SystemPowerControl_State_Not_Notify );

    ( void )
    GetHsm( ).AddState <CustomProductControllerStateFirstBootGreetingTransition> ( stateTop, CUSTOM_PRODUCT_CONTROLLER_STATE_FIRST_BOOT_GREETING_TRANSITION, SYSTEM_STATE_NOTIFIED_NOT_NOTIFY,
                                                                                   SystemPowerControl_State_Not_Notify );

    ( void )
    GetHsm( ).AddState <ProductControllerStateSoftwareInstallTransition> ( stateTop, PRODUCT_CONTROLLER_STATE_SOFTWARE_INSTALL_TRANSITION, SYSTEM_STATE_NOTIFIED_NAME_UPDATING,
                                                                           SystemPowerControl_State_Not_Notify );

    ( void )
    GetHsm( ).AddState <ProductControllerStateSoftwareInstall> ( stateTop, PRODUCT_CONTROLLER_STATE_SOFTWARE_INSTALL, SYSTEM_STATE_NOTIFIED_NAME_UPDATING,
                                                                 SystemPowerControl_State_Not_Notify );

    ( void )
    GetHsm( ).AddState <ProductControllerStateSoftwareInstall> ( stateTop, PRODUCT_CONTROLLER_STATE_SOFTWARE_INSTALL_MANUAL, SYSTEM_STATE_NOTIFIED_NAME_UPDATING_MANUAL,
                                                                 SystemPowerControl_State_Not_Notify );

    ( void )
    GetHsm( ).AddState <ProductControllerStateCriticalError> ( stateTop, PRODUCT_CONTROLLER_STATE_CRITICAL_ERROR, SYSTEM_STATE_NOTIFIED_NAME_CRITICAL_ERROR,
                                                               SystemPowerControl_State_Not_Notify );

    ( void )
    GetHsm( ).AddState <ProductControllerStateFactoryDefault> ( stateTop, PRODUCT_CONTROLLER_STATE_FACTORY_DEFAULT, SYSTEM_STATE_NOTIFIED_NAME_FACTORY_DEFAULT,
                                                                SystemPowerControl_State_Not_Notify );

    ( void )
    GetHsm( ).AddState <CustomProductControllerStateLowPowerStandbyTransition> ( stateTop, CUSTOM_PRODUCT_CONTROLLER_STATE_LOW_POWER_STANDBY_TRANSITION, SYSTEM_STATE_NOTIFIED_NOT_NOTIFY,
                                                                                 SystemPowerControl_State_Not_Notify );

    ( void )
    GetHsm( ).AddState <ProductControllerStateLowPowerStandby> ( stateTop, PRODUCT_CONTROLLER_STATE_LOW_POWER_STANDBY, SYSTEM_STATE_NOTIFIED_NOT_NOTIFY,
                                                                 SystemPowerControl_State_OFF );

    ( void )
    GetHsm( ).AddState <CustomProductControllerStateLowPowerResume> ( stateTop, CUSTOM_PRODUCT_CONTROLLER_STATE_LOW_POWER_RESUME, SYSTEM_STATE_NOTIFIED_NOT_NOTIFY,
                                                                      SystemPowerControl_State_Not_Notify );

    ///
    /// Playable Transition State and Sub-States
    ///
    auto* statePlayableTransition =
        GetHsm( ).AddState <CustomProductControllerStatePlayableTransition> ( stateTop, CUSTOM_PRODUCT_CONTROLLER_STATE_PLAYABLE_TRANSITION, SYSTEM_STATE_NOTIFIED_NOT_NOTIFY,
                                                                              SystemPowerControl_State_Not_Notify );

    auto* statePlayableTransitionInternal =
        GetHsm( ).AddState <ProductControllerStatePlayableTransitionInternal> ( statePlayableTransition, PRODUCT_CONTROLLER_STATE_PLAYABLE_TRANSITION_INTERNAL, SYSTEM_STATE_NOTIFIED_NOT_NOTIFY,
                                                                                SystemPowerControl_State_Not_Notify );

    ( void )
    GetHsm( ).AddState <ProductControllerStatePlayableTransitionIdle> ( statePlayableTransitionInternal, PRODUCT_CONTROLLER_STATE_PLAYABLE_TRANSITION_IDLE, SYSTEM_STATE_NOTIFIED_NOT_NOTIFY,
                                                                        SystemPowerControl_State_OFF );

    ( void )
    GetHsm( ).AddState <CustomProductControllerStatePlayableTransitionNetworkStandby> ( statePlayableTransitionInternal, CUSTOM_PRODUCT_CONTROLLER_STATE_PLAYABLE_TRANSITION_NETWORK_STANDBY, SYSTEM_STATE_NOTIFIED_NOT_NOTIFY,
            SystemPowerControl_State_OFF );

    ///
    /// Top On State
    ///
    auto* stateOn =
        GetHsm( ).AddState <CustomProductControllerStateOn> ( stateTop, CUSTOM_PRODUCT_CONTROLLER_STATE_ON, SYSTEM_STATE_NOTIFIED_NOT_NOTIFY,
                                                              SystemPowerControl_State_Not_Notify );

    ///
    /// Playable State and Sub-States
    ///
    auto* statePlayable =
        GetHsm( ).AddState <CustomProductControllerStatePlayable> ( stateOn, CUSTOM_PRODUCT_CONTROLLER_STATE_PLAYABLE, SYSTEM_STATE_NOTIFIED_NOT_NOTIFY,
                                                                    SystemPowerControl_State_OFF );

    auto* stateNetworkStandby =
        GetHsm( ).AddState <CustomProductControllerStateNetworkStandby> ( statePlayable, CUSTOM_PRODUCT_CONTROLLER_STATE_NETWORK_STANDBY, SYSTEM_STATE_NOTIFIED_NAME_NETWORK_STANDBY,
                                                                          SystemPowerControl_State_OFF );

    ( void )
    GetHsm( ).AddState <ProductControllerStateNetworkStandbyConfigured> ( stateNetworkStandby, PRODUCT_CONTROLLER_STATE_NETWORK_STANDBY_CONFIGURED, SYSTEM_STATE_NOTIFIED_NAME_NETWORK_STANDBY,
                                                                          SystemPowerControl_State_OFF );

    ( void )
    GetHsm( ).AddState <ProductControllerStateNetworkStandbyNotConfigured> ( stateNetworkStandby, PRODUCT_CONTROLLER_STATE_NETWORK_STANDBY_NOT_CONFIGURED, SYSTEM_STATE_NOTIFIED_NAME_NETWORK_STANDBY,
                                                                             SystemPowerControl_State_OFF );

    auto* stateIdle =
        GetHsm( ).AddState <CustomProductControllerStateIdle> ( statePlayable, CUSTOM_PRODUCT_CONTROLLER_STATE_IDLE, SYSTEM_STATE_NOTIFIED_NAME_IDLE,
                                                                SystemPowerControl_State_OFF );
    ( void )
    GetHsm( ).AddState <ProductControllerStateIdleVoiceConfigured> ( stateIdle, PRODUCT_CONTROLLER_STATE_IDLE_VOICE_CONFIGURED, SYSTEM_STATE_NOTIFIED_NAME_IDLE,
                                                                     SystemPowerControl_State_OFF );

    ( void )
    GetHsm( ).AddState <ProductControllerStateIdleVoiceNotConfigured> ( stateIdle, PRODUCT_CONTROLLER_STATE_IDLE_VOICE_NOT_CONFIGURED, SYSTEM_STATE_NOTIFIED_NAME_IDLE,
                                                                        SystemPowerControl_State_OFF );

    ///
    /// Playing Transition State and Sub-States
    ///
    auto* statePlayingTransition =
        GetHsm( ).AddState <CustomProductControllerStatePlayingTransition> ( stateTop, CUSTOM_PRODUCT_CONTROLLER_STATE_PLAYING_TRANSITION, SYSTEM_STATE_NOTIFIED_NOT_NOTIFY,
                                                                             SystemPowerControl_State_Not_Notify );

    ( void )
    GetHsm( ).AddState <ProductControllerStatePlayingTransitionSwitch> ( statePlayingTransition, PRODUCT_CONTROLLER_STATE_PLAYING_TRANSITION_SWITCH, SYSTEM_STATE_NOTIFIED_NOT_NOTIFY,
                                                                         SystemPowerControl_State_Not_Notify );

    ///
    /// Playing State and Sub-States
    ///
    auto* statePlaying =
        GetHsm( ).AddState <CustomProductControllerStatePlaying> ( stateOn, CUSTOM_PRODUCT_CONTROLLER_STATE_PLAYING, SYSTEM_STATE_NOTIFIED_NOT_NOTIFY,
                                                                   SystemPowerControl_State_Not_Notify );

    ( void )
    GetHsm( ).AddState <ProductControllerStatePlayingDeselected> ( statePlaying, PRODUCT_CONTROLLER_STATE_PLAYING_DESELECTED, SYSTEM_STATE_NOTIFIED_NAME_PLAYING_SOURCE_OFF,
                                                                   SystemPowerControl_State_OFF );

    auto* statePlayingSelected =
        GetHsm( ).AddState <CustomProductControllerStatePlayingSelected> ( statePlaying, CUSTOM_PRODUCT_CONTROLLER_STATE_PLAYING_SELECTED, SYSTEM_STATE_NOTIFIED_NAME_SELECTED,
                                                                           SystemPowerControl_State_Not_Notify );

    auto* statePlayingSelectedSilent =
        GetHsm( ).AddState <ProductControllerStatePlayingSelectedSilent> ( statePlayingSelected, PRODUCT_CONTROLLER_STATE_PLAYING_SELECTED_SILENT, SYSTEM_STATE_NOTIFIED_NAME_SELECTED,
                                                                           SystemPowerControl_State_Not_Notify );

    ( void )
    GetHsm( ).AddState <ProductControllerStatePlayingSelectedSilentSourceInvalid> ( statePlayingSelectedSilent, PRODUCT_CONTROLLER_STATE_PLAYING_SELECTED_SILENT_SOURCE_INVALID, SYSTEM_STATE_NOTIFIED_NAME_PLAYING_SOURCE_OFF,
                                                                                    SystemPowerControl_State_OFF );

    ( void )
    GetHsm( ).AddState <ProductControllerStatePlayingSelectedSilentSourceValid> ( statePlayingSelectedSilent, PRODUCT_CONTROLLER_STATE_PLAYING_SELECTED_SILENT_SOURCE_VALID, SYSTEM_STATE_NOTIFIED_NAME_SELECTED,
                                                                                  SystemPowerControl_State_ON );

    auto* statePlayingSelectedNotSilent =
        GetHsm( ).AddState <ProductControllerStatePlayingSelectedNotSilent> ( statePlayingSelected, PRODUCT_CONTROLLER_STATE_PLAYING_SELECTED_NOT_SILENT, SYSTEM_STATE_NOTIFIED_NAME_SELECTED,
                                                                              SystemPowerControl_State_Not_Notify );

    ( void )
    GetHsm( ).AddState <ProductControllerStatePlayingSelectedNotSilentSourceInvalid> ( statePlayingSelectedNotSilent, PRODUCT_CONTROLLER_STATE_PLAYING_SELECTED_NOT_SILENT_SOURCE_INVALID, SYSTEM_STATE_NOTIFIED_NAME_SELECTED,
            SystemPowerControl_State_Not_Notify );

    ( void )
    GetHsm( ).AddState <ProductControllerStatePlayingSelectedNotSilentSourceValid> ( statePlayingSelectedNotSilent, PRODUCT_CONTROLLER_STATE_PLAYING_SELECTED_NOT_SILENT_SOURCE_VALID, SYSTEM_STATE_NOTIFIED_NAME_SELECTED,
            SystemPowerControl_State_ON );

    auto* statePlayingSelectedSetup =
        GetHsm( ).AddState <CustomProductControllerStatePlayingSelectedSetup> ( statePlayingSelected, CUSTOM_PRODUCT_CONTROLLER_STATE_PLAYING_SELECTED_SETUP, SYSTEM_STATE_NOTIFIED_NAME_SELECTED,
                                                                                SystemPowerControl_State_ON );

    auto* statePlayingSelectedSetupNetworkConfig =
        GetHsm( ).AddState <CustomProductControllerStatePlayingSelectedSetupNetworkConfig> ( statePlayingSelectedSetup, CUSTOM_PRODUCT_CONTROLLER_STATE_PLAYING_SELECTED_SETUP_NETWORK_CONFIG, SYSTEM_STATE_NOTIFIED_NAME_NETWORK_CONFIG,
                SystemPowerControl_State_ON );

    ( void )
    GetHsm( ).AddState <ProductControllerStatePlayingSelectedSetupNetworkConfigWiFiTransition> ( statePlayingSelectedSetupNetworkConfig, PRODUCT_CONTROLLER_STATE_PLAYING_SELECTED_SETUP_NETWORK_CONFIG_WIFI_TRANSITION, SYSTEM_STATE_NOTIFIED_NAME_NETWORK_CONFIG,
            SystemPowerControl_State_Not_Notify );

    ( void )
    GetHsm( ).AddState <ProductControllerStatePlayingSelectedSetupOther> ( statePlayingSelectedSetup, PRODUCT_CONTROLLER_STATE_PLAYING_SELECTED_SETUP_OTHER, SYSTEM_STATE_NOTIFIED_NAME_SELECTED,
                                                                           SystemPowerControl_State_ON );

    ( void )
    GetHsm( ).AddState <ProductControllerStatePlayingSelectedSetupExiting> ( statePlayingSelectedSetup, PRODUCT_CONTROLLER_STATE_PLAYING_SELECTED_SETUP_EXITING, SYSTEM_STATE_NOTIFIED_NAME_SELECTED,
                                                                             SystemPowerControl_State_Not_Notify );

    ( void )
    GetHsm( ).AddState <ProductControllerStatePlayingSelectedSetupNetworkConfigWiFiConnection> ( statePlayingSelectedSetupNetworkConfig, PRODUCT_CONTROLLER_STATE_PLAYING_SELECTED_SETUP_NETWORK_CONFIG_WIFI_CONNECTION, SYSTEM_STATE_NOTIFIED_NAME_NETWORK_CONFIG,
            SystemPowerControl_State_ON );

    ( void )
    GetHsm( ).AddState <ProductControllerStatePlayingSelectedSetupNetworkConfigWiFiExiting> ( statePlayingSelectedSetupNetworkConfig, PRODUCT_CONTROLLER_STATE_PLAYING_SELECTED_SETUP_NETWORK_CONFIG_WIFI_EXITING, SYSTEM_STATE_NOTIFIED_NAME_NETWORK_CONFIG,
            SystemPowerControl_State_Not_Notify );

    ( void )
    GetHsm( ).AddState <ProductControllerStatePlayingSelectedSetupNetworkConfigWiFiAborting> ( statePlayingSelectedSetupNetworkConfig, PRODUCT_CONTROLLER_STATE_PLAYING_SELECTED_SETUP_NETWORK_CONFIG_WIFI_ABORTING, SYSTEM_STATE_NOTIFIED_NAME_NETWORK_CONFIG,
            SystemPowerControl_State_Not_Notify );

    ( void )
    GetHsm( ).AddState <ProductControllerStatePlayingSelectedStoppingStreams> ( statePlayingSelected, PRODUCT_CONTROLLER_STATE_PLAYING_SELECTED_STOPPING_STREAMS, SYSTEM_STATE_NOTIFIED_NOT_NOTIFY,
                                                                                SystemPowerControl_State_OFF );

    ///
    /// Accessory Pairing States
    ///
    ( void )
    GetHsm( ).AddState <CustomProductControllerStateAccessoryPairing> ( statePlayingSelected, CUSTOM_PRODUCT_CONTROLLER_STATE_ACCESSORY_PAIRING, SYSTEM_STATE_NOTIFIED_NAME_SELECTED,
                                                                        SystemPowerControl_State_ON );

    ( void )
    GetHsm( ).AddState <CustomProductControllerStateAccessoryPairingCancelling> ( statePlayingSelected, CUSTOM_PRODUCT_CONTROLLER_STATE_ACCESSORY_PAIRING_CANCELLING, SYSTEM_STATE_NOTIFIED_NOT_NOTIFY,
                                                                                  SystemPowerControl_State_Not_Notify );

    ///
    /// AdaptIQ States
    ///
    ( void )
    GetHsm( ).AddState <CustomProductControllerStateAdaptIQ> ( statePlayingSelected, CUSTOM_PRODUCT_CONTROLLER_STATE_ADAPTIQ, SYSTEM_STATE_NOTIFIED_NAME_SELECTED,
                                                               SystemPowerControl_State_ON );

    ( void )
    GetHsm( ).AddState <CustomProductControllerStateAdaptIQCancelling> ( statePlayingSelected, CUSTOM_PRODUCT_CONTROLLER_STATE_ADAPTIQ_CANCELLING, SYSTEM_STATE_NOTIFIED_NOT_NOTIFY,
                                                                         SystemPowerControl_State_Not_Notify );

    ///
    /// Stopping Streams Dedicated State and Sub-States
    ///
    auto* stateStoppingStreamsDedicated =
        GetHsm( ).AddState <ProductControllerStateStoppingStreamsDedicated> ( stateTop, PRODUCT_CONTROLLER_STATE_STOPPING_STREAMS_DEDICATED, SYSTEM_STATE_NOTIFIED_NOT_NOTIFY,
                                                                              SystemPowerControl_State_Not_Notify );

    ( void )
    GetHsm( ).AddState <ProductControllerStateStoppingStreamsDedicatedForFactoryDefault> ( stateStoppingStreamsDedicated, PRODUCT_CONTROLLER_STATE_STOPPING_STREAMS_DEDICATED_FOR_FACTORY_DEFAULT, SYSTEM_STATE_NOTIFIED_NOT_NOTIFY,
            SystemPowerControl_State_Not_Notify );

    ( void )
    GetHsm( ).AddState <ProductControllerStateStoppingStreamsDedicatedForSoftwareUpdate> ( stateStoppingStreamsDedicated, PRODUCT_CONTROLLER_STATE_STOPPING_STREAMS_DEDICATED_FOR_SOFTWARE_UPDATE, SYSTEM_STATE_NOTIFIED_NOT_NOTIFY,
            SystemPowerControl_State_Not_Notify );

    ///
    /// The state machine is initialized.
    ///
    GetHsm( ).Init( this, CUSTOM_PRODUCT_CONTROLLER_STATE_BOOTING );

    ///
    /// Initialize entities in the Common Product Controller
    ///
    CommonInitialize( );

    ///
    /// Set up connection with DeviceController service
    ///
    m_deviceControllerPtr = DeviceControllerClientFactory::Create( "CustomProductController", GetTask( ) );
    bool loadResult = m_deviceControllerPtr->LoadFilter( BLAST_CONFIGURATION_FILE_NAME );
    if( not loadResult )
    {
        BOSE_DIE( "Failed loading key blaster configuration file." );
    }

    auto connectCb = [this]( bool connected )
    {
        BOSE_INFO( s_logger, "Connected to DeviceController!" );
    };

    auto dvcDisconnectCb  = [this, connectCb]()
    {
        BOSE_INFO( s_logger, "Disconnected from DeviceController!" );
        m_deviceControllerPtr->Connect( connectCb );
    };

    m_deviceControllerPtr->RegisterDisconnectCb( dvcDisconnectCb );
    m_deviceControllerPtr->Connect( connectCb );

    ///
    /// Get instances of all the modules.
    ///
    BOSE_DEBUG( s_logger, "----------- Product Controller Starting Modules ------------" );
    BOSE_DEBUG( s_logger, "The Custom Product Controller instantiating and running its modules." );

    m_ProductLpmHardwareInterface = std::make_shared< CustomProductLpmHardwareInterface >( *this );
    m_ProductCecHelper            = std::make_shared< ProductCecHelper                  >( *this );
    m_ProductDspHelper            = std::make_shared< ProductDspHelper                  >( *this );
    m_ProductCommandLine          = std::make_shared< ProductCommandLine                >( *this );
    m_CommonProductCommandLine    = std::make_shared< CommonProductCommandLine          >( );
    m_ProductKeyInputManager      = std::make_shared< CustomProductKeyInputManager      >( *this );
    m_ProductFrontDoorKeyInjectIF = std::make_shared< ProductFrontDoorKeyInjectIF >( GetTask(),
                                    m_ProductKeyInputManager,
                                    m_FrontDoorClientIF );
    m_ProductBLERemoteManager     = std::make_shared< ProductBLERemoteManager           >( *this );
    m_ProductAudioService         = std::make_shared< CustomProductAudioService         >( *this,
                                    m_FrontDoorClientIF,
                                    m_ProductLpmHardwareInterface->GetLpmClient( ) );
    ///
    /// ProductAdaptIQManager depends on CustomProductAudioService, so make sure that CustomProductAudioService is
    /// instantiated first
    ///
    m_ProductAdaptIQManager       = std::make_shared< ProductAdaptIQManager             >( *this );

    if( m_ProductLpmHardwareInterface == nullptr ||
        m_ProductAudioService         == nullptr ||
        m_ProductCommandLine          == nullptr ||
        m_CommonProductCommandLine    == nullptr ||
        m_ProductKeyInputManager      == nullptr ||
        m_ProductFrontDoorKeyInjectIF == nullptr ||
        m_ProductCecHelper            == nullptr ||
        m_ProductDspHelper            == nullptr ||
        m_ProductAdaptIQManager       == nullptr )
    {
        BOSE_CRITICAL( s_logger, "-------- Product Controller Failed Initialization ----------" );
        BOSE_CRITICAL( s_logger, "A Product Controller module failed to be allocated.         " );

        return;
    }

    ///
    /// Apply settings from persistence
    ///
    ApplyOpticalAutoWakeSettingFromPersistence( );
    LoadPowerMacroFromPersistance( );

    /// Register a callback so the autowake from persistence is sent to LPM when connected
    RegisterOpticalAutowakeForLpmConnection( );


    ///
    /// Set up LightBarController
    ///
    LpmClientLiteIF::LpmClientLitePtr lpmLitePtr( std::static_pointer_cast<LpmClientLiteIF>( m_ProductLpmHardwareInterface->GetLpmClient( ) ) );
    m_lightbarController = std::unique_ptr< LightBar::LightBarController >(
                               new LightBar::LightBarController( GetTask( ),
                                                                 m_FrontDoorClientIF,
                                                                 lpmLitePtr ) );

    //
    // Setup UI recovery timer
    //
    m_uiAliveTimer = APTimer::Create( GetTask( ), "UIAliveTimer" );
    StartUiTimer();

    ///
    /// Run all the submodules.
    ///
    m_ProductLpmHardwareInterface->Run( );
    m_ProductAudioService        ->Run( );
    m_ProductCommandLine         ->Run( );
    AsyncCallback<> cancelAlarmCb( std::bind( &ProductController::CancelAlarm, this ), GetTask( ) );
    m_ProductKeyInputManager     ->Run( cancelAlarmCb );
    m_ProductFrontDoorKeyInjectIF->Run( );
    m_ProductCecHelper           ->Run( );
    m_ProductDspHelper           ->Run( );
    m_ProductAdaptIQManager      ->Run( );
    m_ProductBLERemoteManager    ->Run( );


    ///
    /// Register as listener for system sources update
    ///
    auto sourceInfoCb = [ this ]( const SoundTouchInterface::Sources & sources )
    {
        UpdatePowerMacro( );
        ReconcileCurrentProductSource( );
    };
    GetSourceInfo().RegisterSourceListener( sourceInfoCb );

    ///
    /// Register FrontDoor EndPoints
    ///
    RegisterFrontDoorEndPoints( );

    ///
    /// Send initial endpoint requests to the front door.
    ///
    SendCommonInitialRequests( );

    ///
    /// Initialize and register intents for key actions for the Product Controller.
    ///
    m_IntentHandler.Initialize( );

    ///
    /// Initialize the AccessorySoftwareInstallManager.
    ///
    InitializeAccessorySoftwareInstallManager( );

    auto func = [this]( bool enabled )
    {
        if( enabled )
        {
            // Connection to DataCollection server established, send current state info.
            SendPowerMacroToDataCollection( );
        }
    };
    m_dataCollectionClient->RegisterForEnabledNotifications( Callback<bool>( func ) );

    BOSE_DEBUG( s_logger, "------------ Product Controller Initialization End -------------" );
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
    return m_ProductMessageHandler;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   CustomProductController::GetLpmHardwareInterface
///
/// @return This method returns a shared pointer to the LPM hardware interface.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
std::shared_ptr< CustomProductLpmHardwareInterface >& CustomProductController::GetLpmHardwareInterface( )
{
    return m_ProductLpmHardwareInterface;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   CustomProductController::GetProductAudioServiceInstance
///
/// @return This method returns a shared pointer to the Product AudioService which interfaces with AudioPath.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
std::shared_ptr< CustomProductAudioService >& CustomProductController::GetProductAudioServiceInstance( )
{
    return m_ProductAudioService;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   CustomProductController::GetAdaptIQManager
///
/// @return This method returns a shared pointer to the AdaptIQManager instance
///
////////////////////////////////////////////////////////////////////////////////////////////////////
std::shared_ptr< ProductAdaptIQManager >& CustomProductController::GetAdaptIQManager( )
{
    return m_ProductAdaptIQManager;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   CustomProductController::GetBLERemoteManager
///
/// @return This method returns a shared pointer to the BLERemoteManager instance
///
////////////////////////////////////////////////////////////////////////////////////////////////////
std::shared_ptr< ProductBLERemoteManager>& CustomProductController::GetBLERemoteManager( )
{
    return m_ProductBLERemoteManager;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   CustomProductController::GetCecHelper
///
/// @return This method returns a shared pointer to the ProductCecHelper instance.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
std::shared_ptr< ProductCecHelper >& CustomProductController::GetCecHelper( )
{
    return m_ProductCecHelper;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   CustomProductController::GetDspHelper
///
/// @return This method returns a shared pointer to the ProductCecHelper instance.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
std::shared_ptr< ProductDspHelper >& CustomProductController::GetDspHelper( )
{
    return m_ProductDspHelper;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   CustomProductController::IsBooted
///
/// @return This method returns a true or false value, based on a series of set member variables,
///         which all must be true to indicate that the device has booted.
///         This is accomplished by delegating to IsAllModuleReady()
///
/// @note   The CLI command "product boot_status" returns the status of all factors used here. If ever
///         a factor is added, the CLI command needs changing as well. See ProductCommandLine::HandleCommand().
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductController::IsBooted( ) const
{
    BOSE_VERBOSE( s_logger, "------------ Product Controller Booted Check ---------------" );
    BOSE_VERBOSE( s_logger, " " );
    return IsAllModuleReady();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   CustomProductController::IsAllModuleReady
///
/// @return This method returns a true or false value, based on a series of set member variables,
///         which all must be true to indicate that the device has booted.
///
/// @note   The CLI command "product boot_status" returns the status of all factors used here. If ever
///         a factor is added, the CLI command needs changing as well. See ProductCommandLine::HandleCommand().
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductController::IsAllModuleReady( ) const
{
    BOSE_VERBOSE( s_logger, "LPM Connected         :  %s", ( IsLpmReady( )             ? "true" : "false" ) );
    BOSE_VERBOSE( s_logger, "CAPS Initialized      :  %s", ( IsCAPSReady( )            ? "true" : "false" ) );
    BOSE_VERBOSE( s_logger, "Audio Path Connected  :  %s", ( IsAudioPathReady( )       ? "true" : "false" ) );
    BOSE_VERBOSE( s_logger, "STS Initialized       :  %s", ( IsSTSReady( )             ? "true" : "false" ) );
    BOSE_VERBOSE( s_logger, "Software Update Ready :  %s", ( IsSoftwareUpdateReady( )  ? "true" : "false" ) );
    BOSE_VERBOSE( s_logger, "SASS Initialized      :  %s", ( IsSassReady( )            ? "true" : "false" ) );
    BOSE_VERBOSE( s_logger, "Bluetooth Initialized :  %s", ( IsBluetoothModuleReady( ) ? "true" : "false" ) );
    BOSE_VERBOSE( s_logger, "Network Ready         :  %s", ( IsNetworkModuleReady( )   ? "true" : "false" ) );
    BOSE_VERBOSE( s_logger, "Voice Ready           :  %s", ( IsVoiceModuleReady( )     ? "true" : "false" ) );
    BOSE_VERBOSE( s_logger, " " );

    return ( IsLpmReady( )             and
             IsCAPSReady( )            and
             IsAudioPathReady( )       and
             IsSTSReady( )             and
             IsSoftwareUpdateReady( )  and
             IsSassReady( )            and
             IsBluetoothModuleReady( ) and
             IsNetworkModuleReady( )   and
             IsVoiceModuleReady( ) );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   CustomProductController::IsLowPowerExited
///
/// @return This method returns a true or false value, based on a series of set member variables,
///         which all must be true to indicate that the device has exited low power.
///         NOTE: Unlike booting we should only wait for the things killed going to low power
///         However, for convenience and risk-reduction, we just call IsAllModuleReady()
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductController::IsLowPowerExited( ) const
{
    BOSE_INFO( s_logger, "------------ Product Controller Low Power Exit Check ---------------" );
    BOSE_INFO( s_logger, " " );
    return IsAllModuleReady();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   CustomProductController::IsAutoWakeEnabled
///
/// @return This method returns a true or false value, based on a set member variable.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductController::IsAutoWakeEnabled( ) const
{
    return m_IsAutoWakeEnabled;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   CustomProductController::IsSystemLanguageSet
///
/// @return This method returns true if the corresponding member has a system language defined.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductController::IsSystemLanguageSet( ) const
{
    return m_deviceManager.IsLanguageSet( );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   CustomProductController::GetOOBDefaultLastContentItem
///
/// @return This method returns the PassportPB::contentItem value to be used for initializing the OOB LastContentItem
///
////////////////////////////////////////////////////////////////////////////////////////////////////
PassportPB::contentItem CustomProductController::GetOOBDefaultLastContentItem() const
{
    using namespace ProductSTS;

    PassportPB::contentItem item;
    item.set_source( SHELBY_SOURCE::PRODUCT );
    item.set_sourceaccount( ProductSourceSlot_Name( TV ) );
    return item;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   CustomProductController::PossiblyPairBLERemote
///
/// @brief  initiates pairing of the BLE remote if indicated
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductController::PossiblyPairBLERemote( )
{
    // The rules are per PGC-697:
    // On a system without a paired BLE remote, entry into SETUP will activate BLE pairing.
    // On a system with a paired BLE remote, pressing and holding the Action Button will activate BLE pairing.
    m_ProductBLERemoteManager->PossiblyPair();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   CustomProductController::PairBLERemote
///
/// @brief  initiates pairing of the BLE remote
///
/// @param  manualPairingRequest
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductController::PairBLERemote( uint32_t timeout )
{
    // Tell the remote communications module to start pairing
    BOSE_INFO( s_logger, "%s requesting that the BLE remote pairing start", __func__ );

    m_ProductBLERemoteManager->Pairing_Start( timeout );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   CustomProductController::StopPairingBLERemote
///
/// @brief  stops pairing of the BLE remote
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductController::StopPairingBLERemote( )
{
    // No harm if pairing is not active
    // Tell the remote communications module to stop pairing
    BOSE_INFO( s_logger, "%s requesting that the BLE remote pairing stop", __func__ );

    m_ProductBLERemoteManager->Pairing_Cancel();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   CustomProductController::IsBLERemoteConnected
///
/// @return true if the BLE remote is actively connected
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductController::IsBLERemoteConnected( ) const
{
    return m_ProductBLERemoteManager->IsConnected();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   CustomProductController::SetEthernetEnabled
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductController::SetEthernetEnabled( bool enabled )
{
    if( m_ethernetEnabled != enabled )
    {
        m_ethernetEnabled = enabled;
        BOSE_INFO( s_logger, "%s ethernet being set to state %d", __func__, m_ethernetEnabled );
        if( m_ethernetEnabled )
        {
            SystemUtils::Spawn( { "ifconfig", "eth0", "down" } );
            SystemUtils::Spawn( { "ifconfig", "eth0", "up" } );
        }
        else
        {
            SystemUtils::Spawn( {"ip", "addr", "flush", "dev", "eth0" } );
            SystemUtils::Spawn( { "ifconfig", "eth0", "down" } );
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   CustomProductController::HandleCapsNowPlaying
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductController::HandleCapsNowPlaying( SoundTouchInterface::NowPlaying np )
{
    if( IsNowPlayingChanged( np ) )
    {
        m_radioStatus.set_btactive( np.container().contentitem().source() == SHELBY_SOURCE::BLUETOOTH );
        m_ProductLpmHardwareInterface->SendWiFiRadioStatus( m_radioStatus );

        BOSE_VERBOSE( s_logger, "Now Playing Changed Sent New Radio Status: %s", m_radioStatus.ShortDebugString().c_str() );
    }
    ProductController::HandleCapsNowPlaying( np );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   CustomProductController::GetDesiredPlayingVolume
///
/// @return std::pair<bool, int32_t> whether a volume change is desired, and the desired volume level
///
////////////////////////////////////////////////////////////////////////////////////////////////////
std::pair<bool, int32_t> CustomProductController::GetDesiredPlayingVolume( ) const
{
    BOSE_INFO( s_logger, "%s m_cachedVolume = {%s}", __func__, m_cachedVolume.DebugString( ).c_str( ) );

    int32_t desiredVolume = 0; // 0 will never be returned with changeDesired == true
    bool changeDesired = false;

    if( m_cachedVolume.has_value() )
    {
        // vet against the threshold values
        if( m_cachedVolume.value( ) < m_cachedVolume.min( ) )
        {
            desiredVolume = m_cachedVolume.min( );
            changeDesired = true;
        }
        else if( m_cachedVolume.value( ) > m_cachedVolume.max( ) )
        {
            desiredVolume = m_cachedVolume.max( );
            changeDesired = true;
        }
    }

    return { changeDesired, desiredVolume };
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   CustomProductController::SetupProductSTSController
///
/// @brief  This method is called to perform the needed initialization of the ProductSTSController,
///         specifically, provide the set of sources to be created initially.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductController::SetupProductSTSController( )
{
    using namespace ProductSTS;

    std::vector< ProductSTSController::SourceDescriptor > sources;

    ProductSTSStateFactory<ProductSTSStateTopSilent>        silentStateFactory;
    ProductSTSStateFactory<ProductSTSStateTopAiQ>           aiqStateFactory;
    ProductSTSStateFactory<ProductSTSStateDeviceControl>    deviceControlStateFactory;

    ///
    /// ADAPTIQ, SETUP, and PAIRING are never available as a normal source, whereas the TV source
    /// will always be available. SLOT sources need to be set-up before they become available.
    /// This is set up in CustomProductController::SendInitialCapsData().
    ///
    /// The "resumesupported" (5th) constructor argument (copied from "enabled" (3rd) if absent) determines whether the source is SETUP or PRODUCT.
    /// if false, the source is SETUP.
    /// If true, the source is PRODUCT.
    /// See ProductSTSController::Initialize().
    ///
    ProductSTSController::SourceDescriptor descriptor_SETUP   { SETUP,   SetupSourceSlot_Name( SETUP ),     false, silentStateFactory };
    ProductSTSController::SourceDescriptor descriptor_TV      { TV,      ProductSourceSlot_Name( TV ),      true,  deviceControlStateFactory };
    ProductSTSController::SourceDescriptor descriptor_ADAPTIQ { ADAPTIQ, SetupSourceSlot_Name( ADAPTIQ ),   false, aiqStateFactory    };
    ProductSTSController::SourceDescriptor descriptor_PAIRING { PAIRING, SetupSourceSlot_Name( PAIRING ),   false, silentStateFactory };
    ProductSTSController::SourceDescriptor descriptor_SLOT_0  { SLOT_0,  ProductSourceSlot_Name( SLOT_0 ),  false, deviceControlStateFactory, true };
    ProductSTSController::SourceDescriptor descriptor_SLOT_1  { SLOT_1,  ProductSourceSlot_Name( SLOT_1 ),  false, deviceControlStateFactory, true };
    ProductSTSController::SourceDescriptor descriptor_SLOT_2  { SLOT_2,  ProductSourceSlot_Name( SLOT_2 ),  false, deviceControlStateFactory, true };

    sources.push_back( descriptor_SETUP );
    sources.push_back( descriptor_TV );
    sources.push_back( descriptor_ADAPTIQ );
    sources.push_back( descriptor_PAIRING );
    sources.push_back( descriptor_SLOT_0 );
    sources.push_back( descriptor_SLOT_1 );
    sources.push_back( descriptor_SLOT_2 );

    Callback< void >
    CallbackForSTSComplete( std::bind( &ProductController::HandleSTSInitWasComplete,
                                       this ) );


    Callback< ProductSTSAccount::ProductSourceSlot >
    CallbackToHandleSelectSourceSlot( std::bind( &CustomProductController::HandleSelectSourceSlot,
                                                 this,
                                                 std::placeholders::_1 ) );

    m_ProductSTSController.Initialize( sources,
                                       CallbackForSTSComplete,
                                       CallbackToHandleSelectSourceSlot );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   CustomProductController::HandleSelectSourceSlot
///
/// @brief  This method is called from the ProductSTSController, when one of our sources is
///         activated by CAPS via STS.
///
/// @note   This method is called on the ProductSTSController task.
///
/// @param  ProductSTSAccount::ProductSourceSlot sourceSlot This identifies the activated slot.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductController::HandleSelectSourceSlot( ProductSTSAccount::ProductSourceSlot sourceSlot )
{
    ProductMessage message;
    message.mutable_selectsourceslot( )->set_slot( static_cast< ProductSTS::ProductSourceSlot >( sourceSlot ) );

    SendAsynchronousProductMessage( message );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name  CustomProductController::HandleUiHeartBeat
///
/// @brief  Handler for /ui/alive FrontDoor messages
///
/// @param  const DisplayControllerPb::UiHeartBeat & req
///
/// @param  const Callback<DisplayControllerPb::UiHeartBeat> & respCb
///
/// @param  const Callback<FrontDoor::Error> & errorCb
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductController::HandleUiHeartBeat(
    const DisplayControllerPb::UiHeartBeat & req,
    const Callback<DisplayControllerPb::UiHeartBeat> & respCb,
    const Callback<FrontDoor::Error> & errorCb )
{
    BOSE_INFO( s_logger, "%s received UI process heartbeat: %lld", __func__, req.count() );

    // Restart UI Timer
    m_uiAliveTimer->Stop();
    StartUiTimer();

    auto heartbeat = req.count();

    DisplayControllerPb::UiHeartBeat response;
    response.set_count( heartbeat );
    respCb( response );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name CustomProductController::StartUiTimer
///
/// @brief Initialize the UI recovery timer
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductController::StartUiTimer()
{
    m_uiAliveTimer->SetTimeouts( UI_ALIVE_TIMEOUT, 0 );

    m_uiAliveTimer->Start( [ this ]( )
    {
        KillUiProcess();
    } );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name CustomProductController::KillUiProcess
///
/// @brief Kill the UI process, prompting Shepherd to restart it
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductController::KillUiProcess()
{
    BOSE_ERROR( s_logger, "UI process has stopped. No heartbeat in %u MS.", UI_ALIVE_TIMEOUT );

    pid_t pid = 0;

    if( auto fileData = SystemUtils::ReadFile( UI_KILL_PID_FILE ) )
    {
        pid = strtol( fileData->c_str(), nullptr, 10 );
    }

    if( pid != 0 )
    {
        BOSE_ERROR( s_logger, "Killing UI process at pid %i", pid );

        // The nature of the WPE failure is still in question, so SIGKILL is used to ensure termination
        if( kill( pid, SIGKILL ) == -1 )
        {
            BOSE_ERROR( s_logger, "Couldn't kill UI process at pid %d: %s", pid, strerror( errno ) );
        }
    }
    else
    {
        BOSE_DIE( "Failed to recover UI process, pid not found" );
    }

    // Reset Timer
    StartUiTimer();
}


////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductController::HandleAudioVolumeNotification
///
/// @param const SoundTouchInterface::volume& volume
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductController::HandleAudioVolumeNotification( SoundTouchInterface::volume volume )
{
    BOSE_INFO( s_logger, "%s received: %s", __func__, ProtoToMarkup::ToJson( volume ).c_str() );

    m_cachedVolume = volume;
    m_ProductCecHelper->HandleFrontDoorVolume( volume );
}


////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   CustomProductController::RegisterFrontDoorEndPoints
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductController::RegisterFrontDoorEndPoints( )
{
    RegisterCommonEndPoints( );

    m_lightbarController->RegisterLightBarEndPoints( );


    {
        auto callback = [ this ]( Callback< ProductPb::PowerMacro > cb, Callback< FrontDoor::Error > errorCb )
        {
            HandleGetPowerMacro( cb, errorCb );
        };
        GetFrontDoorClient()->RegisterGet( FRONTDOOR_SYSTEM_POWER_MACRO_API,
                                           callback,
                                           FrontDoor::PUBLIC,
                                           FRONTDOOR_PRODUCT_CONTROLLER_VERSION,
                                           FRONTDOOR_PRODUCT_CONTROLLER_GROUP_NAME );
    }
    {
        auto callback = [ this ]( ProductPb::PowerMacro macro, Callback< ProductPb::PowerMacro > cb, Callback< FrontDoor::Error > errorCb )
        {
            HandlePutPowerMacro( macro, cb, errorCb );
        };
        GetFrontDoorClient()->RegisterPut<ProductPb::PowerMacro>( FRONTDOOR_SYSTEM_POWER_MACRO_API,
                                                                  callback,
                                                                  FrontDoor::PUBLIC,
                                                                  FRONTDOOR_PRODUCT_CONTROLLER_VERSION,
                                                                  FRONTDOOR_PRODUCT_CONTROLLER_GROUP_NAME );
    }
    {
        auto callback = [ this ]( Callback< SystemPowerProductPb::SystemPowerModeOpticalAutoWake > respCb,
                                  Callback< FrontDoor::Error > errorCb )
        {
            HandleGetOpticalAutoWake( respCb, errorCb );
        };

        GetFrontDoorClient()->RegisterGet( FRONTDOOR_SYSTEM_POWER_MODE_OPTICALAUTOWAKE_API,
                                           callback,
                                           FrontDoor::PUBLIC,
                                           FRONTDOOR_PRODUCT_CONTROLLER_VERSION,
                                           FRONTDOOR_PRODUCT_CONTROLLER_GROUP_NAME );
    }
    {
        auto callback = [ this ]( SystemPowerProductPb::SystemPowerModeOpticalAutoWake req,
                                  Callback< SystemPowerProductPb::SystemPowerModeOpticalAutoWake > respCb,
                                  Callback< FrontDoor::Error > errorCb )
        {
            HandlePutOpticalAutoWake( req, respCb, errorCb );
        };

        GetFrontDoorClient( )->RegisterPut<SystemPowerProductPb::SystemPowerModeOpticalAutoWake>(
            FRONTDOOR_SYSTEM_POWER_MODE_OPTICALAUTOWAKE_API,
            callback,
            FrontDoor::PUBLIC,
            FRONTDOOR_PRODUCT_CONTROLLER_VERSION,
            FRONTDOOR_PRODUCT_CONTROLLER_GROUP_NAME );
    }
    {
        auto callback = [ this ]( DisplayControllerPb::UiHeartBeat req,
                                  Callback< DisplayControllerPb::UiHeartBeat > respCb,
                                  Callback< FrontDoor::Error > errorCb )
        {
            HandleUiHeartBeat( req, respCb, errorCb );
        };


        GetFrontDoorClient( )->RegisterPut<DisplayControllerPb::UiHeartBeat>(
            FRONTDOOR_UI_ALIVE,
            callback,
            FrontDoor::PUBLIC,
            FRONTDOOR_PRODUCT_CONTROLLER_VERSION,
            FRONTDOOR_PRODUCT_CONTROLLER_GROUP_NAME );

        GetFrontDoorClient( )->RegisterPost<DisplayControllerPb::UiHeartBeat>(
            FRONTDOOR_UI_ALIVE,
            callback,
            FrontDoor::PUBLIC,
            FRONTDOOR_PRODUCT_CONTROLLER_VERSION,
            FRONTDOOR_PRODUCT_CONTROLLER_GROUP_NAME );
    }
    {
        //Audio volume callback for notifications
        AsyncCallback< SoundTouchInterface::volume >
        audioVolumeCb( std::bind( &CustomProductController::HandleAudioVolumeNotification,
                                  this,
                                  std::placeholders::_1 ),
                       GetTask( ) );

        //Audio volume notification registration
        m_FrontDoorClientIF->RegisterNotification< SoundTouchInterface::volume >(
            FRONTDOOR_AUDIO_VOLUME_API,
            audioVolumeCb );
    }
    {
        AsyncCallback< ProductPb::AccessoriesPlayTonesRequest, Callback< ProductPb::AccessoriesPlayTonesRequest >, Callback<FrontDoor::Error> >
        putAccessoriesCb( std::bind( &CustomProductController::AccessoriesPlayTonesPutHandler,
                                     this,
                                     std::placeholders::_1,
                                     std::placeholders::_2,
                                     std::placeholders::_3 ),
                          GetTask( ) );

        m_FrontDoorClientIF->RegisterPut<ProductPb::AccessoriesPlayTonesRequest>(
            FRONTDOOR_ACCESSORIES_PLAYTONES_API,
            putAccessoriesCb,
            FrontDoor::PUBLIC,
            FRONTDOOR_PRODUCT_CONTROLLER_VERSION,
            FRONTDOOR_PRODUCT_CONTROLLER_GROUP_NAME );
    }
    {
        AsyncCallback< Callback< SystemPowerPb::SystemPowerTimeouts >,
                       Callback< FrontDoor::Error > >
                       getSystemPowerCb( std::bind( &CustomProductController::HandleGetTimeouts,
                                                    this,
                                                    std::placeholders::_1,
                                                    std::placeholders::_2 ),
                                         GetTask( ) );

        m_FrontDoorClientIF->RegisterGet( FRONTDOOR_SYSTEM_POWER_TIMEOUTS_API,
                                          getSystemPowerCb,
                                          FrontDoor::PUBLIC,
                                          FRONTDOOR_PRODUCT_CONTROLLER_VERSION,
                                          FRONTDOOR_PRODUCT_CONTROLLER_GROUP_NAME );
    }
    {
        AsyncCallback< SystemPowerPb::SystemPowerTimeouts,
                       Callback<SystemPowerPb::SystemPowerTimeouts>,
                       Callback<FrontDoor::Error> >
                       putSystemPowerCb( std::bind( &CustomProductController::HandlePutTimeouts,
                                                    this,
                                                    std::placeholders::_1,
                                                    std::placeholders::_2,
                                                    std::placeholders::_3 ),
                                         GetTask( ) );

        m_FrontDoorClientIF->RegisterPut< SystemPowerPb::SystemPowerTimeouts >(
            FRONTDOOR_SYSTEM_POWER_TIMEOUTS_API,
            putSystemPowerCb,
            FrontDoor::PUBLIC,
            FRONTDOOR_PRODUCT_CONTROLLER_VERSION,
            FRONTDOOR_PRODUCT_CONTROLLER_GROUP_NAME );
    }
}

void CustomProductController::HandleGetTimeouts( Callback<SystemPowerPb::SystemPowerTimeouts> respCb,
                                                 Callback<FrontDoor::Error> errorCb )
{
    BOSE_INFO( s_logger, "%s::%s", CLASS_NAME, __func__ );

    SystemPowerPb::SystemPowerTimeouts timeouts;
    timeouts.set_noaudio( GetInactivityTimers().IsTimerEnabled( InactivityTimerType::NO_AUDIO_TIMER ) );
    timeouts.set_novideo( GetInactivityTimers().IsTimerEnabled( InactivityTimerType::NO_VIDEO_TIMER ) );
    respCb( timeouts );
}

void CustomProductController::HandlePutTimeouts( SystemPowerPb::SystemPowerTimeouts req,
                                                 Callback<SystemPowerPb::SystemPowerTimeouts> respCb,
                                                 Callback<FrontDoor::Error> errorCb )
{
    BOSE_INFO( s_logger, "%s::%s: req = %s", CLASS_NAME, __func__, req.DebugString().c_str() );

    if( !req.has_noaudio( ) && !req.has_novideo( ) )
    {
        FrontDoor::Error errorMessage;
        errorMessage.set_code( PGCErrorCodes::ERROR_CODE_PRODUCT_CONTROLLER_CUSTOM );
        errorMessage.set_subcode( PGCErrorCodes::ERROR_SUBCODE_INACTIVITY_TIMERS );
        errorMessage.set_message( "noaudio and novideo arguments not found." );
        errorCb( errorMessage );
        return;
    }

    if( req.has_noaudio( ) )
    {
        GetInactivityTimers().EnableTimer( InactivityTimerType::NO_AUDIO_TIMER, req.noaudio( ) );
    }
    if( req.has_novideo( ) )
    {
        GetInactivityTimers().EnableTimer( InactivityTimerType::NO_VIDEO_TIMER, req.novideo( ) );
    }

    HandleGetTimeouts( respCb, errorCb );

    m_FrontDoorClientIF->SendNotification( FRONTDOOR_SYSTEM_POWER_TIMEOUTS_API, req );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   GetWiFiOperationalMode
///
/// @return NetManager::Protobuf::OperationalMode of the WiFi subsystem
///
////////////////////////////////////////////////////////////////////////////////////////////////////
NetManager::Protobuf::OperationalMode CustomProductController::GetWiFiOperationalMode( ) const
{
    return GetNetworkServiceUtil().GetNetManagerOperationMode();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   CustomProductController::HandleMessage
///
/// @brief  This method is called to handle product controller messages, which are sent from the
///         more product specific class instances, and is used to process the state machine for the
///         product.
///
/// @param  ProductMessage& message This argument contains product message event information based
///                                 on the ProductMessage Protocal Buffer.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductController::HandleMessage( const ProductMessage& message )
{
    BOSE_INFO( s_logger, "%s received %s", __func__, message.DebugString().c_str() );

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// LPM status messages require both product-specific and common handling.
    ///////////////////////////////////////////////////////////////////////////////////////////////
    if( message.has_lpmstatus( ) )
    {
        ///
        /// Register for product-specific LPM events if connected. Common handling of the product
        /// message is then done.
        ///
        if( message.lpmstatus( ).has_connected( ) && message.lpmstatus( ).connected( ) )
        {
            m_lightbarController->RegisterLpmEvents();
        }

        ( void ) HandleCommonProductMessage( message );
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// STS slot selected data is handled at this point.
    ///////////////////////////////////////////////////////////////////////////////////////////////
    else if( message.has_selectsourceslot( ) )
    {
        const auto& slot = message.selectsourceslot( ).slot( );

        BOSE_DEBUG( s_logger, "An STS Select message was received for slot %d.", slot );
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// Wireless network status messages are handled at this point.
    ///////////////////////////////////////////////////////////////////////////////////////////////
    else if( message.has_wirelessstatus( ) )
    {
        ///
        /// Send the frequency information (if available) to the LPM to avoid any frequency
        /// interference between the WiFi and in-room radio.
        ///
        if( message.wirelessstatus( ).has_frequencykhz( ) and
            message.wirelessstatus( ).frequencykhz( ) > 0 )
        {
            IpcRadioStatus_t radioStatus;
            radioStatus.set_status( m_radioStatus.status() );
            radioStatus.set_band( m_radioStatus.band( ) );

            if( message.wirelessstatus( ).frequencykhz( ) > 2300000 and
                message.wirelessstatus().frequencykhz( ) < 2600000 )
            {
                radioStatus.set_status( IPC_SOC_NETWORKSTATUS_WIFI );
                radioStatus.set_band( IPC_SOC_RADIO_BAND_24 );
            }
            else if( message.wirelessstatus( ).frequencykhz( ) >= 5000000 and
                     message.wirelessstatus( ).frequencykhz( ) <= 5300000 )
            {
                radioStatus.set_status( IPC_SOC_NETWORKSTATUS_WIFI );
                radioStatus.set_band( IPC_SOC_RADIO_BAND_52 );
            }
            else if( message.wirelessstatus( ).frequencykhz( ) >= 5600000 and
                     message.wirelessstatus( ).frequencykhz( ) <= 5900000 )
            {
                radioStatus.set_status( IPC_SOC_NETWORKSTATUS_WIFI );
                radioStatus.set_band( IPC_SOC_RADIO_BAND_58 );
            }
            else
            {
                BOSE_ERROR( s_logger, "A wireless network message was received with an unknown frequency." );
            }


            if( radioStatus.status() != m_radioStatus.status() ||
                radioStatus.band() != m_radioStatus.band() )
            {
                m_radioStatus.CopyFrom( radioStatus );
                m_ProductLpmHardwareInterface->SendWiFiRadioStatus( m_radioStatus );
            }
        }
        else
        {
            BOSE_ERROR( s_logger, "A wireless network message was received with an unknown frequency." );
        }
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// Handle network operationalmode at this point
    ///////////////////////////////////////////////////////////////////////////////////////////////
    else if( message.has_networkstatus() )
    {
        // When switching away from ethernet we should bring down the interface
        // and let wifi connect
        if( m_isNetworkWired != GetNetworkServiceUtil().IsNetworkConnected() and m_isNetworkWired )
        {
            GetHsm( ).Handle< >( &CustomProductControllerState::HandleEthernetConnectionRemoved );
            m_isNetworkWired = GetNetworkServiceUtil().IsNetworkConnected();
        }
        // if wired we need to update lpm
        if( GetNetworkServiceUtil().IsNetworkWired() )
        {
            m_radioStatus.set_status( IPC_SOC_NETWORKSTATUS_ETHERNET );
            m_radioStatus.set_band( IPC_SOC_RADIO_BAND_INVALID );
            m_ProductLpmHardwareInterface->SendWiFiRadioStatus( m_radioStatus );
        }
        else if( not GetNetworkServiceUtil().IsNetworkConnected() )
        {
            m_radioStatus.set_status( IPC_SOC_NETWORKSTATUS_OFF );
            m_radioStatus.set_band( IPC_SOC_RADIO_BAND_INVALID );
            m_ProductLpmHardwareInterface->SendWiFiRadioStatus( m_radioStatus );
        }

        ( void ) HandleCommonProductMessage( message );
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// Autowake messages are handled at this point.
    ///////////////////////////////////////////////////////////////////////////////////////////////
    else if( message.has_autowakestatus( ) )
    {
        if( message.autowakestatus( ).has_active( ) )
        {
            m_IsAutoWakeEnabled = message.autowakestatus( ).active( );
        }
        else
        {
            BOSE_ERROR( s_logger, "An invalid autowake status message was received." );
            return;
        }

        NotifyFrontdoorAndStoreOpticalAutoWakeSetting( );

        m_ProductLpmHardwareInterface->SendAutowakeStatus( m_IsAutoWakeEnabled );

        GetHsm( ).Handle< bool >
        ( &CustomProductControllerState::HandleAutowakeStatus, m_IsAutoWakeEnabled );
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// Accessory pairing messages are handled at this point.
    ///////////////////////////////////////////////////////////////////////////////////////////////
    else if( message.has_accessorypairing( ) )
    {
        GetHsm( ).Handle< ProductPb::AccessorySpeakerState >
        ( &CustomProductControllerState::HandlePairingStatus, message.accessorypairing( ) );
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// Key action messages are handled at this point, and passed to the state machine based on
    /// the intent associated with the action.
    ///////////////////////////////////////////////////////////////////////////////////////////////
    else if( message.has_action( ) )
    {
        // Note that "action" may be changed by code below, e.g., by FilterIntent()
        auto action = message.action();

        if( m_ProductKeyInputManager->FilterIntent( action ) )
        {
            BOSE_VERBOSE( s_logger, "Action key %s ignored", CustomProductKeyInputManager::IntentName( action ).c_str() );
        }
        ///
        /// The following attempts to handle the key action using a common intent
        /// manager.
        ///
        else if( HandleCommonIntents( action ) )
        {
            BOSE_VERBOSE( s_logger, "Action key %s handled by common intent handler", CustomProductKeyInputManager::IntentName( action ).c_str() );
        }
        ///
        /// The following determines whether the key action is to be handled by the custom intent
        /// manager.
        ///
        else if( GetIntentHandler( ).IsIntentBootupFactoryDefault( action ) )
        {
            // These intents are handled statelessly because at most, if not ignored, they only trigger another ProductMessage which
            // would then be handled statefully
            GetIntentHandler( ).Handle( action );
        }
        else if( GetIntentHandler( ).IsIntentMuteControl( action ) )
        {
            GetHsm( ).Handle< KeyHandlerUtil::ActionType_t >( &CustomProductControllerState::HandleIntentMuteControl,
                                                              action );
        }
        else if( GetIntentHandler( ).IsIntentSpeakerPairing( action ) )
        {
            GetHsm( ).Handle< KeyHandlerUtil::ActionType_t >( &CustomProductControllerState::HandleIntentSpeakerPairing,
                                                              action );
        }
        else if( GetIntentHandler( ).IsIntentPlayProductSource( action ) )
        {
            GetHsm( ).Handle< KeyHandlerUtil::ActionType_t >( &CustomProductControllerState::HandleIntentPlayProductSource,
                                                              action );
        }
        else if( GetIntentHandler( ).IsIntentRating( action ) )
        {
            GetHsm( ).Handle< KeyHandlerUtil::ActionType_t >( &CustomProductControllerState::HandleIntentRating,
                                                              action );
        }
        else if( GetIntentHandler( ).IsIntentPlaySoundTouchSource( action ) )
        {
            GetHsm( ).Handle<>( &CustomProductControllerState::HandleIntentPlaySoundTouchSource );
        }
        else if( GetIntentHandler( ).IsIntentSetupBLERemote( action ) )
        {
            GetHsm( ).Handle<>( &CustomProductControllerState::HandleIntentSetupBLERemote );
        }
        else if( GetIntentHandler( ).IsIntentAudioModeToggle( action ) )
        {
            GetHsm( ).Handle< KeyHandlerUtil::ActionType_t >( &CustomProductControllerState::HandleIntentAudioModeToggle,
                                                              action );
        }
        else if( GetIntentHandler( ).IsIntentVoiceListening( action ) )
        {
            GetHsm( ).Handle<>( &CustomProductControllerState::HandleIntentVoiceListening );
        }
        else if( GetIntentHandler( ).IsIntentForceUpdate( action ) )
        {
            GetHsm( ).Handle<>( &CustomProductControllerState::HandleIntentManualSoftwareInstall );
        }
        else
        {
            BOSE_ERROR( s_logger, "An action key %s was received that has no associated intent.", CustomProductKeyInputManager::IntentName( action ).c_str() );

            GetHsm( ).Handle< KeyHandlerUtil::ActionType_t >( &CustomProductControllerState::HandleIntent,
                                                              action );
        }
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// AdaptIQ status messages are handled at this point.
    ///////////////////////////////////////////////////////////////////////////////////////////////
    else if( message.has_aiqstatus( ) )
    {
        GetHsm( ).Handle< const ProductAdaptIQStatus & >
        ( &CustomProductControllerState::HandleAdaptIQStatus, message.aiqstatus( ) );
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// AdaptIQ controls messages are handled at this point.
    ///////////////////////////////////////////////////////////////////////////////////////////////
    else if( message.has_aiqcontrol( ) )
    {
        GetHsm( ).Handle< const ProductAdaptIQControl & >
        ( &CustomProductControllerState::HandleAdaptIQControl, message.aiqcontrol( ) );
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// CecMode  messages are handled at this point.
    ///////////////////////////////////////////////////////////////////////////////////////////////
    else if( message.has_cecmode( ) )
    {
        m_ProductLpmHardwareInterface->SetCecMode( message.cecmode( ).cecmode( ) );
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// accessoriesareknown  messages are handled at this point.
    ///////////////////////////////////////////////////////////////////////////////////////////////
    else if( message.has_accessoriesareknown( ) )
    {
        BOSE_DEBUG( s_logger, "accessoriesareknown received" );
        m_AccessoriesAreKnown = true;
        GetHsm( ).Handle<>( &CustomProductControllerState::HandleAccessoriesAreKnown );
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// softwareupdatestatus needs to be forwarded to AccessorySoftwareInstallManager in addition to Common handling
    ///////////////////////////////////////////////////////////////////////////////////////////////
    else if( message.has_softwareupdatestatus() )
    {
        m_AccessorySoftwareInstallManager.ProductSoftwareUpdateStateNotified( );
        ( void ) HandleCommonProductMessage( message );
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// accessoriesplaytones messages are handled at this point.
    ///////////////////////////////////////////////////////////////////////////////////////////////
    else if( message.has_accessoriesplaytones() )
    {
        AccessoriesPlayTones( message.accessoriesplaytones( ).subs( ), message.accessoriesplaytones( ).rears( ) );
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// dspbooted messages are handled at this point.
    ///////////////////////////////////////////////////////////////////////////////////////////////
    else if( message.has_dspbooted() )
    {
        GetHsm( ).Handle< const LpmServiceMessages::IpcDeviceBoot_t& >( &CustomProductControllerState::HandleDspBooted, message.dspbooted() );
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// Messages handled in the common code based are processed at this point, unless the message
    /// type is unknown.
    ///////////////////////////////////////////////////////////////////////////////////////////////
    else if( not HandleCommonProductMessage( message ) )
    {
        BOSE_ERROR( s_logger, "An unknown message type was received - %s.",
                    ProtoToMarkup::ToJson( message ).c_str() );
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   CustomProductController::Wait
///
/// @brief  This method is called from a calling task to wait until the Product Controller process
///         ends. It is running from the main task that started the application.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductController::Wait( )
{
    while( m_Running )
    {
        sleep( PRODUCT_CONTROLLER_RUNNING_CHECK_IN_SECONDS );
    }

    ///
    /// Stop all the submodules.
    ///
    m_ProductLpmHardwareInterface->Stop( );
    m_ProductCommandLine         ->Stop( );
    m_ProductKeyInputManager     ->Stop( );
    m_ProductFrontDoorKeyInjectIF->Stop( );
    m_ProductCecHelper           ->Stop( );
    m_ProductDspHelper           ->Stop( );
    m_ProductAdaptIQManager      ->Stop( );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   CustomProductController::GetDefaultProductName
///
/// @brief  This method is used to get the default product name.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
std::string CustomProductController::GetDefaultProductName( ) const
{
    std::string productName;

    ///
    /// Ensure that the device has a valid marketing product name, based on the manufacturing data;
    /// and assign this value to the default product name initially.
    ///
    if( auto productNameValue = MfgData::Get( "productName" ) )
    {
        productName = *productNameValue;
    }
    else
    {
        BOSE_DIE( __func__ << " Fatal Error: No Product Name " );
    }

    ///
    /// Leave the default product name assigned to the marketing product name in the manufacturing
    /// data for production non-development devices; otherwise, assign the default product name
    /// based on its MAC address and product type.
    ///
    if( IsDevelopmentMode( ) )
    {
        std::string macAddress = MacAddressInfo::GetPrimaryMAC( );

        try
        {
            productName = ( macAddress.substr( macAddress.length() - 6 ) );
        }
        catch( const std::out_of_range& error )
        {
            productName = macAddress;

            BOSE_WARNING( s_logger, "%s Warning: Incomplete MAC Address %s", __func__, macAddress.c_str( ) );
        }

        std::string productType;

        if( auto productTypeValue = MfgData::Get( "productType" ) )
        {
            productType = *productTypeValue;

            if( productType.compare( "professor" ) == 0 )
            {
                productName += " SB 500";
            }
            else if( productType.compare( "ginger-cheevers" ) == 0 )
            {
                productName += " SB 700";
            }
            else
            {
                BOSE_DIE( __func__ << " Fatal Error: Invalid Product Type " <<  productType );
            }
        }
    }

    BOSE_INFO( s_logger, "%s: The default product name is %s.", __func__, productName.c_str( ) );

    return productName;
}

void CustomProductController::SendSystemSourcesPropertiesToCAPS()
{
    using namespace SoundTouchInterface;
    // Populate /system/sources::properties
    Sources message;
    auto messageProperties = message.mutable_properties();

    for( uint32_t activationKey = SystemSourcesProperties::ACTIVATION_KEY__MIN;
         activationKey <= SystemSourcesProperties::ACTIVATION_KEY__MAX;
         ++activationKey )
    {
        messageProperties->add_supportedactivationkeys(
            SystemSourcesProperties::ACTIVATION_KEY__Name( static_cast<SystemSourcesProperties::ACTIVATION_KEY_>( activationKey ) ) );
    }
    messageProperties->set_activationkeyrequired( true );

    for( uint32_t deviceType = SystemSourcesProperties::DEVICE_TYPE__MIN; deviceType <= SystemSourcesProperties::DEVICE_TYPE__MAX; ++deviceType )
    {
        messageProperties->add_supporteddevicetypes(
            SystemSourcesProperties::DEVICE_TYPE__Name( static_cast<SystemSourcesProperties::DEVICE_TYPE_>( deviceType ) ) );
    }
    messageProperties->set_devicetyperequired( true );

    messageProperties->add_supportedinputroutes(
        SystemSourcesProperties::INPUT_ROUTE_HDMI__Name( SystemSourcesProperties::INPUT_ROUTE_TV ) );

    messageProperties->set_inputrouterequired( false );

    for( uint32_t friendlyName = SystemSourcesProperties::FRIENDLY_NAME__MIN; friendlyName <= SystemSourcesProperties::FRIENDLY_NAME__MAX; ++friendlyName )
    {
        messageProperties->add_supportedfriendlynames(
            SystemSourcesProperties::FRIENDLY_NAME__Name( static_cast<SystemSourcesProperties::FRIENDLY_NAME_>( friendlyName ) ) );
    }

    messageProperties->set_friendlynamerequired( false );

    auto sourcesRespCb = []( Sources sources )
    {
        BOSE_INFO( s_logger, FRONTDOOR_SYSTEM_SOURCES_API " properties: %s", sources.properties( ).DebugString( ).c_str( ) );
    };

    GetFrontDoorClient()->SendPut<Sources, FrontDoor::Error>(
        FRONTDOOR_SYSTEM_SOURCES_API,
        message,
        sourcesRespCb,
        m_errorCb );
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

    // Properties are sent unconditionally. We need to accommodate systems that were wet up with earlier software
    // versions and may be missing elements introduced later. E.g., Friendly Names introduced by MONTAUK-323.
    SendSystemSourcesPropertiesToCAPS();

    if( defaultCAPSValuesDone )
    {
        // GET the current values, we may have missed an initial notification
        AsyncCallback< volume >
        audioVolumeCb( std::bind( &CustomProductController::HandleAudioVolumeNotification,
                                  this,
                                  std::placeholders::_1 ),
                       GetTask( ) );

        m_FrontDoorClientIF->SendGet<volume, FrontDoor::Error>(
            FRONTDOOR_AUDIO_VOLUME_API,
            audioVolumeCb,
            m_errorCb );
    }
    else
    {
        // Do this only once, after factory default
        if( ! SystemUtils::WriteFile( "", DefaultCAPSValuesStateFile ) )
        {
            BOSE_CRITICAL( s_logger, "File write to %s Failed", DefaultCAPSValuesStateFile.c_str( ) );
        }
        volume desiredVolume;
        desiredVolume.set_min( VOLUME_MIN_THRESHOLD );
        desiredVolume.set_max( VOLUME_MAX_THRESHOLD );
        GetFrontDoorClient()->SendPut<volume, FrontDoor::Error>(
            FRONTDOOR_AUDIO_VOLUME_API,
            desiredVolume,
            { },
            m_errorCb );
        BOSE_INFO( s_logger, "%s sent %s", __func__, desiredVolume.DebugString( ).c_str( ) );

        // Populate /system/sources::properties
        Sources message;

        // Populate status and visibility of PRODUCT sources.
        using namespace ProductSTS;

        Sources_SourceItem* source = message.add_sources( );
        source->set_sourcename( SHELBY_SOURCE::PRODUCT );
        source->set_sourceaccountname( ProductSourceSlot_Name( TV ) );
        source->set_accountid( ProductSourceSlot_Name( TV ) );
        source->set_status( SourceStatus::NOT_CONFIGURED );
        source->set_visible( true );

        source = message.add_sources( );
        source->set_sourcename( SHELBY_SOURCE::PRODUCT );
        source->set_sourceaccountname( ProductSourceSlot_Name( SLOT_0 ) );
        source->set_accountid( ProductSourceSlot_Name( SLOT_0 ) );
        source->set_status( SourceStatus::NOT_CONFIGURED );
        source->set_visible( false );

        source = message.add_sources( );
        source->set_sourcename( SHELBY_SOURCE::PRODUCT );
        source->set_sourceaccountname( ProductSourceSlot_Name( SLOT_1 ) );
        source->set_accountid( ProductSourceSlot_Name( SLOT_1 ) );
        source->set_status( SourceStatus::NOT_CONFIGURED );
        source->set_visible( false );

        source = message.add_sources( );
        source->set_sourcename( SHELBY_SOURCE::PRODUCT );
        source->set_sourceaccountname( ProductSourceSlot_Name( SLOT_2 ) );
        source->set_accountid( ProductSourceSlot_Name( SLOT_2 ) );
        source->set_status( SourceStatus::NOT_CONFIGURED );
        source->set_visible( false );

        // Set the (in)visibility of SETUP sources.
        source = message.add_sources( );
        source->set_sourcename( SHELBY_SOURCE::SETUP );
        source->set_sourceaccountname( SetupSourceSlot_Name( SETUP ) );
        source->set_accountid( SetupSourceSlot_Name( SETUP ) );
        source->set_status( SourceStatus::UNAVAILABLE );
        source->set_visible( false );

        source = message.add_sources( );
        source->set_sourcename( SHELBY_SOURCE::SETUP );
        source->set_sourceaccountname( SetupSourceSlot_Name( ADAPTIQ ) );
        source->set_accountid( SetupSourceSlot_Name( ADAPTIQ ) );
        source->set_status( SourceStatus::UNAVAILABLE );
        source->set_visible( false );

        source = message.add_sources( );
        source->set_sourcename( SHELBY_SOURCE::SETUP );
        source->set_sourceaccountname( SetupSourceSlot_Name( PAIRING ) );
        source->set_accountid( SetupSourceSlot_Name( PAIRING ) );
        source->set_status( SourceStatus::UNAVAILABLE );
        source->set_visible( false );

        auto sourcesRespCb = []( Sources sources )
        {
            BOSE_INFO( s_logger, FRONTDOOR_SYSTEM_SOURCES_API " properties: %s", sources.properties( ).DebugString( ).c_str( ) );
        };

        GetFrontDoorClient()->SendPut<Sources, FrontDoor::Error>(
            FRONTDOOR_SYSTEM_SOURCES_API,
            message,
            sourcesRespCb,
            m_errorCb );
        BOSE_INFO( s_logger, "%s sent %s", __func__, message.DebugString( ).c_str( ) );
    }

    std::string DefaultRebroadcastLatencyModeFile{ g_PersistenceRootDir };
    DefaultRebroadcastLatencyModeFile += g_ProductPersistenceDir;
    DefaultRebroadcastLatencyModeFile += g_DefaultRebroadcastLatencyModeFile;
    const bool defaultRebroadcastLatencyModeDone = SystemUtils::Exists( DefaultRebroadcastLatencyModeFile );
    if( !defaultRebroadcastLatencyModeDone )
    {
        // Do this only once, after factory default or on a system before RebroadcastLatencyMode existed
        if( ! SystemUtils::WriteFile( "", DefaultRebroadcastLatencyModeFile ) )
        {
            BOSE_CRITICAL( s_logger, "File write to %s Failed", DefaultRebroadcastLatencyModeFile.c_str( ) );
        }

        // Set the default value for /audio/rebroadcastLatency/mode
        RebroadcastLatencyModeMsg rebroadcastLatencyModeMsg;
        rebroadcastLatencyModeMsg.set_mode( APControlMsgRebroadcastLatencyMode::APRebroadcastLatencyMode_Name( APControlMsgRebroadcastLatencyMode::SYNC_TO_ROOM ) );
        GetFrontDoorClient()->SendPut<RebroadcastLatencyModeMsg, FrontDoor::Error>(
            FRONTDOOR_AUDIO_REBROADCASTLATENCY_MODE_API,
            rebroadcastLatencyModeMsg,
            { },
            m_errorCb );
        BOSE_INFO( s_logger, "%s sent %s", __func__, rebroadcastLatencyModeMsg.DebugString( ).c_str( ) );
    }

    // Do the Common stuff last, the operations above must come first
    ProductController::SendInitialCapsData();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductController::HandleGetOpticalAutoWake
///
/// @param const Callback<SystemPowerProductPb::SystemPowerModeOpticalAutoWake> & respCb
///
/// @param const Callback<FrontDoor::Error> & errorCb
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductController::HandleGetOpticalAutoWake(
    const Callback<SystemPowerProductPb::SystemPowerModeOpticalAutoWake> & respCb,
    const Callback<FrontDoor::Error> & errorCb ) const
{
    SystemPowerProductPb::SystemPowerModeOpticalAutoWake autowake;
    autowake.set_enabled( m_IsAutoWakeEnabled );
    respCb( autowake );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductController::HandlePutOpticalAutoWake
///
/// @param const SystemPowerProductPb::SystemPowerModeOpticalAutoWake & req
///
/// @param const Callback<SystemPowerProductPb::SystemPowerModeOpticalAutoWake> & respCb
///
/// @param const Callback<FrontDoor::Error> & errorCb
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductController::HandlePutOpticalAutoWake(
    const SystemPowerProductPb::SystemPowerModeOpticalAutoWake & req,
    const Callback<SystemPowerProductPb::SystemPowerModeOpticalAutoWake> & respCb,
    const Callback<FrontDoor::Error> & errorCb )
{
    BOSE_INFO( s_logger, "%s::%s - req = %s", CLASS_NAME, __func__, req.DebugString().c_str() );

    if( req.has_enabled( ) )
    {
        ProductMessage message;
        message.mutable_autowakestatus( )->set_active( req.enabled( ) );

        SendAsynchronousProductMessage( message );
        respCb( req );
    }
    else
    {
        FrontDoor::Error error;
        error.set_code( PGCErrorCodes::ERROR_CODE_PRODUCT_CONTROLLER_CUSTOM );
        error.set_subcode( PGCErrorCodes::ERROR_SUBCODE_OPTICAL_AUTOWAKE );
        error.set_message( "Optical autowake mode was not specified." );
        errorCb.Send( error );
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductController::ApplyOpticalAutoWakeSettingFromPersistence
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductController::ApplyOpticalAutoWakeSettingFromPersistence( )
{
    auto persistence = ProtoPersistenceFactory::Create( "OpticalAutoWake.json", GetProductPersistenceDir( ) );
    SystemPowerProductPb::SystemPowerModeOpticalAutoWake autowake;
    try
    {
        const std::string & s = persistence->Load( );
        ProtoToMarkup::FromJson( s, &autowake );
    }
    catch( const ProtoToMarkup::MarkupError & e )
    {
        BOSE_ERROR( s_logger, "OpticalAutoWake persistence markup error - %s", e.what( ) );
    }
    catch( ProtoPersistenceIF::ProtoPersistenceException & e )
    {
        BOSE_ERROR( s_logger, "OpticalAutoWake persistence error - %s", e.what( ) );
    }

    m_IsAutoWakeEnabled = autowake.enabled( );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductController::NotifyFrontdoorAndStoreOpticalAutoWakeSetting
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductController::NotifyFrontdoorAndStoreOpticalAutoWakeSetting( )
{
    auto persistence = ProtoPersistenceFactory::Create( "OpticalAutoWake.json", GetProductPersistenceDir( ) );
    SystemPowerProductPb::SystemPowerModeOpticalAutoWake autowake;
    autowake.set_enabled( m_IsAutoWakeEnabled );
    GetFrontDoorClient( )->SendNotification( FRONTDOOR_SYSTEM_POWER_MODE_OPTICALAUTOWAKE_API,
                                             autowake );

    try
    {
        persistence->Store( ProtoToMarkup::ToJson( autowake ) );
    }
    catch( const ProtoToMarkup::MarkupError & e )
    {
        BOSE_ERROR( s_logger, "OpticalAutoWake store persistence markup error - %s", e.what( ) );
    }
    catch( ProtoPersistenceIF::ProtoPersistenceException & e )
    {
        BOSE_ERROR( s_logger, "OpticalAutoWake store persistence error - %s", e.what( ) );
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductController::AttemptToStartPlayback
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductController::AttemptToStartPlayback()
{
    BOSE_INFO( s_logger, "Handling CustomAttemptToStartPlayback" );

    if( m_powerMacro.enabled() )
    {
        PassportPB::contentItem pwrMacroContentItem;

        pwrMacroContentItem.set_source( SHELBY_SOURCE::PRODUCT );
        pwrMacroContentItem.set_sourceaccount( ProductSTS::ProductSourceSlot_Name( m_powerMacro.powerondevice() ) );

        SendPlaybackRequestFromContentItem( pwrMacroContentItem );
        m_ProductKeyInputManager->ExecutePowerMacro( m_powerMacro, LpmServiceMessages::BOSE_ASSERT_ON );

        BOSE_INFO( s_logger, "An attempt to play the power macro content item %s has been made.",
                   pwrMacroContentItem.DebugString().c_str( ) );
    }
    else
    {
        // The last content item may point to a now-removed source (PGC-3439)
        if( m_lastContentItem.source( ) == SHELBY_SOURCE::PRODUCT )
        {
            auto sourceItem = GetSourceInfo( ).FindSource( SHELBY_SOURCE::PRODUCT, m_lastContentItem.sourceaccount( ) );
            if( !sourceItem || !GetSourceInfo( ).IsSourceAvailable( *sourceItem ) )
            {
                m_lastContentItem = GetOOBDefaultLastContentItem( );
            }
        }
        ProductController::AttemptToStartPlayback();
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductController::PowerMacroOff
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductController::PowerMacroOff()
{
    if( m_powerMacro.enabled() )
    {
        m_ProductKeyInputManager->ExecutePowerMacro( m_powerMacro, LpmServiceMessages::BOSE_ASSERT_OFF );
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductController::HandleGetPowerMacro
///
/// @param const Callback<SystemPowerProductPb::PowerMacro> & respCb
///
/// @param const Callback<FrontDoor::Error> & errorCb
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductController::HandleGetPowerMacro(
    const Callback<ProductPb::PowerMacro> & respCb,
    const Callback<FrontDoor::Error> & errorCb ) const
{
    respCb( m_powerMacro );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductController::HandlePutPowerMacro
///
/// @param const SystemPowerProductPb::SystemPowerModeOpticalAutoWake & req
///
/// @param const Callback<SystemPowerProductPb::PowerMacro> & respCb
///
/// @param const Callback<FrontDoor::Error> & errorCb
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductController::HandlePutPowerMacro(
    const ProductPb::PowerMacro & req,
    const Callback<ProductPb::PowerMacro> & respCb,
    const Callback<FrontDoor::Error> & errorCb )
{
    FrontDoor::Error error;

    bool success = true;

    if( req.enabled( ) || ( !req.has_enabled( ) && m_powerMacro.enabled( ) ) )
    {
        if( req.powerontv( ) )
        {
            const auto tvSource = GetSourceInfo( ).FindSource( SHELBY_SOURCE::PRODUCT, ProductSTS::ProductSourceSlot_Name( ProductSTS::TV ) );
            if( not( tvSource and tvSource->status() == SoundTouchInterface::SourceStatus::AVAILABLE ) )   // source status field has to be AVAILABLE in order to be controlled
            {
                error.set_message( "TV is not configured but power on tv requested!" );
                success = false;
            }
        }
        if( success and req.has_powerondevice( ) )
        {
            const auto reqSource = GetSourceInfo( ).FindSource( SHELBY_SOURCE::PRODUCT, ProductSTS::ProductSourceSlot_Name( req.powerondevice() ) );

            if( not( reqSource and reqSource->status() == SoundTouchInterface::SourceStatus::AVAILABLE ) )
            {
                error.set_message( "Requested source is not configured or available!" );
                success = false;
            }
        }
    }

    if( success )
    {
        m_powerMacro.MergeFrom( req ); // copy the Booleans
        if( m_powerMacro.enabled( ) && !req.has_powerondevice( ) )
        {
            m_powerMacro.clear_powerondevice( );
        }
        PersistPowerMacro();
        respCb( req );
    }
    else
    {
        error.set_code( PGCErrorCodes::ERROR_CODE_PRODUCT_CONTROLLER_CUSTOM );
        error.set_subcode( PGCErrorCodes::ERROR_SUBCODE_POWER_MACRO );
        errorCb( error );
        BOSE_WARNING( s_logger, "\"%s\": %s", req.ShortDebugString().c_str(), error.ShortDebugString().c_str() );
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductController::LoadPowerMacroFromPersistance
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductController::LoadPowerMacroFromPersistance( )
{
    auto persistence = ProtoPersistenceFactory::Create( "PowerMacro.json", GetProductPersistenceDir( ) );

    try
    {
        const std::string & s = persistence->Load( );
        ProtoToMarkup::FromJson( s, &m_powerMacro );
    }
    catch( const ProtoToMarkup::MarkupError & e )
    {
        BOSE_ERROR( s_logger, "Power Macro persistence markup error - %s", e.what( ) );
    }
    catch( ProtoPersistenceIF::ProtoPersistenceException & e )
    {
        BOSE_ERROR( s_logger, "Power Macro persistence error - %s", e.what( ) );
    }
    // Prior to PGC-4157 Boolean fields were not explicitly stored
    m_powerMacro.set_enabled( m_powerMacro.enabled( ) );
    m_powerMacro.set_powerontv( m_powerMacro.powerontv( ) );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductController::PersistPowerMacro
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductController::PersistPowerMacro( )
{
    auto persistence = ProtoPersistenceFactory::Create( "PowerMacro.json", GetProductPersistenceDir( ) );
    try
    {
        persistence->Store( ProtoToMarkup::ToJson( m_powerMacro ) );
    }
    catch( const ProtoToMarkup::MarkupError & e )
    {
        BOSE_ERROR( s_logger, "Power Macro store persistence markup error - %s", e.what( ) );
    }
    catch( ProtoPersistenceIF::ProtoPersistenceException & e )
    {
        BOSE_ERROR( s_logger, "Power Macro store persistence error - %s", e.what( ) );
    }
    GetFrontDoorClient( )->SendNotification( FRONTDOOR_SYSTEM_POWER_MACRO_API,
                                             m_powerMacro );
    SendPowerMacroToDataCollection( );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductController::SendPowerMacroToDataCollection
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductController::SendPowerMacroToDataCollection( )
{
    auto collectionData = std::make_shared<ProductPb::PowerMacro>( m_powerMacro );
    m_dataCollectionClient->SendData( collectionData, DATA_COLLECTION_POWER_MACRO );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductController::UpdatePowerMacro
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductController::UpdatePowerMacro( )
{
    BOSE_INFO( s_logger, "%s::%s", CLASS_NAME, __func__ );
    bool isChanged = false;
    // if devices enabled in power macro is removed from Control Integration
    // power macro should be updated, and turn control off automatically
    if( m_powerMacro.powerontv() )  // if "powerOnTv" field is not there, it will evaluate as false
    {
        const auto tvSource = GetSourceInfo( ).FindSource( SHELBY_SOURCE::PRODUCT, ProductSTS::ProductSourceSlot_Name( ProductSTS::TV ) );
        if( not( tvSource && tvSource->status( ) == SoundTouchInterface::SourceStatus::AVAILABLE ) )
        {
            m_powerMacro.set_powerontv( false );
            isChanged = true;
        }
    }
    if( m_powerMacro.has_powerondevice( ) )
    {
        const auto reqSource = GetSourceInfo( ).FindSource( SHELBY_SOURCE::PRODUCT, ProductSTS::ProductSourceSlot_Name( m_powerMacro.powerondevice() ) );

        if( not( reqSource and reqSource->status( ) == SoundTouchInterface::SourceStatus::AVAILABLE ) )
        {
            m_powerMacro.clear_powerondevice( );
            isChanged = true;
        }
    }
    if( m_powerMacro.enabled( ) &&
        !m_powerMacro.powerontv( ) &&
        !m_powerMacro.has_powerondevice( ) )
    {
        m_powerMacro.set_enabled( false );
        isChanged = true;
    }

    if( isChanged )
    {
        PersistPowerMacro( );
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductController::ReconcileCurrentProductSource
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductController::ReconcileCurrentProductSource( )
{
    if( GetNowSelection( ).has_contentitem( ) && GetNowSelection( ).contentitem( ).source( ) == SHELBY_SOURCE::PRODUCT )
    {
        auto sourceItem = GetSourceInfo( ).FindSource( SHELBY_SOURCE::PRODUCT, GetNowSelection( ).contentitem( ).sourceaccount( ) );
        if( !sourceItem || !GetSourceInfo( ).IsSourceAvailable( *sourceItem ) )
        {
            // If the active source becomes unavailable, switch to TV source (PGC-3375)
            KeyHandlerUtil::ActionType_t startTvPlayback = static_cast< KeyHandlerUtil::ActionType_t >( Action::ACTION_TV );
            ProductMessage message;
            message.set_action( startTvPlayback );

            SendAsynchronousProductMessage( message );
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductController::GetChimesConfigurationLocation
///
////////////////////////////////////////////////////////////////////////////////////////////////////
string CustomProductController::GetChimesConfigurationLocation( ) const
{
    string retVal{ g_ChimesConfigurationPath };
    retVal += GetProductType( );
    retVal += '/';
    return retVal + g_ChimesConfigurationFile;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductController::GetChimesFilesLocation
///
////////////////////////////////////////////////////////////////////////////////////////////////////
string CustomProductController::GetChimesFilesLocation( ) const
{
    string retVal{ g_ChimesPath };
    retVal += GetProductType( );
    return retVal + '/';
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductController::HandleChimeResponse
///
/// @param ChimesControllerPb::ChimesStatus status
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductController::HandleChimeResponse( ChimesControllerPb::ChimesStatus status )
{
    if( !HandleAccessoriesPlayTonesResponse( status ) )
    {
        ProductController::HandleChimeResponse( status );
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductController::HandleAccessoriesPlayTonesResponse
///
/// @param ChimesControllerPb::ChimesStatus status
///
/// @return true if the chime is Accessory-specific
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductController::HandleAccessoriesPlayTonesResponse( ChimesControllerPb::ChimesStatus status )
{
    BOSE_INFO( s_logger, "%s::%s received %s", CLASS_NAME, __FUNCTION__, status.DebugString( ).c_str( ) );

    if( status.event_id( ) != CHIME_ACCESSORY_PAIRING_COMPLETE_SUB  &&
        status.event_id( ) != CHIME_ACCESSORY_PAIRING_COMPLETE_REAR_SPEAKER )
    {
        // Defer to common handler
        return false;
    }

    if( m_queueRearAccessoryTone &&
        status.event_id( ) == CHIME_ACCESSORY_PAIRING_COMPLETE_SUB &&
        status.state( ) == ChimesControllerPb::ChimesStatus_ChimeState_COMPLETED )
    {
        m_queueRearAccessoryTone = false;
        HandleChimePlayRequest( CHIME_ACCESSORY_PAIRING_COMPLETE_REAR_SPEAKER );
    }
    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief SpeakerPairingManager::AccessoriesPlayTonesPutHandler
///
/// @param const ProductPb::AccessoriesPlayTonesRequest& req
///
/// @param Callback<ProductPb::AccessoriesPlayTonesRequest> resp
///
/// @param Callback<FrontDoor::Error> error
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductController::AccessoriesPlayTonesPutHandler( ProductPb::AccessoriesPlayTonesRequest req,
                                                              Callback<ProductPb::AccessoriesPlayTonesRequest> resp,
                                                              Callback<FrontDoor::Error> error )
{
    BOSE_INFO( s_logger, "%s::%s received %s", CLASS_NAME, __FUNCTION__, req.DebugString( ).c_str( ) );

    if( req.has_subs( ) || req.has_rears( ) )
    {
        AccessoriesPlayTones( req.subs( ), req.rears( ) );
    }
    else
    {
        BOSE_ERROR( s_logger, "Received empty PUT request!" );
        FrontDoor::Error errorMessage;
        errorMessage.set_code( PGCErrorCodes::ERROR_CODE_PRODUCT_CONTROLLER_CUSTOM );
        errorMessage.set_subcode( PGCErrorCodes::ERROR_SUBCODE_ACCESSORIES );
        errorMessage.set_message( "Accessory tone request empty." );
        error.Send( errorMessage );
        return;
    }
    resp( req );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductController::AccessoriesPlayTones
///
/// @param bool subs
///
/// @param bool rears
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductController::AccessoriesPlayTones( bool subs, bool rears )
{
    if( subs )
    {
        HandleChimePlayRequest( CHIME_ACCESSORY_PAIRING_COMPLETE_SUB );
    }
    if( rears )
    {
        if( subs )
        {
            m_queueRearAccessoryTone = true;
        }
        else
        {
            HandleChimePlayRequest( CHIME_ACCESSORY_PAIRING_COMPLETE_REAR_SPEAKER );
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductController::HandleVoiceStatus
///
/// @param VoiceServicePB::VoiceStatus voiceStatus
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductController::HandleVoiceStatus( VoiceServicePB::VoiceStatus voiceStatus )
{
    ProductController::HandleVoiceStatus( voiceStatus );

    if( voiceStatus != m_voiceStatus )
    {
        m_voiceStatus = voiceStatus;
        if( m_voiceStatus == VoiceServicePB::VoiceStatus::LISTENING )
        {
            ProductMessage productMessage;
            auto listening = static_cast< KeyHandlerUtil::ActionType_t >( Action::ACTION_VOICE_LISTENING );
            productMessage.set_action( listening );
            SendAsynchronousProductMessage( productMessage );
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductController::InitializeKeyIdToKeyNameMap
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductController::InitializeKeyIdToKeyNameMap()
{
    BOSE_INFO( s_logger, "CustomProductController::%s:", __func__ );

    // Professor team need to coordinate with the UI team to know which keys are of interest to them
    m_keyIdToKeyNameMap[std::make_pair( KeyOrigin_t::KEY_ORIGIN_IR, LpmServiceMessages::BOSE_VOLUME_UP )]        = KeyNamesPB::keynames::VOLUME_UP;
    m_keyIdToKeyNameMap[std::make_pair( KeyOrigin_t::KEY_ORIGIN_IR, LpmServiceMessages::BOSE_VOLUME_DOWN )]      = KeyNamesPB::keynames::VOLUME_DOWN;
    m_keyIdToKeyNameMap[std::make_pair( KeyOrigin_t::KEY_ORIGIN_IR, LpmServiceMessages::BOSE_NUMBER_1 )]         = KeyNamesPB::keynames::PRESET_1;
    m_keyIdToKeyNameMap[std::make_pair( KeyOrigin_t::KEY_ORIGIN_IR, LpmServiceMessages::BOSE_NUMBER_2 )]         = KeyNamesPB::keynames::PRESET_2;
    m_keyIdToKeyNameMap[std::make_pair( KeyOrigin_t::KEY_ORIGIN_IR, LpmServiceMessages::BOSE_NUMBER_3 )]         = KeyNamesPB::keynames::PRESET_3;
    m_keyIdToKeyNameMap[std::make_pair( KeyOrigin_t::KEY_ORIGIN_IR, LpmServiceMessages::BOSE_NUMBER_4 )]         = KeyNamesPB::keynames::PRESET_4;
    m_keyIdToKeyNameMap[std::make_pair( KeyOrigin_t::KEY_ORIGIN_IR, LpmServiceMessages::BOSE_NUMBER_5 )]         = KeyNamesPB::keynames::PRESET_5;
    m_keyIdToKeyNameMap[std::make_pair( KeyOrigin_t::KEY_ORIGIN_IR, LpmServiceMessages::BOSE_NUMBER_6 )]         = KeyNamesPB::keynames::PRESET_6;
    m_keyIdToKeyNameMap[std::make_pair( KeyOrigin_t::KEY_ORIGIN_IR, LpmServiceMessages::BOSE_MUTE )]             = KeyNamesPB::keynames::MUTE;
    m_keyIdToKeyNameMap[std::make_pair( KeyOrigin_t::KEY_ORIGIN_IR, LpmServiceMessages::BOSE_BLUETOOTH_SOURCE )] = KeyNamesPB::keynames::BLUETOOTH;

    m_keyIdToKeyNameMap[std::make_pair( KeyOrigin_t::KEY_ORIGIN_RF, LpmServiceMessages::BOSE_VOLUME_UP )]        = KeyNamesPB::keynames::VOLUME_UP;
    m_keyIdToKeyNameMap[std::make_pair( KeyOrigin_t::KEY_ORIGIN_RF, LpmServiceMessages::BOSE_VOLUME_DOWN )]      = KeyNamesPB::keynames::VOLUME_DOWN;
    m_keyIdToKeyNameMap[std::make_pair( KeyOrigin_t::KEY_ORIGIN_RF, LpmServiceMessages::BOSE_NUMBER_1 )]         = KeyNamesPB::keynames::PRESET_1;
    m_keyIdToKeyNameMap[std::make_pair( KeyOrigin_t::KEY_ORIGIN_RF, LpmServiceMessages::BOSE_NUMBER_2 )]         = KeyNamesPB::keynames::PRESET_2;
    m_keyIdToKeyNameMap[std::make_pair( KeyOrigin_t::KEY_ORIGIN_RF, LpmServiceMessages::BOSE_NUMBER_3 )]         = KeyNamesPB::keynames::PRESET_3;
    m_keyIdToKeyNameMap[std::make_pair( KeyOrigin_t::KEY_ORIGIN_RF, LpmServiceMessages::BOSE_NUMBER_4 )]         = KeyNamesPB::keynames::PRESET_4;
    m_keyIdToKeyNameMap[std::make_pair( KeyOrigin_t::KEY_ORIGIN_RF, LpmServiceMessages::BOSE_NUMBER_5 )]         = KeyNamesPB::keynames::PRESET_5;
    m_keyIdToKeyNameMap[std::make_pair( KeyOrigin_t::KEY_ORIGIN_RF, LpmServiceMessages::BOSE_NUMBER_6 )]         = KeyNamesPB::keynames::PRESET_6;
    m_keyIdToKeyNameMap[std::make_pair( KeyOrigin_t::KEY_ORIGIN_RF, LpmServiceMessages::BOSE_MUTE )]             = KeyNamesPB::keynames::MUTE;
    m_keyIdToKeyNameMap[std::make_pair( KeyOrigin_t::KEY_ORIGIN_RF, LpmServiceMessages::BOSE_BLUETOOTH_SOURCE )] = KeyNamesPB::keynames::BLUETOOTH;

    return;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   CustomProductController::End
///
/// @brief  This method is called when the Product Controller process should be terminated. It is
///         used to set the running member to false, which will invoke the Wait method idle loop to
///         exit and perform any necessary clean up.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductController::End( )
{
    BOSE_DEBUG( s_logger, "The Product Controller main task is stopping." );

    m_Running = false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name CustomProductController::RegisterOpticalAutowakeForLpmConnection
///
/// @brief This method registers callback with LPM connection. When LPM is connected it sends the
///        autowake status read from NV to LPM
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductController::RegisterOpticalAutowakeForLpmConnection( )
{
    auto lpmFunc = [ this ]( bool connected )
    {
        if( connected )
        {
            m_ProductLpmHardwareInterface->SendAutowakeStatus( m_IsAutoWakeEnabled );
        }
    };
    m_ProductLpmHardwareInterface->RegisterForLpmConnection( lpmFunc );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name CustomProductController::InitializeAccessorySoftwareInstallManager
///
/// @brief Initialize the AccessorySoftwareInstallManager
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductController::InitializeAccessorySoftwareInstallManager( )
{
    auto softwareInstallFunc = [this]( void )
    {
        ProductMessage productMessage;
        productMessage.set_softwareinstall( false );
        GetProductSoftwareInstallManager().SetSwUpdateForeground( true );
        SendAsynchronousProductMessage( productMessage );
    };
    auto softwareInstallcb = std::make_shared<AsyncCallback<void> > ( softwareInstallFunc, GetTask() );

    m_AccessorySoftwareInstallManager.Initialize( softwareInstallcb, GetLpmHardwareInterface( ) );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   CustomProductController::IsProductControlSurface
///
/// @brief  This method is called to determine whether the given key origin is a product control
///         surface.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductController::IsProductControlSurface( LpmServiceMessages::KeyOrigin_t keyOrigin ) const
{
    switch( keyOrigin )
    {
    case LpmServiceMessages::KEY_ORIGIN_CONSOLE_BUTTON:
    case LpmServiceMessages::KEY_ORIGIN_CAPSENSE:
    case LpmServiceMessages::KEY_ORIGIN_IR:
    case LpmServiceMessages::KEY_ORIGIN_RF:
    case LpmServiceMessages::KEY_ORIGIN_TAP:
        return true;

    default:
        return false;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   CustomProductController::InitiateSoftwareInstall
///
/// @brief  This method is called to start the actual update installation
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductController::InitiateSoftwareInstall( )
{
    BOSE_INFO( s_logger, "%s::%s: ProductSoftware is %spending and %sallowed; AccessorySoftware is %spending", CLASS_NAME, __func__,
               GetProductSoftwareInstallManager( ).IsSoftwareUpdatePending( ) ? "" : "not ",
               GetProductSoftwareInstallManager( ).IsSoftwareUpdateAllowed( ) ? "" : "not ",
               m_AccessorySoftwareInstallManager.IsSoftwareUpdatePending( ) ? "" : "not " );

    if( GetProductSoftwareInstallManager( ).IsSoftwareUpdatePending( ) && GetProductSoftwareInstallManager( ).IsSoftwareUpdateAllowed( ) )
    {
        GetProductSoftwareInstallManager( ).InitiateSoftwareInstall( );
    }
    else if( m_AccessorySoftwareInstallManager.IsSoftwareUpdatePending( ) )
    {
        m_AccessorySoftwareInstallManager.InitiateSoftwareInstall( );
    }
    else
    {
        // Let BOSE_DIE reboot the system, we are in a terminal state and there is nothing to install!
        BOSE_DIE( "CustomProductController::InitiateSoftwareInstall( ) cannot initiate any update, we should not have gotten here!" );
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   CustomProductController::IsSwUpdateForeground
///
/// @brief  This method is called to determine whether update installation should be in foreground
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductController::IsSwUpdateForeground( ) const
{
    BOSE_INFO( s_logger, "%s::%s: ProductSoftware is %spending and %sallowed and %sforeground", CLASS_NAME, __func__,
               GetProductSoftwareInstallManager( ).IsSoftwareUpdatePending( ) ? "" : "not ",
               GetProductSoftwareInstallManager( ).IsSoftwareUpdateAllowed( ) ? "" : "not ",
               GetProductSoftwareInstallManager( ).IsSwUpdateForeground( ) ? "" : "not " );

    if( GetProductSoftwareInstallManager( ).IsSoftwareUpdatePending( ) && GetProductSoftwareInstallManager( ).IsSoftwareUpdateAllowed( ) )
    {
        // We are en-route to Product Software Install, let it control
        return GetProductSoftwareInstallManager( ).IsSwUpdateForeground( );
    }

    // We are likely en-route to accessory update, it's always in background
    return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                           End of the Product Application Namespace                           ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
