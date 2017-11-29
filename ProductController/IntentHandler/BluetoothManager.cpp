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
#include "BluetoothSinkEndpoints.h"
#include "ProductController.h"
#include "Intents.h"

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
        BluetoothSinkService::RemoveAll removeAll;

        GetFrontDoorClient()->SendPostEmptyResponse( BluetoothSinkEndpoints::REMOVE_ALL_DEVICES,
                                                     removeAll, {}, m_frontDoorClientErrorCb );
    }
    break;

    case( uint16_t ) Action::CAROUSEL_DISCOVERABLE_CONNECT_TO_LAST:
    {
        BluetoothSinkService::APP_STATUS sinkStatus =
            BluetoothSinkService::APP_INACTIVE;
        if( GetSinkStatus( sinkStatus ) )
        {
            BluetoothSinkService::PairedList pairedList;
            if( ( ( sinkStatus == BluetoothSinkService::APP_INACTIVE ) ||
                  ( sinkStatus == BluetoothSinkService::APP_PAIRABLE ) ) &&
                ( BluetoothDeviceListPresent( pairedList ) ) )
            {
                // Connect to last
                BOSE_DEBUG( s_logger, "Profile of devices present Connect to first in the list" );
                BluetoothSinkService::Connect connect;
                uint8_t index = 0;
                while( pairedList.devices_size() > index )
                {
                    if( pairedList.devices( index ).has_mac() )
                    {
                        connect.set_mac( pairedList.devices( index ).mac() );
                        GetFrontDoorClient()->SendPostEmptyResponse( BluetoothSinkEndpoints::CONNECT, connect,
                                                                     {}, m_frontDoorClientErrorCb );
                        BOSE_DEBUG( s_logger, "Sending connect for device:%s on index :%d ",
                                    pairedList.devices( index ).name().c_str(), index );
                    }
                    else
                    {
                        BOSE_ERROR( s_logger, "No mac address for device: %s: Trying next one in the list if available",
                                    pairedList.devices( index ).name().c_str() );
                    }
                    index++;
                }
            }
            else if( sinkStatus != BluetoothSinkService::APP_PAIRABLE )
            {
                BOSE_DEBUG( s_logger, "Carousel - Send to discoverable mode: sink Status : %d", sinkStatus );
                BluetoothSinkService::Pairable pairable;
                GetFrontDoorClient()->SendPostEmptyResponse( BluetoothSinkEndpoints::PAIRABLE,
                                                             pairable, {}, m_frontDoorClientErrorCb );
            }

        }
        else
        {
            BOSE_ERROR( s_logger, "Failed to get sink Status Ignoring the intent" );
        }

    }
    break;

    case( uint16_t ) Action::SEND_TO_DISCOVERABLE:
    {
        BOSE_DEBUG( s_logger, "Send to discoverable mode" );
        BluetoothSinkService::Pairable pairable;
        GetFrontDoorClient()->SendPostEmptyResponse( BluetoothSinkEndpoints::PAIRABLE,
                                                     pairable, {}, m_frontDoorClientErrorCb );
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

bool BluetoothManager::GetSinkStatus( BluetoothSinkService::APP_STATUS& status )
{
    BOSE_DEBUG( s_logger, "%s", __func__ );

    if( GetProductController().GetBluetoothAppStatus().has_status() )
    {
        BOSE_DEBUG( s_logger, "Number of devices in the PairedList:%d Status: %d",
                    GetProductController().GetBluetoothList().devices_size(),
                    GetProductController().GetBluetoothAppStatus().status() );
        status = GetProductController().GetBluetoothAppStatus().status();
        return ( true );
    }
    else
    {
        BOSE_DEBUG( s_logger, "%s: Status not present", __func__ );
    }
    return false;
}

bool BluetoothManager::BluetoothDeviceConnected()
{
    BOSE_DEBUG( s_logger, "%s", __func__ );
    // If we have a paired Sink list, we have devices connected.

    if( ( GetProductController().GetBluetoothAppStatus().has_status() ) &&
        ( GetProductController().GetBluetoothAppStatus().status() == BluetoothSinkService::APP_CONNECTED ) )
    {
        BOSE_DEBUG( s_logger, "Number of devices in the PairedList:%d",
                    GetProductController().GetBluetoothList().devices_size() );
        return true;
    }
    else
    {
        BOSE_DEBUG( s_logger, "BT: Status present: %d, status type:%d",
                    GetProductController().GetBluetoothAppStatus().has_status(),
                    GetProductController().GetBluetoothAppStatus().has_status() ?
                    GetProductController().GetBluetoothAppStatus().status() :
                    BluetoothSinkService::APP_INACTIVE );
    }
    return false;
}

bool BluetoothManager::BluetoothDeviceListPresent( BluetoothSinkService::PairedList &pairedList )
{
    BOSE_DEBUG( s_logger, "%s", __func__ );

    if( GetProductController().GetBluetoothList().devices_size() > 0 )
    {
        BOSE_DEBUG( s_logger, "Number of devices in the BluetoothList:%d",
                    GetProductController().GetBluetoothList().devices_size() );
        pairedList = GetProductController().GetBluetoothList();
        return true;
    }
    else
    {
        BOSE_DEBUG( s_logger, "No devices in the BluetoothList" );
    }
    return false;
}

}
