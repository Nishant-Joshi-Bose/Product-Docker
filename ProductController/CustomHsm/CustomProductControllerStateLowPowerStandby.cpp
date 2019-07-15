////////////////////////////////////////////////////////////////////////////////
/// @file   CustomProductControllerStateLowPowerStandby.cpp
/// @brief  The Low Power Standby State in the product controller.
///
/// Copyright 2017 Bose Corporation
////////////////////////////////////////////////////////////////////////////////

#include "CustomProductControllerStateLowPowerStandby.h"
#include "ProductControllerHsm.h"
#include "CustomProductController.h"
#include "DPrint.h"

static DPrint s_logger( "CustomProductControllerStateLowPowerStandby" );

namespace ProductApp
{
CustomProductControllerStateLowPowerStandby::CustomProductControllerStateLowPowerStandby( ProductControllerHsm& hsm,
        CHsmState* pSuperState,
        Hsm::STATE stateId,
        const std::string& name ) :
    ProductControllerStateLowPowerStandby( hsm, pSuperState, stateId, name )
{
    BOSE_DEBUG( s_logger, __func__ );
}

void CustomProductControllerStateLowPowerStandby::HandleStateExit()
{
    // Turn ON LCD display controller.
    BOSE_DEBUG( s_logger, "Turn LCD display ON in %s.", __func__ );
    AsyncCallback<void> emptyCb( [] {}, nullptr );
    GetCustomProductController().GetDisplayController()->RequestTurnDisplayOnOff( true, emptyCb );

    ProductControllerStateLowPowerStandby::HandleStateExit();
}

} /// namespace ProductApp
