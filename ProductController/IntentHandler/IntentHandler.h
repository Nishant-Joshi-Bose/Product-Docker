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

    static bool IsIntentCountDown( KeyHandlerUtil::ActionType_t arg )
    {
        return ( ( arg == ( uint16_t ) Action::MANUAL_UPDATE_COUNTDOWN ) ||
                 ( arg == ( uint16_t ) Action::MANUAL_UPDATE_CANCEL ) ||
                 ( arg == ( uint16_t ) Action::FACTORY_DEFAULT_COUNTDOWN ) ||
                 ( arg == ( uint16_t ) Action::FACTORY_DEFAULT_CANCEL ) ||
                 ( arg == ( uint16_t ) Action::MANUAL_SETUP_COUNTDOWN ) ||
                 ( arg == ( uint16_t ) Action::MANUAL_SETUP_CANCEL ) ||
                 ( arg == ( uint16_t ) Action::TOGGLE_WIFI_RADIO_COUNTDOWN ) ||
                 ( arg == ( uint16_t ) Action::TOGGLE_WIFI_RADIO_CANCEL ) ||
                 ( arg == ( uint16_t ) Action::SYSTEM_INFO_COUNTDOWN ) ||
                 ( arg == ( uint16_t ) Action::SYSTEM_INFO_COUNTDOWN ) ||
                 ( arg == ( uint16_t ) Action::BLUETOOTH_CLEAR_PAIRING_COUNTDOWN ) ||
                 ( arg == ( uint16_t ) Action::BLUETOOTH_CLEAR_PAIRING_CANCEL ) );
    }


private:

};
} // namespace ProductApp
