////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file      ProductController.cpp
///
/// @brief     This file contains source code that implements the ProductController class that acts
///            as a container to handle all the main functionality related to this program that is
///            not product specific. In these regards, this class is used as a container to control
///            the product states, as well as to instantiate subclasses to manage the device and
///            lower level hardware, and interface with the user and system level applications.
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
#include "ProductCommandLine.h"     /// This file declares the ProductCommandLine class.

////////////////////////////////////////////////////////////////////////////////////////////////////
///
///            Definitions
///
////////////////////////////////////////////////////////////////////////////////////////////////////
#define PRODUCT_CONTROLLER_RUNNING_CHECK_IN_SECONDS (5)

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
static const char   s_logName[] = "Product Controller";

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductController::GetInstance
///
/// @brief  This static method creates the one and only instance of a ProductController object.
///         The C++ Version 11 compiler guarantees that only one instance is created in a thread
///         safe way.
///
/// @param  void This method does not take any arguments.
///
/// @return This method returns a pointer to a ProductController object.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
ProductController* ProductController::GetInstance( )
{
       static ProductController* instance = new ProductController( );

       s_logger.LogInfo( "%-18s : The instance %8p of the Product Controller was returned. ",
                         s_logName,
                         instance );

       return instance;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductController::ProductController
///
/// @brief  This method is the ProductController constructor, which is declared as being private to
///         ensure that only one instance of this class can be created through the class GetInstance
///         method.
///
/// @param  void This method does not take any arguments.
///
/// @return This method does not return anything.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
ProductController::ProductController( )
                 : APTask( "ProductControllerMainTask" )
{
       return;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductController::GetHardwareManagerInstance
///
/// @param  void This method does not take any arguments.
///
/// @return This method returns a pointer to a ProductHardwareManager class instance.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
ProductHardwareManager* ProductController::GetHardwareManagerInstance( )
{
       if( m_ProductHardwareManager != nullptr )
       {
           return m_ProductHardwareManager;
       }
       else
       {
           m_ProductHardwareManager = ProductHardwareManager::GetInstance( );

           return m_ProductHardwareManager;
       }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductController::ProductDeviceManager
///
/// @param  void This method does not take any arguments.
///
/// @return This method returns a pointer to a ProductDeviceManager class instance.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
ProductDeviceManager* ProductController::GetDeviceManagerInstance( )
{
       if( m_ProductDeviceManager != nullptr )
       {
           return m_ProductDeviceManager;
       }
       else
       {
           m_ProductDeviceManager = ProductDeviceManager::GetInstance( );

           return m_ProductDeviceManager;
       }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductController::ProductUserInterface
///
/// @param  void This method does not take any arguments.
///
/// @return This method returns a pointer to a ProductUserInterface class instance.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
ProductUserInterface* ProductController::GetUserInterfaceInstance( )
{
       if( m_ProductUserInterface != nullptr )
       {
           return m_ProductUserInterface;
       }
       else
       {
           m_ProductUserInterface = ProductUserInterface::GetInstance( );

           return m_ProductUserInterface;
       }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductController::ProductSystemInterface
///
/// @param  void This method does not take any arguments.
///
/// @return This method returns a pointer to a ProductSystemInterface class instance.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
ProductSystemInterface* ProductController::GetSystemInterfaceInstance( )
{
       if( m_ProductSystemInterface != nullptr )
       {
           return m_ProductSystemInterface;
       }
       else
       {
           m_ProductSystemInterface = ProductSystemInterface::GetInstance( );

           return m_ProductSystemInterface;
       }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductController::ProductCommandLine
///
/// @param  void This method does not take any arguments.
///
/// @return This method returns a pointer to a ProductCommandLine class instance.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
ProductCommandLine* ProductController::GetCommandLineInstance( )
{
       if( m_ProductCommandLine != nullptr )
       {
           return m_ProductCommandLine;
       }
       else
       {
           m_ProductCommandLine = ProductCommandLine::GetInstance( );

           return m_ProductCommandLine;
       }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductController::Run
///
/// @brief  This method starts the main task for the ProductController instance. The OnEntry method
///         for the ProductController instance is called just before the main task starts. Also,
///         this main task is used for most of the internal processing for each of the subclass
///         instances.
///
/// @param  void This method does not take any arguments.
///
/// @return This method does not return anything.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductController::Run( )
{
     m_running = true;

     s_logger.LogInfo( "%-18s : The Product Controller main task is starting. ", s_logName );

     Start( );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductController::OnEntry
///
/// @brief  This method is called when the ProductController task starts. It is used to start all
///         the instances of the subclasses contained in the ProductController.
///
/// @param  void This method does not take any arguments.
///
/// @return This method does not return anything.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductController::OnEntry( )
{
     s_logger.LogInfo( "%-18s : The Product Controller is starting up its processes. ", s_logName );

     m_ProductHardwareManager = ProductHardwareManager::GetInstance( );
     m_ProductDeviceManager   = ProductDeviceManager  ::GetInstance( );
     m_ProductUserInterface   = ProductUserInterface  ::GetInstance( );
     m_ProductSystemInterface = ProductSystemInterface::GetInstance( );
     m_ProductCommandLine     = ProductCommandLine    ::GetInstance( );

     if( m_ProductHardwareManager != nullptr &&
         m_ProductUserInterface   != nullptr &&
         m_ProductSystemInterface != nullptr &&
         m_ProductCommandLine     != nullptr &&
         m_ProductHardwareManager != nullptr    )
     {
         m_ProductHardwareManager->Run( );
         m_ProductDeviceManager  ->Run( );
         m_ProductUserInterface  ->Run( );
         m_ProductSystemInterface->Run( );
         m_ProductCommandLine    ->Run( );

         s_logger.LogInfo( "%-18s : All the processes have now been started and are running. ", s_logName );
     }
     else
     {
         s_logger.LogError( "%-18s : A process could not be created. ", s_logName );
     }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductController::Wait
///
/// @brief  This method is called from a calling task to wait until the Product Controller process
///         ends.
///
/// @param  void This method does not take any arguments.
///
/// @return This method does not return anything.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductController::Wait( )
{
     while( m_running )
     {
           sleep( PRODUCT_CONTROLLER_RUNNING_CHECK_IN_SECONDS );
     }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductController::End
///
/// @brief  This method is called when the Product Controller process ends. It is used to stop the
///         main task.
///
/// @param  void This method does not take any arguments.
///
/// @return This method does not return anything.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductController::End( )
{
     s_logger.LogInfo( "%-18s : The Product Controller main task is stopping. ", s_logName );

     m_running = false;

     Stop( );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductController::HandleMessage
///
/// @brief  This method is called to handle generic type messages, which are sent from the more
///         product specific class instances, and is used to process the state machine for the
///         product.
///
/// @param  void This method does not take any arguments.
///
/// @return This method does not return anything.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductController::HandleMessage( ProductControllerMessage message )
{
     return;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
