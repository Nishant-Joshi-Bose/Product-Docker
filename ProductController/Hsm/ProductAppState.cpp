/////////////////////////////////////////////////////////////////////////////
/// @file   ProductAppState.cpp
/// @brief  Provides the base class for all Product Application HSM states.
//      Home for all operations that only states should perform.
///
/// Copyright 2017 Bose Corporation
////////////////////////////////////////////////////////////////////////////////
#include "ProductAppState.h"

namespace ProductApp
{
ProductAppState::ProductAppState( ProductAppHsm& hsm,
                                  CHsmState* pSuperState,
                                  ProductController& productController,
                                  STATE stateId,
                                  const std::string& name ) :
    CHsmState( stateId, &hsm, name, pSuperState ),
    m_hsm( hsm ),
    m_productController( productController )
{
}
} // namespace ProductApp
