////////////////////////////////////////////////////////////////////////////////
/// @file   CustomProductControllerStateSoftwareInstall.cpp
/// @brief  The SoftwareInstall in Eddie Product.
///
/// Copyright 2018 Bose Corporation
////////////////////////////////////////////////////////////////////////////////

#include "CustomProductControllerStateSoftwareInstall.h"
#include "ProductControllerHsm.h"
#include "CustomProductController.h"
#include "DPrint.h"

static DPrint s_logger( "CustomProductControllerStateSoftwareInstall" );

namespace ProductApp
{
CustomProductControllerStateSoftwareInstall::CustomProductControllerStateSoftwareInstall( ProductControllerHsm& hsm,
        CHsmState* pSuperState,
        Hsm::STATE stateId,
        const std::string& name ) :
    ProductControllerStateSoftwareInstall( hsm, pSuperState, stateId, name )
{
    BOSE_INFO( s_logger, __func__ );
}

void CustomProductControllerStateSoftwareInstall::HandleStateStart()
{
    ///Turn OFF LCD display controller
    BOSE_INFO( s_logger, "Turn LCD display OFF in %s.", __func__ );
    GetCustomProductController().GetDisplayController()->RequestTurnDisplayOnOff( false );

    ProductControllerStateSoftwareInstall::HandleStateStart();
}

} /// namespace ProductApp
