////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file      CustomProductControllerStateLowPowerResume.cpp
///
/// @brief     This source code file contains functionality to process product specific events that
///            occur during the low power resume state.
///
/// @attention Copyright (C) 2019 Bose Corporation All Rights Reserved
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "CustomProductControllerStateLowPowerResume.h"
#include "CustomProductController.h"
#include "ProductTelemetry.pb.h"
#include "DPrint.h"

static DPrint s_logger( "CustomProductControllerStateLowPowerResume" );

namespace ProductApp
{

CustomProductControllerStateLowPowerResume::CustomProductControllerStateLowPowerResume( ProductControllerHsm& hsm,
        CHsmState* pSuperState,
        Hsm::STATE stateId,
        const std::string& name ) :
    ProductControllerStateLowPowerResume( hsm, pSuperState, stateId, name )
{
    BOSE_DEBUG( s_logger, __func__ );
}

void CustomProductControllerStateLowPowerResume::HandleStateEnter()
{
    auto& controller = GetCustomProductController();

    controller.GetTelemetry()->IncrementMetric<ProductTelemetry::Counters>( ProductTelemetry::Counters::LOW_POWER_RESUME );

    ProductControllerStateLowPowerResume::HandleStateEnter();
}

void CustomProductControllerStateLowPowerResume::HandleStateExit( )
{
    BOSE_INFO( s_logger, "State %s in %s", GetName( ).c_str( ), __func__ );

    // Get AUX cable state again as it might be missed during LowPowerState
    GetCustomProductController().RequestAuxCableState( );

    ProductControllerStateLowPowerResume::HandleStateExit();
}

} /// namespace ProductApp
