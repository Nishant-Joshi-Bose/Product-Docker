////////////////////////////////////////////////////////////////////////////////
/// @file   ProductAppStates.h
/// @brief  Definition of Product Application Hsm state IDs
///
/// Copyright 2017 Bose Corporation
////////////////////////////////////////////////////////////////////////////////

#pragma once
#include <map>
// Important Note: HsmState framework reserves '0' as top state. DO NOT USE!

enum ProductAppStates
{
    PRODUCT_APP_STATE_BOOTING = 1,
    PRODUCT_APP_STATE_STDOP,
    PRODUCT_APP_STATE_SETUP,
    PRODUCT_APP_STATE_STANDBY,
    PRODUCT_APP_STATE_ON,
    PRODUCT_APP_STATE_SW_UPDATING,
    PRODUCT_APP_STATE_IDLE,
    PRODUCT_APP_STATE_LOW_POWER,
    PRODUCT_APP_CRITICAL_ERROR
};

