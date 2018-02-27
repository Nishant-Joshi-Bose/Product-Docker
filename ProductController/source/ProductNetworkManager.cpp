﻿////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file      ProductNetworkManager.cpp
///
/// @brief     This header file contains declarations for network management.
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
#include "Utilities.h"
#include "FrontDoorClient.h"
#include "NetManager.pb.h"
#include "ProductMessage.pb.h"
#include "ProfessorProductController.h"
#include "ProductNetworkManager.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
///                        Start of the Product Application Namespace                            ///
////////////////////////////////////////////////////////////////////////////////////////////////////
namespace ProductApp
{

////////////////////////////////////////////////////////////////////////////////////////////////
///
/// The following constants define FrontDoor endpoints used by the NetworkManager methods.
///
////////////////////////////////////////////////////////////////////////////////////////////////
constexpr char FRONTDOOR_NETWORK_STATUS[]       = "/network/status";
constexpr char FRONTDOOR_NETWORK_WIFI_PROFILE[] = "/network/wifi/profile";
constexpr char FRONTDOOR_NETWORK_WIFI_STATUS[]  = "/network/wifi/status";

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name  ProductNetworkManager::ProductNetworkManager
///
/// @param ProfessorProductController& ProductController
///
////////////////////////////////////////////////////////////////////////////////////////////////////
ProductNetworkManager::ProductNetworkManager( ProfessorProductController& ProductController )

