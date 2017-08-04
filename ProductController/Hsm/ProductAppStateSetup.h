////////////////////////////////////////////////////////////////////////////////
/// @file   ProductAppStateSetup.h
/// @brief  The Setup State in the Product Application HSM. This state is
///         active on until the product is setup.
///
/// Copyright 2017 Bose Corporation
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <string>
#include "ProductAppHsm.h"
#include "ProductAppState.h"
#include "ProductAppStates.h"
#include "HsmState.h"

namespace ProductApp
{
class ProductAppHsm;

class ProductAppStateSetup : public ProductAppState
{
public:
    ProductAppStateSetup( ProductAppHsm& hsm,
                          CHsmState* pSuperState,
                          ProductController& productController,
                          STATE stateId = PRODUCT_APP_STATE_SETUP,
                          const std::string& name = "ProductAppStateSetup" );

    virtual ~ProductAppStateSetup()
    {
    }

    void HandleStateEnter() override;
    void HandleStateStart() override;
    void HandleStateExit() override;
};
} // namespace ProductApp
