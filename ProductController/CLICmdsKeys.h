////////////////////////////////////////////////////////////////////////////////
/// @file   CLICmdsKeys.h
/// @brief  enum Class to define CLI command keys for Eddie.
///
/// @attention Copyright 2017 Bose Corporation, Framingham, MA
////////////////////////////////////////////////////////////////////////////////

#pragma once
#include <CommonCLICmdsKeys.h>

enum class CLICmdKeys
{
    /// Always start CLI keys from COMMON_CLICMD_KEYS_LAST + 1
    SET_DISPLAY_AUTO_MODE = CommonCLICmdKeys::COMMON_CLICMD_KEYS_LAST + 1,
    GET_PRODUCT_STATE,
    GET_BOOT_STATUS
};