    : m_ProductTask( ProductController.GetTask( ) ),
      m_ProductNotify( ProductController.GetMessageHandler( ) ),
      m_FrontDoorClient( FrontDoor::FrontDoorClient::Create( "ProductNetworkManager" ) ),
      m_WifiProfileCount( 0 )
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductNetworkManager::Run
///
/// @brief  This method attempts starts and runs the network manager. Essentially, it sets up
///         callbacks to respond to network status notifications.
///
/// @return This method returns true if successful.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool ProductNetworkManager::Run( )
{
    ///
    /// The following lambda callbacks are invoked for the network end points, whenever they all
    /// become ready or not ready, respectively; subsequent methods are made then for getting
    /// and registering for the associated network data.
    ///
    auto networkEndPointReadyFunction = [ this ]( const std::list< std::string >& endPointList )
    {
        if( endPointList.empty( ) )
        {
            BOSE_ERROR( s_logger, "No end points were specified as ready." );

            return;
        }

        for( const auto& endPoint : endPointList )
        {
            if( endPoint.compare( FRONTDOOR_NETWORK_STATUS ) == 0 )
            {
                BOSE_DEBUG( s_logger, "The network status end point %s is ready.", endPoint.c_str( ) );

                RegisterForNetworkStatus( );
            }
            else if( endPoint.compare( FRONTDOOR_NETWORK_WIFI_STATUS ) == 0 )
            {
                BOSE_DEBUG( s_logger, "The WiFi status end point %s is ready.", endPoint.c_str( ) );

                RegisterForWiFiStatus( );
            }
            else if( endPoint.compare( FRONTDOOR_NETWORK_WIFI_PROFILE ) == 0 )
            {
                BOSE_DEBUG( s_logger, "The WiFi profiles end point %s is ready.", endPoint.c_str( ) );

                RegisterForWiFiStatus( );
            }
            else
            {
                BOSE_ERROR( s_logger, "An unknown end point %s was sent as ready.", endPoint.c_str( ) );
            }
        }
    };

    auto networkEndPointNotReadyFunction = [ this ]( const std::list< std::string >& endPointList )
    {
        BOSE_ERROR( s_logger, "One or more network end point are not yet ready." );
    };

    AsyncCallback< std::list< std::string > >
        networkEndPointReadyCallback( networkEndPointReadyFunction, m_ProductTask );

    AsyncCallback< std::list< std::string > >
        networkEndPointNotReadyCallback( networkEndPointNotReadyFunction, m_ProductTask );

    ///
    /// The product now registers for all of its network end points of interest.
    ///
    std::list<std::string> networkEndPointList;

    networkEndPointList.push_back( FRONTDOOR_NETWORK_STATUS );
    networkEndPointList.push_back( FRONTDOOR_NETWORK_WIFI_STATUS );
    networkEndPointList.push_back( FRONTDOOR_NETWORK_WIFI_PROFILE );

    m_FrontDoorClient->RegisterEndpointsOfInterest( networkEndPointList,
                                                    networkEndPointReadyCallback,
                                                    networkEndPointNotReadyCallback );

    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name  ProductNetworkManager::RegisterForNetworkStatus
///
/// @brief This method gets and registers for the status of the entire network. The initial get is
///        called to get the intial network status; this ensures that the product will not need to
///        wait for this information until a change in network status occurs.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductNetworkManager::RegisterForNetworkStatus( )
{
    auto functionForNetworkStatusSuccess = [ this ]( const NetManager::Protobuf::NetworkStatus & status )
    {
        HandleNetworkStatus( status );
    };

    auto functionForNetworkStatusFailure = [ this ]( const EndPointsError::Error & error )
    {
        BOSE_ERROR( s_logger, "An error code %d %d <%s> was returned from a network status request.",
                    error.code( ),
                    error.subcode( ),
                    error.message( ).c_str( ) );
    };

    AsyncCallback< NetManager::Protobuf::NetworkStatus >
        callbackForNetworkStatusSuccess( functionForNetworkStatusSuccess, m_ProductTask );

    AsyncCallback< EndPointsError::Error >
        callbackForNetworkStatusFailure( functionForNetworkStatusFailure, m_ProductTask );

    m_FrontDoorClient->RegisterNotification< NetManager::Protobuf::NetworkStatus >(
        FRONTDOOR_NETWORK_STATUS,
        callbackForNetworkStatusSuccess );

    m_FrontDoorClient->SendGet< NetManager::Protobuf::NetworkStatus, EndPointsError::Error >(
        FRONTDOOR_NETWORK_STATUS,
        callbackForNetworkStatusSuccess,
        callbackForNetworkStatusFailure );

    BOSE_DEBUG( s_logger, "A notification and get request for network status changes has been made." );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief ProductNetworkManager::RegisterForWiFiStatus
///
/// @brief This method gets and registers for the status of the WiFi network. The initial get is
///        called to get the intial WiFi status; this ensures that the product will not need to
///        wait for this information until a change in the WiFi status occurs.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductNetworkManager::RegisterForWiFiStatus( )
{
    auto functionForWiFiStatusSuccess = [ this ]( const NetManager::Protobuf::WiFiStatus & status )
    {
        HandleWiFiStatus( status );
    };

    auto functionForWiFiStatusFailure = [ this ]( const EndPointsError::Error & error )
    {
        BOSE_ERROR( s_logger, "An error code %d %d <%s> was returned from a WiFi status request.",
                    error.code( ),
                    error.subcode( ),
                    error.message( ).c_str( ) );
    };

    AsyncCallback< NetManager::Protobuf::WiFiStatus >
        callbackForWiFiStatusSuccess( functionForWiFiStatusSuccess, m_ProductTask );

    AsyncCallback< EndPointsError::Error >
        callbackForWiFiStatusFailure( functionForWiFiStatusFailure, m_ProductTask );

    m_FrontDoorClient->RegisterNotification< NetManager::Protobuf::WiFiStatus >(
        FRONTDOOR_NETWORK_WIFI_STATUS,
        callbackForWiFiStatusSuccess );

    m_FrontDoorClient->SendGet< NetManager::Protobuf::WiFiStatus, EndPointsError::Error >(
        FRONTDOOR_NETWORK_WIFI_STATUS,
        callbackForWiFiStatusSuccess,
        callbackForWiFiStatusFailure );

    BOSE_DEBUG( s_logger, "A notification and get request for wireless status changes has been made." );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief ProductNetworkManager::RegisterForWiFiProfiles
///
/// @brief This method gets and registers for a list of WiFi profiles. The initial get is
///        called to get the intial WiFi profiles list; this ensures that the product will not need
///        to wait for this information until a change in the WiFi profiles list occurs.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductNetworkManager::RegisterForWiFiProfiles( )
{
    auto functionForWiFiProfilesSuccess = [ this ]( const NetManager::Protobuf::WiFiProfiles & profiles )
    {
        HandleWiFiProfiles( profiles );
    };

    auto functionForWiFiProfilesFailure = [ this ]( const EndPointsError::Error & error )
    {
        BOSE_ERROR( s_logger, "An error code %d %d <%s> was returned from a WiFi profiles request.",
                    error.code( ),
                    error.subcode( ),
                    error.message( ).c_str( ) );
    };

    AsyncCallback< NetManager::Protobuf::WiFiProfiles >
        callbackForWiFiProfilesSuccess( functionForWiFiProfilesSuccess, m_ProductTask );

    AsyncCallback< EndPointsError::Error >
        callbackForWiFiProfilesFailure( functionForWiFiProfilesFailure, m_ProductTask );

    m_FrontDoorClient->RegisterNotification< NetManager::Protobuf::WiFiProfiles >(
        FRONTDOOR_NETWORK_WIFI_PROFILE,
        callbackForWiFiProfilesSuccess );

    m_FrontDoorClient->SendGet< NetManager::Protobuf::WiFiProfiles, EndPointsError::Error >(
        FRONTDOOR_NETWORK_WIFI_PROFILE,
        callbackForWiFiProfilesSuccess,
        callbackForWiFiProfilesFailure );

    BOSE_DEBUG( s_logger, "A notification and get request for wireless profile changes has been made." );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name  ProductNetworkManager::HandleNetworkStatus
///
/// @brief This method processes the network status received, and will send a ProductMessage to
///        notify the product controller of the network state, configured, and connected status.
///
/// @param NetManager::Protobuf::NetworkStatus& networkStatus  This parameter is a Google Protocol
///                                                            Buffer that contains a status on
///                                                            the currently available networks.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductNetworkManager::HandleNetworkStatus( const NetManager::Protobuf::NetworkStatus&
                                                 networkStatus )
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
        bool connectedNetworkExists = false;
        bool configuredNetworkExists = false;

        BOSE_DEBUG( s_logger, "-------------- Product Network Manager Status --------------" );

        if( networkStatus.has_primary( ) )
        {
            BOSE_DEBUG( s_logger, "The primary network is type is %s.",
                        NetworkType_Name( networkStatus.primary( ) ).c_str( ) );
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

        BOSE_DEBUG( s_logger, "There are %d networks currently available.", networkStatus.interfaces_size( ) );
        BOSE_DEBUG( s_logger, " " );

        for( int index = 0; index < networkStatus.interfaces_size( ); index++ )
        {
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
                if( networkStatus.interfaces( index ).state( ) ==
                    NetManager::Protobuf::NetworkInterface_State::NetworkInterface_State_DOWN )
                {
                    networkStateString.assign( "Down" );
                }
                else if( networkStatus.interfaces( index ).state( ) ==
                         NetManager::Protobuf::NetworkInterface_State::NetworkInterface_State_UP )
                {
                    networkStateString.assign( "Up" );
                }
            }

            if( networkStatus.interfaces( index ).has_type( ) )
            {
                networkTypeString.assign( NetworkType_Name( networkStatus.interfaces( index ).type( ) ) );
            }

            if( networkStatus.interfaces( index ).has_state( ) and
                networkStatus.interfaces( index ).has_type( ) )
            {
                auto const& networkType = networkStatus.interfaces( index ).type( );
                auto const& networkState = networkStatus.interfaces( index ).state( );

                ///
                /// Only wired Ethernet and wireless network connections are of interest here.
                /// Other network types as wireless access points AP for network setup are excluded.
                /// A network is configured if it is in an up state. It is also considered to be
                /// configured and connected if it has an IP address. Note that only the existence
                /// of a configured or connected network is of concern, so that if another
                /// unconfigured or unconnected network is found do not send the additional status.
                /// This may otherwise set the product controller to a network unconnected or
                /// unconfigured status. Only when there are no configured or connected network
                /// interfaces is a unconfigured unconnected status sent to the product controller.
                ///
                if( networkState == NetManager::Protobuf::NetworkInterface_State::NetworkInterface_State_UP
                    and ( networkType == NetManager::Protobuf::NetworkType::WIRELESS or
                          networkType == NetManager::Protobuf::NetworkType::WIRED_ETH ) )
                {
                    if( networkStatus.interfaces( index ).has_ipinfo( ) and
                        networkStatus.interfaces( index ).ipinfo( ).has_ipaddress( ) )
                    {
                        if( not connectedNetworkExists )
                        {
                            ProductMessage productMessage;
                            auto networkData = productMessage.mutable_networkstatus( );

                            networkData->set_configured( true );
                            networkData->set_connected( true );

                            if( networkType == NetManager::Protobuf::NetworkType::WIRELESS )
                            {
                                networkData->set_networktype( ProductNetworkStatus_ProductNetworkType_Wireless );
                            }
                            else if( networkType ==  NetManager::Protobuf::NetworkType::WIRED_ETH )
                            {
                                networkData->set_networktype( ProductNetworkStatus_ProductNetworkType_Wired );
                            }

                            SendMessage( productMessage );

                            connectedNetworkExists = true;
                            configuredNetworkExists = true;
                        }
                    }
                    else
                    {
                        if( not configuredNetworkExists )
                        {
                            ProductMessage productMessage;
                            auto networkData = productMessage.mutable_networkstatus( );

                            networkData->set_configured( true );
                            networkData->set_connected( false );

                            if( networkType == NetManager::Protobuf::NetworkType::WIRELESS )
                            {
                                networkData->set_networktype( ProductNetworkStatus_ProductNetworkType_Wireless );
                            }
                            else if( networkType ==  NetManager::Protobuf::NetworkType::WIRED_ETH )
                            {
                                networkData->set_networktype( ProductNetworkStatus_ProductNetworkType_Wired );
                            }

                            SendMessage( productMessage );

                            configuredNetworkExists = true;
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

            BOSE_VERBOSE( s_logger, "Interface Index   : %d ", index );
            BOSE_VERBOSE( s_logger, "Interface MAC     : %s ", interfaceMacString.c_str( ) );
            BOSE_VERBOSE( s_logger, "Interface Name    : %s ", interfaceNameString.c_str( ) );
            BOSE_VERBOSE( s_logger, "Network State     : %s ", networkStateString.c_str( ) );
            BOSE_VERBOSE( s_logger, "Network Type      : %s ", networkTypeString.c_str( ) );
            BOSE_VERBOSE( s_logger, "IP Address        : %s ", ipAddressString.c_str( ) );
            BOSE_VERBOSE( s_logger, "Broadcast Address : %s ", broadcastString.c_str( ) );
            BOSE_VERBOSE( s_logger, "Subnet Mask       : %s ", subnetMaskString.c_str( ) );
            BOSE_VERBOSE( s_logger, "Gateway Address   : %s ", gatewayAddressString.c_str( ) );
            BOSE_VERBOSE( s_logger, "Primary DNS       : %s ", primaryDnsString.c_str( ) );
            BOSE_VERBOSE( s_logger, "Secondary DNS     : %s ", secondaryDnsString.c_str( ) );
            BOSE_VERBOSE( s_logger, " " );
        }

        ///
        /// Since no configured or connected network interfaces are available, send an unconfigured
        /// unconnected network status to the product controller.
        ///
        if( not connectedNetworkExists and not configuredNetworkExists )
        {
            ProductMessage productMessage;
            auto networkData = productMessage.mutable_networkstatus( );

            networkData->set_configured( false );
            networkData->set_connected( false );
            networkData->set_networktype( ProductNetworkStatus_ProductNetworkType_Unknown );

            SendMessage( productMessage );
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name ProductNetworkManager::PerformRequestforWiFiProfiles
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductNetworkManager::PerformRequestforWiFiProfiles()
{
    auto CallbackForWiFiProfilesSuccess = [ this ]( const NetManager::Protobuf::WiFiProfiles & profiles )
    {
        HandleWiFiProfiles( profiles );
    };

    auto CallbackForWiFiProfilesFailure = [ this ]( const EndPointsError::Error & error )
    {
        BOSE_ERROR( s_logger, "An error code %d %d <%s> was returned from a WiFi profiles request.",
                    error.code( ),
                    error.subcode( ),
                    error.message( ).c_str( ) );
    };

    m_FrontDoorClient->SendGet< NetManager::Protobuf::WiFiProfiles, EndPointsError::Error >(
        FRONTDOOR_NETWORK_WIFI_PROFILE,
        CallbackForWiFiProfilesSuccess,
        CallbackForWiFiProfilesFailure );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name  ProductNetworkManager::HandleWiFiProfiles
///
/// @brief This method processes the wireless network profiles received, and will send a
///        ProductMessage to notify the Product Controller of the wireless network configured state.
///
/// @param NetManager::Protobuf::WiFiProfiles& wirelessStatus This argument is a Google Protocol
///                                                           Buffer that contains information on
///                                                           the currently available wireless
///                                                           network profiles.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductNetworkManager::HandleWiFiProfiles( const NetManager::Protobuf::WiFiProfiles&
                                                wirelessProfiles )
{
    m_WifiProfileCount = wirelessProfiles.profiles_size( );

    if( m_WifiProfileCount == 0 )
    {
        BOSE_DEBUG( s_logger, "----------------- Product Network Manager ------------------" );
        BOSE_DEBUG( s_logger, "No wireless networks are currently configured." );
    }
    else
    {
        BOSE_DEBUG( s_logger, "----------------- Product Network Manager ------------------" );
        BOSE_DEBUG( s_logger, "There are %d wireless networks currently available.", m_WifiProfileCount );
        BOSE_DEBUG( s_logger, " " );

        ProductMessage productMessage;
        productMessage.mutable_wirelessstatus( )->set_configured( false );
        productMessage.mutable_wirelessstatus( )->set_frequencykhz( 0 );

        for( uint32_t index = 0; index < m_WifiProfileCount; index++ )
        {
            std::string WirlessSsidString( "Unknown" );
            std::string WirlessSecurityString( "Unknown" );

            if( wirelessProfiles.profiles( index ).has_ssid( ) )
            {
                WirlessSsidString.assign( wirelessProfiles.profiles( index ).ssid( ) );

                productMessage.mutable_wirelessstatus( )->set_configured( true );
            }

            if( wirelessProfiles.profiles( index ).has_security( ) )
            {
                WirlessSecurityString.assign( wirelessProfiles.profiles( index ).security( ) );
            }

            BOSE_VERBOSE( s_logger, "Wireless Index    : %d ", index );
            BOSE_VERBOSE( s_logger, "Wireless SSID     : %s ", WirlessSsidString.c_str( ) );
            BOSE_VERBOSE( s_logger, "Wireless Security : %s ", WirlessSecurityString.c_str( ) );
            BOSE_VERBOSE( s_logger, " " );
        }

        ///
        /// If a configured wireless network is available, send a message to the Product Controller.
        /// Otherwise, do not send a network unconfigured message, since a wired Ethernet network
        /// may already be configured and connected.
        ///
        if( productMessage.mutable_wirelessstatus( )->configured( ) )
        {
            SendMessage( productMessage );
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name  ProductNetworkManager::HandleWiFiStatus
///
/// @brief This method processes the WiFi status received, and will send a ProductMessage to
///        notify the Product Controller of a WiFi network configured state if configured.
///
/// @param NetManager::Protobuf::WiFiProfiles& wirelessStatus This argument is a Google Protocol
///                                                           Buffer that contains information on
///                                                           the currently available wireless
///                                                           networks.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductNetworkManager::HandleWiFiStatus( const NetManager::Protobuf::WiFiStatus& wirelessStatus )
{
    ///
    /// Only process the wireless network status if it has an SSID, which indicates that it is
    /// configurable. Send the frequency to the LPM hardware if available.
    ///
    std::string wirlessSsidString( "Unknown" );
    std::string wirlessStateString( "Unknown" );
    std::string wirlessFrequencyInKhz( "Unknown" );
    std::string wirlessSignalDbm( "Unknown" );

    ProductMessage productMessage;
    productMessage.mutable_wirelessstatus( )->set_configured( wirelessStatus.has_ssid( ) );

    if( wirelessStatus.has_frequencykhz( ) )
    {
        wirlessFrequencyInKhz.assign( std::to_string( wirelessStatus.frequencykhz( ) ) );
        productMessage.mutable_wirelessstatus( )->set_frequencykhz( wirelessStatus.frequencykhz( ) );
    }

    if( wirelessStatus.has_ssid( ) )
    {
        wirlessSsidString.assign( wirelessStatus.ssid( ) );
        productMessage.mutable_wirelessstatus( )->set_configured( true );
    }

    if( wirelessStatus.has_state( ) )
    {
        wirlessStateString.assign( WiFiStationState_Name( wirelessStatus.state( ) ) );
    }


    if( wirelessStatus.has_signaldbm( ) )
    {
        wirlessSignalDbm.assign( std::to_string( wirelessStatus.signaldbm( ) ) );
    }

    BOSE_VERBOSE( s_logger, "----------------- Product Network Manager ------------------" );
    BOSE_VERBOSE( s_logger, "The current wireless network has the following information:" );
    BOSE_VERBOSE( s_logger, " " );
    BOSE_VERBOSE( s_logger, "Wireless SSID  : %s ", wirlessSsidString.c_str( ) );
    BOSE_VERBOSE( s_logger, "Wireless State : %s ", wirlessStateString.c_str( ) );
    BOSE_VERBOSE( s_logger, "Frequency kHz  : %s ", wirlessFrequencyInKhz.c_str( ) );
    BOSE_VERBOSE( s_logger, "Signal DBM     : %s ", wirlessSignalDbm.c_str( ) );
    BOSE_VERBOSE( s_logger, " " );

    if( productMessage.mutable_wirelessstatus( )->configured( ) )
    {
        SendMessage( productMessage );
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name  ProductNetworkManager::SendMessage
///
/// @brief This method sends a ProductMessage Protocol Buffer to the product controller.
///
/// @param ProductMessage& message
///
//////////////////////////////////////////////////////////////////////////////////////////////
void ProductNetworkManager::SendMessage( ProductMessage& message )
{
    IL::BreakThread( std::bind( m_ProductNotify, message ), m_ProductTask );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name ProductNetworkManager::Stop
///
/// @todo  Resources, memory, or any client server connections that may need to be released by
///        this module when stopped will need to be determined.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductNetworkManager::Stop( )
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
