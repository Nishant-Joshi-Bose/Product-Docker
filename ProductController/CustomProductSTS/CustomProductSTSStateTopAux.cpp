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
    BOSE_INFO( s_logger, "%s ( %s ) is %sactive, Aux Cable is %sinserted",
               __FUNCTION__, m_account.GetSourceName().c_str(),
               m_active ? "" : "not ", m_auxInserted ? "" : "NOT " );
    if( ! m_active )
    {
        BOSE_ERROR( s_logger,  "%s AUX Source not active", __FUNCTION__ );
        return false;
    }
    m_userPlayStatus = false;
    if( m_auxInserted )
    {
        AuxStop();
    }
    return true;
}

bool CustomProductSTSStateTopAux::HandlePlay( const STS::Void & )
{
    BOSE_INFO( s_logger, "%s ( %s ) is %sactive, Aux Cable is %sinserted",
               __FUNCTION__, m_account.GetSourceName().c_str(),
               m_active ? "" : "not ", m_auxInserted ? "" : "NOT " );
    if( ! m_active )
    {
        BOSE_ERROR( s_logger,  "%s AUX Source not active", __FUNCTION__ );
        return false;
    }
    m_userPlayStatus = true;//save the user selection as PLAY.
    if( m_auxInserted )
    {
        AuxPlay();
    }
    return true;
}

bool CustomProductSTSStateTopAux::HandleMuteStatus( const STS::MuteStatus& ms )
{
    BOSE_INFO( s_logger, "%s: No Action on MUTE ( %s ) MuteStatus :%d ",
               __FUNCTION__, m_account.GetSourceName().c_str(), ms.muteenabled() );
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

bool CustomProductSTSStateTopAux::HandleDeactivateRequest( const STS::DeactivateRequest &req, uint32_t seq )
{
    BOSE_INFO( m_logger, "Custom-HandleDeactivateRequest( %s )", m_account.GetSourceName().c_str() );
    m_userPlayStatus = false;
    ProductSTSStateTop::HandleDeactivateRequest( req, seq );
    return true;
}

bool CustomProductSTSStateTopAux::HandleActivateRequest( const STS::Void &req, uint32_t seq )
{
    BOSE_INFO( m_logger, "Custom-HandleActivateRequest( %s )", m_account.GetSourceName().c_str() );
    m_userPlayStatus = true;
    //update the canStop field
    m_np.set_canstop( true );
    ProductSTSStateTop::HandleActivateRequest( req, seq );
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
        AuxStop();
    }
    else if( m_userPlayStatus )
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
        BOSE_INFO( m_logger, "%s: No Action Current State:%s",
                   __FUNCTION__, STS::PlayStatus::Enum_Name( m_np.playstatus( ) ).c_str() );
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
        BOSE_INFO( m_logger, "%s: No Action ,Current State:%s",
                   __FUNCTION__, STS::PlayStatus::Enum_Name( m_np.playstatus( ) ).c_str() );
    }
}

