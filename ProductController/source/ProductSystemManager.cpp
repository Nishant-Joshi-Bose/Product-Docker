////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file      ProductSystemManager.cpp
///
/// @brief     This header file contains functionality for sending and receiving system information
///            through a Front Door network router process.
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
#include "DPrint.h"
#include "Services.h"
#include "BoseLinkServerMsgReboot.pb.h"
#include "BoseLinkServerMsgIds.pb.h"
#include "IPCDirectory.h"
#include "IPCDirectoryIF.h"
#include "BreakThread.h"
#include "NetworkPortDefines.h"
#include "ProductSystemManager.h"
#include "ConfigurationStatus.pb.h"
#include "ProductMessage.pb.h"
#include "CapsInitializationStatus.pb.h"
#include "NetManager.pb.h"
#include "STSNetworkStatus.pb.h"
#include "NetManager.pb.h"
#include "FrontDoorClient.h"
#include "ProductMessage.pb.h"
#include "Language.pb.h"
#include "ConfigurationStatus.pb.h"

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
#define PRODUCT_NETWORK_MONITOR_CHECK_IN_SECONDS ( 1 )

////////////////////////////////////////////////////////////////////////////////////////////////////
///
///            Global Constants
///
////////////////////////////////////////////////////////////////////////////////////////////////////
const std::string g_ProductDirectory = "product-persistence/";

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// The following declares a DPrint class type object for logging information in this source code
/// file.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
static const DPrint s_logger { "Product" };

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductSystemManager::GetInstance
///
/// @brief  This static method creates the one and only instance of a ProductSystemManager object.
///         The C++ Version 11 compiler guarantees that only one instance is created in a thread
///         safe way.
///
/// @return This method returns a pointer to a ProductSystemManager object.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
ProductSystemManager* ProductSystemManager::GetInstance( NotifyTargetTaskIF*        mainTask,
                                                         Callback< ProductMessage > ProductNotify )
{
    static ProductSystemManager* instance = new ProductSystemManager( mainTask,
                                                                      ProductNotify );

    BOSE_DEBUG( s_logger, "The instance %8p of the Product System Manager was returned.", instance );

    return instance;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductSystemManager::ProductSystemManager
///
/// @brief  This method is the ProductSystemManager constructor, which is declared as being private
///         to ensure that only one instance of this class can be created through the class
///         GetInstance method.
///
/// @param  void This method does not take any arguments.
///
/// @return This method does not return anything.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
ProductSystemManager::ProductSystemManager( NotifyTargetTaskIF*        mainTask,
                                            Callback< ProductMessage > ProductNotify )

    : m_mainTask( mainTask ),
      m_ProductNotify( ProductNotify ),
      m_FrontDoorClient( FrontDoor::FrontDoorClient::Create( "ProductSystemManager" ) ),
      m_LanguageSettingsPersistentStorage( ProtoPersistenceFactory::Create( "ProductLanguage",
                                                                            g_ProductDirectory ) ),
      m_ConfigurationStatusPersistentStorage( ProtoPersistenceFactory::Create( "ConfigurationStatus",
                                                                               g_ProductDirectory ) )
{
    return;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief  ProductSystemManager::Run
///
/// @brief  ProductSystemManager::Run
///
/// @return
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool ProductSystemManager::Run( )
{
    ////////////////////////////////////////////////////////////////////////////////////////////////
    /// Registration for the Bose Content Audio Playback Services or CAPS process for getting a
    /// notification when this process is running is made through the Front Door network router.
    ////////////////////////////////////////////////////////////////////////////////////////////////
    ReadLanguageSettingsFromPersistentStorage( );
    ReadConfigurationStatusFromPersistentStorage( );

    ////////////////////////////////////////////////////////////////////////////////////////////////
    /// Registration for other Bose client processes for getting system language settings is made
    /// through the Front Door network router. The callback HandleLanguageRequest is used to process
    /// language requests.
    ////////////////////////////////////////////////////////////////////////////////////////////////
    {
        AsyncCallback < Callback < ProductPb::Language > >
        callback( std::bind( &ProductSystemManager::HandleGetLanguageRequest,
                             this,
                             std::placeholders::_1 ),
                  m_mainTask );

        m_FrontDoorClient->RegisterGet( "/system/language", callback );

    }

    BOSE_DEBUG( s_logger, "Registration for getting system language requests has been made." );

    ////////////////////////////////////////////////////////////////////////////////////////////////
    /// Registration for posting system language settings to other Bose client processes is made
    /// through the Front Door network router. The callback HandlePostLanguageRequest is used to
    /// process language requests.
    ////////////////////////////////////////////////////////////////////////////////////////////////
    {
        AsyncCallback < ProductPb::Language, Callback < ProductPb::Language > >
        callback( std::bind( &ProductSystemManager::HandlePostLanguageRequest,
                             this,
                             std::placeholders::_1,
                             std::placeholders::_2 ),
                  m_mainTask );

        m_FrontDoorClient->RegisterPost< ProductPb::Language >( "/system/language" , callback );
    }

    BOSE_DEBUG( s_logger, "Registration for posting system language requests has been made." );

    ////////////////////////////////////////////////////////////////////////////////////////////////
    /// Registration for other Bose client processes for getting configuration status settings is
    /// made through the Front Door network router. The callback HandleGetConfigurationStatusRequest
    /// is used to process language requests.
    ////////////////////////////////////////////////////////////////////////////////////////////////
    {
        AsyncCallback < Callback < ProductPb::ConfigurationStatus > >
        callback( std::bind( &ProductSystemManager::HandleGetConfigurationStatusRequest,
                             this,
                             std::placeholders::_1 ),
                  m_mainTask );

        m_FrontDoorClient->RegisterGet( "/system/configuration/status" , callback );
    }

    BOSE_DEBUG( s_logger, "Registration for getting configuration status requests has been made." );

    ////////////////////////////////////////////////////////////////////////////////////////////////
    /// Registration for the Bose Content Audio Playback Services or CAPS process for getting a
    /// notification when this process is running is made through the Front Door network router.
    /// The callback GetNetworkStatusNotification is used to process the notifications.
    ////////////////////////////////////////////////////////////////////////////////////////////////
    {
        AsyncCallback< SoundTouchInterface::CapsInitializationStatus >
        CallbackForNotification( std::bind( &ProductSystemManager::GetCapsNotification,
                                            this,
                                            std::placeholders::_1 ),
                                 m_mainTask );

        AsyncCallback< SoundTouchInterface::CapsInitializationStatus >
        CallbackForSuccess( std::bind( &ProductSystemManager::GetCapsStatus,
                                       this,
                                       std::placeholders::_1 ),
                            m_mainTask );

        AsyncCallback< FRONT_DOOR_CLIENT_ERRORS >
        CallbackForFailure( std::bind( &ProductSystemManager::GetCapsStatusFailed,
                                       this,
                                       std::placeholders::_1 ),
                            m_mainTask );

        m_FrontDoorClient->SendGet< SoundTouchInterface::CapsInitializationStatus >
        ( "/system/capsInitializationStatus", CallbackForSuccess, CallbackForFailure );

        m_FrontDoorClient->RegisterNotification< SoundTouchInterface::CapsInitializationStatus >
        ( "CapsInitializationUpdate", CallbackForNotification );
    }

    BOSE_DEBUG( s_logger, "A notification request for CAPS initialization messages has been made." );

    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief ProductSystemManager::ReadLanguageSettingsFromPersistentStorage
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool ProductSystemManager::IsSystemLanguageSet( )
{
    return m_LanguageSettings.has_code( );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief ProductSystemManager::ReadLanguageSettingsFromPersistentStorage
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductSystemManager::ReadLanguageSettingsFromPersistentStorage( )
{
    try
    {
        BOSE_DEBUG( s_logger, "Language settings are being read from persistent storage." );

        std::string storageString = m_LanguageSettingsPersistentStorage->Load( );
        ProtoToMarkup::FromJson( storageString, &m_LanguageSettings );
    }
    catch( ... )
    {
        BOSE_DEBUG( s_logger, "Reading language settings from persistent storage failed." );
        BOSE_DEBUG( s_logger, "Default language settings will be written to persistent storage." );

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
            BOSE_ERROR( s_logger, "Writing default language settings to persistent storage failed." );
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief ProductSystemManager::WriteLanguageSettingsFromPersistentStorage
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductSystemManager::WriteLanguageSettingsToPersistentStorage( )
{
    try
    {
        BOSE_DEBUG( s_logger, "Language settings are being written to persistent storage." );

        m_LanguageSettingsPersistentStorage->Remove( );
        m_LanguageSettingsPersistentStorage->Store( ProtoToMarkup::ToJson( m_LanguageSettings,
                                                                           false ) );
    }
    catch( ... )
    {
        BOSE_ERROR( s_logger, "Writing language settings to persistent storage failed." );
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief ProductSystemManager::HandleGetLanguageRequest
///
/// @param response
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductSystemManager::HandleGetLanguageRequest( const Callback< ProductPb::Language >& response )
{
    BOSE_DEBUG( s_logger, "The request to get the system and supported languages has been made." );

    response.Send( m_LanguageSettings );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief ProductSystemManager::HandlePostLanguageRequest
///
/// @param language
///
/// @param response
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductSystemManager::HandlePostLanguageRequest( const ProductPb::Language&             language,
                                                      const Callback< ProductPb::Language >& response )
{
    BOSE_DEBUG( s_logger, "The request to set the system language to \"%s\" has been made.",
                language.code( ).c_str( ) );

    m_LanguageSettings.CopyFrom( language );

    response.Send( m_LanguageSettings );

    WriteLanguageSettingsToPersistentStorage( );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief ProductSystemManager::SetConfigurationStatus
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductSystemManager::SetConfigurationStatus( bool network, bool account, bool language )
{
    auto configurationStatus = m_ConfigurationStatus.mutable_status( );

    configurationStatus->set_network( network );
    configurationStatus->set_language( language );
    configurationStatus->set_account( account );

    WriteConfigurationStatusToPersistentStorage( );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief ProductSystemManager::ReadConfigurationStatusToPersistentStorage
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductSystemManager::ReadConfigurationStatusFromPersistentStorage( )
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
            BOSE_DEBUG( s_logger, "Reading the configuration status from persistent storage failed." );
            BOSE_DEBUG( s_logger, "A default configuration status will be written to persistent storage." );

            auto configurationStatus = m_ConfigurationStatus.mutable_status( );

            configurationStatus->set_network( false );
            configurationStatus->set_account( false );

            if( IsSystemLanguageSet( ) )
            {
                configurationStatus->set_language( true );
            }
            else
            {
                configurationStatus->set_language( false );
            }

            m_ConfigurationStatusPersistentStorage->Remove( );
            m_ConfigurationStatusPersistentStorage->Store( ProtoToMarkup::ToJson( m_ConfigurationStatus,
                                                                                  false ) );
        }
        catch( ... )
        {
            BOSE_ERROR( s_logger, "Writing a default configuration status to persistent storage failed." );
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief ProductSystemManager::WriteConfigurationStatusToPersistentStorage
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductSystemManager::WriteConfigurationStatusToPersistentStorage( )
{
    try
    {
        BOSE_INFO( s_logger, "The configuration status is being written to persistent storage." );

        m_ConfigurationStatusPersistentStorage->Remove( );
        m_ConfigurationStatusPersistentStorage->Store( ProtoToMarkup::ToJson( m_ConfigurationStatus,
                                                                              false ) );
    }
    catch( ... )
    {
        BOSE_ERROR( s_logger, "Writing the configuration status to persistent storage failed." );
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief ProductSystemManager::HandleGetConfigurationSProcessNetworkStatustatusRequest
///
/// @param response
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductSystemManager::HandleGetConfigurationStatusRequest( const Callback< ProductPb::ConfigurationStatus >& response )
{
    BOSE_DEBUG( s_logger, "Sending the configuration status for a get request." );

    response.Send( m_ConfigurationStatus );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief ProductSystemManager::GetCapsStatusNotification
///
/// @param status
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductSystemManager::GetCapsNotification( const SoundTouchInterface::CapsInitializationStatus&
                                                status )
{
    BOSE_DEBUG( s_logger, "------------- Product CAPS Notification Event --------------" );

    if( status.capsinitialized( ) )
    {
        BOSE_DEBUG( s_logger, "CAPS has been initialization." );

        ProductMessage productMessage;
        productMessage.mutable_capsstatus( )->set_initialized( true );
        SendMessage( productMessage );
    }
    else
    {
        BOSE_DEBUG( s_logger, "CAPS has not been initialization." );

        ProductMessage productMessage;
        productMessage.mutable_capsstatus( )->set_initialized( false );
        SendMessage( productMessage );
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief ProductSystemManager::GetCapsStatus
///
/// @param status
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductSystemManager::GetCapsStatus( const SoundTouchInterface::CapsInitializationStatus&
                                          status )
{
    BOSE_DEBUG( s_logger, "---------------- Product CAPS Status Event -----------------" );

    if( status.capsinitialized( ) )
    {
        BOSE_DEBUG( s_logger, "CAPS has been initialized." );

        ProductMessage productMessage;
        productMessage.mutable_capsstatus( )->set_initialized( true );
        SendMessage( productMessage );
    }
    else
    {
        BOSE_DEBUG( s_logger, "CAPS has not been initialized." );

        ProductMessage productMessage;
        productMessage.mutable_capsstatus( )->set_initialized( false );
        SendMessage( productMessage );
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief ProductSystemManager::GetCapsStatusFailed
///
/// @param error
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductSystemManager::GetCapsStatusFailed( const FRONT_DOOR_CLIENT_ERRORS error )
{
    BOSE_DEBUG( s_logger, "---------------- Product CAPS Status Failed ----------------" );
    BOSE_ERROR( s_logger, "The CAPS initialization status was not received." );
    BOSE_ERROR( s_logger, "An error having the value %d has occurred.      ", error );

    ProductMessage productMessage;
    productMessage.mutable_capsstatus( )->set_initialized( false );
    SendMessage( productMessage );
}

//////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name  ProductSystemManager::HandleMessage
///
/// @brief ProductSystemManager::HandleMessage
///
/// @param message
///
//////////////////////////////////////////////////////////////////////////////////////////////
void ProductSystemManager::SendMessage( ProductMessage& message )
{
    IL::BreakThread( std::bind( m_ProductNotify, message ), m_mainTask );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief ProductSystemManager::Stop
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductSystemManager::Stop( void )
{
    return;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                               End of ProductApp Namespace                                    ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
