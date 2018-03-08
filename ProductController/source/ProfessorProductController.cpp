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
#include "ProductKeyInputInterface.h"
#include "ProductNetworkManager.h"
#include "ProductSystemManager.h"
#include "ProductCommandLine.h"
#include "ProductAdaptIQManager.h"
#include "ProductSourceInfo.h"
#include "IntentHandler.h"
#include "ProductSTS.pb.h"
#include "SystemSourcesProperties.pb.h"
#include "ProductControllerHsm.h"
#include "CustomProductControllerStates.h"
#include "CustomProductControllerState.h"
#include "ProductControllerStates.h"
#include "ProductControllerState.h"
#include "ProductControllerStateBooting.h"
#include "ProductControllerStateBooted.h"
#include "ProductControllerStateBootedTransition.h"
#include "ProductControllerStateFirstBootGreeting.h"
#include "ProductControllerStateFirstBootGreetingTransition.h"
#include "ProductControllerStateCriticalError.h"
#include "ProductControllerStateFactoryDefault.h"
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
#include "ProductControllerStatePlayingSelected.h"
#include "ProductControllerStatePlayingSelectedNotSilent.h"
#include "ProductControllerStatePlayingSelectedSetup.h"
#include "ProductControllerStatePlayingSelectedSetupNetwork.h"
#include "ProductControllerStatePlayingSelectedSetupNetworkTransition.h"
#include "ProductControllerStatePlayingSelectedSetupExitingAP.h"
#include "ProductControllerStatePlayingSelectedSetupOther.h"
#include "ProductControllerStatePlayingSelectedSetupExiting.h"
#include "ProductControllerStatePlayingSelectedSilent.h"
#include "ProductControllerStatePlayingTransition.h"
#include "ProductControllerStatePlayingTransitionSwitch.h"
#include "ProductControllerStateSoftwareInstall.h"
#include "ProductControllerStateSoftwareUpdateTransition.h"
#include "ProductControllerStateStoppingStreamsDedicatedForFactoryDefault.h"
#include "ProductControllerStateStoppingStreamsDedicatedForSoftwareUpdate.h"
#include "ProductControllerStateStoppingStreamsDedicated.h"
#include "ProductControllerStateStoppingStreams.h"
#include "ProductControllerStateTop.h"
#include "CustomProductControllerStateAdaptIQExiting.h"
#include "CustomProductControllerStateAdaptIQ.h"
#include "CustomProductControllerStateIdle.h"
#include "CustomProductControllerStateOn.h"
#include "CustomProductControllerStatePlayable.h"
#include "CustomProductControllerStatePlayingDeselectedAccessoryPairing.h"
#include "CustomProductControllerStatePlayingDeselected.h"
#include "CustomProductControllerStatePlaying.h"
#include "CustomProductControllerStatePlayingSelectedAccessoryPairing.h"
#include "CustomProductControllerStatePlayingSelected.h"
#include "CustomProductControllerStatePlayingSelectedSetup.h"
#include "CustomProductControllerStatePlayingTransitionAccessoryPairing.h"
#include "MfgData.h"
#include "DeviceManager.pb.h"
#include "ProductEndpointDefines.h"
#include "ProtoPersistenceFactory.h"

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

