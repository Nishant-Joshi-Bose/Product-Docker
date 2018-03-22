////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file      CustomProductControllerStateLowPowerStandby.cpp
///
/// @brief     This source code file contains functionality to process events that occur during the
///            product on state.
///
/// @attention Copyright (C) 2017 Bose Corporation All Rights Reserved
///
///            Bose Corporation
///            The Mountain Road,
///            Framingham, MA 01701-9168
///            U.S.A.
///
///            This program may not be reproduced, in whole or in part, in any form by any means
///            whatsoever without the written permission of Bose Corporation.
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
#include "CustomProductControllerStateLowPowerStandby.h"
#include "ProductControllerHsm.h"
#include "ProfessorProductController.h"
#include "ProductControllerState.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
///
///            Constant Definitions
///
////////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////////
///                            Start of Product Application Namespace                            ///
////////////////////////////////////////////////////////////////////////////////////////////////////
namespace ProductApp
{

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStateLowPowerStandby::CustomProductControllerStateLowPowerStandby
///
/// @param ProductControllerHsm& hsm
///
/// @param CHsmState*            pSuperState
///
/// @param Hsm::STATE            stateId
///
/// @param const std::string&    name
///
////////////////////////////////////////////////////////////////////////////////////////////////////
CustomProductControllerStateLowPowerStandby::CustomProductControllerStateLowPowerStandby( ProductControllerHsm& hsm,
                                                                CHsmState*            pSuperState,
                                                                Hsm::STATE            stateId,
                                                                const std::string&    name )

    : ProductControllerStateLowPowerStandby( hsm, pSuperState, stateId, name )
{
    BOSE_INFO( s_logger, "The %s state is being constructed.", GetName( ).c_str( ) );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief  CustomProductControllerStateLowPowerStandby::HandleIntentPlayProductSource
///
/// @param  KeyHandlerUtil::ActionType_t intent
///
/// @return This method returns a true Boolean value indicating that it has handled the intent.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductControllerStateLowPowerStandby::HandleIntentPlayProductSource( KeyHandlerUtil::ActionType_t intent )
{
    BOSE_INFO( s_logger, "The %s state in %s is handling the intent %u", GetName( ).c_str( ), __FUNCTION__, intent );
    m_cachedAction = intent;
    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief  CustomProductControllerStateLowPowerStandby::HandleIntentPlaySoundTouchSource
///
/// @return This method returns a true Boolean value indicating that it has handled the intent.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductControllerStateLowPowerStandby::HandleIntentPlaySoundTouchSource( )
{
    BOSE_INFO( s_logger, "The %s state is caching play soundtouch source", GetName( ).c_str( ) );
    m_cachedAction = static_cast< KeyHandlerUtil::ActionType_t >( Action::ACTION_SOUNDTOUCH );
    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                           End of the Product Application Namespace                           ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
