////////////////////////////////////////////////////////////////////////////////
/// @file   EddieProductControllerStateNetworkStandby.cpp
/// @brief  The Network Standby State in the Eddie Product.
///
/// Copyright 2017 Bose Corporation
////////////////////////////////////////////////////////////////////////////////

#include "EddieProductControllerStateNetworkStandby.h"
#include "ProductControllerHsm.h"
#include "EddieProductController.h"
#include "DPrint.h"

static DPrint s_logger( "EddieProductControllerStateNetworkStandby" );

namespace ProductApp
{
EddieProductControllerStateNetworkStandby::EddieProductControllerStateNetworkStandby( ProductControllerHsm& hsm,
        CHsmState* pSuperState,
        Hsm::STATE stateId,
        const std::string& name ) :
    ProductControllerStateNetworkStandby( hsm, pSuperState, stateId, name )
{
    BOSE_INFO( s_logger, __func__ );
}

void EddieProductControllerStateNetworkStandby::HandleStateEnter()
{
    BOSE_INFO( s_logger, __func__ );
}

void EddieProductControllerStateNetworkStandby::HandleStateStart()
{
    BOSE_INFO( s_logger, __func__ );
}

void EddieProductControllerStateNetworkStandby::HandleStateExit()
{
    BOSE_INFO( s_logger, __func__ );
}

bool EddieProductControllerStateNetworkStandby::HandleIntents( KeyHandlerUtil::ActionType_t intent )
{
    BOSE_DEBUG( s_logger, "%s, %d", __func__, ( uint16_t ) intent );

    if( ( IntentHandler::IsIntentPlayControl( intent ) ) ||
        ( IntentHandler::IsIntentBlueTooth( intent ) )   ||
        ( IntentHandler::IsIntentAlexa( intent ) )        ||
        ( IntentHandler::IsIntentVolumeControl( intent ) ) ||
        ( IntentHandler::IsIntentAuxIn( intent ) )         ||
        ( IntentHandler::IsPresetSelect( intent ) ) )
    {
        GetCustomProductController().GetIntentHandler().Handle( intent );
        return true;
    }
    return false;
}

bool EddieProductControllerStateNetworkStandby::HandleNowSelectionInfo( const SoundTouchInterface::NowSelectionInfo& nowSelectionInfo )
{
    BOSE_INFO( s_logger,  __func__ );
    ChangeState( PRODUCT_CONTROLLER_STATE_ON );
    return true;
}

} // namespace ProductApp
