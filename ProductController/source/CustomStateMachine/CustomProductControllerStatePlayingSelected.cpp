////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file      CustomProductControllerStatePlayingSelected.cpp
///
/// @brief     This source code file contains functionality to process events that occur during the
///            playing selected state that are custom to the Professor product.
///
/// @attention Copyright (C) 2018 Bose Corporation All Rights Reserved
////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////
///            Included Header Files
////////////////////////////////////////////////////////////////////////////////////////////////////
#include "Utilities.h"
#include "ProductController.h"
#include "ProductControllerStates.h"
#include "ProductControllerStatePlayingSelected.h"
#include "CustomProductControllerStatePlayingSelected.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
///                            Start of Product Application Namespace                            ///
////////////////////////////////////////////////////////////////////////////////////////////////////
namespace ProductApp
{

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStatePlayingSelected::CustomProductControllerStatePlayingSelected
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
CustomProductControllerStatePlayingSelected::CustomProductControllerStatePlayingSelected

( ProductControllerHsm& hsm,
  CHsmState*            pSuperState,
  Hsm::STATE            stateId,
  const std::string&    name )

    : ProductControllerStatePlayingSelected( hsm, pSuperState, stateId, name )
{
    BOSE_INFO( s_logger, "The %s state is being constructed.", GetName( ).c_str( ) );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief  CustomProductControllerStatePlayingSelected::HandleIntentSpeakerPairing
///
/// @return This method returns a true Boolean value indicating that it has handled the event
///         and no futher processing will be required by any of its superstates.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductControllerStatePlayingSelected::HandleIntentSpeakerPairing( KeyHandlerUtil::ActionType_t intent )
{
    BOSE_INFO( s_logger, "The %s state is in %s.", GetName( ).c_str( ), __func__ );

    ChangeState( CUSTOM_PRODUCT_CONTROLLER_STATE_PLAYING_SELECTED_ACCESSORY_PAIRING );

    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief  CustomProductControllerStatePlayingSelected::PossiblyGoToCustomState
///
/// @return This is used for product specific conditional checks and potential state changes to
///         custom states based on the product. It returns a true Boolean value if a custom
///         state change is to take place; otherwise, it returns false and lets the derive
///         common state handle any required state transitions.
///
////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductControllerStatePlayingSelected::PossiblyGoToCustomState( )
{
    BOSE_INFO( s_logger, "The %s state is in %s.", GetName( ).c_str( ), __func__ );

    if( GetProductController( ).GetNowSelection( ).has_contentitem( ) )
    {
        if( GetProductController( ).GetNowSelection( ).contentitem( ).source( ).compare( "PRODUCT" )        == 0 and
            GetProductController( ).GetNowSelection( ).contentitem( ).sourceaccount( ).compare( "ADAPTiQ" ) == 0 )
        {
            ChangeState( CUSTOM_PRODUCT_CONTROLLER_STATE_ADAPTIQ );

            return true;
        }
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
