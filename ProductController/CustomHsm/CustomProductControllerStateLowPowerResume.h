////////////////////////////////////////////////////////////////////////////////
/// @file   CustomProductControllerStateLowPowerResume.h
/// @brief  The Low Power Standby State in the Eddie Product.
///
/// Copyright 2017 Bose Corporation
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <string>
#include "ProductControllerStateLowPowerResume.h"
#include "HsmState.h"

namespace ProductApp
{
class ProductControllerHsm;

class CustomProductControllerStateLowPowerResume : public ProductControllerStateLowPowerResume
{
public:
    CustomProductControllerStateLowPowerResume( ProductControllerHsm& hsm,
                                                CHsmState* pSuperState,
                                                Hsm::STATE stateId,
                                                const std::string& name = "LowPowerStandby" );

    virtual ~CustomProductControllerStateLowPowerResume() override
    {
    }

private:
    bool HandleIntentAuxIn( KeyHandlerUtil::ActionType_t intent ) override;
};
} /// namespace ProductApp