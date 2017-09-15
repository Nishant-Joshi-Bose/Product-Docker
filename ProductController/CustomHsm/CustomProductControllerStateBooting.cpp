////////////////////////////////////////////////////////////////////////////////
/// @file   CustomProductControllerStateBooting.h
/// @brief  The product specific 'Booting' state.
///
/// Copyright 2017 Bose Corporation
////////////////////////////////////////////////////////////////////////////////

#include "CustomProductControllerStateBooting.h"
#include "ProductControllerHsm.h"
#include "HelloWorldProductController.h"
#include "DPrint.h"

static DPrint s_logger( "CustomProductControllerStateBooting" );

namespace ProductApp
{
CustomProductControllerStateBooting::CustomProductControllerStateBooting( ProductControllerHsm& hsm,
                                                                          CHsmState* pSuperState,
                                                                          HelloWorldProductController& helloWorldProductController,
                                                                          STATE stateId,
                                                                          const std::string& name ) :
    ProductControllerStateBooting( hsm, pSuperState, helloWorldProductController, stateId, name )
{
    BOSE_INFO( s_logger, __func__ );
}

void CustomProductControllerStateBooting::HandleStateEnter()
{
    BOSE_INFO( s_logger, __func__ );
}

void CustomProductControllerStateBooting::HandleStateStart()
{
    BOSE_INFO( s_logger, __func__ );
}

void CustomProductControllerStateBooting::HandleStateExit()
{
    BOSE_INFO( s_logger, __func__ );
}

bool CustomProductControllerStateBooting::HandleLanguageRequest()
{
    BOSE_INFO( s_logger, __func__ );

    //Call common product controller Booting state function and perform common product actions.
    ProductControllerStateBooting::HandleLanguageRequest();

    //Do product specific state transitions here.
    ChangeState( PRODUCT_CONTROLLER_STATE_SETUP );

    return true;
}
} // namespace ProductApp
