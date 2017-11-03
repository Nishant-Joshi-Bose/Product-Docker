////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file      ProductUserInterface.h
///
/// @brief     This header file declares a ProductUserInterface class that is used to receive user
///            input.
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
#include <chrono>
#include <thread>
#include <unistd.h>
#include "SystemUtils.h"
#include "Utilities.h"
#include "DPrint.h"
#include "APTask.h"
#include "BreakThread.h"
#include "ProductController.h"
#include "ProductHardwareInterface.h"
#include "ProductUserInterface.h"
#include "ProductMessage.pb.h"
#include "KeyActions.pb.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
///
///            Included Namespaces
///
////////////////////////////////////////////////////////////////////////////////////////////////////
using namespace KeyActionPb;

////////////////////////////////////////////////////////////////////////////////////////////////////
///                          Start of the Product Application Namespace                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
namespace ProductApp
{

////////////////////////////////////////////////////////////////////////////////////////////////////
///
///            Constant Definitions
///
////////////////////////////////////////////////////////////////////////////////////////////////////
constexpr uint32_t PRODUCT_USER_INTERFACE_RETRY_IN_SECONDS = 1;

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
/// @name   ProductUserInterface::GetInstance
///
/// @brief  This static method creates the one and only instance of a ProductUserInterface object.
///         The C++ Version 11 compiler guarantees that only one instance is created in a thread
///         safe way.
///
/// @param  NotifyTargetTaskIF*        ProductTask
///
/// @param  Callback< ProductMessage > ProductNotify
///
/// @param  ProductHardwareInterface*  HardwareInterface
///
/// @param  CliClientMT&               CommandLineInterface
///
/// @return This method returns a pointer to a ProductUserInterface object.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
ProductUserInterface* ProductUserInterface::GetInstance
( NotifyTargetTaskIF*         ProductTask,
  Callback< ProductMessage >  ProductNotify,
  ProductHardwareInterface*   HardwareInterface,
  CliClientMT&                CommandLineInterface )
{
    static ProductUserInterface* instance = new ProductUserInterface( ProductTask,
                                                                      ProductNotify,
                                                                      HardwareInterface,
                                                                      CommandLineInterface );

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
/// @param  NotifyTargetTaskIF*        ProductTask
///
/// @param  Callback< ProductMessage > ProductNotify
///
/// @param  ProductHardwareInterface*  HardwareInterface
///
/// @param  CliClientMT&               CommandLineInterface
///
////////////////////////////////////////////////////////////////////////////////////////////////////
ProductUserInterface::ProductUserInterface( NotifyTargetTaskIF*        ProductTask,
                                            Callback< ProductMessage > ProductNotify,
                                            ProductHardwareInterface*  HardwareInterface,
                                            CliClientMT&               CommandLineInterface )

    : m_ProductTask( ProductTask ),
      m_keyEventTask( IL::CreateTask( "ProductMonitorNetworkTask" ) ),
      m_ProductNotify( ProductNotify ),
      m_ProductHardwareInterface( HardwareInterface ),
      m_running( false ),
      m_KeyHandler( *ProductTask, CommandLineInterface, m_keyConfigFileName )
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductUserInterface::Run
///
/// @brief  This method starts the ProductUserInterface instance.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductUserInterface::Run( )
{
    BOSE_DEBUG( s_logger, "%s: The user interface is starting.", __FUNCTION__ );

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
void ProductUserInterface::RegisterForKeyEvents( )
{
    BOSE_DEBUG( s_logger, "The user interface is attempting to register for key events." );

    ///
    /// Repeated attempts are made to sucessfully pass a callback to the hardware interface to
    /// receive key events from the LPM. This callback when invoked will send a message to the
    /// ProductController class that handles the state machine.
    ///
    Callback< LpmServiceMessages::IpcKeyInformation_t >
    CallbackForKeyEvents( std::bind( &ProductUserInterface::HandleKeyEvent,
                                     this,
                                     std::placeholders::_1 ) );

    while( !m_ProductHardwareInterface->RegisterForLpmEvents( IPC_KEY, CallbackForKeyEvents ) )
    {
        sleep( PRODUCT_USER_INTERFACE_RETRY_IN_SECONDS );
    }

    ///
    /// Register with the key handler and repeat management code to translate raw LPM keys
    /// into actions.
    ///
    auto KeyCallback = [ this ]( uint32_t result )
    {
        KeyInformationCallBack( result );
    };

    auto CallbackForKeyInformation = std::make_shared< AsyncCallback < uint32_t > > ( KeyCallback,
                                     m_ProductTask );


    m_KeyHandler.RegisterKeyHandler( CallbackForKeyInformation );

    BOSE_DEBUG( s_logger, "The user interface has registered for key events." );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name  ProductUserInterface::HandleKeyEvent
///
/// @param LpmServiceMessages::IpcKeyInformation_t keyEvent
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductUserInterface::HandleKeyEvent( LpmServiceMessages::IpcKeyInformation_t keyEvent )
{
    std::string    keyOriginString;
    std::string    keyStateString;
    std::string    keyIdString;

    if( keyEvent.has_keyorigin( ) )
    {
        keyOriginString.assign( KeyOrigin_t_Name( keyEvent.keyorigin( ) ) );
    }
    else
    {
        keyOriginString.assign( "UNKNOWN" );
    }

    if( keyEvent.has_keystate( ) )
    {
        keyStateString.assign( KeyState_t_Name( keyEvent.keystate( ) ) );
    }
    else
    {
        keyStateString.assign( "UNKNOWN" );
    }

    if( keyEvent.has_keyid( ) )
    {
        keyIdString.assign( std::to_string( keyEvent.keyid( ) ) );
    }
    else
    {
        keyIdString.assign( "UNKNOWN" );
    }

    BOSE_DEBUG( s_logger, "--------------- Product Controller Key Event ---------------" );
    BOSE_DEBUG( s_logger, "A key event from the hardware interface was received: " );
    BOSE_DEBUG( s_logger, " " );
    BOSE_DEBUG( s_logger, "  Key Origin : %s ", keyOriginString.c_str( ) );
    BOSE_DEBUG( s_logger, "  Key State  : %s ", keyStateString.c_str( ) );
    BOSE_DEBUG( s_logger, "  Key ID     : %s ", keyIdString.c_str( ) );
    BOSE_DEBUG( s_logger, " " );

    ///
    /// If the message is incomplete, dump it.
    ///
    if( !keyEvent.has_keyorigin( ) || !keyEvent.has_keystate( ) || !keyEvent.has_keyid( ) )
    {
        BOSE_ERROR( s_logger, "This event cannot be processed, as it is missing data." );

        return;
    }

    ///
    /// Feed the key into the key handler.
    ///
    m_KeyHandler.HandleKeys( keyEvent.keyorigin( ),
                             keyEvent.keystate( ),
                             keyEvent.keyid( ) );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief ProductUserInterface::KeyInformationCallBack
///
/// @param uint32_t keyAction
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductUserInterface::KeyInformationCallBack( const int keyAction )
{
    ///
    /// The key action is sent to the Product Controller state machine. Power on or off key actions
    /// are to be sent as simple product power type messages, whereas the value of other key actions
    /// are sent as product key data messages containing the key action value for further
    /// processing.
    ///
    BOSE_DEBUG( s_logger, "A key press or presses have been translated to the action %d.", keyAction );

    if( keyAction == KEY_ACTION_POWER )
    {
        ProductMessage productMessage;
        productMessage.set_power( true );

        IL::BreakThread( std::bind( m_ProductNotify,
                                    productMessage ),
                         m_ProductTask );
    }
    else
    {
        ProductMessage productMessage;
        productMessage.mutable_keydata( )->set_action( static_cast< KEY_ACTION >( keyAction ) );

        IL::BreakThread( std::bind( m_ProductNotify,
                                    productMessage ),
                         m_ProductTask );
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief  ProductUserInterface::GetKeyString
///
/// @param  const KEY_ACTION keyAction
///
/// @return This method return a std::string associated with the key action.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
std::string ProductUserInterface::GetKeyString( const KEY_ACTION keyAction )
{
    std::string keyString( "UNKNOWN" );

    if( KEY_ACTION_IsValid( keyAction ) )
    {
        keyString.assign( KEY_ACTION_Name( keyAction ) );
    }

    return keyString;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductUserInterface::Stop
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductUserInterface::Stop( )
{
    BOSE_DEBUG( s_logger, "The user interface is stopping." );

    m_running = false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                           End of the Product Application Namespace                           ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
