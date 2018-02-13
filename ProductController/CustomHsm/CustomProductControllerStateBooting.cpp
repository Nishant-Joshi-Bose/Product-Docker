////////////////////////////////////////////////////////////////////////////////
/// @file   CustomProductControllerStateBooting.cpp
/// @brief  The Booting State in the Eddie Product.
///
/// Copyright 2017 Bose Corporation
////////////////////////////////////////////////////////////////////////////////

#include "CustomProductControllerStateBooting.h"
#include "ProductControllerHsm.h"
#include "EddieProductController.h"
#include "DPrint.h"

static DPrint s_logger( "CustomProductControllerStateBooting" );

namespace ProductApp
{
CustomProductControllerStateBooting::CustomProductControllerStateBooting( ProductControllerHsm& hsm,
                                                                          CHsmState* pSuperState,
                                                                          Hsm::STATE stateId,
                                                                          const std::string& name ) :
    ProductControllerStateBooting( hsm, pSuperState, stateId, name )
{
    BOSE_INFO( s_logger, __func__ );
}

void CustomProductControllerStateBooting::HandleStateExit()
{
    BOSE_INFO( s_logger, __func__ );
    ProductControllerStateBooting::HandleStateExit();
    GetProductController().GetLpmHardwareInterface()->SetAmp( /*powered=*/ true, /*muted=*/ false );
}

} /// namespace ProductApp
