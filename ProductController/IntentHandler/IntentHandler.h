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
#include "ProductController.h"

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
    SEND_TO_AUX = 25,
    //- AUX

    //+ ALEXA
    ALEXA_CAROUSEL = 26,
    //- ALEXA
};

class IntentHandler
{
public:
    IntentHandler( NotifyTargetTaskIF& task, CliClientMT& cliClient,
                   FrontDoorClientIF_t& fd_client,
                   ProductController& controller );
    virtual ~IntentHandler()
    {
        m_IntentManagerMap.clear();
    }

    // Initialization will include adding various IntentManagers that are
    // associated with intent Values
    void Initialize();

    // Public function to Handle intents
    bool Handle( KeyHandlerUtil::ActionType_t arg ) ;

    // Public function to register any call backs back into Product HSM
    // Intent Managers will not do any state transition, it is only expected
    // to validate,build and send messages (through frontdoor or IPC).
    void RegisterCallBack( KeyHandlerUtil::ActionType_t intent, CbPtr_t cb );

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
    const ProductController& GetProductController() const
    {
        return m_controller;
    }

    bool isIntentPlayControl( KeyHandlerUtil::ActionType_t arg )
    {
        return ( ( arg == ( uint16_t ) Action::PLAY_PAUSE ) ||
                 ( arg == ( uint16_t ) Action::NEXT_TRACK ) ||
                 ( arg == ( uint16_t ) Action::PREV_TRACK ) );
    }
    bool IsIntentBlueTooth( KeyHandlerUtil::ActionType_t arg )
    {
        return ( ( arg == ( uint16_t ) Action::CAROUSEL_DISCOVERABLE_CONNECT_TO_LAST ) ||
                 ( arg == ( uint16_t ) Action::SEND_TO_DISCOVERABLE )                  ||
                 ( arg == ( uint16_t ) Action::CLEAR_PAIRING_LIST ) );
    }
    bool IsIntentAlexa( KeyHandlerUtil::ActionType_t arg )
    {
        return ( arg == ( uint16_t ) Action::ALEXA_CAROUSEL );
    }
    bool IsIntentNetworking( KeyHandlerUtil::ActionType_t arg )
    {
        return ( ( arg == ( uint16_t ) Action::SEND_TO_AP_MODE ) ||
                 ( arg == ( uint16_t ) Action::DISABLE_NETWORKING ) );
    }
    bool IsIntentVolumeControl( KeyHandlerUtil::ActionType_t arg )
    {
        return ( ( arg == ( uint16_t ) Action::VOLUME_UP ) ||
                 ( arg == ( uint16_t ) Action::VOLUME_DOWN ) );
    }
private:
    NotifyTargetTaskIF&   m_task;
    CliClientMT&          m_cliClient;
    FrontDoorClientIF_t&  m_frontDoorClient;
    IntentManagerMap_t    m_IntentManagerMap;
    ProductController&    m_controller;
};
} // namespace ProductApp
