////////////////////////////////////////////////////////////////////////////////
/// @file   CustomProductControllerStateSetup.h
/// @brief  The Setup State in the Eddie Product.
///
/// Copyright 2017 Bose Corporation
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <string>
#include "ProductControllerStateSetup.h"
#include "HsmState.h"

namespace ProductApp
{
class ProductControllerHsm;

class CustomProductControllerStateSetup : public ProductControllerStateSetup
{
public:
    CustomProductControllerStateSetup( ProductControllerHsm& hsm,
                                       CHsmState* pSuperState,
                                       Hsm::STATE stateId,
                                       const std::string& name = "Setup" );

    virtual ~CustomProductControllerStateSetup() override
    {
    }

    bool HandleIntentAuxIn( KeyHandlerUtil::ActionType_t intent ) override;
    bool HandleIntentBlueTooth( KeyHandlerUtil::ActionType_t intent ) override;
};
} /// namespace ProductApp
