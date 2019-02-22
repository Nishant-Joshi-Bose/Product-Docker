////////////////////////////////////////////////////////////////////////////////
///// @file   BatteryManager.h
///// @brief  Battery and charger status declarations
/////
///// @attention Copyright 2019 Bose Corporation, Framingham, MA
//////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <iostream>
#include <memory>
#include <thread>
#include "APTask.h"
#include "APTimer.h"
#include "AsyncCallback.h"
#include "FrontDoorClientIF.h"
#include "BatteryManager.pb.h"
#include "LpmClientIF.h"
#include "NotifyTargetTaskIF.h"

using namespace ::BatteryManagerPb;

namespace ProductApp
{
class ProductController;

class BatteryManager
{
public:

    /*! \brief Constructor.
     * \param controller Reference to main Product Controller.
     * \param fdClient Frontdoor client reference.
     * \param clientPtr LpmClient reference.
     */
    BatteryManager( ProductController& controller,
                    const std::shared_ptr<FrontDoorClientIF>& fdClient,
                    LpmClientIF::LpmClientPtr clientPtr);

    /*! \brief Destructor.
     */
    ~BatteryManager();

    ///////////////////////////////////////////////////////////////////////////////
    /// @name  Initialize()
    /// @brief-
    /// @return void
    ///////////////////////////////////////////////////////////////////////////////
    void Initialize();

    /*! \brief Register for LPM callback events.
     */
    void RegisterLpmEvents();
    //////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief This method is used to return the LPM client pointer.
    ///
    //////////////////////////////////////////////////////////////////////////////////////////////
    inline SystemBatteryResponse GetBatteryStatus( )
    {
        return m_systemBatteryResponse;
    }

    /*!
     */
    void DebugSetBattery(SystemBatteryResponse req);

private:

    BatteryManager( const BatteryManager& ) = delete;
    BatteryManager& operator=( const BatteryManager& ) = delete;

    /*! \brief Register Frontdoor API's.
     */
    void RegisterFrontdoorEndPoints();
    /*!
     */
    void HandleDebugPutBatteryRequest(SystemBatteryResponse req, const Callback <SystemBatteryResponse> resp);
    /*!
     */
    void HandleGetBatteryRequest(const Callback<SystemBatteryResponse>& resp );//Callback<Display> resp );
    /*!
     */
    bool HandleLpmNotificationSystemBattery();
    /*! 
     */
    void ChargeStatusProtoEnumToIpcEnum();
    /*! 
     */
    void ChargeStatusIpcEnumToProtoEnum();

    ProductController&                 m_productController;
    std::shared_ptr<FrontDoorClientIF> m_frontdoorClientPtr;
    LpmClientIF::LpmClientPtr          m_lpmClient;

    NotifyTargetTaskIF*                m_task;                      //!< Background task for periodically polling.

    SystemBatteryResponse              m_systemBatteryResponse;
};
} //namespace ProductApp

