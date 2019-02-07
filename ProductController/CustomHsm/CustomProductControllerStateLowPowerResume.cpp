////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file      CustomProductControllerStateLowPowerResume.cpp
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
#include "CustomProductController.h"
#include "CustomProductControllerStateLowPowerResume.h"

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
/// @brief CustomProductControllerStateLowPowerResume::CustomProductControllerStateLowPowerResume
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
CustomProductControllerStateLowPowerResume::CustomProductControllerStateLowPowerResume( ProductControllerHsm& hsm,
        CHsmState*            pSuperState,
        Hsm::STATE            stateId,
        const std::string&    name )

    : ProductControllerStateLowPowerResume( hsm, pSuperState, stateId, name )
{
    BOSE_INFO( s_logger, "The %s state is being constructed.", GetName( ).c_str( ) );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief  CustomProductControllerStateLowPowerResume::HandleIntentPlayProductSource
///
/// @param  KeyHandlerUtil::ActionType_t intent
///
/// @return This method returns a true Boolean value indicating that it has handled the intent.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductControllerStateLowPowerResume::HandleIntentPlayProductSource( KeyHandlerUtil::ActionType_t intent )
{
    BOSE_INFO( s_logger, "The %s state in %s is caching the intent %s", GetName( ).c_str( ), __func__, CommonIntentHandler::GetIntentName( intent ).c_str( ) );
    s_cachedAction = intent;
    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief  CustomProductControllerStateLowPowerResume::HandleIntentPlaySoundTouchSource
///
/// @return This method returns a true Boolean value indicating that it has handled the intent.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductControllerStateLowPowerResume::HandleIntentPlaySoundTouchSource( )
{
    BOSE_INFO( s_logger, "The %s state is caching play soundtouch source", GetName( ).c_str( ) );
    s_cachedAction = static_cast< KeyHandlerUtil::ActionType_t >( Action::ACTION_SOUNDTOUCH );
    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief CustomProductControllerStateLowPowerResume::HandleLPMPowerStatusNetworkStandby
/// @brief Confirm the LPM has risen to SYSTEM_STATE_STANDBY
/// @return true
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductControllerStateLowPowerResume::HandleLPMPowerStatusNetworkStandby()
{
    BOSE_INFO( s_logger, "The %s state is in %s.", GetName( ).c_str( ), __func__ );
    GetCustomProductController().SetEthernetEnabled( true );

    return ProductControllerStateLowPowerResume::HandleLPMPowerStatusNetworkStandby();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                           End of the Product Application Namespace                           ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
