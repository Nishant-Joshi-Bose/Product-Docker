////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file      CustomProductControllerStateBooting.h
///
/// @brief     This source code file contains functionality to process events that occur during the
///            product booting state.
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
#include "CustomProductControllerStateBooting.h"
#include "ProductControllerHsm.h"
#include "ProfessorProductController.h"
#include "DPrint.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// The following declares a DPrint class type object and a standard string for logging information
/// in this source code file.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
static DPrint s_logger( "CustomProductControllerStateBooting" );

////////////////////////////////////////////////////////////////////////////////////////////////////
///                             Start of Product Namespace                                       ///
////////////////////////////////////////////////////////////////////////////////////////////////////
namespace ProductApp
{

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStateBooting::CustomProductControllerStateBooting
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
CustomProductControllerStateBooting::CustomProductControllerStateBooting( ProductControllerHsm&       hsm,
                                                                          CHsmState*                  pSuperState,
                                                                          ProfessorProductController& productController,
                                                                          STATE                       stateId,
                                                                          const std::string&          name )

    : ProductControllerStateBooting( hsm, pSuperState, productController, stateId, name )
{
    BOSE_DEBUG( s_logger, "The Product Booting State has been constructed." );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStateBooting::HandleStateEnter
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductControllerStateBooting::HandleStateEnter( )
{
     BOSE_DEBUG( s_logger, "The Product Booting State is being entered." );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStateBooting::HandleStateStart
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductControllerStateBooting::HandleStateStart( )
{
     BOSE_DEBUG( s_logger, "The Product Booting State is being started." );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStateBooting::HandleStateExit
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductControllerStateBooting::HandleStateExit( )
{
     BOSE_DEBUG( s_logger, "The Product Booting State is being exited." );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief CustomProductControllerStateBooting::HandleLpmState
/// @param state
/// @return
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductControllerStateBooting::HandleLpmState( bool active )
{
     BOSE_DEBUG( s_logger, "The Product Booting State is handling the LPM state." );

     if( active )
     {
         if( static_cast< ProfessorProductController& >( GetProductController( ) ).IsBooted( ) )
         {
             if( static_cast< ProfessorProductController& >( GetProductController( ) ).GetNetworkStatus( ) )
             {
                 BOSE_DEBUG( s_logger, "The Product Booting State is changing to the Idle state." );
                 ChangeState( PROFESSOR_PRODUCT_CONTROLLER_STATE_IDLE );
             }
             else
             {
                 BOSE_DEBUG( s_logger, "The Product Booting State is changing to the Network Standby state." );
                 ChangeState( PROFESSOR_PRODUCT_CONTROLLER_STATE_NETWORK_STANDBY );
             }
         }
     }

     return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief CustomProductControllerStateBooting::HandleCapsState
/// @param state
/// @return
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductControllerStateBooting::HandleCapsState( bool active )
{
     BOSE_DEBUG( s_logger, "The Product Booting State is handling the CAPS state." );

     if( active )
     {
         if( static_cast< ProfessorProductController& >( GetProductController( ) ).IsBooted( ) )
         {
             if( static_cast< ProfessorProductController& >( GetProductController( ) ).GetNetworkStatus( ) )
             {
                 BOSE_DEBUG( s_logger, "The Product Booting State is changing to the Idle state." );
                 ChangeState( PROFESSOR_PRODUCT_CONTROLLER_STATE_IDLE );
             }
             else
             {
                 BOSE_DEBUG( s_logger, "The Product Booting State is changing to the Network Standby state." );
                 ChangeState( PROFESSOR_PRODUCT_CONTROLLER_STATE_NETWORK_STANDBY );
             }
         }
     }

     return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief CustomProductControllerStateBooting::HandleNetworkState
/// @param state
/// @return
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductControllerStateBooting::HandleNetworkState( bool active )
{
     BOSE_DEBUG( s_logger, "The Product Booting State is handling the network state." );

     if( active )
     {
         if( static_cast< ProfessorProductController& >( GetProductController( ) ).IsBooted( ) )
         {
             if( static_cast< ProfessorProductController& >( GetProductController( ) ).GetNetworkStatus( ) )
             {
                 BOSE_DEBUG( s_logger, "The Product Booting State is changing to the Idle state." );
                 ChangeState( PROFESSOR_PRODUCT_CONTROLLER_STATE_IDLE );
             }
             else
             {
                 BOSE_DEBUG( s_logger, "The Product Booting State is changing to the Network Standby state." );
                 ChangeState( PROFESSOR_PRODUCT_CONTROLLER_STATE_NETWORK_STANDBY );
             }
         }
     }

     return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                       End of Namespace                                       ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                        End of File                                           ///
////////////////////////////////////////////////////////////////////////////////////////////////////
