////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file      ProductFrontDoorNetwork.h
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
/// The following compiler directive prevents this header file from being included more than
/// once, which will cause multiple declaration compiler errors, in any file that includes it.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////
///
///            Included Header Files
///
////////////////////////////////////////////////////////////////////////////////////////////////////
#include "NotifyTargetTaskIF.h"
#include "ProtoPersistenceIF.h"
#include "ConfigurationStatus.pb.h"
#include "Language.pb.h"
#include "ProductMessage.pb.h"
#include "CapsInitializationStatus.pb.h"
#include "NetworkService.pb.h"
#include "STSNetworkStatus.pb.h"
#include "NetManager.pb.h"
#include "BreakThread.h"
#include "APTaskFactory.h"
#include "FrontDoorClientIF.h"
#include "ProductMessage.pb.h"
#include "Language.pb.h"
#include "ConfigurationStatus.pb.h"

namespace ProductApp
{
////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @class ProductFrontDoorNetwork
///
/// @brief This class provides functionality for sending and receiving information through a
///        Front Door network router process, which handles connections and communications
///        between various Bose processes.
///
////////////////////////////////////////////////////////////////////////////////////////////////
class ProductFrontDoorNetwork
{
public:

    //////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @name   ProductFrontDoorNetwork::GetInstance
    ///
    /// @brief  This static method creates the one and only instance of the object
    ///         ProductFrontDoorNetwork. That only one instance is created in a thread safe
    ///         way is guaranteed by the C++ Version 11 compiler.
    ///
    /// @param  void This method does not take any arguments.
    ///
    /// @return This method returns a pointer to a singleton ProductFrontDoorNetwork object.
    ///
    //////////////////////////////////////////////////////////////////////////////////////////
    static ProductFrontDoorNetwork* GetInstance( NotifyTargetTaskIF*        mainTask,
            Callback< ProductMessage > ProductNotify );

    //////////////////////////////////////////////////////////////////////////////////////////
    /// This declaration is used to start and run an instance of the Front Door Network.
    //////////////////////////////////////////////////////////////////////////////////////////
    bool Run          ( void );
    void HandleMessage( ProductMessage& message );

private:

    //////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @name   ProductFrontDoorNetwork
    ///
    /// @brief  The constructor for this class is set to be private. This definition prevents
    ///         this class from being instantiated directly, so that only the static method
    ///         GetInstance to this class can be used to get the one sole instance of it.
    ///
    /// @param mainTask This argument points to a task to process resource requests and
    ///                 notifications.
    ///
    /// @param ProductNotify This is a callback to send events to the Product Controller.
    ///
    //////////////////////////////////////////////////////////////////////////////////////////
    ProductFrontDoorNetwork( NotifyTargetTaskIF*        mainTask,
                             Callback< ProductMessage > ProductNotify );

    //////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// The following copy constructor and equality operator for this class are private and
    /// are set to be undefined through the delete keyword. This prevents this class from
    /// being copied directly, so that only the static method GetInstance to this class can be
    /// used to get the one sole instance of it.
    ///
    //////////////////////////////////////////////////////////////////////////////////////////
    ProductFrontDoorNetwork ( ProductFrontDoorNetwork const& ) = delete;
    void operator =         ( ProductFrontDoorNetwork const& ) = delete;

    //////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// This declaration stores the main task for processing network events and requests. It
    /// is inherited by the ProductFrontDoorNetwork instance.
    ///
    //////////////////////////////////////////////////////////////////////////////////////////
    NotifyTargetTaskIF*        m_mainTask       = nullptr;
    Callback< ProductMessage > m_ProductNotify  = nullptr;
    NotifyTargetTaskIF*        m_networkTask    = nullptr;

    //////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// The following member is a shared pointer to the Front Door network interface for
    /// routing messages between the various Bose processes.
    ///
    //////////////////////////////////////////////////////////////////////////////////////////
    std::shared_ptr<FrontDoorClientIF> m_FrontDoorClient = nullptr;

    ProductPb::ConfigurationStatus          m_ConfigurationStatus;
    ProductPb::Language                     m_LanguageSettings;

    bool Run                             ( NotifyTargetTaskIF*        mainTask,
                                           Callback< ProductMessage > ProductNotify );

    void ServeRequests                   ( void );
    void GetCapsNotification             ( const SoundTouchInterface::CapsInitializationStatus& status );
    void HandleGetLanguageRequest        ( const Callback< ProductPb::Language >&  response );
    void HandlePostLanguageRequest       ( const ProductPb::Language&              language,
                                           const Callback< ProductPb::Language >&  response );

    void        SetSystemLanguageCode       ( std::string& systemLanguage );
    std::string GetSystemLanguageCode       ( void );
    void        HandleGetConfigurationStatusRequest( const Callback< ProductPb::ConfigurationStatus >&
            response );

    //////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief ProductFrontDoorNetwork::SetSystemLanguageCode
    ///
    /// @return
    ///
    //////////////////////////////////////////////////////////////////////////////////////////
    void MonitorNetwork              ( void );
    void GetNetworkStatusSuccess     ( const NetManager::Protobuf::NetworkStatus& networkStatus );
    void GetNetworkStatusNotification( const NetManager::Protobuf::NetworkStatus& networkStatus );
    void GetNetworkStatusFailed      ( const FRONT_DOOR_CLIENT_ERRORS             error );
    void ProcessNetworkStatus        ( const NetManager::Protobuf::NetworkStatus& networkStatus,
                                       bool                                       networkChanged );

    void SendMessage  ( ProductMessage& message );
};
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
