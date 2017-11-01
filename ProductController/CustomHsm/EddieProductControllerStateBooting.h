////////////////////////////////////////////////////////////////////////////////
/// @file   EddieProductControllerStateBooting.h
/// @brief  The Booting State in the Eddie Product.
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
class EddieProductControllerHsm;

class EddieProductControllerStateBooting : public ProductControllerStateBooting
{
public:
    EddieProductControllerStateBooting( EddieProductControllerHsm& hsm,
                                        CHsmState* pSuperState,
                                        Hsm::STATE stateId = PRODUCT_CONTROLLER_STATE_BOOTING,
                                        //Don't change below state name. This is mapped with Madrid's requirement.
                                        const std::string& name = "BOOTING" );

    virtual ~EddieProductControllerStateBooting()
    {
    }

    void HandleStateEnter() override;
    void HandleStateStart() override;
    void HandleStateExit() override;

    ///////////////////////////////////////////////////////////////////////////////
    /// @name   HandleLpmState
    /// @brief  Handles state when LPM is ready
    /// @param  isActive - bool - is LPM Ready or not
    /// @return bool
    ///////////////////////////////////////////////////////////////////////////////
    bool HandleLpmState( bool isActive )  override;

    ///////////////////////////////////////////////////////////////////////////////
    /// @name   HandleLpmInterfaceState
    /// @brief  Handles state when connection to LpmServer has been established
    /// @param  isConnected - bool - is LpmClient connected or not
    /// @return bool
    ///////////////////////////////////////////////////////////////////////////////
    bool HandleLpmInterfaceState( bool isConnected );

    bool HandleModulesReady() override;
    bool HandleIntents( KeyHandlerUtil::ActionType_t result ) override;

private:
    ///////////////////////////////////////////////////////////////////////////////
    /// @name   GoToNextState
    /// @brief  Determines the logic to move ProductController to next state
    /// @return none
    ///////////////////////////////////////////////////////////////////////////////
    void GoToNextState();
};
} // namespace ProductApp
