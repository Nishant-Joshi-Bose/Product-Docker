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
CustomProductControllerStateOn::CustomProductControllerStateOn( ProductControllerHsm&       hsm,
                                                                CHsmState*                  pSuperState,
                                                                Hsm::STATE                  stateId,
                                                                const std::string&          name )

    : ProductControllerStateOn( hsm, pSuperState, stateId, name )
{
    BOSE_INFO( s_logger, "%s is being constructed.", GetName( ).c_str( ) );
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
/// @brief  CustomProductControllerStateOn::HandleIntentVolumeMuteControl
///
/// @param  action
///
/// @return This method returns a true Boolean value indicating that it has handled the action.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductControllerStateOn::HandleIntentVolumeMuteControl( KeyHandlerUtil::ActionType_t action )
{
    BOSE_INFO( s_logger, "%s in %s is handling the action %u", GetName( ).c_str( ), __FUNCTION__, action );

    GetCustomProductController( ).GetIntentHandler( ).Handle( action );

    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief  CustomProductControllerStateOn::HandleIntentSpeakerPairing
///
/// @param  action
///
/// @return This method returns a true Boolean value indicating that it has handled the action.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductControllerStateOn::HandleIntentSpeakerPairing( KeyHandlerUtil::ActionType_t action )
{
    BOSE_INFO( s_logger, "%s in %s is handling the action %u", GetName( ).c_str( ), __FUNCTION__, action );

    ChangeState( PROFESSOR_PRODUCT_CONTROLLER_STATE_ACCESSORY_PAIRING );

    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief  CustomProductControllerStateOn::HandleIntentPlayback
///
/// @param  int action
///
/// @return This method returns a true Boolean value indicating that it has handled the action.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductControllerStateOn::HandleIntentPlayback( KeyHandlerUtil::ActionType_t action )
{
    BOSE_INFO( s_logger, "%s in %s is handling the action %u", GetName( ).c_str( ), __FUNCTION__, action );

    GetCustomProductController( ).GetIntentHandler( ).Handle( action );

    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                           End of the Product Application Namespace                           ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
