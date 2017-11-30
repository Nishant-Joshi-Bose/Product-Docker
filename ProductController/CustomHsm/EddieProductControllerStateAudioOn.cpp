////////////////////////////////////////////////////////////////////////////////
/// @file   EddieProductControllerStateAudioOn.cpp
/// @brief  The Audio On state in the Eddie Product.
///
/// Copyright 2017 Bose Corporation
////////////////////////////////////////////////////////////////////////////////

#include "EddieProductControllerStateAudioOn.h"
#include "ProductControllerHsm.h"
#include "EddieProductController.h"
#include "DPrint.h"

static DPrint s_logger( "EddieProductControllerStateAudioOn" );

namespace ProductApp
{
EddieProductControllerStateAudioOn::EddieProductControllerStateAudioOn( ProductControllerHsm& hsm,
                                                                        CHsmState* pSuperState,
                                                                        Hsm::STATE stateId,
                                                                        const std::string& name ) :
    ProductControllerStateOn( hsm, pSuperState, stateId, name )
{
    BOSE_INFO( s_logger, __func__ );
}

void EddieProductControllerStateAudioOn::HandleStateEnter()
{
    BOSE_INFO( s_logger, __func__ );
}

void EddieProductControllerStateAudioOn::HandleStateStart()
{
    BOSE_INFO( s_logger, __func__ );
}

void EddieProductControllerStateAudioOn::HandleStateExit()
{
    BOSE_INFO( s_logger, __func__ );
}

bool EddieProductControllerStateAudioOn::HandleIntents( KeyHandlerUtil::ActionType_t intent )
{
    BOSE_DEBUG( s_logger, "%s, %d", __func__, ( uint16_t ) intent );

    if( ( IntentHandler::IsIntentPlayControl( intent ) ) ||
        ( IntentHandler::IsIntentBlueTooth( intent ) )   ||
        ( IntentHandler::IsIntentVoice( intent ) )        ||
        ( IntentHandler::IsIntentVolumeControl( intent ) ) ||
        ( IntentHandler::IsIntentNetworkStandby( intent ) ) ||
        ( IntentHandler::IsIntentAuxIn( intent ) ) )
    {
        GetCustomProductController().GetIntentHandler().Handle( intent );
        return true;
    }
    return false;
}

bool EddieProductControllerStateAudioOn::HandleNetworkStandbyIntent( )
{
    BOSE_DEBUG( s_logger, "%s", __func__ );
    ChangeState( CUSTOM_PRODUCT_CONTROLLER_STATE_NETWORK_STANDBY );
    return true;
}

} /// namespace ProductApp
