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
bool CustomProductControllerStatePlayable::HandleIntentUserPower( KeyHandlerUtil::ActionType_t intent )
{
    BOSE_INFO( s_logger, "%s in %s is handling key action %d.", GetName( ).c_str( ), __FUNCTION__, intent );
    // To play Product source, playbackRequest can be made directly by PlaybackRequestManager intentHandler;
    // To play SoundTouch source, additional check required to confirm if network is configured and persisted SoundTouch source is available
    if( GetCustomProductController().GetLastContentItem().source() == "PRODUCT" )
    {
        KeyHandlerUtil::ActionType_t startTvPlayback = static_cast< KeyHandlerUtil::ActionType_t >( Action::ACTION_TV );
        GetCustomProductController( ).GetIntentHandler( ).Handle( startTvPlayback );
    }
    else
    {
        HandleIntentPlaySoundTouchSource( );
    }
    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                           End of the Product Application Namespace                           ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
