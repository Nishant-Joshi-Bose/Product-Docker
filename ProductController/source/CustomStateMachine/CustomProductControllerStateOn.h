////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file      CustomProductControllerStateOn.h
///
/// @brief     This source code file contains functionality to process events that occur during the
///            product on state. It is an all encompassing state that includes the playing and
///            playable substates.
///
/// @author    Stuart J. Lumby
///
/// @date      10/12/2017
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
/// The following compiler directive prevents this header file from being included more than once,
/// which may cause multiple declaration compiler errors.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////
///
///            Included Header Files
///
////////////////////////////////////////////////////////////////////////////////////////////////////
#include <string>
#include "ProductControllerState.h"
#include "ProductControllerStates.h"
#include "HsmState.h"
#include "AudioVolume.h"
#include "FrontDoorClientIF.h"
#include "FrontDoorClient.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
///                            Start of Product Application Namespace                            ///
////////////////////////////////////////////////////////////////////////////////////////////////////
namespace ProductApp
{

////////////////////////////////////////////////////////////////////////////////////////////////////
///
///            Forward Class Declarations
///
////////////////////////////////////////////////////////////////////////////////////////////////////
class ProductControllerHsm;
class ProfessorProductController;

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @class CustomProductControllerState
///
/// @brief This class is used for executing produce specific actions when in an on state.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
class CustomProductControllerStateOn : public ProductControllerState
{
public:

    CustomProductControllerStateOn( ProductControllerHsm&       hsm,
                                    CHsmState*                  pSuperState,
                                    ProfessorProductController& productController,
                                    Hsm::STATE                  stateId = PROFESSOR_PRODUCT_CONTROLLER_STATE_ON,
                                    const std::string&          name    = "CustomProductControllerStateOn" );

    ~CustomProductControllerStateOn( ) override
    {

    }

    void HandleStateEnter( ) override;
    void HandleStateStart( ) override;
    void HandleStateExit( )  override;
    bool HandleKeyAction( int action ) override;

private:
    void UpdateFrontDoorVolume( int32_t volume );
    void HandleResponse();
    void HandleError( FRONT_DOOR_CLIENT_ERRORS e );

    ProfessorProductController&         m_productController;
    std::shared_ptr<FrontDoorClientIF>  m_frontDoorClient;
    AudioVolume<int32_t>                m_volume;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
///                           End of the Product Application Namespace                           ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
