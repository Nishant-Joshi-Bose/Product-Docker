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

// This is just a placeholder
constexpr uint32_t ADAPTIQ_INACTIVITY_TIMEOUT   = 1 * 60 * 1000;

constexpr int ADAPTIQ_SPEAKER_FIRST             = 1;
constexpr int ADAPTIQ_SPEAKER_LAST              = 5;
constexpr int ADAPTIQ_LOCATION_FIRST            = 1;
constexpr int ADAPTIQ_LOCATION_LAST             = 5;

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
      m_timer( APTimer::Create( productController.GetTask( ), "AdaptIQTimer" ) ),
      m_AdaptIQManager( productController.GetAdaptIQManager( ) )
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

    m_timer->SetTimeouts( ADAPTIQ_INACTIVITY_TIMEOUT, 0 );
    m_timer->Start( [ = ]( )
    {
        HandleTimeOut();
    } );

    status.set_smstate( "NA" );
    status.set_mode( "Booting" );
    status.set_currentlocation( ADAPTIQ_LOCATION_FIRST );
    status.set_currentspeaker( ADAPTIQ_SPEAKER_FIRST );
    status.set_hpconnected( true );
    status.set_errorcode( 0 );
    m_AdaptIQManager->SetDefaultProperties( status );
    m_AdaptIQManager->SetStatus( status );
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
/// @brief CustomProductControllerStateAdaptIQ::HandleAdaptIQStatus
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductControllerStateAdaptIQ::HandleAdaptIQStatus( const ProductAdaptIQStatus& aiqStatus )
{
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
///
/// @brief  CustomProductControllerAdaptIQ::HandleAdaptIQControl
///
/// @return This method returns a true Boolean value indicating that it has handled the
///         AdaptIQControl request.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductControllerStateAdaptIQ::HandleAdaptIQControl( const ProductAdaptIQControl& cmd )
{
    // for now just forward the action on the the lpm / dsp; we'll do more complex stuff later
    switch( cmd.action() )
    {
    case ProductAdaptIQControl::Start:
        m_AdaptIQManager->SendAdaptIQControl( ProductAdaptIQControl::Start );
        break;

    case ProductAdaptIQControl::Cancel:
        m_AdaptIQManager->SendAdaptIQControl( ProductAdaptIQControl::Cancel );
        break;

    case ProductAdaptIQControl::Advance:
        m_AdaptIQManager->SendAdaptIQControl( ProductAdaptIQControl::Advance );
        break;

    case ProductAdaptIQControl::Previous:
        m_AdaptIQManager->SendAdaptIQControl( ProductAdaptIQControl::Previous );
        break;

    default:
        break;
    }

    return true;
}



////////////////////////////////////////////////////////////////////////////////////////////////////
///                             End of Product Application Namespace                             ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                        End of File                                           ///
////////////////////////////////////////////////////////////////////////////////////////////////////
