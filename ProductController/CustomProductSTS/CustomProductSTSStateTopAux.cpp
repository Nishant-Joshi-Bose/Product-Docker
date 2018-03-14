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
    ProductSTSStateTop( hsm, pSuperState, account ),
    m_mute( false )
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

    if( m_mute )
        m_account.IPC().SendMuteControlEvent( false );
    else
        m_account.IPC().SendMuteControlEvent( true );
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

    if( m_mute )
        m_account.IPC().SendMuteControlEvent( false );
    else
        m_account.IPC().SendMuteControlEvent( true );
    return true;
}

bool CustomProductSTSStateTopAux::HandleMuteStatus( const STS::MuteStatus& ms )
{
    BOSE_INFO( s_logger, "%s ( %s ) MuteStatus :%d ",
               __FUNCTION__, m_account.GetSourceName().c_str(), ms.muteenabled() );

    if( ! m_active )
    {
        BOSE_ERROR( s_logger,  "%s AUX Source not active", __FUNCTION__ );
        return false;
    }
    if( ms.muteenabled() )
        m_mute = true;
    else
        m_mute = false;

    return true;
}
