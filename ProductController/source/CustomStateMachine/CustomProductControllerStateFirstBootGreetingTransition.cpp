////////////////////////////////////////////////////////////////////////////////
/// @file   CustomProductControllerStateFirstBootGreetingTransition.cpp
/// @brief  The State Prior to first boot greeting state. We need to wait for
///         the LPM in ON state, and then to be sure that wired accessories are either
///         discovered, or be comfortable that they aren't there. See PGC-1926.
///
/// Copyright 2018 Bose Corporation
////////////////////////////////////////////////////////////////////////////////

#include "CustomProductControllerStateFirstBootGreetingTransition.h"
#include "ProductControllerStates.h"
#include "ProfessorProductController.h"
#include "APTimer.h"

namespace ProductApp
{
CustomProductControllerStateFirstBootGreetingTransition::CustomProductControllerStateFirstBootGreetingTransition( ProductControllerHsm& hsm,
        CHsmState* pSuperState,
        Hsm::STATE stateId,
        const std::string& name ) :
    ProductControllerStateFirstBootGreetingTransition( hsm, pSuperState, stateId, name )
{
    BOSE_INFO( s_logger, __func__ );
}

void CustomProductControllerStateFirstBootGreetingTransition::HandleStateEnter()
{
    BOSE_INFO( s_logger, __func__ );
    // SYSTEM_STATE_ON
    if( GetCustomProductController( ).GetLpmSystemState() == SYSTEM_STATE_ON )
    {
        ProductMessage productMessage;
        productMessage.mutable_lpmstatus( )->set_systemstate( SYSTEM_STATE_ON );
        IL::BreakThread( std::bind( GetProductController( ).GetMessageHandler( ),
                                    productMessage ),
                         GetProductController( ).GetTask( ) );
    }
}

bool CustomProductControllerStateFirstBootGreetingTransition::HandleLPMPowerStatusFullPowerOn()
{
    BOSE_INFO( s_logger, __func__ );
    if( GetCustomProductController( ).AreAccessoriesKnown( ) )
    {
        // It is safe to play the chime
        ChangeState( PRODUCT_CONTROLLER_STATE_FIRST_BOOT_GREETING );
    }
    return true;
}

bool CustomProductControllerStateFirstBootGreetingTransition::HandleAccessoriesAreKnown()
{
    BOSE_INFO( s_logger, __func__ );
    ChangeState( PRODUCT_CONTROLLER_STATE_FIRST_BOOT_GREETING );
    return true;
}

} /// namespace ProductApp
