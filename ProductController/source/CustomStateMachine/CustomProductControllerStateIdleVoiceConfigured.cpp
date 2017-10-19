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
/// The following declares a DPrint class type object and a standard string for logging information
/// in this source code file.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
static DPrint s_logger( "Product" );

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

( ProductControllerHsm&       hsm,
  CHsmState*                  pSuperState,
  ProfessorProductController& productController,
  Hsm::STATE                  stateId,
  const std::string&          name )

    : ProductControllerState( hsm, pSuperState, productController, stateId, name ),
      m_productController( productController )
{
    BOSE_DEBUG( s_logger, "The product idle voice configured state is being constructed." );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStateIdleVoiceConfigured::HandleStateEnter
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductControllerStateIdleVoiceConfigured::HandleStateEnter( )
{
    BOSE_DEBUG( s_logger, "The product idle voice configured state is being entered." );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStateIdleVoiceConfigured::HandleStateStart
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductControllerStateIdleVoiceConfigured::HandleStateStart( )
{
    BOSE_DEBUG( s_logger, "The product idle voice configured state is being started." );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStateIdleVoiceConfigured::HandleStateExit
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductControllerStateIdleVoiceConfigured::HandleStateExit( )
{
    BOSE_DEBUG( s_logger, "The product idle voice configured state is being exited." );
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
    BOSE_DEBUG( s_logger, "The product idle voice configured state is handling a network state change." );

    ///
    /// If the network is not configured then it must also be unconnected. In these case, change the
    /// state to a network standby state if autowake is not enabled.
    ///
    if( not configured )
    {
        if( m_productController.IsAutoWakeEnabled( ) )
        {
            BOSE_DEBUG( s_logger, "The product idle voice configured state is not changing." );
        }
        else
        {
            BOSE_DEBUG( s_logger, "The state is changing to a network standby state." );
            ChangeState( PROFESSOR_PRODUCT_CONTROLLER_STATE_NETWORK_STANDBY );
        }

        return true;
    }
    ///
    /// If the network is unconnected or voice for a Virtual Personal Assistant (VPA) is not
    /// configured then change the state to an idle voice unconfigured state.
    ///
    else
    {
        bool networkConnected;
        bool voiceConfigured;

        networkConnected = connected;
        voiceConfigured = m_productController.IsVoiceConfigured( );

        if( not networkConnected or not voiceConfigured )
        {
            BOSE_DEBUG( s_logger, "The state is changing to an idle voice unconfigured state." );
            ChangeState( PROFESSOR_PRODUCT_CONTROLLER_STATE_IDLE_VOICE_UNCONFIGURED );
        }
        else
        {
            BOSE_DEBUG( s_logger, "The product idle voice configured state is not changing." );
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
bool CustomProductControllerStateIdleVoiceConfigured::HandleVoiceState( bool configured )
{
    BOSE_DEBUG( s_logger, "The product idle voice configured state is handling a voice state change." );

    bool voiceConfigured;
    bool networkConnected;

    voiceConfigured = configured;
    networkConnected = m_productController.IsNetworkConfigured( );

    if( not voiceConfigured or not networkConnected )
    {
        BOSE_DEBUG( s_logger, "The product idle state is changing to a voice unconfigured state." );
        ChangeState( PROFESSOR_PRODUCT_CONTROLLER_STATE_IDLE_VOICE_UNCONFIGURED );
    }
    else
    {
        BOSE_DEBUG( s_logger, "The product idle voice configured state is not changing." );
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
