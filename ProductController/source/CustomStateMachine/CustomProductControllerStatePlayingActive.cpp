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
/// @param ProductControllerHsm& hsm
///
/// @param CHsmState*            pSuperState
///
/// @param Hsm::STATE            stateId
///
/// @param const std::string&    name
///
////////////////////////////////////////////////////////////////////////////////////////////////////
CustomProductControllerStatePlayingActive::CustomProductControllerStatePlayingActive

( ProductControllerHsm&       hsm,
  CHsmState*                  pSuperState,
  Hsm::STATE                  stateId,
  const std::string&          name )

    : ProductControllerStatePlayingActive( hsm, pSuperState, stateId, name )
{
    BOSE_INFO( s_logger, "%s is being constructed.", name.c_str() );
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
( const ProductNowPlayingStatus_ProductNowPlayingState& state )
{
    BOSE_ERROR( s_logger, "%s is handling a now playing %s status.",
                GetName( ).c_str( ),
                ProductNowPlayingStatus_ProductNowPlayingState_Name( state ).c_str( ) );

    if( state == ProductNowPlayingStatus_ProductNowPlayingState_Inactive )
    {
        BOSE_INFO( s_logger, "%s is changing to %s.",
                   GetName( ).c_str( ),
                   "CustomProductControllerStatePlayingInactive" );
        ChangeState( PROFESSOR_PRODUCT_CONTROLLER_STATE_PLAYING_INACTIVE );
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief  CustomProductControllerStatePlayingActive::HandleIntent
///
/// @param  KeyHandlerUtil::ActionType_t action
///
/// @return This method returns a Boolean false value in case the key action needs to be handled in
///         one of its superstates.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductControllerStatePlayingActive::HandleIntent( KeyHandlerUtil::ActionType_t action )
{
    ProcessUserActivity( );
    return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief  CustomProductControllerStatePlayingActive::HandleIntentUserPower
///
/// @param  KeyHandlerUtil::ActionType_t action
///
/// @return This method returns a Boolean false value in case the key action needs to be handled in
///         one of its superstates.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductControllerStatePlayingActive::HandleIntentUserPower( KeyHandlerUtil::ActionType_t action )
{
    ProcessUserActivity( );
    return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief  CustomProductControllerStatePlayingActive::HandleIntentMuteControl
///
/// @param  KeyHandlerUtil::ActionType_t action
///
/// @return
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductControllerStatePlayingActive::HandleIntentMuteControl( KeyHandlerUtil::ActionType_t action )
{
    ProcessUserActivity( );
    return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief  CustomProductControllerStatePlayingActive::HandleIntentSpeakerPairing
///
/// @param  KeyHandlerUtil::ActionType_t action
///
/// @return
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductControllerStatePlayingActive::HandleIntentSpeakerPairing( KeyHandlerUtil::ActionType_t action )
{
    ProcessUserActivity( );
    return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStatePlayingActive::HandleIntentPlayback
///
/// @param KeyHandlerUtil::ActionType_t action
///
/// @return
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductControllerStatePlayingActive::HandleIntentPlayback( KeyHandlerUtil::ActionType_t action )
{
    ProcessUserActivity( );
    return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStatePlayingActive::ProcessUserActivity
///
/// @note  This private method is used to reset the timer based on some user key action that was
///         received.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductControllerStatePlayingActive::ProcessUserActivity( )
{
    BOSE_INFO( s_logger, "A key action arrived at %s.", GetName( ).c_str( ) );
    BOSE_INFO( s_logger, "The timer will be stopped and restarted based on user activity." );

    GetProductController( ).GetInactivityTimers( ).CancelTimer( InactivityTimerType::NO_USER_INTERACTION_TIMER );
    GetProductController( ).GetInactivityTimers( ).StartTimer( InactivityTimerType::NO_USER_INTERACTION_TIMER );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                           End of the Product Application Namespace                           ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
