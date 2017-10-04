////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file      CustomProductControllerStateUpdatingSoftware.h
///
/// @brief     This source code file contains functionality to process events that occur during a
///            software updating state.
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
#include "CustomProductControllerStateUpdating.h"
#include "ProductControllerHsm.h"
#include "ProfessorProductController.h"
#include "DPrint.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// The following declares a DPrint class type object and a standard string for logging information
/// in this source code file.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
static DPrint s_logger( "Product" );

////////////////////////////////////////////////////////////////////////////////////////////////////
///                             Start of Product Namespace                                       ///
////////////////////////////////////////////////////////////////////////////////////////////////////
namespace ProductApp
{

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStateUpdatingSoftware::CustomProductControllerStateUpdatingSoftware
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
CustomProductControllerStateUpdatingSoftware::CustomProductControllerStateUpdatingSoftware
( ProductControllerHsm&       hsm,
  CHsmState*                  pSuperState,
  ProfessorProductController& productController,
  Hsm::STATE                  stateId,
  const std::string&          name )

    : ProductControllerState( hsm, pSuperState, productController, stateId, name )
{
    BOSE_DEBUG( s_logger, "The Product Software Update State is being constructed." );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStateUpdatingSoftware::HandleStateEnter
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductControllerStateUpdatingSoftware::HandleStateEnter()
{
    BOSE_DEBUG( s_logger, "The Product Software Update State is being constructed." );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStateUpdatingSoftware::HandleStateStart
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductControllerStateUpdatingSoftware::HandleStateStart()
{
    BOSE_DEBUG( s_logger, "The Product Software Update State is being constructed." );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStateUpdatingSoftware::HandleStateExit
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductControllerStateUpdatingSoftware::HandleStateExit()
{
    BOSE_DEBUG( s_logger, "The Product Software Update State is being constructed." );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief HandleLpmState
///
/// @param active
///
/// @return
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductControllerStateUpdatingSoftware::HandleLpmState( bool active )
{
    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief HandleCapsState
/// @param active
/// @return
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductControllerStateUpdatingSoftware::HandleCapsState( bool active )
{
    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief HandleNetworkState
/// @param active
/// @return
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductControllerStateUpdatingSoftware::HandleNetworkState( bool active )
{
    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                     End of Namespace                                         ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
