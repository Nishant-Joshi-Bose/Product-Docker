////////////////////////////////////////////////////////////////////////////////
/// @file   EddieProductControllerStateNetworkStandby.cpp
/// @brief  The Network Standby State in the Eddie Product.
///
/// Copyright 2017 Bose Corporation
////////////////////////////////////////////////////////////////////////////////

#include "EddieProductControllerStateNetworkStandby.h"
#include "EddieProductControllerHsm.h"
#include "EddieProductController.h"
#include "DPrint.h"

static DPrint s_logger( "EddieProductControllerStateNetworkStandby" );

namespace ProductApp
{
EddieProductControllerStateNetworkStandby::EddieProductControllerStateNetworkStandby( EddieProductControllerHsm& hsm,
        CHsmState* pSuperState,
        EddieProductController& eddieProductController,
        Hsm::STATE stateId,
        const std::string& name ) :
    ProductControllerStateNetworkStandby( hsm, pSuperState, eddieProductController, stateId, name )
{
    BOSE_INFO( s_logger, __func__ );
}

void EddieProductControllerStateNetworkStandby::HandleStateEnter()
{
    BOSE_INFO( s_logger, __func__ );
}

void EddieProductControllerStateNetworkStandby::HandleStateStart()
{
    BOSE_INFO( s_logger, __func__ );
}

void EddieProductControllerStateNetworkStandby::HandleStateExit()
{
    BOSE_INFO( s_logger, __func__ );
}

bool EddieProductControllerStateNetworkStandby::HandleIntents( KeyHandlerUtil::ActionType_t result )
{
    BOSE_DEBUG( s_logger, "%s, %d", __func__, ( uint16_t ) result );

    IntentHandler& IHandle =
        static_cast<EddieProductController&>( GetProductController() ).IntentHandle();
    if( ( IHandle.isIntentPlayControl( result ) ) ||
        ( IHandle.IsIntentBlueTooth( result ) )   ||
        ( IHandle.IsIntentAlexa( result ) )        ||
        ( IHandle.IsIntentVolumeControl( result ) ) )
    {
        IHandle.Handle( result );
    }
    return false;
}

} // namespace ProductApp
