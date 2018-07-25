////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file      CustomProductControllerStateBooting.cpp
///
/// @brief     This source code file contains functionality to process events that occur during the
///            product booting state.
///
/// @attention Copyright (C) 2018 Bose Corporation All Rights Reserved
////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////
///            Included Header Files
////////////////////////////////////////////////////////////////////////////////////////////////////
#include "CustomProductControllerStateBooting.h"
#include "ProductControllerStates.h"
#include "ProductController.h"
#include "Utilities.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
///                            Start of Product Application Namespace                            ///
////////////////////////////////////////////////////////////////////////////////////////////////////
namespace ProductApp
{

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @name  CustomProductControllerStateBooting::CustomProductControllerStateBooting
/// @param ProductControllerHsm& hsm
/// @param CHsmState*            pSuperState
/// @param Hsm::STATE            stateId
/// @param const std::string&    name
////////////////////////////////////////////////////////////////////////////////////////////////////
CustomProductControllerStateBooting::CustomProductControllerStateBooting( ProductControllerHsm& hsm,
                                                                          CHsmState* pSuperState,
                                                                          Hsm::STATE stateId,
                                                                          const std::string& name ) :
    ProductControllerStateBooting( hsm, pSuperState, stateId, name )
{
    BOSE_INFO( s_logger, "The %s state is being constructed.", GetName( ).c_str( ) );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @name  CustomProductControllerStateBooting::PossiblyGoToNextState
/// @brief The functionality contained here is customized for Professor/GingerCheevers. We want the LPM
///        to go to SYSTEM_STATE_ON soonest, so that the wired accessory discovery can start and complete
///        without intruding into the First Boot Greeting chime. See PGC-1926.
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductControllerStateBooting::PossiblyGoToNextState( )
{
    BOSE_INFO( s_logger, "The %s state is in %s. IsPartialSoftwareUpdatePending = %s",
               GetName( ).c_str( ), __func__,
               GetProductController().IsPartialSoftwareUpdatePending() ? "true" : "false" );

    if( !GetProductController().IsPartialSoftwareUpdatePending() && !GetProductController( ).IsFirstBootGreetingDone() )
    {
        // The next state will be PRODUCT_CONTROLLER_STATE_FIRST_BOOT_GREETING_TRANSITION
        if( GetProductController().IsLpmReady( ) && GetProductController().IsAudioPathReady( ) )
        {
            // LPM is ready to detect wired accessory
            static bool doOnce = true;
            if( doOnce )
            {
                // kick the LPM to start detecting wired bassbox
                doOnce = false;
                GetProductController( ).GetLpmHardwareInterface( )->SetSystemState( SYSTEM_STATE_ON );
            }
        }
    }

    ProductControllerStateBooting::PossiblyGoToNextState( );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                           End of the Product Application Namespace                           ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
