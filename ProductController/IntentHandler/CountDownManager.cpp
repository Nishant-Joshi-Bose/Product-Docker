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

static DPrint s_logger( "CountDownManager" );

using namespace IntentHandler::Protobuf;

typedef struct _CountDown
{
    ButtonEventName     intentName;
    uint16_t            countdown;
} CountDownInfo;

static std::map <ProductApp::Action, CountDownInfo> m_countdownIntentInfoMap =
{
    {ProductApp::Action::MANUAL_UPDATE_COUNTDOWN, {ButtonEventName::MANUAL_UPDATE, 5}},
    {ProductApp::Action::FACTORY_DEFAULT_COUNTDOWN, {ButtonEventName::FACTORY_DEFAULT, 10}},
    {ProductApp::Action::MANUAL_SETUP_COUNTDOWN, {ButtonEventName::MANUAL_SETUP, 5}},
    {ProductApp::Action::TOGGLE_WIFI_RADIO_COUNTDOWN, {ButtonEventName::ENABLE_WIFI, 5}},
    {ProductApp::Action::SYSTEM_INFO_COUNTDOWN, {ButtonEventName::SYSTEM_INFO, 5}}
};

namespace ProductApp
{

CountDownManager::CountDownManager( NotifyTargetTaskIF& task,
                                    const CliClientMT& cliClient,
                                    const FrontDoorClientIF_t& frontDoorClient,
                                    ProductController& controller ):
    IntentManager( task, cliClient, frontDoorClient, controller ),
    m_countdownValue( 0 ),
    m_actionType()
{

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
    BOSE_DEBUG( s_logger, "%s - (intent=%d)(m_countdownValue=%d)", __func__, intent, m_countdownValue );

    switch( intent )
    {
    case( uint16_t ) Action::FACTORY_DEFAULT_CANCEL:
    case( uint16_t ) Action::MANUAL_UPDATE_CANCEL:
    case( uint16_t ) Action::SYSTEM_INFO_CANCEL:
    case( uint16_t ) Action::TOGGLE_WIFI_RADIO_CANCEL:
    case( uint16_t ) Action::MANUAL_SETUP_CANCEL:
    {
        if( m_actionType.is_initialized() and m_countdownValue > 0 and m_countdownValue <= m_countdownIntentInfoMap[( ProductApp::Action )m_actionType.get()].countdown )
        {
            NotifyButtonEvent( m_countdownIntentInfoMap[( ProductApp::Action )m_actionType.get()].intentName, ButtonEventState::CANCEL, 0 );
            m_actionType.reset();
        }
        else if( m_countdownValue == 0 )
        {
            m_actionType.reset();
        }
    }
    break;

    case( uint16_t ) Action::FACTORY_DEFAULT_COUNTDOWN:
    case( uint16_t ) Action::MANUAL_UPDATE_COUNTDOWN:
    case( uint16_t ) Action::MANUAL_SETUP_COUNTDOWN:
    case( uint16_t ) Action::TOGGLE_WIFI_RADIO_COUNTDOWN:
    case( uint16_t ) Action::SYSTEM_INFO_COUNTDOWN:
    {
        if( !m_actionType.is_initialized() )
        {
            if( m_countdownIntentInfoMap.find( ( ProductApp::Action )intent ) != m_countdownIntentInfoMap.end() )
            {
                m_countdownValue = m_countdownIntentInfoMap[( ProductApp::Action )intent].countdown + 1;
                m_actionType = ( ProductApp::Action )intent;
            }
            else
            {
                BOSE_ERROR( s_logger, "Missing entry in m_countdownIntentInfoMap for intent = %d", intent );
                return false;
            }
        }

        if( m_countdownValue )
        {
            m_countdownValue--;
            if( m_countdownValue )
            {
                NotifyButtonEvent( m_countdownIntentInfoMap[( ProductApp::Action )intent].intentName, ButtonEventState::COUNTDOWN, m_countdownValue );
            }
            else
            {
                NotifyButtonEvent( m_countdownIntentInfoMap[( ProductApp::Action )intent].intentName, ButtonEventState::COMPLETED, 0 );
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

}
