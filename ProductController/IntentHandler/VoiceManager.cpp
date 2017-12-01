///////////////////////////////////////////////////////////////////////////////
/// @file VoiceManager.cpp
///
/// @brief Implementation of Voice Manager for actions from Voice
//         intends in the product Controller
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
#include "VoiceManager.h"
#include "ProductController.h"

static DPrint s_logger( "VoiceManager" );

namespace ProductApp
{
///////////////////////////////////////////////////////////////////////////////
/// @name  Handle
/// @brief Function to build and send FrontDoor message to execute the
//         to Voice intends coming out of the product controller.
//         The callBack function is called to give control back to the state
//         machine if HSM has registered a call back.
/// @return true: Successful
//          false: Error
////////////////////////////////////////////////////////////////////////////////

bool VoiceManager::Handle( KeyHandlerUtil::ActionType_t& intent )
{
    BOSE_INFO( s_logger, "%s: ", __func__ );

    switch( intent )
    {
    case( uint16_t ) Action::VOICE_CAROUSEL:
    {
        BOSE_INFO( s_logger, "Send voice action" );

    }
    break;

    default:
    {
        BOSE_ERROR( s_logger, "Invalid intent %d received in %s",
                    ( uint16_t ) intent, __func__ );
    }
    break;
    }

    //Fire the cb so the control goes back to the ProductController
    if( GetCallbackObject() != nullptr )
    {
        ( *GetCallbackObject() )( intent );
    }

    return true;
}

}
