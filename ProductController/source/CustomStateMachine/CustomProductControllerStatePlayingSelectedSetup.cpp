////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file      CustomProductControllerStatePlayingSelectedSetup.cpp
/// @brief     This source code file contains functionality to process events
///            that occur in Professor during the product setup state.
///
/// @attention Copyright (C) 2018 Bose Corporation All Rights Reserved
///
///            Bose Corporation
///            The Mountain Road,
///            Framingham, MA 01701-9168
///            U.S.A.
///
///            This program may not be reproduced, in whole or in part, in any form by any means
///            whatsoever without the written permission of Bose Corporation.
///
////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////
///
///            Included Header Files
///
////////////////////////////////////////////////////////////////////////////////////////////////////
#include "CustomProductControllerStatePlayingSelectedSetup.h"
#include "ProductControllerStates.h"
#include "ProductController.h"
#include "DPrint.h"

static DPrint s_logger( "Product" );

////////////////////////////////////////////////////////////////////////////////////////////////////
///                            Start of Product Application Namespace                            ///
////////////////////////////////////////////////////////////////////////////////////////////////////
namespace ProductApp
{

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStatePlayingSelectedSetup::CustomProductControllerStatePlayingSelectedSetup
///
////////////////////////////////////////////////////////////////////////////////////////////////////
CustomProductControllerStatePlayingSelectedSetup::CustomProductControllerStatePlayingSelectedSetup(
    ProductControllerHsm& hsm,
    CHsmState* pSuperState,
    Hsm::STATE stateId,
    const std::string& name ) :
    ProductControllerStatePlayingSelectedSetup( hsm, pSuperState, stateId, name )
{
    BOSE_INFO( s_logger, __func__ );
}

bool CustomProductControllerStatePlayingSelectedSetup::HandleIntentMuteControl( KeyHandlerUtil::ActionType_t intent )
{
    BOSE_LOG( DEBUG, "intent is ignored in Setup " << intent );
    return true;
}

} /// namespace ProductApp
