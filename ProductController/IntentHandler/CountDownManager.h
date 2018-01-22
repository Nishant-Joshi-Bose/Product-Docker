////////////////////////////////////////////////////////////////////////////////
///// @file   CountDownManager.h
///// @brief  CountDownManager class for Riviera based product
/////
///// @attention Copyright 2018 Bose Corporation, Framingham, MA
//////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "IntentManager.h"

#define FACTORY_RESET_TIME            10
#define FIVE_SECOND_TIME              5

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
    uint32_t            m_actionType = 0;
    uint32_t            m_shortCounter = FIVE_SECOND_TIME;
    uint32_t            m_factoryResetCounter = FACTORY_RESET_TIME;

    void NotifyButtonEvent( const std::string& event, const uint32_t state, const uint32_t value );
};
} // namespace ProductApp
