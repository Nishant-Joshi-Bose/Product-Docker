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

/*!
 */
BatteryManager::BatteryManager( ProductController& controller,
                                const std::shared_ptr<FrontDoorClientIF>& fdClient,
                                LpmClientIF::LpmClientPtr clientPtr ):
    m_productController( controller ),
    m_frontdoorClientPtr( fdClient ),
    m_lpmClient( clientPtr )
{
}

/*!
 */
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

/*!
 */
void BatteryManager::RegisterFrontdoorEndPoints()
{
    // GET
    AsyncCallback< Callback<SystemBatteryResponse>, Callback<FrontDoor::Error>> getBatteryCb( std::bind( &BatteryManager::HandleGetBatteryRequest, this, std::placeholders::_1 ),
                                                                             m_productController.GetTask() );
    m_frontdoorClientPtr->RegisterGet( FRONTDOOR_ENDPOINT_BATTERY, getBatteryCb,
                                       FrontDoor::PUBLIC,
                                       FRONTDOOR_PRODUCT_CONTROLLER_VERSION,
                                       FRONTDOOR_PRODUCT_CONTROLLER_GROUP_NAME );
}


void BatteryManager::DebugSetBattery( SystemBatteryResponse req )
{
    SystemBatteryResponse batteryStatusPb;

    batteryStatusPb.set_chargestatus( req.chargestatus() );
    batteryStatusPb.set_minutestofull( req.minutestofull() );
    batteryStatusPb.set_minutestoempty( req.minutestoempty() );
    batteryStatusPb.set_percent( req.percent() );
    m_frontdoorClientPtr->SendNotification( FRONTDOOR_ENDPOINT_BATTERY, batteryStatusPb );

    //The following updates the local struct so the requests sent from TAP are also reflected in following GET requests
    m_batteryStatus.charge = req.chargestatus();
    m_batteryStatus.minutesToFull = req.minutestofull();
    m_batteryStatus.minutesToEmpty = req.minutestoempty();
    m_batteryStatus.percent = req.percent();
}

/*! \brief Frontdoor GET request handler for /system/battery.
 * \param resp Callback into which will be written current battery status as a SystemBatteryResponse protobuf.
 */
void BatteryManager::HandleGetBatteryRequest( const Callback<SystemBatteryResponse>& resp )
{
    SystemBatteryResponse batteryStatusPb;

    batteryStatusPb.set_chargestatus( ( chargeStatus )m_batteryStatus.charge );
    batteryStatusPb.set_minutestofull( m_batteryStatus.minutesToFull );
    batteryStatusPb.set_minutestoempty( m_batteryStatus.minutesToEmpty );
    batteryStatusPb.set_percent( m_batteryStatus.percent );
    resp.Send( batteryStatusPb );
}

} //namespace ProductApp
