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

} // namespace ProductApp
