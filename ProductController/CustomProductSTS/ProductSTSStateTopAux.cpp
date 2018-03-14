////////////////////////////////////////////////////////////////////////////////
/// @file     ProductSTSStateTopAux.cpp
/// @brief    ProductSTSStateTop for sourcces that do not play audio
////////////////////////////////////////////////////////////////////////////////

#include "ProductSTSStateTopAux.h"

static DPrint s_logger( "CustomProductSTS" );
//////////////////////////////////////////////////////////////
ProductSTSStateTopAux::ProductSTSStateTopAux( ProductSTSHsm& hsm,
                                              CHsmState *pSuperState,
                                              ProductSTSAccount& account ) :
    ProductSTSStateTop( hsm, pSuperState, account )
{
}

bool ProductSTSStateTopAux::HandlePlay( const STS::Void & )
{
    BOSE_INFO( s_logger, "%s ( %s )",
               __FUNCTION__, m_account.GetSourceName().c_str() );
    if( ! m_active )
    {
        BOSE_ERROR( s_logger,  "%s AUX Source not active", __FUNCTION__ );
        return false;
    }

    const std::string& URL = GetURL( );
    if( !URL.empty( ) )
    {
        STS::AudioSetURL asu;
        asu.set_url( URL );
        asu.set_startoffsetms( LOW_LATENCY_DELAYED_START_MS );
        m_account.IPC().SendAudioSetURLEvent( asu );
    }
    m_account.IPC().SendAudioPlayEvent();
    return true;
}

bool ProductSTSStateTopAux::HandleStop( const STS::Void & )
{
    BOSE_INFO( s_logger, "%s ( %s )",
               __FUNCTION__, m_account.GetSourceName().c_str() );
    if( ! m_active )
    {
        BOSE_ERROR( s_logger,  "%s AUX Source not active", __FUNCTION__ );
        return false;
    }
    m_account.IPC().SendAudioStopEvent( );
    return true;
}

bool ProductSTSStateTopAux::HandlePause( const STS::Void & )
{
    BOSE_INFO( s_logger, "%s ( %s )",
               __FUNCTION__, m_account.GetSourceName().c_str() );
    if( ! m_active )
    {
        BOSE_ERROR( s_logger,  "%s AUX Source not active", __FUNCTION__ );
        return false;
    }
    m_account.IPC().SendAudioStopEvent( );
    return true;
}

bool ProductSTSStateTopAux::HandleAudioStatus( const STS::AudioStatus& audioStatus )
{
    BOSE_INFO( s_logger, "%s ( %s ) ",
               __FUNCTION__, m_account.GetSourceName().c_str() );

    if( m_active )
    {
        AudioStatusToNowPlaying( audioStatus, m_np );
        STS::NowPlayingChange npc;
        *( npc.mutable_nowplaying() ) = m_np;
        m_account.IPC().SendNowPlayingChangeEvent( npc );
    }
    else
    {
        BOSE_ERROR( s_logger,  "%s Source not active", __FUNCTION__ );
    }
    return true;
}

void ProductSTSStateTopAux::AudioStatusToNowPlaying( const STS::AudioStatus& s, STS::NowPlaying& np )
{
    BOSE_INFO( s_logger, "%s (%s) PlayStatus = %d ", __FUNCTION__, m_account.GetSourceName().c_str(), s.state() );
    if( s.state() == STS::AudioState::PLAYING )
    {
        np.set_playstatus( STS::PlayStatus::PLAY );
    }
    else
    {
        // For AUX, the playStatus is set to pause for appropriate animation on LCD
        np.set_playstatus( STS::PlayStatus::STOP );
    }
}
