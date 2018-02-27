////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file      ProductDspHelper.cpp
///
/// @brief     This header file contains declarations for managing the parts of the product
///            specific interactions with the with external SHARC DSP
///
/// @author    Manoranjani Malisetti
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
#include "ProductDspHelper.h"
#include "FrontDoorClient.h"


namespace
{
const std::string s_FrontDoorAudioFormatUrl = "/audio/format";

// @todo - work out better numbers here ( as of now no async on auto wake or change to my knowledge)
constexpr uint32_t s_PollingFirstPollTime          = 0;
constexpr uint32_t s_PollingTimeAutoWakeMs         = 200;
constexpr uint32_t s_PollingTimeNormalOperationsMs = 1000;

const std::string DEF_STR_AUDIO_FORMAT_LPCM = "LPCM";
const std::string DEF_STR_AUDIO_FORMAT_DOLBY_DIGITAL = "Dolby Digital";
const std::string DEF_STR_AUDIO_FORMAT_DTS = "DTS";
const std::string DEF_STR_AUDIO_FORMAT_AAC = "AAC";
const std::string DEF_STR_AUDIO_FORMAT_DOLBY_TRUEHD = "Dolby TrueHD";
const std::string DEF_STR_AUDIO_FORMAT_DOLBY_DIGITAL_PLUS = "Dolby Digital Plus";
const std::string DEF_STR_AUDIO_FORMAT_UNKNOWN = "Format Unkown";
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

    : m_ProductTask( ProductController.GetTask( ) ),
      m_ProductNotify( ProductController.GetMessageHandler( ) ),
      m_ProductLpmHardwareInterface( ProductController.GetLpmHardwareInterface( ) ),
      m_FrontDoorClientIF( ProductController.GetFrontDoorClient() ),
      m_timer( APTimer::Create( m_ProductTask, "DspStatusPollTimer" ) ),
      m_CustomProductController( static_cast< ProfessorProductController & >( ProductController ) )
{
    BOSE_DEBUG( s_logger, __PRETTY_FUNCTION__ );
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
    BOSE_DEBUG( s_logger, __PRETTY_FUNCTION__ );

    {
        Callback< LpmServiceMessages::IpcDspStatus_t >
        CallbackForDspState( std::bind( &ProductDspHelper::DspStatusCallback,
                                             this,
                                             std::placeholders::_1 ) );

        m_ProductLpmHardwareInterface->RegisterForLpmEvents( IPC_DSP_STATUS, CallbackForDspState );
    }

    {
        AsyncCallback<Callback< ProductPb::AudioFormat >, Callback<EndPointsError::Error> >
        getAudioFormatCb( std::bind( &ProductDspHelper::AudioFormatGetHandler,
                                     this,
                                     std::placeholders::_1,
                                     std::placeholders::_2 ) ,
                          m_ProductTask );

        m_AudioFormatGetConnection = m_FrontDoorClientIF->RegisterGet( s_FrontDoorAudioFormatUrl ,
                                                                       getAudioFormatCb );
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
    BOSE_DEBUG( s_logger, __PRETTY_FUNCTION__ );
    m_AudioFormatGetConnection.Disconnect();
    return;
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
    BOSE_DEBUG( s_logger, __PRETTY_FUNCTION__ );
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
    BOSE_DEBUG( s_logger, __PRETTY_FUNCTION__ );
    m_MonitorAutoWake = enabled;

    // Start timer
    if( !enabled )
    {
        m_timer->Stop( );
    }
    else
    {
        m_timer->SetTimeouts( s_PollingFirstPollTime,
                              s_PollingTimeAutoWakeMs );

        auto pollDspCb = [this] ()
        {
            Callback< LpmServiceMessages::IpcDspStatus_t >
            pollDspCb( std::bind( &ProductDspHelper::DspStatusCallback,
                                  this,
                                  std::placeholders::_1 ) );

            m_ProductLpmHardwareInterface->GetDspStatus( pollDspCb );
        };

        m_timer->Start( pollDspCb );
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
    BOSE_DEBUG( s_logger, __PRETTY_FUNCTION__ );
    m_MonitorAutoWake = false;

    // Start timer
    if( !enabled )
    {
        m_timer->Stop( );
    }
    else
    {
        m_timer->SetTimeouts( s_PollingFirstPollTime,
                              s_PollingTimeNormalOperationsMs );

        auto pollDspCb = [this] ()
        {
            Callback< LpmServiceMessages::IpcDspStatus_t >
            pollDspCb( std::bind( &ProductDspHelper::DspStatusCallback,
                                  this,
                                  std::placeholders::_1 ) );

            m_ProductLpmHardwareInterface->GetDspStatus( pollDspCb );
        };

        m_timer->Start( pollDspCb );
    }
}


//////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductDspHelper::GetAudioFormatChannelString
///
/// @param  uint32_t fullRangeChannels
/// @param  uint32_t lfeChannels
///
//////////////////////////////////////////////////////////////////////////////////////////////
const std::string ProductDspHelper::GetAudioFormatChannelString( uint32_t fullRangeChannels, uint32_t lfeChannels )
{
    return std::to_string(fullRangeChannels) + "." + std::to_string(lfeChannels);
}

//////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductDspHelper::GetAudioFormatNameFromEnum
///
/// @param  InputAudioFormat_t audioFormat
///
//////////////////////////////////////////////////////////////////////////////////////////////
const std::string ProductDspHelper::GetAudioFormatNameFromEnum( LpmServiceMessages::InputAudioFormat_t audioFormat )
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
    BOSE_DEBUG( s_logger, __PRETTY_FUNCTION__ );

    // Call the default callback
    DspStatusCallback( status );

    // Build response
    ProductPb::AudioFormat formatResponse;
    formatResponse.set_channels( GetAudioFormatChannelString( status.fullrangechannels(), status.lfechannels() ) );
    formatResponse.set_format( GetAudioFormatNameFromEnum( status.audioformat() ) );

    resp( formatResponse );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief ProductDspHelper::AudioFormatGetHandler
///
/// @param const Callback<ProductPb::AccessorySpeakerState>& resp
/// @param const Callback<EndPointsError::Error>& error
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductDspHelper::AudioFormatGetHandler( const Callback<ProductPb::AudioFormat>& resp,
                                              const Callback<EndPointsError::Error>& error )
{
    BOSE_DEBUG( s_logger, __PRETTY_FUNCTION__ );
    Callback< LpmServiceMessages::IpcDspStatus_t >
    audioFormatCb( std::bind( &ProductDspHelper::AudioFormatGetDspStatusCallback,
                          this,
                          resp,
                          std::placeholders::_1 ) );
    m_ProductLpmHardwareInterface->GetDspStatus( audioFormatCb );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                           End of the Product Application Namespace                           ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
