////////////////////////////////////////////////////////////////////////////////
///// @file   IntentManager.h
///// @brief  Eddie specific IntentManager class for Riviera based product
//            This is a base class and any Specific IntentManager like
//            TransportControlManager needs to inherit from this class.
/////
///// @attention Copyright 2017 Bose Corporation, Framingham, MA
//////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "NotifyTargetTaskIF.h"
#include "CliClientMT.h"
#include "FrontDoorClientIF.h"
#include "KeyHandler.h"
#include "AsyncCallback.h"

namespace ProductApp
{
using FrontDoorClientIF_t = std::shared_ptr<FrontDoorClientIF>;
using CbPtr_t  = std::shared_ptr<AsyncCallback<KeyHandlerUtil::ActionType_t&> >;

class IntentManager
{
public:
    IntentManager( NotifyTargetTaskIF& task, CliClientMT& cliClient,
                   const FrontDoorClientIF_t& frontDoorClient ):
        m_frontDoorClient( frontDoorClient ),
        m_task( task ),
        m_cliClient( cliClient ),
        m_errorCb( nullptr, &task )
    {
    }
    ~IntentManager() { }

    // Public function to Handle intents
    // This function will build and send message either through FrontDoor
    // or through IPC for action based on the intent received.
    //
    // If cb is not null, the call back will return control to HSM in
    // desired function for desired state change
    //
    virtual bool Handle( KeyHandlerUtil::ActionType_t arg ) = 0;

    // Public function to register any call backs back into Product HSM
    // Intent Managers will not do any state transistion, it is only expected
    // to valid,build and send messages (through frontdoor or IPC).
    void RegisterCallBack( KeyHandlerUtil::ActionType_t intent, CbPtr_t cb )
    {
        m_intent = intent;
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

    const KeyHandlerUtil::ActionType_t& intent() const
    {
        return m_intent;
    }

    AsyncCallback<FRONT_DOOR_CLIENT_ERRORS>& errorCb()
    {
        return m_errorCb;
    }
    virtual void CallBackError( const FRONT_DOOR_CLIENT_ERRORS errorCode ) = 0;

    FrontDoorClientIF_t                     m_frontDoorClient;
    NotifyTargetTaskIF&                     m_task;
private:
    CliClientMT&                            m_cliClient;
    CbPtr_t                                 m_cb;
    KeyHandlerUtil::ActionType_t            m_intent;
    AsyncCallback<FRONT_DOOR_CLIENT_ERRORS> m_errorCb;
};
} // namespace ProductApp
