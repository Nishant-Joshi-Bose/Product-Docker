////////////////////////////////////////////////////////////////////////////////
/// @file   CustomProductControllerStateOn.cpp
/// @brief  The On State in the product controller.
///
/// Copyright 2017 Bose Corporation
////////////////////////////////////////////////////////////////////////////////

#include "CustomProductControllerStateOn.h"
#include "ProductControllerHsm.h"
#include "CustomProductController.h"
#include "DPrint.h"

static DPrint s_logger( "CustomProductControllerStateOn" );

namespace ProductApp
{
CustomProductControllerStateOn::CustomProductControllerStateOn( ProductControllerHsm& hsm,
                                                                CHsmState* pSuperState,
                                                                Hsm::STATE stateId,
                                                                const std::string& name ) :
    ProductControllerStateOn( hsm, pSuperState, stateId, name )
{
    BOSE_DEBUG( s_logger, __func__ );
}

bool CustomProductControllerStateOn::HandleIntentAuxIn( KeyHandlerUtil::ActionType_t intent )
{
    BOSE_LOG( DEBUG, "intent:" << CommonIntentHandler::GetIntentName( intent ) );

    GetCustomProductController().GetIntentHandler().Handle( intent );

    return true;
}

bool CustomProductControllerStateOn::HandleIntentCountDown( KeyHandlerUtil::ActionType_t intent )
{
    BOSE_LOG( DEBUG, "intent:" << CommonIntentHandler::GetIntentName( intent ) );

    GetCustomProductController().GetIntentHandler().Handle( intent );

    return true;
}

} /// namespace ProductApp
