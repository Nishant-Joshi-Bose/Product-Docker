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
#include "PlaybackRequestManager.h"
#include "VoiceManager.h"
#include "EddieProductController.h"

static DPrint s_logger( "IntentHandler" );

namespace ProductApp
{

IntentHandler::IntentHandler( NotifyTargetTaskIF& task,
                              const CliClientMT& cliClient,
                              const FrontDoorClientIF_t& frontDoorClient,
                              ProductController& controller
                            ):
    CommonIntentHandler( task, cliClient, frontDoorClient, controller )
{
    BOSE_DEBUG( s_logger, "%s: ", __func__ );
}

void IntentHandler::Initialize()
{
    CommonIntentHandler::Initialize();
    BOSE_DEBUG( s_logger, "%s", __func__ );

#if 1 // @TODO PGC-321 move HandleNetworkStandbyIntentCb to Common
    auto func = std::bind( &EddieProductController::HandleNetworkStandbyIntentCb , static_cast<EddieProductController*>( &GetProductController() ), std::placeholders::_1 );
    auto cb = std::make_shared<AsyncCallback<KeyHandlerUtil::ActionType_t&> > ( func, &GetTask() );
    KeyHandlerUtil::ActionType_t intent = ( KeyHandlerUtil::ActionType_t ) ActionCommon_t::NETWORK_STANDBY;
    RegisterCallBack( intent, cb );
#endif

    //+ AUX Control API's
    IntentManagerPtr_t playbackRequestManager =
        std::make_shared<PlaybackRequestManager>( GetTask() , GetCli(),
                                                  GetFrontDoorClient(),
                                                  GetProductController() );

    m_IntentManagerMap[( uint16_t )Action::AUX_IN] = playbackRequestManager;
    m_IntentNotificationMap[( uint16_t ) Action::AUX_IN]        = "aux_in" ;
    //- AUX Control API's

    //+ (Manual Update)
    IntentManagerPtr_t valcunKeyManager =
        std::make_shared<CountdownManager>( m_task, m_cliClient,
                                            m_frontDoorClient,
                                            m_controller );

    m_IntentManagerMap[( uint16_t )Action::MANUAL_UPDATE] = valcunKeyManager;
    m_IntentManagerMap[( uint16_t )Action::MANUAL_UPDATE_CANCEL] = valcunKeyManager;
    m_IntentManagerMap[( uint16_t )Action::MANUAL_UPDATE_COUNTDOWN] = valcunKeyManager;
    //- (Manual Update)

    //+ (Factory Reset)
    m_IntentManagerMap[( uint16_t )Action::FACTORY_RESET] = valcunKeyManager;
    m_IntentManagerMap[( uint16_t )Action::FACTORY_RESET_CANCEL] = valcunKeyManager;
    m_IntentManagerMap[( uint16_t )Action::FACTORY_RESET_COUNTDOWN] = valcunKeyManager;
    //- (Factory Reset)

    //+ (Setup AP countdown and cancel)
    m_IntentManagerMap[( uint16_t )Action::SETUP_AP_COUNTDOWN] = valcunKeyManager;
    m_IntentManagerMap[( uint16_t )Action::SETUP_AP_CANCEL] = valcunKeyManager;
    //- (Setup AP countdown and cancel)

    //+ (Disable Network)
    m_IntentManagerMap[( uint16_t )Action::DISABLE_NETWORK_COUNTDOWN] = valcunKeyManager;
    m_IntentManagerMap[( uint16_t )Action::DISABLE_NETWORK_CANCEL] = valcunKeyManager;
    //- (Disable Network)

    //+ (PTS Update)
    m_IntentManagerMap[( uint16_t )Action::PTS_UPDATE] = valcunKeyManager;
    m_IntentManagerMap[( uint16_t )Action::PTS_UPDATE_CANCEL] = valcunKeyManager;
    m_IntentManagerMap[( uint16_t )Action::PTS_UPDATE_COUNTDOWN] = valcunKeyManager;
    //- (PTS Update)

    m_IntentNotificationMap[( uint16_t ) Action::MANUAL_UPDATE]        = "manual_update" ;
    m_IntentNotificationMap[( uint16_t ) Action::MANUAL_UPDATE_CANCEL]        = "manual_update_cancel" ;
    m_IntentNotificationMap[( uint16_t ) Action::MANUAL_UPDATE_COUNTDOWN]        = "manual_update_countdown" ;
    m_IntentNotificationMap[( uint16_t ) Action::FACTORY_RESET_CANCEL]        = "factory_reset_cancel" ;
    m_IntentNotificationMap[( uint16_t ) Action::FACTORY_RESET_COUNTDOWN]        = "factory_reset_countdown" ;
    m_IntentNotificationMap[( uint16_t ) Action::SETUP_AP_COUNTDOWN]        = "setup_ap_countdown" ;
    m_IntentNotificationMap[( uint16_t ) Action::SETUP_AP_CANCEL]        = "setup_ap_cancel" ;
    m_IntentNotificationMap[( uint16_t ) Action::DISABLE_NETWORK_COUNTDOWN]        = "disable_network_countdown" ;
    m_IntentNotificationMap[( uint16_t ) Action::DISABLE_NETWORK_CANCEL]        = "disable_network_cancel" ;
    m_IntentNotificationMap[( uint16_t ) Action::PTS_UPDATE]        = "pts_update" ;
    m_IntentNotificationMap[( uint16_t ) Action::PTS_UPDATE_CANCEL]        = "pts_update_cancel" ;
    m_IntentNotificationMap[( uint16_t ) Action::PTS_UPDATE_COUNTDOWN]        = "pts_update_countdown" ;
}

}

