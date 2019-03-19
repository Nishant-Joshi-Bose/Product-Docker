////////////////////////////////////////////////////////////////////////////////
/// @file   CustomProductControllerStateLowPowerStandbyTransition.cpp
/// @brief  Custom override state in the product controller for transitioning
/// into and out of low power standby.
///
/// Copyright 2017 Bose Corporation
////////////////////////////////////////////////////////////////////////////////

#include "CustomProductControllerStateLowPowerStandbyTransition.h"
#include "ProductControllerHsm.h"
#include "CustomProductController.h"
#include "DPrint.h"
#include "AsyncCallback.h"

static DPrint s_logger( "CustomProductControllerStateLowPowerStandbyTransition" );

namespace ProductApp
{
CustomProductControllerStateLowPowerStandbyTransition::CustomProductControllerStateLowPowerStandbyTransition( ProductControllerHsm& hsm,
        CHsmState* pSuperState,
        Hsm::STATE stateId,
        const std::string& name ) :
    ProductControllerStateLowPowerStandbyTransition( hsm, pSuperState, stateId, name ),
    m_displayControllerIsReady( false )
{
    BOSE_DEBUG( s_logger, __func__ );
}

void CustomProductControllerStateLowPowerStandbyTransition::HandleStateStart()
{
    BOSE_DEBUG( s_logger, __func__ );

    m_displayControllerIsReady = false;

    // Turn OFF LCD display controller.
    AsyncCallback<void> dcReadyCb( std::bind( &CustomProductControllerStateLowPowerStandbyTransition::SetDisplayControllerIsReady, this ),
                                   GetProductController( ).GetTask() );
    GetCustomProductController().GetDisplayController()->RequestTurnDisplayOnOff( false, dcReadyCb );

    ProductControllerStateLowPowerStandbyTransition::HandleStateStart();
}

/*!
 */
bool CustomProductControllerStateLowPowerStandbyTransition::IsReadyForLowPowerState() const
{
    return ( ProductControllerStateLowPowerStandbyTransition::IsReadyForLowPowerState()
             // Wait for display controller to return success of display off (asynchronous)
             && m_displayControllerIsReady );
}

void CustomProductControllerStateLowPowerStandbyTransition::SetDisplayControllerIsReady()
{
    m_displayControllerIsReady = true;

    PossiblyGoToNextState();
}

} /// namespace ProductApp
