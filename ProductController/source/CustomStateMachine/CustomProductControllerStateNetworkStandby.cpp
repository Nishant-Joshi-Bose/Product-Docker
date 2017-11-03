////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file      CustomProductControllerStateNetworkStandby.cpp
///
/// @brief     This source code file contains functionality to process events that occur during the
///            product network standby state.
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
#include "ProductControllerHsm.h"
#include "ProductHardwareInterface.h"
#include "ProfessorProductController.h"
#include "CustomProductControllerStateNetworkStandby.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
///                            Start of Product Application Namespace                            ///
////////////////////////////////////////////////////////////////////////////////////////////////////
namespace ProductApp
{

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStateNetworkStandby::CustomProductControllerStateNetworkStandby
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
CustomProductControllerStateNetworkStandby::CustomProductControllerStateNetworkStandby

( ProductControllerHsm&       hsm,
  CHsmState*                  pSuperState,
  Hsm::STATE                  stateId,
  const std::string&          name )

    : ProductControllerStateNetworkStandby( hsm, pSuperState, stateId, name )
{
    BOSE_VERBOSE( s_logger, "CustomProductControllerStateNetworkStandby is being constructed." );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStateNetworkStandby::HandleStateEnter
///
/// @todo  A transition state may need to be added at this point to ensure that the power state
///        change occurs and to handle any error conditions.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductControllerStateNetworkStandby::HandleStateEnter()
{
    BOSE_VERBOSE( s_logger, "CustomProductControllerStateNetworkStandby is being entered." );

    GetCustomProductController().GetHardwareInterface( )->RequestPowerStateStandby( );

    BOSE_VERBOSE( s_logger, "An attempt to set a standby power state is being made." );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStateNetworkStandby::HandleStateStart
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductControllerStateNetworkStandby::HandleStateStart()
{
    BOSE_VERBOSE( s_logger, "CustomProductControllerStateNetworkStandby is being started." );

    auto const& networkConfigured = GetCustomProductController().IsNetworkConfigured( );

    if( networkConfigured )
    {
        BOSE_VERBOSE( s_logger, "%s is changing to %s.",
                      "CustomProductControllerStateNetworkStandby",
                      "CustomProductControllerStateNetworkStandbyUnconfigured" );
        ChangeState( PROFESSOR_PRODUCT_CONTROLLER_STATE_NETWORK_STANDBY_CONFIGURED );
    }
    else
    {
        BOSE_VERBOSE( s_logger, "%s is changing to %s.",
                      "CustomProductControllerStateNetworkStandby",
                      "CustomProductControllerStateNetworkStandbyConfigured" );
        ChangeState( PROFESSOR_PRODUCT_CONTROLLER_STATE_NETWORK_STANDBY_UNCONFIGURED );
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStateNetworkStandby::HandleStateExit
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductControllerStateNetworkStandby::HandleStateExit()
{
    BOSE_VERBOSE( s_logger, "CustomProductControllerStateNetworkStandby is being exited." );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief  CustomProductControllerStateNetworkStandby::HandleAutowakeStatus
///
/// @param  bool active
///
/// @return This method returns a true Boolean value indicating that it has handled the autowake
///         state change and no futher processing will be required by any of its superstates.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductControllerStateNetworkStandby::HandleAutowakeStatus( bool active )
{
    BOSE_VERBOSE( s_logger, "CustomProductControllerStateNetworkStandby is handling the autowake status." );

    if( active )
    {
        BOSE_VERBOSE( s_logger, "%s is changing %s.",
                      "CustomProductControllerStateNetworkStandby",
                      "CustomProductControllerStateIdle" );
        ChangeState( PROFESSOR_PRODUCT_CONTROLLER_STATE_IDLE );
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                           End of the Product Application Namespace                           ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
