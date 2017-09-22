////////////////////////////////////////////////////////////////////////////////
/// @file   EddieProductControllerHsm.h
/// @brief  Definition and implementation of Eddie Product Controller Hsm.
///
/// Copyright 2017 Bose Corporation
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "ProductControllerHsm.h"
#include "ProductControllerStates.h"
#include "ProductControllerState.h"
#include "NotifyTargetTaskIF.h"

namespace ProductApp
{
class ProductControllerState;
class EddieProductController;

class EddieProductControllerHsm : public ProductControllerHsm
{
public:
    EddieProductControllerHsm( NotifyTargetTaskIF* pTask,
                               const std::string& name,
                               EddieProductController& eddieProductController );
    virtual ~EddieProductControllerHsm()
    {
    }

    bool IsProductNeedsSetup();

private:
    EddieProductController& m_eddieProductController;
};
} // namespace ProductApp
