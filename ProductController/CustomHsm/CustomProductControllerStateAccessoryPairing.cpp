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
#include "CustomProductController.h"
#include "CustomProductControllerStateAccessoryPairing.h"
#include "ProductControllerHsm.h"
#include "SpeakerPairingManager.h"
#include "ProductMessage.pb.h"
#include "ProductSTS.pb.h"

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
    BOSE_INFO( s_logger, "The %s state has received pairing status: %s.",
               GetName( ).c_str( ),
               pairingStatus.DebugString().c_str() );

    if( pairingStatus.pairing( ) )
    {
        return true;
    }

    if( !GetCustomProductController( ).GetSpeakerPairingIsFromLAN( ) )
    {
        // When accessory pairing (not from LAN) is done, play the pairing complete chime
        // Sub chime should be played, if there's subwoofer(s) in the accessoryList, and all of them are valid
        bool isSubValid = pairingStatus.subs_size() > 0;
        if( isSubValid )
        {
            for( int i = 0; i < pairingStatus.subs_size(); i++ )
            {
                if( pairingStatus.subs( i ).configurationstatus( ) != "VALID" )
                {
                    isSubValid = false;
                    break;
                }
            }
        }
        // Rear chime should be played, if there're rear speakers in the accessoryList, and all of them are valid
        bool isRearValid = pairingStatus.rears_size() > 0 ;
        if( isRearValid )
        {
            for( int i = 0; i < pairingStatus.rears_size(); i++ )
            {
                if( pairingStatus.rears( i ).configurationstatus( ) != "VALID" )
                {
                    isRearValid = false;
                    break;
                }
            }
        }

        ProductMessage message;
        message.mutable_accessoriesplaytones( )->set_subs( isSubValid );
        message.mutable_accessoriesplaytones( )->set_rears( isRearValid );
        GetProductController( ).SendAsynchronousProductMessage( message );
    }

    ChangeState( PRODUCT_CONTROLLER_STATE_PLAYING_SELECTED_SILENT );
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
///
/// @brief  CustomProductControllerStateAccessoryPairing::HandleNowSelectionInfo
///
/// @param  const CAPSAPI::NowSelectionInfo& nowSelectionInfo
///
/// @return This method returns a true Boolean value indicating that it has handled the latest now
///         selection. Othewise, application would Exit from AccessoryPairing state which is not
///         desired (Refer PGC-5328 for more details).
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductControllerStateAccessoryPairing::HandleNowSelectionInfo(
    const CAPSAPI::NowSelectionInfo& nowSelectionInfo )
{
    BOSE_INFO( s_logger, "The %s state is in %s.", GetName( ).c_str( ), __func__ );

    using namespace ProductSTS;

    if( nowSelectionInfo.has_contentitem( ) )
    {
        if( nowSelectionInfo.contentitem( ).source( ) == SHELBY_SOURCE::SETUP )
        {
            const auto& sourceAccount = nowSelectionInfo.contentitem( ).sourceaccount( );
            if( sourceAccount == SetupSourceSlot_Name( PAIRING ) )
            {
                return true;
            }
        }
    }
    return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                             End of Product Application Namespace                             ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                        End of File                                           ///
////////////////////////////////////////////////////////////////////////////////////////////////////
