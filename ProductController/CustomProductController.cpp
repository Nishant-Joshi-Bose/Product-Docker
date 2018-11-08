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
#include "ControlIntegrationSTSStateFactory.h"
#include "ControlIntegrationSTSStateTop.h"
#include "ControlIntegrationSTSStateTopSilent.h"
#include "ProductSTSStateTopAiQ.h"
#include "SystemSourcesProperties.pb.h"
#include "ProductControllerHsm.h"
#include "CustomProductControllerStates.h"
#include "CustomProductControllerState.h"
#include "ProductControllerStates.h"
#include "ProductControllerState.h"
#include "ProductControllerStateBooted.h"
#include "ProductControllerStateCriticalError.h"
#include "ProductControllerStateFactoryDefault.h"
#include "ProductControllerStateFirstBootGreeting.h"
#include "ProductControllerStateFirstBootGreetingTransition.h"
#include "ProductControllerStateIdleVoiceConfigured.h"
#include "ProductControllerStateIdleVoiceNotConfigured.h"
#include "ProductControllerStateLowPowerStandby.h"
#include "ProductControllerStateLowPowerStandbyTransition.h"
#include "ProductControllerStateNetworkStandbyConfigured.h"
#include "ProductControllerStateNetworkStandby.h"
#include "ProductControllerStateNetworkStandbyNotConfigured.h"
#include "ProductControllerStateOn.h"
#include "ProductControllerStatePlayable.h"
#include "ProductControllerStatePlayableTransition.h"
#include "ProductControllerStatePlayableTransitionIdle.h"
#include "ProductControllerStatePlayableTransitionInternal.h"
#include "ProductControllerStatePlayableTransitionNetworkStandby.h"
#include "ProductControllerStatePlayingDeselected.h"
#include "ProductControllerStatePlaying.h"
#include "ProductControllerStatePlayingSelected.h"
#include "ProductControllerStatePlayingSelectedNotSilent.h"
#include "ProductControllerStatePlayingSelectedSetupExiting.h"
#include "ProductControllerStatePlayingSelectedSetupExitingAP.h"
#include "ProductControllerStatePlayingSelectedSetup.h"
#include "ProductControllerStatePlayingSelectedSetupNetwork.h"
#include "ProductControllerStatePlayingSelectedSetupNetworkTransition.h"
#include "ProductControllerStatePlayingSelectedSetupOther.h"
#include "ProductControllerStatePlayingSelectedSilent.h"
#include "ProductControllerStatePlayingSelectedSilentSourceInvalid.h"
#include "ProductControllerStatePlayingSelectedSilentSourceValid.h"
#include "ProductControllerStatePlayingSelectedStoppingStreams.h"
#include "ProductControllerStatePlayingTransition.h"
#include "ProductControllerStatePlayingTransitionSwitch.h"
#include "ProductControllerStateSoftwareInstall.h"
#include "ProductControllerStateSoftwareUpdateTransition.h"
#include "ProductControllerStateStoppingStreamsDedicatedForFactoryDefault.h"
#include "ProductControllerStateStoppingStreamsDedicatedForSoftwareUpdate.h"
#include "ProductControllerStateStoppingStreamsDedicated.h"
#include "ProductControllerStateTop.h"
#include "CustomProductControllerStateAccessoryPairing.h"
#include "CustomProductControllerStateAccessoryPairingCancelling.h"
#include "CustomProductControllerStateAdaptIQCancelling.h"
#include "CustomProductControllerStateAdaptIQ.h"
#include "CustomProductControllerStateBooting.h"
#include "CustomProductControllerStateFirstBootGreetingTransition.h"
#include "CustomProductControllerStateIdle.h"
#include "CustomProductControllerStateLowPowerResume.h"
#include "CustomProductControllerStateOn.h"
#include "CustomProductControllerStatePlayable.h"
#include "CustomProductControllerStatePlaying.h"
#include "CustomProductControllerStatePlayingDeselected.h"
#include "CustomProductControllerStatePlayingSelected.h"
#include "CustomProductControllerStatePlayingSelectedSetup.h"
#include "CustomProductControllerStatePlayingSelectedSilentSourceInvalid.h"
#include "MfgData.h"
#include "DeviceManager.pb.h"
#include "ProductBLERemoteManager.h"
#include "ProductEndpointDefines.h"
#include "ProtoPersistenceFactory.h"
#include "PGCErrorCodes.h"
#include "SystemUtils.h"
#include "CustomChimeEvents.h"
#include "LpmClientLiteIF.h"

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
constexpr auto      g_DefaultCAPSValuesStateFile  = "DefaultCAPSValuesDone";
}

