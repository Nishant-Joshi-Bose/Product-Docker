////////////////////////////////////////////////////////////////////////////////
/// @file   EddieProductControllerStateSetup.h
/// @brief  The Setup State in the Eddie Product.
///
/// Copyright 2017 Bose Corporation
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <string>
#include "ProductControllerStateSetup.h"
#include "ProductControllerStates.h"
#include "HsmState.h"

namespace ProductApp
{
class EddieProductControllerHsm;
class EddieProductController;

class EddieProductControllerStateSetup : public ProductControllerStateSetup
{
public:
    EddieProductControllerStateSetup( EddieProductControllerHsm& hsm,
                                      CHsmState* pSuperState,
                                      EddieProductController& eddieProductController,
                                      Hsm::STATE stateId = CUSTOM_PRODUCT_CONTROLLER_STATE_SETUP,
                                      //Don't change below state name. This is mapped with Madrid's requirement.
                                      const std::string& name = "SETUP" );

    virtual ~EddieProductControllerStateSetup()
    {
    }

    void HandleStateEnter() override;
    void HandleStateStart() override;
    void HandleStateExit() override;
    bool HandleIntents( KeyHandlerUtil::ActionType_t result ) override;
    bool HandleNetworkModuleStatus( const NetManager::Protobuf::NetworkStatus& networkStatus, int profileSize ) override;
};
} // namespace ProductApp
