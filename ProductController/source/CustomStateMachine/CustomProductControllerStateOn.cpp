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
#include "AudioService.pb.h"

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
      m_frontDoorClient( FrontDoor::FrontDoorClient::Create( "ProductControllerStateOn" ) ),
// *INDENT-OFF*
      m_volume( [ this ]( int v ) { UpdateFrontDoorVolume( v ); } )
// *INDENT-ON*
{
    BOSE_VERBOSE( s_logger, "CustomProductControllerStateOn is being constructed." );
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
    switch( action )
    {
    case KeyActionPb::KEY_ACTION_VOLUME_UP:
        m_volume++;
        break;

    case KeyActionPb::KEY_ACTION_VOLUME_DOWN:
        m_volume--;
        break;

    default:
        break;
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStateOn::UpdateFrontDoorVolume
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductControllerStateOn::UpdateFrontDoorVolume( int32_t v )
{
// *INDENT-OFF*
    auto respFunc = [](SoundTouchInterface::volume v) { };
    auto errFunc = []( FRONT_DOOR_CLIENT_ERRORS e ) { };
// *INDENT-ON*
    AsyncCallback<SoundTouchInterface::volume> respCb( respFunc, m_productController.GetTask() );
    AsyncCallback<FRONT_DOOR_CLIENT_ERRORS> errCb( errFunc, m_productController.GetTask() );

    SoundTouchInterface::volume volume;
    volume.set_value( v );

    m_frontDoorClient->SendPost<SoundTouchInterface::volume>(
        ProductApp::FRONTDOOR_AUDIO_VOLUME, volume, respFunc, errCb );
}


////////////////////////////////////////////////////////////////////////////////////////////////////
///                           End of the Product Application Namespace                           ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
