////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file      CustomProductControllerStateBooting.h
///
/// @brief     This source code file contains functionality to process events that occur during the
///            product booting state.
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
/// The following declares a DPrint class type object and a standard string for logging information
/// in this source code file.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
static DPrint s_logger( "Product" );

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
                                                                          ProfessorProductController& productController,
                                                                          Hsm::STATE                  stateId,
                                                                          const std::string&          name )

    : ProductControllerStateBooting( hsm, pSuperState, productController, stateId, name ),
      m_productController( productController )
{
    BOSE_DEBUG( s_logger, "The product booting state has been constructed." );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStateBooting::HandleStateEnter
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductControllerStateBooting::HandleStateEnter( )
{
    BOSE_DEBUG( s_logger, "The product booting state is being entered." );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStateBooting::HandleStateStart
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductControllerStateBooting::HandleStateStart( )
{
    BOSE_DEBUG( s_logger, "The product booting state is being started." );

    HandlePotentialStateChange( );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStateBooting::HandleStateExit
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductControllerStateBooting::HandleStateExit( )
{
    BOSE_DEBUG( s_logger, "The product booting state is being exited." );
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
    BOSE_DEBUG( s_logger, "The product booting state is handling the LPM state." );

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
    BOSE_DEBUG( s_logger, "The product booting state is handling the CAPS state." );

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
    BOSE_DEBUG( s_logger, "The product booting state is handling the audio path state." );

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
    BOSE_DEBUG( s_logger, "The product booting state is handling the STSSourcesInit event." );

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
    if( m_productController.IsBooted( ) )
    {
        if( m_productController.IsSoftwareUpdateRequired( ) )
        {
            BOSE_DEBUG( s_logger, "The product booting state is changing to the software updating state." );
            ChangeState( PROFESSOR_PRODUCT_CONTROLLER_STATE_SOFTWARE_UPDATING );
        }
        else
        {
            BOSE_DEBUG( s_logger, "The product booting state is changing to the playable state." );

            ChangeState( PROFESSOR_PRODUCT_CONTROLLER_STATE_PLAYABLE );
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                             End of Product Application Namespace                             ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                        End of File                                           ///
////////////////////////////////////////////////////////////////////////////////////////////////////
