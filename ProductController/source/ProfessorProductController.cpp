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
#include "Callback.h"
#include "DPrint.h"
#include "CliClient.h"
#include "ProductController.h"
#include "ProfessorProductController.h"
#include "ProductHardwareInterface.h"
#include "ProductAudioService.h"
#include "ProductSoftwareServices.h"
#include "ProductUserInterface.h"
#include "ProductFrontDoorNetwork.h"
#include "ProductCommandLine.h"
#include "ProtoPersistenceFactory.h"
#include "ProductMessage.pb.h"
#include "NetManager.pb.h"
#include "ProductControllerStateTop.h"
#include "ProductControllerStateSetup.h"
#include "ProductControllerStateOn.h"
#include "CustomProductControllerStateBooting.h"
#include "CustomProductControllerStateNetworkStandby.h"
#include "CustomProductControllerStateIdle.h"
#include "CustomProductControllerStateUpdating.h"
#include "CustomProductControllerState.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
///                             Start of Product Namespace                                       ///
////////////////////////////////////////////////////////////////////////////////////////////////////
namespace ProductApp
{

////////////////////////////////////////////////////////////////////////////////////////////////////
///
///            Definitions
///
////////////////////////////////////////////////////////////////////////////////////////////////////
#define PRODUCT_CONTROLLER_RUNNING_CHECK_IN_SECONDS ( 5 )

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
///            Global Constants
///
////////////////////////////////////////////////////////////////////////////////////////////////////
const std::string g_ProductDirectory = "product-persistence/";

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// The following declares a DPrint class type object and a standard string for logging information
/// in this source code file.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
static const DPrint s_logger { "Product" };

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
/// @brief  This method is the ProfessorProductController constructor, which is declared as being private to
///         ensure that only one instance of this class can be created through the class GetInstance
///         method.
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
                                     static_cast< ProductController& >( *this ) ),
    ///
    /// Construction of the Common States
    ///
    m_ProductControllerStateTop                (  m_ProductControllerStateMachine,
                                                  nullptr,
                                                  static_cast< ProductApp::ProductController& >( *this ) ),
    m_ProductControllerStateSetup              (  m_ProductControllerStateMachine,
                                                 &m_ProductControllerStateTop,
                                                 static_cast< ProductApp::ProductController& >( *this ) ),
    m_ProductControllerStateOn                 (  m_ProductControllerStateMachine,
                                                 &m_ProductControllerStateTop,
                                                 static_cast< ProductController& >( *this ) ),
    ///
    /// Construction of the Custom Professor States
    ///
    m_CustomProductControllerStateBooting       ( m_ProductControllerStateMachine,
                                                  &m_ProductControllerStateTop,
                                                  *this ),
    m_CustomProductControllerStateNetworkStandby( m_ProductControllerStateMachine,
                                                  &m_ProductControllerStateTop,
                                                  *this ),
    m_CustomProductControllerStateIdle          ( m_ProductControllerStateMachine,
                                                  &m_ProductControllerStateTop,
                                                  *this ),
    m_CustomProductControllerStateUpdating      ( m_ProductControllerStateMachine,
                                                  &m_ProductControllerStateTop,
                                                  *this ),
    ///
    /// Member Variable Initialization
    ///
    m_LanguageSettingsPersistentStorage   ( ProtoPersistenceFactory::Create( "ProductLanguage",
                                                                              g_ProductDirectory ) ),
    m_ConfigurationStatusPersistentStorage( ProtoPersistenceFactory::Create( "ConfigurationStatus",
                                                                              g_ProductDirectory ) ),
    m_IsLpmReady      ( false ),
    m_IsCapsReady     ( false ),
    m_IsAudioPathReady( false ),
    m_IsNetworkReady  ( false ),
    m_IsSTSReady      ( false )
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
     m_running = true;

     BOSE_DEBUG( s_logger, "------------- Product Controller Starting Modules ------------" );
     BOSE_DEBUG( s_logger, "The Professor Product Controller is starting up its processes." );

     ///
     /// Start the Product Controller state machine.
     ///
     m_ProductControllerStateMachine.AddState( &m_ProductControllerStateTop                  );
     m_ProductControllerStateMachine.AddState( &m_ProductControllerStateSetup                );
     m_ProductControllerStateMachine.AddState( &m_ProductControllerStateOn                   );
     m_ProductControllerStateMachine.AddState( &m_CustomProductControllerStateBooting        );
     m_ProductControllerStateMachine.AddState( &m_CustomProductControllerStateNetworkStandby );
     m_ProductControllerStateMachine.AddState( &m_CustomProductControllerStateIdle           );
     m_ProductControllerStateMachine.AddState( &m_CustomProductControllerStateUpdating       );

     m_ProductControllerStateMachine.Init( PROFESSOR_PRODUCT_CONTROLLER_STATE_BOOTING );

     ///
     /// Get instances of all the subprocesses.
     ///
     Callback < ProductMessage > CallbackForMessages( std::bind( &ProfessorProductController::HandleMessage,
                                                                 this,
                                                                 std::placeholders::_1 ) );

     m_ProductHardwareInterface = ProductHardwareInterface::GetInstance( GetTask( ),
                                                                         CallbackForMessages );
     m_ProductFrontDoorNetwork  = ProductFrontDoorNetwork::GetInstance ( GetTask( ),
                                                                         CallbackForMessages );
     m_ProductAudioService      = ProductAudioService::GetInstance     ( GetTask( ),
                                                                         CallbackForMessages);
     m_ProductSoftwareServices  = ProductSoftwareServices::GetInstance ( GetTask( ),
                                                                         CallbackForMessages,
                                                                         m_ProductHardwareInterface );
     m_ProductUserInterface     = ProductUserInterface::GetInstance    ( GetTask( ),
                                                                         CallbackForMessages,
                                                                    m_ProductHardwareInterface,
                                                                    m_CliClientMT );
     m_ProductCommandLine       = ProductCommandLine::GetInstance      ( GetTask( ),
                                                                         m_ProductHardwareInterface );

     ///
     /// Run all the submodules.
     ///
     m_ProductHardwareInterface->Run( );
     m_ProductAudioService     ->Run( );
     m_ProductSoftwareServices ->Run( );
     m_ProductUserInterface    ->Run( );
     m_ProductCommandLine      ->Run( );
     m_ProductFrontDoorNetwork ->Run( );

     ///
     /// Read the language settings and configuration status from persistent storage.
     ///
     ReadLanguageSettingsFromPersistentStorage   ( );
     ReadConfigurationStatusFromPersistentStorage( );

     ///
     /// Set up the STSProductController
     ///
     SetupProductSTSConntroller();

     ///
     /// Send the language settings and configuration status to the Front Door Network.
     ///
     ProductMessage productMessage;

     productMessage.set_id( SYSTEM_LANGUAGE_CHANGE );
     productMessage.mutable_data( )->mutable_languagedata( )->set_systemlanguage( GetSystemLanguageCode( ) );

     m_ProductFrontDoorNetwork->HandleMessage( productMessage );

     bool networkStatus  = m_ConfigurationStatus.mutable_status( )->network ( );
     bool languageStatus = m_ConfigurationStatus.mutable_status( )->language( );
     bool accountStatus  = m_ConfigurationStatus.mutable_status( )->account ( );

     productMessage.set_id( CONFIGURATION_STATUS );
     productMessage.mutable_data( )->mutable_configurationstatus( )->set_network ( networkStatus  );
     productMessage.mutable_data( )->mutable_configurationstatus( )->set_language( languageStatus );
     productMessage.mutable_data( )->mutable_configurationstatus( )->set_account ( accountStatus  );

     m_ProductFrontDoorNetwork->HandleMessage( productMessage );
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
     return ( m_IsLpmReady and m_IsCapsReady and m_IsAudioPathReady and m_IsSTSReady );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief  ProfessorProductController::GetNetworkStatus
