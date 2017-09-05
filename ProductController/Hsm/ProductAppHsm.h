////////////////////////////////////////////////////////////////////////////////
/// @file   ProductAppHsm.h
/// @brief  Definition and implementation of ProductAppHsm
///
/// Copyright 2017 Bose Corporation
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "Hsm.h"
#include "ProductAppStates.h"
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

///////////////////////////////////////////////////////////////////////////////
///// @name GetHsmStateName
///// @brief Function returns HSM state name for a passed state id
///// @return Hsm state name string
///////////////////////////////////////////////////////////////////////////////
    std::string GetHsmStateName( int state_id );
private:

///////////////////////////////////////////////////////////////////////////////
///// @name InitializeHsmStateNameMap
///// @brief Function initializes Product states to names that per WSSAPI
/////        Since ProductAppHsm doesn't exactly have the names that match the
////         specification this translation is required
///// @return void
///////////////////////////////////////////////////////////////////////////////
    void InitializeHsmStateNameMap();
private:
    //maps state id to state string
    std::map <ProductAppStates, std::string> m_hsmState;
    NotifyTargetTaskIF* m_pTask;
    ProductController& m_productController;
};
} // namespace ProductApp
