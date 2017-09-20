/////////////////////////////////////////////////////////////////////////////
/// @file   CustomProductControllerState.cpp
/// @brief  Class for executing product specific event handlers based on
///         product specific events. Home for all product specific event
///         handlers that should be performed within product specific/custom
///         states. This class also holds pure virtual methods for common
///         event handlers.
///
/// Copyright 2017 Bose Corporation
////////////////////////////////////////////////////////////////////////////
#include "CustomProductControllerState.h"
#include "ProductControllerHsm.h"

namespace ProductApp
{
CustomProductControllerState::CustomProductControllerState( ProductControllerHsm& hsm,
                                                            CHsmState* pSuperState,
                                                            STATE stateId,
                                                            const std::string& name ) :
    CHsmState( stateId, &hsm, name, pSuperState )
{
}
} // namespace ProductApp
