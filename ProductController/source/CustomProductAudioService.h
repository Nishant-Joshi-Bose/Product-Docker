///////////////////////////////////////////////////////////////////////////////////////////////////
/// @file   CustomProductAudioService.h
/// @brief   This file contains source code for Professor specific behavior for
///         communicating with APProduct Server and APProduct related FrontDoor interaction
/// Copyright 2017 Bose Corporation
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include "ProductAudioService.h"

namespace ProductApp
{
class CustomProductAudioService: public ProductAudioService
{
public:
    bool Run();
    static CustomProductAudioService* GetInstance( NotifyTargetTaskIF*        task,
                                                   Callback< ProductMessage > ProductNotify );

    ~CustomProductAudioService();
private:
    CustomProductAudioService( NotifyTargetTaskIF*        task,
                               Callback< ProductMessage > ProductNotify );

    //////////////////////////////////////////////////////////////////////////////////////////////
    /// Front Door handler
    /////////////////////////////////////////////////////////////////////////////////////////////
    std::shared_ptr<FrontDoorClientIF> m_FrontDoorClientIF;
    //audio/bass
    CallbackConnection m_registerGetBassCb;
    CallbackConnection m_registerPostBassCb;
    CallbackConnection m_registerPutBassCb;
    //audio/treble
    CallbackConnection m_registerGetTrebleCb;
    CallbackConnection m_registerPostTrebleCb;
    CallbackConnection m_registerPutTrebleCb;
    //audio/center
    CallbackConnection m_registerGetCenterCb;
    CallbackConnection m_registerPostCenterCb;
    CallbackConnection m_registerPutCenterCb;
    //audio/surround
    CallbackConnection m_registerGetSurroundCb;
    CallbackConnection m_registerPostSurroundCb;
    CallbackConnection m_registerPutSurroundCb;
    //audio/gainOffset
    CallbackConnection m_registerGetGainOffsetCb;
    CallbackConnection m_registerPostGainOffsetCb;
    CallbackConnection m_registerPutGainOffsetCb;
    //audio/avSync
    CallbackConnection m_registerGetAvSyncCb;
    CallbackConnection m_registerPostAvSyncCb;
    CallbackConnection m_registerPutAvSyncCb;
    //audio/mode
    CallbackConnection m_registerGetModeCb;
    CallbackConnection m_registerPostModeCb;
    CallbackConnection m_registerPutModeCb;
    //audio/contentType
    CallbackConnection m_registerGetContentTypeCb;
    CallbackConnection m_registerPostContentTypeCb;
    CallbackConnection m_registerPutContentTypeCb;

    /////////////////////////////////////////////////////////////////////////////////////////////////
    /// APProduct handling functions
    /////////////////////////////////////////////////////////////////////////////////////////////////
    void RegisterAudioPathEvent();

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// FrontDoor callback functions
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    void RegisterFrontDoorEvent();
    //audio/bass
    void BassGetHandler( const Callback<ProductPb::AudioBassLevel> &resp );
    void BassPostHandler( const ProductPb::AudioBassLevel &req, const Callback<ProductPb::AudioBassLevel> &resp );
    void BassPutHandler( const ProductPb::AudioBassLevel &req, const Callback<ProductPb::AudioBassLevel> &resp );
    //audio/treble
    void TrebleGetHandler( const Callback<ProductPb::AudioTrebleLevel> &resp );
    void TreblePostHandler( const ProductPb::AudioTrebleLevel &req, const Callback<ProductPb::AudioTrebleLevel> &resp );
    void TreblePutHandler( const ProductPb::AudioTrebleLevel &req, const Callback<ProductPb::AudioTrebleLevel> &resp );
    //audio/center
    void CenterGetHandler( const Callback<ProductPb::AudioCenterLevel> &resp );
    void CenterPostHandler( const ProductPb::AudioCenterLevel &req, const Callback<ProductPb::AudioCenterLevel> &resp );
    void CenterPutHandler( const ProductPb::AudioCenterLevel &req, const Callback<ProductPb::AudioCenterLevel> &resp );
    //audio/surround
    void SurroundGetHandler( const Callback<ProductPb::AudioSurroundLevel> &resp );
    void SurroundPostHandler( const ProductPb::AudioSurroundLevel &req, const Callback<ProductPb::AudioSurroundLevel> &resp );
    void SurroundPutHandler( const ProductPb::AudioSurroundLevel &req, const Callback<ProductPb::AudioSurroundLevel> &resp );
    //audio/gainOffset
    void GainOffsetGetHandler( const Callback<ProductPb::AudioGainOffset> &resp );
    void GainOffsetPostHandler( const ProductPb::AudioGainOffset &req, const Callback<ProductPb::AudioGainOffset> &resp );
    void GainOffsetPutHandler( const ProductPb::AudioGainOffset &req, const Callback<ProductPb::AudioGainOffset> &resp );
    //audio/avSync
    void AvSyncGetHandler( const Callback<ProductPb::AudioAvSync> &resp );
    void AvSyncPostHandler( const ProductPb::AudioAvSync &req, const Callback<ProductPb::AudioAvSync> &resp );
    void AvSyncPutHandler( const ProductPb::AudioAvSync &req, const Callback<ProductPb::AudioAvSync> &resp );
    //audio/mode
    void ModeGetHandler( const Callback<ProductPb::AudioMode> &resp );
    void ModePostHandler( const ProductPb::AudioMode &req, const Callback<ProductPb::AudioMode> &resp );
    void ModePutHandler( const ProductPb::AudioMode &req, const Callback<ProductPb::AudioMode> &resp );
    //audio/contentType
    void ContentTypeGetHandler( const Callback<ProductPb::AudioContentType> &resp );
    void ContentTypePostHandler( const ProductPb::AudioContentType &req, const Callback<ProductPb::AudioContentType> &resp );
    void ContentTypePutHandler( const ProductPb::AudioContentType &req, const Callback<ProductPb::AudioContentType> &resp );

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Clean up function
    //////////////////////////////////////////////////////////////////////////////////////////////////
    void DisconnectFrontDoor();
};
}// namespace ProductApp
