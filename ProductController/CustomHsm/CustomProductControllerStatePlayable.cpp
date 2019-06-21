////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file      CustomProductControllerStatePlayable.cpp
///
/// @brief     This source code file contains functionality to process events that occur during the
///            product playing state.
///
/// @attention Copyright (C) 2018 Bose Corporation All Rights Reserved
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
#include "CustomProductControllerStatePlayable.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
///                            Start of Product Application Namespace                            ///
////////////////////////////////////////////////////////////////////////////////////////////////////
namespace ProductApp
{

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStatePlayable::CustomProductControllerStatePlayable
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
CustomProductControllerStatePlayable::CustomProductControllerStatePlayable(
    ProductControllerHsm&       hsm,
    CHsmState*                  pSuperState,
    Hsm::STATE                  stateId,
    const std::string&          name )

    : ProductControllerStatePlayable( hsm, pSuperState, stateId, name )
{
    BOSE_INFO( s_logger, "The %s state is being constructed.", name.c_str() );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief  CustomProductControllerStatePlayable::HandleIntentVoiceListening
///
/// @return This method returns a true Boolean value indicating that it has handled the event
///         and no futher processing will be required by any of its superstates.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductControllerStatePlayable::HandleIntentVoiceListening( )
{
    BOSE_INFO( s_logger, "The %s state is in %s.", GetName( ).c_str( ), __func__ );

    ///
    /// Go to a playing transition state to bring the product to full power.
    ///
    ChangeState( PRODUCT_CONTROLLER_STATE_PLAYING_TRANSITION_SWITCH );
    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                           End of the Product Application Namespace                           ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
