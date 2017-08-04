////////////////////////////////////////////////////////////////////////////////
/// @file   ProductAppStateBooting.h
/// @brief  The Booting State in the Product Application HSM. This state is
///         active on until other software components are initialized.
///
/// Copyright 2017 Bose Corporation
////////////////////////////////////////////////////////////////////////////////

#include <string>

#include "ProductAppHsm.h"
#include "ProductAppState.h"
#include "ProductAppStates.h"
#include "HsmState.h"

namespace ProductApp
{
class ProductAppHsm;

class ProductAppStateBooting : public ProductAppState
{
public:
    ProductAppStateBooting( ProductAppHsm& hsm,
                            CHsmState* pSuperState,
                            ProductController& productController,
                            STATE stateId = ProductAppStates::PRODUCT_APP_STATE_BOOTING,
                            const std::string& name = "ProductAppStateBooting" );

    virtual ~ProductAppStateBooting()
    {
    }

    void HandleStateEnter() override;
    void HandleStateStart() override;
    void HandleStateExit() override;

    bool HandleSetupEndPoint( SoundTouchInterface::msg_Header const& cookie, std::string const& body, std::string const& operation ) override;
};
} // namespace ProductApp
