////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file      CustomProductControllerStatePlayingInactive.h
///
/// @brief     This source code file contains functionality to process events that occur during the
///            product playing state.
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
/// @class CustomProductControllerStatePlayingInactive
///
/// @brief This class is used for executing produce specific actions when in an playing inactive state.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
class CustomProductControllerStatePlayingInactive : public ProductControllerState
{
public:

    CustomProductControllerStatePlayingInactive

    ( ProductControllerHsm&       hsm,
      CHsmState*                  pSuperState,
      ProfessorProductController& productController,
      Hsm::STATE                  stateId = PROFESSOR_PRODUCT_CONTROLLER_STATE_PLAYING_INACTIVE,
      const std::string&          name    = "CustomProductControllerStatePlayingInactive" );

    virtual ~CustomProductControllerStatePlayingInactive()
    {

    }

    void HandleStateEnter( ) override;
    void HandleStateStart( ) override;
    void HandleStateExit( )  override;

    bool HandlePlaybackRequest( ProductPlaybackRequest_ProductPlaybackState state ) override;
    bool HandleKeyAction( int action ) override;

private:

    ProfessorProductController& m_productController;

    ////////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief This timer is used to monitor the amount of time the device is in this state. It is
    ///        armed on entry to this state and stopped on exit to this state. If it expires in
    ///        20 minutes, the HandleTimeOut method declared below will be invoked.
    ///
    ////////////////////////////////////////////////////////////////////////////////////////////////
    APTimerPtr m_timer;

    ////////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief This method will be invoked by an expired timer, which is defined above and armed on
    ///        entry to this state, if the device has remained in a inactive playing state for 20
    ///        minutes with no active audio or user interaction through a key action.
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
