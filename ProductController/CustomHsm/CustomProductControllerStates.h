////////////////////////////////////////////////////////////////////////////////
/// @file   CustomProductControllerStates.h
/// @brief  Definition of Product Controller Hsm state IDs. Only
///         product specific state Ids should be defined here.
///
/// Copyright 2017 Bose Corporation
////////////////////////////////////////////////////////////////////////////////

#pragma once

/// Important Note: HsmState framework reserves '0' as top state. DO NOT USE!

#include "ProductControllerStates.h"

enum CustomProductControllerStates
{
    //Start adding custom states here
    CUSTOM_PRODUCT_CONTROLLER_STATE_BOOTING = PRODUCT_CONTROLLER_STATE_BOOTING,
    CUSTOM_PRODUCT_CONTROLLER_STATE_SETUP = PRODUCT_CONTROLLER_STATE_SETUP,
    CUSTOM_PRODUCT_CONTROLLER_STATE_SOFTWARE_UPDATING,
    CUSTOM_PRODUCT_CONTROLLER_STATE_NETWORK_STANDBY = PRODUCT_CONTROLLER_STATE_NETWORK_STANDBY,
    CUSTOM_PRODUCT_CONTROLLER_STATE_LOW_POWER_STANDBY = PRODUCT_CONTROLLER_STATE_LOW_POWER_STANDBY,
    CUSTOM_PRODUCT_CONTROLLER_STATE_ON = PRODUCT_CONTROLLER_STATE_ON
};
