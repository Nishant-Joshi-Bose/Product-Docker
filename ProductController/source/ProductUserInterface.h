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
#include "KeyActions.h"
#include "ProductMessage.pb.h"
#include "LpmClientIF.h"
#include "KeyHandler.h"
#include "KeyActions.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
///                          Start of the Product Application Namespace                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
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
/// @brief This class acts to extract raw keys from the LPM hardware, pass them to a key handler,
///        and send the key action to the product controller state machine for processing.
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
    static ProductUserInterface* GetInstance( NotifyTargetTaskIF*         ProductTask,
                                              Callback< ProductMessage >  ProductNotify,
                                              ProductHardwareInterface*   HardwareInterface,
                                              CliClientMT&                CommandLineInterface );

    //////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief  The following public methods are used to start and stop the ProductUserInterface
    ///         instance.
    ///
    //////////////////////////////////////////////////////////////////////////////////////////////
    void Run( void );
    void Stop( void );

    //////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief The following method converts a key action and return its associated value as a
    ///        string.
    ///
    //////////////////////////////////////////////////////////////////////////////////////////////
    std::string GetKeyString( const KEY_ACTION keyAction );

private:

    //////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @name  ProductUserInterface
    ///
    /// @brief The constructor for this class is set to be private. This definition prevents this
    ///        class from being instantiated directly, so that only the static method GetInstance
    ///        to this class can be used to get the one sole instance of it.
    ///
    /// @param NotifyTargetTaskIF* ProductTask
    ///
    /// @param Callback< ProductMessage > ProductNotify
    ///
    /// @param ProductHardwareInterface*  HardwareInterface
    ///
    //////////////////////////////////////////////////////////////////////////////////////////////
    ProductUserInterface( NotifyTargetTaskIF*        ProductTask,
                          Callback< ProductMessage > ProductNotify,
                          ProductHardwareInterface*  HardwareInterface,
                          CliClientMT&               CommandLineInterface );

    //////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief The following copy constructor and equality operator for this class are private
    ///        and are set to be undefined through the delete keyword. This prevents this class
    ///        from being copied directly, so that only the static method GetInstance to this
    ///        class can be used to get the one sole instance of it.
    ///
    //////////////////////////////////////////////////////////////////////////////////////////////
    ProductUserInterface( ProductUserInterface const& ) = delete;
    ProductUserInterface operator = ( ProductUserInterface const& ) = delete;

    //////////////////////////////////////////////////////////////////////////////////////////////
    ///uint32_t
    /// @brief The following methods are used to register for and receive key events from the LPM
    ///        hardware interface and the key handler.
    ///
    //////////////////////////////////////////////////////////////////////////////////////////////
    void RegisterForKeyEvents( void );
    void HandleKeyEvent( LpmServiceMessages::IpcKeyInformation_t keyEvent );

    //////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief The following method receives actions as a result of the key handler processing
    ///         raw keys events.
    ///
    //////////////////////////////////////////////////////////////////////////////////////////////
    void KeyInformationCallBack( const int keyAction );

    //////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief The following subclass instances are used to manage the lower level hardware and
    ///        the device, as well as to interface with the user and higher level system
    ///        applications, respectively.
    ///
    //////////////////////////////////////////////////////////////////////////////////////////////
    NotifyTargetTaskIF*          m_ProductTask;
    NotifyTargetTaskIF*          m_keyEventTask;
    Callback< ProductMessage >   m_ProductNotify;
    ProductHardwareInterface*    m_ProductHardwareInterface;
    bool                         m_running;

    //////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief The following declaration are used for handling key presses.
    ///
    //////////////////////////////////////////////////////////////////////////////////////////////
    KeyHandlerUtil::KeyHandler   m_KeyHandler;
    static constexpr const char* m_keyConfigFileName = "/opt/Bose/etc/KeyConfiguration.json";
};

////////////////////////////////////////////////////////////////////////////////////////////////////
///                           End of the Product Application Namespace                           ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
