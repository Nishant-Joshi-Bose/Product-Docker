////////////////////////////////////////////////////////////////////////////////
///// @file   BatteryManager.cpp
///// @brief  Implements product controller BatteryManager class.
/////
///// @attention Copyright 2019 Bose Corporation, Framingham, MA
//////////////////////////////////////////////////////////////////////////////////

#include <sys/stat.h>
#include <fcntl.h>

#include "APTaskFactory.h"
#include "BreakThread.h"
#include "SystemUtils.h"
#include "FrontDoorClient.h"
#include "BatteryManager.h"
#include "ProductController.h"
#include "LpmClientFactory.h"
#include "SyncCallback.h"
#include "EndPointsDefines.h"
/// TODO remove unncessary libraries

static DPrint s_logger( "BatteryManager" );
static constexpr char FRONTDOOR_ENDPOINT_BATTERY[]   = "/system/battery";

using namespace ::BatteryManagerPb;

namespace ProductApp
{

/*! \brief Constructor.
 */
BatteryManager::BatteryManager( ProductController& controller,
                                const std::shared_ptr<FrontDoorClientIF>& fdClient,
                                LpmClientIF::LpmClientPtr clientPtr):
    m_productController( controller ),
    m_frontdoorClientPtr( fdClient ),
    m_lpmClient( clientPtr ),

    m_task( IL::CreateTask( "BatteryManagerTask" ) )   
{
}

/*! \brief Destructor.
 */
BatteryManager::~BatteryManager()
{
    if( m_task != NULL )
    {
        IL::StopTask( m_task );
        IL::JoinTask( m_task );
    }
}

/*! \brief Initializes BatteryManager class by registering Frontdoor API's
 * \param void
 * \return void
 */
void BatteryManager::Initialize()
{
    BOSE_INFO( s_logger, "BM: Initialized battery manager" );
    RegisterFrontdoorEndPoints(); //Registering endpoints for Frontdoor
}

/*! \brief Registering for LPM callback events
 */
void BatteryManager::RegisterLpmEvents()
{
    BOSE_INFO( s_logger, "BM: Registering LPM events ... " );
}

/*! \brief Registering Frontdoor API's
 */
void BatteryManager::RegisterFrontdoorEndPoints()
{
    BOSE_INFO( s_logger, "BM: Registered FrontDoor endpoints" );
    
    // GET
    AsyncCallback< Callback<SystemBatteryResponse>, Callback<FrontDoor::Error>> getBatteryCb( std::bind (&BatteryManager::HandleGetBatteryRequest, this, std::placeholders::_1),
                                                                                              m_productController.GetTask());
    m_frontdoorClientPtr->RegisterGet( FRONTDOOR_ENDPOINT_BATTERY, getBatteryCb,
                                       FrontDoor::PUBLIC,
                                       FRONTDOOR_PRODUCT_CONTROLLER_VERSION,
                                       FRONTDOOR_PRODUCT_CONTROLLER_GROUP_NAME );

    //TODO: remove PUT 
    // PUT
    AsyncCallback<SystemBatteryResponse, Callback<SystemBatteryResponse>, Callback<FrontDoor::Error>> putBatteryCb(
                                                                                 std::bind( &BatteryManager::HandleDebugPutBatteryRequest, this, std::placeholders::_1, std::placeholders::_2 ),
                                                                                 m_productController.GetTask());
    m_frontdoorClientPtr->RegisterPut<SystemBatteryResponse>( FRONTDOOR_ENDPOINT_BATTERY, putBatteryCb,
                                                            FrontDoor::PUBLIC,
                                                            FRONTDOOR_PRODUCT_CONTROLLER_VERSION,
                                                            FRONTDOOR_PRODUCT_CONTROLLER_GROUP_NAME );
}

//TODO: remove HandlePUT 
/*! \brief Handles PUT requests for debugging purposes.
 * \param req incoming battert status to apply.
 * \param resp Callback into which will be written current battery statys as a SystemBatteryResponse protobuf.
 */
void BatteryManager::HandleDebugPutBatteryRequest (SystemBatteryResponse req, 
                                              const Callback <SystemBatteryResponse> resp)
{
    BOSE_INFO( s_logger, "BM: Handling PUT request ... " );
    m_systemBatteryResponse.set_chargestatus(req.chargestatus());
    m_systemBatteryResponse.set_minutestofull(req.minutestofull());
    m_systemBatteryResponse.set_minutestoempty(req.minutestoempty());
    m_systemBatteryResponse.set_percent(req.percent());

    resp.Send( m_systemBatteryResponse );
    m_frontdoorClientPtr->SendNotification( FRONTDOOR_ENDPOINT_BATTERY, m_systemBatteryResponse );
}

void BatteryManager::DebugSetBattery(SystemBatteryResponse req)
{
    BOSE_INFO( s_logger, "BM: Handling APQ TAP PUT request ... " );
    m_systemBatteryResponse.set_chargestatus(req.chargestatus());
    m_systemBatteryResponse.set_minutestofull(req.minutestofull());
    m_systemBatteryResponse.set_minutestoempty(req.minutestoempty());
    m_systemBatteryResponse.set_percent(req.percent());

    m_frontdoorClientPtr->SendNotification( FRONTDOOR_ENDPOINT_BATTERY, m_systemBatteryResponse );
}

/*! \brief Frontdoor GET request handler for /system/battery.
 * \param resp Callback into which will be written current battery statys as a SystemBatteryResponse protobuf.
 */
void BatteryManager::HandleGetBatteryRequest(const Callback<SystemBatteryResponse>& resp ) //( Callback<Display> resp )
{
    BOSE_INFO( s_logger, "BM: Handling GET request ...");
    resp.Send(m_systemBatteryResponse);
}


/*! \brief
 *  \param 
 */
bool BatteryManager::HandleLpmNotificationSystemBattery()//IpcSystemBatteryResponse_t battery)
{
    /*  TODO change to real values
        // m_systemBatteryResponse.set_chargestatus(ChargerStatusIpcEnumToProtoEnum( (IpcSystemBatteryResponse_t) battery.chargerStatus()) );
        // m_systemBatteryResponse.set_minutestofull(battery.minutesToFull());
        // m_systemBatteryResponse.set_minutestoempty(battery.minutesToEmpty());
        // m_systemBatteryResponse.set_percent(battery.percent());
    */
    BOSE_INFO( s_logger, "BM: LPM sent a notification about battery status." );

    m_systemBatteryResponse.set_chargestatus(DISCHARGING);
    m_systemBatteryResponse.set_minutestofull(100);
    m_systemBatteryResponse.set_minutestoempty(23);
    m_systemBatteryResponse.set_percent(35);

    m_frontdoorClientPtr->SendNotification( FRONTDOOR_ENDPOINT_BATTERY, m_systemBatteryResponse );

    return true;
}

/*! \brief
 *  \param 
 */
void BatteryManager::ChargeStatusProtoEnumToIpcEnum()
{
}

/*! \brief
 *  \param 
 */
void BatteryManager::ChargeStatusIpcEnumToProtoEnum( )
{
}

} //namespace ProductApp
