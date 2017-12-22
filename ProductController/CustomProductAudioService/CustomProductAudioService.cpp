///////////////////////////////////////////////////////////////////////////////////////////////////
/// @file   CustomProductAudioService.cpp
/// @brief   This file contains source code for Eddie specific behavior for
///         communicating with APProduct Server and APProduct related FrontDoor interaction
/// Copyright 2017 Bose Corporation
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "DPrint.h"
#include "AsyncCallback.h"
#include "APProductFactory.h"
#include "FrontDoorClient.h"
#include "CustomProductAudioService.h"

static DPrint s_logger( "CustomProductAudioService" );

constexpr char kBassEndPoint            [] = "/audio/bass";
constexpr char kTrebleEndPoint          [] = "/audio/treble";

namespace ProductApp
{
////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   CustomProductAudioService::CustomProductAudioService
///
/// @param  EddieProductController& ProductController
///
////////////////////////////////////////////////////////////////////////////////////////////////////
CustomProductAudioService::CustomProductAudioService( EddieProductController& ProductController, const FrontDoorClientIF_t& FrontDoorClient ):
    ProductAudioService( ProductController.GetTask( ),
                         ProductController.GetMessageHandler() ),
    m_AudioSettingsMgr( std::unique_ptr<CustomAudioSettingsManager>( new CustomAudioSettingsManager() ) )
{
    BOSE_DEBUG( s_logger, __func__ );
    m_FrontDoorClientIF = FrontDoorClient;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   CustomProductAudioService::RegisterAudioPathEvents
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductAudioService::RegisterAudioPathEvents()
{
    BOSE_DEBUG( s_logger, __func__ );
    m_APPointer = APProductFactory::Create( "ProductAudioService-APProduct", m_ProductTask );
    RegisterCommonAudioPathEvents();
    ConnectToAudioPath();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   CustomProductAudioService::RegisterFrontDoorEvents
///
/// @brief  On Eddie, it register for put/post/get FrontDoor request for
///         bass, treble
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductAudioService::RegisterFrontDoorEvents()
{
    BOSE_DEBUG( s_logger, __func__ );

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// Endpoint /audio/bass - register ProductController as handler for POST/PUT/GET requests
    ///////////////////////////////////////////////////////////////////////////////////////////////
    auto getBassAction = [this]( )
    {
        return m_AudioSettingsMgr->GetBass( );
    };
    auto setBassAction = [this]( ProductPb::AudioBassLevel val )
    {
        return m_AudioSettingsMgr->SetBass( val );
    };
    m_AudioBassSetting = std::unique_ptr<AudioSetting<ProductPb::AudioBassLevel>>( new AudioSetting<ProductPb::AudioBassLevel>
                                                                                   ( kBassEndPoint,
                                                                                           getBassAction,
                                                                                           setBassAction,
                                                                                           m_FrontDoorClientIF,
                                                                                           m_ProductTask ) );

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// Endpoint /audio/treble - register ProductController as handler for POST/PUT/GET requests
    ///////////////////////////////////////////////////////////////////////////////////////////////
    auto getTrebleAction = [ this ]( )
    {
        return m_AudioSettingsMgr->GetTreble( );
    };
    auto setTrebleAction = [ this ]( ProductPb::AudioTrebleLevel val )
    {
        return m_AudioSettingsMgr->SetTreble( val );
    };
    m_AudioTrebleSetting = std::unique_ptr<AudioSetting<ProductPb::AudioTrebleLevel>>( new AudioSetting<ProductPb::AudioTrebleLevel>
                           ( kTrebleEndPoint,
                             getTrebleAction,
                             setTrebleAction,
                             m_FrontDoorClientIF,
                             m_ProductTask ) );
}

}// namespace ProductApp
