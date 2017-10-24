////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file      ProductControllerStates.h
///
/// @brief     This file defines the Product Controller Hierarchical State Machine, abbreviated HSM.
///            Both common and specific product state IDs are defined in this file.
///
/// @author    Stuart J. Lumby
///
/// @date      09/22/2017
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

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @note For the HSM State framework, a zero value, 0, in the enumeration is reserved for the top
///       state and should not be used.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
enum ProductControllerStates
{
    ///
    /// Common product controller states are defined below.
    ///
    PRODUCT_CONTROLLER_STATE_BOOTING                     =  1,
    PRODUCT_CONTROLLER_STATE_SETUP                       =  2,
    PRODUCT_CONTROLLER_STATE_NETWORK_STANDBY             =  3,
    PRODUCT_CONTROLLER_STATE_ON                          =  4,
    PRODUCT_CONTROLLER_STATE_IDLE                        =  5,

    ///
    /// Custom product controller states are defined below.
    ///
    PROFESSOR_PRODUCT_CONTROLLER_STATE_BOOTING                      = 11,
    PROFESSOR_PRODUCT_CONTROLLER_STATE_ON                           = 12,
    PROFESSOR_PRODUCT_CONTROLLER_STATE_OFF                          = 13,
    PROFESSOR_PRODUCT_CONTROLLER_STATE_PLAYABLE                     = 14,
    PROFESSOR_PRODUCT_CONTROLLER_STATE_NETWORK_STANDBY              = 15,
    PROFESSOR_PRODUCT_CONTROLLER_STATE_NETWORK_STANDBY_UNCONFIGURED = 16,
    PROFESSOR_PRODUCT_CONTROLLER_STATE_NETWORK_STANDBY_CONFIGURED   = 17,
    PROFESSOR_PRODUCT_CONTROLLER_STATE_IDLE                         = 18,
    PROFESSOR_PRODUCT_CONTROLLER_STATE_IDLE_VOICE_UNCONFIGURED      = 19,
    PROFESSOR_PRODUCT_CONTROLLER_STATE_IDLE_VOICE_CONFIGURED        = 20,
    PROFESSOR_PRODUCT_CONTROLLER_STATE_PLAYING                      = 21,
    PROFESSOR_PRODUCT_CONTROLLER_STATE_PLAYING_ACTIVE               = 22,
    PROFESSOR_PRODUCT_CONTROLLER_STATE_PLAYING_INACTIVE             = 23,
    PROFESSOR_PRODUCT_CONTROLLER_STATE_SOFTWARE_UPDATING            = 24
};

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
