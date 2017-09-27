////////////////////////////////////////////////////////////////////////////////
/// @file   CustomProductControllerState.h
/// @brief  Class for executing product specific event handlers based on
///         product specific events. Home for all product specific event
///         handlers that should be performed within product specific/custom
///         states.
///
/// Copyright 2017 Bose Corporation
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Hsm.h"
#include "HsmState.h"

namespace ProductApp
{
class ProductControllerHsm;

class CustomProductControllerState : public CHsmState
{
public:
    CustomProductControllerState( ProductControllerHsm& hsm, CHsmState* pSuperState,
                                  STATE stateId = 0, const std::string& name = "Top" );

    virtual ~CustomProductControllerState()
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

    //Add your HandleCustomXXX() functions here that implements product specific event handlers.
    // This function should be overridden in the product specific state derived class
    // that executes product specific event handler.l
    virtual bool HandleCustomEvent();

    virtual bool HandleLanguageRequest()
    {
        return false;
    }
};
} //namespace ProductApp
