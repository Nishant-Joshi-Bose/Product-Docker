////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file      CustomProductControllerStateAccessoryPairingCancelling.cpp
///
/// @brief     This source code file contains functionality to process events that occur when
///            cancelling the product accessory pairing state leading to a power down to the
///            playable state.
///
/// @attention Copyright (C) 2018 Bose Corporation All Rights Reserved
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
#include "ProductController.h"
#include "CustomProductControllerStateAccessoryPairingCancelling.h"
#include "ProductControllerHsm.h"
#include "SpeakerPairingManager.h"
#include "ProductMessage.pb.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
///                            Start of Product Application Namespace                            ///
////////////////////////////////////////////////////////////////////////////////////////////////////
namespace ProductApp
{

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStateAccessoryPairingCancelling::
///        CustomProductControllerStateAccessoryPairingCancelling
///
/// @param ProductControllerHsm&       hsm               This argument references the state machine.
///
/// @param CHsmState*                  pSuperState       This argument references the parent state.
///
/// @param Hsm::STATE                  stateId           This enumeration represents the state ID.
///
/// @param const std::string&          name              This argument names the state.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
CustomProductControllerStateAccessoryPairingCancelling::
CustomProductControllerStateAccessoryPairingCancelling( ProductControllerHsm&       hsm,
                                                        CHsmState*                  pSuperState,
                                                        Hsm::STATE                  stateId,
                                                        const std::string&          name )

    : ProductControllerState( hsm, pSuperState, stateId, name )
{
    BOSE_INFO( s_logger, "The %s state is being constructed.", GetName( ).c_str( ) );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStateAccessoryPairingCancelling::HandleStateStart
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductControllerStateAccessoryPairingCancelling::HandleStateStart( )
{
    BOSE_INFO( s_logger, "The %s state is in %s.", GetName( ).c_str( ), __func__ );

    auto stopPairingAction = static_cast< KeyHandlerUtil::ActionType_t >( Action::ACTION_STOP_PAIR_SPEAKERS );

    GetProductController( ).GetIntentHandler( ).Handle( stopPairingAction );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStateAccessoryPairingCancelling::HandlePairingStatus
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductControllerStateAccessoryPairingCancelling::HandlePairingStatus( ProductPb::AccessorySpeakerState pairingStatus )
{
    BOSE_INFO( s_logger, "The %s state is handling a pairing %s.",
               GetName( ).c_str( ),
               pairingStatus.pairing( ) ? "activation" : "deactivation" );

    if( not pairingStatus.pairing( ) )
    {
        ChangeState( PRODUCT_CONTROLLER_STATE_PLAYING_SELECTED_STOPPING_STREAMS );
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief  CustomProductControllerStateAccessoryPairingCancelling::HandleIntentPowerToggle
///
/// @return This method returns a true Boolean value indicating that it has handled the power
///         toggling intent. It is ignored when cancelling accessory pairing.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductControllerStateAccessoryPairingCancelling::HandleIntentPowerToggle( )
{
    BOSE_INFO( s_logger, "The %s state in %s is ignoring the power toggle.", GetName( ).c_str( ), __func__ );

    ///
    /// The intent is ignored in this custom state.
    ///
    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief  CustomProductControllerStateAccessoryPairingCancelling::HandleIntentPowerOff
///
/// @return This method returns a true Boolean value indicating that it has handled the power
///         off intent. It is ignored when cancelling accessory pairing.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductControllerStateAccessoryPairingCancelling::HandleIntentPowerOff( )
{
    BOSE_INFO( s_logger, "The %s state in %s is ignoring the power off.", GetName( ).c_str( ), __func__ );

    ///
    /// The intent is ignored in this custom state.
    ///
    return true;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
///                             End of Product Application Namespace                             ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                        End of File                                           ///
////////////////////////////////////////////////////////////////////////////////////////////////////
