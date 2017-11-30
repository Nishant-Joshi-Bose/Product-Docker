////////////////////////////////////////////////////////////////////////////////
///// @file   IntentHandler.h
///// @brief  Eddie specific IntentHandler class for Riviera based product
/////
///// @attention Copyright 2017 Bose Corporation, Framingham, MA
//////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "NotifyTargetTaskIF.h"
#include "CliClientMT.h"
#include "FrontDoorClientIF.h"
#include "KeyHandler.h"
#include "IntentManager.h"

namespace ProductApp
{
using IntentManagerPtr_t  = std::shared_ptr<IntentManager>;
using IntentManagerMap_t  = std::unordered_map<uint16_t, IntentManagerPtr_t>;

enum class Action
{
    INVALID = 0,
    //+ MULTIBUTTON ACTIONS for Eddie
    PLAY_PAUSE = 1,
    NEXT_TRACK = 2,
    PREV_TRACK = 3,
    NETWORK_STANDBY = 4,
    SEND_TO_AP_MODE = 5,
    MANUAL_UPDATE = 6,
    DISABLE_NETWORKING = 7,
    FACTORY_RESET = 8,
    LOW_POWER_STANDBY = 9,
    //- MULTIBUTTON ACTIONS for Eddie

    //+ Bluetooth Button
    CAROUSEL_DISCOVERABLE_CONNECT_TO_LAST = 20,
    SEND_TO_DISCOVERABLE = 21,
    CLEAR_PAIRING_LIST = 22,
    //- Bluetooth Button

    //+ VOLUME UP
    VOLUME_UP = 23,
    //- VOLUME UP

    //+ VOLUME DOWN
    VOLUME_DOWN = 24,
    //- VOLUME DOWN

    //+ AUX
    AUX_IN = 25,
    //- AUX

    //+ VOICE - ALEXA
    VOICE_CAROUSEL = 26,
    //- VOICE - ALEXA
};

class IntentHandler
{
public:
    IntentHandler( NotifyTargetTaskIF& task,
                   const CliClientMT& cliClient,
                   const FrontDoorClientIF_t& fd_client,
                   EddieProductController& controller );
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
    EddieProductController& GetProductController()
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
    static bool IsIntentVoice( KeyHandlerUtil::ActionType_t arg )
    {
        return ( arg == ( uint16_t ) Action::VOICE_CAROUSEL );
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

private:

    void NotifyButtonEvent( KeyHandlerUtil::ActionType_t intent );

private:
    NotifyTargetTaskIF&         m_task;
    const CliClientMT&          m_cliClient;
    const FrontDoorClientIF_t&  m_frontDoorClient;
    IntentManagerMap_t          m_IntentManagerMap;
    EddieProductController&     m_controller;
    std::unordered_map <uint16_t, std::string> m_IntentNotificationMap;
};
} // namespace ProductApp
