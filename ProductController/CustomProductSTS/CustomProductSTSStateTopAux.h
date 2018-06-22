////////////////////////////////////////////////////////////////////////////////
/// @file     CustomProductSTSStateTopAux.h
/// @brief    Auxilary sources.
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ProductSTSAccount.h"
#include "ProductSTSStateTop.h"
#include "LpmClientIF.h"
#include "ProductSTSController.h"

typedef union __auxAggregateStatus
{
    uint32_t  key;
    struct __aggrStatus
    {
        bool auxInserted : 1;
        bool userPlayStatus : 1;
    } aggrStatus;
} auxAggregateStatus_t;

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
        m_AuxAggregateStatus.aggrStatus.userPlayStatus = isPlay;
    }
    inline bool GetUserPlayStatus() const
    {
        return m_AuxAggregateStatus.aggrStatus.userPlayStatus;
    }
    inline void SetAuxInertedStatus( bool isInserted )
    {
        m_AuxAggregateStatus.aggrStatus.auxInserted = isInserted;
    }
    inline bool GetAuxInsertedStatus() const
    {
        return m_AuxAggregateStatus.aggrStatus.auxInserted;
    }
    void Init();
    bool ProcessAuxAggregateStatus();
    auxAggregateStatus_t m_AuxAggregateStatus;//current aggregate status
    std::unordered_map<uint32_t, Callback<>> m_AuxPlayStatusMap;
    uint32_t m_prevAggregateKey;//used as cache
};
