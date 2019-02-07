////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file      CustomProductControllerStateIdle.cpp
///
/// @brief     This source code file contains functionality to process events that occur during the
///            product idle state.
///
/// @attention Copyright (C) 2018 Bose Corporation All Rights Reserved
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////
///            Included Header Files
////////////////////////////////////////////////////////////////////////////////////////////////////
#include "ProductControllerStates.h"
#include "CustomProductController.h"
#include "CustomProductControllerStateIdle.h"
#include "DPrint.h"
#include "ProductLogger.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Class name declaration for logging
////////////////////////////////////////////////////////////////////////////////////////////////////
namespace
{
constexpr char CLASS_NAME[ ] = "CustomProductControllerStateIdle";
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                            Start of Product Application Namespace                            ///
////////////////////////////////////////////////////////////////////////////////////////////////////
namespace ProductApp
{

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief CustomProductControllerStateIdle::CustomProductControllerStateIdle
/// @param ProductControllerHsm& hsm
/// @param CHsmState*            pSuperState
/// @param Hsm::STATE            stateId
/// @param const std::string&    name
////////////////////////////////////////////////////////////////////////////////////////////////////
CustomProductControllerStateIdle::CustomProductControllerStateIdle( ProductControllerHsm& hsm,
                                                                    CHsmState* pSuperState,
                                                                    Hsm::STATE stateId,
                                                                    const std::string& name )
    : ProductControllerStateIdle( hsm, pSuperState, stateId, name )
{
    BOSE_INFO( s_logger, "%s - is being constructed.", CLASS_NAME );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief CustomProductControllerStateIdle::HandleStateEnter
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductControllerStateIdle::HandleStateEnter( )
{
    GetCustomProductController( ).GetDspHelper()->SetAutoWakeMonitor( GetCustomProductController( ).IsAutoWakeEnabled() );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief CustomProductControllerStateIdle::HandleAutowakeStatus
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductControllerStateIdle::HandleAutowakeStatus( bool active )
{
    GetCustomProductController( ).GetDspHelper()->SetAutoWakeMonitor( active );
    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief CustomProductControllerStateIdle::HandleStateExit
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductControllerStateIdle::HandleStateExit( )
{
    GetCustomProductController( ).GetDspHelper()->SetAutoWakeMonitor( false );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                           End of the Product Application Namespace                           ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
