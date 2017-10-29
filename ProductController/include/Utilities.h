////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file      Utilities.h
///
/// @brief     This file declares general constants or utilities for use by the product controller
///            code.
///
/// @date      10/24/2017
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
///            Included Header Files
///
////////////////////////////////////////////////////////////////////////////////////////////////////
#include "DPrint.h"
#include <string>

////////////////////////////////////////////////////////////////////////////////////////////////////
///                              Product Application Namespace                                   ///
////////////////////////////////////////////////////////////////////////////////////////////////////
namespace ProductApp
{
////////////////////////////////////////////////////////////////////////////////////////////////
///
/// The following definition determines the compile time log level. Any log messages that are
/// above the given level are compiled out of the executable and cannot be enabled at
/// run-time. Currently, the level is set to the highest level to output all log statements,
/// but it can be changed to a lower level to leave log statements above this level out of
/// the compiled application to reduce its overall size.
///
////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef  BOSE_LOG_LEVEL
#undef  BOSE_LOG_LEVEL
#define BOSE_LOG_LEVEL ( DPrint::INSANE )
#endif

////////////////////////////////////////////////////////////////////////////////////////////////
///
/// The following declares a DPrint class type object for logging information in the product
/// controller source code.
///
////////////////////////////////////////////////////////////////////////////////////////////////
static DPrint s_logger { "Product" };


////////////////////////////////////////////////////////////////////////////////////////////////
///
/// The following constants define known frontdoor directory nodes
///
////////////////////////////////////////////////////////////////////////////////////////////////

// system nodes
constexpr char FRONTDOOR_SYSTEM_LANGUAGE[]                      = "/system/language";
constexpr char FRONTDOOR_SYSTEM_CONFIGURATION_STATUS[]          = "/system/configuration/status";
constexpr char FRONTDOOR_SYSTEM_CAPS_INITIALIZATION_STATUS[]    = "/system/capsInitializationStatus";

// network nodes
constexpr char FRONTDOOR_NETWORK_STATUS[]                       = "/network/status";
constexpr char FRONTDOOR_NETWORK_WIFI_PROFILE[]                 = "/network/wifi/profile";
constexpr char FRONTDOOR_NETWORK_WIFI_STATUS[]                  = "/network/wifi/status";

// CAPS nodes
constexpr char FRONTDOOR_CAPS_INITIALIZATION_UPDATE[]           = "/CapsInitializationUpdate";

// audio nodes
constexpr char FRONTDOOR_AUDIO_VOLUME[]                         = "/audio/volume";

}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
