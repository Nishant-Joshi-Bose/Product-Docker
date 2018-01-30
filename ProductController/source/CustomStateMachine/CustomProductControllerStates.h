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
///            Bose Corporation
///            The Mountain Road,
///            Framingham, MA 01701-9168
///            U.S.A.
///
///            This program may not be reproduced, in whole or in part, in any form by any means
///            whatsoever without the written permission of Bose Corporation.
///
////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// The following compiler directive prevents this header file from being included more than once,
/// which may cause multiple declaration compiler errors.
///
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
    /// Custom product controller states that rely on common states are defined below.
    ///
    PROFESSOR_PRODUCT_CONTROLLER_STATE_BOOTING                      = PRODUCT_CONTROLLER_STATE_BOOTING,
    PROFESSOR_PRODUCT_CONTROLLER_STATE_ON                           = PRODUCT_CONTROLLER_STATE_ON,
    PROFESSOR_PRODUCT_CONTROLLER_STATE_LOW_POWER                    = PRODUCT_CONTROLLER_STATE_LOW_POWER_STANDBY,
    PROFESSOR_PRODUCT_CONTROLLER_STATE_PLAYABLE                     = PRODUCT_CONTROLLER_STATE_PLAYABLE,
    PROFESSOR_PRODUCT_CONTROLLER_STATE_NETWORK_STANDBY              = PRODUCT_CONTROLLER_STATE_NETWORK_STANDBY,
    PROFESSOR_PRODUCT_CONTROLLER_STATE_NETWORK_STANDBY_UNCONFIGURED = PRODUCT_CONTROLLER_STATE_NETWORK_STANDBY_NOT_CONFIGURED,
    PROFESSOR_PRODUCT_CONTROLLER_STATE_NETWORK_STANDBY_CONFIGURED   = PRODUCT_CONTROLLER_STATE_NETWORK_STANDBY_CONFIGURED,
    PROFESSOR_PRODUCT_CONTROLLER_STATE_IDLE                         = PRODUCT_CONTROLLER_STATE_IDLE,
    PROFESSOR_PRODUCT_CONTROLLER_STATE_IDLE_VOICE_UNCONFIGURED      = PRODUCT_CONTROLLER_STATE_IDLE_VOICE_NOT_CONFIGURED,
    PROFESSOR_PRODUCT_CONTROLLER_STATE_IDLE_VOICE_CONFIGURED        = PRODUCT_CONTROLLER_STATE_IDLE_VOICE_CONFIGURED,
    PROFESSOR_PRODUCT_CONTROLLER_STATE_PLAYING                      = PRODUCT_CONTROLLER_STATE_PLAYING,
    PROFESSOR_PRODUCT_CONTROLLER_STATE_PLAYING_ACTIVE               = PRODUCT_CONTROLLER_STATE_PLAYING_ACTIVE,
    PROFESSOR_PRODUCT_CONTROLLER_STATE_PLAYING_INACTIVE             = PRODUCT_CONTROLLER_STATE_PLAYING_INACTIVE,
    PROFESSOR_PRODUCT_CONTROLLER_STATE_SOFTWARE_UPDATING            = PRODUCT_CONTROLLER_STATE_SOFTWARE_UPDATING,
    ///
    /// Custom product controller states that are specific to Professor are defined below.
    ///
    PROFESSOR_PRODUCT_CONTROLLER_STATE_ACCESSORY_PAIRING            = PRODUCT_CONTROLLER_STATE_LAST,
    PROFESSOR_PRODUCT_CONTROLLER_STATE_PLAYING_TRANSITION_ACCESSORY_PAIRING,
    PROFESSOR_PRODUCT_CONTROLLER_STATE_ACCESSORY_PAIRING_SELECTED,
    PROFESSOR_PRODUCT_CONTROLLER_STATE_ACCESSORY_PAIRING_DESELECTED,
    PROFESSOR_PRODUCT_CONTROLLER_STATE_ADAPTIQ,
    PROFESSOR_PRODUCT_CONTROLLER_STATE_ADAPTIQ_EXITING,
};

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
