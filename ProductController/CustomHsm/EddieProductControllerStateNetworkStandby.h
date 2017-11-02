////////////////////////////////////////////////////////////////////////////////
/// @file   EddieProductControllerStateNetworkStandby.h
/// @brief  The Network Standby State in the Eddie Product.
///
/// Copyright 2017 Bose Corporation
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <string>
#include "ProductControllerStateNetworkStandby.h"
#include "ProductControllerStates.h"
#include "HsmState.h"

namespace ProductApp
{
class EddieProductControllerHsm;

class EddieProductControllerStateNetworkStandby : public ProductControllerStateNetworkStandby
{
public:
    EddieProductControllerStateNetworkStandby( EddieProductControllerHsm& hsm,
                                               CHsmState* pSuperState,
                                               Hsm::STATE stateId = CUSTOM_PRODUCT_CONTROLLER_STATE_NETWORK_STANDBY,
                                               //Don't change below state name. This is mapped with Madrid's requirement.
                                               const std::string& name = "NETWORK_STANDBY" );

    virtual ~EddieProductControllerStateNetworkStandby()
    {
    }

    void HandleStateEnter() override;
    void HandleStateStart() override;
    void HandleStateExit() override;
    bool HandleIntents( KeyHandlerUtil::ActionType_t intent ) override;
};
} // namespace ProductApp
