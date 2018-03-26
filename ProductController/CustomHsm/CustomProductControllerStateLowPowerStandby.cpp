////////////////////////////////////////////////////////////////////////////////
/// @file   CustomProductControllerStateLowPowerStandby.cpp
/// @brief  The Low Power Standby State in the Eddie Product.
///
/// Copyright 2017 Bose Corporation
////////////////////////////////////////////////////////////////////////////////

#include "CustomProductControllerStateLowPowerStandby.h"
#include "ProductControllerHsm.h"
#include "EddieProductController.h"
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
    BOSE_INFO( s_logger, __func__ );
}

void CustomProductControllerStateLowPowerStandby::HandleStateStart()
{
    BOSE_INFO( s_logger, __func__ );

    ///Turn OFF LCD display controller
    BOSE_INFO( s_logger, "Turn LCD display OFF in %s.", __func__ );
    GetCustomProductController().TurnDisplayOnOff( false );

    ProductControllerStateLowPowerStandby::HandleStateStart();

    ///Turn ON LCD display controller
    BOSE_INFO( s_logger, "Turn LCD display ON in %s.", __func__ );
    GetCustomProductController().TurnDisplayOnOff( true );
}

} /// namespace ProductApp
