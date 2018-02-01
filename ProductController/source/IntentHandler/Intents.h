////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file      Intents.h
///
/// @brief     This header file contains custom actions, grouped into intents, for a Professor
///            IntentHandler class.
///
/// @attention Copyright (C) 2017 Bose Corporation All Rights Reserved
///
///            Bose Corporation
///            The Mountain Road,
///            Framingham, MA 01701-9168
///            U.S.A.
///
///            This program may not be reproduced, in whole or in part, in any form by any means
///            whatsoever without the written permission of Bose Corporation.
///
////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// The following compiler directive prevents this header file from being included more than once,
/// which may cause multiple declaration compiler errors.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////
///
///            Included Header Files
///
////////////////////////////////////////////////////////////////////////////////////////////////////
#include "CommonIntents.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
///                          Start of the Product Application Namespace                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
namespace ProductApp
{

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief The Action Enumeration Class
///
////////////////////////////////////////////////////////////////////////////////////////////////////
enum class Action
{
    ////////////////////////////////////////////////////////////////////////////////////////////////
    /// The value of 0 is left undefined as a key action.
    ////////////////////////////////////////////////////////////////////////////////////////////////
    INVALID = 0,

    ////////////////////////////////////////////////////////////////////////////////////////////////
    /// Common Defined Actions Valued Under 100
    ////////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// Multi-Button Defined Intents
    ///
    PLAY_PAUSE         = ActionCommon_t::PLAY_PAUSE,
    NEXT_TRACK         = ActionCommon_t::NEXT_TRACK,
    PREV_TRACK         = ActionCommon_t::PREV_TRACK,
    NETWORK_STANDBY    = ActionCommon_t::NETWORK_STANDBY,
    SEND_TO_AP_MODE    = ActionCommon_t::SEND_TO_AP_MODE,
    MANUAL_UPDATE      = ActionCommon_t::MANUAL_UPDATE,
    TOGGLE_WIFI_RADIO  = ActionCommon_t::TOGGLE_WIFI_RADIO,
    FACTORY_DEFAULT    = ActionCommon_t::FACTORY_DEFAULT,
    LOW_POWER_STANDBY  = ActionCommon_t::LOW_POWER_STANDBY,

    ///
    /// Bluetooth Intents
    ///
    CAROUSEL_DISCOVERABLE_CONNECT_TO_LAST = ActionCommon_t::CAROUSEL_DISCOVERABLE_CONNECT_TO_LAST,
    SEND_TO_DISCOVERABLE                  = ActionCommon_t::SEND_TO_DISCOVERABLE,
    CLEAR_PAIRING_LIST                    = ActionCommon_t::CLEAR_PAIRING_LIST,

    ///
    /// Volume Action Intents
    ///
    VOLUME_UP   = ActionCommon_t::VOLUME_UP,
    VOLUME_DOWN = ActionCommon_t::VOLUME_DOWN,

    ///
    /// Voice Action Intents
    ///
    VOICE_CAROUSEL = ActionCommon_t::VOICE_CAROUSEL,

    ///
    /// Preset Storage Intents
    ///
    PRESET_STORE_1  = ActionCommon_t::PRESET_STORE_1,
    PRESET_STORE_2  = ActionCommon_t::PRESET_STORE_2,
    PRESET_STORE_3  = ActionCommon_t::PRESET_STORE_3,
    PRESET_STORE_4  = ActionCommon_t::PRESET_STORE_4,
    PRESET_STORE_5  = ActionCommon_t::PRESET_STORE_5,
    PRESET_STORE_6  = ActionCommon_t::PRESET_STORE_6,

    ///
    /// Preset Selection Intents
    ///
    PRESET_SELECT_1 = ActionCommon_t::PRESET_SELECT_1,
    PRESET_SELECT_2 = ActionCommon_t::PRESET_SELECT_2,
    PRESET_SELECT_3 = ActionCommon_t::PRESET_SELECT_3,
    PRESET_SELECT_4 = ActionCommon_t::PRESET_SELECT_4,
    PRESET_SELECT_5 = ActionCommon_t::PRESET_SELECT_5,
    PRESET_SELECT_6 = ActionCommon_t::PRESET_SELECT_6,

    ///
    /// Volume Ramping Intents
    ///
    VOLUME_RAMP_UP          = ActionCommon_t::VOLUME_RAMP_UP,
    VOLUME_RAMP_DOWN        = ActionCommon_t::VOLUME_RAMP_DOWN,
    VOLUME_RAMP_UP_CANCEL   = ActionCommon_t::VOLUME_RAMP_UP_CANCEL,
    VOLUME_RAMP_DOWN_CANCEL = ActionCommon_t::VOLUME_RAMP_DOWN_CANCEL,

    ///
    /// Last Common Action
    ///
    ACTION_COMMON_LAST = 100,

    ////////////////////////////////////////////////////////////////////////////////////////////////
    /// Custom Professor Defined Actions Valued 101 and Higher
    ////////////////////////////////////////////////////////////////////////////////////////////////
    ACTION_POWER              = 101,
    ACTION_VOLUME_UP_1        = 109,
    ACTION_VOLUME_DOWN_1      = 110,
    ACTION_MUTE               = 114,
    ACTION_SOUNDTOUCH         = 115,
    ACTION_CONNECT            = 116,
    // unused                 = 117,
    ACTION_TV                 = 118,
    ACTION_THUMB_UP           = 119,
    ACTION_THUMB_DOWN         = 120,
    ACTION_PAIR_SPEAKERS      = 124,
    ACTION_STOP_PAIR_SPEAKERS = 200
};

////////////////////////////////////////////////////////////////////////////////////////////////////
///                           End of the Product Application Namespace                           ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
