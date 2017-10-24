////////////////////////////////////////////////////////////////////////////////
///// @file   TransportControlManager.h
///// @brief  Eddie specific TransportControlManager class for Riviera based product
/////
///// @attention Copyright 2017 Bose Corporation, Framingham, MA
//////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "IntentHandler.h"
#include "SoundTouchInterface/PlayerService.pb.h"

namespace ProductApp
{

class TransportControlManager: public IntentManager
{
public:
    TransportControlManager( NotifyTargetTaskIF& task, CliClientMT& cliClient,
                             const FrontDoorClientIF_t& frontDoorClient ):
        IntentManager( task, cliClient, frontDoorClient ),
        m_NowPlayingRsp( nullptr, &task )
    {
        m_frontDoorClientErrorCb = AsyncCallback<FRONT_DOOR_CLIENT_ERRORS>\
            ( std::bind( &TransportControlManager::FrontDoorClientErrorCb,
                         this, std::placeholders::_1 ), &task );

        AsyncCallback<SoundTouchInterface::NowPlayingJson> m_NowPlayingRsp =
            AsyncCallback<SoundTouchInterface::NowPlayingJson> ( std::bind( &TransportControlManager::PutTransportControlCbRsp, this, std::placeholders::_1 ), &task );

    }
    virtual ~TransportControlManager() { }

    // Public function to Handle intents
    // This function will build and send message either through FrontDoor
    // or through IPC for action based on the intent received.
    //
    // If cb is not null, the call back will return control to HSM in
    // desired function for desired state change
    //
    bool Handle( KeyHandlerUtil::ActionType_t arg ) override;

private:
    bool ValidSourceAvailable();
    bool CurrentlyPlaying();

    void PutTransportControlCbRsp( const SoundTouchInterface::NowPlayingJson& resp );

    virtual void FrontDoorClientErrorCb( const FRONT_DOOR_CLIENT_ERRORS errorCode ) override;

    AsyncCallback<SoundTouchInterface::NowPlayingJson> m_NowPlayingRsp;
};
} // namespace ProductApp
