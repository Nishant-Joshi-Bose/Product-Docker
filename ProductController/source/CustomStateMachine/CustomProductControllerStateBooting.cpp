////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file      CustomProductControllerStateBooting.h
///
/// @brief     This source code file contains functionality to process events that occur during the
///            product booting state.
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
#include "CustomProductControllerStateBooting.h"
#include "ProductControllerHsm.h"
#include "ProfessorProductController.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
///                            Start of Product Application Namespace                            ///
////////////////////////////////////////////////////////////////////////////////////////////////////
namespace ProductApp
{

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStateBooting::CustomProductControllerStateBooting
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
CustomProductControllerStateBooting::CustomProductControllerStateBooting( ProductControllerHsm& hsm,
                                                                          CHsmState*            pSuperState,
                                                                          Hsm::STATE            stateId,
                                                                          const std::string&    name )

    : ProductControllerStateBooting( hsm, pSuperState, stateId, name )
{
    BOSE_VERBOSE( s_logger, "The %s state has been constructed.", GetName( ).c_str( ) );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStateBooting::PossiblyGoToAppropriatePlayableState
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductControllerStateBooting::PossiblyGoToAppropriatePlayableState( )
{
    if( GetCustomProductController( ).IsBooted( ) )
    {
        if( GetCustomProductController( ).IsFirstTimeBootUp( ) )
        {
            BOSE_VERBOSE( s_logger, "The %s state is changing to a Welcome state.", GetName( ).c_str( ) );

            ChangeState( PRODUCT_CONTROLLER_STATE_WELCOME );
        }
        else if( GetCustomProductController( ).IsSoftwareUpdateRequired( ) )
        {
            BOSE_VERBOSE( s_logger, "The %s state is changing to an UpdatingSoftware state.", GetName( ).c_str( ) );

            ChangeState( PRODUCT_CONTROLLER_STATE_SOFTWARE_UPDATE_TRANSITION );
        }
        else if( not GetCustomProductController( ).IsOutOfBoxSetupComplete( ) )
        {
            BOSE_VERBOSE( s_logger, "The %s state is changing to a Setup state.", GetName( ).c_str( ) );

            ChangeState( PRODUCT_CONTROLLER_STATE_SETUP );
        }
        else
        {
            ChangeState( PRODUCT_CONTROLLER_STATE_PLAYABLE_TRANSITION );
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                           End of the Product Application Namespace                           ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
