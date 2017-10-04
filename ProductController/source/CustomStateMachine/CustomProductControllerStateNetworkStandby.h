////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file      CustomProductControllerStateNetworkStandby.h
///
/// @brief     This source code file contains functionality to process events that occur during the
///            product network standby state.
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
#include "ProductControllerStateNetworkStandby.h"
#include "ProductControllerStates.h"
#include "HsmState.h"

namespace ProductApp
{
    ///
    /// Forward Class Declarations
    ///
    class ProductControllerHsm;
    class ProfessorProductController;

    class CustomProductControllerStateNetworkStandby : public ProductControllerStateNetworkStandby
    {
      public:

          CustomProductControllerStateNetworkStandby( ProductControllerHsm&       hsm,
                                                      CHsmState*                  pSuperState,
                                                      ProfessorProductController& productController,
                                                      Hsm::STATE                  stateId = PROFESSOR_PRODUCT_CONTROLLER_STATE_NETWORK_STANDBY,
                                                      const std::string&          name    = "CustomProductControllerStateNetworkStandby" );

          virtual ~CustomProductControllerStateNetworkStandby( )
          {

          }

          void HandleStateEnter( ) override;
          void HandleStateStart( ) override;
          void HandleStateExit ( ) override;

          bool HandleLpmState     ( bool active )  override;
          bool HandleCapsState    ( bool active )  override;
          bool HandleNetworkState ( bool active )  override;
    };
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                        End of File                                           ///
////////////////////////////////////////////////////////////////////////////////////////////////////
