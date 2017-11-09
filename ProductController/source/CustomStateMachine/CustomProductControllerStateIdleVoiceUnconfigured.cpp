////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file      CustomProductControllerStateIdleVoiceUnconfigured.cpp
///
/// @brief     This source code file contains functionality to process events that occur during the
///            product idle state when the voice for a Virtual Personal Assistant (VPA) is
///            unconfigured.
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
#include "CustomProductControllerStateIdleVoiceUnconfigured.h"
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
///            Constant Definitions
///
////////////////////////////////////////////////////////////////////////////////////////////////////
constexpr uint32_t VOICE_UNCONFIGURED_MILLISECOND_TIMEOUT_START = ( 20 * 60 ) * 1000;
constexpr uint32_t VOICE_UNCONFIGURED_MILLISECOND_TIMEOUT_RETRY = 0;

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStateIdleVoiceUnconfigured::
///        CustomProductControllerStateIdleVoiceUnconfigured
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
CustomProductControllerStateIdleVoiceUnconfigured::CustomProductControllerStateIdleVoiceUnconfigured

( ProductControllerHsm&       hsm,
  CHsmState*                  pSuperState,
  ProfessorProductController& productController,
  Hsm::STATE                  stateId,
  const std::string&          name )

    : ProductControllerState( hsm, pSuperState, stateId, name ),
      m_timer( APTimer::Create( productController.GetTask( ), "VoiceUnconfiguredTimer" ) )
{
    BOSE_VERBOSE( s_logger, "CustomProductControllerStateIdleVoiceUnconfigured is being constructed." );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStateIdleVoiceUnconfigured::HandleStateEnter
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductControllerStateIdleVoiceUnconfigured::HandleStateEnter( )
{
    BOSE_VERBOSE( s_logger, "CustomProductControllerStateIdleVoiceUnconfigured is being entered." );

    BOSE_VERBOSE( s_logger, "A timer has been set to expire in %d minutes.",
                  VOICE_UNCONFIGURED_MILLISECOND_TIMEOUT_START / 60000 );

    m_timer->SetTimeouts( VOICE_UNCONFIGURED_MILLISECOND_TIMEOUT_START,
                          VOICE_UNCONFIGURED_MILLISECOND_TIMEOUT_RETRY );

    m_timer->Start( std::bind( &CustomProductControllerStateIdleVoiceUnconfigured::HandleTimeOut,
                               this ) );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStateIdleVoiceUnconfigured::HandleStateStart
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductControllerStateIdleVoiceUnconfigured::HandleStateStart( )
{
    BOSE_VERBOSE( s_logger, "CustomProductControllerStateIdleVoiceUnconfigured is being started." );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStateIdleVoiceUnconfigured::HandleStateExit
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductControllerStateIdleVoiceUnconfigured::HandleStateExit( )
{
    m_timer->Stop( );

    BOSE_VERBOSE( s_logger, "CustomProductControllerStateIdleVoiceUnconfigured timer has been stopped." );
    BOSE_VERBOSE( s_logger, "CustomProductControllerStateIdleVoiceUnconfigured is being exited." );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStateIdleVoiceUnconfigured::HandleTimeOut
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductControllerStateIdleVoiceUnconfigured::HandleTimeOut( )
{
    BOSE_VERBOSE( s_logger, "The timer in CustomProductControllerStateIdleVoiceUnconfigured has expired." );

    if( not GetCustomProductController( ).IsAutoWakeEnabled( ) )
    {
        if( GetCustomProductController( ).IsNetworkConfigured( ) )
        {
            BOSE_VERBOSE( s_logger, "%s is changing to %s.",
                          "CustomProductControllerStateIdleVoiceUnconfigured",
                          "CustomProductControllerStateNetworkStandbyUnconfigured" );
            ChangeState( PROFESSOR_PRODUCT_CONTROLLER_STATE_NETWORK_STANDBY_UNCONFIGURED );
        }
        else
        {
            BOSE_VERBOSE( s_logger, "%s is changing to %s.",
                          "CustomProductControllerStateIdleVoiceUnconfigured",
                          "CustomProductControllerStateNetworkStandbyConfigured" );
            ChangeState( PROFESSOR_PRODUCT_CONTROLLER_STATE_NETWORK_STANDBY_CONFIGURED );
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief  CustomProductControllerStateIdleVoiceUnconfigured::HandleAutowakeStatus
///
/// @param  bool active
///
/// @return This method returns a true Boolean value indicating that it has handled the autowake
///         state change and no futher processing will be required by any of its superstates.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductControllerStateIdleVoiceUnconfigured::HandleAutowakeStatus( bool active )
{
    BOSE_VERBOSE( s_logger, "%s is handling an autowake %s.",
                  "CustomProductControllerStateIdleVoiceUnconfigured",
                  active ? "activation" : "deactivation" );

    if( active )
    {
        m_timer->Stop( );
    }
    else
    {
        BOSE_VERBOSE( s_logger, "The timer will be set to expire in %d minutes.",
                      VOICE_UNCONFIGURED_MILLISECOND_TIMEOUT_START / 60000 );

        m_timer->SetTimeouts( VOICE_UNCONFIGURED_MILLISECOND_TIMEOUT_START,
                              VOICE_UNCONFIGURED_MILLISECOND_TIMEOUT_RETRY );

        m_timer->Start( std::bind( &CustomProductControllerStateIdleVoiceUnconfigured::HandleTimeOut,
                                   this ) );
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief  CustomProductControllerStateIdleVoiceUnconfigured::HandleNetworkState
///
/// @param  bool configured
///
/// @param  bool connected
///
/// @return This method returns a true Boolean value indicating that it has handled the network
///         state changed and no futher processing will be required by any of its superstates.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductControllerStateIdleVoiceUnconfigured::HandleNetworkState( bool configured,
                                                                            bool connected )
{
    BOSE_VERBOSE( s_logger, "%s is handling a %s %s network state event.",
                  "CustomProductControllerStateIdleVoiceUnconfigured",
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
/// @brief  CustomProductControllerStateIdleVoiceUnconfigured::HandleVoiceState
///
/// @param  bool configured
///
/// @return This method returns a true Boolean value indicating that it has handled the voice
///         state change and no futher processing will be required by any of its superstates.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductControllerStateIdleVoiceUnconfigured::HandleVoiceState( bool configured )
{
    BOSE_VERBOSE( s_logger, "%s is handling a %s voice state event.",
                  "CustomProductControllerStateIdleVoiceUnconfigured",
                  configured ? "configured" : "unconfigured" );

    HandlePotentialStateChange( GetCustomProductController( ).IsAutoWakeEnabled( ),
                                GetCustomProductController( ).IsNetworkConfigured( ),
                                GetCustomProductController( ).IsNetworkConnected( ),
                                configured );
    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStateIdleVoiceUnconfigured::HandlePotentialStateChange
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductControllerStateIdleVoiceUnconfigured::HandlePotentialStateChange
( bool autoWakeEnabled,
  bool networkConfigured,
  bool networkConnected,
  bool voiceConfigured )
{
    if( not networkConfigured and not autoWakeEnabled )
    {
        BOSE_VERBOSE( s_logger, "%s is changing to %s.",
                      "CustomProductControllerStateIdleVoiceUnconfigured",
                      "CustomProductControllerStateNetworkStandbyUnconfigured" );
        ChangeState( PROFESSOR_PRODUCT_CONTROLLER_STATE_NETWORK_STANDBY_UNCONFIGURED );
    }
    else if( networkConnected and voiceConfigured )
    {
        BOSE_VERBOSE( s_logger, "%s is changing to %s.",
                      "CustomProductControllerStateIdleVoiceUnconfigured",
                      "CustomProductControllerStateIdleVoiceConfigured" );
        ChangeState( PROFESSOR_PRODUCT_CONTROLLER_STATE_IDLE_VOICE_CONFIGURED );
    }
    else
    {
        BOSE_VERBOSE( s_logger, "%s is not changing.",
                      "CustomProductControllerStateIdleVoiceUnconfigured" );
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                           End of the Product Application Namespace                           ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
