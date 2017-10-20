////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file      ProductEdidInterface.h
///
/// @brief     This file contains the source code to handle communication with A4VVideoManager service
///
/// @author    Manoranjani Malisetti
///
/// @date      10/19/2017
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
#include "ThreadMutex.h"
#include "NotifyTargetTaskIF.h"
#include "APTask.h"
#include "APClientSocketListenerIF.h"
#include "APServerSocketListenerIF.h"
#include "IPCMessageRouterIF.h"
#include "APProductIF.h"
#include "ProductMessage.pb.h"
#include "A4VVideoManagerClientFactory.h"
#include "A4V_VideoManagerClientIF.h"
#include "ProductHardwareInterface.h"

namespace ProductApp
{
////////////////////////////////////////////////////////////////////////////////////////////////////
///
///            Included Subclasses
///
////////////////////////////////////////////////////////////////////////////////////////////////////
class ProductHardwareInterface;
class ProductController;

class ProductEdidInterface
{
public:

    //////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief The following aliases refer to the Bose Sound Touch SDK utilities for inter-process
    ///        and inter-thread communications.
    ///
    //////////////////////////////////////////////////////////////////////////////////////////////
    typedef APClientSocketListenerIF::ListenerPtr   ClientListener;
    typedef APClientSocketListenerIF::SocketPtr     ClientSocket;
    typedef APServerSocketListenerIF::ListenerPtr   ServerListener;
    typedef APServerSocketListenerIF::SocketPtr     ServerSocket;
    typedef IPCMessageRouterIF::IPCMessageRouterPtr MessageRouter;


    //////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @name   ProductEdidInterface::GetInstance
    ///
    /// @brief  This static method creates the one and only instance of a ProductEdidInterface
    ///         object. That only one instance is created in a thread safe way is guaranteed by
    ///         the C++ Version 11 compiler.
    ///
    /// @param  task [input]         This argument specifies the task in which to run the hardware
    ///                               interface.
    ///
    /// @param  ProductNotifyCallback This argument specifies a callback to send messages back to
    ///                               the product controller.
    ///
    /// @return This method returns a reference to a ProductEdidInterface object.
    ///
    //////////////////////////////////////////////////////////////////////////////////////////////
    static ProductEdidInterface* GetInstance( NotifyTargetTaskIF*        task,
                                              Callback< ProductMessage > ProductNotifyCallback,
                                              ProductHardwareInterface*  HardwareInterface );

    //////////////////////////////////////////////////////////////////////////////////////////////
    /// This declaration is used to start and run the hardware manager.
    //////////////////////////////////////////////////////////////////////////////////////////////
    bool Run( void );

private:

    ////////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief ProductEdidInterface
    ///
    /// @brief  The constructor for this class is set to be private. This definition prevents this
    ///         class from being instantiated directly, so that only the static method GetInstance
    ///         to this class can be used to get the one sole instance of it.
    ///
    /// @param  task
    ///
    /// @param ProductNotifyCallback
    ///
    ////////////////////////////////////////////////////////////////////////////////////////////////
    ProductEdidInterface( NotifyTargetTaskIF*        task,
                          Callback< ProductMessage > ProductNotifyCallback,
                          ProductHardwareInterface*  HardwareInterface );

    //////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief The following copy constructor and equality operator for this class are private
    ///        and are set to be undefined through the delete keyword. This prevents this class
    ///        from being copied directly, so that only the static method GetInstance to this
    ///        class can be used to get the one sole instance of it.
    ///
    //////////////////////////////////////////////////////////////////////////////////////////////
    ProductEdidInterface( ProductEdidInterface const& ) = delete;
    void operator     = ( ProductEdidInterface const& ) = delete;

    //////////////////////////////////////////////////////////////////////////////////////////////
    /// These declarations store the main task for processing LPM hardware events and requests. It
    /// is passed by the ProductController instance.
    //////////////////////////////////////////////////////////////////////////////////////////////
    NotifyTargetTaskIF*        m_mainTask       = nullptr;
    Callback< ProductMessage > m_ProductNotify  = nullptr;
    A4VVideoManager::A4VVideoManagerClientIF::A4VVideoManagerClientPtr m_EdidClient = nullptr;

    //////////////////////////////////////////////////////////////////////////////////////////////
    /// This member determines whether a connections to the LPM server connection is established.
    //////////////////////////////////////////////////////////////////////////////////////////////
    bool m_connected = false;

    //////////////////////////////////////////////////////////////////////////////////////////////
    /// This method is called when an A4VVM server connection is established.
    //////////////////////////////////////////////////////////////////////////////////////////////
    void Connected( bool  connected );

    //////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief The following method is used to register for and receive key events from the
    ///        A4VVideoManager interface.
    ///
    //////////////////////////////////////////////////////////////////////////////////////////////
    void HandleHpdEvent( A4VVideoManagerServiceMessages::EventHDMIMsg_t hpdEvent );
    void HandleRawEDIDResponse( const A4VVideoManagerServiceMessages::EDIDRawMsg_t rawEdid );
    void HandlePhyAddrResponse( const A4VVideoManagerServiceMessages::CECPhysicalAddrMsg_t cecPhysicalAddress );

    //////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief The following subclass instances are used to manage the lower level hardware and
    ///        the device.
    ///
    //////////////////////////////////////////////////////////////////////////////////////////////
    ProductHardwareInterface* m_ProductHardwareInterface = nullptr;

};
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////