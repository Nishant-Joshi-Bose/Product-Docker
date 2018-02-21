////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file      CustomProductControllerStatePlayingDeselectedAccessoryPairing.cpp
///
/// @brief     This source code file contains functionality to process events that occur during the
///            product accessory pairing state when in a playing deselected superstate.
///
/// @author    Derek Richardson
///
/// @attention Copyright (C) 2017 Bose Corporation All Rights Reserved
///
////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////
///
///            Included Header Files
///
////////////////////////////////////////////////////////////////////////////////////////////////////
#include "Utilities.h"
#include "Intents.h"
#include "IntentHandler.h"
#include "CustomProductControllerStatePlayingDeselectedAccessoryPairing.h"
#include "ProductControllerHsm.h"
#include "ProfessorProductController.h"
#include "SpeakerPairingManager.h"
#include "ProductMessage.pb.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
///                            Start of Product Application Namespace                            ///
////////////////////////////////////////////////////////////////////////////////////////////////////
namespace ProductApp
{


////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStatePlayingDeselectedAccessoryPairing::
///        CustomProductControllerStatePlayingDeselectedAccessoryPairing
///
/// @param ProductControllerHsm&       hsm               This argument references the state machine.
///
/// @param CHsmState*                  pSuperState       This argument references the parent state.
///
/// @param ProfessorProductController& productController This argument references the product controller.
///
/// @param Hsm::STATE                  stateId           This enumeration represents the state ID.
///
/// @param const std::string&          name              This argument names the state.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
CustomProductControllerStatePlayingDeselectedAccessoryPairing::
CustomProductControllerStatePlayingDeselectedAccessoryPairing( ProductControllerHsm&       hsm,
                                                               CHsmState*                  pSuperState,
                                                               ProfessorProductController& productController,
                                                               Hsm::STATE                  stateId,
                                                               const std::string&          name )

    : ProductControllerState( hsm, pSuperState, stateId, name ),
      m_stopPairingOnExit( true )
{
    BOSE_INFO( s_logger, "The %s state is being constructed.", GetName( ).c_str( ) );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStatePlayingDeselectedAccessoryPairing::HandleStateStart
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductControllerStatePlayingDeselectedAccessoryPairing::HandleStateStart( )
{
    BOSE_INFO( s_logger, "The %s state is in %s.", GetName( ).c_str( ), __func__ );

    unsigned int startPairingAction = static_cast< KeyHandlerUtil::ActionType_t >( Action::ACTION_LPM_PAIR_SPEAKERS );

    GetCustomProductController( ).GetIntentHandler( ).Handle( startPairingAction );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStatePlayingDeselectedAccessoryPairing::HandlePairingState
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductControllerStatePlayingDeselectedAccessoryPairing::HandlePairingState( ProductAccessoryPairing pairingStatus )
{
    BOSE_INFO( s_logger, "The %s state is handling a pairing %s.",
               GetName( ).c_str( ),
               pairingStatus.active( ) ? "activation" : "deactivation" );

    if( not pairingStatus.active( ) )
    {
        ChangeState( PRODUCT_CONTROLLER_STATE_PLAYING_DESELECTED );
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStatePlayingDeselectedAccessoryPairing::HandleAudioPathSelect
///
/// @return This method returns a true Boolean value indicating that it has handled the event.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductControllerStatePlayingDeselectedAccessoryPairing::HandleAudioPathSelect( )
{
    BOSE_INFO( s_logger, "The %s state is in %s.", GetName( ).c_str( ), __func__ );

    m_stopPairingOnExit = false;

    ChangeState( CUSTOM_PRODUCT_CONTROLLER_STATE_PLAYING_SELECTED_ACCESSORY_PAIRING );

    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStatePlayingDeselectedAccessoryPairing::HandleStateExit
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductControllerStatePlayingDeselectedAccessoryPairing::HandleStateExit( )
{
    BOSE_INFO( s_logger, "The %s state is in %s.", GetName( ).c_str( ), __func__ );

    if( m_stopPairingOnExit )
    {
        unsigned int stopPairingAction = static_cast< unsigned int >( Action::ACTION_STOP_PAIR_SPEAKERS );

        GetCustomProductController( ).GetIntentHandler( ).Handle( stopPairingAction );
    }
    m_stopPairingOnExit = true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                             End of Product Application Namespace                             ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                        End of File                                           ///
////////////////////////////////////////////////////////////////////////////////////////////////////
