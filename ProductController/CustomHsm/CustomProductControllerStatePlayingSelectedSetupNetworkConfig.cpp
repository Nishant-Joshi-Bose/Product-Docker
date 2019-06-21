////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file      CustomProductControllerStatePlayingSelectedSetupNetworkConfig.cpp
///
/// @brief     This source code file contains functionality to process events that occur during the
///            product network setup state.
///
/// @attention Copyright (C) 2018 Bose Corporation All Rights Reserved
///
///            Bose Corporation
///            The Mountain Road,
///            Framingham, MA 01701-9168
///            U.S.A.
///
///            This program may not be reproduced, in whole or in part, in any form by any means
///            whatsoever without the written permission of Bose Corporation.
///
////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////
///
///            Included Header Files
///
////////////////////////////////////////////////////////////////////////////////////////////////////
#include "Utilities.h"
#include "CustomProductControllerStatePlayingSelectedSetupNetworkConfig.h"
#include "CustomProductController.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
///                            Start of Product Application Namespace                            ///
////////////////////////////////////////////////////////////////////////////////////////////////////
namespace ProductApp
{

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStatePlayingSelectedSetupNetworkConfig::CustomProductControllerStatePlayingSelectedSetupNetworkConfig
///
/// @param ProductControllerHsm& hsm
///
/// @param CHsmState*            pSuperState
///
/// @param Hsm::STATE            stateId
///
/// @param const std::string&    name
///
////////////////////////////////////////////////////////////////////////////////////////////////////
CustomProductControllerStatePlayingSelectedSetupNetworkConfig::CustomProductControllerStatePlayingSelectedSetupNetworkConfig(
    ProductControllerHsm&       hsm,
    CHsmState*                  pSuperState,
    Hsm::STATE                  stateId,
    const std::string&          name )

    : ProductControllerStatePlayingSelectedSetupNetworkConfig( hsm, pSuperState, stateId, name )
{
    BOSE_INFO( s_logger, "The %s state is being constructed.", name.c_str() );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductControllerStatePlayingSelectedSetupNetworkConfig::HandleStateEnter
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductControllerStatePlayingSelectedSetupNetworkConfig::HandleStateEnter( )
{
    BOSE_INFO( s_logger, __func__ );
    ProductControllerStatePlayingSelectedSetupNetworkConfig::HandleStateEnter( );

    // Mute DSP Amp to avoid noise produced during AP mode on
    GetCustomProductController( ).GetLpmHardwareInterface( )->SetAmp( true, true );
    // Prepare to cache chimes that may be generated while system is muted (PGC-2819)
    m_pendingChimeEvent = static_cast<ProductApp::ChimeEvents_t>( 0 );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief  CustomProductControllerStatePlayingSelectedSetupNetworkConfig::HandleStateExit
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductControllerStatePlayingSelectedSetupNetworkConfig::HandleStateExit( )
{
    BOSE_INFO( s_logger, __func__ );
    ProductControllerStatePlayingSelectedSetupNetworkConfig::HandleStateExit( );

    // Unmute DSP Amp
    GetCustomProductController( ).GetLpmHardwareInterface( )->SetAmp( true, false );
    // Play cached chime, if any
    if( m_pendingChimeEvent )
    {
        ( void ) GetProductController().HandleChimePlayRequest( m_pendingChimeEvent );
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief  CustomProductControllerStatePlayingSelectedSetupNetworkConfig::HandlePlayChimeRequest
/// @param  ChimeEvents_t chimeEvent )
/// @return This method returns true after caching the chime
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductControllerStatePlayingSelectedSetupNetworkConfig::HandlePlayChimeRequest( ChimeEvents_t chimeEvent )
{
    BOSE_INFO( s_logger, "The %s state is in %s caching %d.", GetName( ).c_str( ), __func__, chimeEvent );

    if( chimeEvent == BT_DISCOVERABLE || chimeEvent == BT_CONNECT )
    {
        // Only pend chimes that would be playing soon, PGC-4202
        m_pendingChimeEvent = chimeEvent;
    }
    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                           End of the Product Application Namespace                           ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
