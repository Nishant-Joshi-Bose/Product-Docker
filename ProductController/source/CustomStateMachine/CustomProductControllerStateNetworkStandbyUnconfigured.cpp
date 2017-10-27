////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file      CustomProductControllerStateNetworkStandbyUnconfigured.cpp
///
/// @brief     This source code file contains functionality to process events that occur during the
///            product network standby unconfigured state.
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
constexpr uint32_t NETWORK_UNCONFIGURED_MILLISECOND_TIMEOUT_START = ( 2 * ( 60 * 60 ) ) * 1000;
constexpr uint32_t NETWORK_UNCONFIGURED_MILLISECOND_TIMEOUT_RETRY =   0 ;

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
    BOSE_VERBOSE( s_logger, "CustomProductControllerStateNetworkStandbyUnconfigured is being constructed." );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStateNetworkStandbyUnconfigured::HandleStateEnter
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductControllerStateNetworkStandbyUnconfigured::HandleStateEnter()
{
    BOSE_VERBOSE( s_logger, "CustomProductControllerStateNetworkStandbyUnconfigured is being entered." );
    BOSE_VERBOSE( s_logger, "The timer is set to expire in %d minutes unless the network is configured.",
                  NETWORK_UNCONFIGURED_MILLISECOND_TIMEOUT_START / 60000 );

    m_timer->SetTimeouts( NETWORK_UNCONFIGURED_MILLISECOND_TIMEOUT_START,
                          NETWORK_UNCONFIGURED_MILLISECOND_TIMEOUT_RETRY );

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
    BOSE_VERBOSE( s_logger, "CustomProductControllerStateNetworkStandbyUnconfigured is being started." );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStateNetworkStandbyUnconfigured::HandleStateExit
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductControllerStateNetworkStandbyUnconfigured::HandleStateExit()
{
    BOSE_VERBOSE( s_logger, "CustomProductControllerStateNetworkStandbyUnconfigured is being exited." );
    BOSE_VERBOSE( s_logger, "The timer will be stopped." );

    m_timer->Stop( );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStateNetworkStandbyUnconfigured::HandleTimeOut
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductControllerStateNetworkStandbyUnconfigured::HandleTimeOut( void )
{
    BOSE_VERBOSE( s_logger, "A time out in the network standby unconfigured state has occurred." );
    BOSE_VERBOSE( s_logger, "An attempt to set the device to a low power state will be made." );

    m_timer->Stop( );

    ChangeState( PROFESSOR_PRODUCT_CONTROLLER_STATE_LOW_POWER );
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
    BOSE_VERBOSE( s_logger, "CustomProductControllerStateNetworkStandbyUnconfigured is handling a network state change." );

    if( configured and not connected )
    {
        BOSE_VERBOSE( s_logger, "%s is changing to %s.",
                      "CustomProductControllerStateNetworkStandbyUnconfigured",
                      "CustomProductControllerStateNetworkStandbyConfigured" );
        ChangeState( PROFESSOR_PRODUCT_CONTROLLER_STATE_NETWORK_STANDBY_CONFIGURED );
    }
    else
    {
        auto const& networkConnected = connected;
        auto const& voiceConfigured = m_productController.IsVoiceConfigured( );

        if( networkConnected and voiceConfigured )
        {
            BOSE_VERBOSE( s_logger, "%s is changing to %s.",
                          "CustomProductControllerStateNetworkStandbyUnconfigured",
                          "CustomProductControllerStateIdle" );
            ChangeState( PROFESSOR_PRODUCT_CONTROLLER_STATE_IDLE );
        }
        else
        {
            BOSE_VERBOSE( s_logger, "CustomProductControllerStateNetworkStandbyUnconfigured is not changing." );
        }
    }

    return true;
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
    BOSE_VERBOSE( s_logger, "CustomProductControllerStateNetworkStandbyUnconfigured is handling a voice state change." );

    auto const& voiceConfigured = configured;
    auto const& networkConnected = m_productController.IsNetworkConfigured( );

    if( voiceConfigured and networkConnected )
    {
        BOSE_VERBOSE( s_logger, "%s is changing to %s.",
                      "CustomProductControllerStateNetworkStandbyUnconfigured",
                      "CustomProductControllerStateIdle" );
        ChangeState( PROFESSOR_PRODUCT_CONTROLLER_STATE_IDLE );
    }
    else
    {
        BOSE_VERBOSE( s_logger, "CustomProductControllerStateNetworkStandbyUnconfigured is not changing state." );
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
