///////////////////////////////////////////////////////////////////////////////
/// @file TransportControlManager.cpp
///
/// @brief Implementation of Transport Control Manager
///
/// @attention
///    BOSE CORPORATION.
///    COPYRIGHT 2017 BOSE CORPORATION ALL RIGHTS RESERVED.
///    This program may not be reproduced, in whole or in part in any
///    form or any means whatsoever without the written permission of:
///        BOSE CORPORATION
///        The Mountain
///        Framingham, MA 01701-9168
///
///////////////////////////////////////////////////////////////////////////////

#include "DPrint.h"
#include "TransportControlManager.h"

static DPrint s_logger( "TransportControlManager" );

namespace ProductApp
{

bool TransportControlManager::Handle( KeyHandlerUtil::ActionType_t intend )
{
    // This function will build and send message either through FrontDoor
    // Return control to ProductController through callback

    //Build Transport Control protobuf based on intend and its state

    //Send GET/PUT/Post/DELETE through Frontdoor

    //Fire the cb so the control goes back to the ProductController
    if( cb() != nullptr )
    {
        ( *cb() )( intend );
    }
    return true;
}

}
