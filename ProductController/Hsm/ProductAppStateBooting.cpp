////////////////////////////////////////////////////////////////////////////////
/// @file   ProductAppStateBooting.h
/// @brief  The Booting State in the Product Application HSM. This state is
///         active on until other software components are initialized.
///
/// Copyright 2017 Bose Corporation
////////////////////////////////////////////////////////////////////////////////

#include "ProductAppStateBooting.h"
#include "DPrint.h"

static DPrint s_logger( "ProductAppStateBooting" );

namespace ProductApp
{
ProductAppStateBooting::ProductAppStateBooting( ProductAppHsm& hsm,
                                                CHsmState* pSuperState,
                                                ProductController& productController,
                                                STATE stateId,
                                                const std::string& name ) :
    ProductAppState( hsm, pSuperState, productController, stateId, name )
{
}

void ProductAppStateBooting::HandleStateEnter()
{
    BOSE_INFO( s_logger, __func__ );
}

void ProductAppStateBooting::HandleStateStart()
{
    BOSE_INFO( s_logger, __func__ );
}

void ProductAppStateBooting::HandleStateExit()
{
    BOSE_INFO( s_logger, __func__ );
}

bool ProductAppStateBooting::HandleSetupEndPoint( SoundTouchInterface::msg_Header const& cookie, std::string const& body, std::string const& operation )
{
    BOSE_INFO( s_logger, "%s, %s", __func__, operation.c_str() );

    ChangeState( PRODUCT_APP_STATE_STDOP );

    return true;
}

} // namespace ProductApp
