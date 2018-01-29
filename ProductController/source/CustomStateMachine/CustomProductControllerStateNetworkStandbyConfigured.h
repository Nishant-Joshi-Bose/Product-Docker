////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file      CustomProductControllerStateNetworkStandbyConfigured.h
///
/// @brief     This source code file contains functionality to process events that occur during the
///            product network standby configured state.
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
#include "ProductControllerStateNetworkStandbyConfigured.h"
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

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @class CustomProductControllerStateNetworkStandbyConfigured
///
/// @brief This class is used for executing produce specific actions when in an network standby
///        configured state.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
class CustomProductControllerStateNetworkStandbyConfigured :
    public ProductControllerStateNetworkStandbyConfigured
{
public:

    CustomProductControllerStateNetworkStandbyConfigured
    ( ProductControllerHsm&       hsm,
      CHsmState*                  pSuperState,
      Hsm::STATE                  stateId,
      const std::string&          name    = "CustomProductControllerStateNetworkStandbyConfigured" );

    ~CustomProductControllerStateNetworkStandbyConfigured( ) override
    {

    }

    bool HandleNetworkState( bool configured, bool connected ) override;
    bool HandleVoiceState( bool configured )                   override;

private:

    ////////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief This method make the appropriate state change for both network and voice status
    ///        events.
    ///
    ////////////////////////////////////////////////////////////////////////////////////////////////
    void GoToAppropriatePlayableState( bool networkConfigured,
                                       bool networkConnected,
                                       bool voiceConfigured );
};

////////////////////////////////////////////////////////////////////////////////////////////////////
///                           End of the Product Application Namespace                           ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
