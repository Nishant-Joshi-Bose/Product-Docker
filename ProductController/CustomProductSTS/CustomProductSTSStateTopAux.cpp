////////////////////////////////////////////////////////////////////////////////
/// @file     CustomProductSTSStateTopAux.cpp
/// @brief    Custom module to play/stop audio for AUX
////////////////////////////////////////////////////////////////////////////////

#include "CustomProductSTSStateTopAux.h"

static DPrint s_logger( "CustomProductSTSStateTopAux" );
//////////////////////////////////////////////////////////////
CustomProductSTSStateTopAux::CustomProductSTSStateTopAux( ProductSTSHsm& hsm,
                                                          CHsmState *pSuperState,
                                                          ProductSTSAccount& account ) :
    ProductSTSStateTop( hsm, pSuperState, account )
{
}

bool CustomProductSTSStateTopAux::HandleStop( const STS::Void & )
{
    BOSE_INFO( s_logger, "%s ( %s ) Mute status = %d",
               __FUNCTION__, m_account.GetSourceName().c_str(), m_mute );
    if( ! m_active )
    {
        BOSE_ERROR( s_logger,  "%s AUX Source not active", __FUNCTION__ );
        return false;
    }

    ToggleMute();
    return true;
}

bool CustomProductSTSStateTopAux::HandlePause( const STS::Void & )
{
    BOSE_INFO( s_logger, "%s ( %s ) Mute status = %d",
               __FUNCTION__, m_account.GetSourceName().c_str(), m_mute );
    if( ! m_active )
    {
        BOSE_ERROR( s_logger,  "%s AUX Source not active", __FUNCTION__ );
        return false;
    }

    ToggleMute();
    return true;
}

bool CustomProductSTSStateTopAux::HandleMuteStatus( const STS::MuteStatus& ms )
{
    BOSE_INFO( s_logger, "%s ( %s ) MuteStatus :%d ",
               __FUNCTION__, m_account.GetSourceName().c_str(), ms.muteenabled() );

    m_mute = ms.muteenabled();
    return true;
}

// @TODO: https://jirapro.bose.com/browse/CASTLE-14043: Mute will be removed with this Jira fix
void CustomProductSTSStateTopAux::ToggleMute() const
{
    m_account.IPC().SendMuteControlEvent( !m_mute );
}

bool CustomProductSTSStateTopAux::HandleDeactivateRequest( const STS::DeactivateRequest &req, uint32_t seq )
{
    BOSE_INFO( m_logger, "Custom-HandleDeactivateRequest( %s )", m_account.GetSourceName().c_str() );
    if( m_mute )
    {
        ToggleMute();
    }
    ProductSTSStateTop::HandleDeactivateRequest( req, seq );
    return true;
}
