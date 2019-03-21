////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file      CustomProductControllerStatePlayingTransition.cpp
///
/// @brief     This source code file contains functionality to process events that occur during the
///            product playing transition state.
///
/// @attention Copyright (C) 2019 Bose Corporation All Rights Reserved
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
#include "CustomProductControllerStatePlayingTransition.h"
#include "ProductControllerHsm.h"
#include "ProductControllerState.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
///
///            Constant Definitions
///
////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////
///                            Start of Product Application Namespace                            ///
////////////////////////////////////////////////////////////////////////////////////////////////////
namespace ProductApp
{

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStatePlayingTransition::CustomProductControllerStatePlayingTransition
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
CustomProductControllerStatePlayingTransition::CustomProductControllerStatePlayingTransition( ProductControllerHsm& hsm,
        CHsmState*            pSuperState,
        Hsm::STATE            stateId,
        const std::string&    name )

    : ProductControllerStatePlayableTransition( hsm, pSuperState, stateId, name )
{
    BOSE_INFO( s_logger, "The %s state is being constructed.", GetName( ).c_str( ) );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief  CustomProductControllerStatePlayingTransition::CanPlayVolumeTone
/// @param  canPlay , reference to bool variable
/// @return This method returns false
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductControllerStatePlayingTransition::CanPlayVolumeTone( bool & /* canPlay */ )
{
    BOSE_INFO( s_logger, "The %s state is in %s", GetName( ).c_str( ), __func__ );
    return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                           End of the Product Application Namespace                           ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