/// @return
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool ProfessorProductController::GetNetworkStatus( )
{
     if( m_ConfigurationStatus.mutable_status( )->has_network ( ) )
     {
         return m_ConfigurationStatus.mutable_status( )->network ( );
     }
     else
     {
         return false;
     }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief  ProfessorProductController::GetSystemLanguageCode
/// @return
///
////////////////////////////////////////////////////////////////////////////////////////////////////
std::string ProfessorProductController::GetSystemLanguageCode( )
{
     if( m_LanguageSettings.has_code( ) )
     {
         return m_LanguageSettings.code( );
     }
     else
     {
         return std::string( );
     }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief ProfessorProductController::SetSystemLanguageCode
/// @param systemLanguageString
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProfessorProductController::SetSystemLanguageCode( std::string systemLanguageString )
{
     m_LanguageSettings.set_code( systemLanguageString );
     WriteLanguageSettingsToPersistentStorage( );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief ProfessorProductController::ReadLanguageSettingsFromPersistentStorage
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProfessorProductController::ReadLanguageSettingsFromPersistentStorage( )
{
     try
     {
         BOSE_DEBUG( s_logger, "Language settings are being read from persistent storage." );

         std::string storageString = m_LanguageSettingsPersistentStorage->Load( );
         ProtoToMarkup::FromJson( storageString, &m_LanguageSettings );
     }
     catch( ... )
     {
         BOSE_LOG( ERROR, "Reading language settings from persistent storage failed."        );
         BOSE_LOG( ERROR, "Default language settings will be written to persistent storage." );

         ///
         /// Set the default language to English and add the supported languages if this data has not
         /// been set in storage.
         ///
         m_LanguageSettings.set_code( "en" );
         m_LanguageSettings.mutable_properties( )->add_supported_language_codes( "da" ); /// Danish
         m_LanguageSettings.mutable_properties( )->add_supported_language_codes( "de" ); /// German
         m_LanguageSettings.mutable_properties( )->add_supported_language_codes( "en" ); /// English
         m_LanguageSettings.mutable_properties( )->add_supported_language_codes( "es" ); /// Spanish
         m_LanguageSettings.mutable_properties( )->add_supported_language_codes( "fr" ); /// French
         m_LanguageSettings.mutable_properties( )->add_supported_language_codes( "it" ); /// Italian
         m_LanguageSettings.mutable_properties( )->add_supported_language_codes( "nl" ); /// Dutch
         m_LanguageSettings.mutable_properties( )->add_supported_language_codes( "sv" ); /// Swedish
         m_LanguageSettings.mutable_properties( )->add_supported_language_codes( "ja" ); /// Japanese
         m_LanguageSettings.mutable_properties( )->add_supported_language_codes( "zh" ); /// Chinese
         m_LanguageSettings.mutable_properties( )->add_supported_language_codes( "ko" ); /// Korean
         m_LanguageSettings.mutable_properties( )->add_supported_language_codes( "th" ); /// Thai
         m_LanguageSettings.mutable_properties( )->add_supported_language_codes( "cs" ); /// Czechoslovakian
         m_LanguageSettings.mutable_properties( )->add_supported_language_codes( "fi" ); /// Finnish
         m_LanguageSettings.mutable_properties( )->add_supported_language_codes( "el" ); /// Greek
         m_LanguageSettings.mutable_properties( )->add_supported_language_codes( "no" ); /// Norwegian
         m_LanguageSettings.mutable_properties( )->add_supported_language_codes( "pl" ); /// Polish
         m_LanguageSettings.mutable_properties( )->add_supported_language_codes( "pt" ); /// Portuguese
         m_LanguageSettings.mutable_properties( )->add_supported_language_codes( "ro" ); /// Romanian
         m_LanguageSettings.mutable_properties( )->add_supported_language_codes( "ru" ); /// Russian
         m_LanguageSettings.mutable_properties( )->add_supported_language_codes( "sl" ); /// Slovenian
         m_LanguageSettings.mutable_properties( )->add_supported_language_codes( "tr" ); /// Turkish
         m_LanguageSettings.mutable_properties( )->add_supported_language_codes( "hu" ); /// Hungarian

         try
         {
             m_LanguageSettingsPersistentStorage->Remove( );
             m_LanguageSettingsPersistentStorage->Store( ProtoToMarkup::ToJson( m_LanguageSettings,
                                                                              false ) );
         }
         catch( ... )
         {
             BOSE_LOG( ERROR, "Writing default language settings to persistent storage failed." );
         }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief ProfessorProductController::WriteLanguageSettingsFromPersistentStorage
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProfessorProductController::WriteLanguageSettingsToPersistentStorage( )
{
     try
     {
         BOSE_LOG( ERROR, "Language settings are being written to persistent storage." );

         m_LanguageSettingsPersistentStorage->Remove( );
         m_LanguageSettingsPersistentStorage->Store ( ProtoToMarkup::ToJson( m_LanguageSettings,
                                                                             false ) );
     }
     catch( ... )
     {
         BOSE_LOG( ERROR, "Writing language settings to persistent storage failed." );
     }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief ProfessorProductController::ReadConfigurationStatusToPersistentStorage
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProfessorProductController::ReadConfigurationStatusFromPersistentStorage( )
{
     try
     {
         BOSE_DEBUG( s_logger, "The configuration status is being read from persistent storage." );

         std::string storageString = m_ConfigurationStatusPersistentStorage->Load( );
         ProtoToMarkup::FromJson( storageString, &m_ConfigurationStatus );
     }
     catch( ... )
     {
         try
         {
             BOSE_LOG( DEBUG, "Reading the configuration status from persistent storage failed."      );
             BOSE_LOG( DEBUG, "A default configuration status will be written to persistent storage." );

             if( GetSystemLanguageCode( ).empty( ) )
             {
                 m_ConfigurationStatus.mutable_status( )->set_language( false );
             }
             else
             {
                 m_ConfigurationStatus.mutable_status( )->set_language( true );
             }

             m_ConfigurationStatus.mutable_status( )->set_network ( false );
             m_ConfigurationStatus.mutable_status( )->set_account ( false );

             m_ConfigurationStatusPersistentStorage->Remove( );
             m_ConfigurationStatusPersistentStorage->Store ( ProtoToMarkup::ToJson( m_ConfigurationStatus,
                                                                                    false ) );
         }
         catch( ... )
         {
             BOSE_LOG( ERROR, "Writing a default configuration status to persistent storage failed." );
         }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief ProfessorProductController::WriteConfigurationStatusToPersistentStorage
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProfessorProductController::WriteConfigurationStatusToPersistentStorage( )
{
    try
    {
        BOSE_DEBUG( s_logger, "The configuration status is being written to persistent storage." );

        m_ConfigurationStatusPersistentStorage->Remove( );
        m_ConfigurationStatusPersistentStorage->Store ( ProtoToMarkup::ToJson( m_LanguageSettings,
                                                                               false ) );
    }
    catch( ... )
    {
        BOSE_LOG( ERROR, "Writing the configuration status to persistent storage failed." );
    }
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

     if( message.has_id( ) )
     {
         switch( message.id( ) )
         {
            case LPM_HARDWARE_DOWN:
                 BOSE_DEBUG( s_logger, "An LPM Hardware down message was received." );

                 m_IsLpmReady = false;

                 m_ProductControllerStateMachine.Handle< bool >
                ( &CustomProductControllerState::HandleLpmState, false );

                 break;

            case LPM_HARDWARE_UP:
                 BOSE_DEBUG( s_logger, "An LPM Hardware up message was received." );

                 m_IsLpmReady = true;

                 m_ProductControllerStateMachine.Handle< bool >
                ( &CustomProductControllerState::HandleLpmState, true );

                 break;

            case CAPS_DOWN:
                 BOSE_DEBUG( s_logger, "A CAPS Content Audio Playback Services down message was received." );

                 m_IsCapsReady = false;

                 m_ProductControllerStateMachine.Handle< bool >
                ( &CustomProductControllerState::HandleCapsState, false );

                 break;

            case CAPS_UP:
                 BOSE_DEBUG( s_logger, "A CAPS Content Audio Playback Services up message was received." );

                 m_IsCapsReady = true;

                 m_ProductControllerStateMachine.Handle< bool >
                ( &CustomProductControllerState::HandleCapsState, true );

                  break;

            case NETWORK_DOWN:
                 BOSE_DEBUG( s_logger, "A network down message was received." );

                 m_ConfigurationStatus.mutable_status( )->set_network ( false );
                 WriteConfigurationStatusToPersistentStorage( );

                 m_IsNetworkReady = false;

                 m_ProductControllerStateMachine.Handle< bool >
                ( &CustomProductControllerState::HandleNetworkState, false );

                 break;

            case NETWORK_UP:
                 BOSE_DEBUG( s_logger, "A network up message was received." );

                 m_ConfigurationStatus.mutable_status( )->set_network ( true );
                 WriteConfigurationStatusToPersistentStorage( );

                 m_IsNetworkReady = true;

                 m_ProductControllerStateMachine.Handle< bool >
                ( &CustomProductControllerState::HandleNetworkState, true );

                 break;

            case SYSTEM_LANGUAGE_CHANGE:
                 BOSE_DEBUG( s_logger, "A system language change message was received." );

                 m_ConfigurationStatus.mutable_status( )->set_network ( true );
                 WriteLanguageSettingsToPersistentStorage( );

                 break;

            case KEY_PRESS:
                 {
                     auto keyData = message.data( ).keydata( );

                     switch( keyData.state( ) )
                     {
                        case DOWN:
                             BOSE_DEBUG( s_logger, "A down key press message was received with value %d.",
                                         keyData.value( ) );

                             break;
                        case UP:
                             BOSE_DEBUG( s_logger, "An up key press message was received with value %d.",
                                         keyData.value( ) );

                             break;

                        default:
                             BOSE_DEBUG( s_logger, "A key press message in an unknown state was received with value %d.",
                                         keyData.value( ) );

                             break;
                     }
                 }
                 break;

            case AUDIO_LEVEL:
                 BOSE_DEBUG( s_logger, "An audio level message was received." );
                 break;

            case AUDIO_TONE:
                 BOSE_DEBUG( s_logger, "An audio tone message was received." );
                 break;

            case USER_MUTE:
                 BOSE_DEBUG( s_logger, "A user mute message was received." );
                 break;

            case INTERNAL_MUTE:
                 BOSE_DEBUG( s_logger, "An internal mute message was received." );
                 break;

            case SPEAKER_SETTING:
                 BOSE_DEBUG( s_logger, "A speaker setting message was received." );
                 break;

            case SOURCE_SELECTION:
                 BOSE_DEBUG( s_logger, "A source selection message was received." );
                 break;

            case SOURCE_DESELECTION:
                 BOSE_DEBUG( s_logger, "A source deselection message was received." );
                 break;

            case STS_SOURCES_INIT_DONE:
                 BOSE_DEBUG( s_logger, "An STS Sources Initialized message was received." );

                 m_IsSTSReady = true;

                 m_ProductControllerStateMachine.Handle<>
                ( &CustomProductControllerState::HandleSTSSourcesInit );

                 break;

            case SOURCE_SLOT_SELECTED:
            {
                 const auto& slot = message.data().selectsourceslot().slot();
                 BOSE_DEBUG( s_logger, "An STS Select message was received for slot %s.", ProductSourceSlot_Name( slot ).c_str() );
                 break;
            }

            default:
                 BOSE_DEBUG( s_logger, "An unknown message %d was received.", message.id( ) );
                 break;
         }
     }

     return;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProfessorProductController::Wait
///
/// @brief  This method is called from a calling task to wait until the Product Controller process
///         ends.
///
/// @param  void This method does not take any arguments.
///
/// @return This method does not return anything.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProfessorProductController::Wait( )
{
     while( m_running )
     {
            sleep( PRODUCT_CONTROLLER_RUNNING_CHECK_IN_SECONDS );
     }
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

     m_running = false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProfessorProductController::SetupProductSTSConntroller
///
/// @brief  This method is called to perform the needed initialization of the ProductSTSController,
///         specifically, provide the set of sources to be created initially
///
/// @param  void This method does not take any arguments.
///
/// @return This method does not return anything.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProfessorProductController::SetupProductSTSConntroller( void )
{
    std::vector<ProductSTSController::SourceDescriptor> sources;
    ProductSTSController::SourceDescriptor descriptor_AiQ{ ProductSTS::SLOT_AIQ, "ADAPTiQ", false }; // AiQ is not available as a normal source
    sources.push_back( descriptor_AiQ );
    ProductSTSController::SourceDescriptor descriptor_TV{ ProductSTS::SLOT_TV, "TV", true }; // TV is always available
    sources.push_back( descriptor_TV );
    Callback<void> cb_STSInitWasComplete( std::bind( &ProfessorProductController::HandleSTSInitWasComplete, this ) );
    Callback<ProductSTS::ProductSourceSlot> cb_HandleSelectSourceSlot( std::bind( &ProfessorProductController::HandleSelectSourceSlot, this, std::placeholders::_1 ) );
    m_ProductSTSController.Initialize( sources, cb_STSInitWasComplete, cb_HandleSelectSourceSlot );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProfessorProductController::HandleSTSInitWasComplete
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
void ProfessorProductController::HandleSTSInitWasComplete( void )
{
    ProductMessage message;
    message.set_id( STS_SOURCES_INIT_DONE );
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
/// @param  ProductSTS::ProductSourceSlot sourceSlot - identifies the activated slot
///
/// @return This method does not return anything.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProfessorProductController::HandleSelectSourceSlot( ProductSTS::ProductSourceSlot sourceSlot )
{
    ProductMessage message;
    message.set_id( SOURCE_SLOT_SELECTED );
    message.mutable_data()->mutable_selectsourceslot()->set_slot( sourceSlot );
    IL::BreakThread( std::bind( &ProfessorProductController::HandleMessage,
                                this,
                                message ),
                     GetTask( ) );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                               End of ProductApp Namespace                                    ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
