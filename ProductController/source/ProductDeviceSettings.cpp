///////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file      ProductDeviceSettings.cpp
///
/// @brief     This file contains source code for managing the device settings, which includes
///            requests for DSP audio, speaker settings, and product HDMI control.
///
/// @author    Stuart J. Lumby
///
/// @date      09/22/2017
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
#include "SystemUtils.h"
#include "unistd.h"
#include "DPrint.h"
#include "Services.h"
#include "APProductIF.h"
#include "APProductFactory.h"
#include "AudioControls.pb.h"
#include "BoseLinkServerMsgReboot.pb.h"
#include "BoseLinkServerMsgIds.pb.h"
#include "IPCDirectory.h"
#include "IPCDirectoryIF.h"
#include "A4VSystemTimeout.pb.h"
#include "A4VPersistence.pb.h"
#include "RebroadcastLatencyMode.pb.h"
#include "NetworkPortDefines.h"
#include "ProductController.h"
#include "ProductHardwareInterface.h"
#include "ProductDeviceSettings.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
///                             Start of Product Namespace                                       ///
////////////////////////////////////////////////////////////////////////////////////////////////////
namespace ProductApp
{

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief The following aliases refer to the Bose Sound Touch class utilities for inter-process and
///        inter-thread communications.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
typedef APClientSocketListenerIF::ListenerPtr   ClientPointer;
typedef APServerSocketListenerIF::ListenerPtr   ServerPointer;
typedef APClientSocketListenerIF::SocketPtr     ClientSocket;
typedef APServerSocketListenerIF::SocketPtr     ServerSocket;
typedef IPCMessageRouterIF::IPCMessageRouterPtr RouterPointer;
typedef APProductIF::APProductPtr               ProductPointer;

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// The following declares a DPrint class type object and a standard string for logging information
/// in this source code file.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
static const DPrint s_logger { "Product" };

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductDeviceSettings::GetInstance
///
/// @brief  This static method creates the one and only instance of a ProductDeviceSettings object.
///         The C++ Version 11 compiler guarantees that only one instance is created in a thread
///         safe way.
///IPCSource_t
/// @param  void This method does not take any arguments.
///
/// @return This method returns a pointer to a ProductDeviceSettings object.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
ProductDeviceSettings* ProductDeviceSettings::GetInstance( NotifyTargetTaskIF*         mainTask,
                                                          Callback< ProductMessage >  ProductNotify,
                                                          ProductHardwareInterface*   HardwareInterface )
{
    static ProductDeviceSettings* instance = new ProductDeviceSettings( mainTask,
                                                                        ProductNotify,
                                                                        HardwareInterface );

    BOSE_DEBUG( s_logger, "The instance %8p of the Product Device Settings was returned.", instance );

    return instance;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductDeviceSettings::ProductDeviceSettings
///
/// @brief  This method is the ProductDeviceSettings constructor, which is declared as being private
///         to ensure that only one instance of this class can be created through the class
///         GetInstance method.
///
/// @param  void This method does not take any arguments.
///
/// @return This method does not return anything.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
ProductDeviceSettings::ProductDeviceSettings( NotifyTargetTaskIF*        mainTask,
                                              Callback< ProductMessage > ProductNotify,
                                              ProductHardwareInterface*  HardwareInterface )

    : m_mainTask                ( mainTask          ),
      m_ProductNotify           ( ProductNotify     ),
      m_ProductHardwareInterface( HardwareInterface )
{
    return;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductDeviceSettings::Run
///
/// @brief  This method registers for product device settings requests.
///
/// @param  void This method does not take any arguments.
///
/// @return This method does not return anything.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductDeviceSettings::Run( )
{
    BOSE_DEBUG( s_logger, "Registration for device events and requests has been made." );

    m_audioDSPControlHandler = new WebExtensionsUrlHandler< audiodspcontrols >
    ( m_mainTask ,
      WEB_EX_PORT_AUDIO_DSP_CONTROL,
      m_audioDSPControlStore,
      std::bind( &ProductDeviceSettings::SendDSPAudioControls,
                 this ),
      true,
      std::bind( &ProductDeviceSettings::SanitizeAudioDSPControls,
                 this,
                 std::placeholders::_1 ) );

    m_audioProductToneControlHandler = new WebExtensionsUrlHandler< audioproducttonecontrols >
    ( m_mainTask ,
      WEB_EX_PORT_PRODUCT_TONE_CONTROLS,
      m_audioProductToneControlStore,
      std::bind( &ProductDeviceSettings::SendToneAndLevelControls,
                 this ),
      true,
      std::bind( &ProductDeviceSettings::SanitizeAudioToneControls,
                 this,
                 std::placeholders::_1 ) );

    m_audioProductSpeakerLevelHandler = new WebExtensionsUrlHandler< audioproductlevelcontrols >
    ( m_mainTask ,
      WEB_EX_PORT_PRODUCT_LEVEL_CONTROLS,
      m_audioProductSpeakerLevelStore,
      std::bind( &ProductDeviceSettings::SendToneAndLevelControls,
                 this ),
      true,
      std::bind( &ProductDeviceSettings::SanitizeAudioLevelControls,
                 this,
                 std::placeholders::_1 ) );

    m_audioSpeakerAttributeAndSettingHandler = new WebExtensionsUrlHandler< audiospeakerattributeandsetting >
    ( m_mainTask ,
      WEB_EX_PORT_SPEAKER_AVAILABILITY,
      m_audioSpeakerAttributeAndSettingStore,
      std::bind( &ProductDeviceSettings::SendSpeakerSettings,
                 this ),
      true,
      std::bind( &ProductDeviceSettings::SanitizeSpeakerAttributeAndSetting,
                 this,
                 std::placeholders::_1 ) );

    m_systemTimeoutControlHandler = new WebExtensionsUrlHandler< systemtimeoutcontrol >
    ( m_mainTask ,
      WEB_EX_PORT_PRODUCT_SYSTEM_TIMEOUT_CONTROL,
      m_systemTimeoutControlStore,
      std::bind( &ProductDeviceSettings::SendSystemTimeoutEnableBits,
                 this ),
      true );

    m_rebroadcastLatencyModeHandler = new WebExtensionsUrlHandler< rebroadcastlatencymode >
    ( m_mainTask ,
      WEB_EX_PORT_PRODUCT_REBROADCAST_LATENCY_MODE,
      m_rebroadcastLatencyModeStore,
      std::bind( &ProductDeviceSettings::SendRebroadcastLatencyMode,
                 this ),
      true,
      std::bind( &ProductDeviceSettings::SanitizeRebroadcastLatencyMode,
                 this,
                 std::placeholders::_1 ) );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name    ProductDeviceSettings::SanitizeAudioDSPControls
///
/// @brief   This method is called before the DSP DRC Mode and Audio Sync Delay is set in the
///          hardware, and is used to check the validity of the data to be set.
///
/// @param   protobuf [input] This argument stores DSP DRC Mode and Audio Sync Delay in the protobuf
///                           structure audiodspcontrols.
///
/// @returns This method returns a false Boolean value if the argument data supplied is invalid. It
///          returns a true Boolean value otherwise.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool ProductDeviceSettings::SanitizeAudioDSPControls( audiodspcontrols& protobuf )
{
    if( protobuf.audiomode( ) != AUDIO_MODE_NORMAL && protobuf.audiomode( ) != AUDIO_MODE_DIALOG )
    {
        ///////////////////////////////////////////////////////////////////////////////////////////
        /// An invalid value has been specified in this case.
        ///////////////////////////////////////////////////////////////////////////////////////////
        if( protobuf.has_audiomode( ) )
        {
            return false;
        }

        protobuf.set_audiomode( GetAudioDSPControlStore( ).audiomode( ) );
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    /// An invariable parameter has been specified in this case.
    ///////////////////////////////////////////////////////////////////////////////////////////
    if( protobuf.has_supportedaudiomodes( ) )
    {
        return false;
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductDeviceSettings::SendDSPAudioControls
///
/// @brief  This method sends the DSP Audio DRC Mode and Audio Sync Delay to set in the hardware,
///         using the stored values.
///
/// @param  void This method does not take any arguments.
///
/// @return This method does not return anything.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductDeviceSettings::SendDSPAudioControls( )
{
    IpcAudioMode_t audioMode  = ( IpcAudioMode_t ) GetAudioDSPControlStore( ).audiomode( );
    uint32_t       audioDelay = ( uint32_t ) GetAudioDSPControlStore( ).videosyncaudiodelay( );

    std::string audioModeString;

    switch( audioMode )
    {
       case IPC_AUDIO_MODE_UNSPECIFIED:
            audioModeString.assign( "Unspecified" );
            break;
       case IPC_AUDIO_MODE_DIRECT:
            audioModeString.assign( "Direct" );
            break;
       case IPC_AUDIO_MODE_NORMAL:
            audioModeString.assign( "Normal" );
            break;
       case IPC_AUDIO_MODE_DIALOG:
            audioModeString.assign( "Dialog" );
            break;
       case IPC_AUDIO_MODE_NIGHT:
            audioModeString.assign( "Night" );
            break;
       default:
            audioModeString.assign( "Unknown" );
            break;
    }

    BOSE_DEBUG( s_logger, "Audio mode is to be set to %s and audio delay to %d.",
                audioModeString.c_str( ),
                audioDelay );

    m_ProductHardwareInterface->SendSetDSPAudioMode( audioMode );
    m_ProductHardwareInterface->SendLipSyncDelay( audioDelay );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name    ProductDeviceSettings::SanitizeAudioToneControls
///
/// @brief   This method is called before the audio tone control settings are sent to hardware,
///          and is used to check the validity of the audio tone control data.
///
/// @param   protobuf [input] This argument stores audio tone controls in the Protocol Buffer
///                           structure audioproducttonecontrols.
///
/// @returns This method returns a false Boolean value if the argument data supplied is invalid. It
///          returns a true Boolean value otherwise.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool ProductDeviceSettings::SanitizeAudioToneControls( audioproducttonecontrols& protobuf ) const
{
    if( protobuf.has_bass( ) )
    {
        const auto& bassControl = protobuf.bass( );

        if( bassControl.has_minvalue( ) ||
            bassControl.has_maxvalue( ) ||
            bassControl.has_step( ) )
        {
            return false;
        }

        const auto  bassValue = bassControl.value( );
        const auto& bassStore = m_audioProductToneControlStore.bass( );

        if( ( bassValue < bassStore.minvalue( ) ) ||
            ( bassValue > bassStore.maxvalue( ) ) ||
            ( bassValue % bassStore.step( ) ) )
        {
            return false;
        }
    }

    if( protobuf.has_treble( ) )
    {
        const auto& trebleControl = protobuf.treble( );

        if( trebleControl.has_minvalue( ) ||
            trebleControl.has_maxvalue( ) ||
            trebleControl.has_step( ) )
        {
            return false;
        }

        const auto  trebleValue = trebleControl.value( );
        const auto& trebleStore = m_audioProductToneControlStore.treble( );

        if( ( trebleValue < trebleStore.minvalue( ) ) ||
            ( trebleValue > trebleStore.maxvalue( ) ) ||
            ( trebleValue % trebleStore.step( ) ) )
        {
            return false;
        }
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name    ProductDeviceSettings::SanitizeAudioLevelControls
///
/// @brief   This method is called before the audio level control settings are sent to the hardware,
///          and is used to check the validity of the audio level control data.
///
/// @param   protobuf [input] This argument stores audio level controls in the protobuf strucutre
///                           audioproductlevelcontrols.
///
/// @returns This method returns a false Boolean value if the argument data supplied is invalid. It
///          returns a true Boolean value otherwise.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool ProductDeviceSettings::SanitizeAudioLevelControls( audioproductlevelcontrols& protobuf ) const
{
    if( protobuf.has_frontcenterspeakerlevel( ) )
    {
        const auto& frontcenterspeakerlevelControl = protobuf.frontcenterspeakerlevel( );

        if( frontcenterspeakerlevelControl.has_minvalue( ) ||
            frontcenterspeakerlevelControl.has_maxvalue( ) ||
            frontcenterspeakerlevelControl.has_step( ) )
        {
            return false;
        }

        const auto  frontcenterspeakerlevelValue = frontcenterspeakerlevelControl.value( );
        const auto& frontcenterspeakerlevelStore = m_audioProductSpeakerLevelStore.frontcenterspeakerlevel( );

        if( ( frontcenterspeakerlevelValue < frontcenterspeakerlevelStore.minvalue( ) ) ||
            ( frontcenterspeakerlevelValue > frontcenterspeakerlevelStore.maxvalue( ) ) ||
            ( frontcenterspeakerlevelValue % frontcenterspeakerlevelStore.step( ) ) )
        {
            return false;
        }
    }

    if( protobuf.has_rearsurroundspeakerslevel( ) )
    {
        const auto& rearsurroundspeakerslevelControl = protobuf.rearsurroundspeakerslevel( );

        if( rearsurroundspeakerslevelControl.has_minvalue( ) ||
            rearsurroundspeakerslevelControl.has_maxvalue( ) ||
            rearsurroundspeakerslevelControl.has_step( ) )
        {
            return false;
        }

        const auto  rearsurroundspeakerslevelValue = rearsurroundspeakerslevelControl.value( );
        const auto& rearsurroundspeakerslevelStore = m_audioProductSpeakerLevelStore.rearsurroundspeakerslevel( );

        if( ( rearsurroundspeakerslevelValue < rearsurroundspeakerslevelStore.minvalue( ) ) ||
            ( rearsurroundspeakerslevelValue > rearsurroundspeakerslevelStore.maxvalue( ) ) ||
            ( rearsurroundspeakerslevelValue % rearsurroundspeakerslevelStore.step( ) ) )
        {
            return false;
        }
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductDeviceSettings::SendToneAndLevelControls
///
/// @brief  This method sends the set tone and volume level controls to the hardware, using stored
///         values from two different protobufs.
///
/// @param  void This method does not take any arguments.
///
/// @return This method does not return anything.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductDeviceSettings::SendToneAndLevelControls( )
{
    IpcToneControl_t controls;
    const auto&      audioToneControl = GetAudioToneControlStore( );

    controls.set_bass  ( audioToneControl.bass  ( ).value( ) );
    controls.set_treble( audioToneControl.treble( ).value( ) );

    const auto& audioSpeakerLevel = GetAudioSpeakerLevelStore( );

    controls.set_centerspeaker  ( audioSpeakerLevel.frontcenterspeakerlevel  ( ).value( ) );
    controls.set_surroundspeaker( audioSpeakerLevel.rearsurroundspeakerslevel( ).value( ) );

    BOSE_DEBUG( s_logger, "Audio tone and level settings are as follows: " );
    BOSE_DEBUG( s_logger, "               " );
    BOSE_DEBUG( s_logger, "Bass      : %d ", controls.bass  ( ) );
    BOSE_DEBUG( s_logger, "Treble    : %d ", controls.treble( ) );
    BOSE_DEBUG( s_logger, "Center    : %d ", controls.centerspeaker  ( ) );
    BOSE_DEBUG( s_logger, "Surround  : %d ", controls.surroundspeaker( ) );

    m_ProductHardwareInterface->SendToneAndLevelControl( controls );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name    ProductDeviceSettings::SanitizeSpeakerAttributeAndSetting
///
/// @brief   This method is called before the speaker settings are sent to the client application,
///          and is used to check the validity of the speaker settings data.
///
/// @param   protobuf [input] This argument stores speaker settings in the protobuf strucutre
///                           audiospeakerattributeandsetting.
///
/// @returns This method returns a false Boolean value if the argument data supplied is invalid. It
///          returns a true Boolean value otherwise.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool ProductDeviceSettings::SanitizeSpeakerAttributeAndSetting( audiospeakerattributeandsetting& protobuf )
{
    BOSE_DEBUG( s_logger, "Validation of the Speaker settings are to be checked." );

    ////////////////////////////////////////////////////////////////////////////////////////////////
    /// The following code is commented out until a hardware interface with the LPM is complete.
    ////////////////////////////////////////////////////////////////////////////////////////////////
    auto rearDataStore = protobuf.mutable_rear( );

    ////////////////////////////////////////////////////////////////////////////////////////////////
    /// Do not allow any activation for non-controllable speakers.
    ////////////////////////////////////////////////////////////////////////////////////////////////
    if( !GetSpeakerAttributeAndSettingStore( ).rear( ).controllable( ) )
    {
        if( rearDataStore->has_active( ) )
        {
            return false;
        }
    }

    if( rearDataStore->has_available( ) ||
        rearDataStore->has_wireless( ) ||
        rearDataStore->has_controllable( ) )
    {
        return false;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// Note that only one subwoofer is supported at the moment so the subwoofer02 data is ignored.
    ///////////////////////////////////////////////////////////////////////////////////////////////
    auto subwoofer01DataStore = protobuf.mutable_subwoofer01( );

    ////////////////////////////////////////////////////////////////////////////////////////////////
    /// Do not allow any activation for non-controllable speakers.
    ////////////////////////////////////////////////////////////////////////////////////////////////
    if( !GetSpeakerAttributeAndSettingStore( ).subwoofer01( ).controllable( ) )
    {
        if( subwoofer01DataStore->has_active( ) )
        {
            return false;
        }
    }

    if( subwoofer01DataStore->has_available( ) ||
        subwoofer01DataStore->has_wireless( ) ||
        subwoofer01DataStore->has_controllable( ) )
    {
        return false;
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductDeviceSettings::SendSpeakerSettings
///
/// @brief  This method sends the speaker settings to the hardware. It is called after the
///         device information has been set. The data should have been sanitized to prevent unmatched
///         expectations, such as setting a non-existing accessory on the device to active.
///
/// @param  void This method does not take any arguments.
///
/// @return This method does not return anything.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductDeviceSettings::SendSpeakerSettings( )
{
    BOSE_DEBUG( s_logger, "Speaker settings are to be sent to the hardware interface." );

    IpcAccessoryList_t accessoryList;
    const audiospeakerattributeandsetting& speakerAttributeAndSetting = GetSpeakerAttributeAndSettingStore( );

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// Note that only one subwoofer is supported at the moment so the subwoofer02 data is ignored.
    ///////////////////////////////////////////////////////////////////////////////////////////////

    auto leftRearAccessory = accessoryList.add_accessory( );

    leftRearAccessory->set_position( ACCESSORY_POSITION_LEFT_REAR );

    if( speakerAttributeAndSetting.subwoofer01( ).active( ) )
    {
        leftRearAccessory->set_active( 1 );
    }
    else
    {
        leftRearAccessory->set_active( 0 );
    }

    auto rightRearAccessory = accessoryList.add_accessory( );

    rightRearAccessory->set_position( ACCESSORY_POSITION_RIGHT_REAR );

    if( speakerAttributeAndSetting.rear( ).active( ) )
    {
        rightRearAccessory->set_active( 1 );
    }
    else
    {
        rightRearAccessory->set_active( 0 );
    }

    BOSE_DEBUG( s_logger, "Speaker activation settings are as follows: " );
    BOSE_DEBUG( s_logger, "                                            " );
    BOSE_DEBUG( s_logger, "Left  Speaker : %s ", leftRearAccessory->active (  ) ? "Active" : "Inactive" );
    BOSE_DEBUG( s_logger, "Right Speaker : %s ", rightRearAccessory->active(  ) ? "Active" : "Inactive" );

    m_ProductHardwareInterface->SendSpeakerList( accessoryList );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductDeviceSettings::SendSystemTimeoutEnableBits
///
/// @brief  This method sends time out control information to the hardware.
///
/// @param  void This method does not take any arguments.
///
/// @return This method does not return anything.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductDeviceSettings::SendSystemTimeoutEnableBits( )
{
    Ipc_TimeoutControl_t timeoutControl;

    timeoutControl.set_cmd   ( TIMEOUT_CMD_SET );
    timeoutControl.set_enable( GetSystemTimeoutEnableBits( ).autopowerdown( ) );

    BOSE_DEBUG( s_logger, "Auto power down will be set to %s.", timeoutControl.enable( ) ? "on" : "off" );

    m_ProductHardwareInterface->SendSetSystemTimeoutEnableBits( timeoutControl );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name    ProductDeviceSettings::SanitizeRebroadcastLatencyMode
///
/// @brief   called when POST rebroadcastlatecymode is received from API caller
///
/// @param   rebroadcastlatecymode& [input] protobuf
///
/// @returns false if arguments are insane
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool ProductDeviceSettings::SanitizeRebroadcastLatencyMode( rebroadcastlatencymode& protobuf )
{
    if( protobuf.has_controllable( ) )
    {
        return false;
    }

    if( !protobuf.has_mode( ) )
    {
        protobuf.set_mode( GetRebroadcastLatencyModeStore( ).mode( ) );
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name    ProductDeviceSettings::SendRebroadcastLatencyMode
///
/// @brief   Sends the Rebroadcast Latency Mode to the LPM.
///
/// @returns void
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductDeviceSettings::SendRebroadcastLatencyMode( )
{
    std::string latencyMode = REBROADCAST_LATENCY_MODE_Name( GetRebroadcastLatencyModeStore( ).mode( ) );

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// The latency support may need to be tied into the audio path value for support through the
    /// LPM.
    ///////////////////////////////////////////////////////////////////////////////////////////////
    BOSE_DEBUG( s_logger, "A latency mode of %s is being set.", latencyMode.c_str( ) );
    BOSE_DEBUG( s_logger, "A latency mode seting is currently not supported." );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                               End of ProductApp Namespace                                    ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
