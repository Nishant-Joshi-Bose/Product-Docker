///////////////////////////////////////////////////////////////////////////////
/// @file IntendHandler.cpp
///
/// @brief Implementation of Intend Handler
//  This module offloads the work of the Product Controller validate and build
//  LAN API or IPC messages to perform actions of various Intends
//  It also lets the Product Controller get control back in an Async way, so
//  the HSM can perform state changes if it needs to.
//  This Handler should not perform state transistions
//  The Handler inturn delegates its work to customized Intend Managers based
//  on the ActionType that is passed to it.
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
#include "IntendHandler.h"
#include "TransportControlManager.h"

static DPrint s_logger( "IntendHandler" );

namespace ProductApp
{
IntendHandler::IntendHandler( NotifyTargetTaskIF& task, CliClientMT& cliClient,
                              const FrontDoorClientIF_t& frontDoorClient ):
    m_task( task ),
    m_cliClient( cliClient ),
    m_frontDoorClient( frontDoorClient )
{
    m_IntendManagerMap.clear();
}

void IntendHandler::Initialize()
{
    //+ Transport Control API's
    IntendManagerPtr_t transportManager =
        std::make_shared<TransportControlManager>( m_task, m_cliClient,
                                                   m_frontDoorClient );

    m_IntendManagerMap[( uint16_t )Action::PLAY_PAUSE] = transportManager;
    m_IntendManagerMap[( uint16_t )Action::NEXT_TRACK] = transportManager;
    m_IntendManagerMap[( uint16_t )Action::PREV_TRACK] = transportManager;
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

bool IntendHandler::Handle( KeyHandlerUtil::ActionType_t intend )
{
    IntendManagerMap_t::iterator iter = m_IntendManagerMap.find( intend );
    if( iter != m_IntendManagerMap.end() )
    {
        iter->second->Handle( intend );
        BOSE_DEBUG( s_logger, "Found the Handle for intend :%d",
                    intend );
        return( true );
    }
    else
    {
        BOSE_ERROR( s_logger, "Handle not found for intend : %d, check "
                    "initialization code", intend );
        return false;
    }
}

void IntendHandler::RegisterCallBack( KeyHandlerUtil::ActionType_t intend,
                                      CbPtr_t cb )
{
    return;
}

}
