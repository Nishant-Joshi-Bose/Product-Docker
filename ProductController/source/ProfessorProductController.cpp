////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file      ProfessorProductController.cpp
///
/// @brief     This file contains source code that implements the ProfessorProductController class
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
#include "ProfessorProductController.h"
#include "CustomProductLpmHardwareInterface.h"
#include "CustomProductAudioService.h"
#include "CustomAudioSettingsManager.h"
#include "CustomProductKeyInputManager.h"
#include "ProductCommandLine.h"
#include "ProductAdaptIQManager.h"
#include "IntentHandler.h"
#include "ProductSTS.pb.h"
#include "ProductSTSStateFactory.h"
#include "ProductSTSStateTop.h"
#include "ProductSTSStateTopSilent.h"
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
#include "ProductControllerStatePlayingSelectedStoppingStreams.h"
#include "ProductControllerStatePlayingTransition.h"
#include "ProductControllerStatePlayingTransitionSwitch.h"
#include "ProductControllerStateSoftwareInstall.h"
#include "ProductControllerStateSoftwareUpdateTransition.h"
#include "ProductControllerStateStoppingStreamsDedicatedForFactoryDefault.h"
#include "ProductControllerStateStoppingStreamsDedicatedForSoftwareUpdate.h"
#include "ProductControllerStateStoppingStreamsDedicated.h"
#include "ProductControllerStateTop.h"
#include "CustomProductControllerStateBooting.h"
#include "CustomProductControllerStateAccessoryPairing.h"
#include "CustomProductControllerStateAccessoryPairingCancelling.h"
#include "CustomProductControllerStateAdaptIQExiting.h"
#include "CustomProductControllerStateAdaptIQ.h"
#include "CustomProductControllerStateIdle.h"
#include "CustomProductControllerStateLowPowerResume.h"
#include "CustomProductControllerStateOn.h"
#include "CustomProductControllerStatePlaying.h"
#include "CustomProductControllerStatePlayingSelected.h"
#include "CustomProductControllerStatePlayingSelectedSetup.h"
#include "MfgData.h"
#include "DeviceManager.pb.h"
#include "ProductBLERemoteManager.h"
#include "ProductEndpointDefines.h"
#include "ProtoPersistenceFactory.h"
#include "PGCErrorCodes.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
///                          Start of the Product Application Namespace                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
namespace ProductApp
{

////////////////////////////////////////////////////////////////////////////////////////////////////
///
///            Constant Definitions
///
////////////////////////////////////////////////////////////////////////////////////////////////////
constexpr uint32_t PRODUCT_CONTROLLER_RUNNING_CHECK_IN_SECONDS = 4;

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProfessorProductController::ProfessorProductController
///
/// @brief  This method is the ProfessorProductController constructor, which is used to initialize
///         its corresponding module classes and member variables.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
ProfessorProductController::ProfessorProductController( ) :

    ///
    /// Construction of the Product Controller Modules
    ///
    m_ProductLpmHardwareInterface( nullptr ),
    m_ProductCommandLine( nullptr ),
    m_ProductKeyInputManager( nullptr ),
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
    m_ProductSTSController( *this ),

    ///
    /// Intent Handler Initialization
    ///
    m_IntentHandler( *GetTask(),
                     m_CliClientMT,
                     m_FrontDoorClientIF,
                     *this ),

