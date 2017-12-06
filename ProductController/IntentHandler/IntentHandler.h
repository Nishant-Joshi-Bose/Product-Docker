////////////////////////////////////////////////////////////////////////////////
///// @file   IntentHandler.h
///// @brief  IntentHandler class for Riviera based product
/////
///// @attention Copyright 2017 Bose Corporation, Framingham, MA
//////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "NotifyTargetTaskIF.h"
#include "CliClientMT.h"
#include "FrontDoorClientIF.h"
#include "KeyHandler.h"
#include "IntentManager.h"
#include "CommonIntentHandler.h"
#include "Intents.h"

namespace ProductApp
{

class IntentHandler: public CommonIntentHandler
{
public:
    IntentHandler( NotifyTargetTaskIF& task,
                   const CliClientMT& cliClient,
                   const FrontDoorClientIF_t& fd_client,
                   ProductController& controller );
    ~IntentHandler() override
    {
    }

    // Initialization will include adding Product-specific IntentManagers that are
    // associated with intent Values
    void Initialize() override;

    static bool IsIntentAuxIn( KeyHandlerUtil::ActionType_t arg )
    {
        return ( ( arg == ( uint16_t ) Action::AUX_IN ) );
    }

    static bool IsIntentVoice( KeyHandlerUtil::ActionType_t arg )
    {
        return ( arg == ( uint16_t ) Action::VOICE_CAROUSEL );
    }
    static bool IsIntentVoice( KeyHandlerUtil::ActionType_t arg )
    {
        return ( arg == ( uint16_t ) Action::VOICE_CAROUSEL );
    }

private:

};
} // namespace ProductApp
