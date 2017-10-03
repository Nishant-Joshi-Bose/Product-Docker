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
#include "ProfessorProductController.h"
#include "ProductControllerStateIdle.h"

static DPrint s_logger( "ProductControllerStateIdle" );

////////////////////////////////////////////////////////////////////////////////////////////////////
///                             Start of Product Namespace                                       ///
////////////////////////////////////////////////////////////////////////////////////////////////////
namespace ProductApp
{

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
                                                                    STATE                       stateId,
                                                                    const std::string&          name )

    : ProductControllerStateIdle( hsm, pSuperState, productController, stateId, name )
{
    BOSE_DEBUG( s_logger, "The Product Idle State is being constructed." );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief CustomProductControllerStateIdle::HandleStateEnter
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductControllerStateIdle::HandleStateEnter( )
{
    BOSE_DEBUG( s_logger, "The Product Idle State is being entered." );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief CustomProductControllerStateIdle::HandleStateStart
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductControllerStateIdle::HandleStateStart( )
{
    BOSE_DEBUG( s_logger, "The Product Idle State is being started." );

    if( !static_cast< ProfessorProductController& >( GetProductController( ) ).GetNetworkStatus( ) )
    {
        BOSE_DEBUG( s_logger, "The Product Idle State is changing to the Network Standby state." );
        ChangeState( PROFESSOR_PRODUCT_CONTROLLER_STATE_NETWORK_STANDBY );
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief CustomProductControllerStateIdle::HandleStateExit
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductControllerStateIdle::HandleStateExit( )
{
    BOSE_DEBUG( s_logger, "The Product Idle State is being exited." );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief CustomProductControllerStateIdle::HandleLpmState
/// @param active
/// @return
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductControllerStateIdle::HandleLpmState( bool active )
{
     BOSE_DEBUG( s_logger, "The Product Idle State is handling an LPM state change." );

     if( active )
     {
         if( static_cast< ProfessorProductController& >( GetProductController( ) ).IsBooted( ) )
         {
             if( static_cast< ProfessorProductController& >( GetProductController( ) ).GetNetworkStatus( ) )
             {
                 BOSE_DEBUG( s_logger, "The Product Idle State is to remain in the Idle state." );
             }
             else
             {
                 BOSE_DEBUG( s_logger, "The Product Idle State is changing to the Network Standby state." );
                 ChangeState( PROFESSOR_PRODUCT_CONTROLLER_STATE_NETWORK_STANDBY );
             }
         }
         else
         {
             BOSE_DEBUG( s_logger, "The Product Idle State is changing to a booting state." );
             ChangeState( PROFESSOR_PRODUCT_CONTROLLER_STATE_BOOTING );
         }
     }
     else
     {
         BOSE_DEBUG( s_logger, "The Product Idle State is changing to a booting state." );
         ChangeState( PROFESSOR_PRODUCT_CONTROLLER_STATE_BOOTING );
     }

     return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief CustomProductControllerStateIdle::HandleCapsState
/// @param active
/// @return
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductControllerStateIdle::HandleCapsState( bool active )
{
     BOSE_DEBUG( s_logger, "The Product Idle State is handling a CAPS state change." );

     if( active )
     {
         if( static_cast< ProfessorProductController& >( GetProductController( ) ).IsBooted( ) )
         {
             if( static_cast< ProfessorProductController& >( GetProductController( ) ).GetNetworkStatus( ) )
             {
                 BOSE_DEBUG( s_logger, "The Product Idle State is to remain in the Idle state." );
             }
             else
             {
                 BOSE_DEBUG( s_logger, "The Product Idle State is changing to the Network Standby state." );
                 ChangeState( PROFESSOR_PRODUCT_CONTROLLER_STATE_NETWORK_STANDBY );
             }
         }
         else
         {
             BOSE_DEBUG( s_logger, "The Product Idle State is changing to a booting state." );
             ChangeState( PROFESSOR_PRODUCT_CONTROLLER_STATE_BOOTING );
         }
     }
     else
     {
         BOSE_DEBUG( s_logger, "The Product Idle State is changing to a booting state." );
         ChangeState( PROFESSOR_PRODUCT_CONTROLLER_STATE_BOOTING );
     }

     return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief CustomProductControllerStateIdle::HandleNetworkState
/// @param active
/// @return
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductControllerStateIdle::HandleNetworkState( bool active )
{
     BOSE_DEBUG( s_logger, "The Product Idle State is handling a network state change." );

     if( active )
     {
         if( static_cast< ProfessorProductController& >( GetProductController( ) ).IsBooted( ) )
         {
             BOSE_DEBUG( s_logger, "The Product Idle State is reamining in the Idle state." );
         }
         else
         {
             BOSE_DEBUG( s_logger, "The Product Idle State is changing to a booting state." );
             ChangeState( PROFESSOR_PRODUCT_CONTROLLER_STATE_BOOTING );
         }
     }
     else
     {
         if( static_cast< ProfessorProductController& >( GetProductController( ) ).IsBooted( ) )
         {
             BOSE_DEBUG( s_logger, "The Product Idle State is reamining in the Idle state." );
             ChangeState( PROFESSOR_PRODUCT_CONTROLLER_STATE_NETWORK_STANDBY );
         }
         else
         {
             BOSE_DEBUG( s_logger, "The Product Idle State is changing to a booting state." );
             ChangeState( PROFESSOR_PRODUCT_CONTROLLER_STATE_BOOTING );
         }
     }

     return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                     End of Namespace                                         ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                        End of File                                           ///
////////////////////////////////////////////////////////////////////////////////////////////////////
