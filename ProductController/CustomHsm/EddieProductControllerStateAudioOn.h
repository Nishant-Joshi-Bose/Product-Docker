////////////////////////////////////////////////////////////////////////////////
/// @file   EddieProductControllerStateAudioOn.h
/// @brief  The Audio On State in the Eddie Product.
///
/// Copyright 2017 Bose Corporation
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <string>
#include "ProductControllerStateOn.h"
#include "ProductControllerStates.h"
#include "HsmState.h"

namespace ProductApp
{
class EddieProductControllerHsm;

class EddieProductControllerStateAudioOn : public ProductControllerStateOn
{
public:
    EddieProductControllerStateAudioOn( EddieProductControllerHsm& hsm,
                                        CHsmState* pSuperState,
                                        Hsm::STATE stateId = PRODUCT_CONTROLLER_STATE_ON,
                                        //Don't change below state name. This is mapped with Madrid's requirement.
                                        const std::string& name = "AUDIO_ON" );

    virtual ~EddieProductControllerStateAudioOn()
    {
    }

    void HandleStateEnter() override;
    void HandleStateStart() override;
    void HandleStateExit() override;
    bool HandleIntents( KeyHandlerUtil::ActionType_t intent ) override;
    bool HandleNetworkStandbyIntent( ) override;

};
} /// namespace ProductApp
