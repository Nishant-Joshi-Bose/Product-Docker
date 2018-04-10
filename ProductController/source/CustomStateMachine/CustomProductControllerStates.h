////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file      CustomProductControllerStates.h
///
/// @brief     This file defines the Product Controller Hierarchical State Machine, abbreviated HSM.
///            Both common and specific product state IDs are defined in this file.
///
/// @author    Stuart J. Lumby
///
/// @attention Copyright (C) 2017 Bose Corporation All Rights Reserved
///
////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////
///            Included Header Files
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "ProductControllerStates.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @note For the HSM State framework, a zero value, 0, in the enumeration is reserved for the top
///       state and should not be used.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
enum CustomProductControllerStates
{
    ///
    /// Custom product controller states that rely on common states are defined below. Note that
    /// these custom states need to have the same IDs as their base common states; this ensures that
    /// state changes in the common code that need to transition to a derived custom state will do so
    /// correctly.
    ///
    CUSTOM_PRODUCT_CONTROLLER_STATE_LOW_POWER_STANDBY      = PRODUCT_CONTROLLER_STATE_LOW_POWER_STANDBY,
    CUSTOM_PRODUCT_CONTROLLER_STATE_ON                     = PRODUCT_CONTROLLER_STATE_ON,
    CUSTOM_PRODUCT_CONTROLLER_STATE_IDLE                   = PRODUCT_CONTROLLER_STATE_IDLE,
    CUSTOM_PRODUCT_CONTROLLER_STATE_PLAYABLE               = PRODUCT_CONTROLLER_STATE_PLAYABLE,
    CUSTOM_PRODUCT_CONTROLLER_STATE_PLAYING                = PRODUCT_CONTROLLER_STATE_PLAYING,
    CUSTOM_PRODUCT_CONTROLLER_STATE_PLAYING_SELECTED       = PRODUCT_CONTROLLER_STATE_PLAYING_SELECTED,
    CUSTOM_PRODUCT_CONTROLLER_STATE_PLAYING_DESELECTED     = PRODUCT_CONTROLLER_STATE_PLAYING_DESELECTED,
    CUSTOM_PRODUCT_CONTROLLER_STATE_PLAYING_SELECTED_SETUP = PRODUCT_CONTROLLER_STATE_PLAYING_SELECTED_SETUP,
    ///
    /// Custom product controller states that are specific to Professor are defined below.
    ///
    CUSTOM_PRODUCT_CONTROLLER_STATE_PLAYING_TRANSITION_ACCESSORY_PAIRING = PRODUCT_CONTROLLER_STATE_LAST,
    CUSTOM_PRODUCT_CONTROLLER_STATE_PLAYING_SELECTED_ACCESSORY_PAIRING,
    CUSTOM_PRODUCT_CONTROLLER_STATE_PLAYING_DESELECTED_ACCESSORY_PAIRING,
    CUSTOM_PRODUCT_CONTROLLER_STATE_ADAPTIQ,
    CUSTOM_PRODUCT_CONTROLLER_STATE_ADAPTIQ_EXITING
};

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
