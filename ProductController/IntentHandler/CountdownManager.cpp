///////////////////////////////////////////////////////////////////////////////
/// @file CountdownManager.cpp
///
/// @brief Implementation of Bluetooth Manager for actions from Bluetooth
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
#include "CountdownManager.h"
#include "ProductController.h"
#include "Intents.h"

static DPrint s_logger( "CountdownManager" );

namespace ProductApp
{
///////////////////////////////////////////////////////////////////////////////
/// @name  Handle
/// @brief Function to build and send FrontDoor message to execute the
//         to Bluetooth intends coming out of the product controller.
//         The callBack function is called to give control back to the state
//         machine if HSM has registered a call back.
/// @return true: Successful
//          false: Error
////////////////////////////////////////////////////////////////////////////////

bool CountdownManager::Handle( KeyHandlerUtil::ActionType_t& intent )
{
    BOSE_INFO( s_logger, "%s - (intent=%d)(m_counter=%d)", __func__, intent, m_counter );

    switch( intent )
    {
    case( uint16_t ) Action::FACTORY_RESET_COUNTDOWN:
    {
        if ( !m_counter )
        {
            m_complete = false;
            m_counter = 10;
            m_intentType = ( uint16_t )ActionCommon_t::FACTORY_RESET;
        }
        m_counter--;
        /// TODO fire event to monaco for counter display
    }
    break;
    case( uint16_t ) Action::FACTORY_RESET_CANCEL:
    {
        m_counter = 0;
        m_complete = true;
        m_intentType = 0;
        /// TODO fire event to monaco for counter display
    }
    break;
    case( uint16_t ) Action::FACTORY_RESET:
    {
        m_counter = 0;
        m_complete = true;
        /// TODO fire event to monaco for counter display
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
