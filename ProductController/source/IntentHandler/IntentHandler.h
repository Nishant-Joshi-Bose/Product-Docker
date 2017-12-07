////////////////////////////////////////////////////////////////////////////////
///// @file   IntentHandler.h
///// @brief  IntentHandler class for Riviera based product
/////
///// @attention Copyright 2017 Bose Corporation, Framingham, MA
//////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "FrontDoorClientIF.h"
#include "CommonIntentHandler.h"
#include "Intents.h"

class CliClientMT;

namespace ProductApp
{
class ProductController;

class IntentHandler: public CommonIntentHandler
{
public:
    IntentHandler( NotifyTargetTaskIF& task,
                   const CliClientMT& cliClient,
                   const FrontDoorClientIF_t& fd_client,
                   ProductController& controller );
    ~IntentHandler() override
    {
    }

    // Initialization will include adding Product-specific IntentManagers that are
    // associated with intent Values
    void Initialize() override;

private:

};
} // namespace ProductApp