constexpr char     UI_KILL_PID_FILE[] = "/var/run/monaco.pid";
constexpr uint32_t UI_ALIVE_TIMEOUT = 60 * 1000;

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

    ///
    /// Initialization of STS contorller.
    ///
    m_ControlIntegrationSTSController( ),

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
    /// Construction of the Common and Custom States
    ///

    ///
    /// Top State
    ///
    auto* stateTop = new ProductControllerStateTop( GetHsm( ),
                                                    nullptr );
    ///
    /// Booting State and Various System Level States
    ///
    auto* stateBooting = new CustomProductControllerStateBooting
    ( GetHsm( ),
      stateTop,
      CUSTOM_PRODUCT_CONTROLLER_STATE_BOOTING );

    auto* stateBooted = new ProductControllerStateBooted
    ( GetHsm( ),
      stateTop,
      PRODUCT_CONTROLLER_STATE_BOOTED );

    auto* stateFirstBootGreeting = new ProductControllerStateFirstBootGreeting
    ( GetHsm( ),
      stateTop,
      PRODUCT_CONTROLLER_STATE_FIRST_BOOT_GREETING );

    auto* stateFirstBootGreetingTransition = new CustomProductControllerStateFirstBootGreetingTransition
    ( GetHsm( ),
      stateTop,
      CUSTOM_PRODUCT_CONTROLLER_STATE_FIRST_BOOT_GREETING_TRANSITION );

    auto* stateSoftwareUpdateTransition = new ProductControllerStateSoftwareUpdateTransition
    ( GetHsm( ),
      stateTop,
      PRODUCT_CONTROLLER_STATE_SOFTWARE_UPDATE_TRANSITION );

    auto* stateSoftwareInstall = new ProductControllerStateSoftwareInstall
    ( GetHsm( ),
      stateTop,
      PRODUCT_CONTROLLER_STATE_SOFTWARE_INSTALL );

    auto* stateCriticalError = new ProductControllerStateCriticalError
    ( GetHsm( ),
      stateTop,
      PRODUCT_CONTROLLER_STATE_CRITICAL_ERROR );

    auto* stateFactoryDefault = new ProductControllerStateFactoryDefault
    ( GetHsm( ),
      stateTop,
      PRODUCT_CONTROLLER_STATE_FACTORY_DEFAULT );

    auto* stateLowPowerStandbyTransition = new ProductControllerStateLowPowerStandbyTransition
    ( GetHsm( ),
      stateTop,
      PRODUCT_CONTROLLER_STATE_LOW_POWER_STANDBY_TRANSITION );

    auto* stateLowPowerStandby = new ProductControllerStateLowPowerStandby
    ( GetHsm( ),
      stateTop,
      PRODUCT_CONTROLLER_STATE_LOW_POWER_STANDBY );

    auto* stateLowPowerResume = new CustomProductControllerStateLowPowerResume
    ( GetHsm( ),
      stateTop,
      CUSTOM_PRODUCT_CONTROLLER_STATE_LOW_POWER_RESUME );

    ///
    /// Playable Transition State and Sub-States
    ///
    auto* statePlayableTransition = new ProductControllerStatePlayableTransition
    ( GetHsm( ),
      stateTop,
      PRODUCT_CONTROLLER_STATE_PLAYABLE_TRANSITION );

    auto* statePlayableTransitionInternal = new ProductControllerStatePlayableTransitionInternal
    ( GetHsm( ),
      statePlayableTransition,
      PRODUCT_CONTROLLER_STATE_PLAYABLE_TRANSITION_INTERNAL );

    auto* statePlayableTransitionIdle = new ProductControllerStatePlayableTransitionIdle
    ( GetHsm( ),
      statePlayableTransitionInternal,
      PRODUCT_CONTROLLER_STATE_PLAYABLE_TRANSITION_IDLE );

    auto* statePlayableTransitionNetworkStandby = new ProductControllerStatePlayableTransitionNetworkStandby
    ( GetHsm( ),
      statePlayableTransitionInternal,
      PRODUCT_CONTROLLER_STATE_PLAYABLE_TRANSITION_NETWORK_STANDBY );

    ///
    /// Top On State
    ///
    auto* stateOn = new CustomProductControllerStateOn
    ( GetHsm( ),
      stateTop,
      CUSTOM_PRODUCT_CONTROLLER_STATE_ON );

    ///
    /// Playable State and Sub-States
    ///
    auto* statePlayable = new CustomProductControllerStatePlayable
    ( GetHsm( ),
      stateOn,
      CUSTOM_PRODUCT_CONTROLLER_STATE_PLAYABLE );

    auto* stateNetworkStandby = new ProductControllerStateNetworkStandby
    ( GetHsm( ),
      statePlayable,
      PRODUCT_CONTROLLER_STATE_NETWORK_STANDBY );

    auto* stateNetworkStandbyConfigured = new ProductControllerStateNetworkStandbyConfigured
    ( GetHsm( ),
      stateNetworkStandby,
      PRODUCT_CONTROLLER_STATE_NETWORK_STANDBY_CONFIGURED );

    auto* stateNetworkStandbyNotConfigured = new ProductControllerStateNetworkStandbyNotConfigured
    ( GetHsm( ),
      stateNetworkStandby,
      PRODUCT_CONTROLLER_STATE_NETWORK_STANDBY_NOT_CONFIGURED );

    auto* stateIdle = new CustomProductControllerStateIdle
    ( GetHsm( ),
      statePlayable,
      CUSTOM_PRODUCT_CONTROLLER_STATE_IDLE );

    auto* stateIdleVoiceConfigured = new ProductControllerStateIdleVoiceConfigured
    ( GetHsm( ),
      stateIdle,
      PRODUCT_CONTROLLER_STATE_IDLE_VOICE_CONFIGURED );

    auto* stateIdleVoiceNotConfigured = new ProductControllerStateIdleVoiceNotConfigured
    ( GetHsm( ),
      stateIdle,
      PRODUCT_CONTROLLER_STATE_IDLE_VOICE_NOT_CONFIGURED );

    ///
    /// Playing Transition State and Sub-States
    ///
    auto* statePlayingTransition = new ProductControllerStatePlayingTransition
    ( GetHsm( ),
      stateTop,
      PRODUCT_CONTROLLER_STATE_PLAYING_TRANSITION );

    auto* statePlayingTransitionSelected = new ProductControllerStatePlayingTransitionSwitch
    ( GetHsm( ),
      statePlayingTransition,
      PRODUCT_CONTROLLER_STATE_PLAYING_TRANSITION_SWITCH );

    ///
    /// Playing State and Sub-States
    ///
    auto* statePlaying = new CustomProductControllerStatePlaying
    ( GetHsm( ),
      stateOn,
      CUSTOM_PRODUCT_CONTROLLER_STATE_PLAYING );

    auto* statePlayingDeselected = new CustomProductControllerStatePlayingDeselected
    ( GetHsm( ),
      statePlaying,
      CUSTOM_PRODUCT_CONTROLLER_STATE_PLAYING_DESELECTED );

    auto* statePlayingSelected = new CustomProductControllerStatePlayingSelected
    ( GetHsm( ),
      statePlaying,
      CUSTOM_PRODUCT_CONTROLLER_STATE_PLAYING_SELECTED );

    auto* statePlayingSelectedSilent = new ProductControllerStatePlayingSelectedSilent
    ( GetHsm( ),
      statePlayingSelected,
      PRODUCT_CONTROLLER_STATE_PLAYING_SELECTED_SILENT );

    auto* statePlayingSelectedSilentSourceInvalid = new CustomProductControllerStatePlayingSelectedSilentSourceInvalid
    ( GetHsm( ),
      statePlayingSelectedSilent,
      CUSTOM_PRODUCT_CONTROLLER_STATE_PLAYING_SELECTED_SILENT_SOURCE_INVALID );

    auto* statePlayingSelectedSilentSourceValid = new ProductControllerStatePlayingSelectedSilentSourceValid
    ( GetHsm( ),
      statePlayingSelectedSilent,
      PRODUCT_CONTROLLER_STATE_PLAYING_SELECTED_SILENT_SOURCE_VALID );

    auto* statePlayingSelectedNotSilent = new ProductControllerStatePlayingSelectedNotSilent
    ( GetHsm( ),
      statePlayingSelected,
      PRODUCT_CONTROLLER_STATE_PLAYING_SELECTED_NOT_SILENT );

    auto* statePlayingSelectedSetup = new CustomProductControllerStatePlayingSelectedSetup
    ( GetHsm( ),
      statePlayingSelected,
      CUSTOM_PRODUCT_CONTROLLER_STATE_PLAYING_SELECTED_SETUP );

    auto* statePlayingSelectedSetupNetwork = new ProductControllerStatePlayingSelectedSetupNetwork
    ( GetHsm( ),
      statePlayingSelectedSetup,
      PRODUCT_CONTROLLER_STATE_PLAYING_SELECTED_SETUP_NETWORK );

    auto* statePlayingSelectedSetupNetworkTransition = new ProductControllerStatePlayingSelectedSetupNetworkTransition
    ( GetHsm( ),
      statePlayingSelectedSetup,
      PRODUCT_CONTROLLER_STATE_PLAYING_SELECTED_SETUP_NETWORK_TRANSITION );

    auto* statePlayingSelectedSetupOther = new ProductControllerStatePlayingSelectedSetupOther
    ( GetHsm( ),
      statePlayingSelectedSetup,
      PRODUCT_CONTROLLER_STATE_PLAYING_SELECTED_SETUP_OTHER );

    auto* statePlayingSelectedSetupExiting = new ProductControllerStatePlayingSelectedSetupExiting
    ( GetHsm( ),
      statePlayingSelectedSetup,
      PRODUCT_CONTROLLER_STATE_PLAYING_SELECTED_SETUP_EXITING );

    auto* statePlayingSelectedSetupExitingAP = new ProductControllerStatePlayingSelectedSetupExitingAP
    ( GetHsm( ),
      statePlayingSelectedSetup,
      PRODUCT_CONTROLLER_STATE_PLAYING_SELECTED_SETUP_EXITING_AP );

    auto* stateStoppingStreams = new ProductControllerStatePlayingSelectedStoppingStreams
    ( GetHsm( ),
      statePlayingSelected,
      PRODUCT_CONTROLLER_STATE_PLAYING_SELECTED_STOPPING_STREAMS );

    ///
    /// Accessory Pairing States
    ///
    auto* stateAccessoryPairing = new CustomProductControllerStateAccessoryPairing
    ( GetHsm( ),
      statePlayingSelected,
      CUSTOM_PRODUCT_CONTROLLER_STATE_ACCESSORY_PAIRING );

    auto* stateAccessoryPairingCancelling = new CustomProductControllerStateAccessoryPairingCancelling
    ( GetHsm( ),
      statePlayingSelected,
      CUSTOM_PRODUCT_CONTROLLER_STATE_ACCESSORY_PAIRING_CANCELLING );

    ///
    /// AdaptIQ States
    ///
    auto* stateAdaptIQ = new CustomProductControllerStateAdaptIQ
    ( GetHsm( ),
      statePlayingSelected,
      *this,
      CUSTOM_PRODUCT_CONTROLLER_STATE_ADAPTIQ );

    auto* stateAdaptIQCancelling = new CustomProductControllerStateAdaptIQCancelling
    ( GetHsm( ),
      statePlayingSelected,
      CUSTOM_PRODUCT_CONTROLLER_STATE_ADAPTIQ_CANCELLING );

    ///
    /// Stopping Dedicated Streams State and Sub-States
    ///
    auto* stateStoppingStreamsDedicated = new ProductControllerStateStoppingStreamsDedicated
    ( GetHsm( ),
      stateTop,
      PRODUCT_CONTROLLER_STATE_STOPPING_STREAMS_DEDICATED );

    auto* stateStoppingStreamsDedicatedForFactoryDefault = new ProductControllerStateStoppingStreamsDedicatedForFactoryDefault
    ( GetHsm( ),
      stateStoppingStreamsDedicated,
      PRODUCT_CONTROLLER_STATE_STOPPING_STREAMS_DEDICATED_FOR_FACTORY_DEFAULT );

    auto* stateStoppingStreamsDedicatedForSoftwareUpdate = new ProductControllerStateStoppingStreamsDedicatedForSoftwareUpdate
    ( GetHsm( ),
      stateStoppingStreamsDedicated,
      PRODUCT_CONTROLLER_STATE_STOPPING_STREAMS_DEDICATED_FOR_SOFTWARE_UPDATE );

    ///
    /// The states are added to the state machine and the state machine is initialized.
    ///
    using namespace DeviceManagerPb;
    using namespace SystemPowerPb;

    GetHsm( ).AddState( Device_State_Not_Notify,
                        SystemPowerControl_State_Not_Notify,
                        stateTop );

    GetHsm( ).AddState( NotifiedNames::BOOTING,
                        SystemPowerControl_State_Not_Notify,
                        stateBooting );

    GetHsm( ).AddState( NotifiedNames::FIRST_BOOT_GREETING,
                        SystemPowerControl_State_Not_Notify,
                        stateFirstBootGreeting );

    GetHsm( ).AddState( NotifiedNames::UPDATING,
                        SystemPowerControl_State_Not_Notify,
                        stateSoftwareUpdateTransition );

    GetHsm( ).AddState( NotifiedNames::UPDATING,
                        SystemPowerControl_State_Not_Notify,
                        stateSoftwareInstall );

    GetHsm( ).AddState( NotifiedNames::CRITICAL_ERROR,
                        SystemPowerControl_State_Not_Notify,
                        stateCriticalError );

    GetHsm( ).AddState( NotifiedNames::FACTORY_DEFAULT,
                        SystemPowerControl_State_Not_Notify,
                        stateFactoryDefault );

    GetHsm( ).AddState( Device_State_Not_Notify,
                        SystemPowerControl_State_Not_Notify,
                        stateBooted );

    GetHsm( ).AddState( Device_State_Not_Notify,
                        SystemPowerControl_State_Not_Notify,
                        stateFirstBootGreetingTransition );

    GetHsm( ).AddState( Device_State_Not_Notify,
                        SystemPowerControl_State_Not_Notify,
                        stateLowPowerStandbyTransition );

    GetHsm( ).AddState( Device_State_Not_Notify,
                        SystemPowerControl_State_OFF,
                        stateLowPowerStandby );

    GetHsm( ).AddState( Device_State_Not_Notify,
                        SystemPowerControl_State_Not_Notify,
                        stateLowPowerResume );

    GetHsm( ).AddState( Device_State_Not_Notify,
                        SystemPowerControl_State_OFF,
                        statePlayableTransition );

    GetHsm( ).AddState( Device_State_Not_Notify,
                        SystemPowerControl_State_Not_Notify,
                        statePlayableTransitionInternal );

    GetHsm( ).AddState( Device_State_Not_Notify,
                        SystemPowerControl_State_OFF,
                        statePlayableTransitionIdle );

    GetHsm( ).AddState( Device_State_Not_Notify,
                        SystemPowerControl_State_OFF,
                        statePlayableTransitionNetworkStandby );

    GetHsm( ).AddState( Device_State_Not_Notify,
                        SystemPowerControl_State_Not_Notify,
                        stateOn );

    GetHsm( ).AddState( Device_State_Not_Notify,
                        SystemPowerControl_State_OFF,
                        statePlayable );

    GetHsm( ).AddState( NotifiedNames::NETWORK_STANDBY,
                        SystemPowerControl_State_OFF,
                        stateNetworkStandby );

    GetHsm( ).AddState( NotifiedNames::NETWORK_STANDBY,
                        SystemPowerControl_State_OFF,
                        stateNetworkStandbyConfigured );

    GetHsm( ).AddState( NotifiedNames::NETWORK_STANDBY,
                        SystemPowerControl_State_OFF,
                        stateNetworkStandbyNotConfigured );

    GetHsm( ).AddState( NotifiedNames::IDLE,
                        SystemPowerControl_State_OFF,
                        stateIdle );

    GetHsm( ).AddState( NotifiedNames::IDLE,
                        SystemPowerControl_State_OFF,
                        stateIdleVoiceConfigured );

    GetHsm( ).AddState( NotifiedNames::IDLE,
                        SystemPowerControl_State_OFF,
                        stateIdleVoiceNotConfigured );

    GetHsm( ).AddState( Device_State_Not_Notify,
                        SystemPowerControl_State_Not_Notify,
                        statePlayingTransition );

    GetHsm( ).AddState( Device_State_Not_Notify,
                        SystemPowerControl_State_Not_Notify,
                        statePlayingTransitionSelected );

    GetHsm( ).AddState( Device_State_Not_Notify,
                        SystemPowerControl_State_Not_Notify,
                        statePlaying );

    GetHsm( ).AddState( NotifiedNames::PLAYING_SOURCE_OFF,
                        SystemPowerControl_State_OFF,
                        statePlayingDeselected );

    GetHsm( ).AddState( NotifiedNames::SELECTED,
                        SystemPowerControl_State_Not_Notify,
                        statePlayingSelected );

    GetHsm( ).AddState( NotifiedNames::SELECTED,
                        SystemPowerControl_State_Not_Notify,
                        statePlayingSelectedSilent );

    GetHsm( ).AddState( NotifiedNames::PLAYING_SOURCE_OFF,
                        SystemPowerControl_State_OFF,
                        statePlayingSelectedSilentSourceInvalid );

    GetHsm( ).AddState( NotifiedNames::SELECTED,
                        SystemPowerControl_State_ON,
                        statePlayingSelectedSilentSourceValid );

    GetHsm( ).AddState( NotifiedNames::SELECTED,
                        SystemPowerControl_State_ON,
                        statePlayingSelectedNotSilent );

    GetHsm( ).AddState( NotifiedNames::SELECTED,
                        SystemPowerControl_State_ON,
                        statePlayingSelectedSetup );

    GetHsm( ).AddState( NotifiedNames::SELECTED,
                        SystemPowerControl_State_ON,
                        statePlayingSelectedSetupNetwork );

    GetHsm( ).AddState( NotifiedNames::SELECTED,
                        SystemPowerControl_State_Not_Notify,
                        statePlayingSelectedSetupNetworkTransition );

    GetHsm( ).AddState( NotifiedNames::SELECTED,
                        SystemPowerControl_State_ON,
                        statePlayingSelectedSetupOther );

    GetHsm( ).AddState( NotifiedNames::SELECTED,
                        SystemPowerControl_State_Not_Notify,
                        statePlayingSelectedSetupExiting );

    GetHsm( ).AddState( NotifiedNames::SELECTED,
                        SystemPowerControl_State_Not_Notify,
                        statePlayingSelectedSetupExitingAP );

    GetHsm( ).AddState( Device_State_Not_Notify,
                        SystemPowerControl_State_OFF,
                        stateStoppingStreams );

    GetHsm( ).AddState( Device_State_Not_Notify,
                        SystemPowerControl_State_ON,
                        stateAccessoryPairing );

    GetHsm( ).AddState( Device_State_Not_Notify,
                        SystemPowerControl_State_Not_Notify,
                        stateAccessoryPairingCancelling );

    GetHsm( ).AddState( Device_State_Not_Notify,
                        SystemPowerControl_State_ON,
                        stateAdaptIQ );

    GetHsm( ).AddState( Device_State_Not_Notify,
                        SystemPowerControl_State_Not_Notify,
                        stateAdaptIQCancelling );

    GetHsm( ).AddState( Device_State_Not_Notify,
                        SystemPowerControl_State_Not_Notify,
                        stateStoppingStreamsDedicated );

    GetHsm( ).AddState( Device_State_Not_Notify,
                        SystemPowerControl_State_Not_Notify,
                        stateStoppingStreamsDedicatedForFactoryDefault );

    GetHsm( ).AddState( Device_State_Not_Notify,
                        SystemPowerControl_State_Not_Notify,
                        stateStoppingStreamsDedicatedForSoftwareUpdate );

    GetHsm( ).Init( this, PRODUCT_CONTROLLER_STATE_BOOTING );

    ///
    /// Initialize entities in the Common Product Controller
    ///
    CommonInitialize( );

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
    m_ProductKeyInputManager     ->Run( );
    m_ProductFrontDoorKeyInjectIF->Run( );
    m_ProductCecHelper           ->Run( );
    m_ProductDspHelper           ->Run( );
    m_ProductAdaptIQManager      ->Run( );
    m_ProductBLERemoteManager    ->Run( );

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
///
/// @note   The CLI command "product boot_status" returns the status of all factors used here. If ever
///         a factor is added, the CLI command needs changing as well. See ProductCommandLine::HandleCommand().
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductController::IsBooted( ) const
{
    BOSE_VERBOSE( s_logger, "------------ Product Controller Booted Check ---------------" );
    BOSE_VERBOSE( s_logger, " " );
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
///         NOTE: Unlike booting we only wait for the things killed going to low power
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductController::IsLowPowerExited( ) const
{
    BOSE_INFO( s_logger, "------------ Product Controller Low Power Exit Check ---------------" );
    BOSE_INFO( s_logger, " " );
    BOSE_INFO( s_logger, "LPM Connected         :  %s", ( IsLpmReady()       ? "true" : "false" ) );
    BOSE_INFO( s_logger, "Audio Path Connected  :  %s", ( IsAudioPathReady() ? "true" : "false" ) );
    BOSE_INFO( s_logger, "SASS            Init  :  %s", ( IsSassReady()      ? "true" : "false" ) );
    BOSE_INFO( s_logger, " " );

    return( IsLpmReady()            and
            IsSassReady()           and
            IsAudioPathReady() );
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
/// @brief  This method is called to perform the needed initialization of the ControlIntegrationSTSController,
///         specifically, provide the set of sources to be created initially.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductController::SetupProductSTSController( )
{
    using namespace ProductSTS;

    std::vector< ControlIntegrationSTSController::SourceDescriptor > sources;

    ControlIntegrationSTSStateFactory<ControlIntegrationSTSStateTop>          commonStateFactory;
    ControlIntegrationSTSStateFactory<ControlIntegrationSTSStateTopSilent>    silentStateFactory;
    ControlIntegrationSTSStateFactory<ProductSTSStateTopAiQ>                  aiqStateFactory;

    ///
    /// ADAPTIQ, SETUP, and PAIRING are never available as a normal source, whereas the TV source
    /// will always be available. SLOT sources need to be set-up before they become available.
    ///
    ControlIntegrationSTSController::SourceDescriptor descriptor_Setup   { SETUP,   SetupSourceSlot_Name( SETUP ),   false, silentStateFactory };
    ControlIntegrationSTSController::SourceDescriptor descriptor_TV      { TV,      ProductSourceSlot_Name( TV ),      true,  commonStateFactory };
    ControlIntegrationSTSController::SourceDescriptor descriptor_AiQ     { ADAPTIQ, SetupSourceSlot_Name( ADAPTIQ ), false, aiqStateFactory    };
    ControlIntegrationSTSController::SourceDescriptor descriptor_Pairing { PAIRING, SetupSourceSlot_Name( PAIRING ), false, silentStateFactory };
    ControlIntegrationSTSController::SourceDescriptor descriptor_SLOT_0  { SLOT_0,  ProductSourceSlot_Name( SLOT_0 ),  false, commonStateFactory, true };
    ControlIntegrationSTSController::SourceDescriptor descriptor_SLOT_1  { SLOT_1,  ProductSourceSlot_Name( SLOT_1 ),  false, commonStateFactory, true };
    ControlIntegrationSTSController::SourceDescriptor descriptor_SLOT_2  { SLOT_2,  ProductSourceSlot_Name( SLOT_2 ),  false, commonStateFactory, true };

    sources.push_back( descriptor_Setup );
    sources.push_back( descriptor_TV );
    sources.push_back( descriptor_AiQ );
    sources.push_back( descriptor_Pairing );
    sources.push_back( descriptor_SLOT_0 );
    sources.push_back( descriptor_SLOT_1 );
    sources.push_back( descriptor_SLOT_2 );

    Callback< void >
    CallbackForSTSComplete( std::bind( &ProductController::HandleSTSInitWasComplete,
                                       this ) );


    Callback< ControlIntegrationSTSAccount::ProductSourceSlot >
    CallbackToHandleSelectSourceSlot( std::bind( &CustomProductController::HandleSelectSourceSlot,
                                                 this,
                                                 std::placeholders::_1 ) );

    m_ControlIntegrationSTSController.Initialize( sources,
                                                  CallbackForSTSComplete,
                                                  CallbackToHandleSelectSourceSlot );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   CustomProductController::HandleSelectSourceSlot
///
/// @brief  This method is called from the ControlIntegrationSTSController, when one of our sources is
///         activated by CAPS via STS.
///
/// @note   This method is called on the ControlIntegrationSTSController task.
///
/// @param  ControlIntegrationSTSAccount::ProductSourceSlot sourceSlot This identifies the activated slot.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductController::HandleSelectSourceSlot( ControlIntegrationSTSAccount::ProductSourceSlot sourceSlot )
{
    ProductMessage message;
    message.mutable_selectsourceslot( )->set_slot( static_cast< ProductSTS::ProductSourceSlot >( sourceSlot ) );

    IL::BreakThread( std::bind( GetMessageHandler( ),
                                message ),
                     GetTask( ) );
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
                                     std::placeholders::_3 ) ,
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
NetManager::Protobuf::OperationalMode CustomProductController::GetWiFiOperationalMode( )
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
        if( message.networkstatus().has_wifiapstate() )
        {
            GetLpmHardwareInterface( )->SetAmp( true, message.networkstatus( ).wifiapstate( ) );
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
        ///
        /// The following attempts to handle the key action using a common intent
        /// manager.
        ///
        if( HandleCommonIntents( message.action() ) )
        {
            BOSE_VERBOSE( s_logger, "Action key %u handled by common intent handler", message.action() );
        }
        ///
        /// The following determines whether the key action is to be handled by the custom intent
        /// manager.
        ///
        else if( GetIntentHandler( ).IsIntentMuteControl( message.action( ) ) )
        {
            GetHsm( ).Handle< KeyHandlerUtil::ActionType_t >( &CustomProductControllerState::HandleIntentMuteControl,
                                                              message.action( ) );
        }
        else if( GetIntentHandler( ).IsIntentSpeakerPairing( message.action( ) ) )
        {
            GetHsm( ).Handle< KeyHandlerUtil::ActionType_t >( &CustomProductControllerState::HandleIntentSpeakerPairing,
                                                              message.action( ) );
        }
        else if( GetIntentHandler( ).IsIntentPlayProductSource( message.action( ) ) )
        {
            GetHsm( ).Handle< KeyHandlerUtil::ActionType_t >( &CustomProductControllerState::HandleIntentPlayProductSource,
                                                              message.action( ) );
        }
        else if( GetIntentHandler( ).IsIntentRating( message.action( ) ) )
        {
            GetHsm( ).Handle< KeyHandlerUtil::ActionType_t >( &CustomProductControllerState::HandleIntentRating,
                                                              message.action( ) );
        }
        else if( GetIntentHandler( ).IsIntentPlaySoundTouchSource( message.action( ) ) )
        {
            GetHsm( ).Handle<>( &CustomProductControllerState::HandleIntentPlaySoundTouchSource );
        }
        else if( GetIntentHandler( ).IsIntentSetupBLERemote( message.action( ) ) )
        {
            GetHsm( ).Handle<>( &CustomProductControllerState::HandleIntentSetupBLERemote );
        }
        else
        {
            BOSE_ERROR( s_logger, "An action key %u was received that has no associated intent.", message.action( ) );

            GetHsm( ).Handle< KeyHandlerUtil::ActionType_t >( &CustomProductControllerState::HandleIntent,
                                                              message.action( ) );
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
        BOSE_INFO( s_logger, "DefaultCAPSValuesStateFile didn't exist, sent %s", desiredVolume.DebugString( ).c_str( ) );

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
        BOSE_INFO( s_logger, "DefaultCAPSValuesStateFile didn't exist, sent %s", message.DebugString( ).c_str( ) );
    }

    // Do the Common stuff last, the PUT above must come first
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

        IL::BreakThread( std::bind( GetMessageHandler( ),
                                    message ),
                         GetTask( ) );
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
    error.set_code( PGCErrorCodes::ERROR_CODE_PRODUCT_CONTROLLER_CUSTOM );
    error.set_subcode( PGCErrorCodes::ERROR_SUBCODE_POWER_MACRO );

    bool success = true;

    if( req.enabled() )
    {
        if( req.powerontv() )
        {
            const auto tvSource = GetSourceInfo( ).FindSource( SHELBY_SOURCE::PRODUCT, ProductSTS::ProductSourceSlot_Name( ProductSTS::TV ) );
            if( not( tvSource and tvSource->has_details( ) and tvSource->details().has_cicode() ) )
            {
                error.set_message( "TV is not configured but power on tv requested!" );
                success = false;
            }
        }
        if( success and req.has_powerondevice() )
        {
            const auto reqSource = GetSourceInfo( ).FindSource( SHELBY_SOURCE::PRODUCT, ProductSTS::ProductSourceSlot_Name( req.powerondevice() ) );

            if( not( reqSource and reqSource->has_details( ) and reqSource->details().has_cicode() ) )
            {
                error.set_message( "Requested source is not configured or available!" );
                success = false;
            }
        }
    }
    // else no op as we wont act on it if enabled == false

    if( success )
    {
        m_powerMacro.CopyFrom( req );
        auto persistence = ProtoPersistenceFactory::Create( "PowerMacro.json", GetProductPersistenceDir( ) );
        try
        {
            persistence->Store( ProtoToMarkup::ToJson( m_powerMacro ) );
            respCb( req );

            GetFrontDoorClient( )->SendNotification( FRONTDOOR_SYSTEM_POWER_MACRO_API,
                                                     m_powerMacro );

        }
        catch( const ProtoToMarkup::MarkupError & e )
        {
            BOSE_ERROR( s_logger, "Power Macro store persistence markup error - %s", e.what( ) );
            error.set_message( e.what( ) );
            success = false;
        }
        catch( ProtoPersistenceIF::ProtoPersistenceException & e )
        {
            BOSE_ERROR( s_logger, "Power Macro store persistence error - %s", e.what( ) );
            error.set_message( e.what( ) );
            success = false;
        }
    }

    if( not success )
    {
        errorCb( error );
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
/// @brief SpeakerPairingManager::AccessoriesPlayTones
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
/// @brief CustomProductController::InitializeKeyIdToKeyNameMap
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductController::InitializeKeyIdToKeyNameMap()
{
    BOSE_INFO( s_logger, "CustomProductController::%s:", __func__ );

    // Professor team need to coordinate with the UI team to know which keys are of interest to them
    m_keyIdToKeyNameMap[std::make_pair( KeyOrigin_t::KEY_ORIGIN_IR, KeyNamesPB::keyid::VOLUME_UP_KEYID )]     = KeyNamesPB::keynames::VOLUME_UP;
    m_keyIdToKeyNameMap[std::make_pair( KeyOrigin_t::KEY_ORIGIN_IR, KeyNamesPB::keyid::VOLUME_DOWN_KEYID )]   = KeyNamesPB::keynames::VOLUME_DOWN;
    m_keyIdToKeyNameMap[std::make_pair( KeyOrigin_t::KEY_ORIGIN_IR, KeyNamesPB::keyid::PRESET_1_KEYID )]      = KeyNamesPB::keynames::PRESET_1;
    m_keyIdToKeyNameMap[std::make_pair( KeyOrigin_t::KEY_ORIGIN_IR, KeyNamesPB::keyid::PRESET_2_KEYID )]      = KeyNamesPB::keynames::PRESET_2;
    m_keyIdToKeyNameMap[std::make_pair( KeyOrigin_t::KEY_ORIGIN_IR, KeyNamesPB::keyid::PRESET_3_KEYID )]      = KeyNamesPB::keynames::PRESET_3;
    m_keyIdToKeyNameMap[std::make_pair( KeyOrigin_t::KEY_ORIGIN_IR, KeyNamesPB::keyid::PRESET_4_KEYID )]      = KeyNamesPB::keynames::PRESET_4;
    m_keyIdToKeyNameMap[std::make_pair( KeyOrigin_t::KEY_ORIGIN_IR, KeyNamesPB::keyid::PRESET_5_KEYID )]      = KeyNamesPB::keynames::PRESET_5;
    m_keyIdToKeyNameMap[std::make_pair( KeyOrigin_t::KEY_ORIGIN_IR, KeyNamesPB::keyid::PRESET_6_KEYID )]      = KeyNamesPB::keynames::PRESET_6;


    m_keyIdToKeyNameMap[std::make_pair( KeyOrigin_t::KEY_ORIGIN_RF, KeyNamesPB::keyid::VOLUME_UP_KEYID )]     = KeyNamesPB::keynames::VOLUME_UP;
    m_keyIdToKeyNameMap[std::make_pair( KeyOrigin_t::KEY_ORIGIN_RF, KeyNamesPB::keyid::VOLUME_DOWN_KEYID )]   = KeyNamesPB::keynames::VOLUME_DOWN;
    m_keyIdToKeyNameMap[std::make_pair( KeyOrigin_t::KEY_ORIGIN_RF, KeyNamesPB::keyid::PRESET_1_KEYID )]      = KeyNamesPB::keynames::PRESET_1;
    m_keyIdToKeyNameMap[std::make_pair( KeyOrigin_t::KEY_ORIGIN_RF, KeyNamesPB::keyid::PRESET_2_KEYID )]      = KeyNamesPB::keynames::PRESET_2;
    m_keyIdToKeyNameMap[std::make_pair( KeyOrigin_t::KEY_ORIGIN_RF, KeyNamesPB::keyid::PRESET_3_KEYID )]      = KeyNamesPB::keynames::PRESET_3;
    m_keyIdToKeyNameMap[std::make_pair( KeyOrigin_t::KEY_ORIGIN_RF, KeyNamesPB::keyid::PRESET_4_KEYID )]      = KeyNamesPB::keynames::PRESET_4;
    m_keyIdToKeyNameMap[std::make_pair( KeyOrigin_t::KEY_ORIGIN_RF, KeyNamesPB::keyid::PRESET_5_KEYID )]      = KeyNamesPB::keynames::PRESET_5;
    m_keyIdToKeyNameMap[std::make_pair( KeyOrigin_t::KEY_ORIGIN_RF, KeyNamesPB::keyid::PRESET_6_KEYID )]      = KeyNamesPB::keynames::PRESET_6;

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
        productMessage.set_softwareinstall( true );
        IL::BreakThread( std::bind( GetMessageHandler( ), productMessage ), GetTask( ) );
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
