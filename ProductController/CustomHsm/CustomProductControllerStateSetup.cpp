////////////////////////////////////////////////////////////////////////////////
/// @file   CustomProductControllerStateSetup.cpp
/// @brief  The Setup State in the Eddie Product.
///
/// Copyright 2017 Bose Corporation
////////////////////////////////////////////////////////////////////////////////

#include "CustomProductControllerStateSetup.h"
#include "ProductControllerHsm.h"
#include "EddieProductController.h"
#include "DPrint.h"
#include "LpmInterface.h"

static DPrint s_logger( "CustomProductControllerStateSetup" );

namespace ProductApp
{
CustomProductControllerStateSetup::CustomProductControllerStateSetup( ProductControllerHsm& hsm,
                                                                      CHsmState* pSuperState,
                                                                      Hsm::STATE stateId,
                                                                      const std::string& name ) :
    ProductControllerStateSetup( hsm, pSuperState, stateId, name )
{
    BOSE_INFO( s_logger, __func__ );
}

bool CustomProductControllerStateSetup::HandleIntents( KeyHandlerUtil::ActionType_t intent )
{
    BOSE_LOG( INFO, "intent:" << intent );

    if( ( GetCustomProductController( ).GetIntentHandler( ).IsIntentAuxIn( intent ) ) ||
        ( GetCustomProductController( ).GetIntentHandler( ).IsIntentBlueTooth( intent ) ) )
    {
        GetCustomProductController().GetIntentHandler().Handle( intent );
        return true;
    }
    return false;
}

} /// namespace ProductApp
