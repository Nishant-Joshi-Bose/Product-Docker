////////////////////////////////////////////////////////////////////////////////
/// @file     CustomProductSTSStateTopAux.h
/// @brief    Auxilary sources.
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ProductSTSAccount.h"
#include "ProductSTSStateTop.h"
#include "LpmClientIF.h"
#include "ProductSTSController.h"

union AuxAggregateState
{
    AuxAggregateState()
    {
        Reset();
    };
    uint32_t  key;
    struct __aggrStatus
    {
        bool auxInserted : 1;
        bool userPlayStatus : 1;
    } aggrStatus;
    void Reset()
    {
        key ^= key;//max value
    }
    bool operator != ( const AuxAggregateState& t )
    {
        return ( t.key != key );
    }
    void operator = ( const AuxAggregateState& t )
    {
        key = t.key;
    }
} ;

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
    /// @brief Act on Puase request
    /// @param Void
    /// @return true if successful
    ////////////////////////////////////////////////////////
    bool HandlePause( const STS::Void & ) override;

    ////////////////////////////////////////////////////////
    /// @brief Act on play request
    /// @param Void
    /// @return true if successful
    ////////////////////////////////////////////////////////
    bool HandlePlay( const STS::Void & ) override;

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
    void AuxStopPlaying( bool isStop );
    inline void SetUserPlayStatus( bool isPlay )
    {
        m_CurrAggregateStatus.aggrStatus.userPlayStatus = isPlay;
    }
    inline bool GetUserPlayStatus() const
    {
        return m_CurrAggregateStatus.aggrStatus.userPlayStatus;
    }
    inline void SetAuxInertedStatus( bool isInserted )
    {
        m_CurrAggregateStatus.aggrStatus.auxInserted = isInserted;
    }
    inline bool GetAuxInsertedStatus() const
    {
        return m_CurrAggregateStatus.aggrStatus.auxInserted;
    }
    void Init();
    bool ProcessAuxAggregateStatus();
    AuxAggregateState m_CurrAggregateStatus;//current aggregate status
    std::unordered_map<uint32_t, Callback<>> m_AuxPlayStatusMap;
    AuxAggregateState m_prevAggregateState;//used as cache
};
