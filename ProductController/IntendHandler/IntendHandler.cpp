///////////////////////////////////////////////////////////////////////////////
/// @file IntendHandler.cpp
///
/// @brief Implementation of Intend Handler
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

static DPrint s_logger( "IntendHandler" );

namespace ProductApp
{
IntendHandler::IntendHandler(NotifyTargetTaskIF& task, CliClientMT& cliClient,
               const FrontDoorClientIF_t& frontDoorClient):
               m_task(task),
               m_cliClient(cliClient),
               m_frontDoorClient(frontDoorClient)
{
    m_IntendManagerMap.clear();
}

void IntendHandler::Initialize()
{
    return;
}

bool IntendHandler::Handle(KeyHandlerUtil::ActionType_t intend)
{
    return false;
}

bool IntendHandler::RegisterCallBack(KeyHandlerUtil::ActionType_t intend,
                                     CbPtr_t cb)
{
    return false;
}

}
