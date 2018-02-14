////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file      CustomProductControllerStatePlayingSelectedSetup.h
/// @brief     This source code file contains functionality to process events
///            that occur in Professor during the product setup state.
///
/// @attention Copyright (C) 2018 Bose Corporation All Rights Reserved
///
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <string>
#include "ProductControllerStatePlayingSelectedSetup.h"
#include "HsmState.h"

namespace ProductApp
{

class ProductControllerHsm;

class CustomProductControllerStatePlayingSelectedSetup : public ProductControllerStatePlayingSelectedSetup
{
public:
    CustomProductControllerStatePlayingSelectedSetup(
        ProductControllerHsm& hsm,
        CHsmState* pSuperState,
        Hsm::STATE stateId,
        const std::string& name = "Setup" );

    ~CustomProductControllerStatePlayingSelectedSetup() override
    {
    }

    bool HandleIntentMuteControl( KeyHandlerUtil::ActionType_t intent ) override;
};

} /// namespace ProductApp
