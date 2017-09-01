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
    //This function maps states to its string representation
    InitializeHsmStateNameMap();
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

///////////////////////////////////////////////////////////////////////////////
/////// @name InitializeHsmStateNameMap
/////// @brief Function initializes Product states to names that per WSSAPI
///////        Since ProductAppHsm doesn't exactly have the names that match the
//////         specification this translation is required
/////// @return void
///////////////////////////////////////////////////////////////////////////////
void ProductAppHsm::InitializeHsmStateNameMap()
{
    m_hsmState[PRODUCT_APP_STATE_BOOTING]       = "BOOTING";
    m_hsmState[PRODUCT_APP_STATE_STDOP]         = "STDOP";
    m_hsmState[PRODUCT_APP_STATE_SETUP]         = "SETUP";
    m_hsmState[PRODUCT_APP_STATE_STANDBY]       = "NETWORK_STANDBY";
    m_hsmState[PRODUCT_APP_STATE_ON]            = "ON";
    m_hsmState[PRODUCT_APP_STATE_SW_UPDATING]   = "UPDATE";
    m_hsmState[PRODUCT_APP_STATE_IDLE]          = "IDLE";
    m_hsmState[PRODUCT_APP_STATE_LOW_POWER]     = "LOWPOWER";
    m_hsmState[PRODUCT_APP_CRITICAL_ERROR]      = "ERROR";

}

///////////////////////////////////////////////////////////////////////////////
/////// @name GetHsmStateName
/////// @brief Function returns HSM state name for a passed state id
/////// @return Hsm state name string
///////////////////////////////////////////////////////////////////////////////
std::string ProductAppHsm::GetHsmStateName( int state_index )
{
    return m_hsmState[static_cast<ProductAppStates> ( state_index )];


}
} // namespace ProductApp
