////////////////////////////////////////////////////////////////////////////////
/// @file   ProductAppStateStdOp.h
/// @brief  The StdOp State in the Product Application HSM. This state is
///         the home of common operations for example key presses.
///
/// Copyright 2017 Bose Corporation
////////////////////////////////////////////////////////////////////////////////

#include "ProductAppStateStdOp.h"
#include "DPrint.h"

static DPrint s_logger( "ProductAppStateStdOp" );

namespace ProductApp
{
ProductAppStateStdOp::ProductAppStateStdOp( ProductAppHsm& hsm,
                                            CHsmState* pSuperState,
                                            ProductController& productController,
                                            STATE stateId,
                                            const std::string& name ) :
    ProductAppState( hsm, pSuperState, productController, stateId, name )
{
}

void ProductAppStateStdOp::HandleStateEnter()
{
    BOSE_INFO( s_logger, __func__ );
}

void ProductAppStateStdOp::HandleStateStart()
{
    BOSE_INFO( s_logger, __func__ );
}

void ProductAppStateStdOp::HandleStateExit()
{
    BOSE_INFO( s_logger, __func__ );
}

bool ProductAppStateStdOp::HandleSetupEndPoint( SoundTouchInterface::msg_Header const& cookie, std::string const& body, std::string const& operation )
{
    BOSE_INFO( s_logger, "%s, %s", __func__, operation.c_str() );

    ChangeState( PRODUCT_APP_STATE_SETUP );

    return true;
}
} // namespace ProductApp
