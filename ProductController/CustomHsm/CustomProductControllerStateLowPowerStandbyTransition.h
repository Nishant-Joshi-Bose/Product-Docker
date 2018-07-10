////////////////////////////////////////////////////////////////////////////////
/// @file   CustomProductControllerStateLowPowerStandbyTransition.h
/// @brief  Custom override state in the product controller for transitioning
/// into and out of low power standby.
///
/// Copyright 2017 Bose Corporation
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <string>
#include "ProductControllerStateLowPowerStandbyTransition.h"
#include "HsmState.h"

namespace ProductApp
{
class ProductControllerHsm;

class CustomProductControllerStateLowPowerStandbyTransition : public ProductControllerStateLowPowerStandbyTransition
{
public:
    CustomProductControllerStateLowPowerStandbyTransition( ProductControllerHsm& hsm,
                                                           CHsmState* pSuperState,
                                                           Hsm::STATE stateId,
                                                           const std::string& name = "LowPowerStandbyTransition" );

    virtual ~CustomProductControllerStateLowPowerStandbyTransition() override
    {
    }
    void HandleStateStart() override;

protected:

    /*! \copydoc ProductControllerStateLowPowerStandbyTransition::IsReadyForLowPowerState()
     * Include custom handling for the LCD via DisplayController.
     */
    bool IsReadyForLowPowerState() const override;

private:

    /*! \brief Flag that the display controller is ready.
     */
    void SetDisplayControllerIsReady();

    bool m_displayControllerIsReady;        //!< Received response from DisplayController.
};
} /// namespace ProductApp
