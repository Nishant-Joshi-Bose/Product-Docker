////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file      ProductCommandLine.cpp
///
/// @brief     This file contains source code that implements the ProductCommandLine class that is
///            used to set up a command line interface.
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
///            Included Header Files
///
////////////////////////////////////////////////////////////////////////////////////////////////////
#include <sstream>
#include "Intents.h"
#include "IntentHandler.h"
#include "CustomProductLpmHardwareInterface.h"
#include "CustomProductController.h"
#include "ProductCommandLine.h"
#include "ProductEndpointDefines.h"
#include "AccessorySoftwareInstallManager.h"

///
/// Class Name Declaration for Logging
///
namespace
{
constexpr char CLASS_NAME[ ] = "ProductCommandLine";
}
////////////////////////////////////////////////////////////////////////////////////////////////////
///                          Start of the Product Application Namespace                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
namespace ProductApp
{

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductCommandLine::ProductCommandLine
///
/// @param  CustomProductController& ProductController
///
////////////////////////////////////////////////////////////////////////////////////////////////////
ProductCommandLine::ProductCommandLine( CustomProductController& ProductController ):

    m_ProductController( ProductController ),
    m_ProductTask( ProductController.GetTask( ) ),
    m_ProductLpmHardwareInterface( ProductController.GetLpmHardwareInterface( ) )
{
    BOSE_INFO( s_logger, "%s::%s", CLASS_NAME, __func__ );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductCommandLine::Run
///
/// @brief  This method initializes CliClientMT and calls to register commands.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductCommandLine::Run( )
{
    BOSE_INFO( s_logger, "%s::%s", CLASS_NAME, __func__ );

    RegisterCliCmds();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductCommandLine::Stop
///
/// @brief  This method can be used to clean up items before Product Controller terminates.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductCommandLine::Stop( )
{
    BOSE_INFO( s_logger, "%s::%s", CLASS_NAME, __func__ );
}


////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductCommandLine::RegisterCliCmds
///
/// @brief  Register the CLI commands for the product controller.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductCommandLine::RegisterCliCmds( )
{

    BOSE_INFO( s_logger, "%s::%s", CLASS_NAME, __func__ );

    auto callbackForCommands = [ this ]( uint16_t                            cmdKey,
                                         const std::list<std::string>&       argList,
                                         AsyncCallback<std::string, int32_t> respCb,
                                         int32_t                             transact_id )
    {
        HandleCliCmd( cmdKey, argList, respCb, transact_id );
    };

    m_ProductController.GetCommonCliClientMT().RegisterCLIServerCommands( "product autowake",
                                                                          "This command tests setting autowake to an on or off state.",
                                                                          "product autowake [on | off]",
                                                                          m_ProductController.GetTask(),
                                                                          callbackForCommands,
                                                                          static_cast<int>( CLICmdsKeys::AUTOWAKE ) );

    m_ProductController.GetCommonCliClientMT().RegisterCLIServerCommands( "product intent",
                                                                          "This command tests sending an intent value.",
                                                                          "product intent [int from 0 to 254]",
                                                                          m_ProductController.GetTask(),
                                                                          callbackForCommands,
                                                                          static_cast<int>( CLICmdsKeys::INTENT ) );

    m_ProductController.GetCommonCliClientMT().RegisterCLIServerCommands( "product mute",
                                                                          "This command mutes or unmutes the volume.",
                                                                          "product mute [on | off]",
                                                                          m_ProductController.GetTask(),
                                                                          callbackForCommands,
                                                                          static_cast<int>( CLICmdsKeys::MUTE ) );

    m_ProductController.GetCommonCliClientMT().RegisterCLIServerCommands( "product source",
                                                                          "This command selects the audio source.",
                                                                          "product source [tv | st]",
                                                                          m_ProductController.GetTask(),
                                                                          callbackForCommands,
                                                                          static_cast<int>( CLICmdsKeys::SOURCE ) );

    m_ProductController.GetCommonCliClientMT().RegisterCLIServerCommands( "product boot_status",
                                                                          "This command outputs the status of the boot up state.",
                                                                          "product boot_status",
                                                                          m_ProductController.GetTask(),
                                                                          callbackForCommands,
                                                                          static_cast<int>( CLICmdsKeys::BOOT_STATUS ) );

    m_ProductController.GetCommonCliClientMT().RegisterCLIServerCommands( "product test_freq",
                                                                          "This command tests setting the wifi frequency in kHz.",
                                                                          "product test_freq [int from 0 to 10M]",
                                                                          m_ProductController.GetTask(),
                                                                          callbackForCommands,
                                                                          static_cast<int>( CLICmdsKeys::TEST_FREQ ) );

    m_ProductController.GetCommonCliClientMT().RegisterCLIServerCommands( "product test_pairing",
                                                                          "This command tests pairing the device with another speaker.",
                                                                          "product test_pairing",
                                                                          m_ProductController.GetTask(),
                                                                          callbackForCommands,
                                                                          static_cast<int>( CLICmdsKeys::TEST_PAIRING ) );

    m_ProductController.GetCommonCliClientMT().RegisterCLIServerCommands( "product test_power",
                                                                          "This command tests sending a power toggle intent.",
                                                                          "product test_power",
                                                                          m_ProductController.GetTask(),
                                                                          callbackForCommands,
                                                                          static_cast<int>( CLICmdsKeys::TEST_POWER ) );

    m_ProductController.GetCommonCliClientMT().RegisterCLIServerCommands( "product test_voice",
                                                                          "This command tests setting the voice VPA to an on or off configured state.",
                                                                          "product test_voice [on | off]",
                                                                          m_ProductController.GetTask(),
                                                                          callbackForCommands,
                                                                          static_cast<int>( CLICmdsKeys::TEST_VOICE ) );

    m_ProductController.GetCommonCliClientMT().RegisterCLIServerCommands( "product volume",
                                                                          "This command sets the volume to a specified level.",
                                                                          "product volume [int from 0 to 100]",
                                                                          m_ProductController.GetTask(),
                                                                          callbackForCommands,
                                                                          static_cast<int>( CLICmdsKeys::TEST_VOLUME ) );

    m_ProductController.GetCommonCliClientMT().RegisterCLIServerCommands( "product test_accessory_update",
                                                                          "This command triggers the accessory update flow.",
                                                                          "product test_accessory_update",
                                                                          m_ProductController.GetTask(),
                                                                          callbackForCommands,
                                                                          static_cast<int>( CLICmdsKeys::TEST_ACCESSORY_UPDATE ) );

    m_ProductController.GetCommonCliClientMT().RegisterCLIServerCommands( "product test_transition",
                                                                          "This command queries/sets/clears the flag to halt in the PLAYABLE_TRANSITION state.",
                                                                          "product test_transition [on | off]",
                                                                          m_ProductController.GetTask(),
                                                                          callbackForCommands,
                                                                          static_cast<int>( CLICmdsKeys::TEST_TRANSITION ) );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductCommandLine::HandleCliCmd
///
/// @brief  Callback function for registered product controller CLI commands.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductCommandLine::HandleCliCmd( uint16_t                              cmdKey,
                                       const std::list< std::string >&       argList,
                                       AsyncCallback< std::string, int32_t > respCb,
                                       int32_t                               transact_id )
{
    BOSE_INFO( s_logger, "%s::%s - cmd: %d", CLASS_NAME, __func__, cmdKey );

    std::string response( "Success" );

    for( const auto& v : argList )
    {
        BOSE_INFO( s_logger, "%s :", v.c_str() );
    }

    switch( static_cast< CLICmdsKeys >( cmdKey ) )
    {
    case CLICmdsKeys::AUTOWAKE:
        HandleAutowake( argList, response );
        break;

    case CLICmdsKeys::INTENT :
        HandleIntent( argList, response );
        break;

    case CLICmdsKeys::MUTE :
        HandleMute( argList, response );
        break;

    case CLICmdsKeys::SOURCE:
        HandleSource( argList, response );
        break;

    case CLICmdsKeys::BOOT_STATUS:
        HandleBootStatus( argList, response );
        break;

    case CLICmdsKeys::TEST_FREQ:
        HandleTestFreq( argList, response );
        break;

    case CLICmdsKeys::TEST_PAIRING:
        HandleTestPairing( argList, response );
        break;

    case CLICmdsKeys::TEST_POWER:
        HandleTestPower( argList, response );
        break;

    case CLICmdsKeys::TEST_VOICE:
        HandleTestVoice( argList, response );
        break;

    case CLICmdsKeys::TEST_VOLUME:
        HandleVolume( argList, response );
        break;

    case CLICmdsKeys::TEST_ACCESSORY_UPDATE:
        HandleTestAccessoryUpdate( argList, response );
        break;

    case CLICmdsKeys::TEST_TRANSITION:
        HandleTestTransition( argList, response );
        break;

    default:
        response = "Command not found";
        break;
    }

    respCb( response, transact_id );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// Start of callback functions to process each registered product controller CLI command.
///
////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductCommandLine::HandleAutowake
///
/// @brief  This command tests setting autowake to an on or off state and sending it to the
///         product controller state machine.
///
////////////////////////////////////////////////////////////////////////////////////////////////
void ProductCommandLine::HandleAutowake( const std::list<std::string>& argList,
                                         std::string& response )
{
    if( argList.size( ) != 1 )
    {
        response = "Incorrect Usage: product autowake [on | off]";
    }
    else
    {

        const std::string& argumentString = argList.front( );

        ProductMessage productMessage;

        if( argumentString == "on" )
        {
            response  = "An autowake on test will now be made.";

            productMessage.mutable_autowakestatus( )->set_active( true );
        }
        else if( argumentString == "off" )
        {
            response  = "An autowake off test will now be made.";

            productMessage.mutable_autowakestatus( )->set_active( false );
        }
        else
        {
            response = "Incorrect Usage: product autowake [on | off]";
        }

        m_ProductController.SendAsynchronousProductMessage( productMessage );
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductCommandLine::HandleIntent
///
/// @brief  This command tests sending an intent value to the product controller state machine.
///
////////////////////////////////////////////////////////////////////////////////////////////////
void ProductCommandLine::HandleIntent( const std::list<std::string>& argList,
                                       std::string& response )
{
    if( argList.size( ) != 1 )
    {
        response = "Incorrect Usage: product intent [integer from 0 to 254]";
    }
    else
    {
        const std::string& argumentString = argList.front( );
        const uint32_t     intentValue    = std::atoi( argumentString.c_str( ) );

        if( intentValue <= 254 )
        {
            response  = "The intent value ";
            response +=  CommonIntentHandler::GetIntentName( intentValue );
            response += " will be sent to the product controller state machine.\r\n";

            ProductMessage productMessage;
            productMessage.set_action( static_cast< uint32_t >( intentValue ) );

            m_ProductController.SendAsynchronousProductMessage( productMessage );
        }
        else
        {
            response = "Incorrect Usage: product intent [integer from 0 to 254]";
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductCommandLine::HandleMute
///
/// @brief  This command tests changing the mute setting on the device.
///
////////////////////////////////////////////////////////////////////////////////////////////////
void ProductCommandLine::HandleMute( const std::list<std::string>& argList,
                                     std::string& response )
{
    if( argList.size( ) != 1 )
    {
        response = "Incorrect Usage: product mute [on | off] \r\n";
    }
    else
    {
        bool muteStateValue;
        const std::string& muteState = argList.front( );

        if( muteState == "on" || muteState == "off" )
        {
            if( muteState == "on" )
            {

                muteStateValue = "The mute will be turned on. \r\n";
                muteStateValue = true;
            }
            else
            {

                response = "The mute will be turned off. \r\n";
                muteStateValue = false;
            }
            auto errFunc = []( FrontDoor::Error error )
            {
                BOSE_ERROR( s_logger, "An error code %d subcode %d and error string <%s> was returned from a frontdoor mute request.",
                            error.code(),
                            error.subcode(),
                            error.message().c_str() );
            };

            AsyncCallback<FrontDoor::Error> errCb( errFunc, m_ProductTask );

            SoundTouchInterface::volume pbVolume;
            pbVolume.set_muted( muteStateValue );

            BOSE_VERBOSE( s_logger, "Setting FrontDoor mute to %d", pbVolume.muted( ) );
            m_ProductController.GetFrontDoorClient( )->SendPut<SoundTouchInterface::volume, FrontDoor::Error>(
                FRONTDOOR_AUDIO_VOLUME_API, pbVolume, {}, errCb );
        }
        else
        {
            response = "Incorrect Usage: product mute [on | off] \r\n";
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductCommandLine::HandleSource
///
/// @brief  This command tests changing the audio source on the device.
///
////////////////////////////////////////////////////////////////////////////////////////////////
void ProductCommandLine::HandleSource( const std::list<std::string>& argList,
                                       std::string& response )
{
    if( argList.size( ) != 1 )
    {
        response  = "Incorrect Usage: product source [tv | st] \r\n";
    }
    else
    {
        const std::string& sourceString = argList.front( );

        if( sourceString == "tv" )
        {
            KeyHandlerUtil::ActionType_t startTvPlayback = static_cast< KeyHandlerUtil::ActionType_t >( Action::ACTION_TV );
            ProductMessage message;
            message.set_action( startTvPlayback );

            m_ProductController.SendAsynchronousProductMessage( message );
        }
        else if( sourceString == "st" )
        {
            KeyHandlerUtil::ActionType_t startSoundTouchPlayback = static_cast< KeyHandlerUtil::ActionType_t >( Action::ACTION_SOUNDTOUCH );
            ProductMessage message;
            message.set_action( startSoundTouchPlayback );

            m_ProductController.SendAsynchronousProductMessage( message );
        }
        else
        {
            response  = "Incorrect Usage: product source [tv | st] \r\n";
        }

        response  = "The source will be changed to the value ";
        response +=  sourceString.c_str( );
        response += ". \r\n";
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductCommandLine::HandleBootStatus
///
/// @brief  This command outputs the boot up status of the device.
///
////////////////////////////////////////////////////////////////////////////////////////////////
void ProductCommandLine::HandleBootStatus( const std::list<std::string>& argList,
                                           std::string& response )
{
    std::string LpmConnected( m_ProductController.IsLpmReady( )                     ? "true" : "false" );
    std::string CapsInitialized( m_ProductController.IsCAPSReady( )                 ? "true" : "false" );
    std::string audioPathConnected( m_ProductController.IsAudioPathReady( )         ? "true" : "false" );
    std::string StsInitialized( m_ProductController.IsSTSReady( )                   ? "true" : "false" );
    std::string SoftwareUpdateReady( m_ProductController.IsSoftwareUpdateReady( )   ? "true" : "false" );
    std::string SassInitialized( m_ProductController.IsSassReady( )                 ? "true" : "false" );
    std::string bluetoothInitialized( m_ProductController.IsBluetoothModuleReady( ) ? "true" : "false" );
    std::string networkModuleReady( m_ProductController.IsNetworkModuleReady( )     ? "true" : "false" );
    std::string voiceModuleReady( m_ProductController.IsVoiceModuleReady( )         ? "true" : "false" );

    response  = "------------- Product Controller Booting Status -------------\n";
    response += "\n";
    response += "LPM Connected         : ";
    response += LpmConnected;
    response += "\n";
    response += "CAPS Initialized      : ";
    response += CapsInitialized;
    response += "\n";
    response += "Audio Path Connected  : ";
    response += audioPathConnected;
    response += "\n";
    response += "STS Initialized       : ";
    response += StsInitialized;
    response += "\n";
    response += "Software Update Ready : ";
    response += SoftwareUpdateReady;
    response += "\n";
    response += "SASS Initialized      : ";
    response += SassInitialized;
    response += "\n";
    response += "Bluetooth Initialized : ";
    response += bluetoothInitialized;
    response += "\n";
    response += "Network Module Ready  : ";
    response += networkModuleReady;
    response += "\n";
    response += "Voice Module Ready    : ";
    response += voiceModuleReady;
    response += "\n";
    response += "\n";

    if( m_ProductController.IsBooted( ) )
    {
        response += "The device has been successfully booted.";
    }
    else
    {
        response += "The device has not yet been booted.";
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductCommandLine::HandleTestFreq
///
/// @brief  This command tests the system response to delivery of the wireless radio frequency.
///
////////////////////////////////////////////////////////////////////////////////////////////////
void ProductCommandLine::HandleTestFreq( const std::list<std::string>& argList,
                                         std::string& response )
{
    if( argList.size( ) != 1 )
    {
        response = "Incorrect Usage: product test_freq [int from 0 to 10M]";
    }
    else
    {

        const std::string& argumentString = argList.front( );
        const uint32_t     frequencyValue = std::atoi( argumentString.c_str( ) );

        if( frequencyValue <= 10000000 )
        {
            response  = "The wifi frequncy value ";
            response +=  frequencyValue;
            response += "kHz will be sent to the product controller state machine.\r\n";

            ProductMessage productMessage;
            productMessage.mutable_wirelessstatus( )->set_configured( true );
            productMessage.mutable_wirelessstatus( )->set_frequencykhz( frequencyValue );

            m_ProductController.SendAsynchronousProductMessage( productMessage );
        }
        else
        {
            response = "Incorrect Usage: product test_freq [int from 0 to 10M]";
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductCommandLine::HandleTestPairing
///
/// @brief  This command tests pairing the device with another speaker.
///
////////////////////////////////////////////////////////////////////////////////////////////////
void ProductCommandLine::HandleTestPairing( const std::list<std::string>& argList,
                                            std::string& response )
{
    ProductMessage productMessage;
    productMessage.set_action( static_cast< uint32_t >( Action::ACTION_START_PAIR_SPEAKERS ) );

    response  = "An attempt to pair with another speaker to this device will be made.";

    m_ProductController.SendAsynchronousProductMessage( productMessage );
}

////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductCommandLine::HandleTestPower
///
/// @brief  This command tests sending a power toggle intent to the product controller state machine.
///
////////////////////////////////////////////////////////////////////////////////////////////////
void ProductCommandLine::HandleTestPower( const std::list<std::string>& argList,
                                          std::string& response )
{
    ProductMessage productMessage;
    productMessage.set_action( static_cast< uint32_t >( Action::POWER_TOGGLE ) );

    m_ProductController.SendAsynchronousProductMessage( productMessage );
}

////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductCommandLine::HandleTestVoice
///
/// @brief  This command to tests setting the Content Audio Playback Service to an on or off state and
///         sending it to the product controller state machine. Its actual state is not effected.
///
////////////////////////////////////////////////////////////////////////////////////////////////
void ProductCommandLine::HandleTestVoice( const std::list<std::string>& argList,
                                          std::string& response )
{
    if( argList.size( ) != 1 )
    {
        response = "Incorrect Usage: product test_voice [on | off]";
    }
    else
    {
        const std::string& argumentString = argList.front( );

        ProductMessage productMessage;

        if( argumentString == "on" )
        {
            response  = "A voice configured state test will now be made.";

            productMessage.mutable_voicestatus( )->set_microphoneenabled( true );
            productMessage.mutable_voicestatus( )->set_accountconfigured( true );
        }
        else if( argumentString == "off" )
        {
            response  = "A voice unconfigured state test will now be made.";

            productMessage.mutable_voicestatus( )->set_microphoneenabled( false );
            productMessage.mutable_voicestatus( )->set_accountconfigured( false );
        }
        else
        {
            response = "Incorrect Usage: product test_voice [on | off]";
        }

        m_ProductController.SendAsynchronousProductMessage( productMessage );
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductCommandLine::HandleVolume
///
/// @brief  This command tests changing the volume setting on the device.
///
////////////////////////////////////////////////////////////////////////////////////////////////
void ProductCommandLine::HandleVolume( const std::list<std::string>& argList,
                                       std::string& response )
{
    const uint32_t minVolume = static_cast <uint32_t>( m_ProductController.m_cachedVolume.properties( ).minlimit( ) );
    const uint32_t maxVolume = static_cast <uint32_t>( m_ProductController.m_cachedVolume.properties( ).maxlimit( ) );
    std::ostringstream usage;
    usage << "Incorrect Usage: product volume [integer from " << minVolume << " to " << maxVolume << "]\r\n";

    if( argList.size( ) != 1 )
    {
        response = usage.str( );
    }
    else
    {
        const std::string& volumeLevelString = argList.front( );
        const uint32_t     volumeLevelValue  = std::atoi( volumeLevelString.c_str( ) );

        if( volumeLevelValue >= minVolume && volumeLevelValue <= maxVolume )
        {
            response  = "The volume will be changed to the level ";
            response +=  volumeLevelString;
            response += ". \r\n";

            auto errFunc = []( FrontDoor::Error error )
            {
                BOSE_ERROR( s_logger, "An error code %d subcode %d and error string <%s> was returned from a frontdoor volume request.",
                            error.code(),
                            error.subcode(),
                            error.message().c_str() );
            };

            AsyncCallback<FrontDoor::Error> errCb( errFunc, m_ProductTask );

            SoundTouchInterface::volume pbVolume;
            pbVolume.set_value( volumeLevelValue );

            BOSE_VERBOSE( s_logger, "Setting FrontDoor volume to %d", pbVolume.value( ) );
            m_ProductController.GetFrontDoorClient( )->SendPut<SoundTouchInterface::volume, FrontDoor::Error>(
                FRONTDOOR_AUDIO_VOLUME_API, pbVolume, {}, errCb );
        }
        else
        {
            response = usage.str( );
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductCommandLine::HandleTestAccessoryUpdate
///
/// @brief  This command triggers the accessory update flow.
///
////////////////////////////////////////////////////////////////////////////////////////////////
void ProductCommandLine::HandleTestAccessoryUpdate( const std::list<std::string>& argList,
                                                    std::string& response )
{
    LpmServiceMessages::IpcAccessorySpeakerSoftwareStatusMessage_t softwareStatus;
    softwareStatus.set_status( ACCESSORY_UPDATE_INSTALLATION_PENDING );
    m_ProductController.m_AccessorySoftwareInstallManager.m_softwareStatusCache = softwareStatus;
    m_ProductController.m_AccessorySoftwareInstallManager.ProceedWithSoftwareUpdate( );

    response  = "Accessory update triggered";
}

////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductCommandLine::HandleTestTransition
///
/// @brief  This command sets or clears the flag to halt the product state machine in the PLAYABLE_TRANSITION
///         state. This supports testing the system behavior when external interactions take place while the
///         system is in the transition state
///         Current implementation supports the PLAYABLE_TRANSITION_NETWORK_STANDBY state only.
///
////////////////////////////////////////////////////////////////////////////////////////////////
void ProductCommandLine::HandleTestTransition( const std::list<std::string>& argList,
                                               std::string& response )
{
    if( argList.size( ) != 1 )
    {
        response = "Currently ";
        response += m_ProductController.GetHaltInPlayableTransitionNetworkStandby( ) ? "" : "not ";
        response += "halting in PLAYABLE_TRANSITION_NETWORK_STANDBY";
    }
    else
    {
        const std::string& argumentString = argList.front( );

        if( argumentString == "on" )
        {
            response  = "Will halt the system in the PLAYABLE_TRANSITION_STANDBY state";

            m_ProductController.m_haltInPlayableTransitionNetworkStandby = true;
        }
        else if( argumentString == "off" )
        {
            m_ProductController.m_haltInPlayableTransitionNetworkStandby = false;
            if( m_ProductController.GetHsm( ).GetCurrentStateId( ) == CUSTOM_PRODUCT_CONTROLLER_STATE_PLAYABLE_TRANSITION_NETWORK_STANDBY )
            {
                ProductMessage productMessage;
                productMessage.mutable_lpmstatus( )->set_systemstate( SYSTEM_STATE_STANDBY );
                m_ProductController.SendAsynchronousProductMessage( productMessage );
                response  = "Releasing";
            }
            else
            {
                response  = "Will release";
            }

            response += " the system halt in the PLAYABLE_TRANSITION_STANDBY state";
        }
        else
        {
            response = "Incorrect Usage: product test_transition [on | off]";
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                           End of the Product Application Namespace                           ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
