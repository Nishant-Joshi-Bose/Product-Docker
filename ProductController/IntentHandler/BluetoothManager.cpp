///////////////////////////////////////////////////////////////////////////////
/// @file BluetoothManager.cpp
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
#include "BluetoothManager.h"

static DPrint s_logger( "BluetoothManager" );

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

bool BluetoothManager::Handle( KeyHandlerUtil::ActionType_t& intent )
{
    switch( intent )
    {
    case( uint16_t ) Action::CLEAR_PAIRING_LIST:
    {
        BOSE_DEBUG( s_logger, "Send Clear pairling list" );
#if 0
        GetFrontDoorClient()->\
        SendPost<>( "/bluetooth/sink/removeAll", transportControl,
                    m_NowPlayingRsp, m_frontDoorClientErrorCb );
#endif
    }
    break;

    case( uint16_t ) Action::CAROUSEL_DISCOVERABLE_CONNECT_TO_LAST:
    {
        if( BluetoothDeviceConnected() )
        {
            BOSE_DEBUG( s_logger, "Go to discoverable mode" );
#if 0
            GetFrontDoorClient()->\
            SendPost<>( "/bluetooth/sink/pairable", transportControl,
                        m_NowPlayingRsp, m_frontDoorClientErrorCb );
#endif
        }
        else if( BluetoothDeviceListPresent() )
        {
            BOSE_DEBUG( s_logger, "Profile of devices present"
                        " Connect to first in the list" );
#if 0
            GetFrontDoorClient()->\
            SendPost<>( "/bluetooth/sink/connect", transportControl,
                        m_NowPlayingRsp, m_frontDoorClientErrorCb );
#endif
        }
        else
        {
            BOSE_DEBUG( s_logger, "Profile of devices not present"
                        " Ignoring the intent" );
        }
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

bool BluetoothManager::BluetoothDeviceConnected()
{
    BOSE_DEBUG( s_logger, "%s", __func__ );
    return true;
}

bool BluetoothManager::BluetoothDeviceListPresent()
{
    BOSE_DEBUG( s_logger, "%s", __func__ );
    return true;
}

#if 0
void BluetoothManager::PutTransportControlCbRsp( const SoundTouchInterface::NowPlayingJson& resp )
{
    // No Need to handle this as Product Controller will get a nowPlaying that
    // will update update the information.
    BOSE_DEBUG( s_logger, "%s", __func__ );
    return;
}
#endif

void BluetoothManager::FrontDoorClientErrorCb( const FRONT_DOOR_CLIENT_ERRORS errorCode )
{
    // Nothing to do for now, printing this if anyone cares.
    BOSE_ERROR( s_logger, "%s:error code- %d", __func__, errorCode );
    return;
}
}
