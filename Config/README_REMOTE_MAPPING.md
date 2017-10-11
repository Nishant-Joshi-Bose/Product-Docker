Professor <-> BOSE_ remote mapping
==================================

The Riviera LPM performs mapping from the Professor simple remote (which sends "compressed" IR codes) to the BOSE_* constants contained in RivieraLPM_KeyValues.h (see Core/Drivers/Bose/Internal/IR/RiveraLPM_CompressedKeys.[ch] in the LPM code).  The mapping is as follows

Professor Remote            BOSE_* Key
----------------            ----------
POWER                       BOSE_ON_OFF
SOURCE                      BOSE_BLUETOOTH_SOURCE
PRESET1                     BOSE_NUMBER_1
PRESET2                     BOSE_NUMBER_2
PRESET3                     BOSE_NUMBER_3
PRESET4                     BOSE_NUMBER_4 
PRESET5                     BOSE_NUMBER_5
PRESET6                     BOSE_NUMBER_6
VOLUME_UP                   BOSE_VOLUME_UP
VOLUME_DOWN                 BOSE_VOLUME_DOWN
PLAY_PAUSE                  BOSE_PLAY
SKIP_FORWARD                BOSE_QUICK_SKIP 
SKIP_BACK                   BOSE_QUICK_REPLAY
MUTE                        BOSE_MUTE
SOUNDTOUCH                  BOSE_SOUND_TOUCH_SOURCE
CONNECT                     KEY_VAL_AVAIL_11
ACTION                      KEY_VAL_AVAIL_12
TV                          BOSE_TV_SOURCE
THUMB_DOWN                  BOSE_PAGE_THUMBS_DOWN
THUMB_UP                    BOSE_PAGE_THUMBS_UP

