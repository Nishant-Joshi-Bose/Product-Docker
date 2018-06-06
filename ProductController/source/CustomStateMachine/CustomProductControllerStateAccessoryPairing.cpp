////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file      CustomProductControllerStateAccessoryPairing.cpp
///
/// @brief     This source code file contains functionality to process events that occur during the
///            product accessory pairing state.
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
#include "CustomProductControllerStateAccessoryPairing.h"
#include "ProductControllerHsm.h"
#include "SpeakerPairingManager.h"
#include "ProfessorChimeEvents.h"
#include "ProductMessage.pb.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
///                            Start of Product Application Namespace                            ///
////////////////////////////////////////////////////////////////////////////////////////////////////
namespace ProductApp
{

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStateAccessoryPairing::
///        CustomProductControllerStateAccessoryPairing
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
CustomProductControllerStateAccessoryPairing::
CustomProductControllerStateAccessoryPairing( ProductControllerHsm&       hsm,
                                              CHsmState*                  pSuperState,
                                              Hsm::STATE                  stateId,
                                              const std::string&          name )

    : ProductControllerState( hsm, pSuperState, stateId, name )
{
    BOSE_INFO( s_logger, "The %s state is being constructed.", GetName( ).c_str( ) );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStateAccessoryPairing::HandleStateStart
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductControllerStateAccessoryPairing::HandleStateStart( )
{
    BOSE_INFO( s_logger, "The %s state is in %s.", GetName( ).c_str( ), __func__ );

    ///
    /// Disable source selection while in accessory pairing, and start the pairing process.
    ///
    auto startPairingAction = static_cast< KeyHandlerUtil::ActionType_t >( Action::ACTION_LPM_PAIR_SPEAKERS );

    GetProductController( ).SendAllowSourceSelectMessage( false );
    GetProductController( ).GetIntentHandler( ).Handle( startPairingAction );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStateAccessoryPairing::HandleIntentSpeakerPairing
///
/// @return This method returns a true Boolean value indicating that it is handling the speaker
///         pairing intent.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductControllerStateAccessoryPairing::HandleIntentSpeakerPairing( KeyHandlerUtil::ActionType_t intent )
{
    BOSE_INFO( s_logger, "The %s state is in %s.", GetName( ).c_str( ), __func__ );

    if( intent == static_cast< KeyHandlerUtil::ActionType_t >( Action::ACTION_STOP_PAIR_SPEAKERS ) )
    {
        GetProductController( ).GetIntentHandler( ).Handle( intent );
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief  CustomProductControllerStateAccessoryPairing::HandleIntentVolumeControl
///
/// @param  KeyHandlerUtil::ActionType_t intent
///
/// @return This method returns a true Boolean value indicating that it has handled the volume
///         control intent. It is ignored during acessory pairing.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductControllerStateAccessoryPairing::HandleIntentVolumeControl( KeyHandlerUtil::ActionType_t intent )
{
    BOSE_INFO( s_logger, "The %s state in %s is ignoring the intent %u.", GetName( ).c_str( ), __func__, intent );

    ///
    /// The intent is ignored in this custom state.
    ///
    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief  CustomProductControllerStateAccessoryPairing::HandleIntentMuteControl
///
/// @param  KeyHandlerUtil::ActionType_t intent
///
/// @return This method returns a true Boolean value indicating that it has handled the muting
///         intent. It is ignored during acessory pairing.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductControllerStateAccessoryPairing::HandleIntentMuteControl( KeyHandlerUtil::ActionType_t intent )
{
    BOSE_INFO( s_logger, "The %s state is in %s ignored the intent %u.", GetName( ).c_str( ), __func__, intent );

    ///
    /// The intent is ignored in this custom state.
    ///
    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief  CustomProductControllerStateAccessoryPairing::HandleIntentPowerToggle
///
/// @return This method returns a true Boolean value indicating that it handles the PowerToggle
///         intent by changing to an accessory pairing cancelling state.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductControllerStateAccessoryPairing::HandleIntentPowerToggle( )
{
    BOSE_INFO( s_logger, "The %s state in %s cancelling the pairing playback.", GetName( ).c_str( ), __func__ );

    ChangeState( CUSTOM_PRODUCT_CONTROLLER_STATE_ACCESSORY_PAIRING_CANCELLING );

    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStateAccessoryPairing::HandlePairingStatus
///
/// @return This method returns a true Boolean value indicating that it handles the accessory
///         pairing status.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductControllerStateAccessoryPairing::HandlePairingStatus( ProductPb::AccessorySpeakerState pairingStatus )
{
    BOSE_INFO( s_logger, "The %s state has received an %s pairing status.",
               GetName( ).c_str( ),
               pairingStatus.pairing( ) ? "pairing active" : "pairing inactive" );

    if( not pairingStatus.pairing( ) )
    {
        // When accessory pairing is done, play the pairing complete chime
        // Order has to be followed here: subwoofer chime first, then rear surround speakers
        m_pairingCompleteChimeToPlay.clear();
        if( pairingStatus.subs_size() > 0 )
        {
            m_pairingCompleteChimeToPlay.push_back( ACCESSORY_PAIRING_COMPLETE_SUB );
        }
        if( pairingStatus.rears_size() > 0 )
        {
            m_pairingCompleteChimeToPlay.push_back( ACCESSORY_PAIRING_COMPLETE_REAR_SPEAKER );
        }

        if( m_pairingCompleteChimeToPlay.size() == 0 )
        {
            GetProductController( ).SendStopPlaybackMessage( );
            ChangeState( PRODUCT_CONTROLLER_STATE_PLAYING_SELECTED_SILENT );
        }
        else
        {
            PlayPairingCompletedChime();
        }
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStateAccessoryPairing::PlayPairingCompletedChime
///
///         This method start playing the pairing completed chime from the chime queue
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductControllerStateAccessoryPairing::PlayPairingCompletedChime()
{
    BOSE_INFO( s_logger, "The %s state is in %s. %d chimes in the queue to be played", GetName( ).c_str( ), __func__, m_pairingCompleteChimeToPlay.size() );

    if( m_pairingCompleteChimeToPlay.size() > 0 )
    {
        GetProductController( ).HandleChimePlayRequest( m_pairingCompleteChimeToPlay.front() );
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStateAccessoryPairing::HandleChimeSASSPlaybackCompleted
///
///         This method removes finished chime from chime queue, and start playing next chime
///         if chimes are completed, exit pairing state
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductControllerStateAccessoryPairing::HandleChimeSASSPlaybackCompleted( int32_t eventId )
{
    BOSE_INFO( s_logger, "The %s state is in %s.", GetName( ).c_str( ), __func__ );

    if( eventId == m_pairingCompleteChimeToPlay.front() )
    {
        m_pairingCompleteChimeToPlay.pop_front();
        // Play next accessory pairing completed chime if there's one
        if( m_pairingCompleteChimeToPlay.size() == 0 )
        {
            BOSE_INFO( s_logger, "The %s state is exiting the pairing playback.", GetName( ).c_str( ) );
            GetProductController( ).SendStopPlaybackMessage( );
            ChangeState( PRODUCT_CONTROLLER_STATE_PLAYING_SELECTED_SILENT );
        }
        else
        {
            GetProductController( ).HandleChimePlayRequest( m_pairingCompleteChimeToPlay.front() );
        }
    }
    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStateAccessoryPairing::HandleStateExit
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductControllerStateAccessoryPairing::HandleStateExit( )
{
    BOSE_INFO( s_logger, "The %s state is in %s.", GetName( ).c_str( ), __func__ );

    ///
    /// Re-enable source selection.
    ///
    GetProductController( ).SendAllowSourceSelectMessage( true );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                             End of Product Application Namespace                             ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                        End of File                                           ///
////////////////////////////////////////////////////////////////////////////////////////////////////
