////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file      CustomProductControllerStateIdle.cpp
///
/// @brief     This source code file contains functionality to process events that occur during the
///            product idle state.
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
#include "CustomProductControllerStateIdle.h"
#include "ProductControllerHsm.h"
#include "ProductControllerStateIdle.h"
#include "ProductHardwareInterface.h"
#include "ProfessorProductController.h"

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
/// @brief CustomProductControllerStateIdle::CustomProductControllerStateIdle
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
CustomProductControllerStateIdle::CustomProductControllerStateIdle( ProductControllerHsm&       hsm,
                                                                    CHsmState*                  pSuperState,
                                                                    ProfessorProductController& productController,
                                                                    Hsm::STATE                  stateId,
                                                                    const std::string&          name )

    : ProductControllerStateIdle( hsm, pSuperState, productController, stateId, name ),
      m_productController( productController )
{
    BOSE_DEBUG( s_logger, "The product idle state is being constructed." );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStateIdle::HandleStateEnter
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductControllerStateIdle::HandleStateEnter( )
{
    BOSE_DEBUG( s_logger, "The product idle state is being entered by the state machine." );
    BOSE_DEBUG( s_logger, "An attempt to set an autowake power state is now being made." );

    ProductHardwareInterface* HardwareInterface = m_productController.GetHardwareInterface( );

    if( HardwareInterface != nullptr )
    {
        HardwareInterface->RequestPowerStateAutowake( );
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStateIdle::HandleStateStart
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductControllerStateIdle::HandleStateStart( )
{
    BOSE_DEBUG( s_logger, "The product idle state is being started." );

    bool networkConnected;
    bool voiceConfigured;

    networkConnected = m_productController.IsNetworkConfigured( );
    voiceConfigured = m_productController.IsVoiceConfigured( );

    if( networkConnected and voiceConfigured )
    {
        BOSE_DEBUG( s_logger, "The product idle state is changing to a voice configured state." );
        ChangeState( PROFESSOR_PRODUCT_CONTROLLER_STATE_IDLE_VOICE_CONFIGURED );
    }
    else
    {
        BOSE_DEBUG( s_logger, "The product idle state is changing to a voice unconfigured state." );
        ChangeState( PROFESSOR_PRODUCT_CONTROLLER_STATE_IDLE_VOICE_UNCONFIGURED );
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStateIdle::HandleStateExit
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductControllerStateIdle::HandleStateExit( )
{
    BOSE_DEBUG( s_logger, "The product idle state is being exited." );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                             End of Product Application Namespace                             ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                        End of File                                           ///
////////////////////////////////////////////////////////////////////////////////////////////////////
