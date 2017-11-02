////////////////////////////////////////////////////////////////////////////////
/// @file   EddieProductControllerStateTop.h
/// @brief  The Top State in the Eddie Product. Any methods getting
///         called in this state indicate an unexpected call or an error.
///
/// Copyright 2017 Bose Corporation
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <string>
#include "ProductControllerStateTop.h"
#include "ProductControllerStates.h"
#include "HsmState.h"

namespace ProductApp
{
class EddieProductControllerHsm;

class EddieProductControllerStateTop : public ProductControllerStateTop
{
public:
    EddieProductControllerStateTop( EddieProductControllerHsm& hsm,
                                    CHsmState* pSuperState,
                                    Hsm::STATE stateId = 0,
                                    const std::string& name = "TOP" );

    virtual ~EddieProductControllerStateTop()
    {
    }

    bool HandleModulesReady() override;
    bool HandleIntents( KeyHandlerUtil::ActionType_t intent ) override;
};
} // namespace ProductApp
