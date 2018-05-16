////////////////////////////////////////////////////////////////////////////////
/// @file   CustomProductControllerStateLowPowerStandbyTransition.h
/// @brief  Custom override state in Eddie for transitioning into and out of low power standby.
///
/// Copyright 2017 Bose Corporation
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <string>
#include "ProductControllerStateLowPowerStandbyTransition.h"
#include "HsmState.h"

namespace ProductApp
{
class ProductControllerHsm;

class CustomProductControllerStateLowPowerStandbyTransition : public ProductControllerStateLowPowerStandbyTransition
{
public:
    CustomProductControllerStateLowPowerStandbyTransition( ProductControllerHsm& hsm,
                                                           CHsmState* pSuperState,
                                                           Hsm::STATE stateId,
                                                           const std::string& name = "LowPowerStandbyTransition" );

    virtual ~CustomProductControllerStateLowPowerStandbyTransition() override
    {
    }
    void HandleStateStart() override;
};
} /// namespace ProductApp
