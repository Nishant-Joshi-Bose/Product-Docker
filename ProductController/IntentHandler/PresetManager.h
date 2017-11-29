////////////////////////////////////////////////////////////////////////////////
///// @file   PresetManager.h
///// @brief  PresetManager class for Riviera based product
/////
///// @attention Copyright 2017 Bose Corporation, Framingham, MA
//////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "IntentManager.h"
#include "Presets.pb.h"
#include "ContentItem.pb.h"
#include "SoundTouchInterface/ContentSelectionService.pb.h"

namespace ProductApp
{

class PresetManager: public IntentManager
{
public:
    PresetManager( NotifyTargetTaskIF& task,
                   const CliClientMT& cliClient,
                   const FrontDoorClientIF_t& frontDoorClient,
                   ProductController& controller );

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
    ProductPresets::presets    m_presets; // temp
    static PresetID IntentToPresetIdMap( KeyHandlerUtil::ActionType_t& intent );

    ProductPresets::preset* IsPresetContentPresent( PresetID presetId,
                                                    ProductPresets::presets &presetItem ) const;

    void BuildPlaybackRequestFromPresetCI( SoundTouchInterface::playbackRequestJson &pr,
                                           const ContentItem &ci ) const;

    void BuildPresetContentItemFromNp( ContentItem *destinationCI,
                                       const SoundTouchInterface::NowPlayingJson_ContentItemJson &sourceCI ) const;

    AsyncCallback<SoundTouchInterface::NowPlayingJson> m_NowPlayingRsp;
};
} // namespace ProductApp
