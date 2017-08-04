////////////////////////////////////////////////////////////////////////////////
/// @file   ProductAppStateStandby.h
/// @brief  The Standby State in the Product Application HSM. This state is
//         active on as long as the product is asked to play something or
//         receives any other event that requires state transition.
///
/// Copyright 2017 Bose Corporation
////////////////////////////////////////////////////////////////////////////////

#include "ProductAppStateStandby.h"
#include "DPrint.h"

static DPrint s_logger( "ProductAppStateStandby" );

namespace ProductApp
{
ProductAppStateStandby::ProductAppStateStandby( ProductAppHsm& hsm,
                                                CHsmState* pSuperState,
                                                ProductController& productController,
                                                STATE stateId,
                                                const std::string& name ) :
    ProductAppState( hsm, pSuperState, productController, stateId, name )
{
}

void ProductAppStateStandby::HandleStateEnter()
{
    BOSE_INFO( s_logger, __func__ );
}

void ProductAppStateStandby::HandleStateStart()
{
    BOSE_INFO( s_logger, __func__ );
}

void ProductAppStateStandby::HandleStateExit()
{
    BOSE_INFO( s_logger, __func__ );
}
} // namespace ProductApp
