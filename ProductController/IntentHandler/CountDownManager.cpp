///////////////////////////////////////////////////////////////////////////////
/// @file CountDownManager.cpp
///
/// @brief Implementation of Bluetooth Manager for actions from Bluetooth
//         intends in the product Controller
///
/// @attention
///    BOSE CORPORATION.
///    COPYRIGHT 2018 BOSE CORPORATION ALL RIGHTS RESERVED.
///    This program may not be reproduced, in whole or in part in any
///    form or any means whatsoever without the written permission of:
///        BOSE CORPORATION
///        The Mountain
///        Framingham, MA 01701-9168
///
///////////////////////////////////////////////////////////////////////////////

#include "DPrint.h"
#include "IntentHandler.pb.h"
#include "CountDownManager.h"
#include "ProductController.h"
#include "Intents.h"

constexpr char BUTTON_EVENT_NOTIFICATION_URL[] = "/system/buttonEvent";
#define FACTORY_RESET_TIME            10
#define FIVE_SECOND_TIME              5

static DPrint s_logger( "CountDownManager" );

static std::map <ProductApp::Action, std::string> m_eventName =
{
    {ProductApp::Action::MANUAL_UPDATE_COUNTDOWN, "systemUpdate"},
    {ProductApp::Action::FACTORY_RESET_COUNTDOWN, "factoryReset"},
    {ProductApp::Action::SETUP_AP_COUNTDOWN, "setupAp"},
    {ProductApp::Action::DISABLE_NETWORK_COUNTDOWN, "disableNetwork"},
    {ProductApp::Action::PTS_UPDATE_COUNTDOWN, "ptsUpdate"}
};

namespace ProductApp
{

CountDownManager::CountDownManager( NotifyTargetTaskIF& task,
                                    const CliClientMT& cliClient,
                                    const FrontDoorClientIF_t& frontDoorClient,
                                    ProductController& controller ):
    IntentManager( task, cliClient, frontDoorClient, controller ),
    m_eventType( 0 ),
    m_shortCounter( 5 ),
    m_factoryResetCounter( 10 )
{
    m_frontDoorClientErrorCb = AsyncCallback<FRONT_DOOR_CLIENT_ERRORS>
                               ( std::bind( &CountDownManager::FrontDoorClientErrorCb,
                                            this, std::placeholders::_1 ), &task );

}

///////////////////////////////////////////////////////////////////////////////
/// @name  Handle
/// @brief Function to build and send FrontDoor message to execute the
//         to Bluetooth intends coming out of the product controller.
//         The callBack function is called to give control back to the state
//         machine if HSM has registered a call back.
/// @return true: Successful
//          false: Error
////////////////////////////////////////////////////////////////////////////////

bool CountDownManager::Handle( KeyHandlerUtil::ActionType_t& intent )
{
    BOSE_DEBUG( s_logger, "%s - (intent=%d)", __func__, intent );

    switch( intent )
    {
    case( uint16_t ) Action::FACTORY_RESET_CANCEL:
    {
        if( m_factoryResetCounter > 0 and m_factoryResetCounter < FACTORY_RESET_TIME and m_eventType )
        {
            NotifyButtonEvent( m_eventName[( ProductApp::Action )m_eventType], IntentHandler::Protobuf::ButtonEventState::CANCELED, 0 );
            m_eventType = 0;
        }
        m_factoryResetCounter = FACTORY_RESET_TIME;
    }
    break;

    case( uint16_t ) Action::MANUAL_UPDATE_CANCEL:
    case( uint16_t ) Action::PTS_UPDATE_CANCEL:
    case( uint16_t ) Action::DISABLE_NETWORK_CANCEL:
    case( uint16_t ) Action::SETUP_AP_CANCEL:
    {
        if( m_shortCounter > 0 and m_shortCounter < FIVE_SECOND_TIME && m_eventType )
        {
            NotifyButtonEvent( m_eventName[( ProductApp::Action )m_eventType], IntentHandler::Protobuf::ButtonEventState::CANCELED, 0 );
            m_eventType = 0;
        }
        m_shortCounter = FIVE_SECOND_TIME;
    }
    break;

    case( uint16_t ) Action::FACTORY_RESET_COUNTDOWN:
    {
        if( m_factoryResetCounter )
        {
            m_factoryResetCounter--;
            if( m_factoryResetCounter )
            {
                NotifyButtonEvent( m_eventName[( ProductApp::Action )intent], IntentHandler::Protobuf::ButtonEventState::COUNTDOWN, m_factoryResetCounter );
            }
            else
            {
                NotifyButtonEvent( m_eventName[( ProductApp::Action )intent], IntentHandler::Protobuf::ButtonEventState::COMPLETED, m_factoryResetCounter );
            }
            m_eventType = ( uint32_t )intent;
        }
    }
    break;

    case( uint16_t ) Action::MANUAL_UPDATE_COUNTDOWN:
    case( uint16_t ) Action::SETUP_AP_COUNTDOWN:
    case( uint16_t ) Action::DISABLE_NETWORK_COUNTDOWN:
    case( uint16_t ) Action::PTS_UPDATE_COUNTDOWN:
    {
        if( m_shortCounter )
        {
            m_shortCounter--;
            if( m_shortCounter )
            {
                NotifyButtonEvent( m_eventName[( ProductApp::Action )intent], IntentHandler::Protobuf::ButtonEventState::COUNTDOWN, m_shortCounter );
            }
            else
            {
                NotifyButtonEvent( m_eventName[( ProductApp::Action )intent], IntentHandler::Protobuf::ButtonEventState::COMPLETED, m_shortCounter );
            }
            m_eventType = ( uint32_t )intent;
        }
    }
    break;

    }

    //Fire the cb so the control goes back to the ProductController
    if( GetCallbackObject() != nullptr )
    {
        ( *GetCallbackObject() )( intent );
    }
    return true;
}

void CountDownManager::NotifyButtonEvent( const std::string& event, const uint32_t& state, const uint32_t value )
{
    BOSE_DEBUG( s_logger, "%s: event = %s, state = %d, value = %d", __func__, event.c_str(), state, value );
    IntentHandler::Protobuf::ButtonEventNotification buttonNotification;

    buttonNotification.set_event( event );
    buttonNotification.set_state( ( IntentHandler::Protobuf::ButtonEventState )state );

    if( value )
    {
        buttonNotification.set_value( value );
    }
    GetFrontDoorClient()->SendNotification( BUTTON_EVENT_NOTIFICATION_URL, buttonNotification );
}

}
