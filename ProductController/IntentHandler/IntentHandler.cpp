///////////////////////////////////////////////////////////////////////////////
/// @file IntentHandler.cpp
///
/// @brief Implementation of Intent Handler
//  This module will offload the work of the Product Controller validate
//  and build LAN API or IPC messages to perform actions of various Intents
//  It also lets the Product Controller get control back in an Async way, so
//  the HSM can perform state changes if it needs to.
//  This Handler should not perform state transistions.
//  It is intentional that the hsm or productController access is not given
//  to this module.
//  The Handler in-turn would delegate its work to customized Intent
//  Managers based on the ActionType that is passed to it.
//  These customized IntentManager will be taking actions based on
//  1. State of its own subsystem, based on what was processed for the same
//  action before, like a play or pause would have to toggle the actions.
//  2. The IntentHandler will call various IntentManagers that are registered
//  for specific intents. The initializing of IntentManagers needs to be done
//  in IntentHandler::Initialize() for all intents that needs to be handled
//  by this module.
//
///
/// @attention
///    BOSE CORPORATION.
///    COPYRIGHT 2017 BOSE CORPORATION ALL RIGHTS RESERVED.
///    This program may not be reproduced, in whole or in part in any
///    form or any means whatsoever without the written permission of:
///        BOSE CORPORATION
///        The Mountain
///        Framingham, MA 01701-9168
///
///////////////////////////////////////////////////////////////////////////////

#include <unordered_map>
#include "DPrint.h"
#include "IntentHandler.h"
#include "TransportControlManager.h"
#include "PlaybackRequestManager.h"
#include "NetworkStandbyManager.h"
#include "IntentHandler.pb.h"
#include "BluetoothManager.h"
#include "VoiceManager.h"

static DPrint s_logger( "IntentHandler" );

constexpr char BUTTON_EVENT_NOTIFICATION_URL[] = "/system/buttonEvent";

using namespace IntentHandler::Protobuf;

