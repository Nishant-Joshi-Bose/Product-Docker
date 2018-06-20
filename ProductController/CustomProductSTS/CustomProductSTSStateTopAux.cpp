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
    Init();
}

void CustomProductSTSStateTopAux::Init()
{
    BOSE_INFO( m_logger, __func__ );

    // callbacks
    auto AuxPlayCb = [this]()
    {
        AuxPlay();
    };
    auto AuxPauseCb = [this]()
    {
        AuxStopPlaying( false );
    };
    auto AuxStopCb = [this]()
    {
        AuxStopPlaying( true );
    };

    // initialize the aggregateStatus - action map
    // 1) aux cable - IN, UserPlayStatus - PLAY Expected action - PLAY
    SetAuxInertedStatus( true );
    SetUserPlayStatus( true );
    m_AuxPlayStatusMap[m_AuxAggregateStatus.key] = AuxPlayCb;
    // 2) aux cable - IN, UserPlayStatus - PAUSE Expected action - PAUSE
    SetAuxInertedStatus( true );
    SetUserPlayStatus( false );
    m_AuxPlayStatusMap[m_AuxAggregateStatus.key] = AuxPauseCb;
    // 3) aux cable - OUT, UserPlayStatus - PAUSE Expected action - STOP
    SetAuxInertedStatus( false );
    SetUserPlayStatus( false );
    m_AuxPlayStatusMap[m_AuxAggregateStatus.key] = AuxStopCb;
    // 4) aux cable - OUT, UserPlayStatus - PLAY Expected action - STOP
    SetAuxInertedStatus( false );
    SetUserPlayStatus( true );
    m_AuxPlayStatusMap[m_AuxAggregateStatus.key] = AuxStopCb;

    //reset back
    SetAuxInertedStatus( false );
    SetUserPlayStatus( false );
    m_prevAggregateKey ^= m_prevAggregateKey;//max value
    BOSE_INFO( m_logger, "%s: m_prevAggregateKey=0x%x, Aux is %sinserted,User Play status:%s", __func__,
               m_prevAggregateKey, GetAuxInsertedStatus() ? "" : "NOT ", GetUserPlayStatus() ? "PLAY" : "STOP" );

}

bool CustomProductSTSStateTopAux::ProcessAuxAggregateStatus()
{
    bool actionTaken = false;
    BOSE_DEBUG( m_logger, "%s: AUX is %sactive,prevKey=0x%x,CurrentKey=0x%x, Aux is %sinserted,"
                "User Play status:%s", __func__,
                m_active ? "" : "NOT ", m_prevAggregateKey, m_AuxAggregateStatus.key,
                GetAuxInsertedStatus() ? "" : "NOT ", GetUserPlayStatus() ? "PLAY" : "STOP" );
    if( m_active && ( m_prevAggregateKey != m_AuxAggregateStatus.key ) )
    {
        BOSE_INFO( m_logger, "%s: Changing Play status", __func__ );
        m_AuxPlayStatusMap[m_AuxAggregateStatus.key]();
        m_prevAggregateKey = m_AuxAggregateStatus.key;
        actionTaken = true;
    }
    return actionTaken;
}

bool CustomProductSTSStateTopAux::HandleStop( const STS::Void & )
{
    BOSE_DEBUG( s_logger, "%s ( %s ) is %sactive, Aux Cable is %sinserted",
                __FUNCTION__, m_account.GetSourceName().c_str(),
                m_active ? "" : "not ", GetAuxInsertedStatus() ? "" : "NOT " );
    ProcessAuxAggregateStatus();
    return true;
}

bool CustomProductSTSStateTopAux::HandlePause( const STS::Void & )
{
    BOSE_DEBUG( s_logger, "%s ( %s ) is %sactive, Aux Cable is %sinserted",
                __FUNCTION__, m_account.GetSourceName().c_str(),
                m_active ? "" : "not ", GetAuxInsertedStatus() ? "" : "NOT " );
    if( GetAuxInsertedStatus() )
    {
        SetUserPlayStatus( false );
    }
    ProcessAuxAggregateStatus();
    return true;
}

