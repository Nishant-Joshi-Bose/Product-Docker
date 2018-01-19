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

    static bool IsIntentManualUpdateControl( KeyHandlerUtil::ActionType_t arg )
    {
        return ( ( arg == ( uint16_t ) Action::MANUAL_UPDATE_COUNTDOWN ) ||
                 ( arg == ( uint16_t ) Action::MANUAL_UPDATE_CANCEL ) );
    }

    static bool IsIntentFactoryResetControl( KeyHandlerUtil::ActionType_t arg )
    {
        return ( ( arg == ( uint16_t ) Action::FACTORY_RESET_COUNTDOWN ) ||
                 ( arg == ( uint16_t ) Action::FACTORY_RESET_CANCEL ) );
    }

    static bool IsIntentSetupCountdownControl( KeyHandlerUtil::ActionType_t arg )
    {
        return ( ( arg == ( uint16_t ) Action::SETUP_AP_COUNTDOWN ) ||
                 ( arg == ( uint16_t ) Action::SETUP_AP_CANCEL ) );
    }

    static bool IsIntentDisableNetworkCountdownControl( KeyHandlerUtil::ActionType_t arg )
    {
        return ( ( arg == ( uint16_t ) Action::DISABLE_NETWORK_COUNTDOWN ) ||
                 ( arg == ( uint16_t ) Action::DISABLE_NETWORK_CANCEL ) );
    }

    static bool IsIntentPTSUpdateControl( KeyHandlerUtil::ActionType_t arg )
    {
        return ( ( arg == ( uint16_t ) Action::PTS_UPDATE_COUNTDOWN ) ||
                 ( arg == ( uint16_t ) Action::PTS_UPDATE_CANCEL ) );
    }

private:

};
} // namespace ProductApp
