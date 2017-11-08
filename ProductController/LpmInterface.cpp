////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file      LpmInterface.cpp
///
/// @brief     Class for interfacing ProductController
///            with the Low Power Microcontroller or LPM.
///
/// @author
///
/// @date      10/17/2017
///
/// @attention Copyright (C) 2017 Bose Corporation All Rights Reserved
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

#include "LpmInterface.h"
#include "LpmClientFactory.h"
#include "DPrint.h"
#include "APTaskFactory.h"

namespace ProductApp
{
static DPrint s_logger { "LpmInterface" };

LpmInterface::LpmInterface( ProductMessageCb NotifyProductCb, NotifyTargetTaskIF* task ):
    m_ProductControllerTask( task ),
    m_NotifyProductCb( NotifyProductCb )

{
    BOSE_INFO( s_logger, "Creating LPM client" );
    m_LpmClient = LpmClientFactory::Create( "EddieLpmClient", m_ProductControllerTask );
}

void LpmInterface::Initialize()
{
    BOSE_INFO( s_logger, __func__ );

    auto func = std::bind( &LpmInterface::Connected, this );
    AsyncCallback<bool> connectCb( func, m_ProductControllerTask );
    m_LpmClient->Connect( connectCb );
}

void LpmInterface::Connected()
{
    // Connection to Lpm is established
    BOSE_INFO( s_logger, __func__ );
    m_isConnected = true;

    ProductMessage response;
    response.set_id( LPM_INTERFACE_UP );
    m_NotifyProductCb( response );
}

void LpmInterface::SetAmp( bool powered, bool muted )
{
    BOSE_INFO( s_logger, "%s: %spowered, %smuted", __func__,
               powered ? "" : "!", muted ? "" : "!" );
    IpcAmp_t req;
    req.set_on( powered );
    req.set_mute( muted );
    m_LpmClient->SetAmp( req );
}

void LpmInterface::SetSystemState( LpmServiceMessages::IpcLpmSystemState_t state )
{
    if( !m_isConnected )
    {
        BOSE_ERROR( s_logger, "Failed to set Lpm System state. Lpm is not connected" );
        return;
    }
    LpmServiceMessages::IpcLpmSystemStateSet_t lpmStateSet;
    lpmStateSet.set_state( state );

    auto func = std::bind( &LpmInterface::HandleLpmStateResponse, this, std::placeholders::_1 );
    AsyncCallback<LpmServiceMessages::IpcLpmStateResponse_t> LpmSystemCallbackStatus( func, m_ProductControllerTask );
    m_LpmClient->SetSystemState( lpmStateSet, LpmSystemCallbackStatus, IPC_DEVICE_LPM );
}

void LpmInterface::SetPowerState( LpmServiceMessages::IpcLPMPowerState_t state )
{
    if( !m_isConnected )
    {
        BOSE_ERROR( s_logger, "Failed to set Lpm Power state. Lpm is not connected" );
        return;
    }
    LpmServiceMessages::IpcLPMPowerStateSet_t lpmStateSet;
    lpmStateSet.set_state( state );

    auto func = std::bind( &LpmInterface::HandleLpmStateResponse, this, std::placeholders::_1 );
    AsyncCallback<LpmServiceMessages::IpcLpmStateResponse_t> LpmSystemCallbackStatus( func, m_ProductControllerTask );
    m_LpmClient->SetPowerState( lpmStateSet, LpmSystemCallbackStatus, IPC_DEVICE_LPM );
}

void LpmInterface::HandleLpmStateResponse( LpmServiceMessages::IpcLpmStateResponse_t stateResponse )
{
    if( stateResponse.has_sysstate() )
    {
        BOSE_INFO( s_logger, "LPM System State: %s ",
                   LpmServiceMessages::IpcLpmSystemState_t_Name( stateResponse.sysstate() ).c_str() );

        ProductMessage response;
        switch( stateResponse.sysstate() )
        {
        case SYSTEM_STATE_NORMAL:
        {
            BOSE_DEBUG( s_logger, "Received SYSTEM_STATE_NORMAL message from LPM" );
            response.set_id( LPM_HARDWARE_UP );
            m_NotifyProductCb( response );
        }
        break;
        default:
            BOSE_DEBUG( s_logger, "Received unsupported system state message from LPM" );
            break;
        }
    }
    if( stateResponse.has_pwrstate() )
    {
        BOSE_INFO( s_logger, "LPM Power State: %s ",
                   LpmServiceMessages::IpcLPMPowerState_t_Name( stateResponse.pwrstate() ).c_str() );
    }
}

bool LpmInterface::isConnected()
{
    return m_isConnected;
}

}
// namespace ProductApp

