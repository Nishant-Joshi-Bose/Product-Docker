////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file      ProductKeyInputInterface.h
///
/// @brief     This header file declares a ProductKeyInputInterface class that is used to receive
///            raw key input from the LPM hardware and convert it into key actions.
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
#include "KeyHandler.h"
#include "LpmServiceMessages.pb.h"
#include "ProductMessage.pb.h"

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
class ProfessorProductController;

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @class ProductKeyInputInterface Constructor
///
/// @brief This class acts to extract raw keys from the LPM hardware, pass them to a key handler,
///        and send the key action to the product controller state machine for processing.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
class ProductKeyInputInterface
{
public:

    //////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief ProductKeyInputInterface Constructor
    ///
    /// @param ProfessorProductController& ProductController
    ///
    //////////////////////////////////////////////////////////////////////////////////////////////
    ProductKeyInputInterface( ProfessorProductController& ProductController );

    //////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief  The following public methods are used to run and stop instances of the
    ///         ProductKeyInputInterface class, respectively.
    ///
    //////////////////////////////////////////////////////////////////////////////////////////////
    void Run( );
    void Stop( );

private:

    //////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief The following declarations provide a task in which to process key information, a
    ///        method to notify the product controller for receipt of a key action, an interface
    ///        to the lower level hardware via the LPM for raw keys, and a main key handler class
    ///        instance. Boolean variables indicating whether there is a connection to receive raw
    ///        key presses and whether the module is running are also provided.
    ///
    //////////////////////////////////////////////////////////////////////////////////////////////
    NotifyTargetTaskIF*                               m_ProductTask;
    Callback< ProductMessage >                        m_ProductNotify;
    std::shared_ptr< CustomProductHardwareInterface > m_ProductHardwareInterface;
    KeyHandlerUtil::KeyHandler                        m_KeyHandler;
    bool                                              m_connected;
    bool                                              m_running;

    //////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief The following methods are used to register for and receive key events from the LPM
    ///        hardware interface and the key handler.
    ///
    //////////////////////////////////////////////////////////////////////////////////////////////
    void ConnectToLpm( bool connected );
    void RegisterForKeyEvents( );
    void HandleKeyEvent( LpmServiceMessages::IpcKeyInformation_t keyEvent );

    //////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief The following method receives actions as a result of the key handler processing
    ///         raw keys events.
    ///
    //////////////////////////////////////////////////////////////////////////////////////////////
    void KeyInformationCallBack( const int keyAction );
};

////////////////////////////////////////////////////////////////////////////////////////////////////
///                           End of the Product Application Namespace                           ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
