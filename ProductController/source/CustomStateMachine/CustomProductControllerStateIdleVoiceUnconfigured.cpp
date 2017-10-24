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
/// @date      09/22/2017
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
#include "CustomProductControllerStateIdleVoiceUnconfigured.h"
#include "ProductControllerHsm.h"
#include "ProfessorProductController.h"
#include "ProductControllerStateIdle.h"
#include "APTimer.h"

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
constexpr const uint32_t VOICE_UNCONFIGURED_MILLISECOND_TIMEOUT_START = ( ( 20 * 60 ) * 1000 );
constexpr const uint32_t VOICE_UNCONFIGURED_MILLISECOND_TIMEOUT_RETRY = ( 0 );

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// The following declares a DPrint class type object and a standard string for logging information
/// in this source code file.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
static DPrint s_logger( "Product" );

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

    : ProductControllerState( hsm, pSuperState, productController, stateId, name ),
      m_productController( productController ),
      m_timer( APTimer::Create( m_productController.GetTask( ), "VoiceUnconfiguredTimer" ) )
{
    BOSE_DEBUG( s_logger, "The product idle voice unconfigured state is being constructed." );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStateIdleVoiceUnconfigured::HandleStateEnter
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductControllerStateIdleVoiceUnconfigured::HandleStateEnter( )
{
    BOSE_DEBUG( s_logger, "The product idle voice unconfigured state is being entered." );
    BOSE_DEBUG( s_logger, "The timer will be set to expire in 20 minutes." );

    m_timer->SetTimeouts( VOICE_UNCONFIGURED_MILLISECOND_TIMEOUT_START,
                          VOICE_UNCONFIGURED_MILLISECOND_TIMEOUT_RETRY  );

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
    BOSE_DEBUG( s_logger, "The product idle voice unconfigured state is being started." );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStateIdleVoiceUnconfigured::HandleStateExit
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductControllerStateIdleVoiceUnconfigured::HandleStateExit( )
{
    BOSE_DEBUG( s_logger, "The product idle voice unconfigured state is being exited." );
    BOSE_DEBUG( s_logger, "The timer will be stopped." );

    m_timer->Stop( );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStateIdleVoiceUnconfigured::HandleTimeOut
///
////////////////////////////////////////////////////////////////////////////////////////////////////
 void CustomProductControllerStateIdleVoiceUnconfigured::HandleTimeOut( void )
 {
      BOSE_DEBUG( s_logger, "A time out in the idle voice unconfigured state has occurred." );

      m_timer->Stop( );

      if( m_productController.IsAutoWakeEnabled( ) )
      {
          BOSE_DEBUG( s_logger, "The product idle voice unconfigured state is not changing." );
          BOSE_DEBUG( s_logger, "The timer will be set to expire in 20 minutes." );

          m_timer->SetTimeouts( VOICE_UNCONFIGURED_MILLISECOND_TIMEOUT_START,
                                VOICE_UNCONFIGURED_MILLISECOND_TIMEOUT_RETRY  );

          m_timer->Start( std::bind( &CustomProductControllerStateIdleVoiceUnconfigured::HandleTimeOut,
                                     this ) );
      }
      else
      {
          BOSE_DEBUG( s_logger, "An attempt to set the device to a network standby state will be made." );
          BOSE_DEBUG( s_logger, "The timer has been stopped." );

          ChangeState( PROFESSOR_PRODUCT_CONTROLLER_STATE_NETWORK_STANDBY );
      }
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
    BOSE_DEBUG( s_logger, "The product idle voice unconfigured state is handling a network state change." );

    ///
    /// If the network is not configured then it must also be unconnected. In these case, change the
    /// state to a network standby state if autowake is not enabled. The timer for this state will be
    /// stop when it exits.
    ///
    if( not configured )
    {
        if( m_productController.IsAutoWakeEnabled( ) )
        {
            BOSE_DEBUG( s_logger, "The product idle voice unconfigured state is not changing." );
        }
        else
        {
            BOSE_DEBUG( s_logger, "The state is changing to a network standby state." );
            ChangeState( PROFESSOR_PRODUCT_CONTROLLER_STATE_NETWORK_STANDBY );
        }

        return true;
    }
    ///
    /// If the network is connected and voice for a Virtual Personal Assistant (VPA) is configured
    /// then change the state to an idle voice configured state. The timer for this state will be
    /// stop when it exits.
    ///
    else
    {
        bool networkConnected;
        bool voiceConfigured;

        networkConnected = connected;
        voiceConfigured = m_productController.IsVoiceConfigured( );

        if( networkConnected and voiceConfigured )
        {
            BOSE_DEBUG( s_logger, "The state is changing to an idle voice configured state." );
            ChangeState( PROFESSOR_PRODUCT_CONTROLLER_STATE_IDLE_VOICE_CONFIGURED );
        }
        else
        {
            BOSE_DEBUG( s_logger, "The product idle voice unconfigured state is not changing." );
        }

        return true;
    }
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
    BOSE_DEBUG( s_logger, "The product idle voice configured state is handling a voice state change." );

    bool voiceConfigured;
    bool networkConnected;

    voiceConfigured = configured;
    networkConnected = m_productController.IsNetworkConnected( );

    if( voiceConfigured and networkConnected )
    {
        BOSE_DEBUG( s_logger, "The product idle state is changing to a voice configured state." );
        ChangeState( PROFESSOR_PRODUCT_CONTROLLER_STATE_IDLE_VOICE_CONFIGURED );
    }
    else
    {
        BOSE_DEBUG( s_logger, "The product idle voice unconfigured state is not changing." );
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                             End of Product Application Namespace                             ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                        End of File                                           ///
////////////////////////////////////////////////////////////////////////////////////////////////////
