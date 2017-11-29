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
#include "Intents.h"

namespace ProductApp
{
using IntentManagerPtr_t  = std::shared_ptr<IntentManager>;
using IntentManagerMap_t  = std::unordered_map<uint16_t, IntentManagerPtr_t>;

class IntentHandler
{
public:
    IntentHandler( NotifyTargetTaskIF& task,
                   const CliClientMT& cliClient,
                   const FrontDoorClientIF_t& fd_client,
                   ProductController& controller );
    virtual ~IntentHandler()
    {
        m_IntentManagerMap.clear();
    }

    // Initialization will include adding various IntentManagers that are
    // associated with intent Values
    void Initialize();

    // Public function to Handle intents
    bool Handle( KeyHandlerUtil::ActionType_t& arg );

    // Public function to register any call backs back into Product HSM
    // Intent Managers will not do any state transition, it is only expected
    // to validate,build and send messages (through frontdoor or IPC).
    void RegisterCallBack( KeyHandlerUtil::ActionType_t& intent, CbPtr_t cb );

    const NotifyTargetTaskIF& GetTask() const
    {
        return m_task;
    }
    const CliClientMT& GetCli() const
    {
        return m_cliClient;
    }
    const FrontDoorClientIF_t& GetFrontDoorClient() const
    {
        return m_frontDoorClient;
    }
    ProductController& GetProductController()
    {
        return m_controller;
    }

    static bool IsIntentPlayControl( KeyHandlerUtil::ActionType_t arg )
    {
        return ( ( arg == ( uint16_t ) Action::PLAY_PAUSE ) ||
                 ( arg == ( uint16_t ) Action::NEXT_TRACK ) ||
                 ( arg == ( uint16_t ) Action::PREV_TRACK ) );
    }
    static bool IsIntentBlueTooth( KeyHandlerUtil::ActionType_t arg )
    {
        return ( ( arg == ( uint16_t ) Action::CAROUSEL_DISCOVERABLE_CONNECT_TO_LAST ) ||
                 ( arg == ( uint16_t ) Action::SEND_TO_DISCOVERABLE )                  ||
                 ( arg == ( uint16_t ) Action::CLEAR_PAIRING_LIST ) );
    }
    static bool IsIntentAlexa( KeyHandlerUtil::ActionType_t arg )
    {
        return ( arg == ( uint16_t ) Action::ALEXA_CAROUSEL );
    }
    static bool IsIntentNetworking( KeyHandlerUtil::ActionType_t arg )
    {
        return ( ( arg == ( uint16_t ) Action::SEND_TO_AP_MODE ) ||
                 ( arg == ( uint16_t ) Action::DISABLE_NETWORKING ) );
    }
    static bool IsIntentVolumeControl( KeyHandlerUtil::ActionType_t arg )
    {
        return ( ( arg == ( uint16_t ) Action::VOLUME_UP ) ||
                 ( arg == ( uint16_t ) Action::VOLUME_DOWN ) );
    }
    static bool IsIntentAuxIn( KeyHandlerUtil::ActionType_t arg )
    {
        return ( ( arg == ( uint16_t ) Action::AUX_IN ) );
    }

    static bool IsIntentNetworkStandby( KeyHandlerUtil::ActionType_t arg )
    {
        return ( ( arg == ( uint16_t ) Action::NETWORK_STANDBY ) );
    }

    static bool IsPresetSelect( KeyHandlerUtil::ActionType_t arg )
    {
        return ( ( arg == ( uint16_t ) Action::PRESET_SELECT_1 ) ||
                 ( arg == ( uint16_t ) Action::PRESET_SELECT_2 ) ||
                 ( arg == ( uint16_t ) Action::PRESET_SELECT_3 ) ||
                 ( arg == ( uint16_t ) Action::PRESET_SELECT_4 ) ||
                 ( arg == ( uint16_t ) Action::PRESET_SELECT_5 ) ||
                 ( arg == ( uint16_t ) Action::PRESET_SELECT_6 ) );
    }

    static bool IsPresetStore( KeyHandlerUtil::ActionType_t arg )
    {
        return ( ( arg == ( uint16_t ) Action::PRESET_STORE_1 ) ||
                 ( arg == ( uint16_t ) Action::PRESET_STORE_2 ) ||
                 ( arg == ( uint16_t ) Action::PRESET_STORE_3 ) ||
                 ( arg == ( uint16_t ) Action::PRESET_STORE_4 ) ||
                 ( arg == ( uint16_t ) Action::PRESET_STORE_5 ) ||
                 ( arg == ( uint16_t ) Action::PRESET_STORE_6 ) );
    }

private:

    void NotifyButtonEvent( KeyHandlerUtil::ActionType_t intent );

private:
    NotifyTargetTaskIF&         m_task;
    const CliClientMT&          m_cliClient;
    const FrontDoorClientIF_t&  m_frontDoorClient;
    IntentManagerMap_t          m_IntentManagerMap;
    ProductController&          m_controller;
    std::unordered_map <uint16_t, std::string> m_IntentNotificationMap;
};
} // namespace ProductApp
