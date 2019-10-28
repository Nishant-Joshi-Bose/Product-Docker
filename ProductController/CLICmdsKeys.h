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
    AUTOWAKE = static_cast<int>( CommonCLICmdKeys::COMMON_CLICMD_KEYS_LAST ) + 1,
    INTENT,
    MUTE,
    SOURCE,
    BOOT_STATUS,
    TEST_FREQ,
    TEST_PAIRING,
    TEST_POWER,
    TEST_VOICE,
    TEST_VOLUME,
    TEST_ACCESSORY_UPDATE,
    TEST_TRANSITION
};