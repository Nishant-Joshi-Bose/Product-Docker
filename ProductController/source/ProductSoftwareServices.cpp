///////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file      ProductSoftwareServices.cpp
///
/// @brief     This file contains source code for managing software services through the
///            Audio for Video server.
///
/// @author    Stuart J. Lumby
///
/// @date      07/15/2017
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
#include "unistd.h"
#include "DPrint.h"
#include "NotifyTargetTaskIF.h"
#include "APTask.h"
#include "Services.h"
#include "APProductIF.h"
#include "APProductFactory.h"
#include "BoseLinkServerMsgReboot.pb.h"
#include "BoseLinkServerMsgIds.pb.h"
#include "IPCServerListener.h"
#include "APServerSocketListenerFactory.h"
#include "IPCMessageRouterIF.h"
#include "IPCMessageRouterFactory.h"
#include "IPCDirectory.h"
#include "IPCDirectoryIF.h"
#include "NetworkPortDefines.h"
#include "ProductController.h"
#include "ProductHardwareInterface.h"
#include "ProductSoftwareServices.h"
#include "Services.h"

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
typedef APProductIF::APProductPtr               ProductPointer;

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// The following declares a DPrint class type object and a standard string for logging information
/// in this source code file.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
static DPrint s_logger { "Product" };

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductSoftwareServices::GetInstance
///
/// @brief  This static method creates the one and only instance of a ProductSoftwareServices object.
///         The C++ Version 11 compiler guarantees that only one instance is created in a thread
///         safe way.
///IPCSource_t
/// @param  void This method does not take any arguments.
///
/// @return This method returns a pointer to a ProductSoftwareServices object.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
ProductSoftwareServices* ProductSoftwareServices::GetInstance( NotifyTargetTaskIF*        mainTask,
                                                               Callback< ProductMessage > ProductNotify,
                                                               ProductHardwareInterface*  HardwareInterface )
{
    static ProductSoftwareServices* instance = new ProductSoftwareServices( mainTask,
                                                                            ProductNotify,
                                                                            HardwareInterface );

    BOSE_DEBUG( s_logger, "The instance %8p of the Product Software Services was returned.", instance );

    return instance;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductSoftwareServices::ProductSoftwareServices
///
/// @brief  This method is the ProductSoftwareServices constructor, which is declared as being private
///         to ensure that only one instance of this class can be created through the class
///         GetInstance method.
///
/// @param  void This method does not take any arguments.
///
/// @return This method does not return anything.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
ProductSoftwareServices::ProductSoftwareServices( NotifyTargetTaskIF*        mainTask,
                                                  Callback< ProductMessage > ProductNotify,
                                                  ProductHardwareInterface*  HardwareInterface )

    : m_mainTask( mainTask ),
      m_ProductNotify( ProductNotify ),
      m_ProductHardwareInterface( HardwareInterface )
{
    return;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductSoftwareServices::Run
///
/// @brief  This method registers for product events and requests, as well as reboot requests
///         through three registration methods.
///
/// @param  void This method does not take any arguments.
///
/// @return This method does not return anything.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductSoftwareServices::Run( )
{
    BOSE_DEBUG( s_logger, "Creation of a server to handle software services has been made." );

    m_serverListener = IL::CreateServerListener( "ProductSoftwareServicesListener", m_mainTask );


    AsyncCallback< ServerSocket > callback( std::bind( &ProductSoftwareServices::AcceptClient,
                                                       this,
                                                       std::placeholders::_1 ),
                                            m_mainTask );

    m_serverListener->Serve( IPCDirectory::Get( )->DefaultAddress( IPCDirectory::A4V_SERVER ), callback );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductSoftwareServices::AcceptClient
///
/// @brief  This method accepts and establishes client connections for making reboot requests.
///
/// @param  client [input] This argument is a pointer to a client socket class instance that wishes
///                        to connect and register for reboot requests.
///
/// @return This method does not return anything.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductSoftwareServices::AcceptClient( ServerSocket client )
{
    std::string   clientName   = client->GetPeerAddrInfo( ).ToString( );
    RouterPointer messageRouter = IPCMessageRouterFactory::CreateRouter( "ServerRouter" + clientName,
                                                                         m_mainTask );

    BOSE_DEBUG( s_logger, "A client connection %s for reboot requests has been established.",
                clientName.c_str( ) );

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// The callback for handling reboot requests is established and attached to a message router
    /// for processing.
    ///////////////////////////////////////////////////////////////////////////////////////////////
    {
        AsyncCallback< BoseLinkServerMsgReboot > callback( std::bind( &ProductSoftwareServices::SendRebootRequestHandler,
                                                                      this,
                                                                      std::placeholders::_1 ),
                                                           m_mainTask );

        messageRouter->Attach< BoseLinkServerMsgReboot >( BOSELINK_SERVER_MSG_ID_REBOOT, callback );
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// The callback for handling client disconnections is created for the message router, and
    /// message routing is served establishing the client connection.
    ///////////////////////////////////////////////////////////////////////////////////////////////
    {
        AsyncCallback< void > callback( std::bind( &ProductSoftwareServices::HandleClientDisconnect,
                                                   this ),
                                        m_mainTask );

        messageRouter->Serve( std::move( client ), callback );
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductSoftwareServices::HandleClientDisconnect
///
/// @brief  This method is a callback for handling client disconnections for reboot requests.
///
/// @param  void This method does not take any arguments.
///
/// @return This method does not return anything.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductSoftwareServices::HandleClientDisconnect( )
{
    BOSE_DEBUG( s_logger, "A client connection for software services has been disconnected." );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductSoftwareServices::SendRebootRequestHandler
///
/// @brief  This method is used to send a reboot request to the LPM hardware. It is used to called
///         another method SendRebootRequest, which actually sends the request to the hardware. The
///         SendRebootRequest is public though and can also be called through the system interface
///         directly.
///
/// @param  BoseLinkServerMsgReboot [input] This argument contains the reboot message request data.
///
/// @return This method does not return anything.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductSoftwareServices::SendRebootRequestHandler( BoseLinkServerMsgReboot rebootRequest )
{
    BOSE_DEBUG( s_logger, "A reboot after a %d delay has been requested.", rebootRequest.delay( ) );

    unsigned int delay = rebootRequest.delay( );

    SendRebootRequest( delay );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductSoftwareServices::SendRebootRequest
///
/// @brief  This method is used to send a reboot request to the LPM hardware.
///
/// @param  delay [input] This argument contains the delay in seconds to wait before a reboot.
///
/// @return This method does not return anything.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductSoftwareServices::SendRebootRequest( unsigned int delay )
{
    BOSE_DEBUG( s_logger, "A reboot after a %d delay is being processed.", delay );

    sleep( delay );

    m_ProductHardwareInterface->SendRebootRequest( );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief ProductSoftwareServices::Stop
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductSoftwareServices::Stop( void )
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
