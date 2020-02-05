////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file      CustomProductControllerStateControlIntegration.h
///
/// @brief     This source code file contains functionality to process events that occur during the
///            product control integration state.
///
/// @attention Copyright (C) 2019 Bose Corporation All Rights Reserved
///
////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////
///
///            Included Header Files
///
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "ProductControllerState.h"
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
/// @class CustomProductControllerStateControlIntegration
///
/// @brief This class is used for executing control integration.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
class CustomProductControllerStateControlIntegration : public ProductControllerState
{
public:

    CustomProductControllerStateControlIntegration(
        ProductControllerHsm&       hsm,
        CHsmState*                  pSuperState,
        Hsm::STATE                  stateId,
        const std::string&          name = "ControlIntegration" );

    ~CustomProductControllerStateControlIntegration( ) override
    {

    }

    bool HandleIntentPlayProductSource( KeyHandlerUtil::ActionType_t intent )   override;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
///                             End of Product Application Namespace                             ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                        End of File                                           ///
////////////////////////////////////////////////////////////////////////////////////////////////////
