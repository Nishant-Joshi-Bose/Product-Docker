////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file      CustomProductControllerStatePlayingTransitionAccessoryPairing.cpp
///
/// @brief     This source code file contains functionality to process events occurring during a
///            full power transition to a speaker pairing state.
///
/// @attention Copyright (C) 2018 Bose Corporation All Rights Reserved
///
////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////
///            Included Header Files
////////////////////////////////////////////////////////////////////////////////////////////////////
#include "ProductControllerStates.h"
#include "CustomProductControllerStatePlayingTransitionAccessoryPairing.h"
#include "ProductController.h"
#include "DPrint.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
/// The following declares a DPrint class type object for logging information for this state class.
////////////////////////////////////////////////////////////////////////////////////////////////////
static DPrint s_logger( "Product" );

////////////////////////////////////////////////////////////////////////////////////////////////////
///                            Start of Product Application Namespace                            ///
////////////////////////////////////////////////////////////////////////////////////////////////////
namespace ProductApp
{

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStatePlayingTransitionAccessoryPairing::
///        CustomProductControllerStatePlayingTransitionAccessoryPairing
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
CustomProductControllerStatePlayingTransitionAccessoryPairing::
CustomProductControllerStatePlayingTransitionAccessoryPairing(
    ProductControllerHsm& hsm,
    CHsmState*            pSuperState,
    Hsm::STATE            stateId,
    const std::string&    name )

    : ProductControllerState( hsm, pSuperState, stateId, name )
{
    BOSE_INFO( s_logger, "The %s state is being constructed.", GetName( ).c_str( ) );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStatePlayingTransitionAccessoryPairing::HandleStateEnter
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductControllerStatePlayingTransitionAccessoryPairing::HandleStateEnter()
{
    BOSE_INFO( s_logger, "The %s state is in %s.", GetName( ).c_str( ), __func__ );

    GetProductController( ).GetLpmHardwareInterface( )->SetSystemState( SYSTEM_STATE_ON );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStatePlayingTransitionAccessoryPairing::HandleLPMPowerStatusFullPowerOn
///
/// @return This method returns a true value, indicating that it has handled the device transition
///         to a full power on system state for speaker pairing.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductControllerStatePlayingTransitionAccessoryPairing::HandleLPMPowerStatusFullPowerOn( )
{
    BOSE_INFO( s_logger, "The %s state is in %s.", GetName( ).c_str( ), __func__ );
    BOSE_INFO( s_logger, "The state is changing to a PlayingDeselectedAccessoryPairing state." );

    ChangeState( CUSTOM_PRODUCT_CONTROLLER_STATE_PLAYING_DESELECTED_ACCESSORY_PAIRING );

    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                           End of the Product Application Namespace                           ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
