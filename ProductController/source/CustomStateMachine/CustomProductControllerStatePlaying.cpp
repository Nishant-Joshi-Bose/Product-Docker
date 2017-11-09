////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file      CustomProductControllerStatePlaying.cpp
///
/// @brief     This source code file contains functionality to process events that occur during the
///            product playing state.
///
/// @author    Stuart J. Lumby
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
#include "Utilities.h"
#include "CustomProductControllerStatePlaying.h"
#include "ProductControllerHsm.h"
#include "ProductControllerState.h"
#include "ProductHardwareInterface.h"
#include "ProfessorProductController.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
///                            Start of Product Application Namespace                            ///
////////////////////////////////////////////////////////////////////////////////////////////////////
namespace ProductApp
{

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStatePlaying::CustomProductControllerStatePlaying
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
CustomProductControllerStatePlaying::CustomProductControllerStatePlaying
( ProductControllerHsm&       hsm,
  CHsmState*                  pSuperState,
  Hsm::STATE                  stateId,
  const std::string&          name )

    : ProductControllerState( hsm, pSuperState, stateId, name )
{
    BOSE_VERBOSE( s_logger, "CustomProductControllerStatePlaying is being constructed." );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStatePlaying::HandleStateEnter
///
/// @todo  A transition state may need to be added at this point to ensure that the power state
///        change occurs and to handle any error conditions.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductControllerStatePlaying::HandleStateEnter( )
{
    BOSE_VERBOSE( s_logger, "CustomProductControllerStatePlaying is being entered." );

    GetCustomProductController( ).GetHardwareInterface( )->RequestPowerStateFull( );

    BOSE_VERBOSE( s_logger, "An attempt to set to full power is being made." );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStatePlaying::HandleStateStart
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductControllerStatePlaying::HandleStateStart( )
{
    BOSE_VERBOSE( s_logger, "CustomProductControllerStatePlaying is being started." );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStatePlaying::HandleStateExit
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductControllerStatePlaying::HandleStateExit( )
{
    BOSE_VERBOSE( s_logger, "CustomProductControllerStatePlaying is being exited." );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief  CustomProductControllerStatePlaying::HandlePowerState
///
/// @return This method returns a true Boolean value indicating that it has handled the power
///         state changed and no futher processing will be required by any of its superstates.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductControllerStatePlaying::HandlePowerState( )
{
    GetProductController( ).SendStopPlaybackMessage( );

    if( GetCustomProductController( ).IsNetworkConfigured( ) or
        GetCustomProductController( ).IsAutoWakeEnabled( ) )
    {
        if( GetCustomProductController( ).IsNetworkConnected( ) and
            GetCustomProductController( ).IsVoiceConfigured( ) )
        {
            BOSE_VERBOSE( s_logger, "%s is changing to %s.",
                          "CustomProductControllerStatePlaying",
                          "CustomProductControllerStateIdleVoiceConfigured" );
            ChangeState( PROFESSOR_PRODUCT_CONTROLLER_STATE_IDLE_VOICE_CONFIGURED );
        }
        else
        {
            BOSE_VERBOSE( s_logger, "%s is changing to %s.",
                          "CustomProductControllerStatePlaying",
                          "CustomProductControllerStateIdleVoiceUnconfigured" );
            ChangeState( PROFESSOR_PRODUCT_CONTROLLER_STATE_IDLE_VOICE_UNCONFIGURED );
        }
    }
    else
    {
        BOSE_VERBOSE( s_logger, "%s is changing to %s.",
                      "CustomProductControllerStatePlaying",
                      "CustomProductControllerStateNetworkStandbyUnconfigured" );
        ChangeState( PROFESSOR_PRODUCT_CONTROLLER_STATE_NETWORK_STANDBY_UNCONFIGURED );
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
