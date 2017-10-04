////////////////////////////////////////////////////////////////////////////////
/// @file     ProductSTSAccount.h
/// @brief
/// @author   Yishai Sered
/// @date      Creation Date: 8/18/2016
///
/// Copyright 2016 Bose Corporation
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "STSAccount.h"
#include "DPrint.h"
#include "ProductSTSHsm.h"
#include "ProductSTSStateTop.h"
#include "ProductSTSIntrospect.pb.h"
#include "ProductSTS.pb.h"

class ProductSTSController;

class ProductSTSAccount : public STSAccount<ProductSTSStateTop>
{
public:
    ProductSTSAccount( const std::string& name, ProductSTS::ProductSourceSlot source, bool enabled = false );
    ~ProductSTSAccount() override;

    void Initialize( const int proxyPort );
    void HandleProxyConnectResponse();
    void SetEnabled( bool enabled, bool force = false );
    void PlayProductAudio();
    bool GetEnabled() const
    {
        return m_enabled;
    };

    ProductSTS::ProductSourceSlot GetSourceID() const
    {
        return m_source;
    };
    const std::string& GetSourceName() const
    {
        return m_name;
    };
    const SoundTouchInterface::ContentItem& GetSourceContentItem() const
    {
        return m_contentItem;
    };

    ProductSTSIntrospect::ProductSTSAccountIntrospectResponse Introspect();

    static void SetProductSTSController( ProductSTSController* ProductSTSController )
    {
        s_ProductSTSController = ProductSTSController;
    };
    static void HandleSelectSourceSlot( ProductSTS::ProductSourceSlot source );

    static ProductSTSController* s_ProductSTSController;

private:
    void ConnectAction();
    void SetEnabledAction( bool enabled, bool force );
    void PlayProductAudioAction();

private:
    DPrint                      m_logger;
    ProductSTSHsm               m_hsm;
    ProductSTS::ProductSourceSlot   m_source;
    const std::string           m_name;
    ProductSTSStateTop          m_state;
    bool                        m_enabled;
    SoundTouchInterface::ContentItem m_contentItem;
    static std::string          s_productAudioURL;
};
