////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file      ProductFrontDoorNetwork.cpp
///
/// @brief     This header file contains declarations for sending and receiving information through
///            a Front Door network router process, which handles connections and communications
///            between various Bose processes.
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
#include "SystemUtils.h"
#include "DPrint.h"
#include "Services.h"
#include "BoseLinkServerMsgReboot.pb.h"
#include "BoseLinkServerMsgIds.pb.h"
#include "IPCDirectory.h"
#include "IPCDirectoryIF.h"
#include "BreakThread.h"
#include "RebroadcastLatencyMode.pb.h"
#include "NetworkPortDefines.h"
#include "ProductFrontDoorNetwork.h"
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
/// The following declares a DPrint class type object for logging information in this source code
/// file.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
static const DPrint s_logger { "Product" };

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductFrontDoorNetwork::GetInstance
///
/// @brief  This static method creates the one and only instance of a ProductFrontDoorNetwork object.
///         The C++ Version 11 compiler guarantees that only one instance is created in a thread
///         safe way.
///
/// @return This method returns a pointer to a ProductFrontDoorNetwork object.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
ProductFrontDoorNetwork* ProductFrontDoorNetwork::GetInstance( NotifyTargetTaskIF*        mainTask,
        Callback< ProductMessage > ProductNotify )
{
    static ProductFrontDoorNetwork* instance = new ProductFrontDoorNetwork( mainTask,
            ProductNotify );

    BOSE_DEBUG( s_logger, "The instance %8p of the Product Front Door Network was returned.", instance );

    return instance;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductFrontDoorNetwork::ProductFrontDoorNetwork
///
/// @brief  This method is the ProductFrontDoorNetwork constructor, which is declared as being private
///         to ensure that only one instance of this class can be created through the class
///         GetInstance method.
///
/// @param  void This method does not take any arguments.
///
/// @return This method does not return anything.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
ProductFrontDoorNetwork::ProductFrontDoorNetwork( NotifyTargetTaskIF*        mainTask,
        Callback< ProductMessage > ProductNotify )

    : m_mainTask       ( mainTask ),
      m_ProductNotify  ( ProductNotify ),
      m_networkTask    ( IL::CreateTask( "ProductMonitorNetworkTask" ) ),
      m_FrontDoorClient( FrontDoor::FrontDoorClient::Create( "ProductFrontDoorNetwork" ) )
{
    return;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief  ProductFrontDoorNetwork::Run
///
/// @brief  ProductFrontDoorNetwork::Run
///
/// @return
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool ProductFrontDoorNetwork::Run( )
{
    ServeRequests ( );
    MonitorNetwork( );

    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief ProductFrontDoorNetwork::ServeRequests
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductFrontDoorNetwork::ServeRequests( void )
{
    ////////////////////////////////////////////////////////////////////////////////////////////////
    /// Registration for the Bose Content Audio Playback Services or CAPS process for getting a
    /// notification when this process is running is made through the Front Door network router.
    /// The callback GetNetworkStatusNotification is used to process the notifications.
    ////////////////////////////////////////////////////////////////////////////////////////////////
    {
        AsyncCallback< SoundTouchInterface::CapsInitializationStatus >
        CallbackForNotification( std::bind( &ProductFrontDoorNetwork::GetCapsNotification,
                                            this,
                                            std::placeholders::_1 ),
                                 m_mainTask );

        m_FrontDoorClient->RegisterNotification< SoundTouchInterface::CapsInitializationStatus >
        ( "CapsInitializationUpdate", CallbackForNotification );
    }

    BOSE_DEBUG( s_logger, "A notification request for CAPS initialization messages has been made." );

    ////////////////////////////////////////////////////////////////////////////////////////////////
    /// Registration for other Bose client processes for getting system language settings is made
    /// through the Front Door network router. The callback HandleLanguageRequest is used to process
    /// language requests.
    ////////////////////////////////////////////////////////////////////////////////////////////////
    {
        AsyncCallback < Callback < ProductPb::Language > >
        callback( std::bind( &ProductFrontDoorNetwork::HandleGetLanguageRequest,
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
        callback( std::bind( &ProductFrontDoorNetwork::HandlePostLanguageRequest,
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
        callback( std::bind( &ProductFrontDoorNetwork::HandleGetConfigurationStatusRequest,
                             this,
                             std::placeholders::_1 ),
                  m_mainTask );

        m_FrontDoorClient->RegisterGet( "/system/configuration/status" , callback );
    }

    BOSE_DEBUG( s_logger, "Registration for getting configuration status requests has been made." );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief ProductFrontDoorNetwork::GetCapsNotification
///
/// @param status
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductFrontDoorNetwork::GetCapsNotification( const SoundTouchInterface::CapsInitializationStatus& status )
{
    ProductMessage productMessage;

    BOSE_DEBUG( s_logger, "A CAPS initialization message has been received." );

    if( status.capsinitialized( ) )
    {
        productMessage.set_id( CAPS_UP );
        SendMessage          ( productMessage );
    }
    else
    {
        productMessage.set_id( CAPS_DOWN );
        SendMessage          ( productMessage );
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief ProductFrontDoorNetwork::HandleGetLanguageRequest
///
/// @param response
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductFrontDoorNetwork::HandleGetLanguageRequest( const Callback< ProductPb::Language >& response )
{
    ProductPb::Language language;

    BOSE_DEBUG( s_logger, "A request for system language settings has been received." );

    language.set_code( GetSystemLanguageCode( ) );

    language.mutable_properties()->add_supported_language_codes( "da" ); /// Danish
    language.mutable_properties()->add_supported_language_codes( "de" ); /// German
    language.mutable_properties()->add_supported_language_codes( "en" ); /// English
    language.mutable_properties()->add_supported_language_codes( "es" ); /// Spanish
    language.mutable_properties()->add_supported_language_codes( "fr" ); /// French
    language.mutable_properties()->add_supported_language_codes( "it" ); /// Italian
    language.mutable_properties()->add_supported_language_codes( "nl" ); /// Dutch
    language.mutable_properties()->add_supported_language_codes( "sv" ); /// Swedish
    language.mutable_properties()->add_supported_language_codes( "ja" ); /// Japanese
    language.mutable_properties()->add_supported_language_codes( "zh" ); /// Chinese
    language.mutable_properties()->add_supported_language_codes( "ko" ); /// Korean
    language.mutable_properties()->add_supported_language_codes( "th" ); /// Thai
    language.mutable_properties()->add_supported_language_codes( "cs" ); /// Czechoslovakian
    language.mutable_properties()->add_supported_language_codes( "fi" ); /// Finnish
    language.mutable_properties()->add_supported_language_codes( "el" ); /// Greek
    language.mutable_properties()->add_supported_language_codes( "no" ); /// Norwegian
    language.mutable_properties()->add_supported_language_codes( "pl" ); /// Polish
    language.mutable_properties()->add_supported_language_codes( "pt" ); /// Portuguese
    language.mutable_properties()->add_supported_language_codes( "ro" ); /// Romanian
    language.mutable_properties()->add_supported_language_codes( "ru" ); /// Russian
    language.mutable_properties()->add_supported_language_codes( "sl" ); /// Slovenian
    language.mutable_properties()->add_supported_language_codes( "tr" ); /// Turkish
    language.mutable_properties()->add_supported_language_codes( "hu" ); /// Hungarian

    BOSE_DEBUG( s_logger, "The request to get the system and supported languages has been made.");

    response.Send( language );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief ProductFrontDoorNetwork::HandlePostLanguageRequest
///
/// @param language
///
/// @param response
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductFrontDoorNetwork::HandlePostLanguageRequest( const ProductPb::Language&             language,
        const Callback< ProductPb::Language >& response )
{
    ProductMessage productMessage;

    BOSE_DEBUG( s_logger, "The request to set the system language to \"%s\" has been made.", language.code( ).c_str( ) );

    m_LanguageSettings.set_code( language.code( ) );

    productMessage.set_id( SYSTEM_LANGUAGE_CHANGE );
    SendMessage          ( productMessage );

    response.Send( language );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief ProductFrontDoorNetwork::GetSystemLanguageCode
///
/// @return
///
////////////////////////////////////////////////////////////////////////////////////////////////////
std::string ProductFrontDoorNetwork::GetSystemLanguageCode( )
{
    return m_LanguageSettings.code( );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief ProductFrontDoorNetwork::SetSystemLanguageCode
///
/// @return
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductFrontDoorNetwork::SetSystemLanguageCode( std::string& systemLanguage )
{
    m_LanguageSettings.set_code( systemLanguage );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief ProductFrontDoorNetwork::HandleGetConfigurationStatusRequest
///
/// @param response
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductFrontDoorNetwork::HandleGetConfigurationStatusRequest( const Callback< ProductPb::ConfigurationStatus >& response )
{
    BOSE_DEBUG( s_logger, "Sending the configuration status for a get request.");

    response.Send( m_ConfigurationStatus );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name  ProductFrontDoorNetwork::MonitorNetwork
///
/// @brief This method attempts to get and registers for network status notifications.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductFrontDoorNetwork::MonitorNetwork( )
{
    ////////////////////////////////////////////////////////////////////////////////////////////////
    /// Registration as a client for getting the network status is made through the Front Door
    /// network router. The callbacks GetNetworkStatusSuccess and GetNetworkStatusFailed are used
    /// if the network status could be obtained or not, respectively.
    ////////////////////////////////////////////////////////////////////////////////////////////////
    AsyncCallback< NetManager::Protobuf::NetworkStatus >
    CallbackForSuccess( std::bind( &ProductFrontDoorNetwork::GetNetworkStatusSuccess,
                                   this,
                                   std::placeholders::_1 ),
                        m_networkTask );

    AsyncCallback< FRONT_DOOR_CLIENT_ERRORS >
    CallbackForFailure( std::bind( &ProductFrontDoorNetwork::GetNetworkStatusFailed,
                                   this,
                                   std::placeholders::_1 ),
                        m_networkTask );

    AsyncCallback< NetManager::Protobuf::NetworkStatus >
    CallbackForNotification( std::bind( &ProductFrontDoorNetwork::GetNetworkStatusNotification,
                                        this,
                                        std::placeholders::_1 ),
                             m_networkTask );

    BOSE_DEBUG( s_logger, "A notification request for network status changes has been made." );
    BOSE_DEBUG( s_logger, "A request for getting the network status has been made."          );

    m_FrontDoorClient->SendGet< NetManager::Protobuf::NetworkStatus >
    ( "/network/status", CallbackForSuccess, CallbackForFailure );

    m_FrontDoorClient->RegisterNotification< NetManager::Protobuf::NetworkStatus>
    ( "/network/status", CallbackForNotification );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name  ProductFrontDoorNetwork::GetNetworkStatusSuccess
///
/// @brief This method is a callback that is invoked whenever there is a change in the network status.
///        Registration for network status notifications is made from the MonitorNetwork method.
///
/// @param networkStatus  This class corresponds to a Google Protocol Buffer that contains
///                       information on the currently primary network being connected as well as
///                       all the available network interfaces.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductFrontDoorNetwork::GetNetworkStatusSuccess( const NetManager::Protobuf::NetworkStatus&
        networkStatus )
{
    BOSE_DEBUG( s_logger, "A network status message was received." );

    ProcessNetworkStatus( networkStatus, false );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name  ProductFrontDoorNetwork::GetNetworkStatusNotification
///
/// @brief This method is a callback that is invoked whenever there is a change in the network status.
///        Registration for network status notifications is made from the MonitorNetwork method.
///
/// @param networkStatus  This class corresponds to a Google Protocol Buffer that contains
///                       information on the currently primary network being connected as well as
///                       all the available network interfaces.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductFrontDoorNetwork::GetNetworkStatusNotification( const NetManager::Protobuf::NetworkStatus&
        networkStatus )
{
    BOSE_DEBUG( s_logger, "A network status notification message was received." );

    ProcessNetworkStatus( networkStatus, true );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name  ProductFrontDoorNetwork::GetNetworkStatusFailed
///
/// @brief This method is a callback that is invoked when an attempt to send a get request for the
///        network status has failed from the MonitorNetwork method.
///
/// @param error  This argument is an enumeration of type FRONT_DOOR_CLIENT_ERRORS, indicating what
///               error has occurred.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductFrontDoorNetwork::GetNetworkStatusFailed( const FRONT_DOOR_CLIENT_ERRORS error )
{
    ProductMessage productMessage;

    BOSE_ERROR( s_logger, "The network status was not received." );
    BOSE_ERROR( s_logger, "An error %d has occurred.           ", error );

    productMessage.set_id( NETWORK_DOWN   );
    SendMessage          ( productMessage );

    sleep( PRODUCT_NETWORK_MONITOR_CHECK_IN_SECONDS );
    MonitorNetwork( );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name  ProductFrontDoorNetwork::ProcessNetworkStatus
///
/// @brief This method processes the network status received, and will send a ProductMessage to
///        notify the Product Controller of the network IP Address, state, type, and frequency in
///        kHz, as well as whether it is now accessible and connected. Additionally, if the network
///        is not yet up, it will attempt to get and monitor the network status again. This
///        functionality ensures that the Product Controller will be able to re-establish a network
///        connection if either the FrontDoor process or Network Manager process goes down.
///
/// @param networkStatus  This class corresponds to a Google Protocol Buffer that contains
///                       information on the currently primary network being connected as well as
///                       all the available network interfaces.
///
/// @param networkChanged The value of this Boolean argument is true, if the network status has
///                       been changed (typically invoked from the GetNetworkStatusNotification
///                       method), or is false if the network status has just been received
///                       (typically from a GetNetworkStatusSuccess method call).
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductFrontDoorNetwork::ProcessNetworkStatus( const NetManager::Protobuf::NetworkStatus& networkStatus,
        bool                                       networkChanged )
{
    ProductMessage productMessage;

    if( networkStatus.interfaces_size( ) < 0 )
    {
        BOSE_ERROR( s_logger, "No networks are currently available." );

        auto networkData = productMessage.mutable_data( )->mutable_networkdata( );

        networkData->set_type     ( UNKNOWN_NETWORK_TYPE );
        networkData->set_frequency( 0 );

        productMessage.set_id( NETWORK_DOWN );
        SendMessage          ( productMessage );

        sleep( PRODUCT_NETWORK_MONITOR_CHECK_IN_SECONDS );
        MonitorNetwork( );

        return;
    }
    else
    {
        BOSE_ERROR( s_logger, "There are %d networks currently available.", networkStatus.interfaces_size( ) );

        auto networkData = productMessage.mutable_data( )->mutable_networkdata( );

        if( networkStatus.primary( ) == NetManager::Protobuf::NetworkType::WIRED_ETH ||
                networkStatus.primary( ) == NetManager::Protobuf::NetworkType::WIRED_USB   )
        {
            BOSE_ERROR( s_logger, "The primary network is wired." );

            networkData->set_type     ( WIRED );
            networkData->set_frequency( 0 );
        }
        else if( networkStatus.primary( ) == NetManager::Protobuf::NetworkType::WIRELESS    ||
                 networkStatus.primary( ) == NetManager::Protobuf::NetworkType::WIRELESS_AP   )
        {
            BOSE_ERROR( s_logger, "The primary network is wireless." );

            networkData->set_type     ( WIRELESS );
            networkData->set_frequency( 0 );
        }
        else
        {
            BOSE_ERROR( s_logger, "The primary network has an unknown type." );

            networkData->set_type     ( UNKNOWN_NETWORK_TYPE );
            networkData->set_frequency( 0 );
        }

        productMessage.set_id( NETWORK_UP );
        SendMessage          ( productMessage );

        return;
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name  ProductFrontDoorNetwork::HandleMessage
///
/// @brief ProductFrontDoorNetwork::HandleMessage
///
/// @param message
///
//////////////////////////////////////////////////////////////////////////////////////////////
void ProductFrontDoorNetwork::HandleMessage( ProductMessage& message )
{
    if( message.has_id( ) )
    {
        switch( message.id( ) )
        {
        case SYSTEM_LANGUAGE_CHANGE:
        {
            auto        languageData = message.mutable_data( )->mutable_languagedata( );
            std::string systemLanguage;

            systemLanguage.assign( languageData->systemlanguage( ) );
            SetSystemLanguageCode( systemLanguage );

            BOSE_DEBUG( s_logger, "A language setting message was received." );
            BOSE_DEBUG( s_logger, "The system language is now set to %s.   ",
                        systemLanguage.c_str( ) );
        }
        break;

        case CONFIGURATION_STATUS:
        {
            bool networkStatus  = message.mutable_data( )->mutable_configurationstatus( )->network ( );
            bool languageStatus = message.mutable_data( )->mutable_configurationstatus( )->language( );
            bool accountStatus  = message.mutable_data( )->mutable_configurationstatus( )->account ( );

            m_ConfigurationStatus.mutable_status( )->set_network ( networkStatus  );
            m_ConfigurationStatus.mutable_status( )->set_language( languageStatus );
            m_ConfigurationStatus.mutable_status( )->set_account ( accountStatus  );

            BOSE_DEBUG( s_logger, "A configuration status message was received." );
        }
        break;

        default:
        {
            BOSE_DEBUG( s_logger, "An unknown message %d was received.", message.id( ) );
        }
        break;
        }
    }

    return;
}

//////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name  ProductFrontDoorNetwork::HandleMessage
///
/// @brief ProductFrontDoorNetwork::HandleMessage
///
/// @param message
///
//////////////////////////////////////////////////////////////////////////////////////////////
void ProductFrontDoorNetwork::SendMessage( ProductMessage& message )
{
    IL::BreakThread( std::bind( m_ProductNotify, message ), m_mainTask );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                               End of ProductApp Namespace                                    ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
