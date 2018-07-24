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
#include "CustomChimeEvents.h"
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
    BOSE_INFO( s_logger, "The %s state in %s is ignoring the intent %u.", GetName( ).c_str( ), __func__, intent );

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
/// @brief  CustomProductControllerStateAccessoryPairing::HandleIntentPowerOff
///
/// @return This method returns a true Boolean value indicating that it handles the PowerOff
///         intent by changing to an accessory pairing cancelling state.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductControllerStateAccessoryPairing::HandleIntentPowerOff( )
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
        // Add subwoofer pairing complete chime to queue, if all subs are valid
        if( pairingStatus.subs_size() > 0 )
        {
            bool isSubValid = true;
            for( int i = 0; i < pairingStatus.subs_size(); i++ )
            {
                if( pairingStatus.subs( i ).configurationstatus( ) != "VALID" )
                {
                    isSubValid = false;
                }
            }
            if( isSubValid )
            {
                m_pairingCompleteChimeToPlay.push_back( CHIME_ACCESSORY_PAIRING_COMPLETE_SUB );
            }
        }
        // Add rear surround speakers pairing complete chime to queue, if all rears are valid
        if( pairingStatus.rears_size() > 0 )
        {
            bool isRearValid = true;
            for( int i = 0; i < pairingStatus.rears_size(); i++ )
            {
                if( pairingStatus.rears( i ).configurationstatus( ) != "VALID" )
                {
                    isRearValid = false;
                }
            }
            if( isRearValid )
            {
                m_pairingCompleteChimeToPlay.push_back( CHIME_ACCESSORY_PAIRING_COMPLETE_REAR_SPEAKER );
            }
        }

        // Start playing pairing completed chime if there's any chime in queue; otherwise exit pairing state
        if( m_pairingCompleteChimeToPlay.empty( ) )
        {
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
        if( m_pairingCompleteChimeToPlay.empty( ) )
        {
            BOSE_INFO( s_logger, "The %s state is exiting the pairing playback.", GetName( ).c_str( ) );
            ChangeState( PRODUCT_CONTROLLER_STATE_PLAYING_SELECTED_SILENT );
        }
        else
        {
            GetProductController( ).HandleChimePlayRequest( m_pairingCompleteChimeToPlay.front() );
        }
    }
    else
    {
        BOSE_INFO( s_logger, "%s - eventId(%d) is not what we expect(%d), ignore it.",
                   __func__, eventId, m_pairingCompleteChimeToPlay.front() );
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
    GetProductController( ).SendStopPlaybackMessage( );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                             End of Product Application Namespace                             ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                        End of File                                           ///
////////////////////////////////////////////////////////////////////////////////////////////////////
