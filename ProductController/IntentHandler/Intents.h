////////////////////////////////////////////////////////////////////////////////
///// @file   Intents.h
///// @brief  Eddie specific IntentHandler class for Riviera based product
/////
///// @attention Copyright 2017 Bose Corporation, Framingham, MA
//////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "CommonIntents.h"

namespace ProductApp
{
enum class Action
{
    INVALID = 0,
    //+ MULTIBUTTON ACTIONS for Eddie
    PLAY_PAUSE = ActionCommon_t::PLAY_PAUSE,
    NEXT_TRACK = ActionCommon_t::NEXT_TRACK,
    PREV_TRACK = ActionCommon_t::PREV_TRACK,
    NETWORK_STANDBY = ActionCommon_t::NETWORK_STANDBY,
    SEND_TO_AP_MODE = ActionCommon_t::SEND_TO_AP_MODE,
    MANUAL_UPDATE = ActionCommon_t::MANUAL_UPDATE,
    DISABLE_NETWORKING = ActionCommon_t::DISABLE_NETWORKING,
    FACTORY_RESET = ActionCommon_t::FACTORY_RESET,
    LOW_POWER_STANDBY = ActionCommon_t::LOW_POWER_STANDBY,
    //- MULTIBUTTON ACTIONS for Eddie

    //+ Bluetooth Button
    CAROUSEL_DISCOVERABLE_CONNECT_TO_LAST = ActionCommon_t::CAROUSEL_DISCOVERABLE_CONNECT_TO_LAST,
    SEND_TO_DISCOVERABLE = ActionCommon_t::SEND_TO_DISCOVERABLE,
    CLEAR_PAIRING_LIST = ActionCommon_t::CLEAR_PAIRING_LIST,
    //- Bluetooth Button

    //+ VOLUME UP
    VOLUME_UP = ActionCommon_t::VOLUME_UP,
    //- VOLUME UP

    //+ VOLUME DOWN
    VOLUME_DOWN = ActionCommon_t::VOLUME_DOWN,
    //- VOLUME DOWN

    //+ AUX
    AUX_IN = 25,
    //- AUX

    //+ ALEXA
    ALEXA_CAROUSEL = ActionCommon_t::ALEXA_CAROUSEL,
    //- ALEXA
};

} // namespace ProductApp
