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