constexpr auto FRONTDOOR_SYSTEM_SOURCES_API = "/system/sources";

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProfessorProductController::ProfessorProductController
///
/// @brief  This method is the ProfessorProductController constructor, which is declared as being
///         private to ensure that only one instance of this class can be created through the class
///         GetInstance method.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
ProfessorProductController::ProfessorProductController( ) :

    ///
    /// Construction of the Product Controller Modules
    ///
    m_ProductLpmHardwareInterface( nullptr ),
    m_ProductSystemManager( nullptr ),
    m_ProductNetworkManager( nullptr ),
    m_ProductCommandLine( nullptr ),
    m_ProductKeyInputInterface( nullptr ),
    m_ProductCecHelper( nullptr ),
    m_ProductDspHelper( nullptr ),
    m_ProductAdaptIQManager( nullptr ),
    m_ProductAudioService( nullptr ),
    m_ProductSourceInfo( nullptr ),

    ///
    /// Member Variable Initialization
    ///
    m_IsAudioPathReady( false ),
    m_IsNetworkConfigured( false ),
    m_IsNetworkConnected( false ),
    m_IsAutoWakeEnabled( false ),
    m_Running( false ),

    ///
    /// Intent Handler Initialization
    ///
    m_IntentHandler( *GetTask(),
                     m_CliClientMT,
                     m_FrontDoorClientIF,
                     *this )
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name ProfessorProductController::Run
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProfessorProductController::Run( )
{
    CommonInitialize( );

    m_Running = true;

    BOSE_DEBUG( s_logger, "----------- Product Controller State Machine    ------------" );
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
    auto* stateBooting = new ProductControllerStateBooting
    ( GetHsm( ),
      stateTop,
      PRODUCT_CONTROLLER_STATE_BOOTING );

    auto* stateFirstBootGreetingTransition = new ProductControllerStateFirstBootGreetingTransition
    ( GetHsm( ),
      stateTop,
      PRODUCT_CONTROLLER_STATE_FIRST_BOOT_GREETING_TRANSITION );

    auto* stateFirstBootGreeting = new ProductControllerStateFirstBootGreeting
    ( GetHsm( ),
      stateTop,
      PRODUCT_CONTROLLER_STATE_FIRST_BOOT_GREETING );

    auto* stateBootedTransition = new ProductControllerStateBootedTransition
    ( GetHsm( ),
      stateTop,
      PRODUCT_CONTROLLER_STATE_BOOTED_TRANSITION );

    auto* stateBooted = new ProductControllerStateBooted
    ( GetHsm( ),
      stateTop,
      PRODUCT_CONTROLLER_STATE_BOOTED );

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
    auto* customStateOn = new CustomProductControllerStateOn
    ( GetHsm( ),
      stateTop,
      CUSTOM_PRODUCT_CONTROLLER_STATE_ON );

    ///
    /// Playable State and Sub-States
    ///
    auto* customStatePlayable = new CustomProductControllerStatePlayable
    ( GetHsm( ),
      customStateOn,
      CUSTOM_PRODUCT_CONTROLLER_STATE_PLAYABLE );

    auto* stateNetworkStandby = new ProductControllerStateNetworkStandby
    ( GetHsm( ),
      customStatePlayable,
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
      customStatePlayable,
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

    auto* customStatePlayingTransitionAccessoryPairing = new CustomProductControllerStatePlayingTransitionAccessoryPairing
    ( GetHsm( ),
      statePlayingTransition,
      CUSTOM_PRODUCT_CONTROLLER_STATE_PLAYING_TRANSITION_ACCESSORY_PAIRING );

    ///
    /// Playing State and Sub-States
    ///
    auto* customStatePlaying = new CustomProductControllerStatePlaying
    ( GetHsm( ),
      customStateOn,
      CUSTOM_PRODUCT_CONTROLLER_STATE_PLAYING );

    auto* customStatePlayingDeselected = new CustomProductControllerStatePlayingDeselected
    ( GetHsm( ),
      customStatePlaying,
      CUSTOM_PRODUCT_CONTROLLER_STATE_PLAYING_DESELECTED );

    auto* customStatePlayingDeselectedPairing = new CustomProductControllerStatePlayingDeselectedAccessoryPairing
    ( GetHsm( ),
      customStatePlayingDeselected,
      *this,
      CUSTOM_PRODUCT_CONTROLLER_STATE_PLAYING_DESELECTED_ACCESSORY_PAIRING );

    auto* customStatePlayingSelected = new CustomProductControllerStatePlayingSelected
    ( GetHsm( ),
      customStatePlaying,
      CUSTOM_PRODUCT_CONTROLLER_STATE_PLAYING_SELECTED );

    auto* statePlayingSelectedSilent = new ProductControllerStatePlayingSelectedSilent
    ( GetHsm( ),
      customStatePlayingSelected,
      PRODUCT_CONTROLLER_STATE_PLAYING_SELECTED_SILENT );

    auto* statePlayingSelectedNotSilent = new ProductControllerStatePlayingSelectedNotSilent
    ( GetHsm( ),
      customStatePlayingSelected,
      PRODUCT_CONTROLLER_STATE_PLAYING_SELECTED_NOT_SILENT );

    auto* customStatePlayingSelectedSetup = new CustomProductControllerStatePlayingSelectedSetup
    ( GetHsm( ),
      customStatePlayingSelected,
      CUSTOM_PRODUCT_CONTROLLER_STATE_PLAYING_SELECTED_SETUP );

    auto* statePlayingSelectedSetupNetworkTransition = new ProductControllerStatePlayingSelectedSetupNetworkTransition
    ( GetHsm( ),
      customStatePlayingSelectedSetup,
      PRODUCT_CONTROLLER_STATE_PLAYING_SELECTED_SETUP_NETWORK_TRANSITION );

    auto* statePlayingSelectedSetupNetwork = new ProductControllerStatePlayingSelectedSetupNetwork
    ( GetHsm( ),
      customStatePlayingSelectedSetup,
      PRODUCT_CONTROLLER_STATE_PLAYING_SELECTED_SETUP_NETWORK );

    auto* statePlayingSelectedSetupExitingAP = new ProductControllerStatePlayingSelectedSetupExitingAP
    ( GetHsm( ),
      statePlayingSelectedSetup,
      PRODUCT_CONTROLLER_STATE_PLAYING_SELECTED_SETUP_EXITING_AP );

    auto* statePlayingSelectedSetupOther = new ProductControllerStatePlayingSelectedSetupOther
    ( GetHsm( ),
      customStatePlayingSelectedSetup,
      PRODUCT_CONTROLLER_STATE_PLAYING_SELECTED_SETUP_OTHER );

    auto* statePlayingSelectedSetupExiting = new ProductControllerStatePlayingSelectedSetupExiting
    ( GetHsm( ),
      customStatePlayingSelectedSetup,
      PRODUCT_CONTROLLER_STATE_PLAYING_SELECTED_SETUP_EXITING );

    auto* customStatePlayingSelectedPairing = new CustomProductControllerStatePlayingSelectedAccessoryPairing
    ( GetHsm( ),
      customStatePlayingSelected,
      *this,
      CUSTOM_PRODUCT_CONTROLLER_STATE_PLAYING_SELECTED_ACCESSORY_PAIRING );

    auto* stateStoppingStreams = new ProductControllerStateStoppingStreams
    ( GetHsm( ),
      customStatePlayingSelected,
      PRODUCT_CONTROLLER_STATE_STOPPING_STREAMS );

    ///
    /// AdaptIQ States
    ///
    auto* customStateAdaptIQ = new CustomProductControllerStateAdaptIQ
    ( GetHsm( ),
      customStatePlayingSelected,
      *this,
      CUSTOM_PRODUCT_CONTROLLER_STATE_ADAPTIQ );

    auto* customStateAdaptIQExiting = new CustomProductControllerStateAdaptIQExiting
    ( GetHsm( ),
      customStatePlayingSelected,
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
    GetHsm( ).AddState( NotifiedNames_Name( NotifiedNames::FIRST_BOOT_GREETING ), stateFirstBootGreetingTransition );
    GetHsm( ).AddState( NotifiedNames_Name( NotifiedNames::FIRST_BOOT_GREETING ), stateFirstBootGreeting );
    GetHsm( ).AddState( NotifiedNames_Name( NotifiedNames::FIRST_BOOTING ), stateBootedTransition );
    GetHsm( ).AddState( NotifiedNames_Name( NotifiedNames::FIRST_BOOTING ), stateBooted );
    GetHsm( ).AddState( NotifiedNames_Name( NotifiedNames::UPDATING ), stateSoftwareUpdateTransition );
    GetHsm( ).AddState( NotifiedNames_Name( NotifiedNames::UPDATING ), stateSoftwareInstall );
    GetHsm( ).AddState( NotifiedNames_Name( NotifiedNames::CRITICAL_ERROR ), stateCriticalError );
    GetHsm( ).AddState( NotifiedNames_Name( NotifiedNames::FACTORY_DEFAULT ), stateFactoryDefault );
    GetHsm( ).AddState( "", stateLowPowerStandbyTransition );
    GetHsm( ).AddState( "", stateLowPowerStandby );
    GetHsm( ).AddState( "", statePlayableTransition );
    GetHsm( ).AddState( "", statePlayableTransitionInternal );
    GetHsm( ).AddState( "", statePlayableTransitionIdle );
    GetHsm( ).AddState( "", statePlayableTransitionNetworkStandby );
    GetHsm( ).AddState( "", customStateOn );
    GetHsm( ).AddState( "", customStatePlayable );
    GetHsm( ).AddState( NotifiedNames_Name( NotifiedNames::NETWORK_STANDBY ), stateNetworkStandby );
    GetHsm( ).AddState( NotifiedNames_Name( NotifiedNames::NETWORK_STANDBY ), stateNetworkStandbyConfigured );
    GetHsm( ).AddState( NotifiedNames_Name( NotifiedNames::NETWORK_STANDBY ), stateNetworkStandbyNotConfigured );
    GetHsm( ).AddState( NotifiedNames_Name( NotifiedNames::IDLE ), stateIdle );
    GetHsm( ).AddState( NotifiedNames_Name( NotifiedNames::IDLE ), stateIdleVoiceConfigured );
    GetHsm( ).AddState( NotifiedNames_Name( NotifiedNames::IDLE ), stateIdleVoiceNotConfigured );
    GetHsm( ).AddState( "", statePlayingTransition );
    GetHsm( ).AddState( "", statePlayingTransitionSelected );
    GetHsm( ).AddState( "", customStatePlayingTransitionAccessoryPairing );
    GetHsm( ).AddState( "", customStatePlaying );
    GetHsm( ).AddState( NotifiedNames_Name( NotifiedNames::DESELECTED ), customStatePlayingDeselected );
    GetHsm( ).AddState( NotifiedNames_Name( NotifiedNames::DESELECTED ), customStatePlayingDeselectedPairing );
    GetHsm( ).AddState( NotifiedNames_Name( NotifiedNames::SELECTED ), customStatePlayingSelected );
    GetHsm( ).AddState( NotifiedNames_Name( NotifiedNames::SELECTED ), statePlayingSelectedSilent );
    GetHsm( ).AddState( NotifiedNames_Name( NotifiedNames::SELECTED ), statePlayingSelectedNotSilent );
    GetHsm( ).AddState( NotifiedNames_Name( NotifiedNames::SELECTED ), customStatePlayingSelectedSetup );
    GetHsm( ).AddState( NotifiedNames_Name( NotifiedNames::SELECTED ), statePlayingSelectedSetupNetwork );
    GetHsm( ).AddState( NotifiedNames_Name( NotifiedNames::SELECTED ), statePlayingSelectedSetupNetworkTransition );
    GetHsm( ).AddState( NotifiedNames_Name( NotifiedNames::SELECTED ), statePlayingSelectedSetupExitingAP );
    GetHsm( ).AddState( NotifiedNames_Name( NotifiedNames::SELECTED ), statePlayingSelectedSetupOther );
    GetHsm( ).AddState( NotifiedNames_Name( NotifiedNames::SELECTED ), statePlayingSelectedSetupExiting );
    GetHsm( ).AddState( NotifiedNames_Name( NotifiedNames::SELECTED ), customStatePlayingSelectedPairing );
    GetHsm( ).AddState( "", stateStoppingStreams );
    GetHsm( ).AddState( "", customStateAdaptIQ );
    GetHsm( ).AddState( "", customStateAdaptIQExiting );
    GetHsm( ).AddState( "", stateStoppingStreamsDedicated );
    GetHsm( ).AddState( "", stateStoppingStreamsDedicatedForFactoryDefault );
    GetHsm( ).AddState( "", stateStoppingStreamsDedicatedForSoftwareUpdate );

    GetHsm( ).Init( this, PRODUCT_CONTROLLER_STATE_BOOTING );

    ///
    /// Initialize entities in the Common Product Controller
    ///
    m_deviceManager.Initialize( this );

    ///
    /// Get instances of all the modules.
    ///
    BOSE_DEBUG( s_logger, "----------- Product Controller Starting Modules ------------" );
    BOSE_DEBUG( s_logger, "The Professor Product Controller instantiating and running its modules." );

    m_ProductLpmHardwareInterface = std::make_shared< CustomProductLpmHardwareInterface >( *this );
    m_ProductCecHelper            = std::make_shared< ProductCecHelper                  >( *this );
    m_ProductDspHelper            = std::make_shared< ProductDspHelper                  >( *this );
    m_ProductSystemManager        = std::make_shared< ProductSystemManager              >( *this );
    m_ProductNetworkManager       = std::make_shared< ProductNetworkManager             >( *this );
    m_ProductCommandLine          = std::make_shared< ProductCommandLine                >( *this );
    m_ProductKeyInputInterface    = std::make_shared< ProductKeyInputInterface          >( *this );
    m_ProductAdaptIQManager       = std::make_shared< ProductAdaptIQManager             >( *this );
    m_ProductSourceInfo           = std::make_shared< ProductSourceInfo                 >( *this );
    m_ProductAudioService         = std::make_shared< CustomProductAudioService         >( *this, m_FrontDoorClientIF, m_ProductLpmHardwareInterface->GetLpmClient() );

    if( m_ProductLpmHardwareInterface == nullptr ||
        m_ProductSystemManager        == nullptr ||
        m_ProductNetworkManager       == nullptr ||
        m_ProductAudioService         == nullptr ||
        m_ProductCommandLine          == nullptr ||
        m_ProductKeyInputInterface    == nullptr ||
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

    ///
    /// Set up LightBarController
    ///
    m_lightbarController = std::unique_ptr<LightBar::LightBarController>( new LightBar::LightBarController( GetTask(), m_FrontDoorClientIF,  m_ProductLpmHardwareInterface->GetLpmClient() ) );

    ///
    /// Run all the submodules.
    ///
    m_ProductLpmHardwareInterface->Run( );
    m_ProductSystemManager       ->Run( );
    m_ProductNetworkManager      ->Run( );
    m_ProductAudioService        ->Run( );
    m_ProductCommandLine         ->Run( );
    m_ProductKeyInputInterface   ->Run( );
    m_ProductCecHelper           ->Run( );
    m_ProductDspHelper           ->Run( );
    m_ProductAdaptIQManager      ->Run( );
    m_ProductSourceInfo          ->Run( );

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
    SetupProductSTSConntroller( );

    ///
    /// Initialize and register intents for key actions for the Product Controller.
    ///
    m_IntentHandler.Initialize( );

    ///
    /// Register LPM events for LightBar
    ///
    m_lightbarController->RegisterLpmEvents();
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
    Callback < ProductMessage >
    ProductMessageHandler( std::bind( &ProfessorProductController::HandleMessage,
                                      this,
                                      std::placeholders::_1 ) );
    return ProductMessageHandler;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief  ProfessorProductController::GetCommandLineInterface
///
/// @return This method returns a reference to a command line interface for adding module specific
///         commands. Note that this interface is instantiated in the inherited ProductController
///         class; the ProductCommandLine interface instantiated in this class is used for specific
///         product controller commands in Professor.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
CliClientMT& ProfessorProductController::GetCommandLineInterface( )
{
    return m_CliClientMT;
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
/// @name   ProfessorProductController::GetSourceInfo
///
/// @return This method returns a shared pointer to the SourceInfo instance
///
////////////////////////////////////////////////////////////////////////////////////////////////////
std::shared_ptr< ProductSourceInfo >& ProfessorProductController::GetSourceInfo( )
{
    return m_ProductSourceInfo;
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
    BOSE_VERBOSE( s_logger, "LPM Connected         :  %s", ( m_IsLpmReady       ? "true" : "false" ) );
    BOSE_VERBOSE( s_logger, "CAPS Initialized      :  %s", ( m_IsCapsReady      ? "true" : "false" ) );
    BOSE_VERBOSE( s_logger, "Audio Path Connected  :  %s", ( m_IsAudioPathReady ? "true" : "false" ) );
    BOSE_VERBOSE( s_logger, "STS Initialized       :  %s", ( m_IsSTSReady       ? "true" : "false" ) );
    BOSE_VERBOSE( s_logger, "Software Update Init  :  %s", ( m_isSoftwareUpdateReady   ? "true" : "false" ) );
    BOSE_VERBOSE( s_logger, "Bluetooth Initialized :  %s", ( IsBluetoothModuleReady( ) ? "true" : "false" ) );
    BOSE_VERBOSE( s_logger, " " );

    return( m_IsLpmReady            and
            m_IsCapsReady           and
            m_IsAudioPathReady      and
            m_IsSTSReady            and
            m_isSoftwareUpdateReady and
            IsBluetoothModuleReady( ) );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProfessorProductController::IsNetworkConfigured
///
/// @return This method returns a true or false value, based on a set member variable.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool ProfessorProductController::IsNetworkConfigured( ) const
{
    return m_IsNetworkConfigured;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProfessorProductController::IsNetworkConnected
///
/// @return This method returns a true or false value, based on a set member variable.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool ProfessorProductController::IsNetworkConnected( ) const
{
    return m_IsNetworkConnected;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProfessorProductController::GetWifiProfileCount
///
/// @return This method returns the number of WiFi profiles as an unsigned integer.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
uint32_t ProfessorProductController::GetWifiProfileCount( ) const
{
    return m_ProductNetworkManager->GetWifiProfileCount( );
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
/// @name   IsFirstTimeBootUp
///
/// @return This method returns a true or false value, based on whether this is the first time
///         booting up after a factory default.
///
/// @todo   A way of determing whether the device is performing a first time boot up will need to be
///         coded, possibly through adding a method call to the DeviceManager class in the common
///         code base. A JIRA Story PGC-715 has been create for this purpose.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool ProfessorProductController::IsFirstTimeBootUp( ) const
{
    return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   IsOutOfBoxSetupComplete
///
/// @return This method returns a true o0r false value, based on a set member variable.
///
/// @todo   A way of determing whether the out-of-box setup is complete will need to be coded,
///         possibly through adding a method call to the DeviceManager class in the common code base.
///         A JIRA Story PGC-714 has been create for this purpose.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool ProfessorProductController::IsOutOfBoxSetupComplete( ) const
{
    return true;
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
/// @name   ProfessorProductController::GetProductName
///
/// @return This method returns the std::string value to be used for the Product "productName" field
///
////////////////////////////////////////////////////////////////////////////////////////////////////
std::string ProfessorProductController::GetProductName( ) const
{
    // @TODO PGC-788 replace the manufacturing name with the marketing name.
    std::string productName = "professor Soundbar";

    if( auto name = MfgData::Get( "productName" ) )
    {
        productName =  *name;
    }

    return productName;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProfessorProductController::GetProductColor
///
/// @return This method returns the std::string value to be used for the Product "productColor" field
///
////////////////////////////////////////////////////////////////////////////////////////////////////
std::string ProfessorProductController::GetProductColor() const
{
    // @TODO PGC-630
    std::string productColor = "2";

    if( auto color = MfgData::Get( "productColor" ) )
    {
        productColor =  *color;
    }

    return productColor;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProfessorProductController::GetProductType
///
/// @return This method returns the std::string value to be used for the Product "productType" field
///
/// @TODO - Below value may be available through HSP APIs
///
////////////////////////////////////////////////////////////////////////////////////////////////////
std::string ProfessorProductController::GetProductType() const
{
    std::string productType = "professor";

    if( auto type = MfgData::Get( "productType" ) )
    {
        productType =  *type;
    }

    return productType;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProfessorProductController::GetOOBDefaultLastContentItem
///
/// @return This method returns the PassportPB::ContentItem value to be used for initializing the OOB LastContentItem
///
////////////////////////////////////////////////////////////////////////////////////////////////////
PassportPB::ContentItem ProfessorProductController::GetOOBDefaultLastContentItem() const
{
    PassportPB::ContentItem item;
    item.set_source( "PRODUCT" );
    item.set_sourceaccount( "TV" );
    return item;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProfessorProductController::CanPersistAsLastContentItem
///
/// @param  const SoundTouchInterface::ContentItem &ci
///
/// @brief  Determines if the content item can be persisted in m_lastContentItem
///
/// @return Returns true or false
///////////////////////////////////////////////////////////////////////////////
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool ProfessorProductController::CanPersistAsLastContentItem( const SoundTouchInterface::ContentItem &ci ) const
{
    bool retVal = true;
    if( !ProductController::CanPersistAsLastContentItem( ci ) )
    {
        retVal = false;
    }
    if( ci.source() == "PRODUCT" && ( ci.sourceaccount() == "ADAPTiQ" ) )
    {
        retVal = false;
    }

    BOSE_VERBOSE( s_logger, "ContentItem %s can%s persist in Professor as LastContentItem",
                  ProtoToMarkup::ToJson( ci, false ).c_str( ), retVal ? "" : "not" );
    return retVal;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProfessorProductController::SetupProductSTSConntroller
///
/// @brief  This method is called to perform the needed initialization of the ProductSTSController,
///         specifically, provide the set of sources to be created initially.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProfessorProductController::SetupProductSTSConntroller( )
{
    std::vector< ProductSTSController::SourceDescriptor > sources;

    ///
    /// Adapt IQ and Setup is not available as a normal source, whereas the TV source will always
    /// be available.
    ///
    ProductSTSController::SourceDescriptor descriptor_AiQ    { ProductSTS::SLOT_AIQ,   "ADAPTiQ", false };
    ProductSTSController::SourceDescriptor descriptor_Setup  { ProductSTS::SLOT_SETUP, "SETUP",   false };
    ProductSTSController::SourceDescriptor descriptor_TV     { ProductSTS::SLOT_TV,    "TV",      true  };
    ProductSTSController::SourceDescriptor descriptor_SLOT_0 { ProductSTS::SLOT_0,     "SLOT_0",  false };
    ProductSTSController::SourceDescriptor descriptor_SLOT_1 { ProductSTS::SLOT_1,     "SLOT_1",  false };
    ProductSTSController::SourceDescriptor descriptor_SLOT_2 { ProductSTS::SLOT_2,     "SLOT_2",  false };

    sources.push_back( descriptor_AiQ );
    sources.push_back( descriptor_Setup );
    sources.push_back( descriptor_TV );
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

    IL::BreakThread( std::bind( &ProfessorProductController::HandleMessage,
                                this,
                                message ),
                     GetTask( ) );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProfessorProductController::RegisterFrontDoorEndPoints
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProfessorProductController::RegisterFrontDoorEndPoints( )
{
    RegisterCommonEndPoints( );
    m_lightbarController->RegisterLightBarEndPoints();

    {
        auto l = [ = ]( Callback<SystemPowerProductPb::SystemPowerModeOpticalAutoWake> respCb,
                        Callback<EndPointsError::Error> errorCb )
        {
            HandleGetOpticalAutoWake( respCb, errorCb );
        };
        GetFrontDoorClient()->RegisterGet( FRONTDOOR_SYSTEM_POWER_MODE_OPTICALAUTOWAKE_API, l );
    }
    {
        auto l = [ = ]( SystemPowerProductPb::SystemPowerModeOpticalAutoWake req,
                        Callback<SystemPowerProductPb::SystemPowerModeOpticalAutoWake> respCb,
                        Callback<EndPointsError::Error> errorCb )
        {
            HandlePutOpticalAutoWake( req, respCb, errorCb );
        };
        GetFrontDoorClient( )->RegisterPut<SystemPowerProductPb::SystemPowerModeOpticalAutoWake>(
            FRONTDOOR_SYSTEM_POWER_MODE_OPTICALAUTOWAKE_API, l );
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
    BOSE_DEBUG( s_logger, "----------- Product Controller Message Handler -------------" );

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// LPM status messages has both Common handling and Professor-specific handling
    ///////////////////////////////////////////////////////////////////////////////////////////////
    if( message.has_lpmstatus( ) )
    {
        ///
        /// First do the common stuff.
        ///
        ( void ) HandleCommonProductMessage( message );

        ///
        /// Then do the Professor specific stuff.
        ///
        if( message.lpmstatus( ).has_systemstate( ) )
        {
            switch( message.lpmstatus( ).systemstate( ) )
            {
            case SYSTEM_STATE_ON:
                BOSE_DEBUG( s_logger, "Calling HandleLPMPowerStatusFullPower( )" );
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
                m_ProductAudioService->SetThermalMonitorEnabled( false );
                break;
            case SYSTEM_STATE_UPDATE:
                break;
            case SYSTEM_STATE_SHUTDOWN:
                m_ProductAudioService->SetThermalMonitorEnabled( false );
                break;
            case SYSTEM_STATE_FACTORY_DEFAULT:
                break;
            case SYSTEM_STATE_IDLE:
                m_ProductAudioService->SetThermalMonitorEnabled( false );
                break;
            case SYSTEM_STATE_NUM_OF:
                break;
            case SYSTEM_STATE_ERROR:
                break;
            }
        }
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// Audio path status messages are handled at this point.
    ///////////////////////////////////////////////////////////////////////////////////////////////
    else if( message.has_audiopathstatus( ) )
    {
        if( message.audiopathstatus( ).has_connected( ) )
        {
            m_IsAudioPathReady = message.audiopathstatus( ).connected( );
        }
        else
        {
            BOSE_ERROR( s_logger, "An invalid audio path status message was received." );
            return;
        }

        BOSE_DEBUG( s_logger, "An audio path status %s message was received.",
                    m_IsAudioPathReady ? "connected" : "not connected" );

        GetHsm( ).Handle< bool >
        ( &CustomProductControllerState::HandleAudioPathState, m_IsAudioPathReady );
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// STS slot selected data is handled at this point.
    ///////////////////////////////////////////////////////////////////////////////////////////////
    else if( message.has_selectsourceslot( ) )
    {
        const auto& slot = message.selectsourceslot( ).slot( );

        BOSE_DEBUG( s_logger, "An STS Select message was received for slot %s.",
                    ProductSTS::ProductSourceSlot_Name( static_cast<ProductSTS::ProductSourceSlot>( slot ) ).c_str( ) );
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// Network status messages are handled at this point.
    ///////////////////////////////////////////////////////////////////////////////////////////////
    else if( message.has_networkstatus( ) )
    {
        if( message.networkstatus( ).has_configured( ) )
        {
            m_IsNetworkConfigured = message.networkstatus( ).configured( );
        }
        else
        {
            BOSE_ERROR( s_logger, "An invalid network configured status message was received." );
            return;
        }

        if( message.networkstatus( ).has_connected( ) )
        {
            m_IsNetworkConnected = message.networkstatus( ).connected( );
        }
        else
        {
            BOSE_ERROR( s_logger, "An invalid network connected status message was received." );
            return;
        }

        if( message.networkstatus( ).networktype( ) == ProductNetworkStatus_ProductNetworkType_Wireless )
        {
            BOSE_DEBUG( s_logger, "A wireless %s %s network message was received.",
                        m_IsNetworkConfigured ? "configured" : "unconfigured",
                        m_IsNetworkConnected  ? "connected"  : "unconnected" );
        }
        else if( message.networkstatus( ).networktype( ) == ProductNetworkStatus_ProductNetworkType_Wired )
        {
            BOSE_DEBUG( s_logger, "A wired %s %s network message was received.",
                        m_IsNetworkConfigured ? "configured" : "unconfigured",
                        m_IsNetworkConnected  ? "connected"  : "unconnected" );
        }
        else
        {
            BOSE_DEBUG( s_logger, "A unknown %s %s network message was received.",
                        m_IsNetworkConfigured ? "configured" : "unconfigured",
                        m_IsNetworkConnected  ? "connected"  : "unconnected" );
        }

        m_ProductSystemManager->SetNetworkAccoutConfigurationStatus( m_IsNetworkConfigured,
                                                                     m_IsVoiceAccountConfigured );

        GetHsm( ).Handle< bool, bool >
        ( &CustomProductControllerState::HandleNetworkState, m_IsNetworkConfigured, m_IsNetworkConnected );
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
            m_ProductLpmHardwareInterface->SendWiFiRadioStatus( message.wirelessstatus( ).frequencykhz( ) );

            BOSE_DEBUG( s_logger, "A wireless network message was received with frequency %d kHz.",
                        message.wirelessstatus( ).has_frequencykhz( ) ?
                        message.wirelessstatus( ).frequencykhz( ) : 0 );
        }
        else
        {
            BOSE_ERROR( s_logger, "A wireless network message was received with an unknown frequency." );
        }
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

        BOSE_DEBUG( s_logger, "An autowake status %s message has been received.",
                    m_IsAutoWakeEnabled ? "active" : "inactive" );

        NotifyFrontdoorAndStoreOpticalAutoWakeSetting( );

        GetHsm( ).Handle< bool >
        ( &CustomProductControllerState::HandleAutowakeStatus, m_IsAutoWakeEnabled );
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// Accessory pairing messages are handled at this point.
    ///////////////////////////////////////////////////////////////////////////////////////////////
    else if( message.has_accessorypairing( ) )
    {
        GetHsm( ).Handle< ProductAccessoryPairing >
        ( &CustomProductControllerState::HandlePairingState, message.accessorypairing( ) );
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
        else if( GetIntentHandler( ).IsIntentUserPower( message.action( ) ) )
        {
            GetHsm( ).Handle< >( &CustomProductControllerState::HandleIntentPowerToggle );
        }
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
        BOSE_DEBUG( s_logger, "CECMODE set to %d",  message.cecmode( ).cecmode( ) );
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
    m_ProductSystemManager       ->Stop( );
    m_ProductNetworkManager      ->Stop( );
    m_ProductCommandLine         ->Stop( );
    m_ProductKeyInputInterface   ->Stop( );
    m_ProductCecHelper           ->Stop( );
    m_ProductDspHelper           ->Stop( );
    m_ProductAdaptIQManager      ->Stop( );
    m_ProductSourceInfo          ->Stop( );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProfessorProductController::GetDefaultProductName
///
/// @brief  This method is to get the default product name by reading from mac address.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
std::string ProfessorProductController::GetDefaultProductName( ) const
{
    std::string productName = "Bose ";
    std::string macAddress = MacAddressInfo::GetPrimaryMAC( );
    try
    {
        productName += ( macAddress.substr( macAddress.length( ) - 6 ) );
    }
    catch( const std::out_of_range& error )
    {
        productName += macAddress;
        BOSE_WARNING( s_logger, "errorType = %s", error.what( ) );
    }

    BOSE_INFO( s_logger, "%s productName=%s", __func__, productName.c_str( ) );
    return productName;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProfessorProductController::GetVariantId
///
/// @brief  This method is to get the product variant ID, such as its colour.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
BLESetupService::VariantId ProfessorProductController::GetVariantId( ) const
{
    // @TODO https://jirapro.bose.com/browse/PGC-630
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

    GetFrontDoorClient()->SendPut<SoundTouchInterface::NowPlaying, EndPointsError::Error>(
        FRONTDOOR_SYSTEM_SOURCES_API,
        message,
        { },
        m_errorCb );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief ProfessorProductController::ClearWifiProfileCount
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProfessorProductController::ClearWifiProfileCount( )
{
    if( m_ProductNetworkManager != nullptr )
    {
        m_ProductNetworkManager->ClearWifiProfileCount( );
    }
    else
    {
        BOSE_DIE( "ProductNetworkManager has not been instantiated for ClearWifiProfileCount." );
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief ProfessorProductController::PerformRequestforWiFiProfiles
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProfessorProductController::PerformRequestforWiFiProfiles( )
{
    if( m_ProductNetworkManager != nullptr )
    {
        m_ProductNetworkManager->PerformRequestforWiFiProfiles( );
    }
    else
    {
        BOSE_DIE( "ProductNetworkManager has not been instantiated for PerformRequestforWiFiProfiles." );
    }
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

void ProfessorProductController::HandleGetOpticalAutoWake(
    const Callback<SystemPowerProductPb::SystemPowerModeOpticalAutoWake> & respCb,
    const Callback<EndPointsError::Error> & errorCb ) const
{
    SystemPowerProductPb::SystemPowerModeOpticalAutoWake autowake;
    autowake.set_enabled( m_IsAutoWakeEnabled );
    respCb( autowake );
}

void ProfessorProductController::HandlePutOpticalAutoWake(
    const SystemPowerProductPb::SystemPowerModeOpticalAutoWake & req,
    const Callback<SystemPowerProductPb::SystemPowerModeOpticalAutoWake> & respCb,
    const Callback<EndPointsError::Error> & errorCb )
{
    if( req.has_enabled( ) )
    {
        ProductMessage message;
        message.mutable_autowakestatus( )->set_active( req.enabled( ) );
        HandleMessage( message );
    }
    HandleGetOpticalAutoWake( respCb, errorCb );
}

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
///                           End of the Product Application Namespace                           ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
