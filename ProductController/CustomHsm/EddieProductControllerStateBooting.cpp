////////////////////////////////////////////////////////////////////////////////
/// @file   EddieProductControllerStateBooting.cpp
/// @brief  The Booting State in the Eddie Product.
///
/// Copyright 2017 Bose Corporation
////////////////////////////////////////////////////////////////////////////////

#include "EddieProductControllerStateBooting.h"
#include "EddieProductControllerHsm.h"
#include "EddieProductController.h"
#include "DPrint.h"
#include "LpmInterface.h"

static DPrint s_logger( "EddieProductControllerStateBooting" );

namespace ProductApp
{
EddieProductControllerStateBooting::EddieProductControllerStateBooting( EddieProductControllerHsm& hsm,
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
        try
        {
            GetCustomProductController().GetLpmInterface().SetSystemState( SYSTEM_STATE_NORMAL );
        }
        catch( std::bad_cast& e )
        {
            BOSE_ERROR( s_logger, "Failed. Bad cast in HandleLpmInterfaceState: " );
        }
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

bool EddieProductControllerStateBooting::HandleIntents( KeyHandlerUtil::ActionType_t intent )
{
    return false;
}

} /// namespace ProductApp
