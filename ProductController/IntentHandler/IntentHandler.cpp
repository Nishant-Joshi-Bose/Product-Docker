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
#include "AuxInHandler.h"
#include "CountDownManager.h"
#include "VoiceManager.h"

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

    //+ AUX Control API's
    IntentManagerPtr_t auxInHandler =
        std::make_shared<AuxInHandler>( GetTask() , GetCli(),
                                        GetFrontDoorClient(),
                                        GetProductController() );

    m_IntentManagerMap[( uint16_t )Action::AUX_IN] = auxInHandler;
    //- AUX Control API's

    //+ (Common Countdown manager for key combination)
    IntentManagerPtr_t countDownManager =
        std::make_shared<CountDownManager>( GetTask() , GetCli(),
                                            GetFrontDoorClient(),
                                            GetProductController() );

    //+ (Manual Update countdown and cancel)
    m_IntentManagerMap[( uint16_t )Action::MANUAL_UPDATE_COUNTDOWN] = countDownManager;
    m_IntentManagerMap[( uint16_t )Action::MANUAL_UPDATE_CANCEL] = countDownManager;
    //- (Manual Update countdown and cancel)

    //+ (Factory Reset countdown and cancel)
    m_IntentManagerMap[( uint16_t )Action::FACTORY_DEFAULT_COUNTDOWN] = countDownManager;
    m_IntentManagerMap[( uint16_t )Action::FACTORY_DEFAULT_CANCEL] = countDownManager;
    //- (Factory Reset countdown and cancel)

    //+ (Setup AP countdown and cancel)
    m_IntentManagerMap[( uint16_t )Action::MANUAL_SETUP_COUNTDOWN] = countDownManager;
    m_IntentManagerMap[( uint16_t )Action::MANUAL_SETUP_CANCEL] = countDownManager;
    //- (Setup AP countdown and cancel)

    //+ (Disable Network countdown and cancel)
    m_IntentManagerMap[( uint16_t )Action::TOGGLE_WIFI_RADIO_COUNTDOWN] = countDownManager;
    m_IntentManagerMap[( uint16_t )Action::TOGGLE_WIFI_RADIO_CANCEL] = countDownManager;
    //- (Disable Network countdown and cancel)

    //+ (PTS Update countdown and cancel)
    m_IntentManagerMap[( uint16_t )Action::SYSTEM_INFO_COUNTDOWN] = countDownManager;
    m_IntentManagerMap[( uint16_t )Action::SYSTEM_INFO_CANCEL] = countDownManager;
    //- (PTS Update countdown and cancel)
    //+ (BT clear pairing countdown and cancel)
    m_IntentManagerMap[( uint16_t )Action::BLUETOOTH_CLEAR_PAIRING_COUNTDOWN] = countDownManager;
    m_IntentManagerMap[( uint16_t )Action::BLUETOOTH_CLEAR_PAIRING_CANCEL] = countDownManager;
    //- (PTS Update countdown and cancel)

    //+ (Software Update countdown and cancel)
    m_IntentManagerMap[( uint16_t )Action::FORCED_SOFTWARE_UPDATE_COUNTDOWN] = countDownManager;
    m_IntentManagerMap[( uint16_t )Action::FORCED_SOFTWARE_UPDATE_CANCEL] = countDownManager;
    //- (Software Update countdown and cancel)
    //- (Common Countdown manager for key combination)
}

}

