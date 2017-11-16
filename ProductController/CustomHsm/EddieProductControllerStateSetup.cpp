////////////////////////////////////////////////////////////////////////////////
/// @file   EddieProductControllerStateSetup.cpp
/// @brief  The Setup State in the Eddie Product.
///
/// Copyright 2017 Bose Corporation
////////////////////////////////////////////////////////////////////////////////

#include "EddieProductControllerStateSetup.h"
#include "ProductControllerHsm.h"
#include "EddieProductController.h"
#include "DPrint.h"

static DPrint s_logger( "EddieProductControllerStateSetup" );

namespace ProductApp
{
EddieProductControllerStateSetup::EddieProductControllerStateSetup( ProductControllerHsm& hsm,
                                                                    CHsmState* pSuperState,
                                                                    Hsm::STATE stateId,
                                                                    const std::string& name ) :
    ProductControllerStateSetup( hsm, pSuperState, stateId, name )
{
}

void EddieProductControllerStateSetup::HandleStateEnter()
{
    BOSE_INFO( s_logger, __func__ );
}

void EddieProductControllerStateSetup::HandleStateStart()
{
    BOSE_INFO( s_logger, __func__ );
    GetProductController().GetNetworkServiceUtil().EnableWiFiSetupMode();
    GetProductController().GetProductFrontDoorUtility().EnableNetworkAccessPoint();
    GetProductController().GetProductFrontDoorUtility().EnableBTBLEAdvertising();
}

void EddieProductControllerStateSetup::HandleStateExit()
{
    BOSE_INFO( s_logger, __func__ );
    GetProductController().GetNetworkServiceUtil().EnableWiFiAutoSwitchingMode();
    GetProductController().GetProductFrontDoorUtility().DisableNetworkAccessPoint();
    GetProductController().GetProductFrontDoorUtility().DisableBTBLEAdvertising();
}


bool EddieProductControllerStateSetup::HandleIntents( KeyHandlerUtil::ActionType_t intent )
{
    BOSE_DEBUG( s_logger, "%s, %d", __func__, ( uint16_t ) intent );

    if( ( IntentHandler::IsIntentAuxIn( intent ) ) )
    {
        GetCustomProductController().GetIntentHandler().Handle( intent );
        return true;
    }

    return false;
}

bool EddieProductControllerStateSetup::HandleNetworkConfigurationStatus( )
{
    BOSE_INFO( s_logger, __func__ );
    if( GetCustomProductController().IsNetworkConfigured() )
        ChangeState( CUSTOM_PRODUCT_CONTROLLER_STATE_NETWORK_STANDBY );

    return true;
}

bool EddieProductControllerStateSetup::HandleBtLeModuleReady()
{
    GetProductController().GetProductFrontDoorUtility().EnableBTBLEAdvertising();

    return true;
}

} // namespace ProductApp
