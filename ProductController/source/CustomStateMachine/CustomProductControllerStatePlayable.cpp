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
#include "IntentHandler.h"
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
    : ProductControllerStatePlayable( hsm, pSuperState, stateId, name )
{
    BOSE_INFO( s_logger, "%s is being constructed.", GetName( ).c_str( ) );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief  CustomProductControllerStatePlayable::HandleIntentUserPower
///
/// @param  KeyHandlerUtil::ActionType_t action
///
/// @return This method returns a true Boolean value indicating that it has handled the event
///         and no futher processing will be required by any of its superstates.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductControllerStatePlayable::HandleIntentUserPower( KeyHandlerUtil::ActionType_t action )
{
    BOSE_INFO( s_logger, "%s in %s is handling key action %d.", GetName( ).c_str( ), __FUNCTION__, action );
    if( GetCustomProductController().GetLastContentItem().has_source() &&
        GetCustomProductController().GetLastContentItem().source() == "PRODUCT" )
    {
        unsigned int startTvPlayback = static_cast< unsigned int >( Action::ACTION_TV );
        GetCustomProductController( ).GetIntentHandler( ).Handle( startTvPlayback );
    }
    else
    {
        unsigned int startSoundTouchPlayback = static_cast< unsigned int >( Action::ACTION_SOUNDTOUCH );
        HandleIntentPlaySoundTouchSource( startSoundTouchPlayback );
    }
    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief  CustomProductControllerStatePlayable::HandleIntentPlaySoundTouchSource
///
/// @param  KeyHandlerUtil::ActionType_t action
///
/// @return This method returns a true Boolean value indicating that it has handled the action.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductControllerStatePlayable::HandleIntentPlaySoundTouchSource( KeyHandlerUtil::ActionType_t action )
{
    BOSE_INFO( s_logger, "%s in %s is handling the action %u", GetName( ).c_str( ), __FUNCTION__, action );
    // If network is configured, but no persisted last streaming source => go to Deselected
    if( GetCustomProductController().IsNetworkAvailbleForSoundTouchSource() &&
        !GetCustomProductController( ).GetLastStreamingContentItem().has_source() )
    {
        ChangeState( PRODUCT_CONTROLLER_STATE_PLAYING_DESELECTED );
        return true;
    }
    // otherwise, let parent states handle it
    return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                           End of the Product Application Namespace                           ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
