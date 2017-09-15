////////////////////////////////////////////////////////////////////////////////
/// @file   CustomProductControllerStateSwUpdating.h
/// @brief  The product specific 'Software Updating' state.
///
/// Copyright 2017 Bose Corporation
////////////////////////////////////////////////////////////////////////////////

#include "CustomProductControllerStateSwUpdating.h"
#include "DPrint.h"

static DPrint s_logger( "CustomProductControllerStateSoftwareUpdating" );

namespace ProductApp
{
CustomProductControllerStateSwUpdating::CustomProductControllerStateSwUpdating( ProductControllerHsm& hsm,
                                                                                CHsmState* pSuperState,
                                                                                ProductController& productController,
                                                                                STATE stateId,
                                                                                const std::string& name ) :
    ProductControllerState( hsm, pSuperState, productController, stateId, name )
{
    BOSE_INFO( s_logger, __func__ );
}

void CustomProductControllerStateSwUpdating::HandleStateEnter()
{
    BOSE_INFO( s_logger, __func__ );
}

void CustomProductControllerStateSwUpdating::HandleStateStart()
{
    BOSE_INFO( s_logger, __func__ );
}

void CustomProductControllerStateSwUpdating::HandleStateExit()
{
    BOSE_INFO( s_logger, __func__ );
}
} // namespace ProductApp
