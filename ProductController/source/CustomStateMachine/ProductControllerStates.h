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
    PROFESSOR_PRODUCT_CONTROLLER_STATE_BOOTING           =  6,
    PROFESSOR_PRODUCT_CONTROLLER_STATE_UPDATING          =  7,
    PROFESSOR_PRODUCT_CONTROLLER_STATE_NETWORK_STANDBY   =  8,
    PROFESSOR_PRODUCT_CONTROLLER_STATE_IDLE              =  9,
    PROFESSOR_PRODUCT_CONTROLLER_STATE_ON                = 10,
    PROFESSOR_PRODUCT_CONTROLLER_STATE_SOFTWARE_UPDATING = 11
};

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
