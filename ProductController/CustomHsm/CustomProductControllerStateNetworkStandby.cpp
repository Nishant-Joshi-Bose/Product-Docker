////////////////////////////////////////////////////////////////////////////////
/// @file   CustomProductControllerStateNetworkStandby.cpp
/// @brief  Custom override of the "network standby" state for the
///         product controller.  Functionality here will also affect
///         child states.
///
/// Copyright 2017 Bose Corporation
////////////////////////////////////////////////////////////////////////////////

#include "CustomProductControllerStateNetworkStandby.h"
#include "ProductControllerHsm.h"
#include "CustomProductController.h"
#include "DPrint.h"

static DPrint s_logger( "CustomProductControllerStateNetworkStandby" );

namespace ProductApp
{
CustomProductControllerStateNetworkStandby::CustomProductControllerStateNetworkStandby( ProductControllerHsm& hsm,
        CHsmState* pSuperState,
        Hsm::STATE stateId,
        const std::string& name ) :
    ProductControllerStateNetworkStandby( hsm, pSuperState, stateId, name )
{
    BOSE_DEBUG( s_logger, __func__ );
}

void CustomProductControllerStateNetworkStandby::HandleStateEnter()
{
    // For regulation, cap LCD display in standby state.
    GetCustomProductController().GetDisplayController()->SetStandbyLcdBrightnessCapEnabled( true );

    ProductControllerStateNetworkStandby::HandleStateEnter();
}

void CustomProductControllerStateNetworkStandby::HandleStateExit()
{
    GetCustomProductController().GetDisplayController()->SetStandbyLcdBrightnessCapEnabled( false );

    ProductControllerStateNetworkStandby::HandleStateExit();
}

}   // namespace ProductApp
