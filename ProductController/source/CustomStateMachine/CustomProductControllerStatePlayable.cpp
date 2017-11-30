////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file      CustomProductControllerStatePlayable.cpp
///
/// @brief     This source code file contains functionality to process events that occur during the
///            product playable state.
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
#include "CustomProductControllerStatePlayable.h"
#include "ProductControllerHsm.h"
#include "ProfessorProductController.h"
#include "ProductControllerState.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
///                            Start of Product Application Namespace                            ///
////////////////////////////////////////////////////////////////////////////////////////////////////
namespace ProductApp
{

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStatePlayable::CustomProductControllerStatePlayable
///
/// @param  ProductControllerHsm& hsm
///
/// @param  CHsmState*            pSuperState
///
/// @param  Hsm::STATE            stateId
///
/// @param  const std::string&    name
///
////////////////////////////////////////////////////////////////////////////////////////////////////
CustomProductControllerStatePlayable::CustomProductControllerStatePlayable( ProductControllerHsm& hsm,
                                                                            CHsmState*            pSuperState,
                                                                            Hsm::STATE            stateId,
                                                                            const std::string&    name )
    : ProductControllerState( hsm, pSuperState, stateId, name )
{
    BOSE_VERBOSE( s_logger, "CustomProductControllerStatePlayable is being constructed." );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStatePlayable::HandleStateEnter
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductControllerStatePlayable::HandleStateEnter( )
{
    BOSE_VERBOSE( s_logger, "CustomProductControllerStatePlayable is being entered." );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStatePlayable::HandleStateStart
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductControllerStatePlayable::HandleStateStart( )
{
    BOSE_VERBOSE( s_logger, "CustomProductControllerStatePlayable is being started." );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStatePlayable::HandleStateExit
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductControllerStatePlayable::HandleStateExit( )
{
    BOSE_VERBOSE( s_logger, "CustomProductControllerStatePlayable is being exited." );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief  CustomProductControllerStatePlayable::HandleNowSelectionInfo
///
///
/// @param  const SoundTouchInterface::NowSelectionInfo&
///
/// @return This method returns a true Boolean value indicating that it has handled the event
///         and no futher processing will be required by any of its superstates.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductControllerStatePlayable::HandleNowSelectionInfo
( const  SoundTouchInterface::NowSelectionInfo&  nowSelectionInfo )
{
    BOSE_VERBOSE( s_logger, "%s is changing to %s.",
                  "CustomProductControllerStatePlayable",
                  "CustomProductControllerStatePlayingInactive" );
    ChangeState( PROFESSOR_PRODUCT_CONTROLLER_STATE_PLAYING_INACTIVE );

    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief  CustomProductControllerStatePlayable::HandleKeyAction
///
/// @param  int action
///
/// @return This method returns a true Boolean value indicating that it has handled the key action
///         or false if the key has not been handled.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductControllerStatePlayable::HandleKeyAction( int action )
{
    bool handled = false;

    BOSE_INFO( s_logger, "CustomProductControllerStatePlayable is handling key action %d.", action );

    switch( action )
    {
    case KeyActionPb::KEY_ACTION_POWER:
        ///
        /// Start a playback based on the currently selected source. Once the playback starts, the
        /// state machine should receive a now selecting event from the product controller, which
        /// would cause the state machine to transition to a playing state.
        ///
        GetCustomProductController( ).SendPlaybackRequest( GetCustomProductController( ).GetCurrentSource( ) );

        handled = true;
        break;

    default:
        break;
    }

    return handled;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                           End of the Product Application Namespace                           ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
