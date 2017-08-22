////////////////////////////////////////////////////////////////////////////////
/// @file   ProductAppHsm.h
/// @brief  Definition and implementation of ProductAppHsm
///
/// Copyright 2017 Bose Corporation
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "Hsm.h"
#include "ProductAppState.h"
#include "NotifyTargetTaskIF.h"

namespace ProductApp
{
class ProductAppState;
class ProductController;

class ProductAppHsm : public CHsm<ProductAppState>
{
public:
    ProductAppHsm( NotifyTargetTaskIF* pTask, const std::string& name, ProductController& productController );
    virtual ~ProductAppHsm()
    {
    }

    NotifyTargetTaskIF* GetTask( void )
    {
        return m_pTask;
    }
    bool NeedsToBeSetup();

private:
    NotifyTargetTaskIF* m_pTask;
    ProductController& m_productController;
};
} // namespace ProductApp
