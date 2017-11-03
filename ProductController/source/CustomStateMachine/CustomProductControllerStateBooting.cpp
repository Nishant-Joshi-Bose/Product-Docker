////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file      CustomProductControllerStateBooting.h
///
/// @brief     This source code file contains functionality to process events that occur during the
///            product booting state.
///
/// @author    Stuart J. Lumby
///
/// @date      10/24/2017
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
#include "DPrint.h"

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
/// @param CHsmState* pSuperState
///
/// @param ProfessorProductController& productController
///
/// @param Hsm::STATE stateId
///
/// @param const std::string& name
///
////////////////////////////////////////////////////////////////////////////////////////////////////
CustomProductControllerStateBooting::CustomProductControllerStateBooting( ProductControllerHsm&       hsm,
                                                                          CHsmState*                  pSuperState,
                                                                          Hsm::STATE                  stateId,
                                                                          const std::string&          name )

    : ProductControllerStateBooting( hsm, pSuperState, stateId, name )
{
    BOSE_VERBOSE( s_logger, "CustomProductControllerStateBooting has been constructed." );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStateBooting::HandleStateEnter
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductControllerStateBooting::HandleStateEnter( )
{
    BOSE_VERBOSE( s_logger, "CustomProductControllerStateBooting is being entered." );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStateBooting::HandleStateStart
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductControllerStateBooting::HandleStateStart( )
{
    BOSE_VERBOSE( s_logger, "CustomProductControllerStateBooting is being started." );

    HandlePotentialStateChange( );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStateBooting::HandleStateExit
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductControllerStateBooting::HandleStateExit( )
{
    BOSE_VERBOSE( s_logger, "CustomProductControllerStateBooting is being exited." );
    ProductControllerStateBooting::HandleStateExit( );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStateBooting::HandleLpmState
///
/// @param bool active This argument is true if the Low Power Microprocessor (LPM) is active;
///                    it is false otherwise.
///
/// @return This method will always return true, indicating that it has handled the event.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductControllerStateBooting::HandleLpmState( bool active )
{
    BOSE_VERBOSE( s_logger, "CustomProductControllerStateBooting is handling the LPM state." );

    if( active )
    {
        HandlePotentialStateChange( );
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStateBooting::HandleCapsState
///
/// @param bool active This argument is true if the Content Audio Playback Service (CAPS) is active;
///                    it is false otherwise.
///
/// @return This method will always return true, indicating that it has handled the event.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductControllerStateBooting::HandleCapsState( bool active )
{
    BOSE_VERBOSE( s_logger, "CustomProductControllerStateBooting is handling the CAPS state." );

    if( active )
    {
        HandlePotentialStateChange( );
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief  CustomProductControllerStateBooting::HandleAudioPathState
///
/// @param bool active This argument is true if the Audio Path is active and connected; it is false
///                     otherwise.
///
/// @return This method will always return true, indicating that it has handled the event.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductControllerStateBooting::HandleAudioPathState( bool active )
{
    BOSE_VERBOSE( s_logger, "CustomProductControllerStateBooting is handling the audio path state." );

    if( active )
    {
        HandlePotentialStateChange( );
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStateBooting::HandleSTSSourcesInit
///
/// @return This method will always return true, indicating that it has handled the event.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductControllerStateBooting::HandleSTSSourcesInit( void )
{
    BOSE_VERBOSE( s_logger, "CustomProductControllerStateBooting is handling the STSSourcesInit event." );

    HandlePotentialStateChange( );

    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStateBooting::HandlePotentialStateChange
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductControllerStateBooting::HandlePotentialStateChange( void )
{
    if( GetCustomProductController().IsBooted( ) )
    {
        if( GetCustomProductController().IsSoftwareUpdateRequired( ) )
        {
            BOSE_VERBOSE( s_logger, "%s is changing to %s.",
                          "CustomProductControllerStateBooting",
                          "CustomProductControllerStateUpdatingSoftware" );
            ChangeState( PROFESSOR_PRODUCT_CONTROLLER_STATE_SOFTWARE_UPDATING );
        }
        else
        {
            BOSE_VERBOSE( s_logger, "%s is changing to %s.",
                          "CustomProductControllerStateBooting",
                          "CustomProductControllerStatePlaying" );
            ChangeState( PROFESSOR_PRODUCT_CONTROLLER_STATE_PLAYABLE );
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
