////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file      CustomProductControllerStateLowPowerExit.cpp
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
#include "CustomProductControllerStateLowPowerExit.h"
#include "ProductControllerHsm.h"
#include "ProfessorProductController.h"
#include "ProductControllerState.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
///
///            Constant Definitions
///
////////////////////////////////////////////////////////////////////////////////////////////////////
namespace
{
constexpr uint32_t MANUAL_BLE_REMOTE_PAIRING_TIMEOUT_SECONDS = 120;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                            Start of Product Application Namespace                            ///
////////////////////////////////////////////////////////////////////////////////////////////////////
namespace ProductApp
{

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStateLowPowerExit::CustomProductControllerStateLowPowerExit
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
CustomProductControllerStateLowPowerExit::CustomProductControllerStateLowPowerExit( ProductControllerHsm& hsm,
                                                                CHsmState*            pSuperState,
                                                                Hsm::STATE            stateId,
                                                                const std::string&    name )

    : ProductControllerStateLowPowerExit( hsm, pSuperState, stateId, name )
{
    BOSE_INFO( s_logger, "The %s state is being constructed.", GetName( ).c_str( ) );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief  CustomProductControllerStateLowPowerExit::HandleIntentPlayProductSource
///
/// @param  KeyHandlerUtil::ActionType_t intent
///
/// @return This method returns a true Boolean value indicating that it has handled the intent.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductControllerStateLowPowerExit::HandleIntentPlayProductSource( KeyHandlerUtil::ActionType_t intent )
{
    BOSE_INFO( s_logger, "The %s state in %s is handling the intent %u", GetName( ).c_str( ), __FUNCTION__, intent );
    m_cachedAction = intent;
    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief  CustomProductControllerStateLowPowerExit::HandleIntentPlaySoundTouchSource
///
/// @return This method returns a true Boolean value indicating that it has handled the intent.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductControllerStateLowPowerExit::HandleIntentPlaySoundTouchSource( )
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
