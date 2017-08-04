////////////////////////////////////////////////////////////////////////////////
/// @file   ProductAppStateSetup.h
/// @brief  The Setup State in the Product Application HSM. This state is
///         active on until the product is setup.
///
/// Copyright 2017 Bose Corporation
////////////////////////////////////////////////////////////////////////////////

#include "ProductAppStateSetup.h"
#include "DPrint.h"

static DPrint s_logger( "ProductAppStateSetup" );

namespace ProductApp
{
ProductAppStateSetup::ProductAppStateSetup( ProductAppHsm& hsm,
                                            CHsmState* pSuperState,
                                            ProductController& productController,
                                            STATE stateId,
                                            const std::string& name ) :
    ProductAppState( hsm, pSuperState, productController, stateId, name )
{
}

void ProductAppStateSetup::HandleStateEnter()
{
    BOSE_INFO( s_logger, __func__ );
}

void ProductAppStateSetup::HandleStateStart()
{
    BOSE_INFO( s_logger, __func__ );
}

void ProductAppStateSetup::HandleStateExit()
{
    BOSE_INFO( s_logger, __func__ );
}
} // namespace ProductApp
