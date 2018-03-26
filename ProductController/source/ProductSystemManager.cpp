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
#include "EndPointsDefines.h"
#include "FrontDoorClient.h"
#include "ProtoPersistenceFactory.h"
#include "ProfessorProductController.h"
#include "ProductSystemManager.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
///                          Start of the Product Application Namespace                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
namespace ProductApp
{

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief Global Constants
///
////////////////////////////////////////////////////////////////////////////////////////////////////
const std::string g_ProductDirectory = "product-persistence/";

////////////////////////////////////////////////////////////////////////////////////////////////
///
/// The following constants define FrontDoor endpoints used by the SystemManager
///
////////////////////////////////////////////////////////////////////////////////////////////////
const std::string s_FrontDoorSystemConfigurationStatus = FRONTDOOR_SYSTEM_CONFIGURATION_STATUS_API;

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

    ////////////////////////////////////////////////////////////////////////////////////////////////
    /// Registration for other Bose client processes for getting configuration status settings is
    /// made through the FrontDoorClient. The callback HandleGetConfigurationStatusRequest is used
    /// to process configuration requests.
    ////////////////////////////////////////////////////////////////////////////////////////////////
    {
        AsyncCallback < Callback < ProductPb::ConfigurationStatus >, Callback<FrontDoor::Error>>
                callback( std::bind( &ProductSystemManager::HandleGetConfigurationStatusRequest,
                                     this,
                                     std::placeholders::_1,
                                     std::placeholders::_2 ),
                          m_ProductTask );

        m_FrontDoorClient->RegisterGet( s_FrontDoorSystemConfigurationStatus, callback );
    }

    BOSE_DEBUG( s_logger, "Registration for getting configuration status requests has been made." );

    ////////////////////////////////////////////////////////////////////////////////////////////////
    /// Registration is complete so return true.
    ////////////////////////////////////////////////////////////////////////////////////////////////
    return true;
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
void ProductSystemManager::HandleGetConfigurationStatusRequest( const Callback< ProductPb::ConfigurationStatus >& response,
                                                                const Callback<FrontDoor::Error>& errorRsp ) const
{
    BOSE_DEBUG( s_logger, "Sending the configuration status for a get request." );

    response.Send( m_ConfigurationStatus );
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
