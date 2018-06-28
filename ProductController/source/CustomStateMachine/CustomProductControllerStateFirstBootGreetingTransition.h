////////////////////////////////////////////////////////////////////////////////
/// @file   CustomProductControllerStateFirstBootGreetingTransition.h
/// @brief  The State Prior to first time greet state .
///
/// Copyright 2018 Bose Corporation
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <string>
#include "ProductControllerStateFirstBootGreetingTransition.h"
#include "ProductControllerState.h"
#include "HsmState.h"

namespace ProductApp
{
class ProductControllerHsm;
class ProfessorProductController;

class CustomProductControllerStateFirstBootGreetingTransition :
    public ProductControllerStateFirstBootGreetingTransition
{
public:
    CustomProductControllerStateFirstBootGreetingTransition( ProductControllerHsm& hsm,
                                                             CHsmState* pSuperState,
                                                             Hsm::STATE stateId,
                                                             const std::string& name = "FirstBootGreetingTransition" );

    ~CustomProductControllerStateFirstBootGreetingTransition() override
    {
    }

    void HandleStateEnter()                 override;
    bool HandleLPMPowerStatusFullPowerOn()  override;
    bool HandleAccessoriesAreKnown()        override;

private:
    APTimerPtr  m_TimerWaitForAccessories;

};
} /// namespace ProductApp
