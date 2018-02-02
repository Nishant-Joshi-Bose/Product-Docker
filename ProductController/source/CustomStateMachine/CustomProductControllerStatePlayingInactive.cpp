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

    : ProductControllerStatePlayingInactive( hsm, pSuperState, stateId, name )
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
/// @brief  CustomProductControllerStatePlayingInactive::HandleNowPlayingStatus
///
/// @param  ProductNowPlayingStatus_ProductNowPlayingState state
///
/// @return This method returns a true Boolean value indicating that it has handled the playback
///         status and no futher processing will be required by any of its superstates.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductControllerStatePlayingInactive::HandleNowPlayingStatus
( const ProductNowPlayingStatus_ProductNowPlayingState& state )
{
    BOSE_ERROR( s_logger, "%s is handling a now playing %s status.",
                GetName( ).c_str( ),
                ProductNowPlayingStatus_ProductNowPlayingState_Name( state ).c_str( ) );

    if( state == ProductNowPlayingStatus_ProductNowPlayingState_Active )
    {
        BOSE_VERBOSE( s_logger, "%s is changing to %s.",
                      GetName( ).c_str( ),
                      "CustomProductControllerStatePlayingActive" );
        ChangeState( PROFESSOR_PRODUCT_CONTROLLER_STATE_PLAYING_ACTIVE );
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief  CustomProductControllerStatePlayingInactive::HandleIntent
///
/// @param  KeyHandlerUtil::ActionType_t action
///
/// @return This method returns a Boolean false value in case the key action needs to be handled in
///         one of its superstates.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductControllerStatePlayingInactive::HandleIntent( KeyHandlerUtil::ActionType_t action )
{
    ProcessUserActivity( );
    return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief  CustomProductControllerStatePlayingInactive::HandleIntentUserPower
///
/// @param  KeyHandlerUtil::ActionType_t action
///
/// @return This method returns a Boolean false value in case the key action needs to be handled in
///         one of its superstates.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductControllerStatePlayingInactive::HandleIntentUserPower( KeyHandlerUtil::ActionType_t action )
{
    ProcessUserActivity( );
    return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief  CustomProductControllerStatePlayingInactive::HandleIntentMuteControl
///
/// @param  KeyHandlerUtil::ActionType_t action
///
/// @return This method returns a Boolean false value in case the key action needs to be handled in
///         one of its superstates.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductControllerStatePlayingInactive::HandleIntentMuteControl( KeyHandlerUtil::ActionType_t action )
{
    ProcessUserActivity( );
    return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief  CustomProductControllerStatePlayingInactive::HandleIntentSpeakerPairing
///
/// @param  KeyHandlerUtil::ActionType_t action
///
/// @return This method returns a Boolean false value in case the key action needs to be handled in
///         one of its superstates.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductControllerStatePlayingInactive::HandleIntentSpeakerPairing( KeyHandlerUtil::ActionType_t action )
{
    ProcessUserActivity( );
    return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief  CustomProductControllerStatePlayingInactive::HandleIntentPlayback
///
/// @param  KeyHandlerUtil::ActionType_t action
///
/// @return This method returns a Boolean false value in case the key action needs to be handled in
///         one of its superstates.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductControllerStatePlayingInactive::HandleIntentPlayback( KeyHandlerUtil::ActionType_t action )
{
    ProcessUserActivity( );
    return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief  CustomProductControllerStatePlayingInactive::ProcessUserActivity
///
/// @note  This private method is used to reset the timer based on some user key action that was
///         received.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductControllerStatePlayingInactive::ProcessUserActivity( )
{
    BOSE_VERBOSE( s_logger, "A key action arrived at %s.", GetName( ).c_str( ) );
    BOSE_VERBOSE( s_logger, "The timer will be stopped and restarted based on user activity." );

    GetProductController( ).GetInactivityTimers( ).CancelTimer( InactivityTimerType::NO_AUDIO_TIMER );
    GetProductController( ).GetInactivityTimers( ).StartTimer( InactivityTimerType::NO_AUDIO_TIMER );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                           End of the Product Application Namespace                           ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
