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
#include "ProductControllerStateTop.h"
#include "CustomProductControllerStates.h"
#include "IntentHandler.h"
#include "ProductSTS.pb.h"
#include "ProductControllerStates.h"
#include "SystemSourcesProperties.pb.h"
#include "CustomProductControllerState.h"
#include "CustomProductControllerStateBooting.h"
#include "CustomProductControllerStateOn.h"
#include "CustomProductControllerStatePlayable.h"
#include "CustomProductControllerStatePlaying.h"
#include "CustomProductControllerStatePlayingActive.h"
#include "CustomProductControllerStatePlayingInactive.h"
#include "CustomProductControllerStateAccessoryPairing.h"
#include "CustomProductControllerStateAdaptIQ.h"
#include "ProductControllerStateNetworkStandby.h"
#include "ProductControllerStateNetworkStandbyConfigured.h"
#include "ProductControllerStateNetworkStandbyNotConfigured.h"
#include "ProductControllerStateIdle.h"
#include "ProductControllerStateIdleVoiceConfigured.h"
#include "ProductControllerStateIdleVoiceNotConfigured.h"
#include "ProductControllerStateLowPowerStandby.h"
#include "ProductControllerStateLowPowerStandbyTransition.h"
#include "ProductControllerStatePlayingDeselected.h"
#include "ProductControllerStatePlayingSelected.h"
#include "ProductControllerStatePlayingSelectedSilent.h"
#include "ProductControllerStatePlayingSelectedNotSilent.h"
#include "ProductControllerStatePlayingSelectedSetup.h"
#include "ProductControllerStatePlayingSelectedSetupNetwork.h"
#include "ProductControllerStatePlayingSelectedSetupOther.h"
#include "ProductControllerStateStoppingStreams.h"
#include "ProductControllerStatePlayableTransition.h"
#include "ProductControllerStatePlayableTransitionIdle.h"
#include "ProductControllerStatePlayableTransitionNetworkStandby.h"
#include "ProductControllerStateSoftwareUpdateTransition.h"
#include "ProductControllerStatePlayingTransition.h"
#include "ProductControllerStatePlayingTransitionSelected.h"
#include "ProductControllerStateFactoryDefault.h"
#include "ProductControllerStateStoppingStreamsDedicated.h"
#include "ProductControllerStateStoppingStreamsDedicatedForFactoryDefault.h"
#include "ProductControllerStateStoppingStreamsDedicatedForSoftwareUpdate.h"
#include "ProductControllerStatePlayingSelectedSetupExiting.h"
#include "ProductControllerStateWelcome.h"
#include "ProductControllerStateSoftwareUpdating.h"
#include "ProductControllerStateRebooting.h"
#include "ProductControllerStateCriticalError.h"
#include "MfgData.h"

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
    /// Construction of the common Product Controller Class
    ///
    ProductController( "Professor" ),

    ///
    /// Construction of the Product Controller Modules
    ///
    m_ProductLpmHardwareInterface( nullptr ),
    m_ProductSystemManager( nullptr ),
    m_ProductNetworkManager( nullptr ),
    m_ProductCommandLine( nullptr ),
    m_ProductKeyInputInterface( nullptr ),
    m_ProductCecHelper( nullptr ),
    m_ProductAdaptIQManager( nullptr ),
    m_ProductAudioService( nullptr ),

    ///
    /// Member Variable Initialization
    ///
    m_IsCapsReady( false ),
    m_IsAudioPathReady( false ),
    m_IsSTSReady( false ),
    m_IsNetworkConfigured( false ),
    m_IsNetworkConnected( false ),
    m_IsAutoWakeEnabled( false ),
    m_IsAccountConfigured( false ),
    m_IsMicrophoneEnabled( false ),
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
    m_Running = true;

    BOSE_DEBUG( s_logger, "----------- Product Controller State Machine    ------------" );
    BOSE_DEBUG( s_logger, "The Professor Product Controller is setting up the state machine." );

    ///
    /// Construction of the common and custom states
    ///
    auto* stateTop = new ProductControllerStateTop( GetHsm( ),
                                                    nullptr );

    auto* customStateBooting = new CustomProductControllerStateBooting
    ( GetHsm( ),
      stateTop,
      PROFESSOR_PRODUCT_CONTROLLER_STATE_BOOTING );

    auto* stateWelcome = new ProductControllerStateWelcome
    ( GetHsm( ),
      stateTop,
      PRODUCT_CONTROLLER_STATE_WELCOME );

    auto* stateSoftwareUpdateTransition = new ProductControllerStateSoftwareUpdateTransition
    ( GetHsm( ),
      stateTop,
      PRODUCT_CONTROLLER_STATE_SOFTWARE_UPDATE_TRANSITION );

    auto* stateSoftwareUpdating = new ProductControllerSoftwareUpdating
    ( GetHsm( ),
      stateTop,
      PRODUCT_CONTROLLER_STATE_SOFTWARE_UPDATING );

    auto* stateRebooting = new ProductControllerStateRebooting
    ( GetHsm( ),
      stateTop,
      PRODUCT_CONTROLLER_STATE_REBOOTING );

    auto* stateCriticalError = new ProductControllerStateCriticalError
    ( GetHsm( ),
      stateTop,
      PRODUCT_CONTROLLER_STATE_CRITICAL_ERROR );

    auto* stateFactoryDefault = new ProductControllerStateFactoryDefault
    ( GetHsm( ),
      stateTop,
      PRODUCT_CONTROLLER_STATE_FACTORY_DEFAULT );

    auto* statePlayableTransition = new ProductControllerStatePlayableTransition
    ( GetHsm( ),
      stateTop,
      PRODUCT_CONTROLLER_STATE_PLAYABLE_TRANSITION );

    auto* statePlayableTransitionIdle = new ProductControllerStatePlayableTransitionIdle
    ( GetHsm( ),
      statePlayableTransition,
      PRODUCT_CONTROLLER_STATE_PLAYABLE_TRANSITION_IDLE );

    auto* statePlayableTransitionNetworkStandby = new ProductControllerStatePlayableTransitionNetworkStandby
    ( GetHsm( ),
      statePlayableTransition,
      PRODUCT_CONTROLLER_STATE_PLAYABLE_TRANSITION_NETWORK_STANDBY );

    auto* customStateOn = new CustomProductControllerStateOn
    ( GetHsm( ),
      stateTop,
      PROFESSOR_PRODUCT_CONTROLLER_STATE_ON );

    auto* customStatePlayable = new CustomProductControllerStatePlayable
    ( GetHsm( ),
      customStateOn,
      PROFESSOR_PRODUCT_CONTROLLER_STATE_PLAYABLE );

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

    auto* stateLowPowerStandby = new ProductControllerStateLowPowerStandby
    ( GetHsm( ),
      stateTop,
      PRODUCT_CONTROLLER_STATE_LOW_POWER_STANDBY );

    auto* stateLowPowerStandbyTransition = new ProductControllerStateLowPowerStandbyTransition
    ( GetHsm( ),
      stateTop,
      PRODUCT_CONTROLLER_STATE_LOW_POWER_STANDBY_TRANSITION );

    auto* stateIdle = new ProductControllerStateIdle
    ( GetHsm( ),
      customStatePlayable,
      PRODUCT_CONTROLLER_STATE_IDLE );

    auto* stateIdleVoiceConfigured = new ProductControllerStateIdleVoiceConfigured
    ( GetHsm( ),
      stateIdle,
      PRODUCT_CONTROLLER_STATE_IDLE_VOICE_CONFIGURED );

    auto* stateIdleVoiceNotConfigured = new ProductControllerStateIdleVoiceNotConfigured
    ( GetHsm( ),
      stateIdle,
      PRODUCT_CONTROLLER_STATE_IDLE_VOICE_NOT_CONFIGURED );

    auto* customStatePlaying = new CustomProductControllerStatePlaying
    ( GetHsm( ),
      customStateOn,
      PROFESSOR_PRODUCT_CONTROLLER_STATE_PLAYING );

    auto* customStatePlayingActive = new CustomProductControllerStatePlayingActive
    ( GetHsm( ),
      customStatePlaying,
      PROFESSOR_PRODUCT_CONTROLLER_STATE_PLAYING_ACTIVE );

    auto* customStatePlayingInactive = new CustomProductControllerStatePlayingInactive
    ( GetHsm( ),
      customStatePlaying,
      PROFESSOR_PRODUCT_CONTROLLER_STATE_PLAYING_INACTIVE );

    auto* customStateAccessoryPairing = new CustomProductControllerStateAccessoryPairing
    ( GetHsm( ),
      customStatePlayingActive,
      *this,
      PROFESSOR_PRODUCT_CONTROLLER_STATE_ACCESSORY_PAIRING );

    auto* customStateAdaptIQ = new CustomProductControllerStateAdaptIQ
    ( GetHsm( ),
      customStatePlayingActive,
      *this,
      PROFESSOR_PRODUCT_CONTROLLER_STATE_ADAPTIQ );

    ///
    /// @todo These states are stubs for the time being, until the playing transition and its
    ///       associated states are completed based on the JIRA Stories PGC-246 and PGC-632.
    ///
    auto* statePlayingTransition = new ProductControllerStatePlayingTransition
    ( GetHsm( ),
      stateTop,
      PRODUCT_CONTROLLER_STATE_PLAYING_TRANSITION );

    auto* statePlayingTransitionSelected = new ProductControllerStatePlayingTransitionSelected
    ( GetHsm( ),
      statePlayingTransition,
      PRODUCT_CONTROLLER_STATE_PLAYING_TRANSITION_SELECTED );

    auto* statePlayingDeselected = new ProductControllerStatePlayingDeselected
    ( GetHsm( ),
      customStatePlaying,
      PRODUCT_CONTROLLER_STATE_PLAYING_DESELECTED );

    auto* statePlayingSelected = new ProductControllerStatePlayingSelected
    ( GetHsm( ),
      customStatePlaying,
      PRODUCT_CONTROLLER_STATE_PLAYING_SELECTED );

    auto* statePlayingSelectedSilent = new ProductControllerStatePlayingSelectedSilent
    ( GetHsm( ),
      statePlayingSelected,
      PRODUCT_CONTROLLER_STATE_PLAYING_SELECTED_SILENT );

    auto* statePlayingSelectedNotSilent = new ProductControllerStatePlayingSelectedNotSilent
    ( GetHsm( ),
      statePlayingSelected,
      PRODUCT_CONTROLLER_STATE_PLAYING_SELECTED_NOT_SILENT );

    auto* statePlayingSelectedSetup = new ProductControllerStatePlayingSelectedSetup
    ( GetHsm( ),
      statePlayingSelected,
      PRODUCT_CONTROLLER_STATE_PLAYING_SELECTED_SETUP );

    auto* statePlayingSelectedSetupNetwork = new ProductControllerStatePlayingSelectedSetupNetwork
    ( GetHsm( ),
      statePlayingSelectedSetup,
      PRODUCT_CONTROLLER_STATE_PLAYING_SELECTED_SETUP_NETWORK );

    auto* statePlayingSelectedSetupOther = new ProductControllerStatePlayingSelectedSetupOther
    ( GetHsm( ),
      statePlayingSelectedSetup,
      PRODUCT_CONTROLLER_STATE_PLAYING_SELECTED_SETUP_OTHER );

    auto* statePlayingSelectedSetupExiting = new ProductControllerStatePlayingSelectedSetupExiting
    ( GetHsm( ),
      statePlayingSelectedSetup,
      PRODUCT_CONTROLLER_STATE_PLAYING_SELECTED_SETUP_EXITING );

    auto* stateStoppingStreams = new ProductControllerStateStoppingStreams
    ( GetHsm( ),
      statePlayingSelected,
      PRODUCT_CONTROLLER_STATE_STOPPING_STREAMS );

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
    GetHsm( ).AddState( stateTop );
    GetHsm( ).AddState( customStateBooting );
    GetHsm( ).AddState( stateWelcome );
    GetHsm( ).AddState( stateSoftwareUpdateTransition );
    GetHsm( ).AddState( stateSoftwareUpdating );
    GetHsm( ).AddState( stateRebooting );
    GetHsm( ).AddState( stateCriticalError );
    GetHsm( ).AddState( stateFactoryDefault );
    GetHsm( ).AddState( statePlayableTransition );
    GetHsm( ).AddState( statePlayableTransitionIdle );
    GetHsm( ).AddState( statePlayableTransitionNetworkStandby );
    GetHsm( ).AddState( customStateOn );
    GetHsm( ).AddState( customStatePlayable );
    GetHsm( ).AddState( stateNetworkStandby );
    GetHsm( ).AddState( stateNetworkStandbyConfigured );
    GetHsm( ).AddState( stateNetworkStandbyNotConfigured );
    GetHsm( ).AddState( stateLowPowerStandby );
    GetHsm( ).AddState( stateLowPowerStandbyTransition );
    GetHsm( ).AddState( stateIdle );
    GetHsm( ).AddState( stateIdleVoiceConfigured );
    GetHsm( ).AddState( stateIdleVoiceNotConfigured );
    GetHsm( ).AddState( customStatePlaying );
    GetHsm( ).AddState( customStatePlayingActive );
    GetHsm( ).AddState( customStatePlayingInactive );
    GetHsm( ).AddState( customStateAccessoryPairing );
    GetHsm( ).AddState( customStateAdaptIQ );
    GetHsm( ).AddState( statePlayingTransition );
    GetHsm( ).AddState( statePlayingTransitionSelected );
    GetHsm( ).AddState( statePlayingDeselected );
    GetHsm( ).AddState( statePlayingSelected );
    GetHsm( ).AddState( statePlayingSelectedSilent );
    GetHsm( ).AddState( statePlayingSelectedNotSilent );
    GetHsm( ).AddState( statePlayingSelectedSetup );
    GetHsm( ).AddState( statePlayingSelectedSetupNetwork );
    GetHsm( ).AddState( statePlayingSelectedSetupOther );
    GetHsm( ).AddState( statePlayingSelectedSetupExiting );
    GetHsm( ).AddState( stateStoppingStreams );
    GetHsm( ).AddState( stateStoppingStreamsDedicated );
    GetHsm( ).AddState( stateStoppingStreamsDedicatedForFactoryDefault );
    GetHsm( ).AddState( stateStoppingStreamsDedicatedForSoftwareUpdate );

    GetHsm( ).Init( this, PROFESSOR_PRODUCT_CONTROLLER_STATE_BOOTING );

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
    m_ProductSystemManager        = std::make_shared< ProductSystemManager              >( *this );
    m_ProductNetworkManager       = std::make_shared< ProductNetworkManager             >( *this );
    m_ProductCommandLine          = std::make_shared< ProductCommandLine                >( *this );
    m_ProductKeyInputInterface    = std::make_shared< ProductKeyInputInterface          >( *this );
    m_ProductAdaptIQManager       = std::make_shared< ProductAdaptIQManager             >( *this );
    m_ProductAudioService         = std::make_shared< CustomProductAudioService         >( *this, m_FrontDoorClientIF, m_ProductLpmHardwareInterface->GetLpmClient() );

    if( m_ProductLpmHardwareInterface == nullptr ||
        m_ProductSystemManager        == nullptr ||
        m_ProductNetworkManager       == nullptr ||
        m_ProductAudioService         == nullptr ||
        m_ProductCommandLine          == nullptr ||
        m_ProductKeyInputInterface    == nullptr ||
        m_ProductCecHelper            == nullptr ||
        m_ProductAdaptIQManager       == nullptr )
    {
        BOSE_CRITICAL( s_logger, "-------- Product Controller Failed Initialization ----------" );
        BOSE_CRITICAL( s_logger, "A Product Controller module failed to be allocated.         " );

        return;
    }

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
    m_ProductAdaptIQManager      ->Run( );

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
/// @name   ProfessorProductController::GetIntentHandler
///
/// @return This method returns a reference to the IntentHandler instance.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
IntentHandler& ProfessorProductController::GetIntentHandler( )
{
    return m_IntentHandler;
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
/// @name   ProfessorProductController::IsVoiceConfigured
///
/// @return This method returns a true or false value, based on a set member variable.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool ProfessorProductController::IsVoiceConfigured( ) const
{
    return ( m_IsMicrophoneEnabled and m_IsAccountConfigured );
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
/// @name   ProfessorProductController::GetProductType
///
/// @return This method returns the std::string const& value to be used for the Product "Type" field
///
////////////////////////////////////////////////////////////////////////////////////////////////////
std::string const& ProfessorProductController::GetProductType( ) const
{
    static std::string productType = "Professor Soundbar";
    return productType;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProfessorProductController::GetProductColor
///
/// @return This method returns the std::string value to be used for the Product "color" field
///
////////////////////////////////////////////////////////////////////////////////////////////////////
std::string ProfessorProductController::GetProductColor() const
{
    // @TODO https://jirapro.bose.com/browse/PGC-630
    return "BLACK";
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProfessorProductController::GetProductVariant
///
/// @return This method returns the std::string const& value to be used for the Product "Variant" field
///
/// @TODO - Below value may be available through HSP APIs
///
////////////////////////////////////////////////////////////////////////////////////////////////////
std::string const& ProfessorProductController::GetProductVariant( ) const
{
    static std::string productType = "Professor";
    return productType;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProfessorProductController::GetProductModel
///
/// @return This method returns the std::string const& value to be used for the Product "productType" field
///
/// @TODO - Below value may be available through HSP APIs
///
////////////////////////////////////////////////////////////////////////////////////////////////////
std::string const& ProfessorProductController::GetProductModel() const
{
    static std::string productModel = "professor";

    if( auto model = MfgData::Get( "productType" ) )
    {
        productModel =  *model;
    }

    // @TODO PGC-757 replace the manufacturing name with the marketing name.
    return productModel;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProfessorProductController::GetProductDescription
///
/// @return This method returns the std::string const& value to be used for the Product "Description" field
///
/// @TODO - Below value may be available through HSP APIs
///
////////////////////////////////////////////////////////////////////////////////////////////////////
std::string const& ProfessorProductController::GetProductDescription() const
{
    static std::string productDescription = "SoundTouch";

    if( auto description = MfgData::Get( "description" ) )
    {
        productDescription = *description;
    }

    return productDescription;
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
    ProductSTSController::SourceDescriptor descriptor_AiQ  { ProductSTS::SLOT_AIQ,   "ADAPTiQ", false };
    ProductSTSController::SourceDescriptor descriptor_Setup{ ProductSTS::SLOT_SETUP, "SETUP", false };
    ProductSTSController::SourceDescriptor descriptor_TV   { ProductSTS::SLOT_TV,    "TV",      true  };

    sources.push_back( descriptor_AiQ );
    sources.push_back( descriptor_Setup );
    sources.push_back( descriptor_TV );

    Callback< void >
    CallbackForSTSComplete( std::bind( &ProfessorProductController::HandleSTSInitWasComplete,
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
/// @name   ProfessorProductController::HandleSTSInitWasComplete
///
/// @brief  This method is called from the ProductSTSController when all the initially-created
///         sources have been created with CAPS/STS.
///
/// @note   This method is called on the ProductSTSController task.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProfessorProductController::HandleSTSInitWasComplete( )
{
    ProductMessage message;
    message.mutable_stsinterfacestatus( )->set_initialized( true );

    IL::BreakThread( std::bind( &ProfessorProductController::HandleMessage,
                                this,
                                message ),
                     GetTask( ) );
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
    /// Content Audio Playback Services (CAPS) status messages are handled at this point.
    ///////////////////////////////////////////////////////////////////////////////////////////////
    if( message.has_capsstatus( ) )
    {
        if( message.capsstatus( ).has_initialized( ) )
        {
            m_IsCapsReady = message.capsstatus( ).initialized( );
        }
        else
        {
            BOSE_ERROR( s_logger, "An invalid CAPS status message was received." );
            return;
        }

        BOSE_DEBUG( s_logger, "A CAPS Content Audio Playback Services %s message was received.",
                    m_IsCapsReady ? "up" : "down" );

        GetHsm( ).Handle< bool >
        ( &CustomProductControllerState::HandleCapsState, m_IsCapsReady );
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
    /// STS interface status is handled at this point.
    ///////////////////////////////////////////////////////////////////////////////////////////////
    else if( message.has_stsinterfacestatus( ) )
    {
        BOSE_DEBUG( s_logger, "An STS Sources Initialized message was received." );

        m_IsSTSReady = true;

        GetHsm( ).Handle< >
        ( &CustomProductControllerState::HandleSTSSourcesInit );
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
                                                                     m_IsAccountConfigured );

        GetHsm( ).Handle< bool, bool >
        ( &CustomProductControllerState::HandleNetworkState, m_IsNetworkConfigured, m_IsNetworkConnected );
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// Wireless network status messages are handled at this point. Only send information to the
    /// state machine if the wireless network is configured, since a wired network configuration
    /// may be available, and is handle above.
    ///////////////////////////////////////////////////////////////////////////////////////////////
    else if( message.has_wirelessstatus( ) )
    {
        if( message.wirelessstatus( ).has_configured( ) && message.wirelessstatus( ).configured( ) )
        {
            m_IsNetworkConfigured = true;

            m_ProductSystemManager->SetNetworkAccoutConfigurationStatus( m_IsNetworkConfigured,
                                                                         m_IsAccountConfigured );

            GetHsm( ).Handle< bool, bool >
            ( &CustomProductControllerState::HandleNetworkState, m_IsNetworkConfigured, m_IsNetworkConnected );
        }

        ///
        /// Send the frequency information (if available) to the LPM to avoid any frequency
        /// interruption during a speaker Adapt IQ process.
        ///
        if( message.wirelessstatus( ).has_frequencykhz( ) and
            message.wirelessstatus( ).frequencykhz( ) > 0 )
        {
            m_ProductLpmHardwareInterface->SendWiFiRadioStatus( message.wirelessstatus( ).frequencykhz( ) );
        }

        BOSE_DEBUG( s_logger, "A %s wireless network message was received with frequency %d kHz.",
                    message.wirelessstatus( ).configured( ) ? "configured" : "unconfigured",
                    message.wirelessstatus( ).has_frequencykhz( ) ?
                    message.wirelessstatus( ).frequencykhz( ) : 0 );
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Voice messages for the Virtual Personal Assistant or VPA are handled at this point.          ///
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    else if( message.has_voicestatus( ) )
    {
        if( message.voicestatus( ).has_microphoneenabled( ) )
        {
            m_IsMicrophoneEnabled = message.voicestatus( ).microphoneenabled( );
        }
        else
        {
            BOSE_ERROR( s_logger, "An invalid voice status message for the microphone status was received." );
            return;
        }

        if( message.voicestatus( ).has_accountconfigured( ) )
        {
            m_IsAccountConfigured = message.voicestatus( ).accountconfigured( );
        }
        else
        {
            BOSE_ERROR( s_logger, "An invalid voice status message for account configuration was received." );
            return;
        }

        GetHsm( ).Handle< bool >
        ( &CustomProductControllerState::HandleVoiceState, IsVoiceConfigured( ) );
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
            GetHsm( ).Handle< KeyHandlerUtil::ActionType_t >( &CustomProductControllerState::HandleIntentUserPower,
                                                              message.action( ) );
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
    m_ProductAdaptIQManager      ->Stop( );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProfessorProductController::End
///
/// @brief  This method is called when the Product Controller process ends. It is used to set the
///         running member to false, which will invoke the Wait method idle loop to exit and perform
///         any necessary clean up.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProfessorProductController::End( )
{
    BOSE_DEBUG( s_logger, "The Product Controller main task is stopping." );

    m_Running = false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProfessorProductController::GetDefaultProductName
///
/// @brief  This method is to get the default product name by reading from mac address.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
std::string const& ProfessorProductController::GetDefaultProductName( ) const
{
    static std::string productName = "Bose ";
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

void ProfessorProductController::SendInitialCapsData()
{
    BOSE_INFO( s_logger, __func__ );

    // Do the Common stuff first
    ProductController::SendInitialCapsData();

    // PUT /system/sources::properties
    SoundTouchInterface::Sources message;
    auto messageProperties = message.mutable_properties();

    for( uint32_t activationKey = SystemSourcesProperties::ACTIVATION_KEY__MIN; activationKey <= SystemSourcesProperties::ACTIVATION_KEY__MAX; ++activationKey )
    {
        messageProperties->add_supportedactivationkeys(
            SystemSourcesProperties::ACTIVATION_KEY__Name( static_cast<SystemSourcesProperties::ACTIVATION_KEY_>( activationKey ) ) );
    }
    messageProperties->set_activationkeyrequired( true );

#if 0 // @TODO CASTLE-6801 field deviceType missing in SoundTouchInterface::Sources
    for( uint32_t deviceType = SystemSourcesProperties::DEVICE_TYPE__MIN; deviceType <= SystemSourcesProperties::DEVICE_TYPE__MAX; ++deviceType )
    {
        messageProperties->add_supporteddevicetypes(
            SystemSourcesProperties::DEVICE_TYPE__Name( static_cast<SystemSourcesProperties::DEVICE_TYPE_>( deviceType ) ) );
    }
    messageProperties->set_devicetyperequired( true );
#endif

    messageProperties->add_supportedinputroutes( SystemSourcesProperties::INPUT_ROUTE_HDMI__Name( SystemSourcesProperties::INPUT_ROUTE_TV ) );
    messageProperties->set_inputrouterequired( true );

    BOSE_VERBOSE( s_logger, "%s sending %s", __func__, ProtoToMarkup::ToJson( message ).c_str() );

    GetFrontDoorClient()->SendPut<SoundTouchInterface::NowPlaying, EndPointsError::Error>
    ( FRONTDOOR_SYSTEM_SOURCES_API, message, {}, m_errorCb );
}


////////////////////////////////////////////////////////////////////////////////////////////////////
///                           End of the Product Application Namespace                           ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
