////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file      CustomProductControllerStateBooted.cpp
///
/// @brief     This source code file contains functionality to process events that occur during the
///            product booting state.
///
/// @attention Copyright (C) 2018 Bose Corporation All Rights Reserved
////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////
///            Included Header Files
////////////////////////////////////////////////////////////////////////////////////////////////////
#include "CustomProductControllerStateBooted.h"
#include "ProductControllerStates.h"
#include "ProductController.h"
#include "Utilities.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
///                            Start of Product Application Namespace                            ///
////////////////////////////////////////////////////////////////////////////////////////////////////
namespace ProductApp
{

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @name  CustomProductControllerStateBooted::CustomProductControllerStateBooted
/// @param ProductControllerHsm& hsm
/// @param CHsmState*            pSuperState
/// @param Hsm::STATE            stateId
/// @param const std::string&    name
////////////////////////////////////////////////////////////////////////////////////////////////////
CustomProductControllerStateBooted::CustomProductControllerStateBooted( ProductControllerHsm& hsm,
                                                                        CHsmState* pSuperState,
                                                                        Hsm::STATE stateId,
                                                                        const std::string& name ) :
    ProductControllerStateBooted( hsm, pSuperState, stateId, name )
{
    BOSE_INFO( s_logger, "The %s state is being constructed.", GetName( ).c_str( ) );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @name  CustomProductControllerStateBooted::HandleStateExit
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductControllerStateBooted::HandleStateExit( )
{
    ProductControllerStateBooted::HandleStateExit();
    BOSE_INFO( s_logger, "The %s is exiting.", GetName( ).c_str( ) );
    GetCustomProductController( ).SetBootCompleteTime( );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                           End of the Product Application Namespace                           ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
