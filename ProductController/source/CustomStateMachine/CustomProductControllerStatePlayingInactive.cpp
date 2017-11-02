////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file      CustomProductControllerStatePlayingInactive.cpp
///
/// @brief     This source code file contains functionality to process events that occur during the
///            product playing inactive state.
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
#include "DPrint.h"
#include "Utilities.h"
#include "ProductControllerHsm.h"
#include "ProfessorProductController.h"
#include "ProductControllerState.h"
#include "CustomProductControllerStatePlayingInactive.h"

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
constexpr uint32_t PLAYING_NO_AUDIO_MILLISECOND_TIMEOUT_START = ( 20 * 60 ) * 1000;
constexpr uint32_t PLAYING_NO_AUDIO_MILLISECOND_TIMEOUT_RETRY = 0;

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStatePlayingInactive::CustomProductControllerStatePlayingInactive
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
CustomProductControllerStatePlayingInactive::CustomProductControllerStatePlayingInactive

( ProductControllerHsm&       hsm,
  CHsmState*                  pSuperState,
  ProfessorProductController& productController,
  Hsm::STATE                  stateId,
  const std::string&          name )

    : ProductControllerState( hsm, pSuperState, stateId, name ),
      m_timer( APTimer::Create( productController.GetTask( ), "PlayingInactiveNoAudioTimer" ) )
{
    BOSE_VERBOSE( s_logger, "CustomProductControllerStatePlayingInactive is being constructed." );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStatePlayingInactive::HandleStateEnter
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductControllerStatePlayingInactive::HandleStateEnter( )
{
    BOSE_VERBOSE( s_logger, "CustomProductControllerStatePlayingInactive is being entered." );
    BOSE_VERBOSE( s_logger, "A timer for audio being out will be set to expire in %d minutes.",
                  PLAYING_NO_AUDIO_MILLISECOND_TIMEOUT_START / 60000 );

    m_timer->SetTimeouts( PLAYING_NO_AUDIO_MILLISECOND_TIMEOUT_START,
                          PLAYING_NO_AUDIO_MILLISECOND_TIMEOUT_RETRY );

    m_timer->Start( std::bind( &CustomProductControllerStatePlayingInactive::HandleTimeOut,
                               this ) );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStatePlayingInactive::HandleStateStart
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductControllerStatePlayingInactive::HandleStateStart( )
{
    BOSE_VERBOSE( s_logger, "CustomProductControllerStatePlayingInactive is being started." );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStatePlayingInactive::HandleStateExit
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductControllerStatePlayingInactive::HandleStateExit( )
{
    BOSE_VERBOSE( s_logger, "CustomProductControllerStatePlayingInactive is being exited." );
    BOSE_VERBOSE( s_logger, "The timer will be stopped." );

    m_timer->Stop( );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStatePlayingInactive::HandleTimeOut
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductControllerStatePlayingInactive::HandleTimeOut( void )
{
    BOSE_VERBOSE( s_logger, "A time out in CustomProductControllerStatePlayingInactive has occurred." );

    m_timer->Stop( );

    BOSE_VERBOSE( s_logger, "%s is changing to %s.",
                  "CustomProductControllerStatePlayingInactive",
                  "CustomProductControllerStatePlayable" );
    ChangeState( PROFESSOR_PRODUCT_CONTROLLER_STATE_PLAYABLE );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief  CustomProductControllerStatePlayingInactive::HandlePlaybackRequest
///
/// @param  ProductPlaybackRequest_ProductPlaybackState state
///
/// @return This method returns a true Boolean value indicating that it has handled the playback
///         status and no futher processing will be required by any of its superstates.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductControllerStatePlayingInactive::
HandlePlaybackRequest( ProductPlaybackRequest_ProductPlaybackState state )
{
    BOSE_VERBOSE( s_logger, "CustomProductControllerStatePlayingInactive is handling a playback request." );

    if( state == ProductPlaybackRequest_ProductPlaybackState_Play )
    {
        BOSE_VERBOSE( s_logger, "A playback play event was sent." );
        BOSE_VERBOSE( s_logger, "%s is changing to %s.",
                      "CustomProductControllerStatePlayingInactive",
                      "CustomProductControllerStatePlayingActive" );

        ChangeState( PROFESSOR_PRODUCT_CONTROLLER_STATE_PLAYING_ACTIVE );
    }
    else if( state == ProductPlaybackRequest_ProductPlaybackState_Pause )
    {
        BOSE_VERBOSE( s_logger, "CustomProductControllerStatePlayingInactive is not changing state." );
        BOSE_VERBOSE( s_logger, "A playback pause event was sent, and the timer will be stopped and reset." );

        m_timer->Stop( );

        m_timer->SetTimeouts( PLAYING_NO_AUDIO_MILLISECOND_TIMEOUT_START,
                              PLAYING_NO_AUDIO_MILLISECOND_TIMEOUT_RETRY );

        m_timer->Start( std::bind( &CustomProductControllerStatePlayingInactive::HandleTimeOut,
                                   this ) );
    }
    else if( state == ProductPlaybackRequest_ProductPlaybackState_Stop )
    {
        BOSE_VERBOSE( s_logger, "CustomProductControllerStatePlayingInactive is not changing state." );
        BOSE_VERBOSE( s_logger, "A playback stop event was sent, and the timer will be stopped and reset." );

        m_timer->Stop( );

        m_timer->SetTimeouts( PLAYING_NO_AUDIO_MILLISECOND_TIMEOUT_START,
                              PLAYING_NO_AUDIO_MILLISECOND_TIMEOUT_RETRY );

        m_timer->Start( std::bind( &CustomProductControllerStatePlayingInactive::HandleTimeOut,
                                   this ) );
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief  CustomProductControllerStatePlayingInactive::HandleKeyAction
///
/// @param  int action
///
/// @return This method returns a false Boolean value in case processing of the key needs to be
///         handled by any of the superstates.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductControllerStatePlayingInactive::HandleKeyAction( int action )
{
    BOSE_VERBOSE( s_logger, "A key action was sent to CustomProductControllerStatePlayingInactive." );
    BOSE_VERBOSE( s_logger, "The timer will be stopped and reset based on user activity." );

    m_timer->Stop( );

    m_timer->SetTimeouts( PLAYING_NO_AUDIO_MILLISECOND_TIMEOUT_START,
                          PLAYING_NO_AUDIO_MILLISECOND_TIMEOUT_RETRY );

    m_timer->Start( std::bind( &CustomProductControllerStatePlayingInactive::HandleTimeOut,
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
