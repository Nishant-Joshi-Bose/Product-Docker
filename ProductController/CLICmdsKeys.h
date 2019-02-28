////////////////////////////////////////////////////////////////////////////////
/// @file   CLICmdsKeys.h
/// @brief  enum Class to define CLI command keys for the product controller.
///
/// @attention Copyright 2017 Bose Corporation, Framingham, MA
////////////////////////////////////////////////////////////////////////////////

#pragma once
#include <CommonCLICmdsKeys.h>

enum class CLICmdsKeys
{
    /// Always start CLI keys from COMMON_CLICMD_KEYS_LAST + 1
    BOOT_STATUS = static_cast<int>( CommonCLICmdKeys::COMMON_CLICMD_KEYS_LAST ) + 1,
    MFGDATA,
    BACKLIGHT,
    LIGHTSENSOR,
    AMP,
    LCD,
    BATTERY
};
