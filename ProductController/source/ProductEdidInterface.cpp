////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file      ProductEdidInterface.cpp
///
/// @brief     This header file contains declarations for managing the interface with A4VVideoManager
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

////////////////////////////////////////////////////////////////////////////////////////////////////
///
///            Included Header Files
///
////////////////////////////////////////////////////////////////////////////////////////////////////
#include "SystemUtils.h"
#include "DPrint.h"
#include "Services.h"
#include "RebroadcastLatencyMode.pb.h"
#include "ProductEdidInterface.h"
#include "ProductController.h"
#include "ProductMessage.pb.h"
#include "BreakThread.h"
#include "RivieraLPM_IpcProtocol.h"
#include "A4V_VideoManagerMessageIDs.pb.h"
#include "A4V_VideoManager.pb.h"
#include "IPCMessageRouterFactory.h"



////////////////////////////////////////////////////////////////////////////////////////////////////
///                             Start of Product Namespace                                       ///
////////////////////////////////////////////////////////////////////////////////////////////////////
namespace ProductApp
{

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief The following aliases refer to the Bose Sound Touch class utilities for inter-process and
///        inter-thread communications.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
typedef APClientSocketListenerIF::ListenerPtr   ClientPointer;
typedef APServerSocketListenerIF::ListenerPtr   ServerPointer;
typedef APClientSocketListenerIF::SocketPtr     ClientSocket;
typedef APServerSocketListenerIF::SocketPtr     ServerSocket;
typedef IPCMessageRouterIF::IPCMessageRouterPtr RouterPointer;


////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// The following declares a DPrint class type object for logging information in this source code
/// file.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
static const DPrint s_logger { "Product" };

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductEdidInterface::GetInstance
///
/// @brief  This static method creates the one and only instance of a ProductEdidInterface object.
///         The C++ Version 11 compiler guarantees that only one instance is created in a thread
///         safe way.
///
/// @param mainTask
///
/// @param ProductNotify
///
/// @return This method returns a pointer to a ProductEdidInterface object.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
ProductEdidInterface* ProductEdidInterface::GetInstance( NotifyTargetTaskIF*        mainTask,
        Callback< ProductMessage > ProductNotify )
{
    static ProductEdidInterface* instance = new ProductEdidInterface( mainTask, ProductNotify );

    BOSE_DEBUG( s_logger, "The instance %8p of the Product Command Line was returned.", instance );
    return instance;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductEdidInterface::
///
/// @brief  This method is the ProductEdidInterface constructor, which is declared as being private to
///         ensure that only one instance of this class can be created through the class GetInstance
///         method.
///
/// @param  void This method does not take any arguments.
///
/// @return This method does not return anything.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
ProductEdidInterface::ProductEdidInterface( NotifyTargetTaskIF*        mainTask,
                                        Callback< ProductMessage > ProductNotify )
    : m_mainTask( mainTask ),
      m_ProductNotify( ProductNotify ),
      m_connected( false )

{
    return;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductEdidInterface::Run
///
/// @brief  This method starts the main task for the ProductEdidInterface instance. The OnEntry method
///         for the ProductEdidInterface instance is called just before the main task starts. Also,
///         this main task is used for most of the internal processing for each of the subclass
///         instances.
///
/// @param  void This method does not take any arguments.
///
/// @return This method does not return anything.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool ProductEdidInterface::Run( )
{
    BOSE_DEBUG( s_logger, "The hardware connection to the A4VVideoManager is being established." );
    m_EdidClient = A4VVideoManagerClientFactory::Create( "ProductEdidInterface", m_mainTask );
    Callback< bool > ConnectedCallback( std::bind( &ProductEdidInterface::Connected,
                                                   this,
                                                   std::placeholders::_1 ) );

    m_EdidClient->Connect( ConnectedCallback );

    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductEdidInterface::Connected
///
/// @brief  This method sets up the LPM hardware client.
///
/// @param  void This method does not take any arguments.
///
/// @return This method does not return anything.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductEdidInterface::Connected( bool connected )
{
    
    if( !connected )
    {
        BOSE_DEBUG( s_logger, "Connection to A4VVideoManager could not be established." );
        BOSE_DEBUG( s_logger, "An attempt to reconnect to A4VVideoManager will be made." );

        m_connected = false;

        IL::BreakThread( std::bind( &ProductEdidInterface::Run, this ), m_mainTask );

        return;
    }
    else
    {
        BOSE_DEBUG( s_logger, "A hardware connection to A4VVideoManager has been established." );
        BOSE_DEBUG( s_logger, "An attempt to register for HPD will now be made." );

        m_connected = true;

        Callback< A4VVideoManagerServiceMessages::EventHDMIMsg_t >
        CallbackForKeyEvents( std::bind( &ProductEdidInterface::HandleHpdEvent,
                                     this,
                                     std::placeholders::_1 ) );

        m_EdidClient->RegisterForHotplugEvent( CallbackForKeyEvents );

        return;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief ProductEdidInterface::HandleHpdEvent
///
/// @param keyEvent
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductEdidInterface::HandleHpdEvent( A4VVideoManagerServiceMessages::EventHDMIMsg_t hpdEvent )
{
    BOSE_LOG(DEBUG, __PRETTY_FUNCTION__);
    BOSE_LOG(INFO, "Got HDMI event : " << hpdEvent.event());
    
    if (hpdEvent.event() == A4VVideoManagerServiceMessages::EventsHDMI_t::EHDMI_Connected) {
        {
            BOSE_LOG(INFO, "Sending edid raw Request");
            auto func = std::bind(
                    &ProductEdidInterface::HandleRawEDIDResponse,
                    this,
                    std::placeholders::_1);
            AsyncCallback<A4VVideoManagerServiceMessages::EDIDRawMsg_t> cb(func, m_mainTask);
            m_EdidClient->RequestRawEDID(cb);
        }
        {
            BOSE_LOG(INFO, "Sending Phy addr Request");
            auto func = std::bind(
                    &ProductEdidInterface::HandlePhyAddrResponse,
                    this,
                    std::placeholders::_1);
            AsyncCallback<A4VVideoManagerServiceMessages::CECPhysicalAddrMsg_t> cb(func, m_mainTask);
            m_EdidClient->RequestPhyAddr(cb);
        }

    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief ProductEdidInterface::HandleRawEDIDResponse
///
/// @param keyEvent
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductEdidInterface::HandleRawEDIDResponse(const A4VVideoManagerServiceMessages::EDIDRawMsg_t rawEdid )
{
    //TBD - Mano
    
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief ProductEdidInterface::HandlePhyAddrResponse
///
/// @param keyEvent
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductEdidInterface::HandlePhyAddrResponse(const A4VVideoManagerServiceMessages::CECPhysicalAddrMsg_t cecPhysicalAddress )
{
    BOSE_DEBUG( s_logger, "CEC Physical address 0x%x is being set.", cecPhysicalAddress.addr() );

    if( m_connected == false || m_EdidClient == nullptr )
    {
        BOSE_ERROR( s_logger, "A send CEC PA request could not be made, as no connection is available." );

        return;
    }
    else
    {
        BOSE_DEBUG( s_logger, "A send CEC PA request will be made." );

        //TBD - Mano

        return;
    }


}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                               End of ProductApp Namespace                                    ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
