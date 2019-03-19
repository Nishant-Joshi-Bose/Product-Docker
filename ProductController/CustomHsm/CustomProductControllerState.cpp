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
#include "CustomProductController.h"

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
KeyHandlerUtil::ActionType_t CustomProductControllerState::s_ActionPendingFromTransition = ( KeyHandlerUtil::ActionType_t )Action::INVALID;

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
/// @return This method returns the Custom Product Controller Reference of type
///         CustomProductController.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
CustomProductController& CustomProductControllerState::GetCustomProductController( )
{
    return *static_cast< CustomProductController* >( s_productController );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   CustomProductControllerState::FrontDoorErrorCallback
///
/// @param const FrontDoor::Error & error
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductControllerState::FrontDoorErrorCallback( const FrontDoor::Error & error )
{
    BOSE_ERROR( s_logger, "An error code %d subcode %d and error string <%s> was returned from a frontdoor request.",
                error.code(),
                error.subcode(),
                error.message().c_str() );
};

////////////////////////////////////////////////////////////////////////////////////////////////////
///                           End of the Product Application Namespace                           ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
