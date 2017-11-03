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
    BOSE_VERBOSE( s_logger, "CustomProductControllerStateIdleVoiceUnconfigured is being exited." );
    BOSE_VERBOSE( s_logger, "CustomProductControllerStateIdleVoiceUnconfigured timer has been stopped." );

    m_timer->Stop( );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStateIdleVoiceUnconfigured::HandleTimeOut
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductControllerStateIdleVoiceUnconfigured::HandleTimeOut( void )
{
    BOSE_VERBOSE( s_logger, "A time out in CustomProductControllerStateIdleVoiceUnconfigured has occurred." );

    m_timer->Stop( );

    if( not GetCustomProductController().IsAutoWakeEnabled( ) )
    {
        BOSE_VERBOSE( s_logger, "CustomProductControllerStateIdleVoiceUnconfigured timer has been stopped." );
        BOSE_VERBOSE( s_logger, "%s is changing to %s.",
                      "CustomProductControllerStateIdleVoiceUnconfigured",
                      "CustomProductControllerStateNetworkStandby" );
        ChangeState( PROFESSOR_PRODUCT_CONTROLLER_STATE_NETWORK_STANDBY );
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
    BOSE_VERBOSE( s_logger, "CustomProductControllerStateIdleVoiceUnconfigured is handling the autowake status." );

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
    BOSE_VERBOSE( s_logger, "CustomProductControllerStateIdleVoiceUnconfigured is handling a network change." );

    ///
    /// If the network is not configured then it must also be unconnected. In these case, change the
    /// state to a network standby state if autowake is not enabled. The timer for this state will be
    /// stop when it exits.
    ///
    if( not configured )
    {
        if( GetCustomProductController().IsAutoWakeEnabled( ) )
        {
            BOSE_VERBOSE( s_logger, "CustomProductControllerStateIdleVoiceUnconfigured is not changing." );
        }
        else
        {
            BOSE_VERBOSE( s_logger, "%s is changing to %s.",
                          "CustomProductControllerStateIdleVoiceUnconfigured",
                          "CustomProductControllerStateNeworkStandby" );
            ChangeState( PROFESSOR_PRODUCT_CONTROLLER_STATE_NETWORK_STANDBY );
        }
    }
    ///
    /// If the network is connected and voice for a Virtual Personal Assistant (VPA) is configured
    /// then change the state to an idle voice configured state. The timer for this state will be
    /// stop when it exits.
    ///
    else
    {
        auto const& networkConnected = connected;
        auto const& voiceConfigured = GetCustomProductController().IsVoiceConfigured( );

        if( networkConnected and voiceConfigured )
        {
            BOSE_VERBOSE( s_logger, "%s is changing to %s.",
                          "CustomProductControllerStateIdleVoiceUnconfigured",
                          "CustomProductControllerStateIdleVoiceConfigured" );
            ChangeState( PROFESSOR_PRODUCT_CONTROLLER_STATE_IDLE_VOICE_CONFIGURED );
        }
        else
        {
            BOSE_VERBOSE( s_logger, "CustomProductControllerStateIdleVoiceUnconfigured is not changing." );
        }
    }

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
bool CustomProductControllerStateIdleVoiceUnconfigured::HandleVoiceState( bool configured )
{
    BOSE_VERBOSE( s_logger, "CustomProductControllerStateIdleVoiceUnconfigured is handling a voice state change." );

    auto const& voiceConfigured  = configured;
    auto const& networkConnected = GetCustomProductController().IsNetworkConnected( );

    if( voiceConfigured and networkConnected )
    {
        BOSE_VERBOSE( s_logger, "%s is changing to %s.",
                      "CustomProductControllerStateIdleVoiceUnconfigured",
                      "CustomProductControllerStateIdleVoiceConfigured" );
        ChangeState( PROFESSOR_PRODUCT_CONTROLLER_STATE_IDLE_VOICE_CONFIGURED );
    }
    else
    {
        BOSE_VERBOSE( s_logger, "CustomProductControllerStateIdleVoiceUnconfigured is not changing." );
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
