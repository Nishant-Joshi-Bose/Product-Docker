////////////////////////////////////////////////////////////////////////////////
/// @file   ProductAppStateStandby.h
/// @brief  The Standby State in the Product Application HSM. This state is
///         active on as long as the product is asked to play something or
///         receives any other event that requires state transition.
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
class ProductAppStateStandby : public ProductAppState
{
public:
    ProductAppStateStandby( ProductAppHsm& hsm,
                            CHsmState* pSuperState,
                            ProductController& productController,
                            STATE stateId = PRODUCT_APP_STATE_STANDBY,
                            const std::string& name = "ProductAppStateStandby" );

    virtual ~ProductAppStateStandby()
    {
    }

    void HandleStateEnter() override;
    void HandleStateStart() override;
    void HandleStateExit() override;
};
} // namespace ProductApp
