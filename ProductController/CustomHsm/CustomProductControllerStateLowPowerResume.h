////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file      CustomProductControllerStateLowPowerResume.h
///
/// @brief     This source code file contains functionality to process events that occur during the
///            product low power state. In addition it has a queue of depth 1 to store the users
///            desired action while resuming.
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
#include "ProductControllerStateLowPowerResume.h"
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
/// @class CustomProductControllerState
///
/// @brief This class is used for executing produce specific actions when in an on state.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
class CustomProductControllerStateLowPowerResume : public ProductControllerStateLowPowerResume
{
public:

    CustomProductControllerStateLowPowerResume( ProductControllerHsm& hsm,
                                                CHsmState*            pSuperState,
                                                Hsm::STATE            stateId,
                                                const std::string&    name    = "CustomProductControllerStateLowPowerResume" );

    ~CustomProductControllerStateLowPowerResume( ) override
    {

    }
    bool HandleLPMPowerStatusNetworkStandby()                                 override;
    bool HandleIntentPlayProductSource( KeyHandlerUtil::ActionType_t intent ) override;
    bool HandleIntentPlaySoundTouchSource( )                                  override;

};

////////////////////////////////////////////////////////////////////////////////////////////////////
///                           End of the Product Application Namespace                           ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
