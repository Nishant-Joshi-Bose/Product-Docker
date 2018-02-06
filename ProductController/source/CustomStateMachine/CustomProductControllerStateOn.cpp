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
#include "ProfessorProductController.h"
#include "ProductControllerState.h"

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
///
/// @brief CustomProductControllerStateOn::HandleStateExit
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductControllerStateOn::HandleStateExit( )
{
    BOSE_INFO( s_logger, "GetName( ).c_str( ) is in %s.", __FUNCTION__ );

    GetProductController( ).SendAllowSourceSelectMessage( false );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief  CustomProductControllerStateOn::HandleIntentSpeakerPairing
///
/// @param  KeyHandlerUtil::ActionType_t intent
///
/// @return This method returns a true Boolean value indicating that it has handled the action.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductControllerStateOn::HandleIntentSpeakerPairing( KeyHandlerUtil::ActionType_t intent )
{
    BOSE_INFO( s_logger, "The %s state in %s is handling the intent %u.", GetName( ).c_str( ), __func__, intent );

    ChangeState( PROFESSOR_PRODUCT_CONTROLLER_STATE_ACCESSORY_PAIRING );

    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief  CustomProductControllerStateOn::HandleIntentPlaySoundTouchSource
///
/// @param  KeyHandlerUtil::ActionType_t intent
///
/// @return This method returns a true Boolean value indicating that it has handled the intent.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductControllerStateOn::HandleIntentPlaySoundTouchSource( KeyHandlerUtil::ActionType_t intent )
{
    BOSE_INFO( s_logger, "%s in %s is handling the intent %u", GetName( ).c_str( ), __FUNCTION__, intent );
    // If network is configured, and there's persisted last streaming source, handle intent playSoundTouchSource
    if( GetCustomProductController( ).IsNetworkAvailbleForSoundTouchSource() )
    {
        if( GetCustomProductController( ).GetLastStreamingContentItem().has_source() )
        {
            GetCustomProductController( ).GetIntentHandler( ).Handle( intent );
        }
    }
    // If network is not configured
    else
    {
        ChangeState( PRODUCT_CONTROLLER_STATE_PLAYING_SELECTED_SETUP );
    }
    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief  CustomProductControllerStateOn::HandleIntentPlayTVSource
///
/// @param  KeyHandlerUtil::ActionType_t intent
///
/// @return This method returns a true Boolean value indicating that it has handled the intent.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductControllerStateOn::HandleIntentPlayTVSource( KeyHandlerUtil::ActionType_t intent )
{
    BOSE_INFO( s_logger, "%s in %s is handling the intent %u", GetName( ).c_str( ), __FUNCTION__, intent );
    GetCustomProductController( ).GetIntentHandler( ).Handle( intent );
    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                           End of the Product Application Namespace                           ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
