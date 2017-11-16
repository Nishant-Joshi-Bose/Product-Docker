////////////////////////////////////////////////////////////////////////////////
///// @file   PlaybackRequestManager.h
///// @brief  Eddie specific PlaybackRequestManager class for Riviera based product
/////
///// @attention Copyright 2017 Bose Corporation, Framingham, MA
//////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "EddieProductController.h"
#include "IntentHandler.h"
#include "SoundTouchInterface/ContentSelectionService.pb.h"
#include "SoundTouchInterface/PlayerService.pb.h"

namespace ProductApp
{

class PlaybackRequestManager: public IntentManager
{
public:
    PlaybackRequestManager( NotifyTargetTaskIF& task,
                            const CliClientMT& cliClient,
                            const FrontDoorClientIF_t& frontDoorClient,
                            EddieProductController& controller );

    ~PlaybackRequestManager() override
    {
    }

    // Public function to Handle intents
    // This function will build and send message either through FrontDoor
    // or through IPC for action based on the intent received.
    //
    // If cb is not null, the call back will return control to HSM in
    // desired function for desired state change
    //
    bool Handle( KeyHandlerUtil::ActionType_t arg ) override;

private:

    void PostPlaybackRequestCbRsp( const SoundTouchInterface::NowPlayingJson& resp );

    void FrontDoorClientErrorCb( const FRONT_DOOR_CLIENT_ERRORS errorCode ) override;

    AsyncCallback<SoundTouchInterface::NowPlayingJson> m_NowPlayingRsp;
};
} // namespace ProductApp
