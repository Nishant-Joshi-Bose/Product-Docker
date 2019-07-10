////////////////////////////////////////////////////////////////////////////////
/// @file   CustomProductControllerStateLowPowerStandby.h
/// @brief  The Low Power Standby State in the product controller.
///
/// Copyright 2017 Bose Corporation
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <string>
#include "ProductControllerStateLowPowerStandby.h"
#include "HsmState.h"

namespace ProductApp
{
class ProductControllerHsm;

class CustomProductControllerStateLowPowerStandby : public ProductControllerStateLowPowerStandby
{
public:
    CustomProductControllerStateLowPowerStandby( ProductControllerHsm& hsm,
                                                 CHsmState* pSuperState,
                                                 Hsm::STATE stateId,
                                                 const std::string& name = "LowPowerStandby" );

    virtual ~CustomProductControllerStateLowPowerStandby() override
    {
    }

    void HandleStateEnter() override;
    void HandleStateExit() override;
};
} /// namespace ProductApp
