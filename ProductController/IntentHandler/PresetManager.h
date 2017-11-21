////////////////////////////////////////////////////////////////////////////////
///// @file   PresetManager.h
///// @brief  Eddie specific PresetManager class for Riviera based product
/////
///// @attention Copyright 2017 Bose Corporation, Framingham, MA
//////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "IntentHandler.h"
#include "EddieProductController.h"

namespace ProductApp
{
constexpr uint8_t  PRESET_1                   = 1;
constexpr uint8_t  PRESET_2                   = 2;
constexpr uint8_t  PRESET_3                   = 3;
constexpr uint8_t  PRESET_4                   = 4;
constexpr uint8_t  PRESET_5                   = 5;
constexpr uint8_t  PRESET_6                   = 6;
constexpr uint8_t  PRESET_INVALID             = 0xFF;

class PresetManager: public IntentManager
{
public:
    PresetManager( NotifyTargetTaskIF& task,
                   const CliClientMT& cliClient,
                   const FrontDoorClientIF_t& frontDoorClient,
                   EddieProductController& controller );

    ~PresetManager() override
    {
    }

    // Public function to Handle intents
    // This function will build and send message either through FrontDoor
    // or through IPC for action based on the intent received.
    //
    // If cb is not null, the call back will return control to HSM in
    // desired function for desired state change
    //
    bool Handle( KeyHandlerUtil::ActionType_t& arg ) override;

private:
    uint8_t IntentToIdMap( KeyHandlerUtil::ActionType_t& intent );
    bool IsPresetContentPreset( uint8_t presetId,
                                SoundTouchInterface::preset &presetItem );

    void FrontDoorClientErrorCb( const FRONT_DOOR_CLIENT_ERRORS errorCode ) override;
};
} // namespace ProductApp
