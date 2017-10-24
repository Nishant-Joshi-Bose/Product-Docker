////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file      CustomProductControllerStateNetworkStandby.cpp
///
/// @brief     This source code file contains functionality to process events that occur during the
///            product network standby state.
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
#include "ProductControllerHsm.h"
#include "ProductHardwareInterface.h"
#include "ProfessorProductController.h"
#include "CustomProductControllerStateNetworkStandby.h"

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
/// @brief CustomProductControllerStateNetworkStandby::CustomProductControllerStateNetworkStandby
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
CustomProductControllerStateNetworkStandby::CustomProductControllerStateNetworkStandby

( ProductControllerHsm&       hsm,
  CHsmState*                  pSuperState,
  ProfessorProductController& productController,
  Hsm::STATE                  stateId,
  const std::string&          name )

    : ProductControllerStateNetworkStandby( hsm, pSuperState, productController, stateId, name ),
      m_productController( productController )
{
    BOSE_DEBUG( s_logger, "The product network standby state is being constructed." );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStateNetworkStandby::HandleStateEnter
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductControllerStateNetworkStandby::HandleStateEnter()
{
    BOSE_DEBUG( s_logger, "The product network standby state is being entered." );
    BOSE_DEBUG( s_logger, "An attempt to set a standby power state is being made." );

    ProductHardwareInterface* HardwareInterface = m_productController.GetHardwareInterface( );

    if( HardwareInterface != nullptr )
    {
        HardwareInterface->RequestPowerStateStandby( );
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStateNetworkStandby::HandleStateStart
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductControllerStateNetworkStandby::HandleStateStart()
{
    BOSE_DEBUG( s_logger, "The product network standby state is being started." );

    bool networkConfigured = m_productController.IsNetworkConfigured( );

    if( networkConfigured )
    {
        BOSE_DEBUG( s_logger, "The product network standby state is changing to a configured state." );
        ChangeState( PROFESSOR_PRODUCT_CONTROLLER_STATE_NETWORK_STANDBY_CONFIGURED );
    }
    else
    {
        BOSE_DEBUG( s_logger, "The product network standby state is changing to an unconfigured state." );
        ChangeState( PROFESSOR_PRODUCT_CONTROLLER_STATE_NETWORK_STANDBY_UNCONFIGURED );
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStateNetworkStandby::HandleStateExit
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductControllerStateNetworkStandby::HandleStateExit()
{
    BOSE_DEBUG( s_logger, "The product network standby state is being exited." );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief  CustomProductControllerStateNetworkStandby::HandleAutowakeStatus
///
/// @param  bool active
///
/// @return This method returns a true Boolean value indicating that it has handled the voice
///         state change and no futher processing will be required by any of its superstates.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductControllerStateNetworkStandby::HandleAutowakeStatus( bool active )
{
    BOSE_DEBUG( s_logger, "The product network standby state is handling a change in autowake status." );

    if( active )
    {
        BOSE_DEBUG( s_logger, "The product network standby state is changing to an idle state." );
        ChangeState( PROFESSOR_PRODUCT_CONTROLLER_STATE_IDLE );
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
