////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file      CustomProductLpmHardwareInterface.h
///
/// @brief     This header file contains custom Professor declarations for managing the hardware,
///            which interfaces with the Low Power Microprocessor or LPM on Riviera APQ boards.
///
/// @note      This custom class declared here inherits a ProductLpmHardwareInterface class found in
///            the common code repository. This base inherited class starts and runs an LPM client
///            connection, as well as provides several common hardware based methods.
///
/// @author    Stuart J. Lumby
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
#include "ProductLpmHardwareInterface.h"
#include "ProductMessage.pb.h"

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
class ProfessorProductController;

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @class CustomProductLpmHardwareInterface
///
/// @brief This class is used to managing the hardware, which interfaces with the Low Power
///        Microprocessor or LPM.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
class CustomProductLpmHardwareInterface : public ProductLpmHardwareInterface
{
public:

    ////////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief  CustomProductLpmHardwareInterface Constructor
    ///
    /// @param  ProfessorProductController& ProductController
    ///
    ////////////////////////////////////////////////////////////////////////////////////////////////
    CustomProductLpmHardwareInterface( ProfessorProductController& ProductController );

    //////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief These declarations are custom Professor utility calls, which send messages to the
    ///        LPM hardware client.
    ///
    //////////////////////////////////////////////////////////////////////////////////////////////
    bool NotifyVolumeLevel( uint32_t volume );
    bool NotifyMuteState( bool mute );
    bool SendSpeakerList( IpcAccessoryList_t&        accessoryList );
    bool SendWiFiRadioStatus( uint32_t frequencyInKhz );
    void SetBlueToothMacAddress( const std::string&       bluetoothMacAddress );
    void SetBlueToothDeviceName( const std::string&       bluetoothDeviceName );
    bool SendBlueToothDeviceData( const std::string&       bluetoothDeviceName,
                                  const unsigned long long bluetoothMacAddress );
    bool SendSourceSelection( const LPM_IPC_SOURCE_ID      sourceSelect );
    bool SetCecPhysicalAddress( const uint32_t cecPhyAddr );
    bool SendAdaptIQControl( ProductAdaptIQControl::AdaptIQAction action );
    bool BootDSPImage( LpmServiceMessages::IpcImage_t image );
    bool SetCecMode( const uint8_t mode );

    //////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief These methods pertain to the accessory speakers and pairing.
    ///
    //////////////////////////////////////////////////////////////////////////////////////////////
    bool SendAccessoryPairing( bool enabled, const Callback<LpmServiceMessages::IpcSpeakerPairingMode_t>& cb );
    bool SendAccessoryActive( bool rears, bool subs,  const Callback<IpcSpeakersActive_t> &cb );
    bool SendAccessoryDisband( const Callback<IpcAccessoryDisbandCommand_t> &cb );

    //////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief This method is for sending streamConfig to DSP for configuring inputs and gain.
    ///
    //////////////////////////////////////////////////////////////////////////////////////////////
    bool SetStreamConfig( IpcDspStreamConfigReqPayload_t streamConfig, const Callback<bool>& cb );


private:

    //////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief These declarations store the main task for processing LPM hardware events and
    ///        requests. It is passed by the ProductController instance.
    ///
    //////////////////////////////////////////////////////////////////////////////////////////////
    NotifyTargetTaskIF*        m_ProductTask   = nullptr;
    Callback< ProductMessage > m_ProductNotify = nullptr;

    //////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief These declarations are used to handle Bluetooth functionality.
    ///
    //////////////////////////////////////////////////////////////////////////////////////////////
    bool               m_gettingBlueToothData = false;
    unsigned long long m_blueToothMacAddress  = 0ULL;
    std::string        m_blueToothDeviceName;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
///                           End of the Product Application Namespace                           ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
