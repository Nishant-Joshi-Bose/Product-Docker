////////////////////////////////////////////////////////////////////////////////
///// @file   IntendManager.h
///// @brief  Eddie specific IntendManager class for Riviera based product
/////
///// @attention Copyright 2017 Bose Corporation, Framingham, MA
//////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "NotifyTargetTaskIF.h"
#include "CliClientMT.h"
#include "FrontDoorClientIF.h"
#include "KeyHandler.h"

namespace ProductApp
{
using FrontDoorClientIF_t = std::shared_ptr<FrontDoorClientIF>;
using CbPtr_t  = std::shared_ptr<AsyncCallback<KeyHandlerUtil::ActionType_t&> >;

class IntendManager
{
public:
    IntendManager( NotifyTargetTaskIF& task, CliClientMT& cliClient,
                   const FrontDoorClientIF_t& frontDoorClient ):
        m_task( task ),
        m_cliClient( cliClient ),
        m_frontDoorClient( frontDoorClient )
    {
    }
    ~IntendManager() { }

    // Public function to Handle intends
    // This function will build and send message either through FrontDoor
    // or through IPC for action based on the intend received.
    //
    // If cb is not null, the call back will return control to HSM in
    // desired function for desired state change
    //
    virtual bool Handle( KeyHandlerUtil::ActionType_t arg ) = 0;

    // Public function to register any call backs back into Product HSM
    // Intend Managers will not do any state transistion, it is only expected
    // to valid,build and send messages (through frontdoor or IPC).
    void RegisterCallBack( KeyHandlerUtil::ActionType_t intend, CbPtr_t cb )
    {
        m_intend = intend;
        m_cb     = cb;
        return;
    }

protected:
    const NotifyTargetTaskIF& GetTask() const
    {
        return m_task;
    }

    const CliClientMT& GetCli() const
    {
        return m_cliClient;
    }

    const FrontDoorClientIF_t& GetFrontDoor() const
    {
        return m_frontDoorClient;
    }

    const CbPtr_t& cb() const
    {
        return m_cb;
    }

    const KeyHandlerUtil::ActionType_t& intend() const
    {
        return m_intend;
    }

private:

    NotifyTargetTaskIF&               m_task;
    CliClientMT&                      m_cliClient;
    FrontDoorClientIF_t               m_frontDoorClient;
    CbPtr_t                           m_cb;
    KeyHandlerUtil::ActionType_t      m_intend;
};
} // namespace ProductApp
