////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file      CustomProductControllerState.cpp
///
/// @brief     This file contains source code that implements a CustomProductControllerState class
///            for executing produce specific actions based on the currect product state and product
///            specific events.
///
/// @author    Stuart J. Lumby
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
#include "Utilities.h"
#include "CustomProductControllerState.h"
#include "ProductControllerHsm.h"
#include "ProfessorProductController.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
///                            Start of Product Application Namespace                            ///
////////////////////////////////////////////////////////////////////////////////////////////////////
namespace ProductApp
{

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief  This static pointer references the custom product controller instance and is returned
///         through the GetCustomProductController method to access required product controller
///         functionality by the state machine classes, which inherit this class.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
ProductController* CustomProductControllerState::s_productController = nullptr;

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name  CustomProductControllerState::CustomProductControllerState
///
/// @param ProductControllerHsm& productStateMachine
///
/// @param CHsmState*            pSuperState
///
/// @param Hsm::STATE            stateId
///
/// @param const std::string&    name
///
////////////////////////////////////////////////////////////////////////////////////////////////////
CustomProductControllerState::CustomProductControllerState( ProductControllerHsm& productStateMachine,
                                                            CHsmState*            pSuperState,
                                                            Hsm::STATE            stateId,
                                                            const std::string&    name )

    : CHsmState( stateId, &productStateMachine, name, pSuperState )
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   CustomProductControllerState::GetCustomProductController
///
/// @return This method returns the Professor Custom Product Controller Reference of type
///         ProfessorProductController.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
ProfessorProductController& CustomProductControllerState::GetCustomProductController( )
{
    return *static_cast< ProfessorProductController* >( s_productController );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                           End of the Product Application Namespace                           ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
