////////////////////////////////////////////////////////////////////////////////
/////// @file   CustomChimeEvents.h
/////// @brief  Chime events listing matching to ChimesConfiguration.json
///////
/////// @attention Copyright 2017 Bose Corporation, Framingham, MA
////////////////////////////////////////////////////////////////////////////////////
#pragma once

namespace ProductApp
{

typedef enum
{
    CHIME_ACCESSORY_PAIRING_COMPLETE_SUB                    = 20,   // starting at 20 to avoid collision with common ChimeEvents enum
    CHIME_ACCESSORY_PAIRING_COMPLETE_REAR_SPEAKER           = 21,
    CHIME_ACCESSORY_PAIRING_COMPLETE_SUB_FROM_LAN           = 22,
    CHIME_ACCESSORY_PAIRING_COMPLETE_REAR_SPEAKER_FROM_LAN  = 23
} CustomChimeEvents_t;
}//namespace ProductApp
