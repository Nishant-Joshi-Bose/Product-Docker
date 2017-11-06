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
/// @param  ProductControllerHsm&       hsm
///
/// @param  CHsmState*                  pSuperState
///
/// @param  ProfessorProductController& productController
///
/// @param  Hsm::STATE                  stateId
///
/// @param  const std::string&          name
///
////////////////////////////////////////////////////////////////////////////////////////////////////
CustomProductControllerStatePlayable::CustomProductControllerStatePlayable
( ProductControllerHsm&       hsm,
  CHsmState*                  pSuperState,
  Hsm::STATE                  stateId,
  const std::string&          name )

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
/// @brief  CustomProductControllerStatePlayable::HandlePlaybackRequest
///
/// @param  ProductPlaybackRequest_ProductPlaybackState state
///
/// @return This method returns a true Boolean value indicating that it has handled the power
///         state changed and no futher processing will be required by any of its superstates.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductControllerStatePlayable::HandlePlaybackRequest( ProductPlaybackRequest_ProductPlaybackState
                                                                  state )
{
    BOSE_VERBOSE( s_logger, "%s is handling a playback %s request.",
                  "CustomProductControllerStatePlayable",
                  ProductPlaybackRequest_ProductPlaybackState_Name( state ).c_str( ) );

    if( state == ProductPlaybackRequest_ProductPlaybackState_Start )
    {
        BOSE_VERBOSE( s_logger, "%s is changing to %s.",
                      "CustomProductControllerStatePlayable",
                      "CustomProductControllerStatePlayingActive" );
        ChangeState( PROFESSOR_PRODUCT_CONTROLLER_STATE_PLAYING_ACTIVE );
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief  CustomProductControllerStatePlayable::HandlePowerState
///
/// @return This method returns a true Boolean value indicating that it has handled the power
///         state changed and no futher processing will be required by any of its superstates.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductControllerStatePlayable::HandlePowerState( )
{
    BOSE_VERBOSE( s_logger, "%s is changing to %s.",
                  "CustomProductControllerStatePlayable",
                  "CustomProductControllerStatePlayingInactive" );
    ChangeState( PROFESSOR_PRODUCT_CONTROLLER_STATE_PLAYING_INACTIVE );

    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief  CustomProductControllerStatePlayable::HandleKey
///
/// @return This method returns a true Boolean value indicating that it has handled the supplied
///         key and that no futher processing will be required by any of its superstates.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductControllerStatePlayable::HandleKeyAction( int action )
{
    bool handled = true;
    ProfessorProductController &pc = GetCustomProductController();

    switch( action )
    {
    case KeyActionPb::KEY_ACTION_SOUNDTOUCH:
        pc.SelectSource( SOURCE_SOUNDTOUCH );
        break;

    case KeyActionPb::KEY_ACTION_TV:
        pc.SelectSource( SOURCE_TV );
        break;

    default:
        handled = false;
        break;
    }

    return handled;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief  CustomProductControllerStatePlayable::HandleNowSelectionInfo
///
/// @return This method returns a true Boolean value indicating that nowSelectionInfo
///         FrontDoor message is handled, and will change state to Audio ON
///
////////////////////////////////////////////////////////////////////////////////////////////////////

bool CustomProductControllerStatePlayable::HandleNowSelectionInfo( const SoundTouchInterface::NowSelectionInfo& nowSelectionInfo )
{
    BOSE_INFO( s_logger,  __func__ );
    ChangeState( PROFESSOR_PRODUCT_CONTROLLER_STATE_PLAYING_ACTIVE );
    return true;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
///                           End of the Product Application Namespace                           ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
