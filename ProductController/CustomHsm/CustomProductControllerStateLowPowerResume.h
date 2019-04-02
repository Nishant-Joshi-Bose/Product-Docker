////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file      CustomProductControllerStateLowPowerResume.h
///
/// @brief     This header file declares functionality to process product specific events that occur
///            during the low power resume state.
///
/// @attention Copyright (C) 2019 Bose Corporation All Rights Reserved
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ProductControllerStateLowPowerResume.h"

namespace ProductApp
{
class ProductControllerHsm;

class CustomProductControllerStateLowPowerResume : public ProductControllerStateLowPowerResume
{
public:

    CustomProductControllerStateLowPowerResume( ProductControllerHsm& hsm,
                                                CHsmState* pSuperState,
                                                Hsm::STATE stateId,
                                                const std::string& name = "LowPowerResume" );

    ~CustomProductControllerStateLowPowerResume( ) override
    {

    }

    void HandleStateExit() override;
};
} /// namespace ProductApp
