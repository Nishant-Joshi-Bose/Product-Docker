////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file      CustomProductControllerStateOn.cpp
///
/// @brief     This source code file contains functionality to process events that occur during the
///            product on state.
///
/// @author    Stuart J. Lumby
///
/// @date      10/24/2017
///
/// @attention Copyright (C) 2017 Bose Corporation All Rights Reserved
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
#include "DPrint.h"
#include "KeyActions.h"
#include "Utilities.h"
#include "CustomProductControllerStateOn.h"
#include "ProductControllerHsm.h"
#include "ProfessorProductController.h"
#include "ProductControllerState.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
///                            Start of Product Application Namespace                            ///
////////////////////////////////////////////////////////////////////////////////////////////////////
namespace ProductApp
{

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStateOn::CustomProductControllerStateOn
///
/// @param hsm
///
/// @param pSuperState
///
/// @param productController
///
/// @param stateId
///
/// @param name
///
////////////////////////////////////////////////////////////////////////////////////////////////////
CustomProductControllerStateOn::CustomProductControllerStateOn( ProductControllerHsm&       hsm,
                                                                CHsmState*                  pSuperState,
                                                                Hsm::STATE                  stateId,
                                                                const std::string&          name )

    : ProductControllerState( hsm, pSuperState, stateId, name )
{
    BOSE_VERBOSE( s_logger, "CustomProductControllerStateOn is being constructed." );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStateOn::HandleStateEnter
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductControllerStateOn::HandleStateEnter( )
{
    BOSE_VERBOSE( s_logger, "CustomProductControllerStateOn is being entered." );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStateOn::HandleStateStart
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductControllerStateOn::HandleStateStart( )
{
    BOSE_VERBOSE( s_logger, "%s is being started and going to %s.",
                  "CustomProductControllerStateOn",
                  "CustomProductControllerStatePlayable" );

    ChangeState( PROFESSOR_PRODUCT_CONTROLLER_STATE_PLAYABLE );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStateOn::HandleStateExit
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductControllerStateOn::HandleStateExit( )
{
    BOSE_VERBOSE( s_logger, "CustomProductControllerStateOn is being exited." );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief  CustomProductControllerStateOn::HandleKeyAction
///
/// @param  int action
///
/// @return This method returns a true Boolean value indicating that it has handled the key action
///         and false if the key has not been handled
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductControllerStateOn::HandleKeyAction( int action )
{
    BOSE_INFO( s_logger, "A key action was sent to the product state on with action %d.", action );
    switch( action )
    {
    case KEY_ACTION_PAIR_SPEAKERS:
        ChangeState( PROFESSOR_PRODUCT_CONTROLLER_STATE_ACCESSORY_PAIRING );
        return true;
    default:
        return false;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                           End of the Product Application Namespace                           ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
