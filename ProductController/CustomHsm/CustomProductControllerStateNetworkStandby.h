////////////////////////////////////////////////////////////////////////////////
/// @file   CustomProductControllerStateNetworkStandby.h
/// @brief  Custom override of the "network standby" state for Eddie. Functionality
///         here will also effect child states.
///
/// Copyright 2017 Bose Corporation
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <string>
#include "ProductControllerStateNetworkStandby.h"
#include "HsmState.h"

namespace ProductApp
{
class ProductControllerHsm;

class CustomProductControllerStateNetworkStandby : public ProductControllerStateNetworkStandby
{
public:
    CustomProductControllerStateNetworkStandby( ProductControllerHsm& hsm,
                                                CHsmState* pSuperState,
                                                Hsm::STATE stateId,
                                                const std::string& name = "On" );

    virtual ~CustomProductControllerStateNetworkStandby() override
    {
    }

    void HandleStateEnter() override;
    void HandleStateExit() override;
};

} /// namespace ProductApp
