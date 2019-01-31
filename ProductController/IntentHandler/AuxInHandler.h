////////////////////////////////////////////////////////////////////////////////
///// @file   AuxInHandler.h
///// @brief  AuxInHandler class
/////
///// @attention Copyright 2017 Bose Corporation, Framingham, MA
//////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "IntentManager.h"
#include "SoundTouchInterface/ContentSelectionService.pb.h"
#include "SoundTouchInterface/PlayerService.pb.h"

namespace ProductApp
{

class AuxInHandler: public IntentManager
{
public:
    AuxInHandler( NotifyTargetTaskIF& task,
                            const CliClientMT& cliClient,
                            const FrontDoorClientIF_t& frontDoorClient,
                            ProductController& controller );

    ~AuxInHandler() override
    {
    }

    // Public function to handle Aux intents
    //
    bool Handle( KeyHandlerUtil::ActionType_t& arg ) override;

private:

    void PostPlaybackRequestCbRsp( const SoundTouchInterface::NowPlaying& resp );

    AsyncCallback<SoundTouchInterface::NowPlaying> m_NowPlayingRsp;
};
} // namespace ProductApp
