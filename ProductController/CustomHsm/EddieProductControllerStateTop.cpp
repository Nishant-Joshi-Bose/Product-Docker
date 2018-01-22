////////////////////////////////////////////////////////////////////////////////
/// @file   EddieProductControllerStateTop.cpp
/// @brief  The Top State in the Eddie Product.  Any methods getting
///         called in this state indicate an unexpected call or an error.
///
/// Copyright 2017 Bose Corporation
////////////////////////////////////////////////////////////////////////////////

#include "EddieProductControllerStateTop.h"
#include "ProductControllerHsm.h"
#include "EddieProductController.h"
#include "DPrint.h"

static DPrint s_logger( "EddieProductControllerStateTop" );

namespace ProductApp
{
EddieProductControllerStateTop::EddieProductControllerStateTop( ProductControllerHsm& hsm,
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

bool EddieProductControllerStateTop::HandleIntentAuxIn( KeyHandlerUtil::ActionType_t intent )
{
    return true;
}

bool EddieProductControllerStateTop::HandleNowSelectionInfo( const SoundTouchInterface::NowSelectionInfo& info )
{
    BOSE_INFO( s_logger,  __func__ );
    return true;
}


bool EddieProductControllerStateTop::HandleCountDownManager( KeyHandlerUtil::ActionType_t intent )
{
    return true;
}

} // namespace ProductApp
