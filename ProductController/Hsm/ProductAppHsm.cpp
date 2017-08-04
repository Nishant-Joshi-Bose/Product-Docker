////////////////////////////////////////////////////////////////////////////////
/// @file   ProductAppeHsm.cpp
/// @brief  Definition and implementation of ProductAppHsm
///
/// Copyright 2017 Bose Corporation
////////////////////////////////////////////////////////////////////////////////
#include "ProductAppHsm.h"
#include "DPrint.h"

static DPrint s_logger( "ProductAppHsm" );

namespace ProductApp
{
ProductAppHsm::ProductAppHsm( NotifyTargetTaskIF* pTask, const std::string& name, ProductController& productController ) :
    CHsm<ProductAppState>( name , pTask ),
    m_pTask( pTask ),
    m_productController( productController )
{
}
} // namespace ProductApp
