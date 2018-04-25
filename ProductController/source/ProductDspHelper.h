////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file      ProductDspHelper.h
///
/// @brief     This header file contains declarations to handle communication with a A4VVideoManager
///            service.
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

///////////////////////////////////////////////////////////////////////////////////////////////////
///
/// The following compiler directive prevents this header file from being included more than once,
/// which may cause multiple declaration compiler errors.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////
///
///            Included Header Files
///
////////////////////////////////////////////////////////////////////////////////////////////////////
#include "ProductFrontDoorUtility.h"
#include "ProductMessage.pb.h"
#include "AudioFormat.pb.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
///                          Start of the Product Application Namespace                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
namespace ProductApp
{

////////////////////////////////////////////////////////////////////////////////////////////////////
///
///            Forward Class Declarations
///
////////////////////////////////////////////////////////////////////////////////////////////////////
class CustomProductLpmHardwareInterface;

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @class ProductDspHelper
///
/// @brief This class is used to handle communication with a A4VVideoManager service.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
class ProductDspHelper
{
public:

    //////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @name   ProductDspHelper Constructor
    ///
    /// @param  ProfessorProductController& ProductController
    ///
    //////////////////////////////////////////////////////////////////////////////////////////////
    ProductDspHelper( ProfessorProductController& ProductController );

    //////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @name   ProductDspHelper::SetAutoWakeMonitor
    ///
    /// @param  bool enabled
    ///
    //////////////////////////////////////////////////////////////////////////////////////////////
    void SetAutoWakeMonitor( bool enabled );

    //////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @name   ProductDspHelper::SetAutoWakeMonitor
    ///
    /// @param  bool enabled
    ///
    //////////////////////////////////////////////////////////////////////////////////////////////
    void SetNormalOperationsMonitor( bool enabled );

    //////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief The following public methods are used to run and stop instances of the
    ///        ProductDspHelper class, respectively.
    ///
    //////////////////////////////////////////////////////////////////////////////////////////////
    bool Run( );
    void Stop( );

private:
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief The timer to poll dsp and update local status
    ///
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    void StartPollTimer( );
    APTimerPtr m_timer;

    //////////////////////////////////////////////////////////////////////////////////////////////
    /// This holds the cached status for the IpcDspStatus_t response.
    //////////////////////////////////////////////////////////////////////////////////////////////
    LpmServiceMessages::IpcDspStatus_t        m_dspStatus;

    //////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief This member determines whether a connections to the LPM server connection is
    ///        established.
    ///
    //////////////////////////////////////////////////////////////////////////////////////////////
    bool m_MonitorAutoWake = false;
    bool m_NormalDspPoll   = false;

    ////////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief The following member variable stores the custom Professor product controller instance.
    ///
    ////////////////////////////////////////////////////////////////////////////////////////////////
    ProfessorProductController& m_ProductController;


    //////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief The following members store instances of the FrontDoor callbacks.
    ///
    //////////////////////////////////////////////////////////////////////////////////////////////
    CallbackConnection      m_AudioFormatGetConnection;

    //////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief This method is called when an async dsp status arrives or status is requested
    ///
    //////////////////////////////////////////////////////////////////////////////////////////////
    void DspStatusCallback( const LpmServiceMessages::IpcDspStatus_t& status );
    void AutoWakeTriggered();

    //////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief Methods to convert DSP enums to strings to send to front door
    ///
    //////////////////////////////////////////////////////////////////////////////////////////////
    static std::string CreateAudioFormatChannelString( uint32_t fullRangeChannels, uint32_t lfeChannels );
    static std::string CreateAudioFormatNameFromEnum( LpmServiceMessages::InputAudioFormat_t audioFormat );

    //////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief The following is to allow the front door to request audio format from dsp
    ///
    //////////////////////////////////////////////////////////////////////////////////////////////
    void AudioFormatGetDspStatusCallback( const Callback<ProductPb::AudioFormat>& resp,
                                          const LpmServiceMessages::IpcDspStatus_t status );
    void AudioFormatFrontDoorGetHandler( const Callback<ProductPb::AudioFormat>& resp,
                                         const Callback<FrontDoor::Error>& error );
};

////////////////////////////////////////////////////////////////////////////////////////////////////
///                           End of the Product Application Namespace                           ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
