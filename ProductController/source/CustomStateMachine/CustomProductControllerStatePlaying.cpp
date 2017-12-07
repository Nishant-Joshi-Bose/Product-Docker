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
/// @param ProductControllerHsm& hsm
///
/// @param CHsmState*            pSuperState
///
/// @param Hsm::STATE            stateId
///
/// @param const std::string&    name
///
////////////////////////////////////////////////////////////////////////////////////////////////////
CustomProductControllerStatePlaying::CustomProductControllerStatePlaying
( ProductControllerHsm&       hsm,
  CHsmState*                  pSuperState,
  Hsm::STATE                  stateId,
  const std::string&          name )

    : ProductControllerStatePlaying( hsm, pSuperState, stateId, name )
{
    BOSE_VERBOSE( s_logger, "%s is being constructed.", name.c_str() );
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
    BOSE_VERBOSE( s_logger, "%s is being entered.", GetName( ).c_str( ) );

    GetCustomProductController( ).GetHardwareInterface( )->RequestLpmSystemState( SYSTEM_STATE_ON );

    BOSE_VERBOSE( s_logger, "An attempt to set to full power is being made." );
}


////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStatePlaying::HandleStateExit
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductControllerStatePlaying::HandleStateExit( )
{
    BOSE_VERBOSE( s_logger, "%s is being exited.", GetName( ).c_str( ) );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStatePlayingActive::HandleInactivityTimer
///
/// @param InactivityTimerType timerType; only NO_USER_INTERACTION_TIMER or NO_AUDIO_TIMER
///        expected here
///
/// @return This method returns a true Boolean value indicating that it has handled the timer expiration
///         unless the timer that had expired is an irrelevant timer
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductControllerStatePlaying::HandleInactivityTimer( InactivityTimerType timerType )
{
    if( timerType != InactivityTimerType::NO_USER_INTERACTION_TIMER &&
        timerType != InactivityTimerType::NO_AUDIO_TIMER )
    {
        BOSE_ERROR( s_logger, "The timer %d is unexpected in %s.", timerType, GetName( ).c_str( ) );
        return false;
    }

    GoToAppropriateNonPlayingState( );

    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief  CustomProductControllerStatePlaying::HandleKeyAction
///
/// @param  int action
///
/// @return This method returns a true Boolean value indicating that it has handled the key action
///         or false if the key has not been handled.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductControllerStatePlaying::HandleKeyAction( int action )
{
    bool handled = false;

    BOSE_INFO( s_logger, "%s is handling key action %d.", GetName( ).c_str( ), action );

    switch( action )
    {
    case KeyActionPb::KEY_ACTION_POWER:
        ///
        /// Stop the playback based on the currently selected source. Transition to the appropriate
        /// non-playing state.
        ///
        GetProductController( ).SendStopPlaybackMessage( );
        GoToAppropriateNonPlayingState( );
        handled = true;
        break;

    default:
        break;
    }

    return handled;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStatePlaying::GoToAppropriateNonPlayingState
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductControllerStatePlaying::GoToAppropriateNonPlayingState( )
{
    GetCustomProductController( ).GetEdidInterface( )->PowerOff( );

    GetProductController( ).SendStopPlaybackMessage( );

    if( GetCustomProductController( ).IsNetworkConfigured( ) or
        GetCustomProductController( ).IsAutoWakeEnabled( ) )
    {
        if( GetCustomProductController( ).IsNetworkConnected( ) and
            GetCustomProductController( ).IsVoiceConfigured( ) )
        {
            BOSE_VERBOSE( s_logger, "%s is changing to %s.",
                          GetName( ).c_str( ),
                          "CustomProductControllerStateIdleVoiceConfigured" );
            ChangeState( PROFESSOR_PRODUCT_CONTROLLER_STATE_IDLE_VOICE_CONFIGURED );
        }
        else
        {
            BOSE_VERBOSE( s_logger, "%s is changing to %s.",
                          GetName( ).c_str( ),
                          "CustomProductControllerStateIdleVoiceUnconfigured" );
            ChangeState( PROFESSOR_PRODUCT_CONTROLLER_STATE_IDLE_VOICE_UNCONFIGURED );
        }
    }
    else
    {
        BOSE_VERBOSE( s_logger, "%s is changing to %s.",
                      GetName( ).c_str( ),
                      "CustomProductControllerStateNetworkStandbyUnconfigured" );
        ChangeState( PROFESSOR_PRODUCT_CONTROLLER_STATE_NETWORK_STANDBY_UNCONFIGURED );
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief  CustomProductControllerStatePlaying::HandleLPMPowerStatusFull
///
/// @return This method returns a true Boolean value indicating that it has handled the power
///         status from the LPM.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductControllerStatePlaying::HandleLPMPowerStatusFullPower( )
{
    GetCustomProductController( ).GetEdidInterface( )->PowerOn( );
    return true;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
///                           End of the Product Application Namespace                           ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
