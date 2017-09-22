////////////////////////////////////////////////////////////////////////////////
/// @file   EddieProductControllerStateSetup.cpp
/// @brief  The Setup State in the Eddie Product.
///
/// Copyright 2017 Bose Corporation
////////////////////////////////////////////////////////////////////////////////

#include "EddieProductControllerStateSetup.h"
#include "EddieProductControllerHsm.h"
#include "EddieProductController.h"
#include "DPrint.h"

static DPrint s_logger( "EddieProductControllerStateSetup" );

namespace ProductApp
{
EddieProductControllerStateSetup::EddieProductControllerStateSetup( EddieProductControllerHsm& hsm,
                                                                      CHsmState* pSuperState,
                                                                      EddieProductController& eddieProductController,
                                                                      STATE stateId,
                                                                      const std::string& name ) :
    ProductControllerStateSetup( hsm, pSuperState, eddieProductController, stateId, name )
{
}

void EddieProductControllerStateSetup::HandleStateEnter()
{
    BOSE_INFO( s_logger, __func__ );
}

void EddieProductControllerStateSetup::HandleStateStart()
{
    BOSE_INFO( s_logger, __func__ );
    // ToDo
    // GetProductController().SendActivateAccessPointCmd();
}

void EddieProductControllerStateSetup::HandleStateExit()
{
    BOSE_INFO( s_logger, __func__ );
    // ToDo
    //GetProductController().SendDeActivateAccessPointCmd();
}
} // namespace ProductApp
