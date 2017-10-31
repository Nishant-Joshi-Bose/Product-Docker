////////////////////////////////////////////////////////////////////////////////
/// @file   EddieProductControllerHsm.cpp
/// @brief  Definition and implementation of Eddie ProductController Hsm.
///
/// Copyright 2017 Bose Corporation
////////////////////////////////////////////////////////////////////////////////
#include "EddieProductControllerHsm.h"
#include "EddieProductController.h"
#include "DPrint.h"

static DPrint s_logger( "EddieProductControllerHsm" );

namespace ProductApp
{
EddieProductControllerHsm::EddieProductControllerHsm( NotifyTargetTaskIF* pTask,
                                                      const std::string& name,
                                                      EddieProductController& eddieProductController ) :
    ProductControllerHsm( pTask, name, eddieProductController ),
    m_eddieProductController( eddieProductController )
{
    BOSE_INFO( s_logger, __func__ );
}

///////////////////////////////////////////////////////////////////////////////
/// @name  IsProductNeedsSetup()
/// @brief true if the Product needs setup
/// @return bool
///////////////////////////////////////////////////////////////////////////////
bool EddieProductControllerHsm::IsProductNeedsSetup()
{
    s_logger.LogInfo( "%s:: lang=%d, network=%d", __func__, m_eddieProductController.IsLanguageSet(),
                      m_eddieProductController.IsNetworkConfigured() );

    return not( m_eddieProductController.IsLanguageSet() and
                m_eddieProductController.IsNetworkConfigured() );
}

} // namespace ProductApp
