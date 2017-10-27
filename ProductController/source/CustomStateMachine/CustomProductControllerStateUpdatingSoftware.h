////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file      CustomProductControllerStateUpdatingSoftware.h
///
/// @brief     This header file contains functionality to process events that occur during the
///            product software update state.
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
/// @class CustomProductControllerStateIdle
///
/// @brief This class is used for executing produce specific actions when in an idle state.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
class CustomProductControllerStateUpdatingSoftware : public ProductControllerState
{
public:

    CustomProductControllerStateUpdatingSoftware( ProductControllerHsm&       hsm,
                                                  CHsmState*                  pSuperState,
                                                  ProfessorProductController& productController,
                                                  Hsm::STATE                  stateId = PROFESSOR_PRODUCT_CONTROLLER_STATE_SOFTWARE_UPDATING,
                                                  const std::string&          name    = "CustomProductControllerStateSoftwareUpdating" );

    ~CustomProductControllerStateUpdatingSoftware( ) override
    {

    }

    void HandleStateEnter( ) override;
    void HandleStateStart( ) override;
    void HandleStateExit( )  override;

    bool HandleLpmState( bool active )       override;
    bool HandleCapsState( bool active )      override;
    bool HandleAudioPathState( bool active ) override;
    bool HandleSTSSourcesInit( void )        override;

private:

    ProfessorProductController& m_productController;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
///                           End of the Product Application Namespace                           ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
