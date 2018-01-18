////////////////////////////////////////////////////////////////////////////////
///// @file   CountdownManager.h
///// @brief  CountdownManager class for Riviera based product
/////
///// @attention Copyright 2017 Bose Corporation, Framingham, MA
//////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "IntentManager.h"

namespace ProductApp
{

class CountdownManager: public IntentManager
{
public:
    CountdownManager( NotifyTargetTaskIF& task,
                      const CliClientMT& cliClient,
                      const FrontDoorClientIF_t& frontDoorClient,
                      ProductController& controller );
    ~CountdownManager() override { }

    // Public function to Handle intents
    // This function will build and send message either through FrontDoor
    // or through IPC for action based on the intent received.
    //
    // If cb is not null, the call back will return control to HSM in
    // desired function for desired state change
    //
    bool Handle( KeyHandlerUtil::ActionType_t& arg ) override;

private:
    bool            m_complete;
    int32_t         m_counter;
    int32_t         m_countDownType;

    void NotifyButtonEvent( std::string& event, std::string& state, uint32_t value );
};
} // namespace ProductApp
