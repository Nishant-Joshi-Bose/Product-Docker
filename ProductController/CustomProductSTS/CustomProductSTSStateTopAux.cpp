////////////////////////////////////////////////////////////////////////////////
/// @file     CustomProductSTSStateTopAux.cpp
/// @brief    Custom module to play/stop audio for AUX
////////////////////////////////////////////////////////////////////////////////

#include "CustomProductSTSStateTopAux.h"
#include "CustomProductController.h"

static DPrint s_logger( "CustomProductSTSStateTopAux" );
static constexpr uint32_t LOW_LATENCY_DELAYED_START_MS = 25;
//////////////////////////////////////////////////////////////
CustomProductSTSStateTopAux::CustomProductSTSStateTopAux( ProductSTSHsm& hsm,
                                                          CHsmState *pSuperState,
                                                          ProductSTSAccount& account ) :
    ProductSTSStateTop( hsm, pSuperState, account )
{
    Init();
    RegisterAuxPlugStatusCallbacks();
}

void CustomProductSTSStateTopAux::Init()
{
    BOSE_INFO( m_logger, __func__ );

    // callbacks
    auto AuxPlayCallback = [this]()
    {
        AuxPlay();
    };
    auto AuxStopCallback = [this]()
    {
        AuxStopPlaying( );
    };

    auto GenerateKey = []( bool isAuxInserted, bool doesUserWantsPlay )
    {
        AuxSourceState_U AuxSourceState;
        AuxSourceState.key = 0;
        AuxSourceState.state.auxInserted = isAuxInserted;
        AuxSourceState.state.userPlayStatus = doesUserWantsPlay;
        //key is now udpated.
        return AuxSourceState.key;
    };

    // High level design:
    // Each individual status - Aux Insert state, User Play select state,
    // is aggregated and maps to a key. In other words, when you set
    // an individual state, it changes the key (using Union) automatically
    // Each binary state is represented by a bit (bit field).
    //
    // Future changes - With the usage of uint32 as key, we shall be able to
    // accommodate 32 binary states or its equivalent. You need to add
    // that parameter to the GenerateKey().

    // initialize the aggregateStatus - action map
    // 1) isAuxInserted - Yes(true), doesUserWantsPlay - PLAY(true) Expected action - PLAY
    m_AuxStateActionMap[GenerateKey( true, true )] = AuxPlayCallback;
    // 2) isAuxInserted - Yes(true), doesUserWantsPlay - STOP(false) Expected action - PAUSE
    m_AuxStateActionMap[GenerateKey( true, false )] = AuxStopCallback;
    // 3) isAuxInserted - No(false), doesUserWantsPlay - STOP(false) Expected action - STOP
    m_AuxStateActionMap[GenerateKey( false, false )] = AuxStopCallback;
    // 4) isAuxInserted - No(false), doesUserWantsPlay - PLAY(true) Expected action - STOP
    m_AuxStateActionMap[GenerateKey( false, true )] = AuxStopCallback;

    BOSE_INFO( m_logger, "%s: m_CurrentState=0x%x, Aux is %sinserted,User Play status:%s", __func__,
               m_CurrentState.key, GetAuxInsertedStatus() ? "" : "NOT ", GetUserPlayStatus() ? "PLAY" : "STOP" );

}

void CustomProductSTSStateTopAux::ProcessAuxAggregateStatus()
{
    BOSE_DEBUG( m_logger, "%s: AUX is %sactive,CurrentKey=0x%x,NextKey=0x%x, Aux is %sinserted,"
                "User Play status:%s", __func__,
                m_active ? "" : "NOT ", m_CurrentState.key, m_NextState.key,
                GetAuxInsertedStatus() ? "" : "NOT ", GetUserPlayStatus() ? "PLAY" : "STOP" );
    if( m_active && ( m_CurrentState != m_NextState ) )
    {
        BOSE_INFO( m_logger, "%s: Changing Play status ", __func__ );
        m_AuxStateActionMap[m_NextState.key]();
        m_CurrentState = m_NextState;
    }
    return ;
}

bool CustomProductSTSStateTopAux::HandleStop( const STS::Void & )
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
    m_CurrentState.Reset();
    ProductSTSStateTop::HandleDeactivateRequest( req, seq );
    return true;
}

bool CustomProductSTSStateTopAux::HandleActivateRequest( const STS::Void &req, uint32_t seq )
{
    BOSE_INFO( m_logger, "Custom-HandleActivateRequest( %s )", m_account.GetSourceName().c_str() );
    SetUserPlayStatus( true );
    // Activate would start PLAYing, so set the current state
    // to PLAYing.
    m_CurrentState.SetPlaying();
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

    AsyncCallback<LpmServiceMessages::IpcAuxState_t> cb( func, m_account.GetProductSTSController()->GetTask() );

    ( static_cast<ProductApp::CustomProductController*>( &( m_account.GetProductSTSController()->GetProductController() ) ) )->RegisterAuxEvents( cb );
    return;
}
void CustomProductSTSStateTopAux::AuxPlay()
{
    const std::string& URL = GetURL( );
    BOSE_INFO( s_logger, "%s:Playing AUX stream", __func__ );
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

void CustomProductSTSStateTopAux::AuxStopPlaying( )
{
    BOSE_INFO( m_logger, "%s: Current status:%s, req=%s", __func__,
               STS::PlayStatus::Enum_Name( m_np.playstatus( ) ).c_str(), GetAuxInsertedStatus() ? "STOP" : "PAUSE" );
    m_account.IPC().SendAudioStopEvent();
    if( GetAuxInsertedStatus() )
    {
        m_np.set_playstatus( STS::PlayStatus::PAUSE );
    }
    else
    {
        m_np.set_playstatus( STS::PlayStatus::STOP );
    }

    STS::NowPlayingChange npc;
    *( npc.mutable_nowplaying() ) = m_np;
    m_account.IPC().SendNowPlayingChangeEvent( npc );
}

