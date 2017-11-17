////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file      ProductCommandLine.h
///
/// @brief     This header file declares a ProductCommandLine class that is used to set up a command
///            line interface.
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
#include "string"
#include "CliClient.h"
#include "ThreadMutex.h"
#include "NotifyTargetTaskIF.h"
#include "APTask.h"
#include "APClientSocketListenerIF.h"
#include "APServerSocketListenerIF.h"
#include "IPCMessageRouterIF.h"
#include "AutoLpmServiceMessages.pb.h"
#include "ProductMessage.pb.h"

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
typedef APProductIF::APProductPtr               ProductPointer;
typedef APClientSocketListenerIF::ListenerPtr  ClientPointer;
typedef APServerSocketListenerIF::ListenerPtr  ServerPointer;
typedef IPCMessageRouterIF::IPCMessageRouterPtr RouterPointer;
typedef CLIClient::CmdPtr                       CommandPointer;
typedef CLIClient::CLICmdDescriptor             CommandDescription ;

////////////////////////////////////////////////////////////////////////////////////////////////////
///
///            Forward Class Declarations
///
////////////////////////////////////////////////////////////////////////////////////////////////////
class ProductHardwareInterface;
class ProductController;

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @class ProductCommandLine
///
/// @brief This class is used to set up a command line interface through the product controller
///        class. Note that only one instantiation of this class is to be created through its
///        GetInstance static method, which returns a single static reference to an instance of this
///        class.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
class ProductCommandLine
{
public:

    //////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @name   ProductCommandLine::GetInstance
    ///
    /// @brief  This static method creates the one and only instance of a ProductCommandLine
    ///         object. That only one instance is created in a thread safe way is guaranteed by
    ///         the C++ Version 11 compiler.
    ///
    /// @param  NotifyTargetTaskIF* ProductTask
    ///
    /// @param  Callback< ProductMessage > ProductNotify
    ///
    /// @param  ProductHardwareInterface* HardwareInterface
    ///
    /// @return This method returns a reference to a ProductCommandLine object.
    //////////////////////////////////////////////////////////////////////////////////////////////
    static ProductCommandLine* GetInstance( NotifyTargetTaskIF*        ProductTask,
                                            Callback< ProductMessage > ProductNotify,
                                            ProductHardwareInterface*  HardwareInterface );

    //////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief  The following public methods are used to run and stop the ProductCommandLine
    ///         instance, respectively.
    ///
    //////////////////////////////////////////////////////////////////////////////////////////////
    void Run( );
    void Stop( );

private:

    //////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @name   ProductCommandLine
    ///
    /// @brief  The constructor for this class is set to be private. This definition prevents this
    ///         class from being instantiated directly, so that only the static method GetInstance
    ///         to this class can be used to get the one sole instance of it.
    ///
    /// @param  NotifyTargetTaskIF* ProductTask
    ///
    /// @param  Callback< ProductMessage > ProductNotify
    ///
    /// @param  ProductHardwareInterface* HardwareInterface
    ///
    //////////////////////////////////////////////////////////////////////////////////////////////
    ProductCommandLine( NotifyTargetTaskIF*        ProductTask,
                        Callback< ProductMessage > ProductNotify,
                        ProductHardwareInterface*  HardwareInterface );

    //////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief The following copy constructor and equality operator for this class are private
    ///        and are set to be undefined through the delete keyword. This prevents this class
    ///        from being copied directly, so that only the static method GetInstance to this
    ///        class can be used to get the one sole instance of it.
    ///
    //////////////////////////////////////////////////////////////////////////////////////////////
    ProductCommandLine( ProductCommandLine const& ) = delete;
    ProductCommandLine operator = ( ProductCommandLine const& ) = delete;

    //////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief The following member stores a pointer to a command line interface class object.
    ///
    //////////////////////////////////////////////////////////////////////////////////////////////
    CLIClient * m_CommandLineInterface;

    //////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief The following member stores a pointer to the task for the command line.
    ///
    //////////////////////////////////////////////////////////////////////////////////////////////
    NotifyTargetTaskIF* m_ProductTask;

    std::vector< CommandPointer > GetCommandsList( );

    int  HandleCommand( const std::string&              command,
                        const std::list< std::string >& arguments,
                        std::string&                    response ) const;

    //////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief The following subclass instances are used to interface with the product controller
    ///        and the lower level hardware.
    ///
    //////////////////////////////////////////////////////////////////////////////////////////////
    Callback< ProductMessage >  m_ProductNotify;
    ProductHardwareInterface*   m_ProductHardwareInterface = nullptr;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
///                           End of the Product Application Namespace                           ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
