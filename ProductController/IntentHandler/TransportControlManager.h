////////////////////////////////////////////////////////////////////////////////
///// @file   TransportControlManager.h
///// @brief  Eddie specific TransportControlManager class for Riviera based product
/////
///// @attention Copyright 2017 Bose Corporation, Framingham, MA
//////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "IntentHandler.h"
#include "EddieProductController.h"
#include "SoundTouchInterface/PlayerService.pb.h"

namespace ProductApp
{

class TransportControlManager: public IntentManager
{
public:
    TransportControlManager( NotifyTargetTaskIF& task,
                             const CliClientMT& cliClient,
                             const FrontDoorClientIF_t& frontDoorClient,
                             const ProductController& controller );

    ~TransportControlManager() override
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
    void PutTransportControlCbRsp( const SoundTouchInterface::NowPlayingJson& resp );

private:
    bool TogglePlayPause()
    {
        if( m_play )
            m_play = false;
        else
            m_play = true;
        return m_play;
    }
    void Play()
    {
        m_play = true;
    }
    void Pause()
    {
        m_play = false;
    }
    bool ValidSourceAvailable();
    bool CurrentlyPlaying();
    SoundTouchInterface::StatusJson CurrentStatusJson();

    void FrontDoorClientErrorCb( const FRONT_DOOR_CLIENT_ERRORS errorCode ) override;

    AsyncCallback<SoundTouchInterface::NowPlayingJson> m_NowPlayingRsp;
    bool m_play;
};
} // namespace ProductApp
