////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file      CustomProductControllerStateUpdatingSoftware.h
///
/// @brief     This source code file contains functionality to process events that occur during a
///            software updating state.
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
#include "ProductControllerHsm.h"
#include "ProfessorProductController.h"
#include "CustomProductControllerStateUpdatingSoftware.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
///                            Start of Product Application Namespace                            ///
////////////////////////////////////////////////////////////////////////////////////////////////////
namespace ProductApp
{

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStateUpdatingSoftware::CustomProductControllerStateUpdatingSoftware
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
CustomProductControllerStateUpdatingSoftware::CustomProductControllerStateUpdatingSoftware

( ProductControllerHsm&       hsm,
  CHsmState*                  pSuperState,
  Hsm::STATE                  stateId,
  const std::string&          name )

    : ProductControllerState( hsm, pSuperState, stateId, name )
{
    BOSE_VERBOSE( s_logger, "CustomProductControllerStateUpdatingSoftware is being constructed." );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStateUpdatingSoftware::HandleStateEnter
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductControllerStateUpdatingSoftware::HandleStateEnter()
{
    BOSE_VERBOSE( s_logger, "CustomProductControllerStateUpdatingSoftware is being entered." );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStateUpdatingSoftware::HandleStateStart
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductControllerStateUpdatingSoftware::HandleStateStart()
{
    BOSE_VERBOSE( s_logger, "CustomProductControllerStateUpdatingSoftware is being started." );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStateUpdatingSoftware::HandleStateExit
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductControllerStateUpdatingSoftware::HandleStateExit()
{
    BOSE_VERBOSE( s_logger, "CustomProductControllerStateUpdatingSoftware is being exited." );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief  CustomProductControllerStateUpdatingSoftware::HandleLpmState
///
/// @param  bool active
///
/// @return This method will always return true, indicating that it has handled the event.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductControllerStateUpdatingSoftware::HandleLpmState( bool active )
{
    BOSE_WARNING( s_logger, "%s is handling an LPM %s event.",
                  GetName( ).c_str( ),
                  active ? "activation" : "deactivation" );
    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief  CustomProductControllerStateUpdatingSoftware::HandleCapsState
///
/// @param  active
///
/// @return This method will always return true, indicating that it has handled the event.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductControllerStateUpdatingSoftware::HandleCapsState( bool active )
{
    BOSE_WARNING( s_logger, "%s is handling a CAPS %s event.",
                  GetName( ).c_str( ),
                  active ? "activation" : "deactivation" );
    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief  CustomProductControllerStateUpdatingSoftware::HandleAudioPathState
///
/// @param  bool active
///
/// @return This method will always return true, indicating that it has handled the event.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductControllerStateUpdatingSoftware::HandleAudioPathState( bool active )
{
    BOSE_WARNING( s_logger, "%s is handling an audio path %s event.",
                  GetName( ).c_str( ),
                  active ? "activation" : "deactivation" );
    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief  CustomProductControllerStateUpdatingSoftware::HandleSTSSourcesInit
///
/// @return This method will always return true, indicating that it has handled the event.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductControllerStateUpdatingSoftware::HandleSTSSourcesInit( )
{
    BOSE_WARNING( s_logger, "%s is handling an STS initialization.", GetName( ).c_str( ) );
    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                           End of the Product Application Namespace                           ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
