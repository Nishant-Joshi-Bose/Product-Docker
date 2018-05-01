////////////////////////////////////////////////////////////////////////////////
/// @file   CustomProductControllerStateLowPowerResume.cpp
/// @brief  The Low Power Standby State in the Eddie Product.
///
/// Copyright 2017 Bose Corporation
////////////////////////////////////////////////////////////////////////////////

#include "CustomProductControllerStateLowPowerResume.h"
#include "ProductControllerHsm.h"
#include "EddieProductController.h"
#include "DPrint.h"

static DPrint s_logger( "CustomProductControllerStateLowPowerResume" );

namespace ProductApp
{
CustomProductControllerStateLowPowerResume::CustomProductControllerStateLowPowerResume( ProductControllerHsm& hsm,
        CHsmState* pSuperState,
        Hsm::STATE stateId,
        const std::string& name ) :
    ProductControllerStateLowPowerResume( hsm, pSuperState, stateId, name )
{
    BOSE_INFO( s_logger, __func__ );
}

bool CustomProductControllerStateLowPowerResume::HandleIntentAuxIn( KeyHandlerUtil::ActionType_t intent )
{
    BOSE_INFO( s_logger, "The %s state in %s is handling the intent %u", GetName( ).c_str( ), __FUNCTION__, intent );
    m_cachedAction = intent;
    return true;
}

} /// namespace ProductApp