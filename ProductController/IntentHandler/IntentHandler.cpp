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

static DPrint s_logger( "IntentHandler" );

namespace ProductApp
{
IntentHandler::IntentHandler( NotifyTargetTaskIF& task, CliClientMT& cliClient,
                              const FrontDoorClientIF_t& frontDoorClient ):
    m_task( task ),
    m_cliClient( cliClient ),
    m_frontDoorClient( frontDoorClient )
{
}

void IntentHandler::Initialize()
{
    //+ Transport Control API's
    IntentManagerPtr_t transportManager =
        std::make_shared<TransportControlManager>( m_task, m_cliClient,
                                                   m_frontDoorClient );

    m_IntentManagerMap[( uint16_t )Action::PLAY_PAUSE] = transportManager;
    m_IntentManagerMap[( uint16_t )Action::NEXT_TRACK] = transportManager;
    m_IntentManagerMap[( uint16_t )Action::PREV_TRACK] = transportManager;
    //- Transport Control API's
    //
    //+ Bluetooth Control API's

    //- Bluetooth Control API's
    //
    //+ Networking Control API's

    //- Networking Control API's
    //
    //+ Miscellaneous Control API's (LPS, Factory Reset)

    //- Miscellaneous Control API's (LPS, Factory Reset)
    //
    //+ Voice (Alexa) Control API's

    //- Voice (Alexa) Control API's
    //
    //+ Preset Control API's

    //- Preset Control API's
    //+ AUX Control API's

    //- AUX Control API's
    return;
}

bool IntentHandler::Handle( KeyHandlerUtil::ActionType_t intent )
{
    IntentManagerMap_t::iterator iter = m_IntentManagerMap.find( intent );
    if( iter != m_IntentManagerMap.end() )
    {
        iter->second->Handle( intent );
        BOSE_DEBUG( s_logger, "Found the Handle for intent :%d",
                    intent );
        return( true );
    }
    else
    {
        BOSE_ERROR( s_logger, "Handle not found for intent : %d, check "
                    "initialization code", intent );
        return false;
    }
}

void IntentHandler::RegisterCallBack( KeyHandlerUtil::ActionType_t intent,
                                      CbPtr_t cb )
{
    return;
}

}
