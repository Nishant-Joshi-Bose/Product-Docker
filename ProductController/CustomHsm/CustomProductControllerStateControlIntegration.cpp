////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file      CustomProductControllerStateControlIntegration.cpp
///
/// @brief     This source code file contains functionality to process events that occur during the
///            product control integration state.
///
/// @attention Copyright (C) 2019 Bose Corporation All Rights Reserved
///
////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////
///
///            Included Header Files
///
////////////////////////////////////////////////////////////////////////////////////////////////////
#include "Utilities.h"
#include "Intents.h"
#include "IntentHandler.h"
#include "CustomProductController.h"
#include "CustomProductControllerStateControlIntegration.h"
#include "ProductControllerHsm.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
///                            Start of Product Application Namespace                            ///
////////////////////////////////////////////////////////////////////////////////////////////////////
namespace ProductApp
{

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStateControlIntegration::
///        CustomProductControllerStateControlIntegration
///
/// @param ProductControllerHsm&       hsm               This argument references the state machine.
///
/// @param CHsmState*                  pSuperState       This argument references the parent state.
///
/// @param Hsm::STATE                  stateId           This enumeration represents the state ID.
///
/// @param const std::string&          name              This argument names the state.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
CustomProductControllerStateControlIntegration::
CustomProductControllerStateControlIntegration( ProductControllerHsm&       hsm,
                                                CHsmState*                  pSuperState,
                                                Hsm::STATE                  stateId,
                                                const std::string&          name )

    : ProductControllerState( hsm, pSuperState, stateId, name )
{
    BOSE_INFO( s_logger, "The %s state is being constructed.", GetName( ).c_str( ) );
}

void CustomProductControllerStateControlIntegration::HandleStateStart()
{
    GetProductController( ).GetInactivityTimers( ).StartTimer( InactivityTimerType::NO_AUDIO_TIMER );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief  CustomProductControllerStateControlIntegration::HandleIntentPlayProductSource
///
/// @return This method returns a true Boolean value indicating that it handles the intent
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductControllerStateControlIntegration::HandleIntentPlayProductSource( KeyHandlerUtil::ActionType_t intent )
{
    BOSE_INFO( s_logger, "The %s state in %s", GetName( ).c_str( ), __func__ );
    if( intent == static_cast< KeyHandlerUtil::ActionType_t >( Action::ACTION_TV_CEC ) )
    {
        BOSE_INFO( s_logger, "The %s state in %s: ignore CEC TV intent.", GetName( ).c_str( ), __func__ );
        return true;
    }
    return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief CustomProductControllerStateControlIntegration::HandleInactivityTimer
/// @param timerType
/// @return This method returns a true value, indicating that it has handled time out event.
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductControllerStateControlIntegration::HandleInactivityTimer( InactivityTimerType timerType )
{
    if( timerType != InactivityTimerType::NO_AUDIO_TIMER )
    {
        BOSE_INFO( s_logger, "The %s state is in %s: timer type %d is unexpected", GetName( ).c_str( ), __func__, timerType );
        return false;
    }

    BOSE_INFO( s_logger, "The %s state is in %s: timer %d has expired", GetName( ).c_str( ), __func__, timerType );
    ChangeState( PRODUCT_CONTROLLER_STATE_PLAYING_SELECTED_STOPPING_STREAMS );
    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                             End of Product Application Namespace                             ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                        End of File                                           ///
////////////////////////////////////////////////////////////////////////////////////////////////////
