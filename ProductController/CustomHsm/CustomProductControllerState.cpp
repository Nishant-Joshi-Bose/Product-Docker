/////////////////////////////////////////////////////////////////////////////
/// @file   CustomProductControllerState.cpp
/// @brief  Class for executing product specific event handlers based on
///         product specific events. Home for all product specific event
///         handlers that should be performed within product specific/custom
///         states. This class also holds pure virtual methods for common
///         event handlers.
///
/// Copyright 2017 Bose Corporation
////////////////////////////////////////////////////////////////////////////
#include "CustomProductControllerState.h"
#include "ProductControllerHsm.h"
#include "EddieProductController.h"
#include "ProductController.h"

static DPrint s_logger( "CustomProductControllerState" );
namespace ProductApp
{
ProductController* CustomProductControllerState::s_productController = nullptr;

CustomProductControllerState::CustomProductControllerState( ProductControllerHsm& hsm,
                                                            CHsmState* pSuperState,
                                                            Hsm::STATE stateId,
                                                            const std::string& name ) :
    CHsmState( stateId, &hsm, name, pSuperState )
{
    BOSE_INFO( s_logger, __func__ );
}

EddieProductController& CustomProductControllerState::GetCustomProductController()
{
    return *static_cast<EddieProductController*>( s_productController );
}

///////////////////////////////////////////////////////////////////////////////
/// @name  IsProductNeedsSetup()
/// @brief true if the Product needs setup
/// @return bool
///////////////////////////////////////////////////////////////////////////////
bool CustomProductControllerState::IsProductNeedsSetup()
{
    s_logger.LogInfo( "%s:: lang=%d, network=%d", __func__, GetCustomProductController().IsLanguageSet(),
                      GetCustomProductController().GetNetworkServiceUtil().IsNetworkConfigured() );

    return not( GetCustomProductController().GetNetworkServiceUtil().IsNetworkConfigured() );
}

} // namespace ProductApp
