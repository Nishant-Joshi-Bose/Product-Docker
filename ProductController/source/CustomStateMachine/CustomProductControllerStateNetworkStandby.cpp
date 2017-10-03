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
#include "CustomProductControllerStateNetworkStandby.h"
#include "ProductControllerHsm.h"
#include "ProfessorProductController.h"
#include "DPrint.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// The following declares a DPrint class type object and a standard string for logging information
/// in this source code file.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
static DPrint s_logger( "CustomProductControllerStateNetworkStandby" );

////////////////////////////////////////////////////////////////////////////////////////////////////
///                             Start of Product Namespace                                       ///
////////////////////////////////////////////////////////////////////////////////////////////////////
namespace ProductApp
{

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
  STATE                       stateId,
  const std::string&          name )

    : ProductControllerStateNetworkStandby( hsm, pSuperState, productController, stateId, name )
{
    BOSE_DEBUG( s_logger, "The Product Network Standby State is being constructed." );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief CustomProductControllerStateNetworkStandby::HandleStateEnter
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductControllerStateNetworkStandby::HandleStateEnter()
{
    BOSE_DEBUG( s_logger, "The Product Network Standby State is being entered." );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief CustomProductControllerStateNetworkStandby::HandleStateStart
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductControllerStateNetworkStandby::HandleStateStart()
{
    BOSE_DEBUG( s_logger, "The Product Network Standby State is being started." );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief CustomProductControllerStateNetworkStandby::HandleStateExit
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductControllerStateNetworkStandby::HandleStateExit()
{
    BOSE_DEBUG( s_logger, "The Product Network Standby State is being exited." );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief CustomProductControllerStateNetworkStandby::HandleLpmState
/// @param active
/// @return
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductControllerStateNetworkStandby::HandleLpmState( bool active )
{
    BOSE_DEBUG( s_logger, "The Product Network Standby State is handling an LPM state change." );

    if( active )
    {
        if( static_cast< ProfessorProductController& >( GetProductController( ) ).IsBooted( ) )
        {
            if( static_cast< ProfessorProductController& >( GetProductController( ) ).GetNetworkStatus( ) )
            {
                BOSE_DEBUG( s_logger, "The Product Network Standby State is changing to the Idle state." );
                ChangeState( PROFESSOR_PRODUCT_CONTROLLER_STATE_IDLE );
            }
            else
            {
                BOSE_DEBUG( s_logger, "The Product Network Standby State is to remain in this state." );
            }
        }
        else
        {
            BOSE_DEBUG( s_logger, "The Product Network Standby State is changing to a booting state." );
            ChangeState( PROFESSOR_PRODUCT_CONTROLLER_STATE_BOOTING );
        }
    }
    else
    {
        BOSE_DEBUG( s_logger, "The Product Network Standby State is changing to a booting state." );
        ChangeState( PROFESSOR_PRODUCT_CONTROLLER_STATE_BOOTING );
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief CustomProductControllerStateNetworkStandby::HandleCapsState
/// @param active
/// @return
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductControllerStateNetworkStandby::HandleCapsState( bool active )
{
    BOSE_DEBUG( s_logger, "The Product Network Standby State is handling a CAPS state change." );

    if( active )
    {
        if( static_cast< ProfessorProductController& >( GetProductController( ) ).IsBooted( ) )
        {
            if( static_cast< ProfessorProductController& >( GetProductController( ) ).GetNetworkStatus( ) )
            {
                BOSE_DEBUG( s_logger, "The Product Network Standby State is changing to the Idle state." );
                ChangeState( PROFESSOR_PRODUCT_CONTROLLER_STATE_IDLE );
            }
            else
            {
                BOSE_DEBUG( s_logger, "The Product Network Standby State is to remain in this state." );
            }
        }
        else
        {
            BOSE_DEBUG( s_logger, "The Product Network Standby State is changing to a booting state." );
            ChangeState( PROFESSOR_PRODUCT_CONTROLLER_STATE_BOOTING );
        }
    }
    else
    {
        BOSE_DEBUG( s_logger, "The Product Network Standby State is changing to a booting state." );
        ChangeState( PROFESSOR_PRODUCT_CONTROLLER_STATE_BOOTING );
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief CustomProductControllerStateNetworkStandby::HandleNetworkState
/// @param active
/// @return
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductControllerStateNetworkStandby::HandleNetworkState( bool active )
{
    BOSE_DEBUG( s_logger, "The Product Network Standby State is handling a network state change." );

    if( active )
    {
        if( static_cast< ProfessorProductController& >( GetProductController( ) ).IsBooted( ) )
        {
            BOSE_DEBUG( s_logger, "The Product Network Standby State is reamining in the Idle state." );
            ChangeState( PROFESSOR_PRODUCT_CONTROLLER_STATE_IDLE );
        }
        else
        {
            BOSE_DEBUG( s_logger, "The Product Network Standby State is changing to a booting state." );
            ChangeState( PROFESSOR_PRODUCT_CONTROLLER_STATE_BOOTING );
        }
    }
    else
    {
        if( static_cast< ProfessorProductController& >( GetProductController( ) ).IsBooted( ) )
        {
            BOSE_DEBUG( s_logger, "The Product Network Standby State is reamining in this state." );
        }
        else
        {
            BOSE_DEBUG( s_logger, "The Product Network Standby State is changing to a booting state." );
            ChangeState( PROFESSOR_PRODUCT_CONTROLLER_STATE_BOOTING );
        }
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                               End of ProductApp Namespace                                    ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                        End of File                                           ///
////////////////////////////////////////////////////////////////////////////////////////////////////
