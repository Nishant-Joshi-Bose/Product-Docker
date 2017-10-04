////////////////////////////////////////////////////////////////////////////////
/// @file     ProductSTSAccount.cpp
/// @brief
/// @author   Yishai Sered
/// @date      Creation Date: 8/18/2016
///
/// Copyright 2016 Bose Corporation
////////////////////////////////////////////////////////////////////////////////

#include "ProductSTSController.h"
#include "BreakThread.h"

ProductSTSController* ProductSTSAccount::s_ProductSTSController;
std::string ProductSTSAccount::s_productAudioURL = "aux://";

//////////////////////////////////////////////////////////////
ProductSTSAccount::ProductSTSAccount( const std::string& name, ProductSTS::ProductSourceSlot source, bool enabled ) :
    STSAccount( m_hsm, "", name, SHELBY_SOURCE::PRODUCT, 0 ),
    m_logger( "ProductSTSController" ),
    m_hsm( "ProductSTSAccount", m_pTask ),
    m_source( source ),
    m_name( name ),
    m_state( m_hsm, NULL, *this ),
    m_enabled( enabled )
{
    BOSE_INFO( m_logger, "ProductSTSAccount::ProductSTSAccount - account name=%s, source=%d, %s",
               GetSourceName().c_str(), GetSourceID(), GetEnabled() ? "enabled" : "disabled" );
    m_hsm.AddState( &m_state );
    m_hsm.Init( 0 );
    m_contentItem.set_source( SHELBY_SOURCE::PRODUCT );
    m_contentItem.set_sourceaccount( m_name );
    m_contentItem.set_ispresetable( false );
}

//////////////////////////////////////////////////////////////
ProductSTSAccount::~ProductSTSAccount()
{
}

//////////////////////////////////////////////////////////////
void ProductSTSAccount::Initialize( const int proxyPort )
{
    BOSE_INFO( m_logger, "ProductSTSAccount::Initialize(%s) - account(port:%d)\n", m_name.c_str(), proxyPort );
    auto func = std::bind( &ProductSTSAccount::HandleProxyConnectResponse, this );
    AsyncCallback<void> cb( func, m_pTask );
    STSAccount<ProductSTSStateTop>::ProxyConnect( cb, proxyPort );
}

//////////////////////////////////////////////////////////////
void ProductSTSAccount::HandleProxyConnectResponse()
{
    IL::BreakThread( std::bind( &ProductSTSAccount::ConnectAction,
                                this ),
                     m_pTask );
}

//////////////////////////////////////////////////////////////
void ProductSTSAccount::ConnectAction()
{
    // tell STS whether we are enabled or disabled
    SetEnabledAction( m_enabled, true );
}

//////////////////////////////////////////////////////////////
void ProductSTSAccount::SetEnabled( bool enabled, bool force )
{
    IL::BreakThread( std::bind( &ProductSTSAccount::SetEnabledAction,
                                this,
                                enabled, force ),
                     m_pTask );
}

//////////////////////////////////////////////////////////////
void ProductSTSAccount::SetEnabledAction( bool enabled, bool force )
{
    BOSE_INFO( m_logger, "ProductSTSAccount::SetEnabled(%s) - %s", m_name.c_str(), enabled ? "READY" : "UNAVAILABLE" );
    if( force || m_enabled != enabled )
    {
        m_enabled = enabled;
        IPC().SendStatusEvent( enabled ? STS::StatusEnum::READY : STS::StatusEnum::UNAVAILABLE );
    }
}

//////////////////////////////////////////////////////////////
void ProductSTSAccount::PlayProductAudio()
{
    IL::BreakThread( std::bind( &ProductSTSAccount::PlayProductAudioAction,
                                this ),
                     m_pTask );
}

static const uint32_t LOW_LATENCY_DELAYED_START_MS = 25;

//////////////////////////////////////////////////////////////
void ProductSTSAccount::PlayProductAudioAction()
{
    BOSE_INFO( m_logger, "ProductSTSAccount::PlayProductAudioAction" );
    STS::AudioSetURL asu;
    asu.set_url( s_productAudioURL );
    asu.set_startoffsetms( LOW_LATENCY_DELAYED_START_MS );
    IPC().SendAudioSetURLEvent( asu );
}

//////////////////////////////////////////////////////////////
void ProductSTSAccount::HandleSelectSourceSlot( ProductSTS::ProductSourceSlot source )
{
    s_ProductSTSController->HandleSelectSourceSlot( source );
}

//////////////////////////////////////////////////////////////
ProductSTSIntrospect::ProductSTSAccountIntrospectResponse ProductSTSAccount::Introspect()
{
    ProductSTSIntrospect::ProductSTSAccountIntrospectResponse accountIntrospectResponse;
    accountIntrospectResponse.set_account( m_name );
    accountIntrospectResponse.set_enabled( m_enabled );
    accountIntrospectResponse.set_active( m_state.GetActive() );

    return accountIntrospectResponse;
}

