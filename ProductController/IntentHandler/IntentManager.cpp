///////////////////////////////////////////////////////////////////////////////
/// @file IntentManager.cpp
///
/// @brief Implementation of  base class IntentManager
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
#include "ProductController.h"
#include "IntentManager.h"

static DPrint s_logger( "IntentManager" );

namespace ProductApp
{

bool IntentManager::ValidSourceAvailable() const
{
    BOSE_DEBUG( s_logger, "%s", __func__ );
    if( GetProductController().GetNowPlaying().has_source() )
    {
        BOSE_DEBUG( s_logger, "Found nowPlaying" );
        return true;
    }
    return false;
}

SoundTouchInterface::StatusJson IntentManager::CurrentNowPlayingStatusJson() const
{
    BOSE_DEBUG( s_logger, "%s", __func__ );
    if( GetProductController().GetNowPlaying().has_source() &&
        GetProductController().GetNowPlaying().has_state() &&
        GetProductController().GetNowPlaying().state().has_status() )
    {
        BOSE_DEBUG( s_logger, "Found status = %d",
                    GetProductController().GetNowPlaying().state().status() );
        return ( GetProductController().GetNowPlaying().state().status() );
    }
    else
    {
        BOSE_DEBUG( s_logger, "No Status in GetNowPlaying()" );
    }
    return ( SoundTouchInterface::StatusJson::error );
}

void IntentManager::FrontDoorClientErrorCb( const FRONT_DOOR_CLIENT_ERRORS errorCode )
{
    // Nothing to do for now, printing this if anyone cares.
    BOSE_ERROR( s_logger, "%s:error code- %d", __func__, errorCode );
    return;
}


}
