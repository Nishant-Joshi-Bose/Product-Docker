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
#include "ProductSTS.pb.h"
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
        if( GetProductController( ).GetNowSelection( ).contentitem( ).source( ).compare( SHELBY_SOURCE::SETUP ) == 0 )
        {
            if( GetProductController( ).GetNowSelection( ).contentitem( ).sourceaccount( ).compare( SetupSourceSlot_Name( ADAPTIQ ) ) == 0 )
            {
                ChangeState( CUSTOM_PRODUCT_CONTROLLER_STATE_ADAPTIQ );
                return;
            }

            if( GetProductController( ).GetNowSelection( ).contentitem( ).sourceaccount( ).compare( SetupSourceSlot_Name( PAIRING ) ) == 0 )
            {
                ChangeState( CUSTOM_PRODUCT_CONTROLLER_STATE_ACCESSORY_PAIRING );
                return;
            }
        }
    }
    ProductControllerStatePlayingSelected::GoToNextState( );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                           End of the Product Application Namespace                           ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
