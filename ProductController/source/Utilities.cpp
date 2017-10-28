////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file      Utilities.cpp
///
/// @brief     This file declares general constants or utilities for use by the product controller
///            code.
///
/// @date      10/28/2017
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

#include "Utilities.h"

namespace ProductApp
{

// system nodes
const std::string FRONTDOOR_SYSTEM_LANGUAGE( "/system/language" );
const std::string FRONTDOOR_SYSTEM_CONFIGURATION_STATUS( "/system/configuration/status" );
const std::string FRONTDOOR_SYSTEM_CAPS_INITIALIZATION_STATUS( "/system/capsInitializationStatus" );

// network nodes
const std::string FRONTDOOR_NETWORK_STATUS( "/network/status" );
const std::string FRONTDOOR_NETWORK_WIFI_PROFILE( "/network/wifi/profile" );
const std::string FRONTDOOR_NETWORK_WIFI_STATUS( "/network/wifi/status" );

// CAPS nodes
const std::string FRONTDOOR_CAPS_INITIALIZATION_UPDATE( "/CapsInitializationUpdate" );

// audio nodes
const std::string FRONTDOOR_AUDIO_VOLUME( "/audio/volume" );

}

