////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file      CustomProductControllerStateBooting.cpp
///
/// @brief     This source code file contains functionality to process events that occur during the
///            product booting state.
///
/// @attention Copyright (C) 2018 Bose Corporation All Rights Reserved
////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////
///            Included Header Files
////////////////////////////////////////////////////////////////////////////////////////////////////
#include "CustomProductControllerStatePlayingDeselected.h"
#include "ProductControllerStates.h"
#include "CustomProductController.h"
#include "Utilities.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
///                            Start of Product Application Namespace                            ///
////////////////////////////////////////////////////////////////////////////////////////////////////
namespace ProductApp
{

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @name  CustomProductControllerStatePlayingDeselected::CustomProductControllerStatePlayingDeselected
/// @param ProductControllerHsm& hsm
/// @param CHsmState*            pSuperState
/// @param Hsm::STATE            stateId
/// @param const std::string&    name
////////////////////////////////////////////////////////////////////////////////////////////////////
CustomProductControllerStatePlayingDeselected::CustomProductControllerStatePlayingDeselected( ProductControllerHsm& hsm,
        CHsmState* pSuperState,
        Hsm::STATE stateId,
        const std::string& name ) :
    ProductControllerStatePlayingDeselected( hsm, pSuperState, stateId, name )
{
    BOSE_INFO( s_logger, "The %s state is being constructed.", GetName( ).c_str( ) );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStatePlayingDeselected::HandleIntentVolumeControl
///
/// @param  KeyHandlerUtil::ActionType_t intent
///
/// @return This method returns a true Boolean value indicating that it is handling the volume intent
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductControllerStatePlayingDeselected::HandleIntentVolumeControl( KeyHandlerUtil::ActionType_t intent )
{
    BOSE_INFO( s_logger, "The %s state in %s is ignoring the intent %u.", GetName( ).c_str( ), __func__, intent );

    ///
    /// The intent is ignored in this custom state.
    ///
    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief  CustomProductControllerStatePlayingDeselected::HandleIntentMuteControl
///
/// @param  KeyHandlerUtil::ActionType_t intent
///
/// @return This method returns a true Boolean value indicating that it has handled the muting
///         intent. It is ignored in the setup state.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductControllerStatePlayingDeselected::HandleIntentMuteControl( KeyHandlerUtil::ActionType_t intent )
{
    BOSE_INFO( s_logger, "The %s state in %s is ignoring the intent %u.", GetName( ).c_str( ), __func__, intent );

    ///
    /// The intent is ignored in this custom state.
    ///
    return true;
}


bool CustomProductControllerStatePlayingDeselected::HandleIntentPowerOn( )
{
    GetCustomProductController( ).AttemptToStartPlayback( );
    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                           End of the Product Application Namespace                           ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
