////////////////////////////////////////////////////////////////////////////////
/// @file     CustomProductSTSStateTopAux.h
/// @brief    Auxilary sources.
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ProductSTSAccount.h"
#include "ProductSTSStateTop.h"
#include "LpmClientIF.h"
#include "ProductSTSController.h"

// AuxSourceState includes(aggregates) the individual parameter states, like - Aux Cable (insert) state,
// User Play selection
union AuxSourceState_U
{
    AuxSourceState_U()
    {
        Reset();
        SetPlaying();//to set the status for PLAY
    };
    uint32_t  key;
    struct __State
    {
        bool auxInserted : 1;
        bool userPlayStatus : 1;
    } state;
    void Reset()
    {
        key ^= key;//max value
    }
    // PLAY would happen only when
    // Aux is inserted and User has selected to PLAY
    void SetPlaying()
    {
        state.auxInserted = true;
        state.userPlayStatus = true;
    }
    bool operator != ( const AuxSourceState_U& t )
    {
        return ( t.key != key );
    }
    AuxSourceState_U& operator= ( const AuxSourceState_U& t )
    {
        key = t.key;
        return *this;
    }
};

class CustomProductSTSStateTopAux : public ProductSTSStateTop
{
public:
    CustomProductSTSStateTopAux( ProductSTSHsm& hsm,
                                 CHsmState *pSuperState,
                                 ProductSTSAccount& account );
    ~CustomProductSTSStateTopAux() override {}

    ////////////////////////////////////////////////////////
    /// @brief Act on Stop request by translating a mute/unmute to CAPS based on internal state
    /// @param Void
    /// @return true if successful
    ////////////////////////////////////////////////////////
    bool HandleStop( const STS::Void & ) override;

    ////////////////////////////////////////////////////////
    /// @brief Act on play request
    /// @param Void
    /// @return true if successful
    ////////////////////////////////////////////////////////
    bool HandlePlay( const STS::Void & ) override;

    ////////////////////////////////////////////////////////
    /// @brief Act on pause request
    /// @param Void
    /// @return true if successful
    ////////////////////////////////////////////////////////
    bool HandlePause( const STS::Void & ) override;

    ////////////////////////////////////////////////////////
    /// @brief Act HandleAudioStatus only if the AUX cable is not inserted by doing SendAudioStopEvent
    /// @param STS::AudioStatus
    /// @return true if successful
    ////////////////////////////////////////////////////////
    bool HandleAudioStatus( const STS::AudioStatus &audioStatus ) override;

    ////////////////////////////////////////////////////////
    /// @brief Act HandleDeactivateRequest by generating a stop to CAPS and remembering it
    /// and then executing HandleDeactivateRequest from base.
    /// @param STS::const STS::DeactivateRequest &req, uint32_t seq
    /// @return true if successful
    ////////////////////////////////////////////////////////
    bool HandleDeactivateRequest( const STS::DeactivateRequest &req, uint32_t seq ) override;

    ////////////////////////////////////////////////////////
    /// @brief Act on Activate request - send error response for wrong state
    /// @param ActivateRequest - Proto buf of Activate message (unused)
    /// @param seq - transaction sequence number
    /// @return always true
    ////////////////////////////////////////////////////////
    bool HandleActivateRequest( const STS::Void &, uint32_t ) override;
private:
    void ProcessAUXCableState( );
    void HandleAUXCableDetect( LpmServiceMessages::IpcAuxState_t IpcAuxState );
    void RegisterAuxPlugStatusCallbacks();
    void AuxPlay();
    void AuxStopPlaying( );
    inline void SetUserPlayStatus( bool isPlay )
    {
        m_NextState.state.userPlayStatus = isPlay;
    }
    inline bool GetUserPlayStatus() const
    {
        return m_NextState.state.userPlayStatus;
    }
    inline void SetAuxInertedStatus( bool isInserted )
    {
        m_NextState.state.auxInserted = isInserted;
    }
    inline bool GetAuxInsertedStatus() const
    {
        return m_NextState.state.auxInserted;
    }
    void Init();
    void ProcessAuxAggregateStatus();
    bool ProcessUserPlayStatus( bool isPlay );
    AuxSourceState_U m_NextState;//Next state
    std::unordered_map<uint32_t, Callback<>> m_AuxStateActionMap;
    AuxSourceState_U m_CurrentState;// Current State
};
