////////////////////////////////////////////////////////////////////////////////
/// @file   EddieProductControllerStateSetup.cpp
/// @brief  The Setup State in the Eddie Product.
///
/// Copyright 2017 Bose Corporation
////////////////////////////////////////////////////////////////////////////////

#include "EddieProductControllerStateSetup.h"
#include "EddieProductControllerHsm.h"
#include "EddieProductController.h"
#include "DPrint.h"

static DPrint s_logger( "EddieProductControllerStateSetup" );

namespace ProductApp
{
EddieProductControllerStateSetup::EddieProductControllerStateSetup( EddieProductControllerHsm& hsm,
                                                                    CHsmState* pSuperState,
                                                                    EddieProductController& eddieProductController,
                                                                    Hsm::STATE stateId,
                                                                    const std::string& name ) :
    ProductControllerStateSetup( hsm, pSuperState, eddieProductController, stateId, name )
{
}

void EddieProductControllerStateSetup::HandleStateEnter()
{
    BOSE_INFO( s_logger, __func__ );
}

void EddieProductControllerStateSetup::HandleStateStart()
{
    BOSE_INFO( s_logger, __func__ );
    EnableWiFiSetupMode();
    GetProductController().GetProductFrontDoorUtility().EnableNetworkAccessPoint();
    GetProductController().GetProductFrontDoorUtility().EnableBTBLEAdvertising();
}

void EddieProductControllerStateSetup::HandleStateExit()
{
    BOSE_INFO( s_logger, __func__ );
    EnableWiFiAutoSwitchingMode();
    GetProductController().GetProductFrontDoorUtility().DisableNetworkAccessPoint();
    GetProductController().GetProductFrontDoorUtility().DisableBTBLEAdvertising();
}


bool EddieProductControllerStateSetup::HandleIntents( KeyHandlerUtil::ActionType_t result )
{
    return false;
}

bool EddieProductControllerStateSetup::HandleNetworkModuleStatus( const NetManager::Protobuf::NetworkStatus& networkStatus, int profileSize )
{
    BOSE_INFO( s_logger, "%s, profileSize =%d", __func__, profileSize );
    if( profileSize || IsNetworkConfigured( networkStatus ) )
        ChangeState( CUSTOM_PRODUCT_CONTROLLER_STATE_NETWORK_STANDBY );

    return true;
}

} // namespace ProductApp
