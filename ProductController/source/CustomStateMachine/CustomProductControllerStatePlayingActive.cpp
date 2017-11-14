////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file      CustomProductControllerStatePlayingActive.cpp
///
/// @brief     This source code file contains functionality to process events that occur during the
///            product playing active state.
///
/// @author    Stuart J. Lumby
///
/// @todo      There are two timers to consider. Four hours for no user action and 20 minutes for
///            no audio being rendered, which are handled by this state and the custom state
///            CustomProductControllerStatePlayingInactive. There is the possibility where audio is
///            not playing and in this state, where audio is simply playing packets comprised of all
///            0s. This may need to be discussed with the Audio Path team to determine how to fully
///            implement this functionality.
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
#include "ProductControllerHsm.h"
#include "ProfessorProductController.h"
#include "ProductControllerState.h"
#include "CustomProductControllerStatePlayingActive.h"
#include "InactivityTimers.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
///                            Start of Product Application Namespace                            ///
////////////////////////////////////////////////////////////////////////////////////////////////////
namespace ProductApp
{

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStatePlayingActive::CustomProductControllerStatePlayingActive
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
CustomProductControllerStatePlayingActive::CustomProductControllerStatePlayingActive

( ProductControllerHsm&       hsm,
  CHsmState*                  pSuperState,
  ProfessorProductController& productController,
  Hsm::STATE                  stateId,
  const std::string&          name )

    : ProductControllerState( hsm, pSuperState, stateId, name )
{
    BOSE_VERBOSE( s_logger, "%s is being constructed.", name.c_str() );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStatePlayingActive::HandleStateEnter
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductControllerStatePlayingActive::HandleStateEnter( )
{
    BOSE_VERBOSE( s_logger, "%s is being entered.", GetName( ).c_str( ) );
    BOSE_VERBOSE( s_logger, "NO_USER_INTERACTION_TIMER timer is started" );

    GetProductController( ).GetInactivityTimers( ).StartTimer( InactivityTimerType::NO_USER_INTERACTION_TIMER );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStatePlayingActive::HandleStateStart
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductControllerStatePlayingActive::HandleStateStart( )
{
    BOSE_VERBOSE( s_logger, "%s is being started.", GetName( ).c_str( ) );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStatePlayingActive::HandleStateExit
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductControllerStatePlayingActive::HandleStateExit( )
{
    BOSE_VERBOSE( s_logger, "%s is being exited.", GetName( ).c_str( ) );
    BOSE_VERBOSE( s_logger, "The timer will be stopped." );

    GetProductController( ).GetInactivityTimers( ).CancelTimer( InactivityTimerType::NO_USER_INTERACTION_TIMER );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief  CustomProductControllerStatePlayingActive::HandleNowPlayingStatus
///
/// @param  ProductNowPlayingStatus_ProductNowPlayingState state
///
/// @return This method returns a true Boolean value indicating that it has handled the playback
///         status and no futher processing will be required by any of its superstates.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductControllerStatePlayingActive::HandleNowPlayingStatus
( ProductNowPlayingStatus_ProductNowPlayingState state )
{
    BOSE_ERROR( s_logger, "%s is handling a now playing %s status.",
                GetName( ).c_str( ),
                ProductNowPlayingStatus_ProductNowPlayingState_Name( state ).c_str( ) );

    if( state == ProductNowPlayingStatus_ProductNowPlayingState_Inactive )
    {
        BOSE_VERBOSE( s_logger, "%s is changing to %s.",
                      GetName( ).c_str( ),
                      "CustomProductControllerStatePlayingInactive" );
        ChangeState( PROFESSOR_PRODUCT_CONTROLLER_STATE_PLAYING_INACTIVE );
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief  CustomProductControllerStatePlayingActive::HandleKeyAction
///
/// @param  int action
///
/// @return This method returns a false Boolean value in case processing of the key needs to be
///         handled by any of the superstates.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductControllerStatePlayingActive::HandleKeyAction( int action )
{
    BOSE_VERBOSE( s_logger, "A key action arrived at %s.", GetName( ).c_str( ) );
    BOSE_VERBOSE( s_logger, "The timer will be stopped and restarted based on user activity." );

    GetProductController( ).GetInactivityTimers( ).CancelTimer( InactivityTimerType::NO_USER_INTERACTION_TIMER );
    GetProductController( ).GetInactivityTimers( ).StartTimer( InactivityTimerType::NO_USER_INTERACTION_TIMER );

    return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                           End of the Product Application Namespace                           ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
