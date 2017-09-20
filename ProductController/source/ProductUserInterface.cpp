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
#include "SystemUtils.h"
#include "DPrint.h"
#include "CliClient.h"
#include "ProductController.h"
#include "ProductHardwareInterface.h"
#include "ProductUserInterface.h"

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
typedef APProductIF::APProductPtr               ProductPointer;
typedef APClientSocketListenerIF::ListenerPtr   ClientPointer;
typedef APServerSocketListenerIF::ListenerPtr   ServerPointer;
typedef IPCMessageRouterIF::IPCMessageRouterPtr RouterPointer;
typedef CLIClient::CmdPtr                       CommandPointer;
typedef CLIClient::CLICmdDescriptor             CommandDescription;

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// The following declares a DPrint class type object for logging information in this source code
/// file.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
static const DPrint s_logger { "Product" };

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
ProductUserInterface* ProductUserInterface::GetInstance( NotifyTargetTaskIF*         mainTask,
                                                         Callback< ProductMessage >  ProductNotify,
                                                         ProductHardwareInterface*   HardwareInterface )
{
    static ProductUserInterface* instance = new ProductUserInterface( mainTask,
                                                                      ProductNotify,
                                                                      HardwareInterface );

    BOSE_DEBUG( s_logger, "The instance %8p of the Product User Interface has been obtained.", instance );

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
/// @param  mainTask
/// @param  ProductNotify
/// @param  HardwareInterface
///
/// @return This method does not return anything.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
ProductUserInterface::ProductUserInterface( NotifyTargetTaskIF*         mainTask,
                                            Callback< ProductMessage >  ProductNotify,
                                            ProductHardwareInterface*   HardwareInterface )
    : m_mainTask                ( mainTask          ),
      m_ProductNotify           ( ProductNotify     ),
      m_ProductHardwareInterface( HardwareInterface ),
      m_running                  ( false            )

{
    return;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductUserInterface::Run
///
/// @brief  This method starts the ProductUserInterface instance.
///
/// @param  void This method does not take any arguments.
///
/// @return This method does not return anything.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductUserInterface::Run( )
{
    BOSE_DEBUG( s_logger, "The user interface is starting, but has not been implemented." );

    m_running = true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductUserInterface::Run
///
/// @brief  This method starts the ProductUserInterface instance.
///
/// @param  void This method does not take any arguments.
///
/// @return This method does not return anything.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductUserInterface::Stop( )
{
    BOSE_DEBUG( s_logger, "The user interface is starting, but has not been implemented." );

    m_running  = false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                               End of ProductApp Namespace                                    ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
