////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file      CustomProductControllerStatePlayingActive.cpp
///
/// @brief     This source code file contains functionality to process events that occur during the
///            product playing active state.
///
/// @author    Stuart J. Lumby
///
/// @date      10/24/2017
///
/// @todo      There are two timers to consider. Four hours for no user action and 20 minutes for
///            no audio being rendered, which are handled by this state and the custom state
///            CustomProductControllerStatePlayingInactive. There is the possibility where audio is
///            not playing and in this state, where audio is simply playing packets comprised of all
///            0s. This may need to be discussed with the Audio Path team to determine how to fully
///            implement this functionality.
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
#include "DPrint.h"
#include "Utilities.h"
#include "ProductControllerHsm.h"
#include "ProfessorProductController.h"
#include "ProductControllerState.h"
#include "CustomProductControllerStatePlayingActive.h"

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
constexpr uint32_t PLAYING_INACTIVE_MILLISECOND_TIMEOUT_START = ( ( 4 * 60 ) * 60 ) * 1000;
constexpr uint32_t PLAYING_INACTIVE_MILLISECOND_TIMEOUT_RETRY = 0;

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStatePlayingActive::CustomProductControllerStatePlayingActive
///
/// @param hsm
///
/// @param pSuperState
///
/// @param productController
///
/// @param stateId
///
/// @param name
///
////////////////////////////////////////////////////////////////////////////////////////////////////
CustomProductControllerStatePlayingActive::CustomProductControllerStatePlayingActive

( ProductControllerHsm&       hsm,
  CHsmState*                  pSuperState,
  ProfessorProductController& productController,
  Hsm::STATE                  stateId,
  const std::string&          name )

    : ProductControllerState( hsm, pSuperState, productController, stateId, name ),
      m_productController( productController ),
      m_timer( APTimer::Create( m_productController.GetTask( ), "PlayingInactiveTimer" ) )
{
    BOSE_VERBOSE( s_logger, "CustomProductControllerStatePlayingActive is being constructed." );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStatePlayingActive::HandleStateEnter
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductControllerStatePlayingActive::HandleStateEnter( )
{
    BOSE_VERBOSE( s_logger, "CustomProductControllerStatePlayingActive is being entered." );
    BOSE_VERBOSE( s_logger, "A timer for user inactivity will be set to expire in %d minutes.",
                  PLAYING_INACTIVE_MILLISECOND_TIMEOUT_START / 60000 );

    m_timer->SetTimeouts( PLAYING_INACTIVE_MILLISECOND_TIMEOUT_START,
                          PLAYING_INACTIVE_MILLISECOND_TIMEOUT_RETRY );

    m_timer->Start( std::bind( &CustomProductControllerStatePlayingActive::HandleTimeOut,
                               this ) );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStatePlayingActive::HandleStateStart
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductControllerStatePlayingActive::HandleStateStart( )
{
    BOSE_VERBOSE( s_logger, "CustomProductControllerStatePlayingActive is being started." );
    BOSE_VERBOSE( s_logger, "A playback will be initiated at this point when supported." );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStatePlayingActive::HandleStateExit
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductControllerStatePlayingActive::HandleStateExit( )
{
    BOSE_VERBOSE( s_logger, "CustomProductControllerStatePlayingActive is being exited." );
    BOSE_VERBOSE( s_logger, "The timer will be stopped." );

    m_timer->Stop( );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStatePlayingActive::HandleTimeOut
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductControllerStatePlayingActive::HandleTimeOut( void )
{
    BOSE_VERBOSE( s_logger, "A time out in CustomProductControllerStatePlayingActive has occurred." );

    m_timer->Stop( );

    BOSE_VERBOSE( s_logger, "%s is changing to %s.",
                  "CustomProductControllerStatePlayingActive",
                  "CustomProductControllerStatePlayable" );
    ChangeState( PROFESSOR_PRODUCT_CONTROLLER_STATE_PLAYABLE );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief  CustomProductControllerStatePlayingActive::HandlePlaybackRequest
///
/// @param  ProductPlaybackRequest_ProductPlaybackState state
///
/// @return This method returns a true Boolean value indicating that it has handled the playback
///         status and no futher processing will be required by any of its superstates.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductControllerStatePlayingActive::
HandlePlaybackRequest( ProductPlaybackRequest_ProductPlaybackState state )
{
    BOSE_VERBOSE( s_logger, "CustomProductControllerStatePlayingActive is handling a playback request." );

    if( state == ProductPlaybackRequest_ProductPlaybackState_Play )
    {
        BOSE_VERBOSE( s_logger, "A new playback play request will be initiated when supported." );
    }
    else if( state == ProductPlaybackRequest_ProductPlaybackState_Pause )
    {
        BOSE_VERBOSE( s_logger, "A playback pause event was sent." );
        BOSE_VERBOSE( s_logger, "%s is changing to %s.",
                      "CustomProductControllerStatePlayingActive",
                      "CustomProductControllerStatePlayingInactive" );
        ChangeState( PROFESSOR_PRODUCT_CONTROLLER_STATE_PLAYING_INACTIVE );
    }
    else if( state == ProductPlaybackRequest_ProductPlaybackState_Stop )
    {
        BOSE_VERBOSE( s_logger, "A playback stop event was sent." );
        BOSE_VERBOSE( s_logger, "%s is changing to %s.",
                      "CustomProductControllerStatePlayingActive",
                      "CustomProductControllerStatePlayingInactive" );
        ChangeState( PROFESSOR_PRODUCT_CONTROLLER_STATE_PLAYING_INACTIVE );
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief  CustomProductControllerStatePlayingActive::HandleKeyAction
///
/// @param  int action
///
/// @return This method returns a false Boolean value in case processing of the key needs to be
///         handled by any of the superstates.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductControllerStatePlayingActive::HandleKeyAction( int action )
{
    BOSE_VERBOSE( s_logger, "A key action was sent to CustomProductControllerStatePlayingActive." );
    BOSE_VERBOSE( s_logger, "The timer will be stopped and reset based on user activity." );

    m_timer->Stop( );

    m_timer->SetTimeouts( PLAYING_INACTIVE_MILLISECOND_TIMEOUT_START,
                          PLAYING_INACTIVE_MILLISECOND_TIMEOUT_RETRY );

    m_timer->Start( std::bind( &CustomProductControllerStatePlayingActive::HandleTimeOut,
                               this ) );

    return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                           End of the Product Application Namespace                           ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
