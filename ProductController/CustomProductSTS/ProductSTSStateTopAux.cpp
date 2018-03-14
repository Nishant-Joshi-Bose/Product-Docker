////////////////////////////////////////////////////////////////////////////////
/// @file     ProductSTSStateTopAux.cpp
/// @brief    ProductSTSStateTop for sourcces that do not play audio
////////////////////////////////////////////////////////////////////////////////

#include "ProductSTSStateTopAux.h"

//////////////////////////////////////////////////////////////
ProductSTSStateTopAux::ProductSTSStateTopAux( ProductSTSHsm& hsm,
                                              CHsmState *pSuperState,
                                              ProductSTSAccount& account ) :
    ProductSTSStateTop( hsm, pSuperState, account )
{
}

bool ProductSTSStateTopAux::HandlePlay( const STS::Void & )
{
    BOSE_INFO( m_logger, "%s ( %s )",
               __FUNCTION__, m_account.GetSourceName().c_str() );
    if (! m_active )
    {
        BOSE_ERROR( m_logger,  "%s AUX Source not active", __FUNCTION__ );
        return false;
    }
    m_account.IPC().SendMuteControlEvent( false );
    return true;
}

bool ProductSTSStateTopAux::HandleStop( const STS::Void & )
{
    BOSE_INFO( m_logger, "%s ( %s )",
               __FUNCTION__, m_account.GetSourceName().c_str() );
    if (! m_active )
    {
        BOSE_ERROR( m_logger,  "%s AUX Source not active", __FUNCTION__ );
        return false;
    }
    m_account.IPC().SendMuteControlEvent( true );
    return true;
}

bool ProductSTSStateTopAux::HandlePause( const STS::Void & )
{
    BOSE_INFO( m_logger, "%s ( %s )",
               __FUNCTION__, m_account.GetSourceName().c_str() );
    if (! m_active )
    {
        BOSE_ERROR( m_logger,  "%s AUX Source not active", __FUNCTION__ );
        return false;
    }
    m_account.IPC().SendMuteControlEvent( true );
    return true;
}

bool ProductSTSStateTopAux::HandleMuteStatus( const STS::MuteStatus& ms )
{
    if (! m_active )
    {
        BOSE_ERROR( m_logger,  "%s AUX Source not active", __FUNCTION__ );
        return false;
    }
    if ( ms.muteenabled() )
        m_np.set_playstatus( STS::PlayStatus::PAUSE );
    else
        m_np.set_playstatus( STS::PlayStatus::PLAY );

    STS::NowPlayingChange npc;
    *( npc.mutable_nowplaying() ) = m_np;
    m_account.IPC().SendNowPlayingChangeEvent( npc );
    return true;
}

#if 0

bool ProductSTSStateTopAux::HandleAudioStatus( const STS::AudioStatus& audioStatus )
{
    std::string state = ProtoBufUtils::GetStringNameForEnumValue( audioStatus, "state" );

    if ( m_active )
    {
        STS::AudioStatusToNowPlaying( audioStatus, m_np, true );
        m_nowPlaying.SetNowPlaying( m_np );
    }
    else
    {
        BOSE_ERROR( m_logger,  "%s Source not active", __FUNCTION__ );
    }
    return true;
}

if you want to revert
// AJAY m_account.IPC().SendAudioStopEvent( );
    m_account.IPC().SendAudioPlayEvent( );
Want to use SendMuteControlEvent(true/false);
bool HandleMuteStatus( const STS::MuteStatus& ) override;
bool ProductSTSStateTop::HandleNowPlayingRequest( const STS::Void &, uint32_t seq )
{
    BOSE_INFO( m_logger, "HandleNowPlayingRequest( %s ) while %sactive", m_account.GetSourceName().c_str(),
               m_active ? "" : "in" );
    if( m_active )
    {
        STS::NowPlayingResponse npr;
        *( npr.mutable_nowplaying() ) = m_np;
        m_account.IPC().SendNowPlayingResponse( npr, seq );
    }
    else
    {
        STS::AsyncError e;
        e.set_error( SoundTouchInterface::ErrorType::MUSIC_SERVICE_WRONG_STATE_NOW_PLAYING_REQUEST );
        e.set_desc( m_account.GetSourceName() + "RcvdNowPlayingInWrongState" );
        m_account.IPC().SendNowPlayingFailureResponse( e, seq );
    }
    return true;
}
#endif
