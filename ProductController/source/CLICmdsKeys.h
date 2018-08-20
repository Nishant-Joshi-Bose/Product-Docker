////////////////////////////////////////////////////////////////////////////////
/// @file   CLICmdsKeys.h
/// @brief  enum Class to define CLI command keys for the product controller.
///
/// @attention Copyright 2018 Bose Corporation, Framingham, MA
////////////////////////////////////////////////////////////////////////////////

#pragma once
#include <CommonCLICmdsKeys.h>

enum class CLICmdsKeys
{
    /// Always start CLI keys from COMMON_CLICMD_KEYS_LAST + 1
    AUTOWAKE,
    KEY,
    MUTE,
    SOURCE,
    TEST_AP,
    TEST_BOOTUP,
    BOOT_STATUS,
    TEST_CAPS,
    TEST_FREQ,
    TEST_LPM,
    TEST_NOWPLAYING,
    TEST_PAIRING,
    TEST_POWER,
    TEST_STS,
    TEST_VOICE,
    TEST_VOLUME,
    TEST_ACCESSORY_UPDATE
};