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

class EddieProductControllerHsm : public ProductControllerHsm
{
public:
    EddieProductControllerHsm( NotifyTargetTaskIF* pTask,
                               const std::string& name );
    virtual ~EddieProductControllerHsm()
    {
    }
};
} // namespace ProductApp
