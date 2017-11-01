////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file      ProductSystemManager.cpp
///
/// @brief     This header file contains functionality for sending and receiving system information
///            through a FrontDoor process.
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
#include "Utilities.h"
#include "Services.h"
#include "BoseLinkServerMsgReboot.pb.h"
#include "BoseLinkServerMsgIds.pb.h"
#include "IPCDirectory.h"
#include "IPCDirectoryIF.h"
#include "BreakThread.h"
#include "NetworkPortDefines.h"
#include "ProductSystemManager.h"
#include "ConfigurationStatus.pb.h"
#include "CapsInitializationStatus.pb.h"
#include "NetManager.pb.h"
#include "STSNetworkStatus.pb.h"
#include "FrontDoorClient.h"
#include "ProductMessage.pb.h"
#include "Language.pb.h"
#include "SystemInfo.pb.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
///                          Start of the Product Application Namespace                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
namespace ProductApp
{

////////////////////////////////////////////////////////////////////////////////////////////////////
///
///            Global Constants
///
////////////////////////////////////////////////////////////////////////////////////////////////////
const std::string g_ProductDirectory = "product-persistence/";

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductSystemManager::GetInstance
///
/// @brief  This static method creates the one and only instance of a ProductSystemManager object.
///         The C++ Version 11 compiler guarantees that only one instance is created in a thread
///         safe way.
///
/// @param NotifyTargetTaskIF* ProductTask This argument points to a task to process
///                                        resource requests and notifications.
///
/// @param Callback< ProductMessage > ProductNotify This is a callback to send events to
///                                                 the Product Controller.
///
/// @return This method returns a pointer to a ProductSystemManager object.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
ProductSystemManager* ProductSystemManager::GetInstance( NotifyTargetTaskIF*        ProductTask,
                                                         Callback< ProductMessage > ProductNotify )
{
    static ProductSystemManager* instance = new ProductSystemManager( ProductTask,
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
/// @param NotifyTargetTaskIF* ProductTask This argument points to a task to process
///                                        resource requests and notifications.
///
/// @param Callback< ProductMessage > ProductNotify This is a callback to send events to
///                                                 the Product Controller.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
ProductSystemManager::ProductSystemManager( NotifyTargetTaskIF*        ProductTask,
                                            Callback< ProductMessage > ProductNotify )

    : m_ProductTask( ProductTask ),
      m_ProductNotify( ProductNotify ),
      m_FrontDoorClient( FrontDoor::FrontDoorClient::Create( "ProductSystemManager" ) ),
      m_LanguageSettingsPersistentStorage( ProtoPersistenceFactory::Create( "ProductLanguage",
                                                                            g_ProductDirectory ) ),
      m_ConfigurationStatusPersistentStorage( ProtoPersistenceFactory::Create( "ConfigurationStatus",
                                                                               g_ProductDirectory ) ),
      m_SystemInfoPersistentStorage( ProtoPersistenceFactory::Create( "SystemInfo",
                                                                      g_ProductDirectory ) )
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductSystemManager::Run
///
/// @return This method returns true if the module is successfully started.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool ProductSystemManager::Run( )
{
    ReadLanguageSettingsFromPersistentStorage( );
    ReadConfigurationStatusFromPersistentStorage( );
    ReadSystemInfoSettingsFromPersistentStorage( );

    ////////////////////////////////////////////////////////////////////////////////////////////////
    /// Registration for other Bose client processes for getting system language settings is made
    /// through the FrontDoorClient. The callback HandleGetLanguageRequest is used to process
    /// language requests.
    ////////////////////////////////////////////////////////////////////////////////////////////////
    {
        AsyncCallback < Callback < ProductPb::Language > >
        callback( std::bind( &ProductSystemManager::HandleGetLanguageRequest,
                             this,
                             std::placeholders::_1 ),
                  m_ProductTask );

        m_FrontDoorClient->RegisterGet( "/system/language", callback );

    }

    BOSE_DEBUG( s_logger, "Registration for getting system language requests has been made." );

    ////////////////////////////////////////////////////////////////////////////////////////////////
    /// Registration for other Bose client processes for posting system language settings is made
    /// through the FrontDoorClient. The callback HandlePostLanguageRequest is used to process
    /// language requests.
    ////////////////////////////////////////////////////////////////////////////////////////////////
    {
        AsyncCallback < ProductPb::Language, Callback < ProductPb::Language > >
        callback( std::bind( &ProductSystemManager::HandlePostLanguageRequest,
                             this,
                             std::placeholders::_1,
                             std::placeholders::_2 ),
                  m_ProductTask );

        m_FrontDoorClient->RegisterPost< ProductPb::Language >( "/system/language" , callback );
    }

    BOSE_DEBUG( s_logger, "Registration for posting system language requests has been made." );

    ////////////////////////////////////////////////////////////////////////////////////////////////
    /// Registration for other Bose client processes for getting configuration status settings is
    /// made through the FrontDoorClient. The callback HandleGetConfigurationStatusRequest is used
    /// to process language requests.
    ////////////////////////////////////////////////////////////////////////////////////////////////
    {
        AsyncCallback < Callback < ProductPb::ConfigurationStatus > >
        callback( std::bind( &ProductSystemManager::HandleGetConfigurationStatusRequest,
                             this,
                             std::placeholders::_1 ),
                  m_ProductTask );

        m_FrontDoorClient->RegisterGet( "/system/configuration/status" , callback );
    }

    BOSE_DEBUG( s_logger, "Registration for getting configuration status requests has been made." );

    ////////////////////////////////////////////////////////////////////////////////////////////////
    /// Registration for the Bose Content Audio Playback Services or CAPS process for getting a
    /// notification when this process is running is made through the FrontDoorClient. The callback
    /// HandleCapsStatus is used to process the notifications.
    ////////////////////////////////////////////////////////////////////////////////////////////////
    {
        AsyncCallback< SoundTouchInterface::CapsInitializationStatus >
        CallbackForNotification( std::bind( &ProductSystemManager::HandleCapsStatus,
                                            this,
                                            std::placeholders::_1 ),
                                 m_ProductTask );

        AsyncCallback< SoundTouchInterface::CapsInitializationStatus >
        CallbackForSuccess( std::bind( &ProductSystemManager::HandleCapsStatus,
                                       this,
                                       std::placeholders::_1 ),
                            m_ProductTask );

        AsyncCallback< FRONT_DOOR_CLIENT_ERRORS >
        CallbackForFailure( std::bind( &ProductSystemManager::HandleCapsStatusFailed,
                                       this,
                                       std::placeholders::_1 ),
                            m_ProductTask );

        m_FrontDoorClient->SendGet< SoundTouchInterface::CapsInitializationStatus >
        ( "/system/capsInitializationStatus", CallbackForSuccess, CallbackForFailure );

        m_FrontDoorClient->RegisterNotification< SoundTouchInterface::CapsInitializationStatus >
        ( "/CapsInitializationUpdate", CallbackForNotification );
    }

    BOSE_DEBUG( s_logger, "A notification request for CAPS initialization messages has been made." );

    ////////////////////////////////////////////////////////////////////////////////////////////////
    /// Registration for ownership of system/info endpoint accessible over FrontDoor.
    ///     Need to support GET and NOTIFY commands.
    ////////////////////////////////////////////////////////////////////////////////////////////////
    {
        AsyncCallback< Callback<::ProductPb::SystemInfo> >
        asyncCallback( std::bind( &ProductSystemManager::HandleGetSystemInfoRequest,
                                  this,
                                  std::placeholders::_1 ),
                       m_ProductTask );
        m_FrontDoorClient->RegisterGet( "/system/info", asyncCallback );

        BOSE_DEBUG( s_logger, "Registration for /system/info GET request complete." );
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductSystemManager::ReadLanguageSettingsFromPersistentStorage
///
/// @return This method returns true if the corresponding member has a system language defined.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool ProductSystemManager::IsSystemLanguageSet( )
{
    return m_LanguageSettings.has_code( );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name  ProductSystemManager::ReadLanguageSettingsFromPersistentStorage
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

        ////////////////////////////////////////////////////////////////////////////////////////////
        ///
        /// @todo At this time, the default language is set to English and add the supported
        ///       languages are added since the data has not been set in storage. Future support
        ///       may involve reading these values from a configuration file.
        ///
        ////////////////////////////////////////////////////////////////////////////////////////////
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
/// @brief ProductSystemManager::ReadSystemInfoSettingsFromPersistentStorage
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductSystemManager::ReadSystemInfoSettingsFromPersistentStorage( void )
{
    try
    {
        BOSE_DEBUG( s_logger, "Reading system/info from persistent storage" );

        std::string storageString = m_SystemInfoPersistentStorage->Load( );
        ProtoToMarkup::FromJson( storageString, &m_SystemInfo );
    }
    catch( ... )
    {

        try
        {
            BOSE_DEBUG( s_logger, "Reading system/info from persistent storage failed." );
            BOSE_DEBUG( s_logger, "Default system/info value will be written to persistent storage." );

            m_SystemInfo.set_name( "Bose SoundTouch 1234" );

            // TODO: these parameters will need updating for final product
            m_SystemInfo.set_type( "SoundTouch XX" );
            m_SystemInfo.set_variant( "Professor" );
            m_SystemInfo.set_guid( "xxxx-xxxx-xx" );
            m_SystemInfo.set_serialnumber( "1234567890" );
            m_SystemInfo.set_moduletype( "Riviera" );
            m_SystemInfo.set_countrycode( "US" );
            m_SystemInfo.set_regioncode( "US" );

            m_SystemInfoPersistentStorage->Remove( );
            m_SystemInfoPersistentStorage->Store( ProtoToMarkup::ToJson( m_SystemInfo, false ) );
        }
        catch( ... )
        {
            BOSE_ERROR( s_logger, "Writing default /system/info to persistent storage failed." );
        }

    }
}


////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name  ProductSystemManager::WriteLanguageSettingsFromPersistentStorage
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
/// @name  ProductSystemManager::HandleGetLanguageRequest
///
/// @param Callback< ProductPb::Language >& response
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductSystemManager::HandleGetLanguageRequest( const Callback< ProductPb::Language >& response )
{
    BOSE_DEBUG( s_logger, "The request to get the system and supported languages has been made." );

    response.Send( m_LanguageSettings );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief ProductSystemManager::HandleGetSystemInfoRequest
///
/// @param const Callback<::ProductPb::SystemInfo>& response
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductSystemManager::HandleGetSystemInfoRequest(
    const Callback<::ProductPb::SystemInfo>& response )
{
    BOSE_DEBUG( s_logger, "/system/info GET request received." );

    response.Send( m_SystemInfo );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name  ProductSystemManager::HandlePostLanguageRequest
///
/// @param ProductPb::Language& language
///
/// @param Callback< ProductPb::Language >& response
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductSystemManager::HandlePostLanguageRequest( const ProductPb::Language&             language,
                                                      const Callback< ProductPb::Language >& response )
{
    ////////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @todo A check for whether the system language is supported needs to be made.
    ///
    ////////////////////////////////////////////////////////////////////////////////////////////////
    BOSE_DEBUG( s_logger, "The request to set the system language to \"%s\" has been made.",
                language.code( ).c_str( ) );

    m_LanguageSettings.CopyFrom( language );

    response.Send( m_LanguageSettings );

    WriteLanguageSettingsToPersistentStorage( );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name  ProductSystemManager::SetNetworkAccoutConfigurationStatus
///
/// @param bool network
///
/// @param bool account
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductSystemManager::SetNetworkAccoutConfigurationStatus( bool network, bool account )
{
    ////////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @todo For the time being during initial testing, the product controller will handle setting
    ///       the network and account information. Once an end-point for determining the account
    ///       status becomes available the handling of the account may be processed insidet the
    ///       ProductSystemManager class directly.
    ///
    ////////////////////////////////////////////////////////////////////////////////////////////////
    auto configurationStatus = m_ConfigurationStatus.mutable_status( );

    configurationStatus->set_network( network );
    configurationStatus->set_language( IsSystemLanguageSet( ) );
    configurationStatus->set_account( account );

    WriteConfigurationStatusToPersistentStorage( );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name  ProductSystemManager::ReadConfigurationStatusFromPersistentStorage
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
/// @name  ProductSystemManager::WriteConfigurationStatusToPersistentStorage
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
/// @name  ProductSystemManager::HandleGetConfigurationStatusRequest
///
/// @param Callback< ProductPb::ConfigurationStatus >& response
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductSystemManager::HandleGetConfigurationStatusRequest( const
                                                                Callback< ProductPb::ConfigurationStatus >&
                                                                response )
{
    BOSE_DEBUG( s_logger, "Sending the configuration status for a get request." );

    response.Send( m_ConfigurationStatus );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name  ProductSystemManager::HandleCapsStatus
///
/// @param SoundTouchInterface::CapsInitializationStatus& status
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductSystemManager::HandleCapsStatus( const SoundTouchInterface::CapsInitializationStatus&
                                             status )
{
    BOSE_DEBUG( s_logger, "---------------- Product CAPS Status Event -----------------" );
    BOSE_DEBUG( s_logger, "CAPS has %s.", status.capsinitialized( ) ? "been initialized" : "not been initialized" );

    ProductMessage productMessage;
    productMessage.mutable_capsstatus( )->set_initialized( status.capsinitialized( ) );
    SendMessage( productMessage );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name  ProductSystemManager::HandleCapsStatusFailed
///
/// @param FRONT_DOOR_CLIENT_ERRORS error
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductSystemManager::HandleCapsStatusFailed( const FRONT_DOOR_CLIENT_ERRORS error )
{
    BOSE_DEBUG( s_logger, "---------------- Product CAPS Status Failed ----------------" );
    BOSE_ERROR( s_logger, "The CAPS initialization status was not received." );
    BOSE_ERROR( s_logger, "An error having the value %d has occurred.", error );

    ProductMessage productMessage;
    productMessage.mutable_capsstatus( )->set_initialized( false );
    SendMessage( productMessage );
}

//////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name  ProductSystemManager::HandleMessage
///
/// @brief This method sends a ProductMessage Protocol Buffer to the product controller.
///
/// @param ProductMessage& message
///
//////////////////////////////////////////////////////////////////////////////////////////////
void ProductSystemManager::SendMessage( ProductMessage& message )
{
    IL::BreakThread( std::bind( m_ProductNotify, message ), m_ProductTask );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name  ProductSystemManager::Stop
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductSystemManager::Stop( void )
{
    return;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                           End of the Product Application Namespace                           ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
