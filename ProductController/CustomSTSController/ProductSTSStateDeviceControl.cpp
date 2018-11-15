////////////////////////////////////////////////////////////////////////////////
/// @file     ProductSTSStateDeviceControl.cpp
/// @brief    STS interface class for the state used for PRODUCT sources.
////////////////////////////////////////////////////////////////////////////////

#include "Utilities.h"
#include "ProductSTSStateDeviceControl.h"
#include "ProductSTSController.h"
#include "DeviceControllerClientMessages.pb.h"

namespace ProductApp
{

//////////////////////////////////////////////////////////////
ProductSTSStateDeviceControl::ProductSTSStateDeviceControl( ProductSTSHsm& hsm,
                                                            CHsmState *pSuperState,
                                                            ProductSTSAccount& account ) :
    ProductSTSStateTop( hsm, pSuperState, account ),
    m_deviceControllerPtr( account.GetProductSTSController()->GetProductController().GetDeviceControllerClient() )

{
    //InitializeDeviceControllerClient();
}


void ProductSTSStateDeviceControl::InitializeDeviceControllerClient()
{
    BOSE_INFO( m_logger, "%s", __func__ );

    m_deviceControllerPtr = DeviceControllerClientFactory::Create( "ProductSTSStateDeviceControl",
                                                                   m_account.GetProductSTSController()->GetProductController().GetTask() );

    m_deviceControllerPtr->Connect( [ ]( bool connected ) { } );
}

// @TODO what is the proper value here? Is it product-specific? CASTLE-5047 https://jirapro.bose.com/browse/PAELLA-9910
static constexpr uint32_t LOW_LATENCY_DELAYED_START_MS = 25;

bool ProductSTSStateDeviceControl::HandleActivateRequest( const STS::Void &, uint32_t seq )
{
    BOSE_INFO( m_logger, "%s( %s )", __func__, m_account.GetSourceName().c_str() );
    m_account.IPC().SendActivateResponse( seq );

    if( !m_active )
    {
        const std::string& URL = GetURL( );
        if( !URL.empty( ) )
        {
            STS::AudioSetURL asu;
            asu.set_url( URL );
            asu.set_startoffsetms( LOW_LATENCY_DELAYED_START_MS );
            m_account.IPC().SendAudioSetURLEvent( asu );
        }

        const auto source = m_account.GetProductSTSController()->GetProductController().GetSourceInfo().FindSource( m_account.GetSourceContentItem() );
        if( source->status() == SoundTouchInterface::SourceStatus::AVAILABLE )
        {
            m_np.set_canpause( true );
            m_np.set_canstop( true );
            m_np.set_skipenabled( true );           //skip next
            m_np.set_skippreviousenabled( true );   //skip previous
            m_np.set_seeksupported( true );
        }
        else
        {
            m_np.set_canpause( false );
            m_np.set_canstop( false );
            m_np.set_skipenabled( false );           //skip next
            m_np.set_skippreviousenabled( false );   //skip previous
            m_np.set_seeksupported( false );
        }

        m_np.set_playstatus( STS::PlayStatus::PLAY );
        STS::NowPlayingChange npc;
        *( npc.mutable_nowplaying() ) = m_np;
        m_account.IPC().SendNowPlayingChangeEvent( npc );
        m_active = true;
    }
    return true;
}

bool ProductSTSStateDeviceControl::HandleNowPlayingRequest( const STS::Void &, uint32_t seq )
{
    BOSE_INFO( m_logger, "%s( %s ) while %sactive", __func__, m_account.GetSourceName().c_str(),
               m_active ? "" : "in" );
    if( m_active )
    {
        STS::NowPlayingResponse npr;
        *( npr.mutable_nowplaying() ) = m_np;
        m_account.IPC().SendNowPlayingResponse( npr, seq );
    }
    else
    {
        STS::AsyncError e;
        e.set_error( SoundTouchInterface::ErrorType::MUSIC_SERVICE_WRONG_STATE_NOW_PLAYING_REQUEST );
        e.set_desc( m_account.GetSourceName() + "RcvdNowPlayingInWrongState" );
        m_account.IPC().SendNowPlayingFailureResponse( e, seq );
    }
    return true;
}

bool ProductSTSStateDeviceControl::HandleSelectContentItem( const STS::ContentItem  &contentItem )
{
    BOSE_INFO( m_logger, "%s( %s )", __func__, m_account.GetSourceName().c_str() );
    // Need to tell CAPS to proceed with the source selection
    m_account.IPC().SendSourceSelectEvent();
    *( m_np.mutable_contentitem() ) = contentItem;
    m_np.mutable_contentitem()->set_ispresetable( false );
    m_np.set_resumesupported( m_resumeSupported );
    m_account.GetProductSTSController()->HandlePlaybackRequestSlot( m_account.GetSourceID() );
    return true;
}

bool ProductSTSStateDeviceControl::HandleDeactivateRequest( const STS::DeactivateRequest &, uint32_t seq )
{
    BOSE_INFO( m_logger, "%s - HandleDeactivateRequest( %s )", __func__, m_account.GetSourceName().c_str() );
    m_account.IPC().SendAudioStopEvent();
    m_account.IPC().SendDeactivateResponse( seq );
    m_np.set_playstatus( STS::PlayStatus::STOP );
    m_active = false;
    return true;
}

////////////////////////////////////////////////////////
bool ProductSTSStateDeviceControl::HandleIntrospectRequest( const STS::Void&, uint32_t seq )
{
    m_account.IPC().SendIntrospectResponse( ProtoToMarkup::ToXML( m_account.Introspect() ) , seq );
    return true;
}

////////////////////////////////////////////////////////
///
/// The base class version returns the PRODUCT source URL
///
////////////////////////////////////////////////////////
const std::string& ProductSTSStateDeviceControl::GetURL( ) const
{
    return s_productAudioURL;
}

bool ProductSTSStateDeviceControl::HandlePlay( const STS::Void & )
{
    BOSE_INFO( s_logger, "%s( %s )", __func__, m_account.GetSourceName().c_str() );

    DeviceControllerClientMessages::TranportControlMessage_t request;
    request.set_request( DevicePlaybackControl::PLAYBACK_CTRL_PLAY );

    m_deviceControllerPtr->SendTransportControlRequest( request );
    //m_account.GetProductSTSController()->GetProductController().GetDeviceControllerClient()->SendTransportControlRequest( request );
    return true;
}

bool ProductSTSStateDeviceControl::HandlePause( const STS::Void & )
{
    BOSE_INFO( s_logger, "%s( %s )", __func__, m_account.GetSourceName().c_str() );

    DeviceControllerClientMessages::TranportControlMessage_t request;
    request.set_request( DevicePlaybackControl::PLAYBACK_CTRL_PAUSE );

    m_deviceControllerPtr->SendTransportControlRequest( request );
    return true;
}

bool ProductSTSStateDeviceControl::HandleStop( const STS::Void & )
{
    BOSE_INFO( s_logger, "%s( %s )", __func__, m_account.GetSourceName().c_str() );

    DeviceControllerClientMessages::TranportControlMessage_t request;
    request.set_request( DevicePlaybackControl::PLAYBACK_CTRL_STOP );

    m_deviceControllerPtr->SendTransportControlRequest( request );
    return true;
}

bool ProductSTSStateDeviceControl::HandleSkipNext( const STS::Void & )
{
    BOSE_INFO( s_logger, "%s( %s )", __func__, m_account.GetSourceName().c_str() );

    DeviceControllerClientMessages::TranportControlMessage_t request;
    request.set_request( DevicePlaybackControl::PLAYBACK_CTRL_NEXT );

    m_deviceControllerPtr->SendTransportControlRequest( request );
    return true;
}

bool ProductSTSStateDeviceControl::HandleSkipPrevious( const STS::Void & )
{
    BOSE_INFO( s_logger, "%s( %s )", __func__, m_account.GetSourceName().c_str() );

    DeviceControllerClientMessages::TranportControlMessage_t request;
    request.set_request( DevicePlaybackControl::PLAYBACK_CTRL_PREVIOUS );

    m_deviceControllerPtr->SendTransportControlRequest( request );
    return true;
}

bool ProductSTSStateDeviceControl::HandleMuteStatus( const STS::MuteStatus & )
{
    BOSE_INFO( s_logger, "%s( %s )", __func__, m_account.GetSourceName().c_str() );
    return false;
}

bool ProductSTSStateDeviceControl::HandlePowerOff( const STS::Void & )
{
    BOSE_INFO( s_logger, "%s( %s )", __func__, m_account.GetSourceName().c_str() );
    return false;
}

bool ProductSTSStateDeviceControl::HandleSearchRequest( const STS::SearchRequest &, uint32_t )
{
    BOSE_INFO( s_logger, "%s( %s )", __func__, m_account.GetSourceName().c_str() );
    return false;
}

bool ProductSTSStateDeviceControl::HandleSeekToTime( const STS::SeekToTime & )
{
    BOSE_INFO( s_logger, "%s( %s )", __func__, m_account.GetSourceName().c_str() );
    return false;
}
}
