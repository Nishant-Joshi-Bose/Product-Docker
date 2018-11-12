////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file      CustomProductControllerStatePlayableTransitionNetworkStandby.cpp
///
/// @brief     This source code file contains functionality to process events that occur during the
///            product playable transition to network standby state.
///
/// @attention Copyright (C) 2018 Bose Corporation All Rights Reserved
////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////
///
///            Included Header Files
///
////////////////////////////////////////////////////////////////////////////////////////////////////
#include "ProductControllerStates.h"
#include "CustomProductControllerStatePlayableTransitionNetworkStandby.h"
#include "CustomProductController.h"
#include "ProductLogger.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
///                            Start of Product Application Namespace                            ///
////////////////////////////////////////////////////////////////////////////////////////////////////
namespace ProductApp
{

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief CustomProductControllerStatePlayableTransitionNetworkStandby::
///        CustomProductControllerStatePlayableTransitionNetworkStandby
/// @param ProductControllerHsm& hsm
/// @param CHsmState*            pSuperState
/// @param Hsm::STATE            stateId
/// @param const std::string&    name
////////////////////////////////////////////////////////////////////////////////////////////////////
CustomProductControllerStatePlayableTransitionNetworkStandby::
CustomProductControllerStatePlayableTransitionNetworkStandby( ProductControllerHsm& hsm,
                                                              CHsmState* pSuperState,
                                                              Hsm::STATE stateId,
                                                              const std::string& name )

    : ProductControllerStatePlayableTransitionNetworkStandby( hsm, pSuperState, stateId, name )
{
    BOSE_INFO( s_logger, "The %s state is being constructed.", GetName( ).c_str( ) );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief CustomProductControllerStatePlayableTransitionNetworkStandby::HandleLPMPowerStatusNetworkStandby
/// @return This method returns a true value, indicating that it has handled the device transition
///         to a network standby power system state.
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductControllerStatePlayableTransitionNetworkStandby::HandleLPMPowerStatusNetworkStandby( )
{
    BOSE_INFO( s_logger, "The %s state is in %s.", GetName( ).c_str( ), __func__ );

    if( !GetCustomProductController( ).GetHaltInPlayableTransitionNetworkStandby( ) )
    {
        ChangeState( PRODUCT_CONTROLLER_STATE_NETWORK_STANDBY );
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                           End of the Product Application Namespace                           ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
