////////////////////////////////////////////////////////////////////////////////
///// @file   PresetManager.h
///// @brief  PresetManager class for Riviera based product
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
    void PutTransportControlCbRsp( const SoundTouchInterface::NowPlayingJson& resp );

private:
    SoundTouchInterface::presets                m_presets; // temp
    static SoundTouchInterface::PresetID IntentToPresetIdMap( KeyHandlerUtil::ActionType_t& intent );

    SoundTouchInterface::preset* IsPresetContentPresent( SoundTouchInterface::PresetID presetId,
                                                         SoundTouchInterface::presets &presetItem ) const;

    void BuildPlaybackRequestFromPresetCI( SoundTouchInterface::playbackRequestJson &pr,
                                           const SoundTouchInterface::ContentItem &ci ) const;

    void BuildPresetContentItemFromNp( SoundTouchInterface::ContentItem *destinationCI,
                                       const SoundTouchInterface::NowPlayingJson_ContentItemJson &sourceCI ) const;

    void FrontDoorClientErrorCb( const FRONT_DOOR_CLIENT_ERRORS errorCode ) override;
    AsyncCallback<SoundTouchInterface::NowPlayingJson> m_NowPlayingRsp;
};
} // namespace ProductApp
