////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file      ProductDspHelper.cpp
///
/// @brief     This header file contains declarations for managing the parts of the product
///            specific interactions with the with external SHARC DSP
///
/// @attention Copyright (C) 2017 Bose Corporation All Rights Reserved
///
///            Bose Corporation
///            The Mountain Road,
///            Framingham, MA 01701-9168
///            U.S.A.
///
///            This program may not be reproduced, in whole or in part, in any form by any means
///            whatsoever without the written permission of Bose Corporation.
///
////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////
///
///            Included Header Files
///
////////////////////////////////////////////////////////////////////////////////////////////////////
#include "Utilities.h"
#include "ProfessorProductController.h"
#include "CustomProductLpmHardwareInterface.h"
#include "SoundTouchInterface/PlayerService.pb.h"
#include "ProductDspHelper.h"
#include "FrontDoorClient.h"
#include "EndPointsDefines.h"
#include "ProductEndpointDefines.h"
#include "ProductDataCollectionDefines.h"

namespace
{
// Decided because CEC latency is 200ms so similar timing for autowake
constexpr uint32_t s_PollingTimeAutoWakeMs         = 200;
// @todo - PGC-942 - revisit with minimumOutputLatencyMs and totalLatencyMs handling
constexpr uint32_t s_PollingTimeNormalOperationsMs = 5000;

const std::string DEF_STR_AUDIO_FORMAT_LPCM = "LPCM";
const std::string DEF_STR_AUDIO_FORMAT_DOLBY_DIGITAL = "Dolby Digital";
const std::string DEF_STR_AUDIO_FORMAT_DTS = "DTS";
const std::string DEF_STR_AUDIO_FORMAT_AAC = "AAC";
const std::string DEF_STR_AUDIO_FORMAT_DOLBY_TRUEHD = "Dolby TrueHD";
const std::string DEF_STR_AUDIO_FORMAT_DOLBY_DIGITAL_PLUS = "Dolby Digital Plus";
const std::string DEF_STR_AUDIO_FORMAT_UNKNOWN = "Format Unknown";
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                             Start of Product Namespace                                       ///
////////////////////////////////////////////////////////////////////////////////////////////////////
namespace ProductApp
{


////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductDspHelper::ProductDspHelper
///
/// @param  ProfessorProductController& ProductController
///
////////////////////////////////////////////////////////////////////////////////////////////////////
ProductDspHelper::ProductDspHelper( ProfessorProductController& ProductController )

