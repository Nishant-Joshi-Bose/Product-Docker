////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file      CustomProductControllerStateNetworkStandbyConfigured.cpp
///
/// @brief     This source code file contains functionality to process events that occur during the
///            product network standby configured state.
///
/// @author    Stuart J. Lumby
///
/// @date      10/24/2017
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
#include "DPrint.h"
#include "Utilities.h"
#include "CustomProductControllerStateNetworkStandbyConfigured.h"
#include "ProductControllerHsm.h"
#include "ProfessorProductController.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
///                            Start of Product Application Namespace                            ///
////////////////////////////////////////////////////////////////////////////////////////////////////
namespace ProductApp
{

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStateNetworkStandbyConfigured::
///        CustomProductControllerStateNetworkStandbyConfigured
///
/// @param hsm
///
/// @param pSuperState
///
/// @param productController
///
/// @param stateId
///
/// @param name
///
////////////////////////////////////////////////////////////////////////////////////////////////////
CustomProductControllerStateNetworkStandbyConfigured::
CustomProductControllerStateNetworkStandbyConfigured

( ProductControllerHsm&       hsm,
  CHsmState*                  pSuperState,
  Hsm::STATE                  stateId,
  const std::string&          name )
    : ProductControllerState( hsm, pSuperState, stateId, name )
{
    BOSE_VERBOSE( s_logger, "CustomProductControllerStateNetworkStandbyConfigured is being constructed." );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStateNetworkStandbyConfigured::HandleStateEnter
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductControllerStateNetworkStandbyConfigured::HandleStateEnter()
{
    BOSE_VERBOSE( s_logger, "CustomProductControllerStateNetworkStandbyConfigured is being entered." );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStateNetworkStandbyConfigured::HandleStateStart
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductControllerStateNetworkStandbyConfigured::HandleStateStart()
{
    BOSE_VERBOSE( s_logger, "CustomProductControllerStateNetworkStandbyConfigured is being started." );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStateNetworkStandbyConfigured::HandleStateExit
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductControllerStateNetworkStandbyConfigured::HandleStateExit()
{
    BOSE_VERBOSE( s_logger, "CustomProductControllerStateNetworkStandbyConfigured is being exited." );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief  CustomProductControllerStateNetworkStandbyConfigured::HandleNetworkState
///
/// @param  bool configured
///
/// @param  bool connected
///
/// @return This method returns a true Boolean value indicating that it has handled the network
///         state change and no futher processing will be required by any of its superstates.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductControllerStateNetworkStandbyConfigured::HandleNetworkState( bool configured,
                                                                               bool connected )
{
    BOSE_VERBOSE( s_logger, "CustomProductControllerStateNetworkStandbyConfigured is handling a network state change." );

    if( not configured )
    {
        BOSE_VERBOSE( s_logger, "%s is changing to %s.",
                      "CustomProductControllerStateNetworkStandbyConfigured",
                      "CustomProductControllerStateNetworkStandbyUnconfigured" );
        ChangeState( PROFESSOR_PRODUCT_CONTROLLER_STATE_NETWORK_STANDBY_UNCONFIGURED );
    }
    else
    {
        auto const& networkConnected = connected;
        auto const& voiceConfigured = GetCustomProductController().IsVoiceConfigured( );

        if( networkConnected and voiceConfigured )
        {
            BOSE_VERBOSE( s_logger, "%s is changing to %s.",
                          "CustomProductControllerStateNetworkStandbyConfigured",
                          "CustomProductControllerStateIdle" );
            ChangeState( PROFESSOR_PRODUCT_CONTROLLER_STATE_IDLE );
        }
        else
        {
            BOSE_VERBOSE( s_logger, "CustomProductControllerStateNetworkStandbyConfigured is not changing state." );
        }
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief  CustomProductControllerStateNetworkStandbyConfigured::HandleVoiceState
///
/// @param  bool configured
///
/// @return This method returns a true Boolean value indicating that it has handled the voice
///         state change and no futher processing will be required by any of its superstates.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductControllerStateNetworkStandbyConfigured::HandleVoiceState( bool configured )
{
    BOSE_VERBOSE( s_logger, "CustomProductControllerStateNetworkStandbyConfigured is handling a voice state change." );

    auto const& voiceConfigured = configured;
    auto const& networkConnected = GetCustomProductController().IsNetworkConnected( );

    if( voiceConfigured and networkConnected )
    {
        BOSE_VERBOSE( s_logger, "%s is changing to %s.",
                      "CustomProductControllerStateNetworkStandbyConfigured",
                      "CustomProductControllerStateIdle" );
        ChangeState( PROFESSOR_PRODUCT_CONTROLLER_STATE_IDLE );
    }
    else
    {
        BOSE_VERBOSE( s_logger, "CustomProductControllerStateNetworkStandbyConfigured is not changing." );
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                           End of the Product Application Namespace                           ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
