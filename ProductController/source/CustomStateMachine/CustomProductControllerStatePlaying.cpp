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
#include "CustomProductLpmHardwareInterface.h"
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
    BOSE_INFO( s_logger, "%s is being constructed.", name.c_str() );
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
    BOSE_INFO( s_logger, "%s is being entered.", GetName( ).c_str( ) );

    ///
    /// @todo Go to a full power state for the time being, until the new state machine is fully in
    ///       place for Professor. This will allow Professor to work with the latest state machine
    ///       in common code without influencing further development. Note that this method will be
    ///       removed once the changes are in place and the transition to full power will take place
    ///       in a playing transition state from the common code. This eventual change is covered
    ///       under the JIRA Story PGC-350.
    ///
    GetCustomProductController( ).GetLpmHardwareInterface( )->SetSystemState( SYSTEM_STATE_ON );

    BOSE_INFO( s_logger, "An attempt to set to full power is being made." );
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
/// @brief  CustomProductControllerStatePlaying::HandleIntentUserPower
///
/// @param  KeyHandlerUtil::ActionType_t action
///
/// @return
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductControllerStatePlaying::HandleIntentUserPower( KeyHandlerUtil::ActionType_t action )
{
    BOSE_INFO( s_logger, "%s in %s is handling key action %d.", GetName( ).c_str( ), __func__, action );

    GetProductController( ).SendStopPlaybackMessage( );
    GoToAppropriateNonPlayingState( );

    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief  CustomProductControllerStateOn::HandleIntentMuteControl
///
/// @param  action
///
/// @return This method returns a true Boolean value indicating that it has handled the action.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductControllerStatePlaying::HandleIntentMuteControl( KeyHandlerUtil::ActionType_t action )
{
    BOSE_INFO( s_logger, "%s in %s is handling the action %u", GetName( ).c_str( ), __FUNCTION__, action );

    GetCustomProductController( ).GetIntentHandler( ).Handle( action );

    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStatePlaying::GoToAppropriateNonPlayingState
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductControllerStatePlaying::GoToAppropriateNonPlayingState( )
{
    GetCustomProductController( ).GetCecHelper( )->PowerOff( );

    GetProductController( ).SendStopPlaybackMessage( );

    if( GetCustomProductController( ).IsNetworkConfigured( ) or
        GetCustomProductController( ).IsAutoWakeEnabled( ) )
    {
        if( GetCustomProductController( ).IsNetworkConnected( ) and
            GetCustomProductController( ).IsVoiceConfigured( ) )
        {
            BOSE_INFO( s_logger, "%s is changing to %s.",
                       GetName( ).c_str( ),
                       "CustomProductControllerStateIdleVoiceConfigured" );
            ChangeState( PRODUCT_CONTROLLER_STATE_IDLE_VOICE_CONFIGURED );
        }
        else
        {
            BOSE_INFO( s_logger, "%s is changing to %s.",
                       GetName( ).c_str( ),
                       "CustomProductControllerStateIdleVoiceNotConfigured" );
            ChangeState( PRODUCT_CONTROLLER_STATE_IDLE_VOICE_NOT_CONFIGURED );
        }
    }
    else
    {
        BOSE_INFO( s_logger, "%s is changing to %s.",
                   GetName( ).c_str( ),
                   "ProductControllerStateNetworkStandbyNotConfigured" );
        ChangeState( PRODUCT_CONTROLLER_STATE_NETWORK_STANDBY_NOT_CONFIGURED );
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief  CustomProductControllerStatePlaying::HandleLPMPowerStatusFullOn
///
/// @return This method returns a true Boolean value indicating that it has handled the power
///         status from the LPM.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductControllerStatePlaying::HandleLPMPowerStatusFullPowerOn( )
{
    GetCustomProductController( ).GetCecHelper( )->PowerOn( );
    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief  CustomProductControllerStatePlaying::HandleAdaptIQControl
///
/// @return This method returns a true Boolean value indicating that it has handled the
///         AdaptIQControl request.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductControllerStatePlaying::HandleAdaptIQControl( const ProductAdaptIQControl& cmd )
{
    if( cmd.action() == ProductAdaptIQControl::Start )
    {
        // TODO: does this need to be further qualified by anything (i.e. can't run if we're already
        // in another substate of Playing?)
        ChangeState( PROFESSOR_PRODUCT_CONTROLLER_STATE_ADAPTIQ );
        return true;
    }

    return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                           End of the Product Application Namespace                           ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
