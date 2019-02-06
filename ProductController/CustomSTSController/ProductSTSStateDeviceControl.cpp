////////////////////////////////////////////////////////////////////////////////
/// @file     ProductSTSStateDeviceControl.cpp
/// @brief    STS interface class for the state used for PRODUCT sources.
////////////////////////////////////////////////////////////////////////////////

#include "Utilities.h"
#include "ProductSTSStateDeviceControl.h"
#include "ProductSTSController.h"
#include "DeviceControllerClientMessages.pb.h"
#include "CustomProductController.h"

namespace ProductApp
{

//////////////////////////////////////////////////////////////
ProductSTSStateDeviceControl::ProductSTSStateDeviceControl( ProductSTSHsm& hsm,
                                                            CHsmState *pSuperState,
                                                            ProductSTSAccount& account ) :
    ProductSTSStateTop( hsm, pSuperState, account ),
    m_deviceControllerPtr( ( static_cast<CustomProductController&>( account.GetProductSTSController()->GetProductController() ) ).GetDeviceControllerClient() )

{
}

// @TODO what is the proper value here? Is it product-specific? CASTLE-5047 https://jirapro.bose.com/browse/PAELLA-9910
static constexpr uint32_t LOW_LATENCY_DELAYED_START_MS = 25;

bool ProductSTSStateDeviceControl::HandleActivateRequest( const STS::Void & request, uint32_t seq )
{
    BOSE_INFO( s_logger, "%s( %s ): %s", __func__, m_account.GetSourceName().c_str(), request.DebugString().c_str() );

    const auto source = m_account.GetProductSTSController()->GetProductController().GetSourceInfo().FindSource( m_account.GetSourceContentItem() );
    if( source != nullptr && source->has_accountid() )
    {
        // Form activation callback
        auto cb = [ this, seq ]( DeviceControllerClientMessages::DeviceActivationRequest_t req )
        {
            m_account.IPC().SendActivateResponse( seq );
        };
        AsyncCallback< DeviceControllerClientMessages::DeviceActivationRequest_t >
        activationCb( cb, m_account.GetProductSTSController()->GetProductController().GetTask() );
        // Send activation request
        DeviceControllerClientMessages::DeviceActivationRequest_t request;
        request.set_activation( true );
        request.set_id( source->accountid() );
        m_deviceControllerPtr->SendDeviceActivationRequest( request, activationCb );

        // Update nowPlaying to STS
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
            // set friendly name in nowPlaying
            m_np.mutable_contentitem()->set_name( source->details().friendlyname() );

            m_np.set_playstatus( STS::PlayStatus::PLAY );
            STS::NowPlayingChange npc;
            *( npc.mutable_nowplaying() ) = m_np;
            m_account.IPC().SendNowPlayingChangeEvent( npc );
            m_active = true;
        }
    }
    else
    {
        m_account.IPC().SendActivateResponse( seq );
        //TODO: what would be cases where accountId is empty
    }

    return true;
}

bool ProductSTSStateDeviceControl::HandleDeactivateRequest( const STS::DeactivateRequest & request, uint32_t seq )
{
    BOSE_INFO( s_logger, "%s( %s ): request = %s", __func__, m_account.GetSourceName().c_str(), request.DebugString().c_str() );

    const auto source = m_account.GetProductSTSController()->GetProductController().GetSourceInfo().FindSource( m_account.GetSourceContentItem() );
    if( source != nullptr && source->has_accountid() )
    {
        // Form dectivation callback
        auto cb = [ this, seq ]( DeviceControllerClientMessages::DeviceActivationRequest_t req )
        {
            m_account.IPC().SendAudioStopEvent();
            m_account.IPC().SendDeactivateResponse( seq );
            m_np.set_playstatus( STS::PlayStatus::STOP );
            m_active = false;
        };
        AsyncCallback< DeviceControllerClientMessages::DeviceActivationRequest_t >
        activationCb( cb, m_account.GetProductSTSController()->GetProductController().GetTask() );
        // Send activation request
        DeviceControllerClientMessages::DeviceActivationRequest_t request;
        request.set_activation( false );
        request.set_id( source->accountid() );
        m_deviceControllerPtr->SendDeviceActivationRequest( request, activationCb );
    }
    else
    {
        m_account.IPC().SendDeactivateResponse( seq );
    }
    return true;
}

bool ProductSTSStateDeviceControl::HandleSelectContentItem( const STS::ContentItem  &contentItem )
{
    BOSE_INFO( s_logger, "%s( %s ): contentItem = %s", __func__, m_account.GetSourceName().c_str(), contentItem.DebugString().c_str() );
    // Need to tell CAPS to proceed with the source selection
    m_account.IPC().SendSourceSelectEvent();
    *( m_np.mutable_contentitem() ) = contentItem;
    m_np.mutable_contentitem()->set_ispresetable( false );
    m_np.set_resumesupported( m_resumeSupported );
    m_account.GetProductSTSController()->HandlePlaybackRequestSlot( m_account.GetSourceID() );

    if( !contentItem.location().empty() )
    {
        DeviceControllerClientMessages::DeviceSelectAdditionalInfo_t additionalInfo;
        additionalInfo.set_info( contentItem.location() );
        m_deviceControllerPtr->SendSelectAdditionalInfo( additionalInfo );
    }
    return true;
}

bool ProductSTSStateDeviceControl::HandlePlay( const STS::Void & )
{
    BOSE_INFO( s_logger, "%s( %s )", __func__, m_account.GetSourceName().c_str() );

    DeviceControllerClientMessages::TranportControlMessage_t request;
    request.set_request( DeviceControllerClientMessages::DevicePlaybackControl::PLAYBACK_CTRL_PLAY );

    m_deviceControllerPtr->SendTransportControlRequest( request );
    return true;
}

bool ProductSTSStateDeviceControl::HandlePause( const STS::Void & )
{
    BOSE_INFO( s_logger, "%s( %s )", __func__, m_account.GetSourceName().c_str() );

    DeviceControllerClientMessages::TranportControlMessage_t request;
    request.set_request( DeviceControllerClientMessages::DevicePlaybackControl::PLAYBACK_CTRL_PAUSE );

    m_deviceControllerPtr->SendTransportControlRequest( request );
    return true;
}

bool ProductSTSStateDeviceControl::HandleStop( const STS::Void & )
{
    BOSE_INFO( s_logger, "%s( %s )", __func__, m_account.GetSourceName().c_str() );

    DeviceControllerClientMessages::TranportControlMessage_t request;
    request.set_request( DeviceControllerClientMessages::DevicePlaybackControl::PLAYBACK_CTRL_STOP );

    m_deviceControllerPtr->SendTransportControlRequest( request );
    return true;
}

bool ProductSTSStateDeviceControl::HandleSkipNext( const STS::Void & )
{
    BOSE_INFO( s_logger, "%s( %s )", __func__, m_account.GetSourceName().c_str() );

    DeviceControllerClientMessages::TranportControlMessage_t request;
    request.set_request( DeviceControllerClientMessages::DevicePlaybackControl::PLAYBACK_CTRL_NEXT );

    m_deviceControllerPtr->SendTransportControlRequest( request );
    return true;
}

bool ProductSTSStateDeviceControl::HandleSkipPrevious( const STS::Void & )
{
    BOSE_INFO( s_logger, "%s( %s )", __func__, m_account.GetSourceName().c_str() );

    DeviceControllerClientMessages::TranportControlMessage_t request;
    request.set_request( DeviceControllerClientMessages::DevicePlaybackControl::PLAYBACK_CTRL_PREVIOUS );

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
