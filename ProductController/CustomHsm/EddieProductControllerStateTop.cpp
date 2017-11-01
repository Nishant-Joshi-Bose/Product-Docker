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
                                                                Hsm::STATE stateId,
                                                                const std::string& name ) :
    ProductControllerStateTop( hsm, pSuperState, stateId, name )
{
    BOSE_INFO( s_logger,  __func__ );
}

bool EddieProductControllerStateTop::HandleModulesReady()
{
    BOSE_INFO( s_logger, "%s, %d", __func__,  GetCustomProductController().IsAllModuleReady() );
    return true;
}

bool EddieProductControllerStateTop::HandleIntents( KeyHandlerUtil::ActionType_t result )
{
    return true;
}

} // namespace ProductApp
