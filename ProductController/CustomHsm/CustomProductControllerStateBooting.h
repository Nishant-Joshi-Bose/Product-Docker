////////////////////////////////////////////////////////////////////////////////
/// @file   CustomProductControllerStateBooting.h
/// @brief  The Booting State in the Eddie Product.
///
/// Copyright 2017 Bose Corporation
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <string>
#include "ProductControllerStateBooting.h"
#include "HsmState.h"

namespace ProductApp
{
class ProductControllerHsm;

class CustomProductControllerStateBooting : public ProductControllerStateBooting
{
public:
    CustomProductControllerStateBooting( ProductControllerHsm& hsm,
                                         CHsmState* pSuperState,
                                         Hsm::STATE stateId,
                                         const std::string& name = "Booting" );

    virtual ~CustomProductControllerStateBooting() override
    {
    }
    void PossiblyGoToNextState() override;
    void HandleStateExit() override;
};
} /// namespace ProductApp
