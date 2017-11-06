////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file      CustomProductControllerStateIdleVoiceUnconfigured.h
///
/// @brief     This source code file contains functionality to process events that occur during the
///            product idle state when the voice for a Virtual Personal Assistant (VPA) is
///            unconfigured.
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
/// @class CustomProductControllerStateIdleVoiceUnconfigured
///
/// @brief This class is used for executing produce specific actions when in an idle voice
///        unconfigured state.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
class CustomProductControllerStateIdleVoiceUnconfigured : public ProductControllerState
{
public:

    CustomProductControllerStateIdleVoiceUnconfigured

    ( ProductControllerHsm&       hsm,
      CHsmState*                  pSuperState,
      ProfessorProductController& productController,
      Hsm::STATE                  stateId = PROFESSOR_PRODUCT_CONTROLLER_STATE_IDLE_VOICE_UNCONFIGURED,
      const std::string&          name    = "CustomProductControllerStateIdleVoiceUnconfigured" );

    ~CustomProductControllerStateIdleVoiceUnconfigured( ) override
    {

    }

    void HandleStateEnter( ) override;
    void HandleStateStart( ) override;
    void HandleStateExit( )  override;

    bool HandleAutowakeStatus( bool active )                   override;
    bool HandleNetworkState( bool configured, bool connected ) override;
    bool HandleVoiceState( bool configured )                   override;

private:

    ////////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief This method handles potential state changes for both network and voice status events.
    ///
    ////////////////////////////////////////////////////////////////////////////////////////////////
    void HandlePotentialStateChange( bool networkConfigured, bool networkConnected, bool voiceConfigured );

    ////////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief This timer is used to monitor the amount of time the device is in this state.
    ///
    ////////////////////////////////////////////////////////////////////////////////////////////////
    APTimerPtr m_timer;

    ////////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief This method will be invoked by the expired timer defined above.
    ///
    ////////////////////////////////////////////////////////////////////////////////////////////////
    void HandleTimeOut( );
};

////////////////////////////////////////////////////////////////////////////////////////////////////
///                           End of the Product Application Namespace                           ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
