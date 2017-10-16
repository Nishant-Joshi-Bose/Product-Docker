////////////////////////////////////////////////////////////////////////////////
///// @file   IntendHandler.h
///// @brief  Eddie specific IntendHandler class for Riviera based product
/////
///// @attention Copyright 2017 Bose Corporation, Framingham, MA
//////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "NotifyTargetTaskIF.h"
#include "CliClientMT.h"
#include "FrontDoorClientIF.h"
#include "KeyHandler.h"
#include "IntendManager.h"

namespace ProductApp
{
using IntendManagerPtr_t  = std::shared_ptr<IntendManager>;
using IntendManagerMap_t  = std::unordered_map<uint16_t, IntendManagerPtr_t>;

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

class IntendHandler
{
public:
    IntendHandler( NotifyTargetTaskIF& task, CliClientMT& cliClient,
                   const FrontDoorClientIF_t& fd_client );
    virtual ~IntendHandler() { }

    // Initialization will include adding various IntendManagers that are
    // associated with intend Values
    void Initialize();

    // Public function to Handle intends
    bool Handle( KeyHandlerUtil::ActionType_t arg );

    // Public function to register any call backs back into Product HSM
    // Intend Managers will not do any state transistion, it is only expected
    // to valid,build and send messages (through frontdoor or IPC).
    void RegisterCallBack( KeyHandlerUtil::ActionType_t intend, CbPtr_t cb );

    const NotifyTargetTaskIF& GetTask() const
    {
        return m_task;
    }
    const CliClientMT& GetCli() const
    {
        return m_cliClient;
    }

    const FrontDoorClientIF_t& GetFrontDoor() const
    {
        return m_frontDoorClient;
    }
private:
    NotifyTargetTaskIF&      m_task;
    CliClientMT&             m_cliClient;
    FrontDoorClientIF_t      m_frontDoorClient;
    IntendManagerMap_t       m_IntendManagerMap;
};
} // namespace ProductApp
