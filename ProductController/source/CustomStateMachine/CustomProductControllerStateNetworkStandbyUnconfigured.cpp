////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file      CustomProductControllerStateNetworkStandbyUnconfigured.cpp
///
/// @brief     This source code file contains functionality to process events that occur during the
///            product network standby unconfigured state.
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
#include "ProductControllerHsm.h"
#include "ProductHardwareInterface.h"
#include "CustomProductControllerStateNetworkStandbyUnconfigured.h"
#include "ProfessorProductController.h"
#include "InactivityTimers.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
///                            Start of Product Application Namespace                            ///
////////////////////////////////////////////////////////////////////////////////////////////////////
namespace ProductApp
{


////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStateNetworkStandbyUnconfigured::
///        CustomProductControllerStateNetworkStandbyUnconfigured
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
CustomProductControllerStateNetworkStandbyUnconfigured::
CustomProductControllerStateNetworkStandbyUnconfigured

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
/// @brief CustomProductControllerStateNetworkStandbyUnconfigured::HandleStateEnter
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductControllerStateNetworkStandbyUnconfigured::HandleStateEnter()
{
    BOSE_VERBOSE( s_logger, "%s is being entered.", GetName( ).c_str( ) );
    BOSE_VERBOSE( s_logger, "NO_NETWORK_CONFIGURED_TIMER timer is started" );

    GetProductController( ).GetInactivityTimers( ).StartTimer( InactivityTimerType::NO_NETWORK_CONFIGURED_TIMER );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStateNetworkStandbyUnconfigured::HandleStateStart
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductControllerStateNetworkStandbyUnconfigured::HandleStateStart()
{
    BOSE_VERBOSE( s_logger, "%s is being started.", GetName( ).c_str( ) );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStateNetworkStandbyUnconfigured::HandleStateExit
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductControllerStateNetworkStandbyUnconfigured::HandleStateExit()
{
    BOSE_VERBOSE( s_logger, "%s is being exited.", GetName( ).c_str( ) );
    BOSE_VERBOSE( s_logger, "The timer will be stopped." );

    GetProductController( ).GetInactivityTimers( ).CancelTimer( InactivityTimerType::NO_NETWORK_CONFIGURED_TIMER );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStateNetworkStandbyUnconfigured::HandleInactivityTimer
///
/// @param InactivityTimerType timerType; only NO_NETWORK_CONFIGURED_TIMER expected here
///
/// @return This method returns a true Boolean value indicating that it has handled the timer expiration
///         unless the timer that had expired is an irrelevant timer
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductControllerStateNetworkStandbyUnconfigured::HandleInactivityTimer( InactivityTimerType timerType )
{
    if( timerType != InactivityTimerType::NO_NETWORK_CONFIGURED_TIMER )
    {
        BOSE_ERROR( s_logger, "The timer %d is unexpected in %s.", timerType, GetName( ).c_str( ) );
        return false;
    }

    BOSE_VERBOSE( s_logger, "The timer %d in %s has expired.", timerType, GetName( ).c_str( ) );
    BOSE_VERBOSE( s_logger, "An attempt to set the device to a low power state will be made." );

    ChangeState( PROFESSOR_PRODUCT_CONTROLLER_STATE_LOW_POWER );

    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief  CustomProductControllerStateNetworkStandbyUnconfigured::HandleNetworkState
///
/// @param  bool configured
///
/// @param  bool connected
///
/// @return This method returns a true Boolean value indicating that it has handled the network
///         state change and no futher processing will be required by any of its superstates.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductControllerStateNetworkStandbyUnconfigured::HandleNetworkState( bool configured, bool connected )
{
    BOSE_VERBOSE( s_logger, "%s is handling a %sconfigured, %sconnected network state event.",
                  GetName( ).c_str( ),
                  configured ? "" : "un",
                  connected  ? "" : "un" );

    GoToAppropriatePlayableState( configured,
                                  connected,
                                  GetCustomProductController().IsVoiceConfigured( ) );
    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief  CustomProductControllerStateNetworkStandbyUnconfigured::HandleVoiceState
///
/// @param  bool configured
///
/// @return This method returns a true Boolean value indicating that it has handled the voice
///         state change and no futher processing will be required by any of its superstates.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductControllerStateNetworkStandbyUnconfigured::HandleVoiceState( bool configured )
{
    BOSE_VERBOSE( s_logger, "%s is handling a %sconfigured voice state event.",
                  GetName( ).c_str( ),
                  configured ? "" : "un" );

    GoToAppropriatePlayableState( GetCustomProductController( ).IsNetworkConfigured( ),
                                  GetCustomProductController( ).IsNetworkConnected( ),
                                  configured );
    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStateNetworkStandbyUnconfigured::GoToAppropriatePlayableState
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductControllerStateNetworkStandbyUnconfigured::GoToAppropriatePlayableState
( bool networkConfigured,
  bool networkConnected,
  bool voiceConfigured )
{
    if( networkConnected and voiceConfigured )
    {
        BOSE_VERBOSE( s_logger, "%s is changing to %s.",
                      GetName( ).c_str( ),
                      "CustomProductControllerStateIdleVoiceConfigured" );
        ChangeState( PROFESSOR_PRODUCT_CONTROLLER_STATE_IDLE_VOICE_CONFIGURED );
    }
    else if( networkConfigured )
    {
        BOSE_VERBOSE( s_logger, "%s is changing to %s.",
                      GetName( ).c_str( ),
                      "CustomProductControllerStateNetworkStandbyUnconfigured" );
        ChangeState( PROFESSOR_PRODUCT_CONTROLLER_STATE_NETWORK_STANDBY_CONFIGURED );
    }
    else
    {
        BOSE_VERBOSE( s_logger, "%s is not changing.", GetName( ).c_str( ) );
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                           End of the Product Application Namespace                           ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
