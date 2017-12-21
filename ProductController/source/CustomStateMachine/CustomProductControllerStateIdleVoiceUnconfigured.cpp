////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file      CustomProductControllerStateIdleVoiceUnconfigured.cpp
///
/// @brief     This source code file contains functionality to process events that occur during the
///            product idle state when the voice for a Virtual Personal Assistant (VPA) is
///            unconfigured.
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
#include "CustomProductControllerStateIdleVoiceUnconfigured.h"
#include "ProductControllerHsm.h"
#include "ProfessorProductController.h"
#include "ProductControllerStateIdle.h"
#include "InactivityTimers.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
///                            Start of Product Application Namespace                            ///
////////////////////////////////////////////////////////////////////////////////////////////////////
namespace ProductApp
{


////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStateIdleVoiceUnconfigured::
///        CustomProductControllerStateIdleVoiceUnconfigured
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
CustomProductControllerStateIdleVoiceUnconfigured::CustomProductControllerStateIdleVoiceUnconfigured

( ProductControllerHsm&       hsm,
  CHsmState*                  pSuperState,
  Hsm::STATE                  stateId,
  const std::string&          name )

    : ProductControllerStateIdleVoiceNotConfigured( hsm, pSuperState, stateId, name )
{
    BOSE_VERBOSE( s_logger, "%s is being constructed.", name.c_str( ) );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStateIdleVoiceUnconfigured::HandleStateEnter
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductControllerStateIdleVoiceUnconfigured::HandleStateEnter( )
{
    if( not GetCustomProductController( ).IsAutoWakeEnabled( ) )
    {
        BOSE_VERBOSE( s_logger, "%s is being entered.", GetName( ).c_str( ) );
        BOSE_VERBOSE( s_logger, "NO_AUDIO_TIMER has been started" );

        GetProductController( ).GetInactivityTimers( ).StartTimer( InactivityTimerType::NO_AUDIO_TIMER );
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStateIdleVoiceUnconfigured::HandleStateExit
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductControllerStateIdleVoiceUnconfigured::HandleStateExit( )
{
    GetProductController( ).GetInactivityTimers( ).CancelTimer( InactivityTimerType::NO_AUDIO_TIMER );

    BOSE_VERBOSE( s_logger, "%s timer has been stopped.", GetName( ).c_str( ) );
    BOSE_VERBOSE( s_logger, "%s is being exited.", GetName( ).c_str( ) );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStateIdleVoiceUnconfigured::HandleInactivityTimer
///
/// @param InactivityTimerType timerType; only NO_AUDIO_TIMER expected here
///
/// @return This method returns a true Boolean value indicating that it has handled the timer expiration
///         unless the timer that had expired is an irrelevant timer
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductControllerStateIdleVoiceUnconfigured::HandleInactivityTimer( InactivityTimerType timerType )
{
    if( timerType != InactivityTimerType::NO_AUDIO_TIMER )
    {
        BOSE_ERROR( s_logger, "The timer %d is unexpected in %s.", timerType, GetName( ).c_str( ) );
        return false;
    }

    BOSE_VERBOSE( s_logger, "The timer %d in %s has expired.", timerType, GetName( ).c_str( ) );

    if( not GetCustomProductController( ).IsAutoWakeEnabled( ) )
    {
        if( GetCustomProductController( ).IsNetworkConfigured( ) )
        {
            BOSE_VERBOSE( s_logger, "%s is changing to %s.",
                          GetName( ).c_str( ),
                          "CustomProductControllerStateNetworkStandbyConfigured" );
            ChangeState( PROFESSOR_PRODUCT_CONTROLLER_STATE_NETWORK_STANDBY_CONFIGURED );
        }
        else
        {
            BOSE_VERBOSE( s_logger, "%s is changing to %s.",
                          GetName( ).c_str( ),
                          "CustomProductControllerStateNetworkStandbyUnconfigured" );
            ChangeState( PROFESSOR_PRODUCT_CONTROLLER_STATE_NETWORK_STANDBY_UNCONFIGURED );
        }
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief  CustomProductControllerStateIdleVoiceUnconfigured::HandleAutowakeStatus
///
/// @param  bool active
///
/// @return This method returns a true Boolean value indicating that it has handled the autowake
///         state change and no futher processing will be required by any of its superstates.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductControllerStateIdleVoiceUnconfigured::HandleAutowakeStatus( bool active )
{
    BOSE_VERBOSE( s_logger, "%s is handling an autowake %sactivation.",
                  GetName( ).c_str( ),
                  active ? "" : "de" );

    if( active )
    {
        GetProductController( ).GetInactivityTimers( ).CancelTimer( InactivityTimerType::NO_AUDIO_TIMER );
    }
    else
    {
        BOSE_VERBOSE( s_logger, "InactivityTimerType::NO_AUDIO_TIMER has been started" );

        GetProductController( ).GetInactivityTimers( ).StartTimer( InactivityTimerType::NO_AUDIO_TIMER );
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief  CustomProductControllerStateIdleVoiceUnconfigured::HandleNetworkState
///
/// @param  bool configured
///
/// @param  bool connected
///
/// @return This method returns a true Boolean value indicating that it has handled the network
///         state changed and no futher processing will be required by any of its superstates.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductControllerStateIdleVoiceUnconfigured::HandleNetworkState( bool configured,
                                                                            bool connected )
{
    BOSE_VERBOSE( s_logger, "%s is handling a %sconfigured, %sconnected network state event.",
                  GetName( ).c_str( ),
                  configured ? "" : "un",
                  connected  ? "" : "un" );

    GoToAppropriatePlayableState( GetCustomProductController( ).IsAutoWakeEnabled( ),
                                  configured,
                                  connected,
                                  GetCustomProductController().IsVoiceConfigured( ) );
    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief  CustomProductControllerStateIdleVoiceUnconfigured::HandleVoiceState
///
/// @param  bool configured
///
/// @return This method returns a true Boolean value indicating that it has handled the voice
///         state change and no futher processing will be required by any of its superstates.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductControllerStateIdleVoiceUnconfigured::HandleVoiceState( bool configured )
{
    BOSE_VERBOSE( s_logger, "%s is handling a %sconfigured voice state event.",
                  GetName( ).c_str( ),
                  configured ? "" : "un" );

    GoToAppropriatePlayableState( GetCustomProductController( ).IsAutoWakeEnabled( ),
                                  GetCustomProductController( ).IsNetworkConfigured( ),
                                  GetCustomProductController( ).IsNetworkConnected( ),
                                  configured );
    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStateIdleVoiceUnconfigured::GoToAppropriatePlayableState
///
/// @param bool autoWakeEnabled
///
/// @param bool networkConfigured
///
/// @param bool networkConnected
///
/// @param bool voiceConfigured
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductControllerStateIdleVoiceUnconfigured::GoToAppropriatePlayableState
( bool autoWakeEnabled,
  bool networkConfigured,
  bool networkConnected,
  bool voiceConfigured )
{
    if( not networkConfigured and not autoWakeEnabled )
    {
        BOSE_VERBOSE( s_logger, "%s is changing to %s.",
                      GetName( ).c_str( ),
                      "CustomProductControllerStateNetworkStandbyUnconfigured" );
        ChangeState( PROFESSOR_PRODUCT_CONTROLLER_STATE_NETWORK_STANDBY_UNCONFIGURED );
    }
    else if( networkConnected and voiceConfigured )
    {
        BOSE_VERBOSE( s_logger, "%s is changing to %s.",
                      GetName( ).c_str( ),
                      "CustomProductControllerStateIdleVoiceConfigured" );
        ChangeState( PROFESSOR_PRODUCT_CONTROLLER_STATE_IDLE_VOICE_CONFIGURED );
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