    ///
    /// Intitialization for the Product Message Handler Reference
    ///
    m_ProductMessageHandler( static_cast< Callback < ProductMessage > >
                             ( std::bind( &ProfessorProductController::HandleMessage,
                                          this,
                                          std::placeholders::_1 ) ) )
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name ProfessorProductController::Start
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
void ProfessorProductController::Start( )
{
    m_Running = true;

    IL::BreakThread( std::bind( &ProfessorProductController::Run, this ), GetTask( ) );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name ProfessorProductController::Run
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProfessorProductController::Run( )
{
    BOSE_DEBUG( s_logger, "----------- Product Controller Initialization Start ------------" );
    BOSE_DEBUG( s_logger, "The Professor Product Controller is setting up the state machine." );

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

    auto* stateFirstBootGreetingTransition = new ProductControllerStateFirstBootGreetingTransition
    ( GetHsm( ),
      stateTop,
      PRODUCT_CONTROLLER_STATE_FIRST_BOOT_GREETING_TRANSITION );

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
    auto* statePlayable = new ProductControllerStatePlayable
    ( GetHsm( ),
      stateOn,
      PRODUCT_CONTROLLER_STATE_PLAYABLE );

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

    auto* statePlayingDeselected = new ProductControllerStatePlayingDeselected
    ( GetHsm( ),
      statePlaying,
      PRODUCT_CONTROLLER_STATE_PLAYING_DESELECTED );

    auto* statePlayingSelected = new CustomProductControllerStatePlayingSelected
    ( GetHsm( ),
      statePlaying,
      CUSTOM_PRODUCT_CONTROLLER_STATE_PLAYING_SELECTED );

    auto* statePlayingSelectedSilent = new ProductControllerStatePlayingSelectedSilent
    ( GetHsm( ),
      statePlayingSelected,
      PRODUCT_CONTROLLER_STATE_PLAYING_SELECTED_SILENT );

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

    auto* stateAdaptIQExiting = new CustomProductControllerStateAdaptIQExiting
    ( GetHsm( ),
      statePlayingSelected,
      CUSTOM_PRODUCT_CONTROLLER_STATE_ADAPTIQ_EXITING );

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

    GetHsm( ).AddState( "", stateTop );
    GetHsm( ).AddState( NotifiedNames_Name( NotifiedNames::BOOTING ), stateBooting );
    GetHsm( ).AddState( NotifiedNames_Name( NotifiedNames::FIRST_BOOT_GREETING ), stateFirstBootGreeting );
    GetHsm( ).AddState( NotifiedNames_Name( NotifiedNames::UPDATING ), stateSoftwareUpdateTransition );
    GetHsm( ).AddState( NotifiedNames_Name( NotifiedNames::UPDATING ), stateSoftwareInstall );
    GetHsm( ).AddState( NotifiedNames_Name( NotifiedNames::CRITICAL_ERROR ), stateCriticalError );
    GetHsm( ).AddState( NotifiedNames_Name( NotifiedNames::FACTORY_DEFAULT ), stateFactoryDefault );
    GetHsm( ).AddState( "", stateBooted );
    GetHsm( ).AddState( "", stateFirstBootGreetingTransition );
    GetHsm( ).AddState( "", stateLowPowerStandbyTransition );
    GetHsm( ).AddState( "", stateLowPowerStandby );
    GetHsm( ).AddState( "", stateLowPowerResume );
    GetHsm( ).AddState( "", statePlayableTransition );
    GetHsm( ).AddState( "", statePlayableTransitionInternal );
    GetHsm( ).AddState( "", statePlayableTransitionIdle );
    GetHsm( ).AddState( "", statePlayableTransitionNetworkStandby );
    GetHsm( ).AddState( "", stateOn );
    GetHsm( ).AddState( "", statePlayable );
    GetHsm( ).AddState( NotifiedNames_Name( NotifiedNames::NETWORK_STANDBY ), stateNetworkStandby );
    GetHsm( ).AddState( NotifiedNames_Name( NotifiedNames::NETWORK_STANDBY ), stateNetworkStandbyConfigured );
    GetHsm( ).AddState( NotifiedNames_Name( NotifiedNames::NETWORK_STANDBY ), stateNetworkStandbyNotConfigured );
    GetHsm( ).AddState( NotifiedNames_Name( NotifiedNames::IDLE ), stateIdle );
    GetHsm( ).AddState( NotifiedNames_Name( NotifiedNames::IDLE ), stateIdleVoiceConfigured );
    GetHsm( ).AddState( NotifiedNames_Name( NotifiedNames::IDLE ), stateIdleVoiceNotConfigured );
    GetHsm( ).AddState( "", statePlayingTransition );
    GetHsm( ).AddState( "", statePlayingTransitionSelected );
    GetHsm( ).AddState( "", statePlaying );
    GetHsm( ).AddState( NotifiedNames_Name( NotifiedNames::DESELECTED ), statePlayingDeselected );
    GetHsm( ).AddState( NotifiedNames_Name( NotifiedNames::SELECTED ),   statePlayingSelected );
    GetHsm( ).AddState( NotifiedNames_Name( NotifiedNames::SELECTED ),   statePlayingSelectedSilent );
    GetHsm( ).AddState( NotifiedNames_Name( NotifiedNames::SELECTED ),   statePlayingSelectedNotSilent );
    GetHsm( ).AddState( NotifiedNames_Name( NotifiedNames::SELECTED ),   statePlayingSelectedSetup );
    GetHsm( ).AddState( NotifiedNames_Name( NotifiedNames::SELECTED ),   statePlayingSelectedSetupNetwork );
    GetHsm( ).AddState( NotifiedNames_Name( NotifiedNames::SELECTED ),   statePlayingSelectedSetupNetworkTransition );
    GetHsm( ).AddState( NotifiedNames_Name( NotifiedNames::SELECTED ),   statePlayingSelectedSetupOther );
    GetHsm( ).AddState( NotifiedNames_Name( NotifiedNames::SELECTED ),   statePlayingSelectedSetupExiting );
    GetHsm( ).AddState( NotifiedNames_Name( NotifiedNames::SELECTED ),   statePlayingSelectedSetupExitingAP );
    GetHsm( ).AddState( "", stateStoppingStreams );
    GetHsm( ).AddState( "", stateAccessoryPairing );
    GetHsm( ).AddState( "", stateAccessoryPairingCancelling );
    GetHsm( ).AddState( "", stateAdaptIQ );
    GetHsm( ).AddState( "", stateAdaptIQExiting );
    GetHsm( ).AddState( "", stateStoppingStreamsDedicated );
    GetHsm( ).AddState( "", stateStoppingStreamsDedicatedForFactoryDefault );
    GetHsm( ).AddState( "", stateStoppingStreamsDedicatedForSoftwareUpdate );

    GetHsm( ).Init( this, PRODUCT_CONTROLLER_STATE_BOOTING );

    ///
    /// Initialize entities in the Common Product Controller
    ///
    CommonInitialize( );

    ///
    /// Get instances of all the modules.
    ///
    BOSE_DEBUG( s_logger, "----------- Product Controller Starting Modules ------------" );
    BOSE_DEBUG( s_logger, "The Professor Product Controller instantiating and running its modules." );

    m_ProductLpmHardwareInterface = std::make_shared< CustomProductLpmHardwareInterface >( *this );
    m_ProductCecHelper            = std::make_shared< ProductCecHelper                  >( *this );
    m_ProductDspHelper            = std::make_shared< ProductDspHelper                  >( *this );
    m_ProductCommandLine          = std::make_shared< ProductCommandLine                >( *this );
    m_ProductKeyInputManager      = std::make_shared< CustomProductKeyInputManager      >( *this );
    m_ProductAdaptIQManager       = std::make_shared< ProductAdaptIQManager             >( *this );
    m_ProductBLERemoteManager     = std::make_shared< ProductBLERemoteManager           >( *this );
    m_ProductAudioService         = std::make_shared< CustomProductAudioService         >( *this,
                                    m_FrontDoorClientIF,
                                    m_ProductLpmHardwareInterface->GetLpmClient( ) );

    if( m_ProductLpmHardwareInterface == nullptr ||
        m_ProductAudioService         == nullptr ||
        m_ProductCommandLine          == nullptr ||
        m_ProductKeyInputManager      == nullptr ||
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

    /// Register a callback so the autowake from persistence is sent to LPM when connected
    RegisterOpticalAutowakeForLpmConnection( );

    ///
    /// Set up LightBarController
    ///
    m_lightbarController = std::unique_ptr< LightBar::LightBarController >(
                               new LightBar::LightBarController( GetTask( ),
                                                                 m_FrontDoorClientIF,
                                                                 m_ProductLpmHardwareInterface->GetLpmClient( ) ) );

    ///
    /// Run all the submodules.
    ///
    m_ProductLpmHardwareInterface->Run( );
    m_ProductAudioService        ->Run( );
    m_ProductCommandLine         ->Run( );
    m_ProductKeyInputManager     ->Run( );
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
    /// Set up the STSProductController
    ///
    SetupProductSTSController( );

    ///
    /// Initialize and register intents for key actions for the Product Controller.
    ///
    m_IntentHandler.Initialize( );

    ///
    /// Register LPM events for LightBar
    ///
    m_lightbarController->RegisterLpmEvents();

    BOSE_DEBUG( s_logger, "------------ Product Controller Initialization End -------------" );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief  ProfessorProductController::GetMessageHandler
///
/// @return Callback < ProductMessage >
///
////////////////////////////////////////////////////////////////////////////////////////////////////
Callback < ProductMessage > ProfessorProductController::GetMessageHandler( )
{
    return m_ProductMessageHandler;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProfessorProductController::GetLpmHardwareInterface
///
/// @return This method returns a shared pointer to the LPM hardware interface.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
std::shared_ptr< CustomProductLpmHardwareInterface >& ProfessorProductController::GetLpmHardwareInterface( )
{
    return m_ProductLpmHardwareInterface;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProfessorProductController::GetProductAudioServiceInstance
///
/// @return This method returns a shared pointer to the Product AudioService which interfaces with AudioPath.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
std::shared_ptr< CustomProductAudioService >& ProfessorProductController::GetProductAudioServiceInstance( )
{
    return m_ProductAudioService;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProfessorProductController::GetAdaptIQManager
///
/// @return This method returns a shared pointer to the AdaptIQManager instance
///
////////////////////////////////////////////////////////////////////////////////////////////////////
std::shared_ptr< ProductAdaptIQManager >& ProfessorProductController::GetAdaptIQManager( )
{
    return m_ProductAdaptIQManager;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProfessorProductController::GetBLERemoteManager
///
/// @return This method returns a shared pointer to the BLERemoteManager instance
///
////////////////////////////////////////////////////////////////////////////////////////////////////
std::shared_ptr< ProductBLERemoteManager>& ProfessorProductController::GetBLERemoteManager( )
{
    return m_ProductBLERemoteManager;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProfessorProductController::GetCecHelper
///
/// @return This method returns a shared pointer to the ProductCecHelper instance.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
std::shared_ptr< ProductCecHelper >& ProfessorProductController::GetCecHelper( )
{
    return m_ProductCecHelper;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProfessorProductController::GetDspHelper
///
/// @return This method returns a shared pointer to the ProductCecHelper instance.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
std::shared_ptr< ProductDspHelper >& ProfessorProductController::GetDspHelper( )
{
    return m_ProductDspHelper;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProfessorProductController::IsBooted
///
/// @return This method returns a true or false value, based on a series of set member variables,
///         which all must be true to indicate that the device has booted.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool ProfessorProductController::IsBooted( ) const
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
    BOSE_VERBOSE( s_logger, " " );

    return ( IsLpmReady( )             and
             IsCAPSReady( )            and
             IsAudioPathReady( )       and
             IsSTSReady( )             and
             IsSoftwareUpdateReady( )  and
             IsSassReady( )            and
             IsBluetoothModuleReady( ) );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProfessorProductController::IsLowPowerExited
///
/// @return This method returns a true or false value, based on a series of set member variables,
///         which all must be true to indicate that the device has exited low power.
///         NOTE: Unlike booting we only wait for the things killed going to low power
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool ProfessorProductController::IsLowPowerExited( ) const
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
/// @name   ProfessorProductController::IsAutoWakeEnabled
///
/// @return This method returns a true or false value, based on a set member variable.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool ProfessorProductController::IsAutoWakeEnabled( ) const
{
    return m_IsAutoWakeEnabled;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProfessorProductController::IsSystemLanguageSet
///
/// @return This method returns true if the corresponding member has a system language defined.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool ProfessorProductController::IsSystemLanguageSet( ) const
{
    return m_deviceManager.IsLanguageSet( );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProfessorProductController::GetOOBDefaultLastContentItem
///
/// @return This method returns the PassportPB::contentItem value to be used for initializing the OOB LastContentItem
///
////////////////////////////////////////////////////////////////////////////////////////////////////
PassportPB::contentItem ProfessorProductController::GetOOBDefaultLastContentItem() const
{
    using namespace ProductSTS;

    PassportPB::contentItem item;
    item.set_source( ProductSourceSlot_Name( PRODUCT ) );
    item.set_sourceaccount( ProductSourceSlot_Name( TV ) );
    return item;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProfessorProductController::PossiblyPairBLERemote
///
/// @brief  initiates pairing of the BLE remote if indicated
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProfessorProductController::PossiblyPairBLERemote( )
{
    // The rules are per PGC-697:
    // On a system without a paired BLE remote, entry into SETUP will activate BLE pairing.
    // On a system with a paired BLE remote, pressing and holding the Action Button will activate BLE pairing.
    m_ProductBLERemoteManager->PossiblyPair();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProfessorProductController::PairBLERemote
///
/// @brief  initiates pairing of the BLE remote
///
/// @param  manualPairingRequest
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProfessorProductController::PairBLERemote( uint32_t timeout )
{
    // Tell the remote communications module to start pairing
    BOSE_INFO( s_logger, "%s requesting that the BLE remote pairing start", __func__ );

    m_ProductBLERemoteManager->Pairing_Start( timeout );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProfessorProductController::StopPairingBLERemote
///
/// @brief  stops pairing of the BLE remote
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProfessorProductController::StopPairingBLERemote( )
{
    // No harm if pairing is not active
    // Tell the remote communications module to stop pairing
    BOSE_INFO( s_logger, "%s requesting that the BLE remote pairing stop", __func__ );

    m_ProductBLERemoteManager->Pairing_Cancel();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProfessorProductController::IsBLERemoteConnected
///
/// @return true if the BLE remote is actively connected
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool ProfessorProductController::IsBLERemoteConnected( ) const
{
    return m_ProductBLERemoteManager->IsConnected();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProfessorProductController::SetupProductSTSController
///
/// @brief  This method is called to perform the needed initialization of the ProductSTSController,
///         specifically, provide the set of sources to be created initially.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProfessorProductController::SetupProductSTSController( )
{
    using namespace ProductSTS;

    std::vector< ProductSTSController::SourceDescriptor > sources;

    ProductSTSStateFactory<ProductSTSStateTop>       commonStateFactory;
    ProductSTSStateFactory<ProductSTSStateTopSilent> silentStateFactory;
    ProductSTSStateFactory<ProductSTSStateTopAiQ>    aiqStateFactory;

    ///
    /// ADAPTIQ, SETUP, and PAIRING are never available as a normal source, whereas the TV source
    /// will always be available. SLOT sources need to be set-up before they become available.
    ///
    ProductSTSController::SourceDescriptor descriptor_Setup   { SETUP,   ProductSourceSlot_Name( SETUP ),   false, silentStateFactory };
    ProductSTSController::SourceDescriptor descriptor_TV      { TV,      ProductSourceSlot_Name( TV ),      true,  commonStateFactory };
    ProductSTSController::SourceDescriptor descriptor_AiQ     { ADAPTIQ, ProductSourceSlot_Name( ADAPTIQ ), false, aiqStateFactory    };
    ProductSTSController::SourceDescriptor descriptor_Pairing { PAIRING, ProductSourceSlot_Name( PAIRING ), false, silentStateFactory };
    ProductSTSController::SourceDescriptor descriptor_SLOT_0  { SLOT_0,  ProductSourceSlot_Name( SLOT_0 ),  false, commonStateFactory, true };
    ProductSTSController::SourceDescriptor descriptor_SLOT_1  { SLOT_1,  ProductSourceSlot_Name( SLOT_1 ),  false, commonStateFactory, true };
    ProductSTSController::SourceDescriptor descriptor_SLOT_2  { SLOT_2,  ProductSourceSlot_Name( SLOT_2 ),  false, commonStateFactory, true };

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


    Callback< ProductSTSAccount::ProductSourceSlot >
    CallbackToHandleSelectSourceSlot( std::bind( &ProfessorProductController::HandleSelectSourceSlot,
                                                 this,
                                                 std::placeholders::_1 ) );

    m_ProductSTSController.Initialize( sources,
                                       CallbackForSTSComplete,
                                       CallbackToHandleSelectSourceSlot );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProfessorProductController::HandleSelectSourceSlot
///
/// @brief  This method is called from the ProductSTSController, when one of our sources is
///         activated by CAPS via STS.
///
/// @note   This method is called on the ProductSTSController task.
///
/// @param  ProductSTSAccount::ProductSourceSlot sourceSlot This identifies the activated slot.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProfessorProductController::HandleSelectSourceSlot( ProductSTSAccount::ProductSourceSlot sourceSlot )
{
    ProductMessage message;
    message.mutable_selectsourceslot( )->set_slot( static_cast< ProductSTS::ProductSourceSlot >( sourceSlot ) );

    IL::BreakThread( std::bind( GetMessageHandler( ),
                                message ),
                     GetTask( ) );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief ProfessorProductController::HandleUiHeartBeat
///
/// @param const DisplayControllerPb::UiHeartBeat & req
///
/// @param const Callback<DisplayControllerPb::UiHeartBeat> & respCb
///
/// @param const Callback<FrontDoor::Error> & errorCb
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProfessorProductController::HandleUiHeartBeat(
    const DisplayControllerPb::UiHeartBeat & req,
    const Callback<DisplayControllerPb::UiHeartBeat> & respCb,
    const Callback<FrontDoor::Error> & errorCb )
{
    BOSE_INFO( s_logger, "%s received UI heartbeat: %lld", __func__, req.count() );

    auto heartbeat = req.count();

    DisplayControllerPb::UiHeartBeat response;
    response.set_count( heartbeat );
    respCb( response );
}


////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProfessorProductController::RegisterFrontDoorEndPoints
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProfessorProductController::RegisterFrontDoorEndPoints( )
{
    RegisterCommonEndPoints( );

    m_lightbarController->RegisterLightBarEndPoints( );

    {
        auto callback = [ = ]( Callback< SystemPowerProductPb::SystemPowerModeOpticalAutoWake > respCb,
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
        auto callback = [ = ]( SystemPowerProductPb::SystemPowerModeOpticalAutoWake req,
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
        auto callback = [ = ]( DisplayControllerPb::UiHeartBeat req,
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
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   GetWiFiOperationalMode
///
/// @return NetManager::Protobuf::OperationalMode of the WiFi subsystem
///
////////////////////////////////////////////////////////////////////////////////////////////////////
NetManager::Protobuf::OperationalMode ProfessorProductController::GetWiFiOperationalMode( )
{
    return GetNetworkServiceUtil().GetNetManagerOperationMode();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProfessorProductController::HandleMessage
///
/// @brief  This method is called to handle product controller messages, which are sent from the
///         more product specific class instances, and is used to process the state machine for the
///         product.
///
/// @param  ProductMessage& message This argument contains product message event information based
///                                 on the ProductMessage Protocal Buffer.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProfessorProductController::HandleMessage( const ProductMessage& message )
{
    BOSE_INFO( s_logger, "%s received %s", __func__, message.DebugString().c_str() );

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// STS slot selected data is handled at this point.
    ///////////////////////////////////////////////////////////////////////////////////////////////
    if( message.has_selectsourceslot( ) )
    {
        const auto& slot = message.selectsourceslot( ).slot( );

        BOSE_DEBUG( s_logger, "An STS Select message was received for slot %s.",
                    ProductSTS::ProductSourceSlot_Name( static_cast<ProductSTS::ProductSourceSlot>( slot ) ).c_str( ) );
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
            message.wirelessstatus( ).frequencykhz( ) >= 0 )
        {
            IpcRadioStatus_t radioStatus;
            radioStatus.set_status( IPC_SOC_NETWORKSTATUS_OFF );
            radioStatus.set_band( IPC_SOC_RADIO_BAND_INVALID );

            if( message.wirelessstatus( ).frequencykhz( ) > 0 and
                message.wirelessstatus().frequencykhz( ) < 2500000 )
            {
                radioStatus.set_status( IPC_SOC_NETWORKSTATUS_WIFI );
                radioStatus.set_band( IPC_SOC_RADIO_BAND_24 );
            }
            else if( message.wirelessstatus( ).frequencykhz( ) >= 5100000 and
                     message.wirelessstatus( ).frequencykhz( ) >= 5200000 )
            {
                radioStatus.set_status( IPC_SOC_NETWORKSTATUS_WIFI );
                radioStatus.set_band( IPC_SOC_RADIO_BAND_52 );
            }
            else if( message.wirelessstatus( ).frequencykhz( ) >= 5700000 and
                     message.wirelessstatus( ).frequencykhz( ) >= 5800000 )
            {
                radioStatus.set_status( IPC_SOC_NETWORKSTATUS_WIFI );
                radioStatus.set_band( IPC_SOC_RADIO_BAND_58 );
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
    else if( message.networkstatus().has_wifiapstate() )
    {
        if( message.networkstatus().wifiapstate() )
        {
            GetLpmHardwareInterface()->SetAmp( true, true );
        }
        else
        {
            GetLpmHardwareInterface()->SetAmp( true, false );
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
/// @name   ProfessorProductController::Wait
///
/// @brief  This method is called from a calling task to wait until the Product Controller process
///         ends. It is running from the main task that started the application.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProfessorProductController::Wait( )
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
    m_ProductCecHelper           ->Stop( );
    m_ProductDspHelper           ->Stop( );
    m_ProductAdaptIQManager      ->Stop( );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProfessorProductController::GetDefaultProductName
///
/// @brief  This method is used to get the default product name.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
std::string ProfessorProductController::GetDefaultProductName( ) const
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
/// @brief ProfessorProductController::SendInitialCapsData
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProfessorProductController::SendInitialCapsData()
{
    BOSE_INFO( s_logger, __func__ );

    // Do the Common stuff first
    ProductController::SendInitialCapsData();

    // PUT /system/sources::properties
    SoundTouchInterface::Sources message;
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

    BOSE_VERBOSE( s_logger, "%s sending %s", __func__, ProtoToMarkup::ToJson( message ).c_str() );

    GetFrontDoorClient()->SendPut<SoundTouchInterface::Sources, FrontDoor::Error>(
        FRONTDOOR_SYSTEM_SOURCES_API,
        message,
        { },
        m_errorCb );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief ProfessorProductController::HandleGetOpticalAutoWake
///
/// @param const Callback<SystemPowerProductPb::SystemPowerModeOpticalAutoWake> & respCb
///
/// @param const Callback<FrontDoor::Error> & errorCb
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProfessorProductController::HandleGetOpticalAutoWake(
    const Callback<SystemPowerProductPb::SystemPowerModeOpticalAutoWake> & respCb,
    const Callback<FrontDoor::Error> & errorCb ) const
{
    SystemPowerProductPb::SystemPowerModeOpticalAutoWake autowake;
    autowake.set_enabled( m_IsAutoWakeEnabled );
    respCb( autowake );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief ProfessorProductController::HandlePutOpticalAutoWake
///
/// @param const SystemPowerProductPb::SystemPowerModeOpticalAutoWake & req
///
/// @param const Callback<SystemPowerProductPb::SystemPowerModeOpticalAutoWake> & respCb
///
/// @param const Callback<FrontDoor::Error> & errorCb
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProfessorProductController::HandlePutOpticalAutoWake(
    const SystemPowerProductPb::SystemPowerModeOpticalAutoWake & req,
    const Callback<SystemPowerProductPb::SystemPowerModeOpticalAutoWake> & respCb,
    const Callback<FrontDoor::Error> & errorCb )
{
    if( req.has_enabled( ) )
    {
        ProductMessage message;
        message.mutable_autowakestatus( )->set_active( req.enabled( ) );

        IL::BreakThread( std::bind( GetMessageHandler( ),
                                    message ),
                         GetTask( ) );

        HandleGetOpticalAutoWake( respCb, errorCb );
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
/// @brief ProfessorProductController::ApplyOpticalAutoWakeSettingFromPersistence
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProfessorProductController::ApplyOpticalAutoWakeSettingFromPersistence( )
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
/// @brief ProfessorProductController::NotifyFrontdoorAndStoreOpticalAutoWakeSetting
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProfessorProductController::NotifyFrontdoorAndStoreOpticalAutoWakeSetting( )
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
/// @brief ProfessorProductController::GetChimesConfigurationLocation
///
////////////////////////////////////////////////////////////////////////////////////////////////////
string ProfessorProductController::GetChimesConfigurationLocation( ) const
{
    string retVal{ g_ChimesConfigurationPath };
    retVal += GetProductType( );
    retVal += '/';
    return retVal + g_ChimesConfigurationFile;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief ProfessorProductController::GetChimesFilesLocation
///
////////////////////////////////////////////////////////////////////////////////////////////////////
string ProfessorProductController::GetChimesFilesLocation( ) const
{
    string retVal{ g_ChimesPath };
    retVal += GetProductType( );
    return retVal + '/';
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief ProfessorProductController::InitializeKeyIdToKeyNameMap
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProfessorProductController::InitializeKeyIdToKeyNameMap()
{
    BOSE_INFO( s_logger, "ProfessorProductController::%s:", __func__ );

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
/// @name   ProfessorProductController::End
///
/// @brief  This method is called when the Product Controller process should be terminated. It is
///         used to set the running member to false, which will invoke the Wait method idle loop to
///         exit and perform any necessary clean up.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProfessorProductController::End( )
{
    BOSE_DEBUG( s_logger, "The Product Controller main task is stopping." );

    m_Running = false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name ProfessorProductController::RegisterOpticalAutowakeForLpmConnection
///
/// @brief This method registers callback with LPM connection. When LPM is connected it sends the
///        autowake status read from NV to LPM
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProfessorProductController::RegisterOpticalAutowakeForLpmConnection( )
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
///                           End of the Product Application Namespace                           ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
