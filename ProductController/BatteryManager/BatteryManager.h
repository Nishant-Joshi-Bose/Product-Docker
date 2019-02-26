////////////////////////////////////////////////////////////////////////////////
///// @file   BatteryManager.h
///// @brief  Battery and charger status declarations
/////
///// @attention Copyright 2019 Bose Corporation, Framingham, MA
//////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "FrontDoorClientIF.h"
#include "BatteryManager.pb.h"
#include "LpmClientIF.h"
#include "ProductController.h"

using namespace ::BatteryManagerPb;

namespace ProductApp
{
class ProductController;

class BatteryManager
{
public:
    struct BatteryStatus
    {
        chargeStatus charge = CHARGING;
        int32_t minutesToEmpty    = 0;
        int32_t minutesToFull     = 0;
        int32_t percent           = 0;
    };
    /*! \brief Constructor.
     * \param controller Reference to main Product Controller.
     * \param fdClient Frontdoor client reference.
     * \param clientPtr LpmClient reference.
     */
    BatteryManager( ProductController& controller,
                    const std::shared_ptr<FrontDoorClientIF>& fdClient,
                    LpmClientIF::LpmClientPtr clientPtr );

    void Initialize();

    /*!
     */
    void RegisterLpmEvents();

    /*! \brief This method is used for TAP debugging purposes.
    */
    inline BatteryStatus GetBatteryStatus( )
    {
        return m_batteryStatus;
    }
    /*! \brief This method is used for TAP debugging purposes.
     */
    void DebugSetBattery( SystemBatteryResponse req );

private:

    BatteryManager( const BatteryManager& ) = delete;
    BatteryManager& operator=( const BatteryManager& ) = delete;

    /*!
     */
    void RegisterFrontdoorEndPoints();

    ProductController&                 m_productController;
    std::shared_ptr<FrontDoorClientIF> m_frontdoorClientPtr;
    LpmClientIF::LpmClientPtr          m_lpmClient;

    BatteryStatus                      m_batteryStatus;

};
} //namespace ProductApp

