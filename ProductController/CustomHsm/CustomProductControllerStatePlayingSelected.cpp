////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file      CustomProductControllerStatePlayingSelected.cpp
///
/// @brief     This source code file contains functionality to process events that occur during the
///            playing selected state that are custom for the product.
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
#include "ProductSTS.pb.h"
#include "CustomProductController.h"
#include "SHELBY_SOURCE.h"

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

////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief  CustomProductControllerStatePlayingSelected::GoToNextState
///
///         This is used for product specific conditional checks and potential state changes to
///         custom states based on the product. If no custom state change required, it will execute
///         Common GoToNextState
///
////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductControllerStatePlayingSelected::GoToNextState( )
{
    BOSE_INFO( s_logger, "The %s state is in %s.", GetName( ).c_str( ), __func__ );

    using namespace ProductSTS;

    if( GetProductController( ).GetNowSelection( ).has_contentitem( ) )
    {
        if( GetProductController( ).GetNowSelection( ).contentitem( ).source( ) == SHELBY_SOURCE::SETUP )
        {
            const auto& sourceAccount = GetProductController( ).GetNowSelection( ).contentitem( ).sourceaccount( );
            if( sourceAccount == SetupSourceSlot_Name( ADAPTIQ ) )
            {
                ChangeState( CUSTOM_PRODUCT_CONTROLLER_STATE_ADAPTIQ );
                return;
            }
            if( sourceAccount == SetupSourceSlot_Name( PAIRING ) )
            {
                ChangeState( CUSTOM_PRODUCT_CONTROLLER_STATE_ACCESSORY_PAIRING );
                return;
            }
            if( sourceAccount == SetupSourceSlot_Name( CONTROL_INTEGRATION ) )
            {
                ChangeState( CUSTOM_PRODUCT_CONTROLLER_STATE_CONTROL_INTEGRATION );
                return;
            }
        }
    }
    ProductControllerStatePlayingSelected::GoToNextState( );
}

bool CustomProductControllerStatePlayingSelected::HandleIntentAudioModeToggle( KeyHandlerUtil::ActionType_t intent )
{
    BOSE_INFO( s_logger, "%s in %s is handling the intent action %s", GetName( ).c_str( ), __func__, CommonIntentHandler::GetIntentName( intent ).c_str( ) );

    GetCustomProductController( ).GetIntentHandler( ).Handle( intent );

    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                           End of the Product Application Namespace                           ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
