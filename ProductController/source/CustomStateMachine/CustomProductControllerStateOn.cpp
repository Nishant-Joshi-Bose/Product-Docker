////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file      CustomProductControllerStateOn.cpp
///
/// @brief     This source code file contains functionality to process events that occur during the
///            product on state.
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
#include "CustomProductControllerStateOn.h"
#include "ProductControllerHsm.h"
#include "ProfessorProductController.h"
#include "ProductControllerState.h"
#include "KeyActions.pb.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
///                            Start of Product Application Namespace                            ///
////////////////////////////////////////////////////////////////////////////////////////////////////
namespace ProductApp
{

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStateOn::CustomProductControllerStateOn
///
/// @param ProductControllerHsm& hsm
///
/// @param CHsmState*            pSuperState
///
/// @param Hsm::STATE            stateId
///
/// @param const std::string&    name
///
////////////////////////////////////////////////////////////////////////////////////////////////////
CustomProductControllerStateOn::CustomProductControllerStateOn( ProductControllerHsm&       hsm,
                                                                CHsmState*                  pSuperState,
                                                                Hsm::STATE                  stateId,
                                                                const std::string&          name )

    : ProductControllerState( hsm, pSuperState, stateId, name )
{
    BOSE_VERBOSE( s_logger, "CustomProductControllerStateOn is being constructed." );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStateOn::HandleStateEnter
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductControllerStateOn::HandleStateEnter( )
{
    BOSE_VERBOSE( s_logger, "CustomProductControllerStateOn is being entered." );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStateOn::HandleStateStart
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductControllerStateOn::HandleStateStart( )
{
    BOSE_VERBOSE( s_logger, "CustomProductControllerStateOn is being started." );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStateOn::HandleStateExit
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductControllerStateOn::HandleStateExit( )
{
    BOSE_VERBOSE( s_logger, "CustomProductControllerStateOn is being exited." );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief  CustomProductControllerStateOn::HandleKeyAction
///
/// @param  int action
///
/// @return This method returns a true Boolean value indicating that it has handled the key action
///         or false if the key has not been handled.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductControllerStateOn::HandleKeyAction( int action )
{
    bool handled = false;

    BOSE_INFO( s_logger, "CustomProductControllerStateOn is handling key action %d.", action );

    switch( action )
    {
    case KeyActionPb::KEY_ACTION_VOLUME_UP_1:
        GetCustomProductController( ).GetVolumeManager()->Increment( 1 );
        handled = true;
        break;

    case KeyActionPb::KEY_ACTION_VOLUME_DOWN_1:
        GetCustomProductController( ).GetVolumeManager()->Decrement( 1 );
        handled = true;
        break;

    case KeyActionPb::KEY_ACTION_MUTE:
        GetCustomProductController( ).GetVolumeManager()->ToggleMute();
        handled = true;
        break;

    case KeyActionPb::KEY_ACTION_PAIR_SPEAKERS:
        ChangeState( PROFESSOR_PRODUCT_CONTROLLER_STATE_ACCESSORY_PAIRING );
        handled = true;
        break;

    case KeyActionPb::KEY_ACTION_SOUNDTOUCH:
        GetCustomProductController( ).SendPlaybackRequest( SOURCE_SOUNDTOUCH );
        handled = true;
        break;

    case KeyActionPb::KEY_ACTION_TV:
        GetCustomProductController( ).SendPlaybackRequest( SOURCE_TV );
        handled = true;
        break;

    default:
        break;
    }

    return handled;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                           End of the Product Application Namespace                           ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
