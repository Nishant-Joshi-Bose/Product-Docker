////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file      ProductUserInterface.h
///
/// @brief     This header file declares a ProductUserInterface class that is used to receive user
///             input.
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
#include "SystemUtils.h"            /// This file contains Bose system utility declarations.
#include "DPrint.h"                 /// This file contains the DPrint class used for logging.
#include "CliClient.h"              /// This file declares functionality for a command line interface.
#include "ProductController.h"      /// This file declares the ProductController class.
#include "ProductHardwareManager.h" /// This file declares the ProductHardwareManager class.
#include "ProductDeviceManager.h"   /// This file declares the ProductDeviceManager class.
#include "ProductUserInterface.h"   /// This file declares the ProductUserInterface class.
#include "ProductSystemInterface.h" /// This file declares the ProductSystemInterface class.

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief The following aliases refer to the Bose Sound Touch class utilities for inter-process and
///        inter-thread communications.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
typedef APProductIF::APProductPtr               ProductPointer;
typedef APClientSocketListenerIF::ListenerPtr   ClientPointer;
typedef APServerSocketListenerIF::ListenerPtr   ServerPointer;
typedef IPCMessageRouterIF::IPCMessageRouterPtr RouterPointer;
typedef CLIClient::CmdPtr                       CommandPointer;
typedef CLIClient::CLICmdDescriptor             CommandDescription ;

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// The following declares a DPrint class type object and a standard string for logging information
/// in this source code file.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
static const DPrint s_logger    { "Product" };
static const char   s_logName[] = "Product User";

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductUserInterface::GetInstance
///
/// @brief  This static method creates the one and only instance of a ProductUserInterface object.
///         The C++ Version 11 compiler guarantees that only one instance is created in a thread
///         safe way.
///
/// @param  void This method does not take any arguments.
///
/// @return This method returns a pointer to a ProductUserInterface object.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
ProductUserInterface* ProductUserInterface::GetInstance( )
{
       static ProductUserInterface* instance = new ProductUserInterface( );

       s_logger.LogInfo( "%-18s : The instance %8p of the Product User Interface has been obtained. ",
                         s_logName,
                         instance );

       return instance;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductUserInterface::ProductUserInterface
///
/// @brief  This method is the ProductUserInterface constructor, which is declared as being
///         private to ensure that only one instance of this class can be created through the class
///         GetInstance method.
///
/// @param  void This method does not take any arguments.
///
/// @return This method does not return anything.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
ProductUserInterface::ProductUserInterface( )
{
       return;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductUserInterface::~ProductUserInterface
///
/// @brief  This method is the ProductUserInterface destructor, which stops the Sound Touch system
///         process.
///
/// @param  void This method does not take any arguments.
///
/// @return This method does not return anything.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
ProductUserInterface::~ProductUserInterface( )
{
       s_logger.LogInfo( "%-18s : The Product User Interface instance is being destroyed. ", s_logName );

       m_running = false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductUserInterface::Run
///
/// @brief  This method starts the main task for the ProductUserInterface instance. The OnEntry method
///         for the ProductUserInterface instance is called just before the main task starts. Also,
///         this main task is used for most of the internal processing for each of the subclass
///         instances.
///
/// @param  void This method does not take any arguments.
///
/// @return This method does not return anything.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductUserInterface::Run( )
{
     m_ProductController      =   ProductController::GetInstance( );
     m_ProductHardwareManager = m_ProductController->GetHardwareManagerInstance( );
     m_ProductSystemInterface = m_ProductController->GetSystemInterfaceInstance( );
     m_ProductDeviceManager   = m_ProductController->GetDeviceManagerInstance ( );

     s_logger.LogInfo( "%-18s : The user interface is starting. ", s_logName );

     m_running  = true;
     m_mainTask = m_ProductController->GetMainTask( );

     s_logger.LogInfo( "%-18s : The system interface is now running. ", s_logName );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
