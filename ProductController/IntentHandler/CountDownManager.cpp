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
namespace ProductApp
{
using KeyActionMode_t = std::pair<ActionCustom_t::Actions, uint32_t>;
}

namespace std
{
template<>
struct hash<ProductApp::KeyActionMode_t>
{
    typedef ProductApp::KeyActionMode_t argument_type;
    typedef std::size_t result_type;
    result_type operator()( argument_type const& s ) const
    {
        std::size_t seed = 0;
        seed += ( ( uint32_t ) s.first * 1 );
        seed += ( s.second * 10 );
        seed = seed % ProductApp::MAX_SEED_SIZE;
        return seed;
    }
};

template<>
struct equal_to<ProductApp::KeyActionMode_t>
{
    typedef ProductApp::KeyActionMode_t argument_type;
    bool operator()( argument_type const& s1, argument_type const& s2 ) const
    {
        return( s1 == s2 );

    }
};
}

typedef struct _CountDown
{
    ButtonEventName     intentName;
    uint16_t            countdown;
} CountDownInfo;


static std::unordered_map < ProductApp::KeyActionMode_t, CountDownInfo> m_countdownIntentInfoMap =
{
    {( std::make_pair( Action::FACTORY_DEFAULT_COUNTDOWN, 0 ) ), {ButtonEventName::FACTORY_DEFAULT, 10}},
    {( std::make_pair( Action::MANUAL_SETUP_COUNTDOWN,    0 ) ), {ButtonEventName::MANUAL_SETUP, 5}},
    {( std::make_pair( Action::TOGGLE_WIFI_RADIO_COUNTDOWN, ( uint32_t )( NetManager::Protobuf::NetworkOperationalMode::WIFI_OFF ) ) ) , {ButtonEventName::ENABLE_WIFI, 5}},
    {( std::make_pair( Action::TOGGLE_WIFI_RADIO_COUNTDOWN, ( uint32_t )( NetManager::Protobuf::NetworkOperationalMode::AUTO ) ) ) , {ButtonEventName::DISABLE_WIFI, 5}},
    {( std::make_pair( Action::TOGGLE_WIFI_RADIO_COUNTDOWN, ( uint32_t )( NetManager::Protobuf::NetworkOperationalMode::WIFI_SETUP ) ) ) , {ButtonEventName::DISABLE_WIFI, 5}},
    {( std::make_pair( Action::SYSTEM_INFO_COUNTDOWN, ( uint32_t )( false ) ) ), {ButtonEventName::ENTER_SYSTEM_INFO, 5}},
    {( std::make_pair( Action::SYSTEM_INFO_COUNTDOWN, ( uint32_t )( true ) ) ), {ButtonEventName::EXIT_SYSTEM_INFO, 5}},
    {( std::make_pair( Action::BLUETOOTH_CLEAR_PAIRING_COUNTDOWN,     0 ) ), {ButtonEventName::BLUETOOTH_CLEAR_PAIRING, 5}},
    {( std::make_pair( Action::MANUAL_SOFTWARE_INSTALL_COUNTDOWN, 0 ) ), {ButtonEventName::MANUAL_SOFTWARE_INSTALL, 10}}
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
    BOSE_DEBUG( s_logger, "%s - (intent=%s)(m_countdownValue=%d)", __func__, CommonIntentHandler::GetIntentName( intent ).c_str( ), m_countdownValue );

    auto getMode = [this]( ActionCustom_t::Actions action )
    {
        return ( ( ( action == Action::TOGGLE_WIFI_RADIO_COUNTDOWN ) )  ? ( ( uint32_t ) GetProductController().GetWiFiOperationalMode() ) :
                 ( ( action == Action::SYSTEM_INFO_COUNTDOWN ) ) ? ( ( uint32_t ) GetProductController().IsSystemInfoModeOn() ) : 0 ) ;
    };

    switch( intent )
    {
    case( uint16_t ) Action::FACTORY_DEFAULT_CANCEL:
    case( uint16_t ) Action::SYSTEM_INFO_CANCEL:
    case( uint16_t ) Action::TOGGLE_WIFI_RADIO_CANCEL:
    case( uint16_t ) Action::MANUAL_SETUP_CANCEL:
    case( uint16_t ) Action::BLUETOOTH_CLEAR_PAIRING_CANCEL:
    case( uint16_t ) Action::MANUAL_SOFTWARE_INSTALL_CANCEL:
    {
        if( m_actionType.is_initialized() and m_countdownValue > 0 and m_countdownValue <= m_countdownIntentInfoMap[ std::make_pair( ( ActionCustom_t::Actions )m_actionType.get(), getMode( ( ActionCustom_t::Actions ) m_actionType.get() ) )].countdown )
        {
            NotifyButtonEvent( m_countdownIntentInfoMap[std::make_pair( ( ActionCustom_t::Actions )m_actionType.get(), getMode( ( ActionCustom_t::Actions ) m_actionType.get() ) ) ].intentName, ButtonEventState::CANCEL, 0 );
            m_actionType.reset();
        }
        else if( m_countdownValue == 0 )
        {
            m_actionType.reset();
        }
    }
    break;

    case( uint16_t ) Action::FACTORY_DEFAULT_COUNTDOWN:
    case( uint16_t ) Action::MANUAL_SETUP_COUNTDOWN:
    case( uint16_t ) Action::TOGGLE_WIFI_RADIO_COUNTDOWN:
    case( uint16_t ) Action::SYSTEM_INFO_COUNTDOWN:
    case( uint16_t ) Action::BLUETOOTH_CLEAR_PAIRING_COUNTDOWN:
    case( uint16_t ) Action::MANUAL_SOFTWARE_INSTALL_COUNTDOWN:
    {
        if( !m_actionType.is_initialized() )
        {
            auto iter = m_countdownIntentInfoMap.find( std::make_pair( ( ActionCustom_t::Actions )intent, getMode( ( ActionCustom_t::Actions ) intent ) ) ) ;
            if( iter != m_countdownIntentInfoMap.end() )
            {
                m_countdownValue = iter->second.countdown + 1;
                m_actionType = ( ActionCustom_t::Actions )intent;
            }
            else
            {
                BOSE_ERROR( s_logger, "Missing entry in m_countdownIntentInfoMap for intent = %s", CommonIntentHandler::GetIntentName( intent ).c_str( ) );
                return false;
            }
        }

        if( m_countdownValue )
        {
            m_countdownValue--;
            if( m_countdownValue )
            {
                NotifyButtonEvent( m_countdownIntentInfoMap[ std::make_pair( ( ActionCustom_t::Actions )intent, getMode( ( ActionCustom_t::Actions )intent ) ) ].intentName, ButtonEventState::COUNTDOWN, m_countdownValue );
            }
            else
            {
                NotifyButtonEvent( m_countdownIntentInfoMap[ std::make_pair( ( ActionCustom_t::Actions )intent, getMode( ( ActionCustom_t::Actions )intent ) ) ].intentName, ButtonEventState::COMPLETED, 0 );
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
