/// @file   ProductAppState.h
/// @brief  Provides the base class for all ProductApplication HSM states.
///     Home for all operations that only states should perform.
///
/// Copyright 2017 Bose Corporation
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "Hsm.h"
#include "HsmState.h"
#include "ProductAppHsm.h"
#include "SoundTouchInterface/Msg.pb.h"

namespace ProductApp
{
class ProductAppHsm;
class ProductController;

class ProductAppState : public CHsmState
{
public:
    ProductAppState( ProductAppHsm& hsm, CHsmState* pSuperState, ProductController& productController,
                     STATE stateId = 0, const std::string& name = "Top" );

    virtual ~ProductAppState()
    {
    }

    virtual void HandleStateEnter() override
    {
    }

    virtual void HandleStateStart() override
    {
    }

    virtual void HandleStateExit() override
    {
    }

    //ToDo: Add HandleSomething() functions and callbacks here.

    //This function demonstrates how websocket request triggers product application's state machine.
    virtual bool HandleSetupEndPoint( SoundTouchInterface::msg_Header const& cookie, std::string const& body, std::string const& operation )
    {
        return false;
    }

protected:
    ProductAppHsm& GetHsm()
    {
        return m_hsm;
    }
private:
    ProductAppHsm& m_hsm;
    ProductController& m_productController;
};
} // namespace ProductApp
