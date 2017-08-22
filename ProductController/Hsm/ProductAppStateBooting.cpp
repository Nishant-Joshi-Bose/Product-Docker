////////////////////////////////////////////////////////////////////////////////
/// @file   ProductAppStateBooting.h
/// @brief  The Booting State in the Product Application HSM. This state is
///         active on until other software components are initialized.
///
/// Copyright 2017 Bose Corporation
////////////////////////////////////////////////////////////////////////////////

#include "ProductAppStateBooting.h"
#include "ProductController.h"
#include "DPrint.h"

static DPrint s_logger( "ProductAppStateBooting" );

namespace ProductApp
{
ProductAppStateBooting::ProductAppStateBooting( ProductAppHsm& hsm,
                                                CHsmState* pSuperState,
                                                ProductController& productController,
                                                STATE stateId,
                                                const std::string& name ) :
    ProductAppState( hsm, pSuperState, productController, stateId, name )
{
}

void ProductAppStateBooting::HandleStateEnter()
{
    BOSE_INFO( s_logger, __func__ );
}

void ProductAppStateBooting::HandleStateStart()
{
    BOSE_INFO( s_logger, __func__ );
    HandleModulesReady();
}

void ProductAppStateBooting::HandleStateExit()
{
    BOSE_INFO( s_logger, __func__ );
}

bool ProductAppStateBooting::HandleModulesReady()
{
    if( GetProductController().IsAllModuleReady() )
    {
        if( GetHsm().NeedsToBeSetup() )
        {
            ChangeState( PRODUCT_APP_STATE_SETUP );
        }
        else
        {
            ChangeState( PRODUCT_APP_STATE_STANDBY );
        }
    }
    return true;
}

} // namespace ProductApp
