////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file      CustomProductControllerStateAccessoryPairingExiting.cpp
///
/// @brief     This source code file contains functionality to process events that occur when
///            exiting the product accessory pairing state.
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
#include "CustomProductControllerStateAccessoryPairingExiting.h"
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
/// @brief CustomProductControllerStateAccessoryPairingExiting::
///        CustomProductControllerStateAccessoryPairingExiting
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
CustomProductControllerStateAccessoryPairingExiting::
CustomProductControllerStateAccessoryPairingExiting( ProductControllerHsm&       hsm,
                                                     CHsmState*                  pSuperState,
                                                     ProfessorProductController& productController,
                                                     Hsm::STATE                  stateId,
                                                     const std::string&          name )

    : ProductControllerState( hsm, pSuperState, stateId, name )
{
    BOSE_INFO( s_logger, "The %s state is being constructed.", GetName( ).c_str( ) );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStateAccessoryPairingExiting::HandleStateStart
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductControllerStateAccessoryPairingExiting::HandleStateStart( )
{
    BOSE_INFO( s_logger, "The %s state is in %s.", GetName( ).c_str( ), __func__ );

    GetProductController( ).SendStopPlaybackMessage( );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief  CustomProductControllerStateAccessoryPairingExiting::HandleIntentVolumeControl
///
/// @param  KeyHandlerUtil::ActionType_t intent
///
/// @return This method returns a true Boolean value indicating that it has handled the volume
///         control intent. It is ignored when exiting acessory pairing.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductControllerStateAccessoryPairingExiting::HandleIntentVolumeControl( KeyHandlerUtil::ActionType_t intent )
{
    BOSE_INFO( s_logger, "The %s state in %s is ignoring the intent %u.", GetName( ).c_str( ), __func__, intent );

    ///
    /// The intent is ignored in this custom state.
    ///
    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief  CustomProductControllerStateAccessoryPairingExiting::HandleIntentMuteControl
///
/// @param  KeyHandlerUtil::ActionType_t intent
///
/// @return This method returns a true Boolean value indicating that it has handled the muting
///         intent. It is ignored when exiting acessory pairing.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductControllerStateAccessoryPairingExiting::HandleIntentMuteControl( KeyHandlerUtil::ActionType_t intent )
{
    BOSE_INFO( s_logger, "The %s state is in %s ignored the intent %u.", GetName( ).c_str( ), __func__, intent );

    ///
    /// The intent is ignored in this custom state.
    ///
    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief  CustomProductControllerStateAccessoryPairingExiting::HandleIntentPowerToggle
///
/// @return This method returns a true Boolean value indicating that it has handled the power
///         toggling intent. It is ignored when exiting acessory pairing.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductControllerStateAccessoryPairingExiting::HandleIntentPowerToggle( )
{
    BOSE_INFO( s_logger, "The %s state in %s is ignoring the power toggle.", GetName( ).c_str( ), __func__ );

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
