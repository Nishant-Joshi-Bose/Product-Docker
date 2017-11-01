////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file      CustomProductControllerStateOn.cpp
///
/// @brief     This source code file contains functionality to process events that occur during the
///            product on state.
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
#include "CustomProductControllerStateOn.h"
#include "ProductControllerHsm.h"
#include "ProfessorProductController.h"
#include "ProductControllerState.h"
#include "KeyActions.pb.h"
#include "ProductHardwareInterface.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
///                            Start of Product Application Namespace                            ///
////////////////////////////////////////////////////////////////////////////////////////////////////
namespace ProductApp
{

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStateOn::CustomProductControllerStateOn
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
CustomProductControllerStateOn::CustomProductControllerStateOn( ProductControllerHsm&       hsm,
                                                                CHsmState*                  pSuperState,
                                                                ProfessorProductController& productController,
                                                                Hsm::STATE                  stateId,
                                                                const std::string&          name )

    : ProductControllerState( hsm, pSuperState, productController, stateId, name ),
      m_productController( productController ),
      m_frontDoorClient( FrontDoor::FrontDoorClient::Create( "ProductControllerStateOn" ) )
{
    BOSE_VERBOSE( s_logger, "CustomProductControllerStateOn is being constructed." );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStateOn::Create
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
/// @return Pointer to a CustomProductControllerState instance
///
////////////////////////////////////////////////////////////////////////////////////////////////////
CustomProductControllerStateOn *CustomProductControllerStateOn::Create(
    ProductControllerHsm&       hsm,
    CHsmState*                  pSuperState,
    ProfessorProductController& productController,
    Hsm::STATE                  stateId,
    const std::string&          name )
{
    auto *state = new CustomProductControllerStateOn( hsm,
                                                      pSuperState,
                                                      productController,
                                                      stateId,
                                                      name );

    state->Initialize();

    return state;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStateOn::Initialize
///
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductControllerStateOn::Initialize()
{
    auto fVolume = [ this ]( int32_t v )
    {
        UpdateFrontDoorVolume( v );
    };
    m_volume = std::make_shared< AudioVolume<int32_t> >( fVolume );

    auto fNotify = [ this ]( SoundTouchInterface::volume v )
    {
        ReceiveFrontDoorVolume( v );
    };
    m_frontDoorClient->RegisterNotification< SoundTouchInterface::volume >
    ( FRONTDOOR_AUDIO_VOLUME, fNotify );
}


////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStateOn::HandleStateEnter
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductControllerStateOn::HandleStateEnter( )
{
    BOSE_VERBOSE( s_logger, "CustomProductControllerStateOn is being entered." );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStateOn::HandleStateStart
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductControllerStateOn::HandleStateStart( )
{
    BOSE_VERBOSE( s_logger, "%s is being started and going to %s.",
                  "CustomProductControllerStateOn",
                  "CustomProductControllerStatePlayable" );

    ChangeState( PROFESSOR_PRODUCT_CONTROLLER_STATE_PLAYABLE );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStateOn::HandleStateExit
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductControllerStateOn::HandleStateExit( )
{
    BOSE_VERBOSE( s_logger, "CustomProductControllerStateOn is being exited." );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStateOn::HandleStateExit
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductControllerStateOn::HandleKeyAction( int action )
{
    bool handled = true;

    switch( action )
    {
    case KeyActionPb::KEY_ACTION_VOLUME_UP:
        ( *m_volume )++;
        break;

    case KeyActionPb::KEY_ACTION_VOLUME_DOWN:
        ( *m_volume )--;
        break;

    default:
        handled = false;
        break;
    }

    return handled;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                           End of the Product Application Namespace                           ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
