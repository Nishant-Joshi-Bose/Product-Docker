////////////////////////////////////////////////////////////////////////////////
/// @file   ProductSource.h
/// @brief  Product Source class.
///
/// @attention Copyright 2017 Bose Corporation, Framingham, MA
////////////////////////////////////////////////////////////////////////////////
//
#pragma once

#include "NotifyTargetTaskIF.h"
#include "FrontDoorClientIF.h"
#include "AsyncCallback.h"
#include "SoundTouchInterface/ContentSelectionService.pb.h"
#include "SoundTouchInterface/PlayerService.pb.h"

namespace ProductApp
{
class ProductSource
{
public:
    ProductSource( const std::shared_ptr<FrontDoorClientIF>& frontDoorClientIF,
                   NotifyTargetTaskIF& task );
    virtual ~ProductSource();

public:
    void Initialize();

////////////////////////////////////////////////////////////////////////////////
/// @name   SendPostAUXPlaybackRequest
/// @brief- Sends a POST request for AUX playback to CAPS
/// @return void
////////////////////////////////////////////////////////////////////////////////
    void SendPostAUXPlaybackRequest();

public:
    //ToDo This code will be removed after integrating new KeyHandler component.
    enum KEY_ID
    {
        SELECT_AUX_SOURCE = 3
    };

private:
    /// Disable copies
    ProductSource( const ProductSource& ) = delete;
    ProductSource& operator=( const ProductSource& ) = delete;

private:
////////////////////////////////////////////////////////////////////////////////
/// @name   PostAUXPlaybackRequestResponse
/// @brief- Handles NowPlaying response of AUX playback POST request
/// @return void
////////////////////////////////////////////////////////////////////////////////
    void PostAUXPlaybackRequestResponse( const SoundTouchInterface::NowPlayingJson& resp );

////////////////////////////////////////////////////////////////////////////////
/// @name   AUXPlaybackRequestCallbackError
/// @brief- Handles error response of AUX playback POST request
/// @return void
////////////////////////////////////////////////////////////////////////////////
    void AUXPlaybackRequestCallbackError( const FRONT_DOOR_CLIENT_ERRORS errorCode );

private:
    std::shared_ptr<FrontDoorClientIF>                  m_FrontDoorClientIF;
    NotifyTargetTaskIF&                                 m_ProductControllerTask;

    AsyncCallback<FRONT_DOOR_CLIENT_ERRORS>             errorCb;
    AsyncCallback<SoundTouchInterface::NowPlayingJson>  postAUXPlaybackRequestResponseCb;
};
} //namespace ProductApp

