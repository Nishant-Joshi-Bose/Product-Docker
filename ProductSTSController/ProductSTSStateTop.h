////////////////////////////////////////////////////////////////////////////////
/// @file     ProductSTSStateTop.h
/// @brief
/// @author   Yishai Sered
/// @date      Creation Date: 8/18/2016
///
/// Copyright 2016 Bose Corporation
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "STSState.h"
#include "STSNowPlaying.pb.h"
#include "DPrint.h"

class ProductSTSAccount;
class ProductSTSHsm;

class ProductSTSStateTop : public STSState<ProductSTSStateTop>
{
public:
    ProductSTSStateTop( ProductSTSHsm& hsm,
                        CHsmState *pSuperState,
                        ProductSTSAccount& account );
    ~ProductSTSStateTop() override {}

    bool HandleSelectContentItem( const STS::ContentItem & ) override;

    ////////////////////////////////////////////////////////
    /// @brief Act on Activate request - send error response for wrong state
    /// @param ActivateRequest - Proto buf of Activate message (unused)
    /// @param seq - transaction sequence number
    /// @return always true
    ////////////////////////////////////////////////////////
    bool HandleActivateRequest( const STS::Void &, uint32_t ) override;

    ////////////////////////////////////////////////////////
    /// @brief Act on Deactivate request - send error response for wrong state
    /// @param DeactivateRequest - Proto buf of Deactivate message (unused)
    /// @param seq - transaction sequence number
    /// @return always true
    ////////////////////////////////////////////////////////
    bool HandleDeactivateRequest( const STS::DeactivateRequest &, uint32_t ) override;

    ////////////////////////////////////////////////////////
    /// @brief Act on NowPlaying request - send error response for wrong state
    /// @param NowPlayingRequest - Proto buf of NowPlaying message
    /// @param seq - transaction sequence number
    /// @return always true
    ////////////////////////////////////////////////////////
    bool HandleNowPlayingRequest( const STS::Void &, uint32_t ) override;

    ////////////////////////////////////////////////////////
    /// @brief Act on Introspect request - send response
    /// @param IntrospectRequest - Proto buf of Introspect message
    /// @param seq - transaction sequence number
    /// @return always true
    ////////////////////////////////////////////////////////
    bool HandleIntrospectRequest( const STS::Void&, uint32_t seq ) override;

    bool GetActive() const
    {
        return m_active;
    };

protected:
    DPrint m_logger;
    ProductSTSAccount& m_account;
    STS::NowPlaying m_np;
private:
    bool    m_active;
};
