////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file      ProductKeyInputInterface.cpp
///
/// @brief     This source code files implements a ProductKeyInputInterface class that is used to
///            receive raw key input from the LPM hardware and convert it into key actions.
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
#include "KeyActions.pb.h"
#include "ProductMessage.pb.h"
#include "ProfessorProductController.h"
#include "CustomProductHardwareInterface.h"
#include "ProductKeyInputInterface.h"

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
constexpr const char* KEY_CONFIGURATION_FILE_NAME = "/opt/Bose/etc/KeyConfiguration.json";

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductKeyInputInterface::ProductKeyInputInterface
///
/// @brief  This method is the ProductKeyInputInterface constructor.
///
/// @param  ProfessorProductController& ProductController
///
////////////////////////////////////////////////////////////////////////////////////////////////////
ProductKeyInputInterface::ProductKeyInputInterface( ProfessorProductController& ProductController ) :

    ///
    /// Product Controller Interface
    ///
    m_ProductTask( ProductController.GetTask( ) ),
    m_ProductNotify( ProductController.GetMessageHandler( ) ),
    m_ProductHardwareInterface( ProductController.GetHardwareInterface( ) ),
    ///
    /// Instantiation of the Key Handler
    ///
    m_KeyHandler( *m_ProductTask,
                  ProductController.GetCommandLineInterface( ),
                  KEY_CONFIGURATION_FILE_NAME ),
    ///
    /// Initialization of Class Members
    ///
    m_connected( false ),
    m_running( false )
{
    ///
    /// Register for LPM connected events after which key registration can be made.
    ///
    Callback< bool > callback( std::bind( &ProductKeyInputInterface::ConnectToLpm,
                                          this,
                                          std::placeholders::_1 ) );

    m_ProductHardwareInterface->RegisterForLpmConnection( callback );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductKeyInputInterface::ConnectToLpm
///
/// @brief  This method starts the ProductKeyInputInterface instance.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductKeyInputInterface::ConnectToLpm( bool connected )
{
    BOSE_DEBUG( s_logger, "%s: The user interface is starting.", __FUNCTION__ );

    m_connected = connected;

    ///
    /// Attempt to register for key events if connected to the LPM hardware interface.
    ///
    if( m_connected )
    {
        IL::BreakThread( std::bind( &ProductKeyInputInterface::RegisterForKeyEvents,
                                    this ),
                         m_ProductTask );
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductKeyInputInterface::Run
///
/// @brief  This method runs the ProductKeyInputInterface instance. Note that key events will not be
///         sent to the product controller, which creates an instance of this class, until it is
///         set to run.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductKeyInputInterface::Run( )
{
    BOSE_DEBUG( s_logger, "%s: The user interface is running.", __FUNCTION__ );

    m_running = true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief ProductKeyInputInterface::RegisterForKeyEvents
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductKeyInputInterface::RegisterForKeyEvents( )
{
    BOSE_DEBUG( s_logger, "The user interface is attempting to register for key events." );

    ///
    /// A callback is passed to the hardware interface to receive key events from the LPM. This
    /// callback when invoked will send a message to the ProductController class that handles the
    /// state machine.
    ///
    Callback< LpmServiceMessages::IpcKeyInformation_t >
    CallbackForKeyEvents( std::bind( &ProductKeyInputInterface::HandleKeyEvent,
                                     this,
                                     std::placeholders::_1 ) );

    m_ProductHardwareInterface->RegisterForLpmEvents( IPC_KEY, CallbackForKeyEvents );

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
/// @name  ProductKeyInputInterface::HandleKeyEvent
///
/// @param LpmServiceMessages::IpcKeyInformation_t keyEvent
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductKeyInputInterface::HandleKeyEvent( LpmServiceMessages::IpcKeyInformation_t keyEvent )
{
    std::string keyOriginString;
    std::string keyStateString;
    std::string keyIdString;

    if( not m_running )
    {
        BOSE_DEBUG( s_logger, "--------------- Product Controller User Key Event ---------------" );
        BOSE_DEBUG( s_logger, "The user interface for key events has not been set to run." );

        return;
    }

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
/// @brief ProductKeyInputInterface::KeyInformationCallBack
///
/// @param uint32_t keyAction
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductKeyInputInterface::KeyInformationCallBack( const int keyAction )
{
    ///
    /// The key action is sent to the Product Controller for processing.
    ///
    BOSE_DEBUG( s_logger, "A key press or presses have been translated to the action %d.", keyAction );

    ProductMessage productMessage;
    if( keyAction > KeyActionPb::KEY_ACTION_LAST_COMMON )
    {
        // @TODO for now, until Professor implements its non-common intents
        productMessage.mutable_keydata( )->set_action( static_cast< KEY_ACTION >( keyAction ) );
    }
    else
    {
        // Need to send a no-op key to ProductController, to reset the inactivity timer
        ProductMessage productNoopKeyMessage;
        productNoopKeyMessage.mutable_keydata( )->set_action( static_cast< KEY_ACTION >( 0 ) );
        IL::BreakThread( std::bind( m_ProductNotify,
                                    productNoopKeyMessage ),
                         m_ProductTask );

        productMessage.set_intent( keyAction );
    }

    IL::BreakThread( std::bind( m_ProductNotify,
                                productMessage ),
                     m_ProductTask );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductKeyInputInterface::Stop
///
/// @todo  Resources, memory, or any client server connections that may need to be released by
///        this module when stopped will need to be determined.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductKeyInputInterface::Stop( )
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
