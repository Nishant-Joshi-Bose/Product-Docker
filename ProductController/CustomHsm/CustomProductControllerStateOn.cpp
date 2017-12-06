////////////////////////////////////////////////////////////////////////////////
/// @file   CustomProductControllerStateOn.cpp
/// @brief  The On State in the Eddie Product.
///
/// Copyright 2017 Bose Corporation
////////////////////////////////////////////////////////////////////////////////

#include "CustomProductControllerStateOn.h"
#include "ProductControllerHsm.h"
#include "EddieProductController.h"
#include "DPrint.h"
#include "LpmInterface.h"

static DPrint s_logger( "CustomProductControllerStateOn" );

namespace ProductApp
{
CustomProductControllerStateOn::CustomProductControllerStateOn( ProductControllerHsm& hsm,
                                                                CHsmState* pSuperState,
                                                                Hsm::STATE stateId,
                                                                const std::string& name ) :
    ProductControllerStateOn( hsm, pSuperState, stateId, name )
{
    BOSE_INFO( s_logger, __func__ );
}

bool CustomProductControllerStateOn::HandleIntents( KeyHandlerUtil::ActionType_t intent )
{
    BOSE_LOG( INFO, "intent:" << intent );

    if( ( GetCustomProductController( ).GetIntentHandler( ).IsIntentAuxIn( intent ) ) ||
        ( GetCustomProductController( ).GetIntentHandler( ).IsIntentVoice( intent ) ) )
    {
        GetCustomProductController().GetIntentHandler().Handle( intent );
        return true;
    }
    return ( ProductControllerStateOn::HandleIntents( intent ) );
}

} /// namespace ProductApp
