////////////////////////////////////////////////////////////////////////////////
///// @file   PresetManager.h
///// @brief  Eddie specific PresetManager class for Riviera based product
/////
///// @attention Copyright 2017 Bose Corporation, Framingham, MA
//////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "IntentHandler.h"
#include "EddieProductController.h"
#include "SoundTouchInterface/Presets.pb.h"

namespace ProductApp
{

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
    SoundTouchInterface::presets                m_presets; // temp

private:
    SoundTouchInterface::PresetID IntentToPresetIdMap( \
                                                       KeyHandlerUtil::ActionType_t& intent );

    SoundTouchInterface::preset* IsPresetContentPresent( \
                                                         SoundTouchInterface::PresetID presetId,
                                                         SoundTouchInterface::presets &presetItem );

    void BuildPlaybackRequestFromPresetCI( \
                                           SoundTouchInterface::playbackRequestJson &pr,
                                           const SoundTouchInterface::ContentItem &ci );

    void BuildPresetContentItemFromNp( \
                                       SoundTouchInterface::ContentItem *destinationCI,
                                       const SoundTouchInterface::NowPlayingJson_ContentItemJson &sourceCI );

    void FrontDoorClientErrorCb( const FRONT_DOOR_CLIENT_ERRORS errorCode ) override;
};
} // namespace ProductApp
