////////////////////////////////////////////////////////////////////////////////
///// @file   BlueToothManager.h
///// @brief  Eddie specific BlueToothManager class for Riviera based product
/////
///// @attention Copyright 2017 Bose Corporation, Framingham, MA
//////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "ProductController.h"
#include "IntentHandler.h"
#include "EddieProductController.h"

namespace ProductApp
{

class BlueToothManager: public IntentManager
{
public:
    BlueToothManager( NotifyTargetTaskIF& task,
                             const CliClientMT& cliClient,
                             const FrontDoorClientIF_t& frontDoorClient,
                             const ProductController& controller ):
        IntentManager( task, cliClient, frontDoorClient, controller )
    {
        m_frontDoorClientErrorCb = AsyncCallback<FRONT_DOOR_CLIENT_ERRORS>\
                                   ( std::bind( &BlueToothManager::FrontDoorClientErrorCb,
                                                this, std::placeholders::_1 ), &task );

    }
    virtual ~BlueToothManager() { }

    // Public function to Handle intents
    // This function will build and send message either through FrontDoor
    // or through IPC for action based on the intent received.
    //
    // If cb is not null, the call back will return control to HSM in
    // desired function for desired state change
    //
    bool Handle( KeyHandlerUtil::ActionType_t arg ) override;

private:
    virtual void FrontDoorClientErrorCb( const FRONT_DOOR_CLIENT_ERRORS errorCode ) override;

};
} // namespace ProductApp
