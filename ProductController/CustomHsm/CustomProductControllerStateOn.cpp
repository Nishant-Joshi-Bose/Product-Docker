////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file      CustomProductControllerStateOn.cpp
///
/// @brief     This source code file contains functionality to process events that occur during the
///            product on state.
///
/// @author    Stuart J. Lumby
///
/// @attention Copyright (C) 2017 Bose Corporation All Rights Reserved
///
///            Bose Corporation
///            The Mountain Road,
///            Framingham, MA 01701-9168
///            U.S.A.
///
///            This program may not be reproduced, in whole or in part, in any form by any means
///            whatsoever without the written permission of Bose Corporation.
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
#include "CustomProductControllerStateOn.h"
#include "ProductControllerHsm.h"
#include "CustomProductController.h"
#include "ProductControllerState.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
///
///            Constant Definitions
///
////////////////////////////////////////////////////////////////////////////////////////////////////
namespace
{
constexpr uint32_t MANUAL_BLE_REMOTE_PAIRING_TIMEOUT_SECONDS = 120;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                            Start of Product Application Namespace                            ///
////////////////////////////////////////////////////////////////////////////////////////////////////
namespace ProductApp
{

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStateOn::CustomProductControllerStateOn
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
CustomProductControllerStateOn::CustomProductControllerStateOn( ProductControllerHsm& hsm,
                                                                CHsmState*            pSuperState,
                                                                Hsm::STATE            stateId,
                                                                const std::string&    name )

    : ProductControllerStateOn( hsm, pSuperState, stateId, name )
{
    BOSE_INFO( s_logger, "The %s state is being constructed.", GetName( ).c_str( ) );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief CustomProductControllerStateOn::HandleStateEnter
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductControllerStateOn::HandleStateEnter( )
{
    ProductControllerStateOn::HandleStateEnter( );

    if( s_ActionPendingFromTransition != ( KeyHandlerUtil::ActionType_t )Action::INVALID )
    {
        GetCustomProductController( ).GetIntentHandler( ).Handle( s_ActionPendingFromTransition );
    }
    s_ActionPendingFromTransition = ( KeyHandlerUtil::ActionType_t )Action::INVALID;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief  CustomProductControllerStateOn::HandleIntentPlayProductSource
///
/// @param  KeyHandlerUtil::ActionType_t intent
///
/// @return This method returns a true Boolean value indicating that it has handled the intent.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductControllerStateOn::HandleIntentPlayProductSource( KeyHandlerUtil::ActionType_t intent )
{
    BOSE_INFO( s_logger, "The %s state in %s is handling the intent %s", GetName( ).c_str( ), __func__, CommonIntentHandler::GetIntentName( intent ).c_str( ) );

    GetCustomProductController( ).GetIntentHandler( ).Handle( intent );

    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief  CustomProductControllerStateOn::HandleIntentRating
///
/// @param  KeyHandlerUtil::ActionType_t intent
///
/// @return This method returns a true Boolean value indicating that it has handled the intent.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductControllerStateOn::HandleIntentRating( KeyHandlerUtil::ActionType_t intent )
{
    BOSE_INFO( s_logger, "The %s state in %s is handling the intent %s", GetName( ).c_str( ), __func__, CommonIntentHandler::GetIntentName( intent ).c_str( ) );

    GetCustomProductController( ).GetIntentHandler( ).Handle( intent );

    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief  CustomProductControllerStateOn::HandleAdaptIQControl
///
/// @param  const ProductAdaptIQControl& cmd
///
/// @return This method returns a true Boolean value indicating that it has handled the
///         AdaptIQControl request.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductControllerStateOn::HandleAdaptIQControl( const ProductAdaptIQControl& cmd )
{
    BOSE_INFO( s_logger, "The %s state is in %s.", GetName( ).c_str( ), __func__ );

    if( cmd.action() == ProductAdaptIQControl::Start )
    {
        unsigned int startAdaptIQ = static_cast< unsigned int >( Action::ACTION_APAPTIQ_START );

        GetCustomProductController( ).GetIntentHandler( ).Handle( startAdaptIQ );

        return true;
    }

    return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief  CustomProductControllerStateOn::HandleIntentSpeakerPairing
///
/// @return This method returns a true Boolean value indicating that it has handled the event
///         and no futher processing will be required by any of its superstates.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductControllerStateOn::HandleIntentSpeakerPairing( KeyHandlerUtil::ActionType_t intent )
{
    BOSE_INFO( s_logger, "The %s state in %s is handling the intent %s", GetName( ).c_str( ), __func__, CommonIntentHandler::GetIntentName( intent ).c_str( ) );

    if( intent == ( KeyHandlerUtil::ActionType_t )Action::ACTION_START_PAIR_SPEAKERS ||
        intent == ( KeyHandlerUtil::ActionType_t )Action::ACTION_START_PAIR_SPEAKERS_LAN )
    {
        GetCustomProductController( ).GetIntentHandler( ).Handle( intent );
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief  CustomProductControllerStateOn::HandleIntentSetupBLERemote
///
/// @return This method returns a true Boolean value indicating that it has handled the intent.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductControllerStateOn::HandleIntentSetupBLERemote( )
{
    BOSE_INFO( s_logger, "The %s state is in %s: remote is %sconnected, network is %sconnected, PassportAccount %s",
               GetName( ).c_str( ), __func__ , GetCustomProductController().IsBLERemoteConnected() ? "" : "not ",
               GetCustomProductController().GetNetworkServiceUtil().IsNetworkConnected() ? "" : "not ",
               PassportPB::AssociateStatus_Name( GetCustomProductController().GetPassportAccountAssociationStatus() ).c_str() );

    ///
    /// @note Conditions for initiating pairing: this feature shall be disabled when: active bonded
    ///       remote is connected or (active network connection and product associated with a My Bose
    ///        account received)
    ///
    /// @todo IsProductSettingsReceived() is used as a proxy for "product associated with a My Bose account",
    ///       see CASTLE-13960
    ///
    if( !( GetCustomProductController().IsBLERemoteConnected() ||
           ( GetCustomProductController().GetNetworkServiceUtil().IsNetworkConnected() &&
             ( GetCustomProductController().GetPassportAccountAssociationStatus() == PassportPB::ASSOCIATED ) ) ) )
    {
        GetCustomProductController().PairBLERemote( MANUAL_BLE_REMOTE_PAIRING_TIMEOUT_SECONDS );
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief  CustomProductControllerStateOn::HandleIntentMuteControl
///
/// @param  action
///
/// @return This method returns a true Boolean value indicating that it has handled the action.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductControllerStateOn::HandleIntentMuteControl( KeyHandlerUtil::ActionType_t intent )
{
    BOSE_INFO( s_logger, "The %s state in %s is handling the intent %s", GetName( ).c_str( ), __func__, CommonIntentHandler::GetIntentName( intent ).c_str( ) );

    GetCustomProductController( ).GetIntentHandler( ).Handle( intent );

    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief  CustomProductControllerStateOn::ShouldPlayVolumeTone
/// @param  canPlay , reference to bool variable
/// @return This method returns false
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductControllerStateOn::ShouldPlayVolumeTone( bool & /* canPlay */ )
{
    BOSE_INFO( s_logger, "The %s state is in %s", GetName( ).c_str( ), __func__ );
    return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                           End of the Product Application Namespace                           ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
