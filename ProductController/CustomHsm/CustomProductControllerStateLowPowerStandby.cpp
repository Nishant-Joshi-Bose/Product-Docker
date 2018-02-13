////////////////////////////////////////////////////////////////////////////////
/// @file   CustomProductControllerStateLowPowerStandby.cpp
/// @brief  The Low Power Standby State in the Eddie Product.
///
/// Copyright 2017 Bose Corporation
////////////////////////////////////////////////////////////////////////////////

#include "CustomProductControllerStateLowPowerStandby.h"
#include "ProductControllerHsm.h"
#include "EddieProductController.h"
#include "DPrint.h"
#include "SystemUtils.h"

static DPrint s_logger( "CustomProductControllerStateLowPowerStandby" );

namespace ProductApp
{
CustomProductControllerStateLowPowerStandby::CustomProductControllerStateLowPowerStandby( ProductControllerHsm& hsm,
        CHsmState* pSuperState,
        Hsm::STATE stateId,
        const std::string& name ) :
    ProductControllerStateLowPowerStandby( hsm, pSuperState, stateId, name )
{
    BOSE_INFO( s_logger, __func__ );
}

void CustomProductControllerStateLowPowerStandby::HandleStateStart()
{
    BOSE_INFO( s_logger, __func__ );

    ///
    ///Turn OFF LCD display controller
    ///TODO: Handshake with Display Controller module and perform this action
    ///
    BOSE_INFO( s_logger, "Disable LCD display controller TE_SYNC in %s.", __func__ );
    SystemUtils::Spawn( { "echo -n 0 > /sys/devices/soc/7af6000.spi/spi_master/spi6/spi6.1/graphics/fb1/te" } );
    BOSE_INFO( s_logger, "LCD display controller is turned OFF in %s.", __func__ );
    SystemUtils::Spawn( { "/opt/Bose/webkit/target_scripts/lcd/turn-lcd-off" } );

    ProductControllerStateLowPowerStandby::HandleStateStart();

    ///
    ///Turn ON LCD display controller
    ///TODO: Handshake with Display Controller module and perform this action
    ///
    BOSE_INFO( s_logger, "LCD display controller is turned ON in %s.", __func__ );
    SystemUtils::Spawn( { "/opt/Bose/webkit/target_scripts/lcd/turn-lcd-on" } );
    BOSE_INFO( s_logger, "Enable LCD display controller TE_SYNC in %s.", __func__ );
    SystemUtils::Spawn( { "echo -n 1 > /sys/devices/soc/7af6000.spi/spi_master/spi6/spi6.1/graphics/fb1/te" } );
}

} /// namespace ProductApp
