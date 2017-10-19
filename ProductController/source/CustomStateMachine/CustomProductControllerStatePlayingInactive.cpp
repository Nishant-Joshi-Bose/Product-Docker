////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file      CustomProductControllerStatePlayingInactive.cpp
///
/// @brief     This source code file contains functionality to process events that occur during the
///            product playing inactive state.
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
#include "DPrint.h"
#include "CustomProductControllerState.h"
#include "ProductControllerHsm.h"
#include "ProfessorProductController.h"
#include "ProductControllerState.h"
#include "CustomProductControllerStatePlayingInactive.h"
#include "APTimer.h"

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
constexpr const uint32_t PLAYING_NO_AUDIO_MILLISECOND_TIMEOUT_START = ( ( 20 * 60 ) * 1000 );
constexpr const uint32_t PLAYING_NO_AUDIO_MILLISECOND_TIMEOUT_RETRY = ( 0 );

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// The following declares a DPrint class type object and a standard string for logging information
/// in this source code file.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
static DPrint s_logger( "Product" );

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

    : ProductControllerState( hsm, pSuperState, productController, stateId, name ),
      m_productController( productController ),
      m_timer( APTimer::Create( m_productController.GetTask( ), "PlayingInactiveNoAudioTimer" ) )
{
    BOSE_DEBUG( s_logger, "The product playing inactive state is being constructed." );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStatePlayingInactive::HandleStateEnter
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductControllerStatePlayingInactive::HandleStateEnter( )
{
    BOSE_DEBUG( s_logger, "The product playing inactive state is being entered." );
    BOSE_DEBUG( s_logger, "A timer for audio being out will be set to expire in 20 minutes." );

    m_timer->SetTimeouts( PLAYING_NO_AUDIO_MILLISECOND_TIMEOUT_START,
                          PLAYING_NO_AUDIO_MILLISECOND_TIMEOUT_RETRY  );

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
    BOSE_DEBUG( s_logger, "The product playing inactive state is being started." );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStatePlayingInactive::HandleStateExit
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductControllerStatePlayingInactive::HandleStateExit( )
{
    BOSE_DEBUG( s_logger, "The product playing inactive state is being exited." );
    BOSE_DEBUG( s_logger, "The timer will be stopped." );

    m_timer->Stop( );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStatePlayingInactive::HandleTimeOut
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductControllerStatePlayingInactive::HandleTimeOut( void )
{
      BOSE_DEBUG( s_logger, "A time out in the product playing inactive state has occurred." );
      BOSE_DEBUG( s_logger, "An attempt to set the device to a playable state will be made." );

      m_timer->Stop( );

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
    BOSE_DEBUG( s_logger, "The product playing inactive state is handling a playback request." );

    if( state == ProductPlaybackRequest_ProductPlaybackState_Play )
    {
        BOSE_DEBUG( s_logger, "The product playing inactive state is changing to a playing active state." );
        BOSE_DEBUG( s_logger, "A playback play event was sent." );

        ChangeState( PROFESSOR_PRODUCT_CONTROLLER_STATE_PLAYING_ACTIVE );
    }
    else if( state == ProductPlaybackRequest_ProductPlaybackState_Pause )
    {
        BOSE_DEBUG( s_logger, "The product playing inactive state is not changing state." );
        BOSE_DEBUG( s_logger, "A playback pause event was sent, and the timer will be stopped and reset." );

        m_timer->Stop( );

        m_timer->SetTimeouts( PLAYING_NO_AUDIO_MILLISECOND_TIMEOUT_START,
                              PLAYING_NO_AUDIO_MILLISECOND_TIMEOUT_RETRY  );

        m_timer->Start( std::bind( &CustomProductControllerStatePlayingInactive::HandleTimeOut,
                                   this ) );
    }
    else if( state == ProductPlaybackRequest_ProductPlaybackState_Stop )
    {
        BOSE_DEBUG( s_logger, "The product playing inactive state is not changing state." );
        BOSE_DEBUG( s_logger, "A playback stop event was sent, and the timer will be stopped and reset." );

        m_timer->Stop( );

        m_timer->SetTimeouts( PLAYING_NO_AUDIO_MILLISECOND_TIMEOUT_START,
                              PLAYING_NO_AUDIO_MILLISECOND_TIMEOUT_RETRY  );

        m_timer->Start( std::bind( &CustomProductControllerStatePlayingInactive::HandleTimeOut,
                                   this ) );
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief  CustomProductControllerStatePlayingActive::HandleKeyAction
///
/// @param  int action
///
/// @return This method returns a true Boolean value indicating that it has handled the key action
///         and no futher processing will be required by any of its superstates.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductControllerStatePlayingInactive::HandleKeyAction( int action )
{
    BOSE_DEBUG( s_logger, "A key action was sent to the product playing inactive state." );
    BOSE_DEBUG( s_logger, "The timer will be stopped and reset based on user activity." );

    m_timer->Stop( );

    m_timer->SetTimeouts( PLAYING_NO_AUDIO_MILLISECOND_TIMEOUT_START,
                          PLAYING_NO_AUDIO_MILLISECOND_TIMEOUT_RETRY  );

    m_timer->Start( std::bind( &CustomProductControllerStatePlayingInactive::HandleTimeOut,
                               this ) );

    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                             End of Product Application Namespace                             ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                        End of File                                           ///
////////////////////////////////////////////////////////////////////////////////////////////////////
