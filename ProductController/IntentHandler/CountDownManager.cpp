///////////////////////////////////////////////////////////////////////////////
/// @file CountDownManager.cpp
///
/// @brief Implementation of Count Down Manager for actions to display countdown
///        intends in the product Controller for multiple keys press
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

static DPrint s_logger( "CountDownManager" );

typedef struct _CountDown
{
    std::string     intentName;
    uint16_t        countdown;
} CountDownInfo;

static std::map <ProductApp::Action, CountDownInfo> m_countdownIntentInfoMap =
{
    {ProductApp::Action::MANUAL_UPDATE_COUNTDOWN, {"systemUpdate", 5}},
    {ProductApp::Action::FACTORY_DEFAULT_COUNTDOWN, {"factoryDefault", 10}},
    {ProductApp::Action::MANUAL_SETUP_COUNTDOWN, {"manualSetup", 5}},
    {ProductApp::Action::TOGGLE_WIFI_RADIO_COUNTDOWN, {"toggleWiFiRadio", 5}},
    {ProductApp::Action::SYSTEM_INFO_COUNTDOWN, {"systemInfo", 5}}
};

namespace ProductApp
{

CountDownManager::CountDownManager( NotifyTargetTaskIF& task,
                                    const CliClientMT& cliClient,
                                    const FrontDoorClientIF_t& frontDoorClient,
                                    ProductController& controller ):
    IntentManager( task, cliClient, frontDoorClient, controller ),
    m_counter( 0 ),
    m_actionType( 0 )
{
    m_frontDoorClientErrorCb = AsyncCallback<EndPointsError::Error>
                               ( std::bind( &CountDownManager::FrontDoorClientErrorCb,
                                            this, std::placeholders::_1 ), &task );
}

///////////////////////////////////////////////////////////////////////////////
/// @name  Handle
/// @brief Function to build and send FrontDoor message to execute the
//         key combination intends coming out of the product controller.
//         The callBack function is called to give control back to the state
//         machine if HSM has registered a call back.
/// @return true: Successful
//          false: Error
////////////////////////////////////////////////////////////////////////////////

bool CountDownManager::Handle( KeyHandlerUtil::ActionType_t& intent )
{
    BOSE_DEBUG( s_logger, "%s - (intent=%d)(m_counter=%d)", __func__, intent, m_counter );

    switch( intent )
    {
    case( uint16_t ) Action::FACTORY_DEFAULT_CANCEL:
    case( uint16_t ) Action::MANUAL_UPDATE_CANCEL:
    case( uint16_t ) Action::SYSTEM_INFO_CANCEL:
    case( uint16_t ) Action::TOGGLE_WIFI_RADIO_CANCEL:
    case( uint16_t ) Action::MANUAL_SETUP_CANCEL:
    {
        if( m_actionType and m_counter > 0 and m_counter <= m_countdownIntentInfoMap[( ProductApp::Action )m_actionType].countdown )
        {
            NotifyButtonEvent( m_countdownIntentInfoMap[( ProductApp::Action )m_actionType].intentName, IntentHandler::Protobuf::ButtonEventState::CANCELED, 0 );
            m_actionType = 0;
        }
        else if( m_counter == 0 )
        {
            m_actionType = 0;
        }
    }
    break;

    case( uint16_t ) Action::FACTORY_DEFAULT_COUNTDOWN:
    case( uint16_t ) Action::MANUAL_UPDATE_COUNTDOWN:
    case( uint16_t ) Action::MANUAL_SETUP_COUNTDOWN:
    case( uint16_t ) Action::TOGGLE_WIFI_RADIO_COUNTDOWN:
    case( uint16_t ) Action::SYSTEM_INFO_COUNTDOWN:
    {
        if( !m_actionType )
        {
            if( m_countdownIntentInfoMap.find( ( ProductApp::Action )intent ) != m_countdownIntentInfoMap.end() )
            {
                m_counter = m_countdownIntentInfoMap[( ProductApp::Action )intent].countdown + 1;
                m_actionType = ( uint16_t )intent;
            }
            else
            {
                BOSE_ERROR( s_logger, "Missing entry in m_countdownIntentInfoMap fpr intent = %d", intent );
                return false;
            }
        }

        if( m_counter )
        {
            m_counter--;
            if( m_counter )
            {
                NotifyButtonEvent( m_countdownIntentInfoMap[( ProductApp::Action )intent].intentName, IntentHandler::Protobuf::ButtonEventState::COUNTDOWN, m_counter );
            }
            else
            {
                NotifyButtonEvent( m_countdownIntentInfoMap[( ProductApp::Action )intent].intentName, IntentHandler::Protobuf::ButtonEventState::COMPLETED, 0 );
            }
        }
    }
    break;

    default:
    {
        BOSE_ERROR( s_logger, "Invalid Action type" );
    }
    return false;
    }

    //Fire the cb so the control goes back to the ProductController
    if( GetCallbackObject() != nullptr )
    {
        ( *GetCallbackObject() )( intent );
    }

    return true;
}

void CountDownManager::NotifyButtonEvent( const std::string& event, const uint16_t state, const uint16_t value )
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
