////////////////////////////////////////////////////////////////////////////////
/// @file     CustomProductSTSStateTopAux.h
/// @brief    Auxilary sources.
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ProductSTSAccount.h"
#include "ProductSTSStateTop.h"

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
    /// @brief Act on Stop request by translating a mute/unmute to CAPS based on internal state
    /// @param Void
    /// @return true if successful
    ////////////////////////////////////////////////////////
    bool HandlePause( const STS::Void & ) override;

    ////////////////////////////////////////////////////////
    /// @brief Act HandleMuteStatus by generating a mute to CAPS and remembering it.
    /// @param STS::MuteStatus
    /// @return true if successful
    ////////////////////////////////////////////////////////
    bool HandleMuteStatus( const STS::MuteStatus& ms ) override;

private:
    void ToggleMute() const;
    bool m_mute = false;
};
