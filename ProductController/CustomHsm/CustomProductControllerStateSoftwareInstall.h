////////////////////////////////////////////////////////////////////////////////
/// @file   CustomProductControllerStateSoftwareInstall.h
/// @brief  The SoftwareInstall State in the Eddie Product.
///
/// Copyright 2018 Bose Corporation
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <string>
#include "ProductControllerStateSoftwareInstall.h"
#include "HsmState.h"

namespace ProductApp
{
class ProductControllerHsm;

class CustomProductControllerStateSoftwareInstall : public ProductControllerStateSoftwareInstall
{
public:
    CustomProductControllerStateSoftwareInstall( ProductControllerHsm& hsm,
                                                 CHsmState* pSuperState,
                                                 Hsm::STATE stateId,
                                                 const std::string& name = "SoftwareInstall" );

    virtual ~CustomProductControllerStateSoftwareInstall() override
    {
    }
    void HandleStateStart() override;
};
} /// namespace ProductApp
