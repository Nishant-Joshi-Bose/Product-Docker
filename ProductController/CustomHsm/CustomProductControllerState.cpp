/////////////////////////////////////////////////////////////////////////////
/// @file   CustomProductControllerState.cpp
/// @brief  Class for executing product specific event handlers based on
///         product specific events. Home for all product specific event
///         handlers that should be performed within product specific/custom
///         states.
///
/// Copyright 2017 Bose Corporation
////////////////////////////////////////////////////////////////////////////
#include "CustomProductControllerState.h"
#include "ProductControllerHsm.h"
#include "DPrint.h"

static DPrint s_logger( "CustomProductControllerState" );

namespace ProductApp
{
CustomProductControllerState::CustomProductControllerState( ProductControllerHsm& hsm,
                                                            CHsmState* pSuperState,
                                                            STATE stateId,
                                                            const std::string& name ) :
    CHsmState( stateId, &hsm, name, pSuperState )
{
    BOSE_INFO( s_logger, __func__ );
}

bool CustomProductControllerState::HandleCustomEvent()
{
    BOSE_INFO( s_logger, __func__ );
    return true;
}
} // namespace ProductApp
