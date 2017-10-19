////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file      KeyActions.h
///
/// @brief     This file declares an enumeration and array of strings that are indexed according to
///            the action of the key returned from the key handler.
///
/// @date      10/18/2017
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
/// The ifdef-style of include guard here is just to play nice with the key generator script, as
/// the pycparser does not like pragmas.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef __KEYACTION_H__
#define __KEYACTION_H__

typedef enum
{
    ///
    /// This enumeration starts at 1, since the CastleKeyHandler appears to reserve action for 0.
    ///
    KEY_ACTION_POWER = 1,
    KEY_ACTION_SOURCE,
    KEY_ACTION_PRESET_1,
    KEY_ACTION_PRESET_2,
    KEY_ACTION_PRESET_3,
    KEY_ACTION_PRESET_4,
    KEY_ACTION_PRESET_5,
    KEY_ACTION_PRESET_6,
    KEY_ACTION_VOLUME_UP,
    KEY_ACTION_VOLUME_DOWN,
    KEY_ACTION_PLAY_PAUSE,
    KEY_ACTION_SKIP_FORWARD,
    KEY_ACTION_SKIP_BACK,
    KEY_ACTION_MUTE,
    KEY_ACTION_SOUNDTOUCH,
    KEY_ACTION_CONNECT,
    KEY_ACTION_ACTION,
    KEY_ACTION_TV,
    KEY_ACTION_THUMB_UP,
    KEY_ACTION_THUMB_DOWN,
    KEY_ACTION_FACTORY_DEFAULT,
    KEY_ACTION_WIFI_OFF,
    KEY_ACTION_AP_SETUP,
    KEY_ACTION_PAIR_SPEAKERS,
}
KEY_ACTION;

#endif

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
