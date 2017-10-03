////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file      ProductUserInterface.h
///
/// @brief     This header file declares a ProductUserInterface class that is used to receive user
///             input.
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
///            Included Header Files
///
////////////////////////////////////////////////////////////////////////////////////////////////////
#include <chrono>
#include <thread>
#include <unistd.h>
#include "SystemUtils.h"
#include "DPrint.h"
#include "APTask.h"
#include "BreakThread.h"
#include "ProductController.h"
#include "ProductHardwareInterface.h"
#include "ProductUserInterface.h"
#include "ProductMessage.pb.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
///                             Start of Product Namespace                                       ///
////////////////////////////////////////////////////////////////////////////////////////////////////
namespace ProductApp
{

////////////////////////////////////////////////////////////////////////////////////////////////////
///
///            Definitions
///
////////////////////////////////////////////////////////////////////////////////////////////////////
#define PRODUCT_USER_INTERFACE_RETRY_IN_SECONDS ( 1 )

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
static DPrint s_logger { "Product" };

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
                                                         ProductHardwareInterface*   HardwareInterface,
                                                         CliClientMT                 &cliClientMT )
{
    static ProductUserInterface* instance = new ProductUserInterface( mainTask,
                                                                      ProductNotify,
                                                                      HardwareInterface,
                                                                      cliClientMT );

    BOSE_INFO( s_logger, "The instance %8p of the Product User Interface has been obtained.", instance );

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
                                            ProductHardwareInterface*   HardwareInterface,
                                            CliClientMT                 &cliClientMT )
    : m_mainTask( mainTask ),
      m_keyEventTask( IL::CreateTask( "ProductMonitorNetworkTask" ) ),
      m_ProductNotify( ProductNotify ),
      m_ProductHardwareInterface( HardwareInterface ),
      m_running( false ),
      m_KeyHandler( *mainTask, cliClientMT )

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
    BOSE_INFO( s_logger, "%s: The user interface is starting.", __FUNCTION__ );

    ///
    /// Attempt to register for key events in a separate task.
    ///
    IL::BreakThread( std::bind( &ProductUserInterface::RegisterForKeyEvents,
                                this ),
                     m_keyEventTask );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief ProductUserInterface::RegisterForKeyEvents
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductUserInterface::RegisterForKeyEvents( void )
{
    BOSE_INFO( s_logger, "%s: The user interface is attempting to register for key events.", __FUNCTION__ );

    ///
    /// Repeated attempts are made to sucessfully pass a callback to the hardware interface to
    /// receive key events from the LPM. This callback when invoked will send a message to the
    /// ProductController class that handles the state machine.
    ///
    Callback< LpmServiceMessages::IpcKeyInformation_t >
    CallbackForKeyEvents( std::bind( &ProductUserInterface::HandleKeyEvent,
                                     this,
                                     std::placeholders::_1 ) );

    while( !m_ProductHardwareInterface->RegisterForKeyEvents( CallbackForKeyEvents ) )
    {
        sleep( PRODUCT_USER_INTERFACE_RETRY_IN_SECONDS );
    }

    // Register with the key handler / repeat management code (this is how raw lpm keys get translated to "intents")
    m_KeyHandler.RegisterKeyHandler( ProductUserInterface::KeyInformationCallBack, this );

    BOSE_INFO( s_logger, "%s: The user interface has registered for key events.", __FUNCTION__ );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief ProductUserInterface::KeyInformationCallBack
///
/// @param result
/// @param context
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductUserInterface:: KeyInformationCallBack( const int result, void *context )
{
    BOSE_INFO( s_logger, "Keys have been translated to intend = %d", result );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief ProductUserInterface::HandleKeyEvent
///
/// @param keyEvent
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductUserInterface::HandleKeyEvent( LpmServiceMessages::IpcKeyInformation_t keyEvent )
{
    ProductMessage productMessage;
    std::string    keyOriginString;
    std::string    keyStateString;
    std::string    keyIdString;

    auto keyData = productMessage.mutable_data( )->mutable_keydata( );

    productMessage.set_id( KEY_PRESS );

    switch( keyEvent.keyorigin( ) )
    {
    case KEY_ORIGIN_CONSOLE_BUTTON:
        keyOriginString.assign( "CONSOLE_BUTTON" );
        break;
    case KEY_ORIGIN_CAPSENSE:
        keyOriginString.assign( "CAPSENSE" );
        break;
    case KEY_ORIGIN_IR:
        keyOriginString.assign( "IR" );
        break;
    case KEY_ORIGIN_RF:
        keyOriginString.assign( "RF" );
        break;
    case KEY_ORIGIN_CEC:
        keyOriginString.assign( "CEC" );
        break;
    case KEY_ORIGIN_NETWORK:
        keyOriginString.assign( "NETWORK" );
        break;
    case KEY_ORIGIN_TAP:
        keyOriginString.assign( "TAP" );
        break;
    default:
        keyOriginString = "UNKNOWN " + std::to_string( keyEvent.keyorigin( ) );
        break;
    }

    switch( keyEvent.keystate( ) )
    {
    case KEY_RELEASED:
        keyStateString.assign( "RELEASED" );

        keyData->set_state( UP );
        keyData->set_value( keyEvent.keyid( ) );

        break;
    case KEY_PRESSED:
        keyStateString.assign( "PRESSED" );

        keyData->set_state( DOWN );
        keyData->set_value( keyEvent.keyid( ) );

        break;
    default:
        keyStateString = "UNKNOWN " + std::to_string( keyEvent.keystate( ) );

        keyData->set_state( UNKNOWN_KEY_STATE );
        keyData->set_value( keyEvent.keyid( ) );

        break;
    }

    keyIdString.assign( std::to_string( keyEvent.keyid( ) ) );

    BOSE_INFO( s_logger, "----------- Product Controller Key Event ------------" );
    BOSE_INFO( s_logger, "A key event from the hardware interface was received: " );
    BOSE_INFO( s_logger, " " );
    BOSE_INFO( s_logger, "  Key Origin : %s ", keyOriginString.c_str( ) );
    BOSE_INFO( s_logger, "  Key State  : %s ", keyStateString.c_str( ) );
    BOSE_INFO( s_logger, "  Key ID     : %s ", keyIdString.c_str( ) );
    BOSE_INFO( s_logger, " " );

    // Message is incomplete, dump it
    if( !keyEvent.has_keyorigin() || !keyEvent.has_keystate() || !keyEvent.has_keyid() )
    {
        BOSE_INFO( s_logger, "Can't process keys with missing information: keyorigin:%d, keystate:%d, keyid:%d",
                   keyEvent.keyorigin(), keyEvent.keystate(), keyEvent.keyid() );
        return;
    }

    KeyHandlerUtil::KeyRepeatManager *ptrRepeatMgr = m_KeyHandler.RepeatMgr( keyEvent.keyorigin() );

    if( !ptrRepeatMgr )
    {
        s_logger.LogError( "Source %d not registered", keyEvent.has_keyorigin() );
        return;
    }

    // Feed it into the keyHandler

#if 0
    // TODO - reenable
    m_CliClientMT.SendAsyncResponse( "Received from LPM, KeySource: CONSOLE, State " + \
                                     std::to_string( keyEvent.keystate() ) + " KeyId " + \
                                     std::to_string( keyEvent.keyid() ) );
#endif
    ptrRepeatMgr->HandleKeys( keyEvent.keyorigin(),
                              keyEvent.keystate(), keyEvent.keyid() );

    IL::BreakThread( std::bind( m_ProductNotify, productMessage ), m_mainTask );
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
    BOSE_INFO( s_logger, "The user interface is starting, but has not been implemented." );

    m_running = false;
}



////////////////////////////////////////////////////////////////////////////////////////////////////
///                               End of ProductApp Namespace                                    ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
