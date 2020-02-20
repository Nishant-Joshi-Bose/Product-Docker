////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file      CustomProductControllerStatePlaying.cpp
///
/// @brief     This source code file contains functionality to process events that occur during the
///            product playing state.
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
#include "CustomProductControllerStatePlaying.h"
#include "ProductControllerHsm.h"
#include "ProductControllerState.h"
#include "CustomProductLpmHardwareInterface.h"
#include "CustomProductController.h"
#include "ProductBLERemoteManager.h"
#include "ProductEndpointDefines.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
///                            Start of Product Application Namespace                            ///
////////////////////////////////////////////////////////////////////////////////////////////////////
namespace ProductApp
{

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStatePlaying::CustomProductControllerStatePlaying
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
CustomProductControllerStatePlaying::CustomProductControllerStatePlaying(
    ProductControllerHsm&       hsm,
    CHsmState*                  pSuperState,
    Hsm::STATE                  stateId,
    const std::string&          name )

    : ProductControllerStatePlaying( hsm, pSuperState, stateId, name )
{
    BOSE_INFO( s_logger, "The %s state is being constructed.", name.c_str() );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name CustomProductControllerStatePlaying::HandleStateEnter
///
/// @brief The CEC is powered on when this state is entered, which occurs after the playing
///        transition state has brought the product to full power.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductControllerStatePlaying::HandleStateEnter( )
{
    ProductControllerStatePlaying::HandleStateEnter( );

    BOSE_INFO( s_logger, "The %s state is in %s powering CEC on.", GetName( ).c_str( ), __func__ );
    GetCustomProductController( ).GetDspHelper( )->SetNormalOperationsMonitor( true );

    // Limit the volume to threshold when entering PLAYING, perhaps volume was changed while system was in another state
    SetVolumeToThresholdLimit( );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name  CustomProductControllerStatePlaying::HandleStateExit
///
/// @brief The CEC is powered off when this state is exited occuring before a change to the
///        playable transition state takes place, which will bring the product to idle or network
///        standby power mode.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductControllerStatePlaying::HandleStateExit( )
{
    ProductControllerStatePlaying::HandleStateExit( );

    BOSE_INFO( s_logger, "The %s state is in %s powering CEC off.", GetName( ).c_str( ), __func__ );
    GetCustomProductController( ).GetDspHelper()->SetNormalOperationsMonitor( false );

    // Limit the volume to threshold when exiting PLAYING, so UI can show the value expected when we resume playing
    SetVolumeToThresholdLimit( );

    GetCustomProductController( ).PowerMacroOff();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name  CustomProductControllerStatePlaying::SetVolumeToThresholdLimit
///
/// @brief Consult ProductController and set volume to the limited value if desired
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductControllerStatePlaying::SetVolumeToThresholdLimit( )
{
    auto desiredPlayingVolumePair = GetCustomProductController( ).GetDesiredPlayingVolume( );
    if( desiredPlayingVolumePair.first )
    {
        CAPSAPI::volume v;
        v.set_value( desiredPlayingVolumePair.second );
        GetCustomProductController( ).GetFrontDoorClient( )->SendPut<CAPSAPI::volume, FrontDoor::Error>(
            FRONTDOOR_AUDIO_VOLUME_API, v, {}, FrontDoorErrorCallback );
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief  CustomProductControllerStatePlaying::ShouldPlayVolumeTone
/// @param  canPlay , reference to bool variable
/// @return This method returns true
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductControllerStatePlaying::ShouldPlayVolumeTone( bool &canPlay )
{
    BOSE_DEBUG( s_logger, "The %s state is in %s ", GetName( ).c_str( ), __func__ );
    canPlay = true;
    return true;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
///                           End of the Product Application Namespace                           ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
