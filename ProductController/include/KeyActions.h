#ifndef __KEYACTION_H__
#define __KEYACTION_H__

// the #ifdef-style of include guard here is just to play nice
// with the key generator script, as pycparser doesn't like pragmas

typedef enum
{
    // start at 1, CastleKeyHandler appears to reserve action 0
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
} KEY_ACTION;

#endif

