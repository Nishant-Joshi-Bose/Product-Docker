////////////////////////////////////////////////////////////////////////////////
/// @file   CustomProductControllerState.h
/// @brief  Class for executing product specific event handlers based on
///         product specific events. Home for all product specific event
///         handlers that should be performed within product specific/custom
///         states. This class also holds pure virtual methods for common
///         event handlers.
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
                                  Hsm::STATE stateId = 0, const std::string& name = "Top" );

    virtual ~CustomProductControllerState()
    {
    }

    void HandleStateEnter() override
    {
    }

    void HandleStateStart() override
    {
    }

    void HandleStateExit() override
    {
    }

    virtual bool HandleModulesReady( )
    {
        return false;
    }

    //Declare pure virtual methods for common event handlers here
    //and implement them in derived ProductController state class.
};
} //namespace ProductApp