bool CustomProductSTSStateTopAux::HandlePlay( const STS::Void & )
{
    BOSE_DEBUG( s_logger, "%s ( %s ) is %sactive, Aux Cable is %sinserted",
                __FUNCTION__, m_account.GetSourceName().c_str(),
                m_active ? "" : "not ", GetAuxInsertedStatus() ? "" : "NOT " );
    if( GetAuxInsertedStatus() )
    {
        SetUserPlayStatus( true );
    }
    ProcessAuxAggregateStatus();
    return true;
}

bool CustomProductSTSStateTopAux::HandleAudioStatus( const STS::AudioStatus &audioStatus )
{
    BOSE_DEBUG( m_logger, "Custom-HandleAudioStatus( %s ), audioStatus = %s, AUX is %sInserted",
                m_account.GetSourceName().c_str(), STS::AudioState::Enum_Name( audioStatus.state() ).c_str( ),
                GetAuxInsertedStatus() ? "" : "NOT " );
    ProcessAuxAggregateStatus();
    return true;
}

bool CustomProductSTSStateTopAux::HandleDeactivateRequest( const STS::DeactivateRequest &req, uint32_t seq )
{
    BOSE_INFO( m_logger, "Custom-HandleDeactivateRequest( %s )", m_account.GetSourceName().c_str() );
    SetUserPlayStatus( false );
    ProductSTSStateTop::HandleDeactivateRequest( req, seq );
    return true;
}

bool CustomProductSTSStateTopAux::HandleActivateRequest( const STS::Void &req, uint32_t seq )
{
    BOSE_INFO( m_logger, "Custom-HandleActivateRequest( %s )", m_account.GetSourceName().c_str() );
    SetUserPlayStatus( true );
    //update the canStop field
    m_np.set_canstop( true );
    //update the canPause field
    m_np.set_canpause( true );
    ProductSTSStateTop::HandleActivateRequest( req, seq );
    return true;
}

void CustomProductSTSStateTopAux::HandleAUXCableDetect( LpmServiceMessages::IpcAuxState_t IpcAuxState )
{
    // Process the AUX Cable operational state
    SetAuxInertedStatus( ( IpcAuxState.state() == LpmServiceMessages::AuxState_t::AUX_CONNECTED ) );
    BOSE_INFO( m_logger, "Custom-HandleAUXCableDetect( %s ), AUX Inserted: %s", m_account.GetSourceName().c_str(),
               GetAuxInsertedStatus() ? "TRUE" : "FALSE" );

    ProcessAUXCableState( );
}

void CustomProductSTSStateTopAux::ProcessAUXCableState( )
{
    BOSE_INFO( m_logger, "%s: Aux Cable is %sinserted, User Selection is %s",
               __func__, GetAuxInsertedStatus() ? "" : "NOT ", GetUserPlayStatus() ? "PLAY" : "STOP" );
    ProcessAuxAggregateStatus();
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

void CustomProductSTSStateTopAux::AuxStopPlaying( bool isStop )
{
    BOSE_DEBUG( m_logger, "%s: Current status:%s, req=%s", __func__,
                STS::PlayStatus::Enum_Name( m_np.playstatus( ) ).c_str(), isStop ? "STOP" : "PAUSE" );
    m_account.IPC().SendAudioStopEvent();
    if( isStop )
    {
        m_np.set_playstatus( STS::PlayStatus::STOP );
    }
    else
    {
        m_np.set_playstatus( STS::PlayStatus::PAUSE );
    }

    STS::NowPlayingChange npc;
    *( npc.mutable_nowplaying() ) = m_np;
    m_account.IPC().SendNowPlayingChangeEvent( npc );
}

