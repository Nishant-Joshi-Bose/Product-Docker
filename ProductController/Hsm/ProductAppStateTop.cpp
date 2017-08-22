////////////////////////////////////////////////////////////////////////////////
/// @file   ProductAppStateTop.pp
/// @brief  The Top State in the Product Application HSM.  Any methods getting
///         called in this state indicate an unexpected call or an error.
///
/// Copyright 2017 Bose Corporation
////////////////////////////////////////////////////////////////////////////////

#include "ProductAppStateTop.h"
#include "ProductController.h"
#include "DPrint.h"

static DPrint s_logger( "ProductAppStateTop" );

namespace ProductApp
{
ProductAppStateTop::ProductAppStateTop( ProductAppHsm& hsm,
                                        CHsmState* pSuperState,
                                        ProductController& productController,
                                        STATE stateId,
                                        const std::string& name ) :
    ProductAppState( hsm, pSuperState, productController, stateId, name )
{
    BOSE_INFO( s_logger,  __func__ );
}

bool ProductAppStateTop::HandleModulesReady()
{
    BOSE_INFO( s_logger, "%s, %d", __func__, GetProductController().IsAllModuleReady() );
    return true;
}
} // namespace ProductApp
