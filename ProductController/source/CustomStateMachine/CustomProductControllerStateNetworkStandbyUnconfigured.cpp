////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file      CustomProductControllerStateNetworkStandbyUnconfigured.cpp
///
/// @brief     This source code file contains functionality to process events that occur during the
///            product network standby unconfigured state.
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
#include "APTimer.h"
#include "ProductControllerHsm.h"
#include "ProductHardwareInterface.h"
#include "CustomProductControllerStateNetworkStandbyUnconfigured.h"
#include "ProfessorProductController.h"

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
constexpr const uint32_t NETWORK_UNCONFIGURED_MILLISECOND_TIMEOUT_START = ( ( 2 * ( 60 * 60 ) ) * 1000 );
constexpr const uint32_t NETWORK_UNCONFIGURED_MILLISECOND_TIMEOUT_RETRY = ( 0 );

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// The following declares a DPrint class type object and a standard string for logging information
/// in this source code file.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
static DPrint s_logger( "Product" );

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStateNetworkStandbyUnconfigured::
///        CustomProductControllerStateNetworkStandbyUnconfigured
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
CustomProductControllerStateNetworkStandbyUnconfigured::
CustomProductControllerStateNetworkStandbyUnconfigured

( ProductControllerHsm&       hsm,
  CHsmState*                  pSuperState,
  ProfessorProductController& productController,
  Hsm::STATE                  stateId,
  const std::string&          name )

    : ProductControllerState( hsm, pSuperState, productController, stateId, name ),
      m_productController( productController ),
      m_timer( APTimer::Create( m_productController.GetTask( ), "NetworkUnconfiguredTimer" ) )

{
    BOSE_DEBUG( s_logger, "The product network standby unconfigured state is being constructed." );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStateNetworkStandbyUnconfigured::HandleStateEnter
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductControllerStateNetworkStandbyUnconfigured::HandleStateEnter()
{
    BOSE_DEBUG( s_logger, "The product network standby unconfigured state is being entered." );
    BOSE_DEBUG( s_logger, "The timer is set to expire in 2 hours unless the network configured." );

    m_timer->SetTimeouts( NETWORK_UNCONFIGURED_MILLISECOND_TIMEOUT_START,
                          NETWORK_UNCONFIGURED_MILLISECOND_TIMEOUT_RETRY  );

    m_timer->Start( std::bind( &CustomProductControllerStateNetworkStandbyUnconfigured::HandleTimeOut,
                               this ) );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStateNetworkStandbyUnconfigured::HandleStateStart
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductControllerStateNetworkStandbyUnconfigured::HandleStateStart()
{
    BOSE_DEBUG( s_logger, "The product network standby unconfigured state is being started." );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStateNetworkStandbyUnconfigured::HandleStateExit
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductControllerStateNetworkStandbyUnconfigured::HandleStateExit()
{
    BOSE_DEBUG( s_logger, "The product network standby unconfigured state is being exited." );
    BOSE_DEBUG( s_logger, "The timer will be stopped." );

    m_timer->Stop( );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStateNetworkStandbyUnconfigured::HandleTimeOut
///
////////////////////////////////////////////////////////////////////////////////////////////////////
 void CustomProductControllerStateNetworkStandbyUnconfigured::HandleTimeOut( void )
 {
      BOSE_DEBUG( s_logger, "A time out in the network standby unconfigured state has occurred." );
      BOSE_DEBUG( s_logger, "An attempt to set the device to a low power state will be made." );

      m_timer->Stop( );

      ChangeState( PROFESSOR_PRODUCT_CONTROLLER_STATE_OFF );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief  CustomProductControllerStateNetworkStandbyUnconfigured::HandleNetworkState
///
/// @param  bool configured
///
/// @param  bool connected
///
/// @return This method returns a true Boolean value indicating that it has handled the network
///         state change and no futher processing will be required by any of its superstates.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductControllerStateNetworkStandbyUnconfigured::HandleNetworkState( bool configured,
                                                                                 bool connected )
{
    BOSE_DEBUG( s_logger, "The product network standby unconfigured state is handling a network state change." );

    if( configured and not connected)
    {
        BOSE_DEBUG( s_logger, "The state is changing to a network standby configured state." );
        ChangeState( PROFESSOR_PRODUCT_CONTROLLER_STATE_NETWORK_STANDBY_CONFIGURED );

        return true;
    }
    else
    {
        bool networkConnected;
        bool voiceConfigured;

        networkConnected = connected;
        voiceConfigured = m_productController.IsVoiceConfigured( );

        if( networkConnected and voiceConfigured )
        {
            BOSE_DEBUG( s_logger, "The product state is changing to an idle state." );
            ChangeState( PROFESSOR_PRODUCT_CONTROLLER_STATE_IDLE );
        }
        else
        {
            BOSE_DEBUG( s_logger, "The product network standby unconfigured state is not changing." );
        }

        return true;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief  CustomProductControllerStateNetworkStandbyUnconfigured::HandleVoiceState
///
/// @param  bool configured
///
/// @return This method returns a true Boolean value indicating that it has handled the voice
///         state change and no futher processing will be required by any of its superstates.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductControllerStateNetworkStandbyUnconfigured::HandleVoiceState( bool configured )
{
    BOSE_DEBUG( s_logger, "The product network standby unconfigured state is handling a voice state change." );

    bool voiceConfigured;
    bool networkConnected;

    voiceConfigured = configured;
    networkConnected = m_productController.IsNetworkConfigured( );

    if( voiceConfigured and networkConnected )
    {
        BOSE_DEBUG( s_logger, "The product state is changing to an idle state." );
        ChangeState( PROFESSOR_PRODUCT_CONTROLLER_STATE_IDLE );
    }
    else
    {
        BOSE_DEBUG( s_logger, "The product network standby unconfigured state is not changing." );
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
