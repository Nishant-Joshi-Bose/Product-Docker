////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file      CustomProductControllerStatePlayingSelectedAccessoryPairing.cpp
///
/// @brief     This source code file contains functionality to process events that occur during the
///            product accessory pairing state when in a playing selected superstate.
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
#include "CustomProductControllerStatePlayingSelectedAccessoryPairing.h"
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
///            Constant Definitions
///
////////////////////////////////////////////////////////////////////////////////////////////////////
constexpr uint32_t PAIRING_MAX_TIME_MILLISECOND_TIMEOUT_START = 4 * 60 * 1000;
constexpr uint32_t PAIRING_MAX_TIME_MILLISECOND_TIMEOUT_RETRY = 0 ;

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStatePlayingSelectedAccessoryPairing::
///        CustomProductControllerStatePlayingSelectedAccessoryPairing
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
CustomProductControllerStatePlayingSelectedAccessoryPairing::
CustomProductControllerStatePlayingSelectedAccessoryPairing( ProductControllerHsm&       hsm,
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
/// @brief CustomProductControllerStatePlayingSelectedAccessoryPairing::HandleStateStart
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductControllerStatePlayingSelectedAccessoryPairing::HandleStateStart( )
{
    BOSE_INFO( s_logger, "The %s state is in %s.", GetName( ).c_str( ), __func__ );

    auto startPairingAction = static_cast< KeyHandlerUtil::ActionType_t >( Action::ACTION_LPM_PAIR_SPEAKERS );

    GetCustomProductController( ).GetIntentHandler( ).Handle( startPairingAction );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStatePlayingSelectedAccessoryPairing::HandlePairingState
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductControllerStatePlayingSelectedAccessoryPairing::HandlePairingState( ProductAccessoryPairing pairingStatus )
{
    BOSE_INFO( s_logger, "The %s state is handling a pairing %s.",
               GetName( ).c_str( ),
               pairingStatus.active( ) ? "activation" : "deactivation" );

    if( not pairingStatus.active( ) )
    {
        ChangeState( PRODUCT_CONTROLLER_STATE_PLAYING_SELECTED );
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStatePlayingSelectedAccessoryPairing::HandleIntentSpeakerPairing
///
/// @return This method returns a true Boolean value indicating that it is handling the speaker
///         pairing intent.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductControllerStatePlayingSelectedAccessoryPairing::HandleIntentSpeakerPairing( KeyHandlerUtil::ActionType_t intent )

{
    BOSE_INFO( s_logger, "The %s state is in %s.", GetName( ).c_str( ), __func__ );

    if( intent == ( unsigned int )Action::ACTION_STOP_PAIR_SPEAKERS )
    {
        GetCustomProductController( ).GetIntentHandler( ).Handle( intent );
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStatePlayingSelectedAccessoryPairing::HandleAudioPathDeselect
///
/// @return This method returns a true Boolean value indicating that it has handled the event.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductControllerStatePlayingSelectedAccessoryPairing::HandleAudioPathDeselect( )
{
    BOSE_INFO( s_logger, "The %s state is in %s.", GetName( ).c_str( ), __func__ );

    m_stopPairingOnExit = false;

    ChangeState( CUSTOM_PRODUCT_CONTROLLER_STATE_PLAYING_DESELECTED_ACCESSORY_PAIRING );

    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStatePlayingSelectedAccessoryPairing::HandleStateExit
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductControllerStatePlayingSelectedAccessoryPairing::HandleStateExit( )
{
    BOSE_INFO( s_logger, "The %s state is in %s.", GetName( ).c_str( ), __func__ );

    if( m_stopPairingOnExit )
    {
        auto stopPairingAction = static_cast< KeyHandlerUtil::ActionType_t >( Action::ACTION_STOP_PAIR_SPEAKERS );

        GetCustomProductController( ).GetIntentHandler( ).Handle( stopPairingAction );
    }
    m_stopPairingOnExit = true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief  CustomProductControllerStatePlayingSelectedAccessoryPairing::HandleNowSelectionInfo
///
/// @param  const SoundTouchInterface::NowSelectionInfo& nowSelectionInfo
///
/// @return This method returns a true Boolean value indicating that it has handled the new now
///         selection.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductControllerStatePlayingSelectedAccessoryPairing::HandleNowSelectionInfo(
    const SoundTouchInterface::NowSelectionInfo& nowSelectionInfo )
{
    BOSE_INFO( s_logger, "The %s state is in %s.", GetName( ).c_str( ), __func__ );

    if( ( nowSelectionInfo.has_contentitem( ) ) and
        ( nowSelectionInfo.contentitem( ).source( ).compare( "PRODUCT" )      == 0 ) )
    {
        if( nowSelectionInfo.contentitem( ).sourceaccount( ).compare( "SETUP" ) == 0 )
        {
            ChangeState( PRODUCT_CONTROLLER_STATE_PLAYING_SELECTED_SETUP );

        }
        else if( nowSelectionInfo.contentitem( ).sourceaccount( ).compare( "ADAPTiQ" ) == 0 )
        {
            ChangeState( CUSTOM_PRODUCT_CONTROLLER_STATE_ADAPTIQ );
        }
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                             End of Product Application Namespace                             ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                        End of File                                           ///
////////////////////////////////////////////////////////////////////////////////////////////////////
