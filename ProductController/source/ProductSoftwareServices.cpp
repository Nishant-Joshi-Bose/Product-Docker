///////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file      ProductSoftwareServices.cpp
///
/// @brief     This file contains source code for managing software services through the
///            Audio for Video server.
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
#include "unistd.h"
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
///                          Start of the Product Application Namespace                          ///
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
/// @name   ProductSoftwareServices::GetInstance
///
/// @brief  This static method creates the one and only instance of a ProductSoftwareServices object.
///         The C++ Version 11 compiler guarantees that only one instance is created in a thread
///         safe way.
///
/// @param NotifyTargetTaskIF* ProductTask This argument points to a task to process
///                                        resource requests and notifications.
///
/// @param Callback< ProductMessage > ProductNotify This is a callback to send events to
///                                                 the Product Controller.
///
/// @param ProductHardwareInterface* HardwareInterface This argument points to the hardware
///                                                    interface.
///
/// @return This method returns a pointer to a ProductSoftwareServices object.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
ProductSoftwareServices* ProductSoftwareServices::GetInstance( NotifyTargetTaskIF*        ProductTask,
                                                               Callback< ProductMessage > ProductNotify,
                                                               ProductHardwareInterface*  HardwareInterface )
{
    static ProductSoftwareServices* instance = new ProductSoftwareServices( ProductTask,
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
/// @param NotifyTargetTaskIF* ProductTask This argument points to a task to process
///                                        resource requests and notifications.
///
/// @param Callback< ProductMessage > ProductNotify This is a callback to send events to
///                                                 the Product Controller.
///
/// @param ProductHardwareInterface* HardwareInterface This argument points to the hardware
///                                                    interface.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
ProductSoftwareServices::ProductSoftwareServices( NotifyTargetTaskIF*        ProductTask,
                                                  Callback< ProductMessage > ProductNotify,
                                                  ProductHardwareInterface*  HardwareInterface )

    : m_ProductTask( ProductTask ),
      m_ProductNotify( ProductNotify ),
      m_ProductHardwareInterface( HardwareInterface )
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductSoftwareServices::Run
///
/// @brief  This method establishes a server for software services.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductSoftwareServices::Run( )
{
    BOSE_DEBUG( s_logger, "Creation of a server to handle software services has been made." );

    m_serverListener = IL::CreateServerListener( "ProductSoftwareServicesListener", m_ProductTask );


    AsyncCallback< ServerSocket > callback( std::bind( &ProductSoftwareServices::AcceptClient,
                                                       this,
                                                       std::placeholders::_1 ),
                                            m_ProductTask );

    m_serverListener->Serve( IPCDirectory::Get( )->DefaultAddress( IPCDirectory::A4V_SERVER ), callback );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductSoftwareServices::AcceptClient
///
/// @brief  This method accepts and establishes client connections for making reboot requests.
///
/// @param  ServerSocket client This argument is a pointer to a client socket class instance that
///                             wishes to connect and register for reboot requests.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductSoftwareServices::AcceptClient( ServerSocket client )
{
    std::string   clientName   = client->GetPeerAddrInfo( ).ToString( );
    RouterPointer messageRouter = IPCMessageRouterFactory::CreateRouter( "ServerRouter" + clientName,
                                                                         m_ProductTask );

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
                                                           m_ProductTask );

        messageRouter->Attach< BoseLinkServerMsgReboot >( BOSELINK_SERVER_MSG_ID_REBOOT, callback );
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// The callback for handling client disconnections is created for the message router, and
    /// message routing is served establishing the client connection.
    ///////////////////////////////////////////////////////////////////////////////////////////////
    {
        AsyncCallback< void > callback( std::bind( &ProductSoftwareServices::HandleClientDisconnect,
                                                   this ),
                                        m_ProductTask );

        messageRouter->Serve( std::move( client ), callback );
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductSoftwareServices::HandleClientDisconnect
///
/// @brief  This method is a callback for handling client disconnections for reboot requests.
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
/// @param  BoseLinkServerMsgReboot rebootRequest This argument contains reboot message data.
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
/// @param  int delay
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
/// @name ProductSoftwareServices::Stop
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductSoftwareServices::Stop( )
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
