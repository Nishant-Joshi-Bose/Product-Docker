////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file      CustomProductControllerStatePlaying.cpp
///
/// @brief     This source code file contains functionality to process events that occur during the
///            product playing state.
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
#include "CustomProductControllerStatePlaying.h"
#include "ProductControllerHsm.h"
#include "ProductControllerState.h"
#include "CustomProductLpmHardwareInterface.h"
#include "ProfessorProductController.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
///                            Start of Product Application Namespace                            ///
////////////////////////////////////////////////////////////////////////////////////////////////////
namespace ProductApp
{

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStatePlaying::CustomProductControllerStatePlaying
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
CustomProductControllerStatePlaying::CustomProductControllerStatePlaying(
        ProductControllerHsm&       hsm,
        CHsmState*                  pSuperState,
        Hsm::STATE                  stateId,
        const std::string&          name )

    : ProductControllerStatePlaying( hsm, pSuperState, stateId, name )
{
    BOSE_INFO( s_logger, "The %s state is being constructed.", name.c_str() );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name CustomProductControllerStatePlaying::HandleStateEnter
///
/// @brief The CEC is powered on when this state is entered, which occurs after the playing
///        transition state has brought the product to full power.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductControllerStatePlaying::HandleStateEnter( )
{
    BOSE_INFO( s_logger, "The %s state is in %s powering CEC on.", GetName( ).c_str( ), __func__ );

    GetCustomProductController( ).GetCecHelper( )->PowerOn( );
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
bool CustomProductControllerStatePlaying::HandleIntentMuteControl( KeyHandlerUtil::ActionType_t action )
{
    BOSE_INFO( s_logger, "%s in %s is handling the intent action %u", GetName( ).c_str( ), __FUNCTION__, action );

    GetCustomProductController( ).GetIntentHandler( ).Handle( action );

    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name  CustomProductControllerStatePlaying::HandleStateExit
///
/// @brief The CEC is powered off when this state is exited occuring before a change to the
///        playable transition state takes place, which will bring the product to idle or network
///        standby power mode.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductControllerStatePlaying::HandleStateExit( )
{
    BOSE_INFO( s_logger, "The %s state is in %s powering CEC off.", GetName( ).c_str( ), __func__ );

    GetCustomProductController( ).GetCecHelper( )->PowerOff( );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                           End of the Product Application Namespace                           ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
