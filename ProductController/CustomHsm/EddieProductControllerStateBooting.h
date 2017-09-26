////////////////////////////////////////////////////////////////////////////////
/// @file   EddieProductControllerStateBooting.h
/// @brief  The Booting State in the Eddie Product.
///
/// Copyright 2017 Bose Corporation
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <string>
#include "ProductControllerStateBooting.h"
#include "ProductControllerStates.h"
#include "HsmState.h"

namespace ProductApp
{
class EddieProductControllerHsm;
class EddieProductController;

class EddieProductControllerStateBooting : public ProductControllerStateBooting
{
public:
    EddieProductControllerStateBooting( EddieProductControllerHsm& hsm,
                                        CHsmState* pSuperState,
                                        EddieProductController& eddieProductController,
                                        Hsm::STATE stateId = CUSTOM_PRODUCT_CONTROLLER_STATE_BOOTING,
                                        //Don't change below state name. This is mapped with Madrid's requirement.
                                        const std::string& name = "BOOTING" );

    virtual ~EddieProductControllerStateBooting()
    {
    }

    void HandleStateEnter() override;
    void HandleStateStart() override;
    void HandleStateExit() override;

    bool HandleModulesReady() override;
};
} // namespace ProductApp
