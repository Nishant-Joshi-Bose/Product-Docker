////////////////////////////////////////////////////////////////////////////////
///// @file   TransportControlManager.h
///// @brief  Eddie specific TransportControlManager class for Riviera based product
/////
///// @attention Copyright 2017 Bose Corporation, Framingham, MA
//////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "IntendManager.h"

namespace ProductApp
{

class TransportControlManager: public IntendManager
{
public:
    TransportControlManager(NotifyTargetTaskIF& task, CliClientMT& cliClient,
                            const FrontDoorClientIF_t& frontDoorClient):
                  IntendManager(task, cliClient, frontDoorClient)
    {
    }   
    virtual ~TransportControlManager() { } 

    // Public function to Handle intends
    // This function will build and send message either through FrontDoor
    // or through IPC for action based on the intend received.
    //
    // If cb is not null, the call back will return control to HSM in
    // desired function for desired state change
    //
    bool Handle(KeyHandlerUtil::ActionType_t arg);

};
} // namespace ProductApp
