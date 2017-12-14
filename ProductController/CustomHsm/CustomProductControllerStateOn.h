////////////////////////////////////////////////////////////////////////////////
/// @file   CustomProductControllerStateOn.h
/// @brief  The On State in the Eddie Product.
///
/// Copyright 2017 Bose Corporation
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <string>
#include "ProductControllerStateOn.h"
#include "HsmState.h"

namespace ProductApp
{
class ProductControllerHsm;

class CustomProductControllerStateOn : public ProductControllerStateOn
{
public:
    CustomProductControllerStateOn( ProductControllerHsm& hsm,
                                    CHsmState* pSuperState,
                                    Hsm::STATE stateId,
                                    const std::string& name = "On" );

    virtual ~CustomProductControllerStateOn() override
    {
    }
    bool HandleIntentAuxIn( KeyHandlerUtil::ActionType_t intent ) override;
    bool HandleIntentVoice( KeyHandlerUtil::ActionType_t intent ) override;
};

} /// namespace ProductApp
