////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file      ProfessorProductController.cpp
///
/// @brief     This file contains source code that implements the ProfessorProductController class
///            that acts as a container to handle all the main functionality related to this program
///            that is product specific. In these regards, this class is used as a container to
///            control the product states, as well as to instantiate subclasses to manage the device
///            and lower level hardware, and interface with the user and system level applications.
///
/// @author    Stuart J. Lumby
///
/// @date      09/22/2017
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
#include <chrono>
#include <thread>
#include <unistd.h>
#include "SystemUtils.h"
#include "DPrint.h"
#include "KeyActions.h"
#include "ProductController.h"
#include "ProfessorProductController.h"
#include "ProductHardwareInterface.h"
#include "ProductAudioService.h"
#include "ProductSoftwareServices.h"
#include "ProductUserInterface.h"
#include "ProductNetworkManager.h"
#include "ProductSystemManager.h"
#include "ProductCommandLine.h"
#include "ProtoPersistenceFactory.h"
#include "ProductMessage.pb.h"
#include "NetManager.pb.h"
#include "Callback.h"
#include "ProductEdidInterface.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
///                         Start of Product Application Namespace                               ///
////////////////////////////////////////////////////////////////////////////////////////////////////
namespace ProductApp
{

////////////////////////////////////////////////////////////////////////////////////////////////////
///
///            Constant Definitions
///
////////////////////////////////////////////////////////////////////////////////////////////////////
constexpr const uint32_t PRODUCT_CONTROLLER_RUNNING_CHECK_IN_SECONDS = ( 4 );

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief The following aliases refer to the Bose Sound Touch class utilities for inter-process and
///        inter-thread communications.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
typedef APClientSocketListenerIF::ListenerPtr   ClientPointer;
typedef APServerSocketListenerIF::ListenerPtr   ServerPointer;
typedef APClientSocketListenerIF::SocketPtr     ClientSocket;
typedef APServerSocketListenerIF::SocketPtr     ServerSocket;
typedef IPCMessageRouterIF::IPCMessageRouterPtr RouterPointer;

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// The following declares a DPrint class type object and a standard string for logging information
/// in this source code file.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
static const DPrint s_logger { "Product" };

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// The following declares a list of strings whose index corresponding to the key action enumeration.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
std::string KeyAction[ ] =
{
    "",
    "KEY_ACTION_POWER",
    "KEY_ACTION_SOURCE",
    "KEY_ACTION_PRESET_1",
    "KEY_ACTION_PRESET_2",
    "KEY_ACTION_PRESET_3",
    "KEY_ACTION_PRESET_4",
    "KEY_ACTION_PRESET_5",
    "KEY_ACTION_PRESET_6",
    "KEY_ACTION_VOLUME_UP",
    "KEY_ACTION_VOLUME_DOWN",
    "KEY_ACTION_PLAY_PAUSE",
    "KEY_ACTION_SKIP_FORWARD",
    "KEY_ACTION_SKIP_BACK",
    "KEY_ACTION_MUTE",
    "KEY_ACTION_SOUNDTOUCH",
    "KEY_ACTION_CONNECT",
    "KEY_ACTION_ACTION",
    "KEY_ACTION_TV",
    "KEY_ACTION_THUMB_UP",
    "KEY_ACTION_THUMB_DOWN",
    "KEY_ACTION_FACTORY_DEFAULT",
    "KEY_ACTION_WIFI_OFF",
    "KEY_ACTION_AP_SETUP",
    "KEY_ACTION_PAIR_SPEAKERS"
};

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProfessorProductController::GetInstance
///
/// @brief  This static method creates the one and only instance of a ProfessorProductController object.
///         The C++ Version 11 compiler guarantees that only one instance is created in a thread
///         safe way.
///
/// @param  void This method does not take any arguments.
///
/// @return This method returns a pointer to a ProfessorProductController object.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
ProfessorProductController* ProfessorProductController::GetInstance( )
{
    static ProfessorProductController* instance = new ProfessorProductController( );

    BOSE_DEBUG( s_logger, "The instance %8p of the Product Controller was returned.", instance );

    return instance;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProfessorProductController::ProfessorProductController
///
/// @brief  This method is the ProfessorProductController constructor, which is declared as being
///         private to ensure that only one instance of this class can be created through the class
///         GetInstance method.
///
/// @param  void This method does not take any arguments.
///
/// @return This method does not return anything.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
ProfessorProductController::ProfessorProductController( ) :

    ///
    /// Construction of the Inherited Classes
    ///
    ProductController( "Professor" ),

    ///
    /// Construction of the State Machine
    ///
    m_ProductControllerStateMachine( GetTask( ),
                                     "ProfessorStateMachine",
                                     static_cast< ProductController & >( *this ) ),
    ///
    /// Create the top state for the product controller, which all other states are sub-states.
    ///
    m_ProductControllerStateTop( m_ProductControllerStateMachine,
                                 nullptr,
                                 static_cast< ProductApp::ProductController & >( *this ) ),

    ///
    /// The following states are all immediate sub-states of the top state.
    ///
    m_ProductControllerStateSetup( m_ProductControllerStateMachine,
                                   &m_ProductControllerStateTop,
                                   static_cast< ProductApp::ProductController & >( *this ) ),

    m_CustomProductControllerStateBooting( m_ProductControllerStateMachine,
                                           &m_ProductControllerStateTop,
                                           *this ),

    m_CustomProductControllerStateUpdatingSoftware( m_ProductControllerStateMachine,
                                                    &m_ProductControllerStateTop,
                                                    *this ),

    m_CustomProductControllerStateOff( m_ProductControllerStateMachine,
                                       &m_ProductControllerStateTop,
                                       *this ),

    m_CustomProductControllerStateOn( m_ProductControllerStateMachine,
                                      &m_ProductControllerStateTop,
                                      *this ),

    ///
    /// The following playable state is a sub-state of the on state. Substates to the playable
    /// state include the network standby and idle state, along with their lower sub-states.
    ///
    m_CustomProductControllerStatePlayable( m_ProductControllerStateMachine,
                                            &m_CustomProductControllerStateOn,
                                            *this ),

    m_CustomProductControllerStateNetworkStandby( m_ProductControllerStateMachine,
                                                  &m_CustomProductControllerStatePlayable,
                                                  *this ),

    m_CustomProductControllerStateNetworkStandbyConfigured( m_ProductControllerStateMachine,
                                                            &m_CustomProductControllerStateNetworkStandby,
                                                            *this ),

    m_CustomProductControllerStateNetworkStandbyUnconfigured( m_ProductControllerStateMachine,
                                                              &m_CustomProductControllerStateNetworkStandby,
                                                              *this ),

    m_CustomProductControllerStateIdle( m_ProductControllerStateMachine,
                                        &m_CustomProductControllerStatePlayable,
                                        *this ),

    m_CustomProductControllerStateIdleVoiceConfigured( m_ProductControllerStateMachine,
                                                       &m_CustomProductControllerStateIdle,
                                                       *this ),

    m_CustomProductControllerStateIdleVoiceUnconfigured( m_ProductControllerStateMachine,
                                                         &m_CustomProductControllerStateIdle,
                                                         *this ),

    ///
    /// The following playing state is a sub-state of the on state. Substates to the playable
    /// state include the playing active and inactive state.
    ///
    m_CustomProductControllerStatePlaying( m_ProductControllerStateMachine,
                                           &m_CustomProductControllerStateOn,
                                           *this ),

    m_CustomProductControllerStatePlayingActive( m_ProductControllerStateMachine,
                                                 &m_CustomProductControllerStatePlaying,
                                                 *this ),

    m_CustomProductControllerStatePlayingInactive( m_ProductControllerStateMachine,
                                                   &m_CustomProductControllerStatePlaying,
                                                   *this ),

    ///
    /// Construction of the Product Controller Modules
    ///
    m_ProductHardwareInterface( nullptr ),
    m_ProductSystemManager( nullptr ),
    m_ProductNetworkManager( nullptr ),
    m_ProductAudioService( nullptr ),
    m_ProductSoftwareServices( nullptr ),
    m_ProductCommandLine( nullptr ),
    m_ProductUserInterface( nullptr ),

    ///
    /// Member Variable Initialization
    ///
    m_IsLpmReady( false ),
    m_IsCapsReady( false ),
    m_IsAudioPathReady( false ),
    m_IsSTSReady( false ),
    m_IsNetworkConfigured( false ),
    m_IsNetworkConnected( false ),
    m_IsAutoWakeEnabled( false ),
    m_IsAccountConfigured( false ),
    m_IsMicrophoneEnabled( false ),
    m_IsSoftwareUpdateRequired( false ),
    m_Running( false )
{
    return;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProfessorProductController::Run
///
/// @brief
///
/// @param  void This method does not take any arguments.
///
/// @return This method does not return anything.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProfessorProductController::Run( )
{
    m_Running = true;

    BOSE_DEBUG( s_logger, "---------- - Product Controller Starting Modules ------------" );
    BOSE_DEBUG( s_logger, "The Professor Product Controller is starting up its modules." );

    ///
    /// Start the Product Controller state machine.
    ///
    m_ProductControllerStateMachine.AddState( &m_ProductControllerStateTop );
    m_ProductControllerStateMachine.AddState( &m_CustomProductControllerStateBooting );
    m_ProductControllerStateMachine.AddState( &m_CustomProductControllerStateUpdatingSoftware );
    m_ProductControllerStateMachine.AddState( &m_CustomProductControllerStateOff );
    m_ProductControllerStateMachine.AddState( &m_CustomProductControllerStateOn );
    m_ProductControllerStateMachine.AddState( &m_CustomProductControllerStatePlayable );
    m_ProductControllerStateMachine.AddState( &m_CustomProductControllerStateNetworkStandby );
    m_ProductControllerStateMachine.AddState( &m_CustomProductControllerStateNetworkStandbyConfigured );
    m_ProductControllerStateMachine.AddState( &m_CustomProductControllerStateNetworkStandbyUnconfigured );
    m_ProductControllerStateMachine.AddState( &m_CustomProductControllerStateIdle );
    m_ProductControllerStateMachine.AddState( &m_CustomProductControllerStateIdleVoiceConfigured );
    m_ProductControllerStateMachine.AddState( &m_CustomProductControllerStateIdleVoiceUnconfigured );
    m_ProductControllerStateMachine.AddState( &m_CustomProductControllerStatePlaying );
    m_ProductControllerStateMachine.AddState( &m_CustomProductControllerStatePlayingActive );
    m_ProductControllerStateMachine.AddState( &m_CustomProductControllerStatePlayingInactive );

    m_ProductControllerStateMachine.Init( PROFESSOR_PRODUCT_CONTROLLER_STATE_BOOTING );

    ///
    /// Get instances of all the subprocesses.
    ///
    Callback < ProductMessage > CallbackForMessages( std::bind( &ProfessorProductController::HandleMessage,
                                                                this,
                                                                std::placeholders::_1 ) );

    m_ProductHardwareInterface = ProductHardwareInterface::GetInstance( GetTask( ),
                                                                        CallbackForMessages );
                                                                        
    m_ProductEdidInterface     = ProductEdidInterface::GetInstance( GetTask( ),
                                                                    CallbackForMessages,
                                                                    m_ProductHardwareInterface );
                                                                    
    m_ProductSystemManager     = ProductSystemManager    ::GetInstance( GetTask( ),
                                                                        CallbackForMessages );

    m_ProductNetworkManager    = ProductNetworkManager   ::GetInstance( GetTask( ),
                                                                        CallbackForMessages );

    m_ProductAudioService      = ProductAudioService     ::GetInstance( GetTask( ),
                                                                        CallbackForMessages );

    m_ProductSoftwareServices  = ProductSoftwareServices ::GetInstance( GetTask( ),
                                                                        CallbackForMessages,
                                                                        m_ProductHardwareInterface );
    m_ProductCommandLine       = ProductCommandLine      ::GetInstance( GetTask( ),
                                                                        CallbackForMessages,
                                                                        m_ProductHardwareInterface );

    m_ProductUserInterface     = ProductUserInterface    ::GetInstance( GetTask( ),
                                                                        CallbackForMessages,
                                                                        m_ProductHardwareInterface,
                                                                        m_CliClientMT );

    if( m_ProductHardwareInterface == nullptr ||
        m_ProductSystemManager     == nullptr ||
        m_ProductNetworkManager    == nullptr ||
        m_ProductAudioService      == nullptr ||
        m_ProductSoftwareServices  == nullptr ||
        m_ProductCommandLine       == nullptr ||
        m_ProductUserInterface     == nullptr ||
        m_ProductEdidInterface     == nullptr )
    {
        BOSE_DEBUG( s_logger, "-------- Product Controller Failed Initialization ----------" );
        BOSE_DEBUG( s_logger, "A Product Controller module failed to be allocated.         " );

        return;
    }

    ///
    /// Run all the submodules.
    ///
    m_ProductHardwareInterface ->Run( );
    m_ProductSystemManager     ->Run( );
    m_ProductNetworkManager    ->Run( );
    m_ProductAudioService      ->Run( );
    m_ProductSoftwareServices  ->Run( );
    m_ProductCommandLine       ->Run( );
    m_ProductUserInterface     ->Run( );
    m_ProductEdidInterface     ->Run( );

    ///
    /// Set up the STSProductController
    ///
    SetupProductSTSConntroller( );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief ProfessorProductController::IsBooted
///
/// @return
///
////////////////////////////////////////////////////////////////////////////////////////////////////
ProductHardwareInterface* ProfessorProductController::GetHardwareInterface( )
{
    return m_ProductHardwareInterface;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief ProfessorProductController::IsBooted
///
/// @return
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool ProfessorProductController::IsBooted( )
{
    BOSE_DEBUG( s_logger, "------------ Product Controller Booted Check ---------------" );
    BOSE_DEBUG( s_logger, " " );
    BOSE_DEBUG( s_logger, "LPM Connected        :  %s", ( m_IsLpmReady       ? "true" : "false" ) );
    BOSE_DEBUG( s_logger, "CAPS Initialized     :  %s", ( m_IsCapsReady      ? "true" : "false" ) );
    BOSE_DEBUG( s_logger, "Audio Path Connected :  %s", ( m_IsAudioPathReady ? "true" : "false" ) );
    BOSE_DEBUG( s_logger, "STS Initialized      :  %s", ( m_IsSTSReady       ? "true" : "false" ) );
    BOSE_DEBUG( s_logger, " " );

    return ( m_IsLpmReady and m_IsCapsReady and m_IsAudioPathReady and m_IsSTSReady );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief ProfessorProductController::IsNetworkConfigured
///
/// @return
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool ProfessorProductController::IsNetworkConfigured( )
{
    return m_IsNetworkConfigured;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief ProfessorProductController::IsNetworkConfigured
///
/// @return
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool ProfessorProductController::IsNetworkConnected( )
{
    return m_IsNetworkConnected;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief ProfessorProductController::IsAutoWakeEnabled
///
/// @return
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool ProfessorProductController::IsAutoWakeEnabled( )
{
    return m_IsAutoWakeEnabled;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief ProfessorProductController::IsVoiceConfigured
///
/// @return
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool ProfessorProductController::IsVoiceConfigured( )
{
    return ( m_IsMicrophoneEnabled and m_IsAccountConfigured );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief ProfessorProductController::IsSoftwareUpdateRequired
///
/// @return
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool ProfessorProductController::IsSoftwareUpdateRequired( )
{
    return m_IsSoftwareUpdateRequired;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProfessorProductController::SetupProductSTSConntroller
///
/// @brief  This method is called to perform the needed initialization of the ProductSTSController,
///         specifically, provide the set of sources to be created initially.
///
/// @param  void This method does not take any arguments.
///
/// @return This method does not return anything.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProfessorProductController::SetupProductSTSConntroller( void )
{
    std::vector<ProductSTSController::SourceDescriptor> sources;

    ///
    /// Adapt IQ is not available as a normal source, whereas the TV source will always be available.
    ///
    ProductSTSController::SourceDescriptor descriptor_AiQ{ ProductSTS::SLOT_AIQ, "ADAPTiQ", false };
    ProductSTSController::SourceDescriptor descriptor_TV { ProductSTS::SLOT_TV,  "TV",      true  };

    sources.push_back( descriptor_AiQ );
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
/// @note   THIS METHOD IS CALLED ON THE ProductSTSController THREAD
///
/// @param  void This method does not take any arguments.
///
/// @return This method does not return anything.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProfessorProductController::HandleSTSInitWasComplete( void )
{
    ProductMessage message;
    message.mutable_selectsourcestatus( )->set_initialized( true );

    IL::BreakThread( std::bind( &ProfessorProductController::HandleMessage,
                                this,
                                message ),
                     GetTask( ) );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProfessorProductController::HandleSelectSourceSlot
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
/// @name   ProfessorProductController::HandleMessage
///
/// @brief  This method is called to handle product controller messages, which are sent from the
///         more product specific class instances, and is used to process the state machine for the
///         product.
///
/// @param  ProductMessage& This argument contains product message event information based on the
///                         ProductMessage Protocal Buffer.
///
/// @return This method does not return anything.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProfessorProductController::HandleMessage( const ProductMessage& message )
{
    BOSE_DEBUG( s_logger, "----------- Product Controller Message Handler -------------" );

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// LPM status messages are handled at this point.
    ///////////////////////////////////////////////////////////////////////////////////////////////
    if( message.has_lpmstatus( ) )
    {
        if( message.lpmstatus( ).connected( ) )
        {
            BOSE_DEBUG( s_logger, "An LPM Hardware up message was received." );

            m_IsLpmReady = true;

            m_ProductControllerStateMachine.Handle< bool >
            ( &CustomProductControllerState::HandleLpmState, true );
        }
        else
        {
            BOSE_DEBUG( s_logger, "An LPM Hardware down message was received." );

            m_IsLpmReady = false;

            m_ProductControllerStateMachine.Handle< bool >
            ( &CustomProductControllerState::HandleLpmState, false );
        }
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// CAPS status messages are handled at this point.
    ///////////////////////////////////////////////////////////////////////////////////////////////
    else if( message.has_capsstatus( ) )
    {
        if( message.capsstatus( ).initialized( ) )
        {
            BOSE_DEBUG( s_logger, "A CAPS Content Audio Playback Services up message was received." );

            m_IsCapsReady = true;

            m_ProductControllerStateMachine.Handle< bool >
            ( &CustomProductControllerState::HandleCapsState, true );
        }
        else
        {
            BOSE_DEBUG( s_logger, "A CAPS Content Audio Playback Services down message was received." );

            m_IsCapsReady = false;

            m_ProductControllerStateMachine.Handle< bool >
            ( &CustomProductControllerState::HandleCapsState, false );
        }
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// CAPS status messages are handled at this point.
    ///////////////////////////////////////////////////////////////////////////////////////////////
    else if( message.has_audiopathstatus( ) )
    {
        if( message.audiopathstatus( ).connected( ) )
        {
            BOSE_DEBUG( s_logger, "An Audio Path services up message was received." );

            m_IsAudioPathReady = true;

            m_ProductControllerStateMachine.Handle< bool >
            ( &CustomProductControllerState::HandleAudioPathState, true );
        }
        else
        {
            BOSE_DEBUG( s_logger, "An Audio Path services down message was received." );

            m_IsAudioPathReady = false;

            m_ProductControllerStateMachine.Handle< bool >
            ( &CustomProductControllerState::HandleAudioPathState, false );
        }
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// Source selection status and slot selected data are handled at this point.
    ///////////////////////////////////////////////////////////////////////////////////////////////
    else if( message.has_selectsourcestatus( ) )
    {
        BOSE_DEBUG( s_logger, "An STS Sources Initialized message was received." );

        m_IsSTSReady = true;

        m_ProductControllerStateMachine.Handle<>
        ( &CustomProductControllerState::HandleSTSSourcesInit );
    }
    else if( message.has_selectsourceslot( ) )
    {
        const auto& slot = message.selectsourceslot( ).slot( );

        BOSE_DEBUG( s_logger, "An STS Select message was received for slot %s.",
                    ProductSourceSlot_Name( slot ).c_str( ) );
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// Network status messages are handled at this point.
    ///////////////////////////////////////////////////////////////////////////////////////////////
    else if( message.has_networkstatus( ) )
    {
        m_IsNetworkConfigured = false;
        m_IsNetworkConnected  = false;

        if( message.networkstatus( ).configured( ) )
        {
            m_IsNetworkConfigured = true;
        }

        if( message.networkstatus( ).connected( ) )
        {
            m_IsNetworkConnected = true;
        }

        if( message.networkstatus( ).networktype( ) == ProductNetworkStatus_ProductNetworkType_Wireless )
        {
            BOSE_DEBUG( s_logger, "A wireless %s and %s network message was received.",
                        m_IsNetworkConfigured ? "configured" : "not configured",
                        m_IsNetworkConnected  ? "connected"  : "not connected" );
        }
        else if( message.networkstatus( ).networktype( ) == ProductNetworkStatus_ProductNetworkType_Wired )
        {
            BOSE_DEBUG( s_logger, "A wired %s and %s network message was received.",
                        m_IsNetworkConfigured ? "configured" : "not configured",
                        m_IsNetworkConnected  ? "connected"  : "not connected" );
        }
        else
        {
            BOSE_DEBUG( s_logger, "A unknown %s and %s network message was received.",
                        m_IsNetworkConfigured ? "configured" : "not configured",
                        m_IsNetworkConnected  ? "connected"  : "not connected" );
        }
        m_ProductSystemManager->SetConfigurationStatus( m_IsNetworkConfigured,
                                                        false,
                                                        m_ProductSystemManager->IsSystemLanguageSet( ) );
        m_ProductControllerStateMachine.Handle< bool, bool >
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

            m_ProductSystemManager->SetConfigurationStatus( m_IsNetworkConfigured,
                                                            false,
                                                            m_ProductSystemManager->IsSystemLanguageSet( ) );

            m_ProductControllerStateMachine.Handle< bool, bool >
            ( &CustomProductControllerState::HandleNetworkState, m_IsNetworkConfigured, m_IsNetworkConnected );
        }

        ///
        /// Send the frequency information (if available) to the LPM to avoid any frequency
        /// interruption during a speaker Adapt IQ process.
        ///
        if( message.wirelessstatus( ).frequencykhz( ) > 0 )
        {
            m_ProductHardwareInterface->SendWiFiRadioStatus( message.wirelessstatus( ).frequencykhz( ) );
        }

        BOSE_DEBUG( s_logger, "A %s wireless network message was received with frequency %d kHz.",
                    message.wirelessstatus( ).configured( ) ? "configured" : "unconfigured",
                    message.wirelessstatus( ).frequencykhz( ) );
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
            m_IsMicrophoneEnabled = false;
        }

        if( message.voicestatus( ).has_accountconfigured( ) )
        {
            m_IsAccountConfigured = message.voicestatus( ).accountconfigured( );
        }
        else
        {
            m_IsAccountConfigured = false;
        }

        m_ProductControllerStateMachine.Handle< bool >
        ( &CustomProductControllerState::HandleVoiceState, IsVoiceConfigured( ) );
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// Key data messages are handled at this point.
    ///////////////////////////////////////////////////////////////////////////////////////////////
    else if( message.has_keydata( ) )
    {
        auto keyData = message.keydata( );

        BOSE_DEBUG( s_logger, "The key action value %s( %d ) was received.",
                    KeyAction[ keyData.action( ) ].c_str( ),
                    keyData.action( ) );

        m_ProductControllerStateMachine.Handle< int >
        ( &CustomProductControllerState::HandleKeyAction, keyData.action( ) );
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// Power key messages are handled at this point. Whether the power is to be changed is
    /// determined by the currently active state.
    ///////////////////////////////////////////////////////////////////////////////////////////////
    else if( message.has_power( ) )
    {
        BOSE_DEBUG( s_logger, "A power message has been received." );

        m_ProductControllerStateMachine.Handle< >
        ( &CustomProductControllerState::HandlePowerState );
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
            m_IsAutoWakeEnabled = false;
        }

        m_ProductControllerStateMachine.Handle< bool >
        ( &CustomProductControllerState::HandleAutowakeStatus, m_IsAutoWakeEnabled );
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// Unknown message types are handled at this point.
    ///////////////////////////////////////////////////////////////////////////////////////////////
    else
    {
        BOSE_DEBUG( s_logger, "An unknown message type was received." );
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
        BOSE_DEBUG( s_logger, "A check was made to determine if the Product Controller is running." );

        sleep( PRODUCT_CONTROLLER_RUNNING_CHECK_IN_SECONDS );
    }

    ///
    /// Stop all the submodules.
    ///
    m_ProductHardwareInterface->Stop( );
    m_ProductSystemManager    ->Stop( );
    m_ProductNetworkManager   ->Stop( );
    m_ProductSoftwareServices ->Stop( );
    m_ProductCommandLine      ->Stop( );
    m_ProductUserInterface    ->Stop( );

    ///
    /// Delete all the submodules.
    ///
    delete m_ProductHardwareInterface;
    delete m_ProductSystemManager;
    delete m_ProductNetworkManager;
    delete m_ProductAudioService;
    delete m_ProductSoftwareServices;
    delete m_ProductCommandLine;
    delete m_ProductUserInterface;

    m_ProductHardwareInterface = nullptr;
    m_ProductSystemManager = nullptr;
    m_ProductNetworkManager = nullptr;
    m_ProductAudioService = nullptr;
    m_ProductSoftwareServices = nullptr;
    m_ProductCommandLine = nullptr;
    m_ProductUserInterface = nullptr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProfessorProductController::End
///
/// @brief  This method is called when the Product Controller process ends. It is used to stop the
///         main task.
///
/// @param  void This method does not take any arguments.
///
/// @return This method does not return anything.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProfessorProductController::End( )
{
    BOSE_DEBUG( s_logger, "The Product Controller main task is stopping." );

    m_Running = false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                         End of Product Application Namespace                                 ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
