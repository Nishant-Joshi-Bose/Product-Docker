////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file      ProductUserInterface.h
///
/// @brief     This header file declares a ProductUserInterface class that is used to receive user
///            input.
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
#include "CliClient.h"
#include "APTask.h"
#include "BreakThread.h"
#include "APTaskFactory.h"
#include "APClientSocketListenerIF.h"
#include "APServerSocketListenerIF.h"
#include "IPCMessageRouterIF.h"
#include "APProductIF.h"
#include "ProductMessage.pb.h"
#include "LpmClientIF.h"
#include "KeyHandler.h"

namespace ProductApp
{
////////////////////////////////////////////////////////////////////////////////////////////////////
///
///            Forward Class Declarations
///
////////////////////////////////////////////////////////////////////////////////////////////////////
class ProductHardwareInterface;
class ProductController;

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @class ProductUserInterface
///
/// @brief This class acts as a container to handle all the main functionality related to this
///        program that is not product specific, including controlling the product states, as well
///        as to instantiating subclasses to manage the device and lower level hardware, and to
///        interface with the user and higher level applications. Note that only one instantiation
///        of this class is to be created through its GetInstance static method, which returns a
///        single static reference to an instance of this class.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
class ProductUserInterface
{
public:

    //////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @name   ProductUserInterface::GetInstance
    ///
    /// @brief  This static method creates the one and only instance of a ProductUserInterface
    ///         object. That only one instance is created in a thread safe way is guaranteed by
    ///         the C++ Version 11 compiler.
    ///
    /// @param  void This method does not take any arguments.
    ///
    /// @return This method returns a reference to a ProductUserInterface object.
    ///
    //////////////////////////////////////////////////////////////////////////////////////////////
    static ProductUserInterface* GetInstance( NotifyTargetTaskIF*         mainTask,
                                              Callback< ProductMessage >  ProductNotify,
                                              ProductHardwareInterface*   HardwareInterface,
                                              CliClientMT                 &cliClienMT );

    //////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief  The following public methods are used to start and stop the ProductUserInterface
    ///         instance.
    ///
    //////////////////////////////////////////////////////////////////////////////////////////////
    void Run( void );
    void Stop( void );

private:

    //////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @name   ProductUserInterface
    ///
    /// @brief  The constructor for this class is set to be private. This definition prevents this
    ///         class from being instantiated directly, so that only the static method GetInstance
    ///         to this class can be used to get the one sole instance of it.
    ///
    /// @param  void This method does not take any arguments.
    ///
    /// @return This method does not return anything.
    ///
    //////////////////////////////////////////////////////////////////////////////////////////////
    ProductUserInterface( NotifyTargetTaskIF*         mainTask,
                          Callback< ProductMessage >  ProductNotify,
                          ProductHardwareInterface*   HardwareInterface,
                          CliClientMT                 &cliClientMT );

    //////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief The following copy constructor and equality operator for this class are private
    ///        and are set to be undefined through the delete keyword. This prevents this class
    ///        from being copied directly, so that only the static method GetInstance to this
    ///        class can be used to get the one sole instance of it.
    ///
    //////////////////////////////////////////////////////////////////////////////////////////////
    ProductUserInterface( ProductUserInterface const& ) = delete;
    void operator = ( ProductUserInterface const& ) = delete;

    //////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief The following method is used to register for and receive key events from the LPM
    ///        hardware interface.
    ///
    //////////////////////////////////////////////////////////////////////////////////////////////
    void RegisterForKeyEvents( void );
    void HandleKeyEvent( LpmServiceMessages::IpcKeyInformation_t keyEvent );
    void HandleLpmKeyInformation( LpmServiceMessages::IpcKeyInformation_t keyInformation );

    //////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief The following method receives intents as a result of the key handler processing
    /// "raw" keys events.
    ///
    //////////////////////////////////////////////////////////////////////////////////////////////
    void KeyInformationCallBack( const int result );

    //////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief The following subclass instances are used to manage the lower level hardware and
    ///        the device, as well as to interface with the user and higher level system
    ///        applications, respectively.
    ///
    //////////////////////////////////////////////////////////////////////////////////////////////
    NotifyTargetTaskIF*         m_mainTask;
    NotifyTargetTaskIF*         m_keyEventTask;
    Callback< ProductMessage >  m_ProductNotify;
    ProductHardwareInterface*   m_ProductHardwareInterface = nullptr;
    bool                        m_running;
    KeyHandlerUtil::KeyHandler  m_KeyHandler;
    static constexpr const char *m_keyConfigFileName = "/opt/Bose/etc/KeyConfiguration.json";
};
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