namespace ProductApp
{

IntentHandler::IntentHandler( NotifyTargetTaskIF& task,
                              const CliClientMT& cliClient,
                              const FrontDoorClientIF_t& frontDoorClient,
                              EddieProductController& controller
                            ):
    m_task( task ),
    m_cliClient( cliClient ),
    m_frontDoorClient( frontDoorClient ),
    m_controller( controller )
{
    BOSE_DEBUG( s_logger, "%s: ", __func__ );
    Initialize();
}

void IntentHandler::Initialize()
{
    BOSE_DEBUG( s_logger, "%s", __func__ );
    //+ Transport Control API's
    IntentManagerPtr_t transportManager =
        std::make_shared<TransportControlManager>( m_task, m_cliClient,
                                                   m_frontDoorClient,
                                                   m_controller );

    m_IntentManagerMap[( uint16_t )Action::PLAY_PAUSE] = transportManager;
    m_IntentManagerMap[( uint16_t )Action::NEXT_TRACK] = transportManager;
    m_IntentManagerMap[( uint16_t )Action::PREV_TRACK] = transportManager;

    //- Transport Control API's
    //
    //+ Bluetooth Control API's
    IntentManagerPtr_t bluetoothRequestManager =
        std::make_shared<BluetoothManager>( m_task, m_cliClient,
                                            m_frontDoorClient,
                                            m_controller );

    m_IntentManagerMap[( uint16_t )Action::CAROUSEL_DISCOVERABLE_CONNECT_TO_LAST] = bluetoothRequestManager;
    m_IntentManagerMap[( uint16_t )Action::SEND_TO_DISCOVERABLE] = bluetoothRequestManager;
    m_IntentManagerMap[( uint16_t )Action::CLEAR_PAIRING_LIST] = bluetoothRequestManager;

    //- Bluetooth Control API's
    //
    //+ Networking Control API's

    //- Networking Control API's
    //
    //+ Miscellaneous Control API's (LPS, Factory Reset, NetworkStandy)
    IntentManagerPtr_t networkStandbyManager =
        std::make_shared<NetworkStandbyManager>( m_task, m_cliClient,
                                                 m_frontDoorClient,
                                                 m_controller );

    m_IntentManagerMap[( uint16_t )Action::NETWORK_STANDBY] = networkStandbyManager;

    auto func = std::bind( &EddieProductController::HandleNetworkStandbyIntentCb , &GetProductController(), std::placeholders::_1 );
    auto cb = std::make_shared<AsyncCallback<KeyHandlerUtil::ActionType_t&> > ( func, &m_task );
    KeyHandlerUtil::ActionType_t intent = ( KeyHandlerUtil::ActionType_t ) Action::NETWORK_STANDBY;
    RegisterCallBack( intent, cb );
    //- Miscellaneous Control API's (LPS, Factory Reset, NetworkStandy)
    //+ Preset Control API's

    //- Preset Control API's
    //+ AUX Control API's
    IntentManagerPtr_t playbackRequestManager =
        std::make_shared<PlaybackRequestManager>( m_task, m_cliClient,
                                                  m_frontDoorClient,
                                                  m_controller );

    m_IntentManagerMap[( uint16_t )Action::AUX_IN] = playbackRequestManager;
    //- AUX Control API's

    //+ Voice (Alexa) Control API's
    IntentManagerPtr_t voiceRequestManager =
        std::make_shared<VoiceManager>( m_task, m_cliClient,
                                        m_frontDoorClient,
                                        m_controller );
    m_IntentManagerMap[( uint16_t )Action::VOICE_CAROUSEL] = voiceRequestManager;
    //- Voice (Alexa) Control API's


    // prepare map for button event notification
    m_IntentNotificationMap[( uint16_t ) Action::PLAY_PAUSE]    = "play_pause" ;
    m_IntentNotificationMap[( uint16_t ) Action::NEXT_TRACK]    = "next_track" ;
    m_IntentNotificationMap[( uint16_t ) Action::PREV_TRACK]    = "prev_track" ;

    m_IntentNotificationMap[( uint16_t ) Action::CAROUSEL_DISCOVERABLE_CONNECT_TO_LAST] \
        = "carousel_discoverable_connect_to_last" ;
    m_IntentNotificationMap[( uint16_t ) Action::SEND_TO_DISCOVERABLE]     = "send_to_discoverable" ;
    m_IntentNotificationMap[( uint16_t ) Action::CLEAR_PAIRING_LIST] = "clear_pairing_list" ;

    m_IntentNotificationMap[( uint16_t ) Action::NETWORK_STANDBY] = "network_standby" ;

    m_IntentNotificationMap[( uint16_t ) Action::VOLUME_UP]     = "volume_up" ;
    m_IntentNotificationMap[( uint16_t ) Action::VOLUME_DOWN]   = "volume_down" ;

    m_IntentNotificationMap[( uint16_t ) Action::AUX_IN]        = "aux_in" ;

    m_IntentNotificationMap[( uint16_t ) Action::VOICE_CAROUSEL] = "Voice_Control" ;

    return;
}

bool IntentHandler::Handle( KeyHandlerUtil::ActionType_t& intent )
{
    BOSE_DEBUG( s_logger, "%s: ", __func__ );

    //notify button event if required to notify
    NotifyButtonEvent( intent );

    IntentManagerMap_t::iterator iter = m_IntentManagerMap.find( intent );
    if( iter != m_IntentManagerMap.end() )
    {

        iter->second->Handle( intent );
        BOSE_DEBUG( s_logger, "Found the Handle for intent :%d", intent );
        return( true );
    }
    else
    {
        BOSE_ERROR( s_logger, "Handle not found for intent : %d, check "
                    "initialization code", intent );
        return false;
    }
}

void IntentHandler::RegisterCallBack( KeyHandlerUtil::ActionType_t& intent,
                                      CbPtr_t cb )
{
    BOSE_DEBUG( s_logger, "%s: ", __func__ );
    IntentManagerMap_t::iterator iter = m_IntentManagerMap.find( intent );
    if( iter != m_IntentManagerMap.end() )
    {
        iter->second->RegisterCallBack( intent, cb );
        BOSE_DEBUG( s_logger, "Found the Manager for intent :%d", intent );
        return;
    }
    else
    {
        BOSE_ERROR( s_logger, "Manager not found for intent : %d, check "
                    "initialization code", intent );
        return;
    }
    return;
}

void IntentHandler::NotifyButtonEvent( KeyHandlerUtil::ActionType_t intent )
{

    BOSE_DEBUG( s_logger, "%s: ", __func__ );
    auto iter = m_IntentNotificationMap.find( ( uint16_t )intent );

    //found handle, notify
    if( iter != m_IntentNotificationMap.end() )
    {
        ButtonEventNotification btn_notification;
        btn_notification.set_event( m_IntentNotificationMap[ intent ] );
        m_frontDoorClient->SendNotification( BUTTON_EVENT_NOTIFICATION_URL, btn_notification );
    }
}
}

