///////////////////////////////////////////////////////////////////////////////
/// @file RepeatManager.h
///
/// @brief Implementation of RepeatManager
///
/// @attention
///    BOSE CORPORATION.
///    COPYRIGHT 2017 BOSE CORPORATION ALL RIGHTS RESERVED.
///    This program may not be reproduced, in whole or in part in any
///    form or any means whatsoever without the written permission of:
///        BOSE CORPORATION
///        The Mountain
///        Framingham, MA 01701-9168
///
///////////////////////////////////////////////////////////////////////////////
#pragma once

#include "APTimer.h"
#include "RepeatManager.pb.h"

#define MAXIMUM_KEYS        10

enum RAW_KEY_VALUE {
    KEY_VAL_PLAY = 0,
    KEY_VAL_PAUSE = 1,
    KEY_VAL_STOP = 2,
    KEY_VAL_PREV_TRACK = 3,
    KEY_VAL_NEXT_TRACK = 4,
    KEY_VAL_THUMBS_UP = 5,
    KEY_VAL_THUMBS_DOWN = 6,
    KEY_VAL_BOOKMARK = 7,
    KEY_VAL_POWER = 8,
    KEY_VAL_MUTE = 9,
    KEY_VAL_VOLUME_UP = 10,
    KEY_VAL_VOLUME_DOWN = 11,
    KEY_VAL_PRESET_1 = 12,
    KEY_VAL_PRESET_2 = 13,
    KEY_VAL_PRESET_3 = 14,
    KEY_VAL_PRESET_4 = 15,
    KEY_VAL_PRESET_5 = 16,
    KEY_VAL_PRESET_6 = 17,
    KEY_VAL_AUX_INPUT = 18,
    KEY_VAL_SHUFFLE_OFF = 19,
    KEY_VAL_SHUFFLE_ON = 20,
    KEY_VAL_REPEAT_OFF = 21,
    KEY_VAL_REPEAT_ONE = 22,
    KEY_VAL_REPEAT_ALL = 23,
    KEY_VAL_PLAY_PAUSE = 24,
    KEY_VAL_ADD_FAVORITE = 25,
    KEY_VAL_REMOVE_FAVORITE = 26,
    KEY_VAL_BLUETOOTH = 27,
    INVALID_KEY_VAL = 28
};

enum {
    KEY_STATE_INVALID = -1,
    KEY_STATE_PRESSED = 1,
    KEY_STATE_RELEASED = 2,
    KEY_STATE_MOVED = 3,
};

static std::map<std::string, int> keyMap = {
                                        {"play",            KEY_VAL_PLAY},
                                        {"pause",           KEY_VAL_PAUSE},
                                        {"stop",            KEY_VAL_STOP},
                                        {"prev_track",      KEY_VAL_PREV_TRACK},
                                        {"next_track",      KEY_VAL_NEXT_TRACK},
                                        {"thumbs_up",       KEY_VAL_THUMBS_UP},
                                        {"thumbs_down",     KEY_VAL_THUMBS_DOWN},
                                        {"bookmark",        KEY_VAL_BOOKMARK},
                                        {"power",           KEY_VAL_POWER},
                                        {"mute",            KEY_VAL_MUTE},
                                        {"volume_up",       KEY_VAL_VOLUME_UP},
                                        {"volume_down",     KEY_VAL_VOLUME_DOWN},
                                        {"preset_1",        KEY_VAL_PRESET_1},
                                        {"preset_2",        KEY_VAL_PRESET_2},
                                        {"preset_3",        KEY_VAL_PRESET_3},
                                        {"preset_4",        KEY_VAL_PRESET_4},
                                        {"preset_5",        KEY_VAL_PRESET_5},
                                        {"preset_6",        KEY_VAL_PRESET_6},
                                        {"aux_input",       KEY_VAL_AUX_INPUT},
                                        {"shuffel_off",     KEY_VAL_SHUFFLE_OFF},
                                        {"shuffel_on",      KEY_VAL_SHUFFLE_ON},
                                        {"repeat_off",      KEY_VAL_REPEAT_OFF},
                                        {"repeat_one",      KEY_VAL_REPEAT_ONE},
                                        {"repeat_all",      KEY_VAL_REPEAT_ALL},
                                        {"play_pause",      KEY_VAL_PLAY_PAUSE},
                                        {"add_favorite",    KEY_VAL_ADD_FAVORITE},
                                        {"remove_favorite", KEY_VAL_REMOVE_FAVORITE},
                                        {"bluetooth",       KEY_VAL_BLUETOOTH},
                                        {"invalid",         INVALID_KEY_VAL},
                                        } ;

class KeyRepeatManager
{
public:
    KeyRepeatManager( NotifyTargetTaskIF* task );
    ~KeyRepeatManager();

    typedef void RepeatManagerResultsCb( const int result, void *context);
    void HandleKeys( const RepeatManager::Repeat & keyEvent );
    void SetRepeatManagerResultsCb( RepeatManagerResultsCb *cb, void *ctx);

private:
    typedef struct
    {
        int                 Keys[MAXIMUM_KEYS];
        int                 KeyTimeout;
        int                 EventAction;
        bool                ActionOnTimeout;
        bool                Repeat;
    } _KeyTableEntry;

    KeyRepeatManager( const KeyRepeatManager& ) = delete;
    const KeyRepeatManager& operator= ( const KeyRepeatManager& ) = delete;

    void DeInitializeVariables();
    int GetKeyNumberForPosition( int xPos );
    void CallBackFunction( int keyState, int keyNumber );
    int GetKeyTableIndexNumber( int keyNo, int index, int keyComboCounter );

private:
    int m_keyTrack[MAXIMUM_KEYS];
    int m_keyTableSize;
    int m_keyTableIndex;
    int m_keyComboCounter;
    int m_previousTableIndex;

    void *m_keyCallBackbContext;
    RepeatManagerResultsCb *m_keyResultCallBack;
    APTimerPtr m_keyTimer;
    _KeyTableEntry *m_keyTableEntry;
};


