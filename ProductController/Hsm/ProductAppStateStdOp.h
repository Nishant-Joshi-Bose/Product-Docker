////////////////////////////////////////////////////////////////////////////////
/// @file   ProductAppStateStdOp.h
/// @brief  The StdOp State in the Product Application HSM. This state is
///         the home of common operations for example key presses.
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

class ProductAppStateStdOp : public ProductAppState
{
public:
    ProductAppStateStdOp( ProductAppHsm& hsm,
                          CHsmState* pSuperState,
                          ProductController& productController,
                          STATE stateId = PRODUCT_APP_STATE_STDOP,
                          const std::string& name = "ProductAppStateStdOp" );

    virtual ~ProductAppStateStdOp()
    {
    }

    void HandleStateEnter() override;
    void HandleStateStart() override;
    void HandleStateExit() override;

    bool HandleSetupEndPoint( SoundTouchInterface::msg_Header const& cookie, std::string const& body, std::string const& operation ) override;
};
} // namespace ProductApp
