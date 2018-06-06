////////////////////////////////////////////////////////////////////////////////
/////// @file   ProfessorChimeEvents.h
/////// @brief  Chime events listing matching to Professor/GingerCheevers ChimesConfiguration.json
///////
/////// @attention Copyright 2017 Bose Corporation, Framingham, MA
////////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "ChimesController/ChimeEvents.h"

namespace ProductApp
{

typedef enum
{
    ACCESSORY_PAIRING_COMPLETE_SUB              = 20,   // starting at 20 to avoid collision with common ChimeEvents enum
    ACCESSORY_PAIRING_COMPLETE_REAR_SPEAKER     = 21,
} ProfessorChimeEvents_t;
}//namespace ProductApp
