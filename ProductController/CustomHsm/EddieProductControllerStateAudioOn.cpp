////////////////////////////////////////////////////////////////////////////////
/// @file   EddieProductControllerStateAudioOn.cpp
/// @brief  The Audio On state in the Eddie Product.
///
/// Copyright 2017 Bose Corporation
////////////////////////////////////////////////////////////////////////////////

#include "EddieProductControllerStateAudioOn.h"
#include "EddieProductControllerHsm.h"
#include "EddieProductController.h"
#include "DPrint.h"

static DPrint s_logger( "EddieProductControllerStateAudioOn" );

namespace ProductApp
{
EddieProductControllerStateAudioOn::EddieProductControllerStateAudioOn( EddieProductControllerHsm& hsm,
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
        ( IntentHandler::IsIntentAlexa( intent ) )        ||
        ( IntentHandler::IsIntentVolumeControl( intent ) ) ||
        ( IntentHandler::IsIntentAuxIn( intent ) ) )
    {
        GetCustomProductController().GetIntentHandler().Handle( intent );
        return true;
    }
    return false;
}

} /// namespace ProductApp
