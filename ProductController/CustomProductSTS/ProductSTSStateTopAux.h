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
    /// @brief Act on Stop request
    /// @param STS::MuteStatus (mute enabled - True/false)
    /// @return always true if successful
    ////////////////////////////////////////////////////////
    bool HandleMuteStatus( const STS::MuteStatus& ms ) override;
};
