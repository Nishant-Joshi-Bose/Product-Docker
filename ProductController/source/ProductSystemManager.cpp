////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file      ProductSystemManager.cpp
///
/// @brief     This header file contains functionality for sending and receiving system information
///            through a FrontDoor process.
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
#include "Services.h"
#include "BoseLinkServerMsgReboot.pb.h"
#include "BoseLinkServerMsgIds.pb.h"
#include "IPCDirectory.h"
#include "IPCDirectoryIF.h"
#include "BreakThread.h"
#include "NetworkPortDefines.h"
#include "IHsm.h"
#include "ProfessorProductController.h"
#include "ProductSystemManager.h"
#include "FrontDoorClient.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
///                          Start of the Product Application Namespace                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
namespace ProductApp
{

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief Global Constants
///
/// @todo  The location and name for files to store persistence data needs to coordinated with the
///        Eddie team. It will be helpful for debugging and testing efforts to have the persistence
///        in a single, known place. In ECO1, all persistent date was stored under the directory
///        /mnt/nv/BosePersistence/1.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
const std::string g_ProductDirectory = "product-persistence/";

////////////////////////////////////////////////////////////////////////////////////////////////
///
/// The following constants define FrontDoor endpoints used by the SystemManager
///
////////////////////////////////////////////////////////////////////////////////////////////////
constexpr char FRONTDOOR_SYSTEM_CONFIGURATION_STATUS[ ]       = "/system/configuration/status";
constexpr char FRONTDOOR_SYSTEM_CAPS_INITIALIZATION_STATUS[ ] = "/system/capsInitializationStatus";
constexpr char FRONTDOOR_SYSTEM_INFO[ ]                       = "/system/info";
constexpr char FRONTDOOR_CAPS_INITIALIZATION_UPDATE[ ]        = "CapsInitializationUpdate";
constexpr char FRONTDOOR_SYSTEM_STATE[ ]                      = "/system/state";

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductSystemManager::ProductSystemManager
///
/// @param ProfessorProductController& ProductController
///
////////////////////////////////////////////////////////////////////////////////////////////////////
ProductSystemManager::ProductSystemManager( ProfessorProductController&  ProductController ) :

    ///
    /// Product Controller and Front Door Interfaces
    ///
    m_ProductController( ProductController ),
    m_ProductTask( ProductController.GetTask( ) ),
    m_ProductNotify( ProductController.GetMessageHandler( ) ),
    m_FrontDoorClient( FrontDoor::FrontDoorClient::Create( "ProductSystemManager" ) ),
    ///
    /// Presistent Storage Initialization
    ///
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
    ReadConfigurationStatusFromPersistentStorage( );
    ReadSystemInfoSettingsFromPersistentStorage( );

    ////////////////////////////////////////////////////////////////////////////////////////////////
    /// Registration for other Bose client processes for getting configuration status settings is
    /// made through the FrontDoorClient. The callback HandleGetConfigurationStatusRequest is used
    /// to process configuration requests.
    ////////////////////////////////////////////////////////////////////////////////////////////////
    {
        AsyncCallback < Callback < ProductPb::ConfigurationStatus > >
        callback( std::bind( &ProductSystemManager::HandleGetConfigurationStatusRequest,
                             this,
                             std::placeholders::_1 ),
                  m_ProductTask );

        m_FrontDoorClient->RegisterGet( FRONTDOOR_SYSTEM_CONFIGURATION_STATUS, callback );
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
        ( FRONTDOOR_SYSTEM_CAPS_INITIALIZATION_STATUS, CallbackForSuccess, CallbackForFailure );

        m_FrontDoorClient->RegisterNotification< SoundTouchInterface::CapsInitializationStatus >
        ( FRONTDOOR_CAPS_INITIALIZATION_UPDATE, CallbackForNotification );
    }

    BOSE_DEBUG( s_logger, "A notification request for CAPS initialization messages has been made." );

    ////////////////////////////////////////////////////////////////////////////////////////////////
    /// Registration for other Bose client processes for getting system information settings is
    /// made through the FrontDoorClient. The callback HandleGetSystemInfoRequest is used to process
    /// system information get requests.
    ////////////////////////////////////////////////////////////////////////////////////////////////
    {
        AsyncCallback< Callback< ProductPb::SystemInfo > >
        asyncCallback( std::bind( &ProductSystemManager::HandleGetSystemInfoRequest,
                                  this,
                                  std::placeholders::_1 ),
                       m_ProductTask );

        m_FrontDoorClient->RegisterGet( FRONTDOOR_SYSTEM_INFO, asyncCallback );
    }

