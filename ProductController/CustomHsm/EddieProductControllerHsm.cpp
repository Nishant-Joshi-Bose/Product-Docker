////////////////////////////////////////////////////////////////////////////////
/// @file   EddieProductControllerHsm.cpp
/// @brief  Definition and implementation of Eddie ProductController Hsm.
///
/// Copyright 2017 Bose Corporation
////////////////////////////////////////////////////////////////////////////////
#include "EddieProductControllerHsm.h"
#include "CustomProductControllerState.h"
#include "EddieProductController.h"
#include "DPrint.h"

static DPrint s_logger( "EddieProductControllerHsm" );

namespace ProductApp
{
EddieProductControllerHsm::EddieProductControllerHsm( NotifyTargetTaskIF* pTask,
                                                      const std::string& name ) :
    ProductControllerHsm( pTask, name )
{
    BOSE_INFO( s_logger, __func__ );
}
} // namespace ProductApp
