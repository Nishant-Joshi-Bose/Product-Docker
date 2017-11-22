///////////////////////////////////////////////////////////////////////////////
/// @file IntentManager.cpp
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
#include "EddieProductController.h"
#include "IntentManager.h"

static DPrint s_logger( "IntentManager" );

namespace ProductApp
{

bool IntentManager::ValidSourceAvailable()
{
    BOSE_DEBUG( s_logger, "%s", __func__ );
    if( GetProductController().GetNowPlaying().has_source() )
    {
        BOSE_DEBUG( s_logger, "Found nowPlaying" );
        return true;
    }
    return false;
}

SoundTouchInterface::StatusJson IntentManager::CurrentStatusJson()
{
    BOSE_DEBUG( s_logger, "%s", __func__ );
    if( GetProductController().GetNowPlaying().has_source() &&
        GetProductController().GetNowPlaying().has_state() &&
        GetProductController().GetNowPlaying().state().has_status() &&
        GetProductController().GetNowPlaying().has_state() )
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

}
