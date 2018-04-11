////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file      ProductEndpointDefines.h
///
/// @brief     This header file declares Front Door End Point definitions used by the custom product
///            controller for Professor.
///
/// @attention Copyright (C) 2018 Bose Corporation All Rights Reserved
///
////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////
///
///            Header Parameters
///
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "EndPointsDefines.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
///
///           Front Door Custom End Point Definitions Used by Professor
///
////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// The following definitions are alphabetically listed end point resource strings, and are used by
/// the custom product controller (specific to Professor) as a client or consumer to receive data
/// from other processes or to send data to other processes to drive product functionality.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
#define FRONTDOOR_SYSTEM_SOURCES_API "/system/sources"

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// The following definitions are alphabetically listed end point resource strings, and are used by
/// the custom product controller (specific to Professor) as a server or producer to send data
/// requested by other processes or to receive data from other processes to drive functionality.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
#define FRONTDOOR_ACCESSORIES_API                       "/accessories"
#define FRONTDOOR_ADAPTIQ_API                           "/adaptiq"
#define FRONTDOOR_AUDIO_AVSYNC_API                      "/audio/avSync"
#define FRONTDOOR_AUDIO_BASS_API                        "/audio/bass"
#define FRONTDOOR_AUDIO_CENTER_API                      "/audio/center"
#define FRONTDOOR_AUDIO_CONTENTTYPE_API                 "/audio/contentType"
#define FRONTDOOR_AUDIO_DUALMONOSELECT_API              "/audio/dualMonoSelect"
#define FRONTDOOR_AUDIO_EQSELECT_API                    "/audio/eqSelect"
#define FRONTDOOR_AUDIO_FORMAT_API                      "/audio/format"
#define FRONTDOOR_AUDIO_GAINOFFSET_API                  "/audio/gainOffset"
#define FRONTDOOR_AUDIO_MODE_API                        "/audio/mode"
#define FRONTDOOR_AUDIO_SUBWOOFERGAIN_API               "/audio/subWooferGain"
#define FRONTDOOR_AUDIO_SUBWOOFERPOLARITY_API           "/audio/subwooferPolarity"
#define FRONTDOOR_AUDIO_SURROUND_API                    "/audio/surround"
#define FRONTDOOR_AUDIO_SURROUNDDELAY_API               "/audio/surroundDelay"
#define FRONTDOOR_AUDIO_TREBLE_API                      "/audio/treble"
#define FRONTDOOR_CEC_API                               "/cec"
#define FRONTDOOR_SYSTEM_POWER_MODE_OPTICALAUTOWAKE_API "/system/power/mode/opticalAutoWake"

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// Definitions that specify the group name and version of the end point resources list are found in
/// the common end points header file EndPointsDefines.h.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
