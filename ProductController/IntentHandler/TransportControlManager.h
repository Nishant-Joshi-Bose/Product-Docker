////////////////////////////////////////////////////////////////////////////////
///// @file   TransportControlManager.h
///// @brief  Eddie specific TransportControlManager class for Riviera based product
/////
///// @attention Copyright 2017 Bose Corporation, Framingham, MA
//////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "IntentManager.h"

namespace ProductApp
{

class TransportControlManager: public IntentManager
{
public:
    TransportControlManager( NotifyTargetTaskIF& task, CliClientMT& cliClient,
                             const FrontDoorClientIF_t& frontDoorClient ):
        IntentManager( task, cliClient, frontDoorClient )
    {
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

};
} // namespace ProductApp
