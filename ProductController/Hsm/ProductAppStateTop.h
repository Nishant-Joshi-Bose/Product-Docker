////////////////////////////////////////////////////////////////////////////////
/// @file   ProductAppStateTop.h
/// @brief  The Top State in the Product Application HSM.  Any methods getting
///         called in this state indicate an unexpected call or an error.
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
class ProductAppStateTop : public ProductAppState
{
public:
    ProductAppStateTop( ProductAppHsm& hsm,
                        CHsmState* pSuperState,
                        ProductController& productController,
                        STATE stateId = 0,
                        const std::string& name = "ProductAppStateTop" );

    virtual ~ProductAppStateTop()
    {
    }

    bool HandleModulesReady() override;
};
} // namespace ProductApp
