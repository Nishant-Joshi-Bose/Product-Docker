////////////////////////////////////////////////////////////////////////////////
/// @file   CustomProductControllerStateSwUpdating.h
/// @brief  The product specific 'Software Updating' state.
///
/// Copyright 2017 Bose Corporation
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <string>
#include "ProductControllerHsm.h"
#include "ProductControllerState.h"
#include "ProductControllerStates.h"
#include "HsmState.h"

namespace ProductApp
{
class ProductControllerHsm;

class CustomProductControllerStateSwUpdating : public ProductControllerState
{
public:
    CustomProductControllerStateSwUpdating( ProductControllerHsm& hsm,
                                            CHsmState* pSuperState,
                                            ProductController& productController,
                                            STATE stateId = CUSTOM_PRODUCT_CONTROLLER_STATE_SOFTWARE_UPDATING,
                                            const std::string& name = "CustomProductControllerStateSoftwareUpdating" );

    virtual ~CustomProductControllerStateSwUpdating()
    {
    }

    void HandleStateEnter() override;
    void HandleStateStart() override;
    void HandleStateExit() override;
};
} // namespace ProductApp
