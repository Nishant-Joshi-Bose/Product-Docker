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
#include "CustomProductControllerState.h"
#include "ProductControllerHsm.h"

namespace ProductApp
{
CustomProductControllerState::CustomProductControllerState( ProductControllerHsm& productStateMachine,
        CHsmState*            pSuperState,
        Hsm::STATE            stateId,
        const std::string&    name )

    : CHsmState( stateId, &productStateMachine, name, pSuperState )
{
    /// return;
}

bool CustomProductControllerState::HandleLpmState( bool active )
{
    return true;
}

bool CustomProductControllerState::HandleCapsState( bool active )
{
    return true;
}

bool CustomProductControllerState::HandleNetworkState( bool active )
{
    return true;
}

bool CustomProductControllerState::HandleSTSSourcesInit ( void )
{
    return true;
}
}
////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
