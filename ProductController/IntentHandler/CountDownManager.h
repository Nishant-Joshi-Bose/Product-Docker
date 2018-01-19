////////////////////////////////////////////////////////////////////////////////
///// @file   CountDownManager.h
///// @brief  CountDownManager class for Riviera based product
/////
///// @attention Copyright 2018 Bose Corporation, Framingham, MA
//////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "IntentManager.h"
#include "Intents.h"

namespace ProductApp
{

class CountDownManager: public IntentManager
{
public:
    CountDownManager( NotifyTargetTaskIF& task,
                      const CliClientMT& cliClient,
                      const FrontDoorClientIF_t& frontDoorClient,
                      ProductController& controller );
    ~CountDownManager() override { }

    // Public function to Handle intents
    // This function will build and send message either through FrontDoor
    // or through IPC for action based on the intent received.
    //
    // If cb is not null, the call back will return control to HSM in
    // desired function for desired state change
    //
    bool Handle( KeyHandlerUtil::ActionType_t& arg ) override;

private:
    uint32_t            m_eventType = 0;
    uint32_t            m_shortCounter = 5;
    uint32_t            m_factoryResetCounter = 10;

    void NotifyButtonEvent( const std::string& event, const std::string& state, uint32_t value );
};
} // namespace ProductApp
