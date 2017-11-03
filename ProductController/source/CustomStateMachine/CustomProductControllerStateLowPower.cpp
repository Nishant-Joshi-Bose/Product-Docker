////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file      CustomProductControllerStateLowPower.cpp
///
/// @brief     This source code file contains functionality to process events that occur during a
///            low product state.
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
#include "Utilities.h"
#include "CustomProductControllerStateLowPower.h"
#include "ProductControllerHsm.h"
#include "ProductControllerStateIdle.h"
#include "ProductHardwareInterface.h"
#include "ProfessorProductController.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
///                            Start of Product Application Namespace                            ///
////////////////////////////////////////////////////////////////////////////////////////////////////
namespace ProductApp
{

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStateLowPower::CustomProductControllerStateLowPower
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
CustomProductControllerStateLowPower::CustomProductControllerStateLowPower
( ProductControllerHsm&       hsm,
  CHsmState*                  pSuperState,
  Hsm::STATE                  stateId,
  const std::string&          name )

    : ProductControllerState( hsm, pSuperState, stateId, name )
{
    BOSE_VERBOSE( s_logger, "CustomProductControllerStateLowPower is being constructed." );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStateLowPower::HandleStateEnter
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductControllerStateLowPower::HandleStateEnter( )
{
    BOSE_VERBOSE( s_logger, "CustomProductControllerStateLowPower is being entered." );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStateLowPower::HandleStateStart
///
/// @todo  A transition state may need to be added at this point to ensure that the power state
///        change occurs and to handle any error conditions.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductControllerStateLowPower::HandleStateStart( )
{
    BOSE_VERBOSE( s_logger, "CustomProductControllerStateLowPower is being started." );

    GetCustomProductController().GetHardwareInterface( )->RequestPowerStateOff( );

    BOSE_VERBOSE( s_logger, "An attempt to set the device to a low power state is being made." );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStateLowPower::HandleStateExit
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductControllerStateLowPower::HandleStateExit( )
{
    BOSE_VERBOSE( s_logger, "CustomProductControllerStateLowPower is being exited." );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief  CustomProductControllerStateLowPower::HandlePowerState
///
/// @return This method returns a true Boolean value indicating that it has handled the power
///         state changed and no futher processing will be required by any of its superstates.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductControllerStateLowPower::HandlePowerState( )
{
    BOSE_VERBOSE( s_logger, "%s is changing to %s.",
                  "CustomProductControllerStateLowPower",
                  "CustomProductControllerStatePlayable" );
    ChangeState( PROFESSOR_PRODUCT_CONTROLLER_STATE_PLAYABLE );

    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                           End of the Product Application Namespace                           ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
