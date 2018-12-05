////////////////////////////////////////////////////////////////////////////////
/// @file     ProductSTSStateDeviceControl.h
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

    bool HandleActivateRequest( const STS::Void &, uint32_t seq ) override;
    bool HandleDeactivateRequest( const STS::DeactivateRequest &, uint32_t seq ) override;

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

private:
    DeviceController::DeviceControllerClientIF::DeviceControllerClientPtr m_deviceControllerPtr;
};
}
