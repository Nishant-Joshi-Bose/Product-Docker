////////////////////////////////////////////////////////////////////////////////
/// @file   ProductAppStateTop.pp
/// @brief  The Top State in the Product Application HSM.  Any methods getting
///         called in this state indicate an unexpected call or an error.
///
/// Copyright 2017 Bose Corporation
////////////////////////////////////////////////////////////////////////////////

#include "ProductAppStateTop.h"
#include "DPrint.h"

static DPrint s_logger( "ProductAppStateTop" );

namespace ProductApp
{
ProductAppStateTop::ProductAppStateTop( ProductAppHsm& hsm,
                                        CHsmState* pSuperState,
                                        ProductController& productController,
                                        STATE stateId,
                                        const std::string& name ) :
    ProductAppState( hsm, pSuperState, productController, stateId, name )
{
    BOSE_INFO( s_logger,  __func__ );
}

bool ProductAppStateTop::HandleSetupEndPoint( SoundTouchInterface::msg_Header const& cookie, std::string const& body, std::string const& operation )
{
    BOSE_INFO( s_logger, "%s, %s", __func__, operation.c_str() );

    return true;
}
} // namespace ProductApp
