////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file      CustomProductControllerStateNetworkStandbyUnconfigured.h
///
/// @brief     This source code file contains functionality to process events that occur during the
///            product network standby unconfigured state.
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
#include "APTimer.h"

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
/// @class CustomProductControllerStateNetworkStandbyUnconfigured
///
/// @brief This class is used for executing produce specific actions when in an network standby
///        unconfigured state.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
class CustomProductControllerStateNetworkStandbyUnconfigured : public ProductControllerState
{
public:

    CustomProductControllerStateNetworkStandbyUnconfigured
    ( ProductControllerHsm&       hsm,
      CHsmState*                  pSuperState,
      ProfessorProductController& productController,
      Hsm::STATE                  stateId = PROFESSOR_PRODUCT_CONTROLLER_STATE_NETWORK_STANDBY_UNCONFIGURED,
      const std::string&          name    = "CustomProductControllerStateNetworkStandbyUnconfigured" );

    virtual ~CustomProductControllerStateNetworkStandbyUnconfigured( )
    {

    }

    void HandleStateEnter( ) override;
    void HandleStateStart( ) override;
    void HandleStateExit( )  override;

    bool HandleNetworkState( bool configured, bool connected ) override;
    bool HandleVoiceState( bool configured )                   override;

private:

    ProfessorProductController& m_productController;

    ////////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief This timer is used to monitor the amount of time the device is in this state. It is
    ///        armed on entry to this state and stopped on exit to this state. If it expires in
    ///        2 hours, the HandleTimeOut method declared below will be invoked.
    ///
    ////////////////////////////////////////////////////////////////////////////////////////////////
    APTimerPtr m_timer;

    ////////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief This method will be invoked by an expired timer, which is defined above and armed on
    ///        entry to this state, if the device has remained in a network standby unconfigured
    ///        state for 2 hours.
    ///
    ////////////////////////////////////////////////////////////////////////////////////////////////
    void HandleTimeOut( void );
};

////////////////////////////////////////////////////////////////////////////////////////////////////
///                             End of Product Application Namespace                             ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                        End of File                                           ///
////////////////////////////////////////////////////////////////////////////////////////////////////
