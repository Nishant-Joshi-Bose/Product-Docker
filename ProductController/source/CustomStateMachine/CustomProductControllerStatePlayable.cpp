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
#include "ProductEndpointDefines.h"

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
    BOSE_INFO( s_logger, "The %s state is being constructed.", GetName( ).c_str( ) );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name CustomProductControllerStatePlayable::HandleStateEnter
///
/// @brief The volume level expected upon exit into PLAYING is set, so that API queries would report
///        the value that would be enforced
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductControllerStatePlayable::HandleStateEnter( )
{
    ProductControllerStatePlayable::HandleStateEnter( );

    BOSE_INFO( s_logger, "The %s state is in %s", GetName( ).c_str( ), __func__ );

    SoundTouchInterface::volume v;
    v.set_value( GetCustomProductController( ).GetDesiredPlayingVolume( ) );
    GetCustomProductController( ).GetFrontDoorClient( )->SendPut<SoundTouchInterface::volume, FrontDoor::Error>(
        FRONTDOOR_AUDIO_VOLUME, v, {}, FrontDoorErrorCallback );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief  CustomProductControllerStatePlayable::HandleIntentSpeakerPairing
///
/// @return This method returns a true Boolean value indicating that it has handled the event
///         and no futher processing will be required by any of its superstates.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductControllerStatePlayable::HandleIntentSpeakerPairing( KeyHandlerUtil::ActionType_t intent )
{
    BOSE_INFO( s_logger, "The %s state is in %s.", GetName( ).c_str( ), __func__ );

    if( intent == ( unsigned int )Action::ACTION_START_PAIR_SPEAKERS )
    {
        ChangeState( CUSTOM_PRODUCT_CONTROLLER_STATE_PLAYING_TRANSITION_ACCESSORY_PAIRING );
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
