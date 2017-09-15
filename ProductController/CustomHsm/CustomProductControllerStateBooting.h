////////////////////////////////////////////////////////////////////////////////
/// @file   CustomProductControllerStateBooting.h
/// @brief  The product specific 'Booting' state.
///
/// Copyright 2017 Bose Corporation
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <string>
#include "ProductControllerStateBooting.h"
#include "ProductControllerStates.h"
#include "HsmState.h"

namespace ProductApp
{
class ProductControllerHsm;
class HelloWorldProductController;

class CustomProductControllerStateBooting : public ProductControllerStateBooting
{
public:
    CustomProductControllerStateBooting( ProductControllerHsm& hsm,
                                         CHsmState* pSuperState,
                                         HelloWorldProductController& helloWorldProductController,
                                         STATE stateId = CUSTOM_PRODUCT_CONTROLLER_STATE_BOOTING,
                                         const std::string& name = "CustomProductControllerStateBooting" );

    virtual ~CustomProductControllerStateBooting()
    {
    }

    void HandleStateEnter() override;
    void HandleStateStart() override;
    void HandleStateExit() override;

    //The function that handles system/language endpoint request.
    bool HandleLanguageRequest() override;
};
} // namespace ProductApp
