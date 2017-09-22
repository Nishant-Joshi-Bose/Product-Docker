////////////////////////////////////////////////////////////////////////////////
/// @file   EddieProductControllerStateBooting.cpp
/// @brief  The Booting State in the Eddie Product.
///
/// Copyright 2017 Bose Corporation
////////////////////////////////////////////////////////////////////////////////

#include "EddieProductControllerStateBooting.h"
#include "EddieProductControllerHsm.h"
#include "EddieProductController.h"
#include "DPrint.h"

static DPrint s_logger( "EddieProductControllerStateBooting" );

namespace ProductApp
{
EddieProductControllerStateBooting::EddieProductControllerStateBooting( EddieProductControllerHsm& hsm,
                                                                          CHsmState* pSuperState,
                                                                          EddieProductController& eddieProductController,
                                                                          STATE stateId,
                                                                          const std::string& name ) :
    ProductControllerStateBooting( hsm, pSuperState, eddieProductController, stateId, name )
{
    BOSE_INFO( s_logger, __func__ );
}

void EddieProductControllerStateBooting::HandleStateEnter()
{
    BOSE_INFO( s_logger, __func__ );
}

void EddieProductControllerStateBooting::HandleStateStart()
{
    BOSE_INFO( s_logger, __func__ );
}

void EddieProductControllerStateBooting::HandleStateExit()
{
    BOSE_INFO( s_logger, __func__ );
}

bool EddieProductControllerStateBooting::HandleModulesReady()
{
    BOSE_INFO( s_logger, __func__ );
    if( static_cast<EddieProductController&>( GetProductController() ).IsAllModuleReady() )
    {
        if( static_cast<EddieProductControllerHsm&>( GetHsm() ).IsProductNeedsSetup() )
        {
            ChangeState( CUSTOM_PRODUCT_CONTROLLER_STATE_SETUP );
        }
        else
        {
            ChangeState( CUSTOM_PRODUCT_CONTROLLER_STATE_NETWORK_STANDBY );
        }
    }
    return true;
}

} // namespace ProductApp
