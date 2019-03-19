////////////////////////////////////////////////////////////////////////////////
/// @file   CustomProductControllerStateTop.cpp
/// @brief  The Top State in the product controller.  Any methods getting
///         called in this state indicate an unexpected call or an error.
///
/// Copyright 2017 Bose Corporation
////////////////////////////////////////////////////////////////////////////////

#include "CustomProductControllerStateTop.h"
#include "ProductControllerHsm.h"
#include "CustomProductController.h"
#include "DPrint.h"

static DPrint s_logger( "CustomProductControllerStateTop" );

namespace ProductApp
{
CustomProductControllerStateTop::CustomProductControllerStateTop( ProductControllerHsm& hsm,
                                                                  CHsmState* pSuperState,
                                                                  Hsm::STATE stateId,
                                                                  const std::string& name ) :
    ProductControllerStateTop( hsm, pSuperState, stateId, name )
{
    BOSE_DEBUG( s_logger,  __func__ );
}

bool CustomProductControllerStateTop::HandleModulesReady()
{
    BOSE_INFO( s_logger, "%s, %d", __func__,  GetCustomProductController().IsAllModuleReady() );
    return true;
}

bool CustomProductControllerStateTop::HandleIntentAuxIn( KeyHandlerUtil::ActionType_t intent )
{
    return true;
}

bool CustomProductControllerStateTop::HandleNowSelectionInfo( const SoundTouchInterface::NowSelectionInfo& info )
{
    BOSE_DEBUG( s_logger,  __func__ );
    return true;
}


bool CustomProductControllerStateTop::HandleIntentCountDown( KeyHandlerUtil::ActionType_t intent )
{
    return true;
}

} // namespace ProductApp
