////////////////////////////////////////////////////////////////////////////////
/// @file   CustomProductControllerStateLowPowerStandbyTransition.cpp
/// @brief  Custom override state in Eddie for transitioning into and out of low power standby.
///
/// Copyright 2017 Bose Corporation
////////////////////////////////////////////////////////////////////////////////

#include "CustomProductControllerStateLowPowerStandbyTransition.h"
#include "ProductControllerHsm.h"
#include "EddieProductController.h"
#include "DPrint.h"

static DPrint s_logger( "CustomProductControllerStateLowPowerStandbyTransition" );

namespace ProductApp
{
CustomProductControllerStateLowPowerStandbyTransition::CustomProductControllerStateLowPowerStandbyTransition( ProductControllerHsm& hsm,
        CHsmState* pSuperState,
        Hsm::STATE stateId,
        const std::string& name ) :
    ProductControllerStateLowPowerStandbyTransition( hsm, pSuperState, stateId, name )
{
    BOSE_INFO( s_logger, __func__ );
}

void CustomProductControllerStateLowPowerStandbyTransition::HandleStateStart()
{
    BOSE_INFO( s_logger, __func__ );

    // Turn OFF LCD display controller.
    BOSE_INFO( s_logger, "Turn LCD display OFF in %s.", __func__ );
    GetCustomProductController().TurnDisplayOnOff( false );

    ProductControllerStateLowPowerStandbyTransition::HandleStateStart();
}

} /// namespace ProductApp
