////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file      ProductEndpointDefines.h
///
/// @brief     This header file declares Front Door End Point definitions used by the custom product
///            controller.
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
///           Front Door Custom End Point Definitions
///
////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// The following definitions are alphabetically listed end point resource strings, and are used by
/// the custom product controller as a client or consumer to receive data
/// from other processes or to send data to other processes to drive product functionality.
///
////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// The following definitions are alphabetically listed end point resource strings, and are used by
/// the custom product controller as a server or producer to send data
/// requested by other processes or to receive data from other processes to drive functionality.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
#define FRONTDOOR_ACCESSORIES_API                       "/accessories"
#define FRONTDOOR_ACCESSORIES_PLAYTONES_API             "/accessories/playTones"
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
#define FRONTDOOR_AUDIO_REBROADCASTLATENCY_MODE_API     "/audio/rebroadcastLatency/mode"
#define FRONTDOOR_AUDIO_SUBWOOFERGAIN_API               "/audio/subwooferGain"
#define FRONTDOOR_AUDIO_SUBWOOFERPOLARITY_API           "/audio/subwooferPolarity"
#define FRONTDOOR_AUDIO_SURROUND_API                    "/audio/surround"
#define FRONTDOOR_AUDIO_SURROUNDDELAY_API               "/audio/surroundDelay"
#define FRONTDOOR_AUDIO_TREBLE_API                      "/audio/treble"
#define FRONTDOOR_AUDIO_VOLUME_API                      "/audio/volume"
#define FRONTDOOR_AUDIO_ZONE_API                        "/audio/zone"
#define FRONTDOOR_CEC_API                               "/cec"
#define FRONTDOOR_RATING_API                            "/content/nowPlaying/rating"
#define FRONTDOOR_SYSTEM_POWER_MODE_OPTICALAUTOWAKE_API "/system/power/mode/opticalAutoWake"
#define FRONTDOOR_SYSTEM_POWER_MACRO_API                "/system/power/macro"
#define FRONTDOOR_SYSTEM_POWER_TIMEOUTS_API             "/system/power/timeouts"
#define FRONTDOOR_UI_ALIVE                              "/ui/alive"
#define FRONTDOOR_BT_SOURCE_STATUS                      "/bluetooth/source/status"


////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// Definitions that specify the group name and version of the end point resources list are found in
/// the common end points header file EndPointsDefines.h.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
