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

////////////////////////////////////////////////////////////////////////////////////////////////////
///                            Start of Product Application Namespace                            ///
////////////////////////////////////////////////////////////////////////////////////////////////////
namespace ProductApp
{

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerState::CustomProductControllerState
///
/// @param ProductControllerHsm& hsm
///
/// @param CHsmState* pSuperState
///
/// @param ProfessorProductController& productController
///
/// @param Hsm::STATE stateId
///
/// @param const std::string& name
///
////////////////////////////////////////////////////////////////////////////////////////////////////
CustomProductControllerState::CustomProductControllerState( ProductControllerHsm& productStateMachine,
                                                            CHsmState*            pSuperState,
                                                            Hsm::STATE            stateId,
                                                            const std::string&    name )

    : CHsmState( stateId, &productStateMachine, name, pSuperState )
{
    return;
}

bool CustomProductControllerState::HandleLpmState( bool active )
{
    return false;
}

bool CustomProductControllerState::HandleCapsState( bool active )
{
    return false;
}

bool CustomProductControllerState::HandleAudioPathState( bool active )
{
    return false;
}

bool CustomProductControllerState::HandleSTSSourcesInit( void )
{
    return false;
}

bool CustomProductControllerState::HandleNetworkState( bool configured, bool connected )
{
    return false;
}

bool CustomProductControllerState::HandleVoiceState( bool configured )
{
    return false;
}

bool CustomProductControllerState::HandlePlaybackRequest( ProductPlaybackRequest_ProductPlaybackState state )
{
    return false;
}

bool CustomProductControllerState::HandlePowerState( void )
{
    return false;
}

bool CustomProductControllerState::HandleAutowakeStatus( bool active )
{
    return false;
}

bool CustomProductControllerState::HandleKeyAction( int action )
{
    return false;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
///                             End of Product Application Namespace                             ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
