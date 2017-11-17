////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file      CustomProductControllerStateNetworkStandbyConfigured.cpp
///
/// @brief     This source code file contains functionality to process events that occur during the
///            product network standby configured state.
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
/// @param ProductControllerHsm& hsm
///
/// @param CHsmState*            pSuperState
///
/// @param Hsm::STATE            stateId
///
/// @param const std::string&    name
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
    BOSE_VERBOSE( s_logger, "%s is handling a %s %s network state event.",
                  "CustomProductControllerStateNetworkStandbyConfigured",
                  configured ? "configured" : "unconfigured,",
                  connected ? "connected" : "unconnected" );

    GoToAppropriatePlayableState( configured,
                                  connected,
                                  GetCustomProductController().IsVoiceConfigured( ) );
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
    BOSE_VERBOSE( s_logger, "%s is handling a %s voice state event.",
                  "CustomProductControllerStateNetworkStandbyConfigured",
                  configured ? "configured" : "unconfigured" );

    GoToAppropriatePlayableState( GetCustomProductController( ).IsNetworkConfigured( ),
                                  GetCustomProductController( ).IsNetworkConnected( ),
                                  configured );
    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStateNetworkStandbyConfigured::GoToAppropriatePlayableState
///
/// @param bool networkConfigured
///
/// @param bool networkConnected
///
/// @param bool voiceConfigured
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductControllerStateNetworkStandbyConfigured::GoToAppropriatePlayableState
( bool networkConfigured,
  bool networkConnected,
  bool voiceConfigured )
{
    if( networkConnected and voiceConfigured )
    {
        BOSE_VERBOSE( s_logger, "%s is changing to %s.",
                      "CustomProductControllerStateNetworkStandbyConfigured",
                      "CustomProductControllerStateIdleVoiceConfigured" );
        ChangeState( PROFESSOR_PRODUCT_CONTROLLER_STATE_IDLE_VOICE_CONFIGURED );
    }
    else if( not networkConfigured )
    {
        BOSE_VERBOSE( s_logger, "%s is changing to %s.",
                      "CustomProductControllerStateNetworkStandbyConfigured",
                      "CustomProductControllerStateNetworkStandbyUnconfigured" );
        ChangeState( PROFESSOR_PRODUCT_CONTROLLER_STATE_NETWORK_STANDBY_UNCONFIGURED );
    }
    else
    {
        BOSE_VERBOSE( s_logger, "%s is not changing.",
                      "CustomProductControllerStateNetworkStandbyConfigured" );
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                           End of the Product Application Namespace                           ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
