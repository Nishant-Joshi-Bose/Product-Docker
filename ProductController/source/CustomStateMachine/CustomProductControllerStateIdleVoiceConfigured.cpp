////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file      CustomProductControllerStateIdleVoiceConfigured.cpp
///
/// @brief     This source code file contains functionality to process events that occur during the
///            product idle state when the voice for a Virtual Personal Assistant (VPA) is
///            configured.
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
#include "CustomProductControllerStateIdleVoiceConfigured.h"
#include "ProductControllerHsm.h"
#include "ProfessorProductController.h"
#include "ProductControllerStateIdle.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
///                            Start of Product Application Namespace                            ///
////////////////////////////////////////////////////////////////////////////////////////////////////
namespace ProductApp
{

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStateIdleVoiceConfigured::
///        CustomProductControllerStateIdleVoiceConfigured
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
CustomProductControllerStateIdleVoiceConfigured::CustomProductControllerStateIdleVoiceConfigured

( ProductControllerHsm& hsm,
  CHsmState*            pSuperState,
  Hsm::STATE            stateId,
  const std::string&    name )

    : ProductControllerState( hsm, pSuperState, stateId, name )
{
    BOSE_VERBOSE( s_logger, "CustomProductControllerStateIdleVoiceConfigured is being constructed." );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStateIdleVoiceConfigured::HandleStateEnter
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductControllerStateIdleVoiceConfigured::HandleStateEnter( )
{
    BOSE_VERBOSE( s_logger, "CustomProductControllerStateIdleVoiceConfigured is being entered." );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStateIdleVoiceConfigured::HandleStateStart
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductControllerStateIdleVoiceConfigured::HandleStateStart( )
{
    BOSE_VERBOSE( s_logger, "CustomProductControllerStateIdleVoiceConfigured is being started." );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStateIdleVoiceConfigured::HandleStateExit
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductControllerStateIdleVoiceConfigured::HandleStateExit( )
{
    BOSE_VERBOSE( s_logger, "CustomProductControllerStateIdleVoiceConfigured is being exited." );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStateIdleVoiceConfigured::HandleNetworkState
///
/// @param  bool configured
///
/// @param  bool connected
///
/// @return This method returns a true Boolean value indicating that it has handled the network
///         state changed and no futher processing will be required by any of its superstates.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductControllerStateIdleVoiceConfigured::HandleNetworkState( bool configured,
                                                                          bool connected )
{
    BOSE_VERBOSE( s_logger, "%s is handling a %s %s network state event.",
                  "CustomProductControllerStateIdleVoiceConfigured",
                  configured ? "configured" : "unconfigured,",
                  connected ? "connected" : "unconnected" );

    HandlePotentialStateChange( GetCustomProductController( ).IsAutoWakeEnabled( ),
                                configured,
                                connected,
                                GetCustomProductController().IsVoiceConfigured( ) );
    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief  CustomProductControllerStateIdleVoiceConfigured::HandleVoiceState
///
/// @param  bool configured
///
/// @return This method returns a true Boolean value indicating that it has handled the voice
///         state change and no futher processing will be required by any of its superstates.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductControllerStateIdleVoiceConfigured::HandleVoiceState( bool configured )
{
    BOSE_VERBOSE( s_logger, "%s is handling a %s voice state event.",
                  "CustomProductControllerStateIdleVoiceConfigured",
                  configured ? "configured" : "unconfigured" );

    HandlePotentialStateChange( GetCustomProductController( ).IsAutoWakeEnabled( ),
                                GetCustomProductController( ).IsNetworkConfigured( ),
                                GetCustomProductController( ).IsNetworkConnected( ),
                                configured );
    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStateIdleVoiceConfigured::HandlePotentialStateChange
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductControllerStateIdleVoiceConfigured::HandlePotentialStateChange
( bool autoWakeEnabled,
  bool networkConfigured,
  bool networkConnected,
  bool voiceConfigured )
{
    if( not networkConfigured and not autoWakeEnabled )
    {
        BOSE_VERBOSE( s_logger, "%s is changing to %s.",
                      "CustomProductControllerStateIdleVoiceConfigured",
                      "CustomProductControllerStateNetworkStandbyUnconfigured" );
        ChangeState( PROFESSOR_PRODUCT_CONTROLLER_STATE_NETWORK_STANDBY_UNCONFIGURED );
    }
    else if( not networkConnected or not voiceConfigured )
    {
        BOSE_VERBOSE( s_logger, "%s is changing to %s.",
                      "CustomProductControllerStateIdleVoiceConfigured",
                      "CustomProductControllerStateIdleVoiceUnconfigured" );
        ChangeState( PROFESSOR_PRODUCT_CONTROLLER_STATE_IDLE_VOICE_UNCONFIGURED );
    }
    else
    {
        BOSE_VERBOSE( s_logger, "%s is not changing.",
                      "CustomProductControllerStateIdleVoiceConfigured" );
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                           End of the Product Application Namespace                           ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
