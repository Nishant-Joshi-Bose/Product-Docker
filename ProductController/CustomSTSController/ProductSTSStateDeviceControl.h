////////////////////////////////////////////////////////////////////////////////
/// @file     ControlIntegrationSTSStateTop.h
/// @brief    STS interface class for the state used for PRODUCT sources.
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ProductSTSStateTop.h"
#include "DeviceControllerClientFactory.h"

namespace ProductApp
{
class ProductSTSStateDeviceControl : public ProductSTSStateTop
{
public:
    ProductSTSStateDeviceControl( ProductSTSHsm& hsm,
                                  CHsmState *pSuperState,
                                  ProductSTSAccount& account );
    ~ProductSTSStateDeviceControl() override {}

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
    /// @brief Act on SelectContentItem request - send error response for wrong state
    /// @param ContentItem - Proto buf contains the ContentItem
    /// @return always true
    ////////////////////////////////////////////////////////
    bool HandleSelectContentItem( const STS::ContentItem & ) override;

    ////////////////////////////////////////////////////////
    /// @brief Act on Introspect request - send response
    /// @param IntrospectRequest - Proto buf of Introspect message
    /// @param seq - transaction sequence number
    /// @return always true
    ////////////////////////////////////////////////////////
    bool HandleIntrospectRequest( const STS::Void&, uint32_t seq ) override;


    ////////////////////////////////////////////////////////
    /// Below are functions we need to proxy to the
    ///     DeviceController
    ////////////////////////////////////////////////////////

    bool HandlePlay( const STS::Void & ) override;
    bool HandlePause( const STS::Void & ) override;
    bool HandleStop( const STS::Void & ) override;
    bool HandleSkipNext( const STS::Void & ) override;
    bool HandleSkipPrevious( const STS::Void & ) override;
    bool HandleMuteStatus( const STS::MuteStatus & ) override;
    bool HandlePowerOff( const STS::Void & ) override;
    bool HandleSearchRequest( const STS::SearchRequest &, uint32_t ) override;
    bool HandleSeekToTime( const STS::SeekToTime & ) override;

    bool GetActive() const
    {
        return m_active;
    };

    void SetResumeSupported( bool resumeSupported )
    {
        m_resumeSupported = resumeSupported;
    };

    virtual const std::string& GetURL( ) const;

private:
    DeviceController::DeviceControllerClientIF::DeviceControllerClientPtr m_deviceControllerPtr;

};
}
