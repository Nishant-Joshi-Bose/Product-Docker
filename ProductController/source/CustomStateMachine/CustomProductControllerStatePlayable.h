////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file      CustomProductControllerStatePlayable.h
///
/// @brief     This source code file contains functionality to process events that occur during the
///            product playing state.
///
/// @attention Copyright (C) 2018 Bose Corporation All Rights Reserved
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
#include "ProductControllerStatePlayable.h"
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

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @class CustomProductControllerStatePlayable
///
/// @brief This class is used for executing produce specific actions when in an playing state.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
class CustomProductControllerStatePlayable : public ProductControllerStatePlayable
{
public:

    CustomProductControllerStatePlayable( ProductControllerHsm& hsm,
                                          CHsmState*            pSuperState,
                                          Hsm::STATE            stateId,
                                          const std::string&    name  = "CustomPlayable" );

    ~CustomProductControllerStatePlayable( ) override
    {

    }

    bool HandleIntentMuteControl( KeyHandlerUtil::ActionType_t intent )   override;
    bool HandleIntentVolumeControl( KeyHandlerUtil::ActionType_t intent ) override;

};

////////////////////////////////////////////////////////////////////////////////////////////////////
///                           End of the Product Application Namespace                           ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
