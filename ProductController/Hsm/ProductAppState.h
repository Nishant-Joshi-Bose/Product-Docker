/// @file   ProductAppState.h
/// @brief  Provides the base class for all ProductApplication HSM states.
///     Home for all operations that only states should perform.
///
/// Copyright 2017 Bose Corporation
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "Hsm.h"
#include "HsmState.h"
#include "ProductAppHsm.h"

namespace ProductApp
{
class ProductAppHsm;
class ProductController;

class ProductAppState : public CHsmState
{
public:
    ProductAppState( ProductAppHsm& hsm, CHsmState* pSuperState, ProductController& productController,
                     STATE stateId = 0, const std::string& name = "Top" );

    virtual ~ProductAppState()
    {
    }

    virtual void HandleStateEnter() override
    {
    }

    virtual void HandleStateStart() override
    {
    }

    virtual void HandleStateExit() override
    {
    }

    virtual bool HandleModulesReady( )
    {
        return false;
    }

protected:
    ProductAppHsm& GetHsm()
    {
        return m_hsm;
    }
    ProductController& GetProductController()
    {
        return m_productController;
    }

private:
    ProductAppHsm& m_hsm;
    ProductController& m_productController;
};
} // namespace ProductApp
