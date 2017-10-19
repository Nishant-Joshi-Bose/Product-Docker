////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file      ProductNetworkManager.cpp
///
/// @brief     This header file contains declarations for network management.
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
#include "BreakThread.h"
#include "FrontDoorClient.h"
#include "NetworkPortDefines.h"
#include "ProductNetworkManager.h"
#include "ConfigurationStatus.pb.h"
#include "ProductMessage.pb.h"
#include "CapsInitializationStatus.pb.h"
#include "NetManager.pb.h"
#include "STSNetworkStatus.pb.h"
#include "NetManager.pb.h"
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
/// @name   ProductNetworkManager::GetInstance
///
/// @brief  This static method creates the one and only instance of a ProductNetworkManager object.
///         The C++ Version 11 compiler guarantees that only one instance is created in a thread
///         safe way.
///
/// @return This method returns a pointer to a ProductNetworkManager object.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
ProductNetworkManager* ProductNetworkManager::GetInstance( NotifyTargetTaskIF*        mainTask,
                                                           Callback< ProductMessage > ProductNotify )
{
    static ProductNetworkManager* instance = new ProductNetworkManager( mainTask,
                                                                        ProductNotify );

    BOSE_DEBUG( s_logger, "The instance %8p of the Product Network Manager was returned.", instance );

    return instance;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductNetworkManager::ProductNetworkManager
///
/// @brief  This method is the ProductNetworkManager constructor, which is declared as being private
///         to ensure that only one instance of this class can be created through the class
///         GetInstance method.
///
/// @param  void This method does not take any arguments.
///
/// @return This method does not return anything.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
ProductNetworkManager::ProductNetworkManager( NotifyTargetTaskIF*        mainTask,
                                              Callback< ProductMessage > ProductNotify )

    : m_mainTask( mainTask ),
      m_ProductNotify( ProductNotify ),
      m_FrontDoorClient( FrontDoor::FrontDoorClient::Create( "ProductNetworkManager" ) )
{
    return;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief  ProductNetworkManager::Run
///
/// @brief  ProductNetworkManager::Run
///
/// @return
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool ProductNetworkManager::Run( )
{
    MonitorNetwork( );

    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name  ProductNetworkManager::MonitorNetwork
///
/// @brief This method attempts to registers for network status notifications.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductNetworkManager::MonitorNetwork( )
{
    ////////////////////////////////////////////////////////////////////////////////////////////////
    /// Registration as a client for getting the entire network status is made through the Front
    /// Door router. The callback GetEntireNetworkStatus is used to receive notifications regarding
    /// the entire network status.
    ////////////////////////////////////////////////////////////////////////////////////////////////
    AsyncCallback< NetManager::Protobuf::NetworkStatus >
    CallbackForEntireNetworkStatus( std::bind( &ProductNetworkManager::GetEntireNetworkStatus,
                                               this,
                                               std::placeholders::_1 ),
                                    m_mainTask );

    m_FrontDoorClient->RegisterNotification< NetManager::Protobuf::NetworkStatus >
    ( "/network/status", CallbackForEntireNetworkStatus );

    BOSE_DEBUG( s_logger, "A notification request for the entire network status has been made." );

    ////////////////////////////////////////////////////////////////////////////////////////////////
    /// Registration as a client for getting all the available wireless network profiles is made
    /// through the Front Door router. The callback GetWirelessNetworkProfiles is used to receive
    /// these notifications.
    ////////////////////////////////////////////////////////////////////////////////////////////////
    AsyncCallback< NetManager::Protobuf::WiFiProfiles >
    CallbackForWirelessNetworkProfiles( std::bind( &ProductNetworkManager::GetWirelessNetworkProfiles,
                                                   this,
                                                   std::placeholders::_1 ),
                                        m_mainTask );

    m_FrontDoorClient->RegisterNotification< NetManager::Protobuf::WiFiProfiles >
    ( "network/wifi/profile", CallbackForWirelessNetworkProfiles );

    BOSE_DEBUG( s_logger, "A notification request for network wireless profile data has been made." );

    ////////////////////////////////////////////////////////////////////////////////////////////////
    /// Registration as a client for getting the current wireless network status is made through the
    /// Front Door router. The callback GetWirelessNetworkStatus is used to receive these
    /// notifications.
    ////////////////////////////////////////////////////////////////////////////////////////////////
    AsyncCallback< NetManager::Protobuf::WiFiStatus >
    CallbackForWirelessNetworkStatus( std::bind( &ProductNetworkManager::GetWirelessNetworkStatus,
                                                 this,
                                                 std::placeholders::_1 ),
                                      m_mainTask );

    m_FrontDoorClient->RegisterNotification< NetManager::Protobuf::WiFiStatus >
    ( "network/wifi/status", CallbackForWirelessNetworkStatus );

    BOSE_DEBUG( s_logger, "A notification request for network wireless status changes has been made." );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name  ProductNetworkManager::GetEntireNetworkStatus
///
/// @brief This method processes the network status received, and will send a ProductMessage to
///        notify the Product Controller of the network state, configured, and connected status.
///
/// @param networkStatus  This class corresponds to a Google Protocol Buffer that contains
///                       information on the current available networks.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductNetworkManager::GetEntireNetworkStatus( const NetManager::Protobuf::NetworkStatus& networkStatus )
{
    if( networkStatus.interfaces_size( ) <= 0 )
    {
        BOSE_DEBUG( s_logger, "-------------- Product Network Manager Status --------------" );
        BOSE_DEBUG( s_logger, "No networks are currently available." );

        ProductMessage productMessage;
        auto networkData = productMessage.mutable_networkstatus( );

        networkData->set_configured( false );
        networkData->set_connected( false );
        networkData->set_networktype( ProductNetworkStatus_ProductNetworkType_Unknown );

        SendMessage( productMessage );
    }
    else
    {
        BOSE_DEBUG( s_logger, "-------------- Product Network Manager Status --------------" );
        BOSE_DEBUG( s_logger, "There are %d networks currently available.", networkStatus.interfaces_size( ) );
        BOSE_DEBUG( s_logger, " " );

        ProductMessage productMessage;
        auto networkData = productMessage.mutable_networkstatus( );

        networkData->set_configured( false );
        networkData->set_connected( false );
        networkData->set_networktype( ProductNetworkStatus_ProductNetworkType_Unknown );

        std::string interfaceMacString( "Unknown" );
        std::string interfaceNameString( "Unknown" );
        std::string networkStateString( "Unknown" );
        std::string networkTypeString( "Unknown" );
        std::string ipAddressString( "Unknown" );
        std::string broadcastString( "Unknown" );
        std::string subnetMaskString( "Unknown" );
        std::string gatewayAddressString( "Unknown" );
        std::string primaryDnsString( "Unknown" );
        std::string secondaryDnsString( "Unknown" );

        int  index;

        for( index = 0; index < networkStatus.interfaces_size( ); index++ )
        {
            interfaceMacString.assign( "Unknown" );
            interfaceNameString.assign( "Unknown" );
            networkStateString.assign( "Unknown" );
            networkTypeString.assign( "Unknown" );
            ipAddressString.assign( "Unknown" );
            broadcastString.assign( "Unknown" );
            subnetMaskString.assign( "Unknown" );
            gatewayAddressString.assign( "Unknown" );
            primaryDnsString.assign( "Unknown" );
            secondaryDnsString.assign( "Unknown" );

            if( networkStatus.interfaces( index ).has_macaddress( ) )
            {
                interfaceMacString.assign( networkStatus.interfaces( index ).macaddress( ) );
            }

            if( networkStatus.interfaces( index ).has_name( ) )
            {
                interfaceNameString.assign( networkStatus.interfaces( index ).name( ) );
            }

            if( networkStatus.interfaces( index ).has_state( ) )
            {
                switch( networkStatus.interfaces( index ).state( ) )
                {
                case NetManager::Protobuf::NetworkInterface_State::NetworkInterface_State_DOWN:
                    networkStateString.assign( "Down" );
                    break;
                case NetManager::Protobuf::NetworkInterface_State::NetworkInterface_State_UP:
                    networkStateString.assign( "Up" );
                    break;
                default:
                    break;
                }
            }

            if( networkStatus.interfaces( index ).has_type( ) )
            {
                switch( networkStatus.interfaces( index ).type( ) )
                {
                case NetManager::Protobuf::NetworkType::WIRED_ETH:
                    networkTypeString.assign( "Wired Ethernet" );
                    break;
                case NetManager::Protobuf::NetworkType::WIRELESS:
                    networkTypeString.assign( "Wireless" );
                    break;
                case NetManager::Protobuf::NetworkType::WIRELESS_AP:
                    networkTypeString.assign( "Wireless Access Point" );
                    break;
                case NetManager::Protobuf::NetworkType::WIRED_USB:
                    networkTypeString.assign( "Wired USB" );
                    break;
                default:
                    break;
                }
            }

            if( networkStatus.interfaces( index ).has_state( ) &&
                networkStatus.interfaces( index ).has_type( ) )
            {
                if( networkStatus.interfaces( index ).type( ) == NetManager::Protobuf::NetworkType::WIRED_ETH )
                {
                    if( networkStatus.interfaces( index ).state( ) ==
                        NetManager::Protobuf::NetworkInterface_State::NetworkInterface_State_UP )
                    {
                        if( networkStatus.interfaces( index ).has_ipinfo( ) )
                        {
                            if( networkStatus.interfaces( index ).ipinfo( ).has_ipaddress( ) )
                            {
                                networkData->set_configured( true );
                                networkData->set_connected( true );
                                networkData->set_networktype( ProductNetworkStatus_ProductNetworkType_Wired );
                            }
                            else
                            {
                                networkData->set_configured( true );
                                networkData->set_connected( false );
                                networkData->set_networktype( ProductNetworkStatus_ProductNetworkType_Wired );
                            }
                        }
                        else
                        {
                            networkData->set_configured( true );
                            networkData->set_connected( false );
                            networkData->set_networktype( ProductNetworkStatus_ProductNetworkType_Wired );
                        }
                    }
                }
                else if( networkStatus.interfaces( index ).type( ) == NetManager::Protobuf::NetworkType::WIRELESS    ||
                         networkStatus.interfaces( index ).type( ) == NetManager::Protobuf::NetworkType::WIRELESS_AP )
                {
                    if( networkStatus.interfaces( index ).state( ) ==
                        NetManager::Protobuf::NetworkInterface_State::NetworkInterface_State_UP )
                    {
                        if( networkStatus.interfaces( index ).has_ipinfo( ) )
                        {
                            if( networkStatus.interfaces( index ).ipinfo( ).has_ipaddress( ) )
                            {
                                networkData->set_configured( true );
                                networkData->set_connected( true );
                                networkData->set_networktype( ProductNetworkStatus_ProductNetworkType_Wireless );
                            }
                            else
                            {
                                networkData->set_configured( true );
                                networkData->set_connected( false );
                                networkData->set_networktype( ProductNetworkStatus_ProductNetworkType_Wireless );
                            }
                        }
                        else
                        {
                            networkData->set_configured( true );
                            networkData->set_connected( false );
                            networkData->set_networktype( ProductNetworkStatus_ProductNetworkType_Wireless );
                        }
                    }
                }
            }

            if( networkStatus.interfaces( index ).has_ipinfo( ) )
            {
                if( networkStatus.interfaces( index ).ipinfo( ).has_ipaddress( ) )
                {
                    ipAddressString.assign( networkStatus.interfaces( index ).ipinfo( ).ipaddress( ) );
                }
                if( networkStatus.interfaces( index ).ipinfo( ).has_broadcastaddress( ) )
                {
                    broadcastString.assign( networkStatus.interfaces( index ).ipinfo( ).broadcastaddress( ) );
                }
                if( networkStatus.interfaces( index ).ipinfo( ).has_subnetmask( ) )
                {
                    subnetMaskString.assign( networkStatus.interfaces( index ).ipinfo( ).subnetmask( ) );
                }
                if( networkStatus.interfaces( index ).ipinfo( ).has_gateway( ) )
                {
                    gatewayAddressString.assign( networkStatus.interfaces( index ).ipinfo( ).gateway( ) );
                }
                if( networkStatus.interfaces( index ).ipinfo( ).has_primarydns( ) )
                {
                    primaryDnsString.assign( networkStatus.interfaces( index ).ipinfo( ).primarydns( ) );
                }
                if( networkStatus.interfaces( index ).ipinfo( ).has_secondarydns( ) )
                {
                    secondaryDnsString.assign( networkStatus.interfaces( index ).ipinfo( ).secondarydns( ) );
                }
            }

            BOSE_DEBUG( s_logger, "Interface Index   : %d ", index );
            BOSE_DEBUG( s_logger, "Interface MAC     : %s ", interfaceMacString.c_str( ) );
            BOSE_DEBUG( s_logger, "Interface Name    : %s ", interfaceNameString.c_str( ) );
            BOSE_DEBUG( s_logger, "Network State     : %s ", networkStateString.c_str( ) );
            BOSE_DEBUG( s_logger, "Network Type      : %s ", networkTypeString.c_str( ) );
            BOSE_DEBUG( s_logger, "IP Address        : %s ", ipAddressString.c_str( ) );
            BOSE_DEBUG( s_logger, "Broadcast Address : %s ", broadcastString.c_str( ) );
            BOSE_DEBUG( s_logger, "Subnet Mask       : %s ", subnetMaskString.c_str( ) );
            BOSE_DEBUG( s_logger, "Gateway Address   : %s ", gatewayAddressString.c_str( ) );
            BOSE_DEBUG( s_logger, "Primary DNS       : %s ", primaryDnsString.c_str( ) );
            BOSE_DEBUG( s_logger, "Secondary DNS     : %s ", secondaryDnsString.c_str( ) );
            BOSE_DEBUG( s_logger, " " );
        }

        if( networkStatus.primary( ) == NetManager::Protobuf::NetworkType::WIRED_ETH )
        {
            BOSE_DEBUG( s_logger, "The primary network is Wired Ethernet." );
        }
        else if( networkStatus.primary( ) == NetManager::Protobuf::NetworkType::WIRED_USB )
        {
            BOSE_DEBUG( s_logger, "The primary network is Wired USB." );
        }
        else if( networkStatus.primary( ) == NetManager::Protobuf::NetworkType::WIRELESS )
        {
            BOSE_DEBUG( s_logger, "The primary network is Wireless." );
        }
        else if( networkStatus.primary( ) == NetManager::Protobuf::NetworkType::WIRELESS_AP )
        {
            BOSE_DEBUG( s_logger, "The primary network is Wireless AP." );
        }
        else
        {
            BOSE_DEBUG( s_logger, "The primary network has an unknown type." );
        }

        if( networkStatus.has_isprimaryup( ) )
        {
            if( networkStatus.isprimaryup( ) )
            {
                BOSE_DEBUG( s_logger, "The primary network is currently up." );
            }
            else
            {
                BOSE_DEBUG( s_logger, "The primary network is currently down." );
            }
        }
        else
        {
            BOSE_DEBUG( s_logger, "The primary network is in an unknown state." );
        }

        SendMessage( productMessage );
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name  ProductNetworkManager::GetWirelessNetworkProfiles
///
/// @brief This method processes the wireless network profiles received, and will send a
///        ProductMessage to notify the Product Controller of the wireless network configured state.
///
/// @param wirelessStatus This class corresponds to a Google Protocol Buffer that contains
///                       information on the current available wireless network profiles.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductNetworkManager::GetWirelessNetworkProfiles( const NetManager::Protobuf::WiFiProfiles&
                                                        wirelessProfiles )
{
    auto wirelessProfileSize = wirelessProfiles.profiles_size( );

    if( wirelessProfileSize <= 0 )
    {
        BOSE_DEBUG( s_logger, "----------------- Product Network Manager ------------------" );
        BOSE_DEBUG( s_logger, "No wireless networks are currently available." );
    }
    else
    {
        BOSE_DEBUG( s_logger, "----------------- Product Network Manager ------------------" );
        BOSE_DEBUG( s_logger, "There are %d wireless networks currently available.", wirelessProfileSize );
        BOSE_DEBUG( s_logger, " " );

        int  index;
        std::string WirlessSsidString( "Unknown" );
        std::string WirlessSecurityString( "Unknown" );

        ProductMessage productMessage;
        productMessage.mutable_wirelessstatus( )->set_configured( false );
        productMessage.mutable_wirelessstatus( )->set_frequencykhz( 0 );

        for( index = 0; index < wirelessProfileSize; index++ )
        {
            WirlessSsidString.assign( "Unknown" );
            WirlessSecurityString.assign( "Unknown" );

            if( wirelessProfiles.profiles( index ).has_ssid( ) )
            {
                WirlessSsidString.assign( wirelessProfiles.profiles( index ).ssid( ) );

                productMessage.mutable_wirelessstatus( )->set_configured( true );
            }

            if( wirelessProfiles.profiles( index ).has_security( ) )
            {
                WirlessSecurityString.assign( wirelessProfiles.profiles( index ).security( ) );
            }

            BOSE_DEBUG( s_logger, "Wireless Index    : %d ", index );
            BOSE_DEBUG( s_logger, "Wireless SSID     : %s ", WirlessSsidString.c_str( ) );
            BOSE_DEBUG( s_logger, "Wireless Security : %s ", WirlessSecurityString.c_str( ) );
            BOSE_DEBUG( s_logger, " " );
        }

        ///
        /// If a configured wireless network is available, send a message to the Product Controller.
        /// Otherwise, do not send a message, since a wired Ethernet network may already be
        /// configured and available.
        ///
        if( productMessage.mutable_wirelessstatus( )->configured( ) )
        {
            BOSE_DEBUG( s_logger, "A configured wireless network is currently available." );
            SendMessage( productMessage );
        }
        else
        {
            BOSE_DEBUG( s_logger, "No configured wireless network is currently available." );
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name  ProductNetworkManager::GetWirelessNetworkProfiles
///
/// @brief This method processes the wireless network profiles received, and will send a
///        ProductMessage to notify the Product Controller of the wireless network configured state.
///
/// @param wirelessStatus This class corresponds to a Google Protocol Buffer that contains
///                       information on the current available wireless networks.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductNetworkManager::GetWirelessNetworkStatus( const NetManager::Protobuf::WiFiStatus&
                                                      wirelessStatus )
{
    ///
    /// Only process the wireless network status if it has an SSID, which indicates that it is
    /// configurable. In this case, record the frequency for sending frequency information from the
    /// Product Controller to the LPM to avoid any frequency interruption during a speaker Adapt IQ
    /// process.
    ///
    std::string wirlessSsidString( "Unknown" );
    std::string wirlessStateString( "Unknown" );
    std::string wirlessFrequencyInKhz( "Unknown" );
    std::string wirlessSignalDbm( "Unknown" );

    ProductMessage productMessage;
    productMessage.mutable_wirelessstatus( )->set_configured( false );
    productMessage.mutable_wirelessstatus( )->set_frequencykhz( 0 );

    if( wirelessStatus.has_ssid( ) )
    {
        wirlessSsidString.assign( wirelessStatus.ssid( ) );
        productMessage.mutable_wirelessstatus( )->set_configured( true );
    }

    if( wirelessStatus.has_state( ) )
    {
        switch( wirelessStatus.state( ) )
        {
        case NetManager::Protobuf::WiFiStationState::WIFI_STATION_DISCONNECTED:
            wirlessStateString.assign( "Disconnected" );
            break;
        case NetManager::Protobuf::WiFiStationState::WIFI_STATION_CONNECTING:
            wirlessStateString.assign( "Connecting" );
            break;
        case NetManager::Protobuf::WiFiStationState::WIFI_STATION_CONNECTED:
            wirlessStateString.assign( "Connected" );
            break;
        case NetManager::Protobuf::WiFiStationState::WIFI_STATION_FAILED_WRONG_PASSWORD:
            wirlessStateString.assign( "Wrong Password" );
            break;
        case NetManager::Protobuf::WiFiStationState::WIFI_STATION_FAILED_TIMEOUT:
            wirlessStateString.assign( "Timed Out" );
            break;
        case NetManager::Protobuf::WiFiStationState::WIFI_STATION_FAILED_REJECTED:
            wirlessStateString.assign( "Failed" );
            break;
        default:
            break;
        }
    }

    if( wirelessStatus.has_frequencykhz( ) )
    {
        wirlessFrequencyInKhz.assign( std::to_string( wirelessStatus.frequencykhz( ) ) );
        productMessage.mutable_wirelessstatus( )->set_frequencykhz( wirelessStatus.frequencykhz( ) );
    }

    if( wirelessStatus.has_signaldbm( ) )
    {
        wirlessSignalDbm.assign( std::to_string( wirelessStatus.signaldbm( ) ) );
    }

    BOSE_DEBUG( s_logger, "----------------- Product Network Manager ------------------" );
    BOSE_DEBUG( s_logger, "The current wireless network has the following information:" );
    BOSE_DEBUG( s_logger, " " );
    BOSE_DEBUG( s_logger, "Wireless SSID  : %s ", wirlessSsidString.c_str( ) );
    BOSE_DEBUG( s_logger, "Wireless State : %s ", wirlessStateString.c_str( ) );
    BOSE_DEBUG( s_logger, "Frequency kHz  : %s ", wirlessFrequencyInKhz.c_str( ) );
    BOSE_DEBUG( s_logger, "Signal DBM     : %s ", wirlessSignalDbm.c_str( ) );
    BOSE_DEBUG( s_logger, " " );

    if( productMessage.mutable_wirelessstatus( )->configured( ) )
    {
        BOSE_DEBUG( s_logger, "A configured wireless network is currently available." );
        SendMessage( productMessage );
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name  ProductNetworkManager::HandleMessage
///
/// @brief ProductNetworkManager::HandleMessage
///
/// @param message
///
//////////////////////////////////////////////////////////////////////////////////////////////
void ProductNetworkManager::SendMessage( ProductMessage& message )
{
    IL::BreakThread( std::bind( m_ProductNotify, message ), m_mainTask );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief ProductNetworkManager::Stop
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductNetworkManager::Stop( void )
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
