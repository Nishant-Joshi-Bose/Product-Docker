////////////////////////////////////////////////////////////////////////////////
///// @file   NetworkStandbyManager.h
///// @brief  NetworkStandbyManager class for Riviera based product
/////
///// @attention Copyright 2017 Bose Corporation, Framingham, MA
//////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "IntentHandler.h"
#include "EddieProductController.h"
#include "SoundTouchInterface/PlayerService.pb.h"

namespace ProductApp
{

class NetworkStandbyManager: public IntentManager
{
public:
    NetworkStandbyManager( NotifyTargetTaskIF& task,
                           const CliClientMT& cliClient,
                           const FrontDoorClientIF_t& frontDoorClient,
                           EddieProductController& controller );

    ~NetworkStandbyManager() override
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
};
} // namespace ProductApp
