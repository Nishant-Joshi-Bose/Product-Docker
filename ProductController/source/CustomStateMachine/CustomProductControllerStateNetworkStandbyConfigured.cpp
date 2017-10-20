////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file      CustomProductControllerStateNetworkStandbyConfigured.cpp
///
/// @brief     This source code file contains functionality to process events that occur during the
///            product network standby configured state.
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
#include "CustomProductControllerStateNetworkStandbyConfigured.h"
#include "ProductControllerHsm.h"
#include "ProfessorProductController.h"
#include "DPrint.h"

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
  ProfessorProductController& productController,
  Hsm::STATE                  stateId,
  const std::string&          name )

    : ProductControllerState( hsm, pSuperState, productController, stateId, name ),
      m_productController( productController )
{
    BOSE_DEBUG( s_logger, "The product network standby configured state is being constructed." );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStateNetworkStandbyConfigured::HandleStateEnter
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductControllerStateNetworkStandbyConfigured::HandleStateEnter()
{
    BOSE_DEBUG( s_logger, "The product network standby configured state is being entered." );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStateNetworkStandbyConfigured::HandleStateStart
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductControllerStateNetworkStandbyConfigured::HandleStateStart()
{
    BOSE_DEBUG( s_logger, "The product network standby configured state is being started." );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStateNetworkStandbyConfigured::HandleStateExit
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductControllerStateNetworkStandbyConfigured::HandleStateExit()
{
    BOSE_DEBUG( s_logger, "The product network standby configured state is being exited." );
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
    BOSE_DEBUG( s_logger, "The product network standby configured state is handling a network state change." );

    if( not configured )
    {
        BOSE_DEBUG( s_logger, "The state is changing to a network standby unconfigured state." );
        ChangeState( PROFESSOR_PRODUCT_CONTROLLER_STATE_NETWORK_STANDBY_UNCONFIGURED );

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
            BOSE_DEBUG( s_logger, "The product network standby configured state is changing to an idle state." );
            ChangeState( PROFESSOR_PRODUCT_CONTROLLER_STATE_IDLE );
        }
        else
        {
            BOSE_DEBUG( s_logger, "The product network standby configured state is not changing state." );
        }

        return true;
    }
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
    BOSE_DEBUG( s_logger, "The product network standby configured state is handling a voice state change." );

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
        BOSE_DEBUG( s_logger, "The product network standby configured state is not changing." );
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
