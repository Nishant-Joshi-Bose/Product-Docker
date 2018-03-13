////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file      CustomProductControllerStatePlayingDeselected.cpp
///
/// @brief     This source code file contains functionality to process events that occur during the
///            product deselected state.
///
/// @attention Copyright (C) 2018 Bose Corporation All Rights Reserved
////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////
///            Included Header Files
////////////////////////////////////////////////////////////////////////////////////////////////////
#include "Utilities.h"
#include "ProductController.h"
#include "ProductControllerStates.h"
#include "ProductControllerStatePlayingDeselected.h"
#include "CustomProductControllerStatePlayingDeselected.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
///                            Start of Product Application Namespace                            ///
////////////////////////////////////////////////////////////////////////////////////////////////////
namespace ProductApp
{

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStatePlayingDeselected::CustomProductControllerStatePlayingDeselected
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
CustomProductControllerStatePlayingDeselected::CustomProductControllerStatePlayingDeselected(
    ProductControllerHsm& hsm,
    CHsmState*            pSuperState,
    Hsm::STATE            stateId,
    const std::string&    name )

    : ProductControllerStatePlayingDeselected( hsm, pSuperState, stateId, name )
{
    BOSE_INFO( s_logger, "The %s state is being constructed.", GetName( ).c_str( ) );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStatePlayingDeselected::HandleIntentSpeakerPairing
///
/// @return This method returns a true Boolean value indicating that it is handling the speaker
///         pairing intent.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductControllerStatePlayingDeselected::HandleIntentSpeakerPairing( KeyHandlerUtil::ActionType_t intent )

{
    BOSE_INFO( s_logger, "The %s state is in %s.", GetName( ).c_str( ), __func__ );

    if( intent == ( unsigned int )Action::ACTION_START_PAIR_SPEAKERS )
    {
        BOSE_INFO( s_logger, "The state is changing to the CustomDeselectedAccessoryPairing state." );
        ChangeState( CUSTOM_PRODUCT_CONTROLLER_STATE_PLAYING_DESELECTED_ACCESSORY_PAIRING );
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
