////////////////////////////////////////////////////////////////////////////////
/// @file   ProductAppeHsm.cpp
/// @brief  Definition and implementation of ProductAppHsm
///
/// Copyright 2017 Bose Corporation
////////////////////////////////////////////////////////////////////////////////
#include "ProductAppHsm.h"
#include "DPrint.h"
#include "ProductController.h"

static DPrint s_logger( "ProductAppHsm" );

namespace ProductApp
{
ProductAppHsm::ProductAppHsm( NotifyTargetTaskIF* pTask, const std::string& name, ProductController& productController ) :
    CHsm<ProductAppState>( name , pTask ),
    m_pTask( pTask ),
    m_productController( productController )
{
}

///////////////////////////////////////////////////////////////////////////////
/// @name  NeedsToBeSetup
/// @brief true if the system needs to be setup
/// @return bool
////////////////////////////////////////////////////////////////////////////////
bool ProductAppHsm::NeedsToBeSetup()
{
    s_logger.LogInfo( "%s:: lang=%d, network=%d", __func__, m_productController.IsLanguageSet(), m_productController.IsNetworkSetupDone() );

    return not( m_productController.IsLanguageSet() and
                m_productController.IsNetworkSetupDone() );
}
} // namespace ProductApp
