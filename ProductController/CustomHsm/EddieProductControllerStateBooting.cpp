////////////////////////////////////////////////////////////////////////////////
/// @file   EddieProductControllerStateBooting.cpp
/// @brief  The Booting State in the Eddie Product.
///
/// Copyright 2017 Bose Corporation
////////////////////////////////////////////////////////////////////////////////

#include "EddieProductControllerStateBooting.h"
#include "ProductControllerHsm.h"
#include "EddieProductController.h"
#include "DPrint.h"
#include "LpmInterface.h"

static DPrint s_logger( "EddieProductControllerStateBooting" );

namespace ProductApp
{
EddieProductControllerStateBooting::EddieProductControllerStateBooting( ProductControllerHsm& hsm,
                                                                        CHsmState* pSuperState,
                                                                        Hsm::STATE stateId,
                                                                        const std::string& name ) :
    ProductControllerStateBooting( hsm, pSuperState, stateId, name )
{
    BOSE_INFO( s_logger, __func__ );
}

void EddieProductControllerStateBooting::HandleStateEnter()
{
    BOSE_INFO( s_logger, __func__ );
    ProductControllerStateBooting::HandleStateEnter();
}

void EddieProductControllerStateBooting::HandleStateStart()
{
    BOSE_INFO( s_logger, __func__ );
    ProductControllerStateBooting::HandleStateStart();
}

void EddieProductControllerStateBooting::HandleStateExit()
{
    BOSE_INFO( s_logger, __func__ );
    ProductControllerStateBooting::HandleStateExit();

    // TODO: For now, turn the amplifier on unconditionally.
    // Eventually we'll turn it on and off as needed.
    GetCustomProductController().GetLpmInterface().SetAmp( /*powered=*/ true, /*muted=*/ false );
}

bool EddieProductControllerStateBooting::HandleModulesReady()
{
    BOSE_INFO( s_logger, __func__ );
    GoToNextState();
    return true;
}

bool EddieProductControllerStateBooting::HandleLpmState( bool isActive )
{
    if( isActive )
    {
        BOSE_LOG( INFO, "LPM hardware is Ready. Go to next state" );
        GoToNextState();
        return true;
    }
    BOSE_ERROR( s_logger, "Failed to handle Lpm State. Lpm is not ready" );
    return false;
}

bool EddieProductControllerStateBooting::HandleLpmInterfaceState( bool isConnected )
{
    if( isConnected )
    {
        BOSE_LOG( INFO, "LPM hardware is Down. Set LPM System State to NORMAL" );
        GetCustomProductController().GetLpmInterface().SetSystemState( SYSTEM_STATE_NORMAL );
        return true;
    }
    BOSE_ERROR( s_logger, "Failed. LPM interface is down" );
    return false;
}

void EddieProductControllerStateBooting::GoToNextState()
{
    BOSE_INFO( s_logger, __func__ );
    if( GetCustomProductController().IsAllModuleReady() )
    {
        if( IsProductNeedsSetup() )
        {
            ChangeState( CUSTOM_PRODUCT_CONTROLLER_STATE_SETUP );
        }
        else
        {
            ChangeState( CUSTOM_PRODUCT_CONTROLLER_STATE_NETWORK_STANDBY );
        }
    }
}

} /// namespace ProductApp