    BOSE_DEBUG( s_logger, "The registration for %s get request has been made.", FRONTDOOR_SYSTEM_INFO );

    ////////////////////////////////////////////////////////////////////////////////////////////////
    /// Registration for other Bose client processes for getting the system state is made
    /// through the FrontDoorClient. The callback HandleGetSystemStateRequest is used to process
    /// system state requests.
    ////////////////////////////////////////////////////////////////////////////////////////////////
    {
        AsyncCallback < Callback < ProductPb::SystemState > >
        callback( std::bind( &ProductSystemManager::HandleGetSystemStateRequest,
                             this,
                             std::placeholders::_1 ),
                  m_ProductTask );

        m_FrontDoorClient->RegisterGet( FRONTDOOR_SYSTEM_STATE, callback );
    }

    BOSE_DEBUG( s_logger, "Registration for getting system state requests has been made." );

    ////////////////////////////////////////////////////////////////////////////////////////////////
    /// Registration is complete so return true.
    ////////////////////////////////////////////////////////////////////////////////////////////////
    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief ProductSystemManager::ReadSystemInfoSettingsFromPersistentStorage
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductSystemManager::ReadSystemInfoSettingsFromPersistentStorage( )
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
            BOSE_DEBUG( s_logger, "Reading system information from persistent storage failed." );
            BOSE_DEBUG( s_logger, "Default system information value will be written to persistent storage." );

            m_SystemInfo.set_name( "Bose SoundTouch 1234" );

            ////////////////////////////////////////////////////////////////////////////////////////
            ///
            /// @todo  these parameters will need updating for final product
            ///
            ////////////////////////////////////////////////////////////////////////////////////////
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
            BOSE_ERROR( s_logger, "Writing default system information to persistent storage failed." );
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief ProductSystemManager::HandleGetSystemInfoRequest
///
/// @param const Callback<::ProductPb::SystemInfo>& response
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductSystemManager::HandleGetSystemInfoRequest(
    const Callback< ProductPb::SystemInfo >& response ) const
{
    BOSE_DEBUG( s_logger, "A system information get request was received." );

    response.Send( m_SystemInfo );
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
    ///       status becomes available the handling of the account may be processed inside the
    ///       ProductSystemManager class directly.
    ///
    ////////////////////////////////////////////////////////////////////////////////////////////////
    auto configurationStatus = m_ConfigurationStatus.mutable_status( );

    configurationStatus->set_network( network );
    configurationStatus->set_language( m_ProductController.IsSystemLanguageSet( ) );
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
            configurationStatus->set_language( m_ProductController.IsSystemLanguageSet( ) );

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
/// @param const Callback< ProductPb::ConfigurationStatus >& response
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductSystemManager::HandleGetConfigurationStatusRequest( const
                                                                Callback< ProductPb::ConfigurationStatus >&
                                                                response ) const
{
    BOSE_DEBUG( s_logger, "Sending the configuration status for a get request." );

    response.Send( m_ConfigurationStatus );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name  ProductSystemManager::HandleCapsStatus
///
/// @param const SoundTouchInterface::CapsInitializationStatus& status
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
/// @param const FRONT_DOOR_CLIENT_ERRORS error
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

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief ProductSystemManager::HandleGetSystemStateRequest
///
/// @param const ProductPb::SystemState& systemstate
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductSystemManager::HandleGetSystemStateRequest
( const Callback< ProductPb::SystemState >& stateResponse ) const
{
    Hsm::STATE  stateId   = m_ProductController.GetHsm( ).GetCurrentState( )->GetId( );
    std::string stateName = m_ProductController.GetHsm( ).GetCurrentState( )->GetName( );

    ProductPb::SystemState currentState;
    currentState.set_id( stateId );
    currentState.set_name( stateName );

    BOSE_DEBUG( s_logger, "--------------- Product Current State Request --------------" );
    BOSE_DEBUG( s_logger, "The current state name is %s with ID %u.",
                stateName.c_str( ),
                static_cast< unsigned int >( stateId ) );

    stateResponse.Send( currentState );
}

//////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name  ProductSystemManager::HandleMessage
///
/// @brief This method sends a ProductMessage Protocol Buffer to the product controller.
///
/// @param const ProductMessage& message
///
//////////////////////////////////////////////////////////////////////////////////////////////
void ProductSystemManager::SendMessage( const ProductMessage& message ) const
{
    IL::BreakThread( std::bind( m_ProductNotify, message ), m_ProductTask );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name  ProductSystemManager::Stop
///
/// @todo  Resources, memory, or any client server connections that may need to be released by
///        this module when stopped will need to be determined.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductSystemManager::Stop( )
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
