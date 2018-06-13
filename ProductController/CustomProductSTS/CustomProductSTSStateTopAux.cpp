////////////////////////////////////////////////////////////////////////////////
/// @file     CustomProductSTSStateTopAux.cpp
/// @brief    Custom module to play/stop audio for AUX
////////////////////////////////////////////////////////////////////////////////

#include "CustomProductSTSStateTopAux.h"
#include "EddieProductController.h"

static DPrint s_logger( "CustomProductSTSStateTopAux" );
static constexpr uint32_t LOW_LATENCY_DELAYED_START_MS = 25;
//////////////////////////////////////////////////////////////
CustomProductSTSStateTopAux::CustomProductSTSStateTopAux( ProductSTSHsm& hsm,
                                                          CHsmState *pSuperState,
                                                          ProductSTSAccount& account ) :
    ProductSTSStateTop( hsm, pSuperState, account )
{
    RegisterAuxPlugStatusCallbacks();
}

bool CustomProductSTSStateTopAux::HandleStop( const STS::Void & )
{
    BOSE_INFO( s_logger, "%s ( %s ) is %sactive ",
               __FUNCTION__, m_account.GetSourceName().c_str(), m_active ? "" : "not " );
    if( ! m_active )
    {
        BOSE_ERROR( s_logger,  "%s AUX Source not active", __FUNCTION__ );
        return false;
    }
    AuxStop();
    return true;
}

bool CustomProductSTSStateTopAux::HandlePlay( const STS::Void & )
{
    BOSE_INFO( s_logger, "%s ( %s ) is %sactive ",
               __FUNCTION__, m_account.GetSourceName().c_str(), m_active ? "" : "not " );
    if( ! m_active )
    {
        BOSE_ERROR( s_logger,  "%s AUX Source not active", __FUNCTION__ );
        return false;
    }
    AuxPlay();
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

bool CustomProductSTSStateTopAux::HandleAudioStatus( const STS::AudioStatus &audioStatus )
{
    BOSE_DEBUG( m_logger, "Custom-HandleAudioStatus( %s ), audioStatus = %s, AUX Inserted: %d",
                m_account.GetSourceName().c_str(), STS::AudioState::Enum_Name( audioStatus.state() ).c_str( ),
                m_auxInserted );
    if( ( !m_auxInserted ) && ( audioStatus.state() == STS::AudioState::PLAYING ) )
    {
        BOSE_DEBUG( s_logger, "AUX Cable is not inserted, so issue a stop" );
        m_account.IPC().SendAudioStopEvent();
        m_np.set_playstatus( STS::PlayStatus::STOP );

        STS::NowPlayingChange npc;
        *( npc.mutable_nowplaying() ) = m_np;
        m_account.IPC().SendNowPlayingChangeEvent( npc );
    }
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

void CustomProductSTSStateTopAux::HandleAUXCableDetect( LpmServiceMessages::IpcAuxState_t IpcAuxState )
{
    // Process the AUX Cable operational state
    m_auxInserted = ( IpcAuxState.state() == LpmServiceMessages::AuxState_t::AUX_CONNECTED );
    BOSE_INFO( m_logger, "Custom-HandleAUXCableDetect( %s ), AUX Inserted: %d", m_account.GetSourceName().c_str(), m_auxInserted );

    if( ! m_active )
    {
        return;
    }

    ProcessAUXCableState( );
}

void CustomProductSTSStateTopAux::ProcessAUXCableState( )
{
    if( m_auxInserted == false )
    {
        AuxPlay();
    }
    else
    {
        AuxPlay();
    }
    return;
}
void CustomProductSTSStateTopAux::RegisterAuxPlugStatusCallbacks()
{
    BOSE_INFO( m_logger, "%s: ", __FUNCTION__ );
    auto func = [ this ]( LpmServiceMessages::IpcAuxState_t IpcAuxState )
    {
        HandleAUXCableDetect( IpcAuxState );
    };

    AsyncCallback<LpmServiceMessages::IpcAuxState_t> cb( func, m_account.s_ProductSTSController->GetTask() );

    ( static_cast<ProductApp::EddieProductController*>( &( m_account.s_ProductSTSController->GetProductController() ) ) )->RegisterAuxEvents( cb );
    return;
}
void CustomProductSTSStateTopAux::AuxPlay()
{
    if( m_np.playstatus( ) != STS::PlayStatus::PLAY )
    {
        const std::string& URL = GetURL( );
        BOSE_DEBUG( s_logger, "%s:Playing AUX stream", __func__ );
        if( !URL.empty( ) )
        {
            STS::AudioSetURL asu;
            asu.set_url( URL );
            asu.set_startoffsetms( LOW_LATENCY_DELAYED_START_MS );
            m_account.IPC().SendAudioSetURLEvent( asu );
        }

        m_account.IPC().SendAudioPlayEvent();
        m_np.set_playstatus( STS::PlayStatus::PLAY );

        STS::NowPlayingChange npc;
        *( npc.mutable_nowplaying() ) = m_np;
        m_account.IPC().SendNowPlayingChangeEvent( npc );
    }
    else
    {
        //something went wrong. Do not expect PLAY when it is already PLAY. log it
        BOSE_WARNING( m_logger, "%s: PLAY called when current status is already PLAY", __FUNCTION__ );
    }
}

void CustomProductSTSStateTopAux::AuxStop()
{
    if( m_np.playstatus( ) ==  STS::PlayStatus::PLAY )
    {
        BOSE_DEBUG( s_logger, "%s: Stopping AUX stream", __func__ );
        m_account.IPC().SendAudioStopEvent();
        m_np.set_playstatus( STS::PlayStatus::STOP );

        STS::NowPlayingChange npc;
        *( npc.mutable_nowplaying() ) = m_np;
        m_account.IPC().SendNowPlayingChangeEvent( npc );
    }
    else
    {
        //something went wrong. Do not expect stop while is not PLAY. log it
        BOSE_WARNING( m_logger, "%s: STOP called when current status is not PLAY", __FUNCTION__ );
    }
}

