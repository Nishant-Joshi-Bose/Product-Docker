////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file      ProductNetworkManager.h
///
/// @brief     This header file contains functionality for for network management.
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

////////////////////////////////////////////////////////////////////////////////////////////////////
///                             Start of Product Namespace                                       ///
////////////////////////////////////////////////////////////////////////////////////////////////////
namespace ProductApp
{

////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @class ProductNetworkManager
///
/// @brief This class provides functionality for sending and receiving information through a
///        Front Door network router process, which handles connections and communications
///        between various Bose processes.
///
////////////////////////////////////////////////////////////////////////////////////////////////
class ProductNetworkManager
{
public:

    //////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @name   ProductNetworkManager::GetInstance
    ///
    /// @brief  This static method creates the one and only instance of the object
    ///         ProductNetworkManager. That only one instance is created in a thread safe
    ///         way is guaranteed by the C++ Version 11 compiler.
    ///
    /// @param  void This method does not take any arguments.
    ///
    /// @return This method returns a pointer to a singleton ProductNetworkManager object.
    ///
    //////////////////////////////////////////////////////////////////////////////////////////
    static ProductNetworkManager* GetInstance( NotifyTargetTaskIF*        mainTask,
                                               Callback< ProductMessage > ProductNotify );

    //////////////////////////////////////////////////////////////////////////////////////////
    /// This declaration is used to start and run an instance of the Front Door Network.
    //////////////////////////////////////////////////////////////////////////////////////////
    bool Run( void );
    void HandleMessage( ProductMessage& message );
    void Stop( void );

private:

    //////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @name   ProductNetworkManager
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
    ProductNetworkManager( NotifyTargetTaskIF*        mainTask,
                           Callback< ProductMessage > ProductNotify );

    //////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// The following copy constructor and equality operator for this class are private and
    /// are set to be undefined through the delete keyword. This prevents this class from
    /// being copied directly, so that only the static method GetInstance to this class can be
    /// used to get the one sole instance of it.
    ///
    //////////////////////////////////////////////////////////////////////////////////////////
    ProductNetworkManager( ProductNetworkManager const& ) = delete;
    ProductNetworkManager operator = ( ProductNetworkManager const& ) = delete;

    //////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// This declaration stores the main task for processing network events and requests. It
    /// is inherited by the ProductNetworkManager instance.
    ///
    //////////////////////////////////////////////////////////////////////////////////////////
    NotifyTargetTaskIF*                m_mainTask;
    Callback< ProductMessage >         m_ProductNotify;
    std::shared_ptr<FrontDoorClientIF> m_FrontDoorClient;

    //////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// The following methods are used to handle the network status.
    ///
    //////////////////////////////////////////////////////////////////////////////////////////
    void MonitorNetwork( void );
    void GetEntireNetworkStatus( const NetManager::Protobuf::NetworkStatus&    networkStatus );
    void GetWirelessNetworkProfiles( const NetManager::Protobuf::WiFiProfiles& wirelessProfiles );
    void GetWirelessNetworkStatus( const NetManager::Protobuf::WiFiStatus&     wirelessStatus );

    void SendMessage( ProductMessage& message );
};

////////////////////////////////////////////////////////////////////////////////////////////////////
///                               End of ProductApp Namespace                                    ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
