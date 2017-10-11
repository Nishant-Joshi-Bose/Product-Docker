////////////////////////////////////////////////////////////////////////////////
/// @file   ProductSource.cpp
/// @brief  Product Source class.
///
/// @attention Copyright 2017 Bose Corporation, Framingham, MA
////////////////////////////////////////////////////////////////////////////////

#include "ProductSource.h"
#include "DPrint.h"

static DPrint s_logger( "ProductSource" );

namespace ProductApp
{
ProductSource::ProductSource( const std::shared_ptr<FrontDoorClientIF>& frontDoorClientIF,
                              NotifyTargetTaskIF& task ):
    m_FrontDoorClientIF( frontDoorClientIF ),
    m_ProductControllerTask( task ),
    errorCb( nullptr, &m_ProductControllerTask ),
    postAUXPlaybackRequestResponseCb( nullptr, &m_ProductControllerTask )
{
    BOSE_INFO( s_logger, __func__ );
}

ProductSource::~ProductSource()
{
    BOSE_INFO( s_logger, __func__ );
}

void ProductSource::Initialize()
{
    BOSE_DEBUG( s_logger, __func__ );

    errorCb = AsyncCallback<FRONT_DOOR_CLIENT_ERRORS> ( std::bind( &ProductSource::AUXPlaybackRequestCallbackError,
                                                                   this, std::placeholders::_1 ), &m_ProductControllerTask );

    postAUXPlaybackRequestResponseCb = AsyncCallback<SoundTouchInterface::NowPlayingJson> ( std::bind( &ProductSource::PostAUXPlaybackRequestResponse,
                                       this, std::placeholders::_1 ), &m_ProductControllerTask );
}

void ProductSource::SendPostAUXPlaybackRequest()
{
    //Setup the playbackRequest data for AUX source
    SoundTouchInterface::playbackRequestJson playbackRequestData;
    playbackRequestData.set_source( "AUX" );
    playbackRequestData.set_sourceaccount( "AUX" );

    //Send POST for /content/playbackRequest
    m_FrontDoorClientIF->SendPost<SoundTouchInterface::NowPlayingJson>( "/content/playbackRequest", playbackRequestData,
                                                                        postAUXPlaybackRequestResponseCb, errorCb );
}

void ProductSource::PostAUXPlaybackRequestResponse( const SoundTouchInterface::NowPlayingJson& resp )
{
    BOSE_DEBUG( s_logger, __func__ );
    BOSE_LOG( INFO, "GOT Response to AUX playbackRequest: " << resp.source().sourcedisplayname() );
}

void ProductSource::AUXPlaybackRequestCallbackError( const FRONT_DOOR_CLIENT_ERRORS errorCode )
{
    BOSE_ERROR( s_logger, "%s:error code- %d", __func__, errorCode );
}
} //namespace ProductApp

