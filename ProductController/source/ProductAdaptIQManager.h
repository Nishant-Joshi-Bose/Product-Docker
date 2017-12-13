////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file      ProductAdaptIQManager.h
///
/// @brief     This file contains the source code to handle communication with A4VVideoManager service
///
/// @author    Chris Houston
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


///////////////////////////////////////////////////////////////////////////////////////////////////
///
/// The following compiler directive prevents this header file from being included more than once,
/// which may cause multiple declaration compiler errors.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////
///
///            Included Header Files
///
////////////////////////////////////////////////////////////////////////////////////////////////////
#include "NotifyTargetTaskIF.h"
#include "FrontDoorClient.h"
#include "AdaptIQ.pb.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
///                          Start of the Product Application Namespace                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
namespace ProductApp
{

////////////////////////////////////////////////////////////////////////////////////////////////////
///
///            Included Subclasses
///
////////////////////////////////////////////////////////////////////////////////////////////////////
class CustomProductHardwareInterface;
class ProductController;

class ProductAdaptIQManager
{
public:
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @name   ProductAdaptIQManager::ProductAdaptIQManager
    ///
    /// @param ProductController
    ///
    /// @return This method does not return anything.
    ///
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    ProductAdaptIQManager( ProfessorProductController& ProductController );

    //////////////////////////////////////////////////////////////////////////////////////////////
    /// This declaration is used to start and run the hardware manager.
    //////////////////////////////////////////////////////////////////////////////////////////////
    void Run( void );
    void Stop( void );

private:
    //////////////////////////////////////////////////////////////////////////////////////////////
    /// These declarations store the main task for processing LPM hardware events and requests. It
    /// is passed by the ProductController instance.
    //////////////////////////////////////////////////////////////////////////////////////////////
    NotifyTargetTaskIF*        m_ProductTask        = nullptr;
    Callback< ProductMessage > m_ProductNotify      = nullptr;

    //////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief The following method provides for one-time initialization after the constructor
    ///        has completed
    ///
    //////////////////////////////////////////////////////////////////////////////////////////////
    void Initialize();


    //////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief The following subclass instance is used to manage the lower level hardware and
    ///        the device.
    ///
    //////////////////////////////////////////////////////////////////////////////////////////////
    std::shared_ptr<CustomProductHardwareInterface> m_ProductHardwareInterface;

    //////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief The following subclass instance is used to communicate with the FrontDoor.
    ///
    //////////////////////////////////////////////////////////////////////////////////////////////
    std::shared_ptr<FrontDoorClientIF>      m_FrontDoorClient;

    //////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief The following methods handle interaction with the FrontDoor.
    ///
    //////////////////////////////////////////////////////////////////////////////////////////////
    void HandleGet( ProductPb::AdaptIQStatus& );
    void HandlePut( const ProductPb::AdaptIQReq&, ProductPb::AdaptIQReq& );

    //////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief The following members store instances of the FrontDoor callbacks.
    ///
    //////////////////////////////////////////////////////////////////////////////////////////////
    CallbackConnection      m_PutConnection;
    CallbackConnection      m_GetConnection;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
///                           End of the Product Application Namespace                           ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
