////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file      CustomProductControllerStatePlayingInactive.cpp
///
/// @brief     This source code file contains functionality to process events that occur during the
///            product playing inactive state.
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
#include "CustomProductControllerStatePlayingInactive.h"
#include "InactivityTimers.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
///                            Start of Product Application Namespace                            ///
////////////////////////////////////////////////////////////////////////////////////////////////////
namespace ProductApp
{

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStatePlayingInactive::CustomProductControllerStatePlayingInactive
///
/// @param ProductControllerHsm& hsm
///
/// @param CHsmState*            pSuperState
///
/// @param Hsm::STATE            stateId
///
/// @param const std::string&    name
///
////////////////////////////////////////////////////////////////////////////////////////////////////
CustomProductControllerStatePlayingInactive::CustomProductControllerStatePlayingInactive

( ProductControllerHsm&       hsm,
  CHsmState*                  pSuperState,
  Hsm::STATE                  stateId,
  const std::string&          name )

    : ProductControllerState( hsm, pSuperState, stateId, name )
{
    BOSE_VERBOSE( s_logger, "%s is being constructed.", name.c_str() );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStatePlayingInactive::HandleStateEnter
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductControllerStatePlayingInactive::HandleStateEnter( )
{
    BOSE_VERBOSE( s_logger, "%s is being entered.", GetName( ).c_str( ) );
    BOSE_VERBOSE( s_logger, "NO_AUDIO_TIMER timer is started" );

    GetProductController( ).GetInactivityTimers( ).StartTimer( InactivityTimerType::NO_AUDIO_TIMER );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStatePlayingInactive::HandleStateStart
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductControllerStatePlayingInactive::HandleStateStart( )
{
    BOSE_VERBOSE( s_logger, "%s is being started.", GetName( ).c_str( ) );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStatePlayingInactive::HandleStateExit
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductControllerStatePlayingInactive::HandleStateExit( )
{
    BOSE_VERBOSE( s_logger, "%s is being exited.", GetName( ).c_str( ) );
    BOSE_VERBOSE( s_logger, "The timer will be stopped." );

    GetProductController( ).GetInactivityTimers( ).CancelTimer( InactivityTimerType::NO_AUDIO_TIMER );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief  CustomProductControllerStatePlayingInactive::HandleKeyAction
///
/// @param  int action
///
/// @return This method returns a false Boolean value in case processing of the key needs to be
///         handled by any of the superstates.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductControllerStatePlayingInactive::HandleKeyAction( int action )
{
    BOSE_VERBOSE( s_logger, "A key action arrived at %s.", GetName( ).c_str( ) );
    BOSE_VERBOSE( s_logger, "The timer will be stopped and restarted based on user activity." );

    GetProductController( ).GetInactivityTimers( ).CancelTimer( InactivityTimerType::NO_AUDIO_TIMER );
    GetProductController( ).GetInactivityTimers( ).StartTimer( InactivityTimerType::NO_AUDIO_TIMER );

    return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                           End of the Product Application Namespace                           ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
