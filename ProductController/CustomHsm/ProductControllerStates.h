////////////////////////////////////////////////////////////////////////////////
/// @file   ProductControllerStates.h
/// @brief  Definition of Product Controller Hsm state IDs. Both common and
///         product specific state Ids should be defined here.
///
/// Copyright 2017 Bose Corporation
////////////////////////////////////////////////////////////////////////////////

#pragma once

// Important Note: HsmState framework reserves '0' as top state. DO NOT USE!

enum ProductControllerStates
{
    PRODUCT_CONTROLLER_STATE_BOOTING = 1,
    PRODUCT_CONTROLLER_STATE_SETUP,
    PRODUCT_CONTROLLER_STATE_NETWORK_STANDBY,
    PRODUCT_CONTROLLER_STATE_ON,
    PRODUCT_CONTROLLER_STATE_IDLE,
    PRODUCT_CONTROLLER_STATE_STANDBY_TRANSITION,

    //Start adding custom states here
    CUSTOM_PRODUCT_CONTROLLER_STATE_SETUP,
    CUSTOM_PRODUCT_CONTROLLER_STATE_NETWORK_STANDBY,
    CUSTOM_PRODUCT_CONTROLLER_STATE_IDLE
};
