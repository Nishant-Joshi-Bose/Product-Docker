////////////////////////////////////////////////////////////////////////////////
/// @file     ProductSTSStateTopAux.h
/// @brief    Auxilary sources that translates Pause and Stop to Mute
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ProductSTSAccount.h"
#include "ProductSTSStateTop.h"

class ProductSTSStateTopAux : public ProductSTSStateTop
{
public:
    ProductSTSStateTopAux( ProductSTSHsm& hsm,
                           CHsmState *pSuperState,
                           ProductSTSAccount& account );
    ~ProductSTSStateTopAux() override {}

    ////////////////////////////////////////////////////////
    /// @brief Act on Play request by translating a un-mute to CAPS
    /// @param Void
    /// @return always true if successful
    ////////////////////////////////////////////////////////
    bool HandlePlay( const STS::Void & ) override;

    ////////////////////////////////////////////////////////
    /// @brief Act on Stop request by translating a mute to CAPS
    /// @param Void
    /// @return always true if successful
    ////////////////////////////////////////////////////////
    bool HandleStop( const STS::Void & ) override;

    ////////////////////////////////////////////////////////
    /// @brief Act on Pause request by translating a mute to CAPS
    /// @param Void
    /// @return always true if successful
    ////////////////////////////////////////////////////////
    bool HandlePause( const STS::Void & ) override;

    ////////////////////////////////////////////////////////
    /// @brief Act HandleAudioStatus by generating a nowPlayingChange to CAPS
    /// @param STS::AudioStatus
    /// @return always true if successful
    ////////////////////////////////////////////////////////
    bool HandleAudioStatus( const STS::AudioStatus& audioStatus ) override;

private:
    void AudioStatusToNowPlaying( const STS::AudioStatus& s, STS::NowPlaying& np );

};