    : m_timer( APTimer::Create( ProductController.GetTask( ), "DspStatusPollTimer" ) ),
      m_ProductController( ProductController )
{
    BOSE_INFO( s_logger, __PRETTY_FUNCTION__ );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductDspHelper::Run
///
/// @brief  This method connects and starts the handling of communication with A4VVideoManager
///         service.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool ProductDspHelper::Run( )
{
    BOSE_INFO( s_logger, __PRETTY_FUNCTION__ );

    {
        Callback< LpmServiceMessages::IpcDspStatus_t >
        CallbackForDspState( std::bind( &ProductDspHelper::DspStatusCallback,
                                        this,
                                        std::placeholders::_1 ) );

        m_ProductController.GetLpmHardwareInterface( )->RegisterForLpmEvents( IPC_DSP_STATUS,
                                                                              CallbackForDspState );
    }
    {
        Callback< LpmServiceMessages::DspDataCollection >
        CallbackForDspDataCollection( std::bind( &ProductDspHelper::ReceiveDspDataCollection,
                                                 this,
                                                 std::placeholders::_1 ) );

        m_ProductController.GetLpmHardwareInterface( )->RegisterForLpmEvents( ( IpcOpcodes_t ) DSP_DATA_COLLECTION,
                                                                              CallbackForDspDataCollection );
    }
    {
        AsyncCallback<Callback< ProductPb::AudioFormat >, Callback<FrontDoor::Error> >
        getAudioFormatCb( std::bind( &ProductDspHelper::AudioFormatFrontDoorGetHandler,
                                     this,
                                     std::placeholders::_1,
                                     std::placeholders::_2 ) ,
                          m_ProductController.GetTask( ) );

        m_AudioFormatGetConnection
            = m_ProductController.GetFrontDoorClient()->RegisterGet( FRONTDOOR_AUDIO_FORMAT_API,
                                                                     getAudioFormatCb,
                                                                     FrontDoor::PUBLIC,
                                                                     FRONTDOOR_PRODUCT_CONTROLLER_VERSION,
                                                                     FRONTDOOR_PRODUCT_CONTROLLER_GROUP_NAME );
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief ProductDspHelper::Stop
///
/// @brief Resources, memory, or any client server connections that may need to be released by
///        this module when stopped will need to be determined.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductDspHelper::Stop( )
{
    BOSE_INFO( s_logger, __PRETTY_FUNCTION__ );
    m_AudioFormatGetConnection.Disconnect();
    return;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief ProductDspHelper::AutoWakeTriggered
///
/// @brief When auto wake event is triggered this function will send a playback request to CAPS
///        in order to drive the system to TV source
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductDspHelper::AutoWakeTriggered()
{
    BOSE_INFO( s_logger, __PRETTY_FUNCTION__ );
    auto playbackRequestResponseCallback = [ this ]( const SoundTouchInterface::NowPlaying & response )
    {
        BOSE_DEBUG( s_logger, "A response to the playback request was received: %s" ,
                    ProtoToMarkup::ToJson( response, false ).c_str( ) );
    };

    auto playbackRequestErrorCallback = [ this ]( const FrontDoor::Error & error )
    {
        BOSE_ERROR( s_logger, "An error code %d subcode %d and error string <%s> was returned from a playback request.",
                    error.code(),
                    error.subcode(),
                    error.message().c_str() );
    };

    SoundTouchInterface::PlaybackRequest playbackRequestData;

    playbackRequestData.set_source( "PRODUCT" );
    playbackRequestData.set_sourceaccount( "TV" );

    m_ProductController.GetFrontDoorClient( )->SendPost<SoundTouchInterface::NowPlaying, FrontDoor::Error>( FRONTDOOR_CONTENT_PLAYBACKREQUEST_API,
            playbackRequestData,
            playbackRequestResponseCallback,
            playbackRequestErrorCallback );

    m_timer->Stop( );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief ProductDspHelper::DspStatusCallback
///
/// @param LpmServiceMessages::IpcDspStatus_t status
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductDspHelper::DspStatusCallback( const LpmServiceMessages::IpcDspStatus_t& status )
{
    BOSE_INFO( s_logger, "%s { autoWakePoll %d, noramlPoll %d }", __PRETTY_FUNCTION__, m_MonitorAutoWake, m_NormalDspPoll );

    BOSE_INFO( s_logger, " %s - Energy present: %d",  __PRETTY_FUNCTION__, status.energypresent() );
    if( m_MonitorAutoWake && status.energypresent() && !m_dspStatus.energypresent() )
    {
        AutoWakeTriggered();
    }

    // @todo - PGC-942update presentation latency

    m_dspStatus.CopyFrom( status );
}

//////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductDspHelper::SetAutoWakeMonitor
///
/// @param  bool enabled
///
//////////////////////////////////////////////////////////////////////////////////////////////
void ProductDspHelper::SetAutoWakeMonitor( bool enabled )
{
    BOSE_INFO( s_logger, "%s : %d", __PRETTY_FUNCTION__, enabled );
    m_NormalDspPoll = false;
    m_MonitorAutoWake = enabled;

    if( !enabled )
    {
        m_timer->Stop( );
    }
    else
    {
        m_timer->SetTimeouts( s_PollingTimeAutoWakeMs,
                              s_PollingTimeAutoWakeMs );

        StartPollTimer();
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductDspHelper::SetNormalOperationsMonitor
///
/// @param  bool enabled
///
//////////////////////////////////////////////////////////////////////////////////////////////
void ProductDspHelper::SetNormalOperationsMonitor( bool enabled )
{
    BOSE_INFO( s_logger, "%s : %d", __PRETTY_FUNCTION__, enabled );
    m_MonitorAutoWake = false;
    m_NormalDspPoll = enabled;

    if( !enabled )
    {
        m_timer->Stop( );
    }
    else
    {
        m_timer->SetTimeouts( s_PollingTimeNormalOperationsMs,
                              s_PollingTimeNormalOperationsMs );

        StartPollTimer();
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductDspHelper::StartPollTimer
///
//////////////////////////////////////////////////////////////////////////////////////////////
void ProductDspHelper::StartPollTimer( )
{
    BOSE_INFO( s_logger, __PRETTY_FUNCTION__ );
    auto pollDspCb = [this]()
    {
        Callback< LpmServiceMessages::IpcDspStatus_t >
        pollDspCb( std::bind( &ProductDspHelper::DspStatusCallback,
                              this,
                              std::placeholders::_1 ) );

        m_ProductController.GetLpmHardwareInterface( )->GetDspStatus( pollDspCb );
    };

    m_timer->Start( pollDspCb );
}

//////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductDspHelper::CreateAudioFormatChannelString
///
/// @param  uint32_t fullRangeChannels
/// @param  uint32_t lfeChannels
///
//////////////////////////////////////////////////////////////////////////////////////////////
std::string ProductDspHelper::CreateAudioFormatChannelString( uint32_t fullRangeChannels, uint32_t lfeChannels )
{
    return std::to_string( fullRangeChannels ) + "." + std::to_string( lfeChannels );
}

//////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductDspHelper::CreateAudioFormatNameFromEnum
///
/// @param  InputAudioFormat_t audioFormat
///
//////////////////////////////////////////////////////////////////////////////////////////////
std::string ProductDspHelper::CreateAudioFormatNameFromEnum( LpmServiceMessages::InputAudioFormat_t audioFormat )
{
    switch( audioFormat )
    {
    case LpmServiceMessages::AUDIO_FORMAT_PCM:
        return DEF_STR_AUDIO_FORMAT_LPCM;

    case LpmServiceMessages::AUDIO_FORMAT_AC3:
        return DEF_STR_AUDIO_FORMAT_DOLBY_DIGITAL;

    case LpmServiceMessages::AUDIO_FORMAT_DTS:
        return DEF_STR_AUDIO_FORMAT_DTS;

    case LpmServiceMessages::AUDIO_FORMAT_AAC:
        return DEF_STR_AUDIO_FORMAT_AAC;

    case LpmServiceMessages::AUDIO_FORMAT_MLP:
        return DEF_STR_AUDIO_FORMAT_DOLBY_TRUEHD;

    case LpmServiceMessages::AUDIO_FORMAT_EAC3:
        return DEF_STR_AUDIO_FORMAT_DOLBY_DIGITAL_PLUS;

    default:
        return DEF_STR_AUDIO_FORMAT_UNKNOWN;
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief ProductDspHelper::AudioFormatGetDspStatusCallback
///
/// @param LpmServiceMessages::IpcDspStatus_t status
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductDspHelper::AudioFormatGetDspStatusCallback( const Callback<ProductPb::AudioFormat>& resp,
                                                        const LpmServiceMessages::IpcDspStatus_t status )
{
    BOSE_INFO( s_logger, __PRETTY_FUNCTION__ );

    // Call the default callback
    DspStatusCallback( status );

    // Build response
    ProductPb::AudioFormat formatResponse;
    formatResponse.set_channels( CreateAudioFormatChannelString( status.fullrangechannels(), status.lfechannels() ) );
    formatResponse.set_format( CreateAudioFormatNameFromEnum( status.audioformat() ) );

    resp( formatResponse );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief ProductDspHelper::AudioFormatFrontDoorGetHandler
///
/// @param const Callback<ProductPb::AccessorySpeakerState>& resp
/// @param const Callback<FrontDoor::Error>& error
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductDspHelper::AudioFormatFrontDoorGetHandler( const Callback<ProductPb::AudioFormat>& resp,
                                                       const Callback<FrontDoor::Error>& error )
{
    BOSE_INFO( s_logger, __PRETTY_FUNCTION__ );
    Callback< LpmServiceMessages::IpcDspStatus_t >
    audioFormatCb( std::bind( &ProductDspHelper::AudioFormatGetDspStatusCallback,
                              this,
                              resp,
                              std::placeholders::_1 ) );
    m_ProductController.GetLpmHardwareInterface( )->GetDspStatus( audioFormatCb );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief ProductDspHelper::ReceiveDspDataCollection
///
/// @param const LpmServiceMessages::DspDataCollection& data - blob of data from dsp to be collected
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductDspHelper::ReceiveDspDataCollection( const LpmServiceMessages::DspDataCollection& data )
{
    m_ProductController.GetDataCollectionClient()->SendData(
        std::make_shared< LpmServiceMessages::DspDataCollection >( data ),
        DATA_COLLECTION_DSP_AIQ );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                           End of the Product Application Namespace                           ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
