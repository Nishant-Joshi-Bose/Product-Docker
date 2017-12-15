////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file      CustomProductControllerStateAdaptIQ.cpp
///
/// @brief     This source code file contains functionality to process events that occur during the
///            AdaptIQ state.
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
#include "CustomProductControllerStateAdaptIQ.h"
#include "ProductControllerHsm.h"
#include "ProfessorProductController.h"
#include "ProductMessage.pb.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
///                            Start of Product Application Namespace                            ///
////////////////////////////////////////////////////////////////////////////////////////////////////
namespace ProductApp
{

////////////////////////////////////////////////////////////////////////////////////////////////////
///
///            Constant Definitions
///
////////////////////////////////////////////////////////////////////////////////////////////////////
constexpr uint32_t PAIRING_MAX_TIME_MILLISECOND_TIMEOUT_START = 4 * 60 * 1000;
constexpr uint32_t PAIRING_MAX_TIME_MILLISECOND_TIMEOUT_RETRY = 0 ;

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStateAdaptIQ::CustomProductControllerStateAdaptIQ
///
/// @param ProductControllerHsm&       hsm               This argument references the state machine.
///
/// @param CHsmState*                  pSuperState       This argument references the parent state.
///
/// @param ProfessorProductController& productController This argument references the product controller.
///
/// @param Hsm::STATE                  stateId           This enumeration represents the state ID.
///
/// @param const std::string&          name              This argument names the state.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
CustomProductControllerStateAdaptIQ::CustomProductControllerStateAdaptIQ

( ProductControllerHsm&       hsm,
  CHsmState*                  pSuperState,
  ProfessorProductController& productController,
  Hsm::STATE                  stateId,
  const std::string&          name )

    : ProductControllerState( hsm, pSuperState, stateId, name ),
      m_timer( APTimer::Create( productController.GetTask( ), "AdaptIQTimer" ) )
{
    BOSE_INFO( s_logger, "CustomProductControllerStateAdaptIQ is being constructed." );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStateAdaptIQ::HandleStateStart
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductControllerStateAdaptIQ::HandleStateStart( )
{
    BOSE_INFO( s_logger, "CustomProductControllerStateAdaptIQ is being started." );

}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStateAdaptIQ::HandleTimeOut
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductControllerStateAdaptIQ::HandleTimeOut( )
{
    BOSE_INFO( s_logger, "A time out during AdaptIQ has occurred." );

    ///
    /// Go to the superstate of this state, which should be the last state that the
    /// product controller was in, to resume functionality.
    ///
    ChangeState( GetSuperId( ) );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStateAdaptIQ::HandleAdaptIQState
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductControllerStateAdaptIQ::HandleAdaptIQState( ProductAdaptIQ aiqStatus )
{
        ///
        /// Go to the superstate of this pairing state, which should be the last state that the
        /// product controller was in, to resume functionality.
        ///
        ChangeState( GetSuperId( ) );

        m_timer->SetTimeouts( PAIRING_MAX_TIME_MILLISECOND_TIMEOUT_START,
                              PAIRING_MAX_TIME_MILLISECOND_TIMEOUT_RETRY );

        m_timer->Start( std::bind( &CustomProductControllerStateAdaptIQ::HandleTimeOut,
                                   this ) );

    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStateAdaptIQ::HandleStateExit
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductControllerStateAdaptIQ::HandleStateExit( )
{
    BOSE_INFO( s_logger, "CustomProductControllerStateAdaptIQ is being exited." );
    m_timer->Stop( );

}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                             End of Product Application Namespace                             ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                        End of File                                           ///
////////////////////////////////////////////////////////////////////////////////////////////////////
