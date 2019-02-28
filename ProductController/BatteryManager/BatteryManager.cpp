////////////////////////////////////////////////////////////////////////////////
///// @file   BatteryManager.cpp
///// @brief  Implements product controller BatteryManager class.
/////
///// @attention Copyright 2019 Bose Corporation, Framingham, MA
//////////////////////////////////////////////////////////////////////////////////

#include "BatteryManager.h"

static DPrint s_logger( "BatteryManager" );
static constexpr char FRONTDOOR_ENDPOINT_BATTERY[]   = "/system/battery";

using namespace ::BatteryManagerPb;

namespace ProductApp
{

BatteryManager::BatteryManager( ProductController& controller,
                                const std::shared_ptr<FrontDoorClientIF>& fdClient,
                                LpmClientIF::LpmClientPtr clientPtr ):
    m_productController( controller ),
    m_frontdoorClientPtr( fdClient ),
    m_lpmClient( clientPtr )
{
}

void BatteryManager::Initialize()
{
    RegisterFrontdoorEndPoints();
}

/*! TODO: Register for LPM callback events.
 */
void BatteryManager::RegisterLpmEvents()
{
    BOSE_VERBOSE( s_logger, "BM: Registering LPM events ... " );
}

void BatteryManager::RegisterFrontdoorEndPoints()
{
    auto batteryGetHandler = [this]( const Callback<SystemBatteryResponse>& resp,
                                     const Callback<FrontDoor::Error>& errorCb )
    {
        SystemBatteryResponse batteryStatusPb;

        batteryStatusPb.set_chargestatus( static_cast<ChargeStatus>( m_batteryStatus.chargeStatus ) );
        batteryStatusPb.set_minutestofull( m_batteryStatus.minutesToFull );
        batteryStatusPb.set_minutestoempty( m_batteryStatus.minutesToEmpty );
        batteryStatusPb.set_percent( m_batteryStatus.percent );
        resp.Send( batteryStatusPb );
    };

    m_frontdoorClientPtr->RegisterGet( FRONTDOOR_ENDPOINT_BATTERY, batteryGetHandler,
                                       FrontDoor::PUBLIC,
                                       FRONTDOOR_PRODUCT_CONTROLLER_VERSION,
                                       FRONTDOOR_PRODUCT_CONTROLLER_GROUP_NAME );
}


void BatteryManager::DebugSetBattery( const BatteryStatus& req )
{
    m_batteryStatus = req; //updating local struct with the TAP request received

    //Sending a NOTIFY with updated battery status when a TAP request is received
    SystemBatteryResponse batteryStatusResp;
    batteryStatusResp.set_chargestatus( static_cast<ChargeStatus>( req.chargeStatus ) );
    batteryStatusResp.set_minutestofull( req.minutesToFull );
    batteryStatusResp.set_minutestoempty( req.minutesToEmpty );
    batteryStatusResp.set_percent( req.percent );
    m_frontdoorClientPtr->SendNotification( FRONTDOOR_ENDPOINT_BATTERY, batteryStatusResp );
}

} //namespace ProductApp
