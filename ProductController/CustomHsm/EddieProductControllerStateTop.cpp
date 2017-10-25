////////////////////////////////////////////////////////////////////////////////
/// @file   EddieProductControllerStateTop.cpp
/// @brief  The Top State in the Eddie Product.  Any methods getting
///         called in this state indicate an unexpected call or an error.
///
/// Copyright 2017 Bose Corporation
////////////////////////////////////////////////////////////////////////////////

#include "EddieProductControllerStateTop.h"
#include "EddieProductControllerHsm.h"
#include "EddieProductController.h"
#include "DPrint.h"

static DPrint s_logger( "EddieProductControllerStateTop" );

namespace ProductApp
{
EddieProductControllerStateTop::EddieProductControllerStateTop( EddieProductControllerHsm& hsm,
                                                                CHsmState* pSuperState,
                                                                EddieProductController& eddieProductController,
                                                                Hsm::STATE stateId,
                                                                const std::string& name ) :
    ProductControllerStateTop( hsm, pSuperState, eddieProductController, stateId, name )
{
    BOSE_INFO( s_logger,  __func__ );
}

bool EddieProductControllerStateTop::HandleModulesReady()
{
    BOSE_INFO( s_logger, "%s, %d", __func__, static_cast<EddieProductController&>( GetProductController() ).IsAllModuleReady() );
    return true;
}

bool EddieProductControllerStateTop::HandleIntents( KeyHandlerUtil::ActionType_t result )
{
    return true;
}

bool EddieProductControllerStateTop::HandleNetworkModuleStatus( const NetManager::Protobuf::NetworkStatus& networkStatus, int profileSize )
{
    if( profileSize || networkStatus.isprimaryup() )
    {
        ChangeState( CUSTOM_PRODUCT_CONTROLLER_STATE_NETWORK_STANDBY );
    }
    else
    {
        ChangeState( CUSTOM_PRODUCT_CONTROLLER_STATE_SETUP );
    }
    return true;
}

} // namespace ProductApp
