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
#include "Intents.h"
#include "IntentHandler.h"
#include "CustomProductLpmHardwareInterface.h"
#include "ProfessorProductController.h"
#include "ProductCommandLine.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
///                          Start of the Product Application Namespace                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
namespace ProductApp
{

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// The following constants define FrontDoor endpoints used by the VolumeManager
///
///////////////////////////////////////////////////////////////////////////////////////////////////
constexpr char  FRONTDOOR_AUDIO_VOLUME[ ]           = "/audio/volume";

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductCommandLine::ProductCommandLine
///
/// @param  ProfessorProductController& ProductController
///
////////////////////////////////////////////////////////////////////////////////////////////////////
ProductCommandLine::ProductCommandLine( ProfessorProductController& ProductController )

    : m_ProductController( ProductController ),
      m_ProductTask( ProductController.GetTask( ) ),
      m_ProductNotify( ProductController.GetMessageHandler( ) ),
      m_ProductLpmHardwareInterface( ProductController.GetLpmHardwareInterface( ) ),
      m_CommandLineInterface( new CLIClient( "ProductCommandLineInterface" ) )
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductCommandLine::Run
///
/// @brief  This method starts and runs the ProductCommandLine instance on the product controller
///         task, using a CLIClient, which is implemented in the Sound Touch library. A telnet to
///         the device on port 17000 will allow the user to issue commands after CLIClient is
///         initialized.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductCommandLine::Run( )
{
    m_CommandLineInterface->Initialize( m_ProductTask,
                                        GetCommandsList( ),
                                        std::bind( &ProductCommandLine::HandleCommand,
                                                   this,
                                                   std::placeholders::_1,
                                                   std::placeholders::_2,
                                                   std::placeholders::_3 ) );

    BOSE_DEBUG( s_logger, "A command line has been established." );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name  ProductCommandLine::Stop
///
/// @brief This method is used to stop the ProductCommandLine from running.
///
/// @todo  Resources, memory, or any client server connections that may need to be released by
///        this module when stopped will need to be determined.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductCommandLine::Stop( )
{
    return;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductCommandLine::GetCommandsList
///
/// @brief  This method sets up the commands that can be entered by the user through a telnet
///         interface to the device.
///
/// @return This method returns a vector of the available product commands and is sent during
///         initialization of the CLIClient, which is implemented in the Sound Touch library.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
std::vector< CommandPointer > ProductCommandLine::GetCommandsList( )
{
    std::vector< CommandPointer > commands;

    commands.push_back( static_cast<CommandPointer>( new CommandDescription( "product autowake",
                                                                             "This command tests setting autowake to an on or off state.",
                                                                             "\t product autowake [on | off] \t\t" ) ) );

    commands.push_back( static_cast<CommandPointer>( new CommandDescription( "product key",
                                                                             "This command tests sending a key action value.",
                                                                             "\t\t product key [int from 0 to 254] \t" ) ) );

    commands.push_back( static_cast<CommandPointer>( new CommandDescription( "product mute",
                                                                             "This command mutes or unmutes the volume.",
                                                                             "\t\t product mute [on | off] \t\t" ) ) );

    commands.push_back( static_cast<CommandPointer>( new CommandDescription( "product source",
                                                                             "This command selects the audio source.",
                                                                             "\t\t product source [tv | st] \t\t" ) ) );

    commands.push_back( static_cast<CommandPointer>( new CommandDescription( "product state",
                                                                             "This command returns the current state name and ID.",
                                                                             "\t\t product state \t\t\t\t" ) ) );

    commands.push_back( static_cast<CommandPointer>( new CommandDescription( "product test_ap",
                                                                             "This command tests setting the audio path to an on or off state.",
                                                                             "\t product test_ap [on | off] \t\t" ) ) );

    commands.push_back( static_cast<CommandPointer>( new CommandDescription( "product test_bootup",
                                                                             "This command tests setting the device in a boot up state.",
                                                                             "\t product test_bootup \t\t\t" ) ) );

    commands.push_back( static_cast<CommandPointer>( new CommandDescription( "product boot_status",
                                                                             "This command outputs the status of the boot up state.",
                                                                             "\t product boot_status \t\t\t" ) ) );

    commands.push_back( static_cast<CommandPointer>( new CommandDescription( "product test_caps",
                                                                             "This command tests setting CAPS to an on or off state.",
                                                                             "\t product test_caps [on | off] \t\t" ) ) );

    commands.push_back( static_cast<CommandPointer>( new CommandDescription( "product test_freq",
                                                                             "This command tests setting the wifi frequency in kHz.",
                                                                             "\t product test_freq [int from 0 to 10M] \t" ) ) );

    commands.push_back( static_cast<CommandPointer>( new CommandDescription( "product test_lpm",
                                                                             "This command tests setting the LPM state.",
                                                                             "\t product test_lpm [on | off] \t\t" ) ) );

    commands.push_back( static_cast<CommandPointer>( new CommandDescription( "product test_nowplaying",
                                                                             "This command tests sending a now playing active or inactive status.",
                                                                             " product test_nowplaying [act | inact] \t" ) ) );

    commands.push_back( static_cast<CommandPointer>( new CommandDescription( "product test_pairing",
                                                                             "This command tests pairing the device with another speaker.",
                                                                             "\t product test_pairing \t\t\t" ) ) );

    commands.push_back( static_cast<CommandPointer>( new CommandDescription( "product test_power",
                                                                             "This command tests sending a power key action.",
                                                                             "\t product test_power \t\t\t" ) ) );

    commands.push_back( static_cast<CommandPointer>( new CommandDescription( "product test_sts",
                                                                             "This command tests setting STS intialization to complete.",
                                                                             "\t product test_sts \t\t\t" ) ) );

    commands.push_back( static_cast<CommandPointer>( new CommandDescription( "product test_voice",
                                                                             "This command tests setting the voice VPA to an on or off configured state.",
                                                                             "\t product test_voice [on | off] \t\t" ) ) );

    commands.push_back( static_cast<CommandPointer>( new CommandDescription( "product volume",
                                                                             "This command sets the volume to a specified level.",
                                                                             "\t\t product volume [int from 0 to 100] \t" ) ) );

    return commands;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductCommandLine::HandleCommand
///
/// @param  std::string& command
///
/// @param  std::list< std::string >& arguments
///
/// @param  std::string& response
///
/// @return This method returns -1 on an error or 1 if successful.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
int ProductCommandLine::HandleCommand( const std::string&              command,
                                       const std::list< std::string >& arguments,
                                       std::string&                    response ) const
{
    ////////////////////////////////////////////////////////////////////////////////////////////////
    /// This command tests changing the volume setting on the device.
    ////////////////////////////////////////////////////////////////////////////////////////////////
    if( command.compare( "product volume" ) == 0 )
    {
        if( arguments.size( ) != 1 )
        {
            response = "Incorrect Usage: product volume [integer from 0 to 100] \r\n";

            return -1;
        }

        const std::string& volumeLevelString = arguments.front( );
        const uint32_t     volumeLevelValue  = std::atoi( volumeLevelString.c_str( ) );

        if( 0 <= volumeLevelValue && volumeLevelValue <= 100 )
        {
            response  = "The volume will be changed to the level ";
            response +=  volumeLevelString;
            response += ". \r\n";

            auto errFunc = []( const FrontDoor::Error & e )
            {
                BOSE_ERROR( s_logger, "Error setting FrontDoor mute" );
            };
            auto respFunc = [ this ]( SoundTouchInterface::volume v )
            {
                BOSE_INFO( s_logger, "Volume set to %d, mute set to %d", v.value( ), v.muted( ) );
            };

            AsyncCallback<SoundTouchInterface::volume> respCb( respFunc, m_ProductTask );
            AsyncCallback<FrontDoor::Error> errCb( errFunc, m_ProductTask );

            SoundTouchInterface::volume pbVolume;
            pbVolume.set_value( volumeLevelValue );

            BOSE_VERBOSE( s_logger, "Setting FrontDoor mute to %d", pbVolume.muted( ) );
            m_ProductController.GetFrontDoorClient( )->SendPut<SoundTouchInterface::volume, FrontDoor::Error>(
                ProductApp::FRONTDOOR_AUDIO_VOLUME, pbVolume, respFunc, errCb );
        }
        else
        {
            response = "Incorrect Usage: product volume [integer from 0 to 100] \r\n";

            return -1;
        }
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////
    /// This command tests changing the mute setting on the device.
    ////////////////////////////////////////////////////////////////////////////////////////////////
    else if( command.compare( "product mute" ) == 0 )
    {
        bool muteStateValue;
        if( arguments.size( ) != 1 )
        {
            response = "Incorrect Usage: product mute [on | off] \r\n";

            return -1;
        }

        const std::string& muteState = arguments.front( );

        if( muteState == "on" )
        {
            muteStateValue = "The mute will be turned on. \r\n";

            muteStateValue = true;
        }
        else if( muteState == "off" )
        {
            response = "The mute will be turned off. \r\n";

            muteStateValue = false;
        }
        else
        {
            response = "Incorrect Usage: product mute [on | off] \r\n";

            return -1;
        }

        auto errFunc = []( const FrontDoor::Error & e )
        {
            BOSE_ERROR( s_logger, "Error setting FrontDoor mute" );
        };
        auto respFunc = [ this ]( SoundTouchInterface::volume v )
        {
            BOSE_INFO( s_logger, "Volume set to %d, mute set to %d", v.value( ), v.muted( ) );
        };

        AsyncCallback<SoundTouchInterface::volume> respCb( respFunc, m_ProductTask );
        AsyncCallback<FrontDoor::Error> errCb( errFunc, m_ProductTask );

        SoundTouchInterface::volume pbVolume;
        pbVolume.set_muted( muteStateValue );

        BOSE_VERBOSE( s_logger, "Setting FrontDoor mute to %d", pbVolume.muted( ) );
        m_ProductController.GetFrontDoorClient( )->SendPut<SoundTouchInterface::volume, FrontDoor::Error>(
            ProductApp::FRONTDOOR_AUDIO_VOLUME, pbVolume, respFunc, errCb );
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////
    /// This command tests changing the audio source on the device.
    ////////////////////////////////////////////////////////////////////////////////////////////////
    else if( command.compare( "product source" ) == 0 )
    {
        if( arguments.size( ) != 1 )
        {
            response  = "Incorrect Usage: product source [tv | st] \r\n";

            return -1;
        }

        const std::string& sourceString = arguments.front( );

        if( sourceString == "tv" )
        {
            KeyHandlerUtil::ActionType_t startTvPlayback = static_cast< KeyHandlerUtil::ActionType_t >( Action::ACTION_TV );
            ProductMessage message;
            message.set_action( startTvPlayback );

            IL::BreakThread( std::bind( m_ProductController.GetMessageHandler( ),
                                        message ),
                             GetTask( ) );
        }
        else if( sourceString == "st" )
        {
            KeyHandlerUtil::ActionType_t startSoundTouchPlayback = static_cast< KeyHandlerUtil::ActionType_t >( Action::ACTION_SOUNDTOUCH );
            ProductMessage message;
            message.set_action( startSoundTouchPlayback );

            IL::BreakThread( std::bind( m_ProductController.GetMessageHandler( ),
                                        message ),
                             GetTask( ) );
        }
        else
        {
            response  = "Incorrect Usage: product source [tv | st] \r\n";
            return -1;
        }

        response  = "The source will be changed to the value ";
        response +=  sourceString.c_str( );
        response += ". \r\n";
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////
    /// This command tests setting the LPM to an on or off state and sending it to the product
    /// controller state machine. Its actual state is not effected.
    ////////////////////////////////////////////////////////////////////////////////////////////////
    else if( command.compare( "product test_lpm" ) == 0 )
    {
        if( arguments.size( ) != 1 )
        {
            response = "Incorrect Usage: product test_lpm [on | off] \r\n";

            return -1;
        }

        const std::string& argumentString = arguments.front( );

        ProductMessage productMessage;

        if( argumentString == "on" )
        {
            response  = "An LPM on state test will now be made.";

            productMessage.mutable_lpmstatus( )->set_connected( true );
        }
        else if( argumentString == "off" )
        {
            response  = "An LPM off state test will now be made.";

            productMessage.mutable_lpmstatus( )->set_connected( false );
        }
        else
        {
            response = "Incorrect Usage: product test_lpm [on | off] \r\n";

            return -1;
        }

        IL::BreakThread( std::bind( m_ProductNotify, productMessage ), m_ProductTask );
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////
    /// This command tests setting the Content Audio Playback Service or CAPS to an on or off
    /// state and sending it to the product controller state machine. Its actual state is not
    /// effected.
    ////////////////////////////////////////////////////////////////////////////////////////////////
    else if( command.compare( "product test_caps" ) == 0 )
    {
        if( arguments.size( ) != 1 )
        {
            response = "Incorrect Usage: product test_caps [on | off]";

            return -1;
        }

        const std::string& argumentString = arguments.front( );

        ProductMessage productMessage;

        if( argumentString == "on" )
        {
            response  = "A CAPS on state test will now be made.";

            productMessage.mutable_capsstatus( )->set_initialized( true );
        }
        else if( argumentString == "off" )
        {
            response  = "A CAPS off state test will now be made.";

            productMessage.mutable_capsstatus( )->set_initialized( false );
        }
        else
        {
            response = "Incorrect Usage: product test_caps [on | off]";

            return -1;
        }

        IL::BreakThread( std::bind( m_ProductNotify, productMessage ), m_ProductTask );
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////
    /// This command tests setting the Audio Path to an on or off state and sending it to the
    /// product controller state machine. Its actual state is not effected.
    ////////////////////////////////////////////////////////////////////////////////////////////////
    else if( command.compare( "product test_ap" ) == 0 )
    {
        if( arguments.size( ) != 1 )
        {
            response = "Incorrect Usage: product test_ap [on | off]";
        }

        const std::string& argumentString = arguments.front( );

        ProductMessage productMessage;

        if( argumentString == "on" )
        {
            response  = "An audio path connected on state test will now be made.";

            productMessage.mutable_audiopathstatus( )->set_connected( true );
        }
        else if( argumentString == "off" )
        {
            response  = "An audio path connected off state test will now be made.";

            productMessage.mutable_audiopathstatus( )->set_connected( false );
        }
        else
        {
            response = "Incorrect Usage: product test_ap [on | off]";

            return -1;
        }

        IL::BreakThread( std::bind( m_ProductNotify, productMessage ), m_ProductTask );
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////
    /// This command tests setting STS initialization to completed.
    ////////////////////////////////////////////////////////////////////////////////////////////////
    else if( command.compare( "product test_sts" ) == 0 )
    {
        ProductMessage productMessage;
        productMessage.mutable_stsinterfacestatus( )->set_initialized( true );

        IL::BreakThread( std::bind( m_ProductNotify, productMessage ), m_ProductTask );

        response  = "A test setting the STS initialization to complete has been made.";
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////
    /// This command tests setting the device to a boot up state.
    ////////////////////////////////////////////////////////////////////////////////////////////////
    else if( command.compare( "product test_bootup" ) == 0 )
    {
        {
            ProductMessage productMessage;
            productMessage.mutable_lpmstatus( )->set_connected( true );

            IL::BreakThread( std::bind( m_ProductNotify, productMessage ), m_ProductTask );
        }
        {
            ProductMessage productMessage;
            productMessage.mutable_capsstatus( )->set_initialized( true );

            IL::BreakThread( std::bind( m_ProductNotify, productMessage ), m_ProductTask );
        }
        {
            ProductMessage productMessage;
            productMessage.mutable_audiopathstatus( )->set_connected( true );

            IL::BreakThread( std::bind( m_ProductNotify, productMessage ), m_ProductTask );
        }
        {
            ProductMessage productMessage;
            productMessage.mutable_stsinterfacestatus( )->set_initialized( true );

            IL::BreakThread( std::bind( m_ProductNotify, productMessage ), m_ProductTask );
        }

        response  = "Requests to set the device in a boot up state have been made.";
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////
    /// This command outputs the boot up status of the device.
    ////////////////////////////////////////////////////////////////////////////////////////////////
    else if( command.compare( "product boot_status" ) == 0 )
    {
        std::string LpmConnected( m_ProductController.IsLpmReady( )                     ? "true" : "false" );
        std::string CapsInitialized( m_ProductController.IsCAPSReady( )                 ? "true" : "false" );
        std::string audioPathConnected( m_ProductController.IsAudioPathReady( )         ? "true" : "false" );
        std::string StsInitialized( m_ProductController.IsSTSReady( )                   ? "true" : "false" );
        std::string SoftwareUpdateReady( m_ProductController.IsSoftwareUpdateReady( )   ? "true" : "false" );
        std::string SassInitialized( m_ProductController.IsSassReady( )                 ? "true" : "false" );
        std::string bluetoothInitialized( m_ProductController.IsBluetoothModuleReady( ) ? "true" : "false" );

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
    /// This command tests setting autowake to an on or off state and sending it to the product
    /// controller state machine.
    ////////////////////////////////////////////////////////////////////////////////////////////////
    else if( command.compare( "product autowake" ) == 0 )
    {
        if( arguments.size( ) != 1 )
        {
            response = "Incorrect Usage: product autowake [on | off]";

            return -1;
        }

        const std::string& argumentString = arguments.front( );

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

            return -1;
        }

        IL::BreakThread( std::bind( m_ProductNotify, productMessage ), m_ProductTask );
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////
    /// This command to tests setting the Content Audio Playback Service to an on or off state and
    /// sending it to the product controller state machine. Its actual state is not effected.
    ////////////////////////////////////////////////////////////////////////////////////////////////
    else if( command.compare( "product test_freq" ) == 0 )
    {
        if( arguments.size( ) != 1 )
        {
            response = "Incorrect Usage: product test_freq [int from 0 to 10M]";

            return -1;
        }

        const std::string& argumentString = arguments.front( );
        const uint32_t     frequencyValue = std::atoi( argumentString.c_str( ) );

        if( 0 <= frequencyValue && frequencyValue <= 10000000 )
        {
            response  = "The wifi frequncy value ";
            response +=  frequencyValue;
            response += "kHz will be sent to the product controller state machine.\r\n";

            ProductMessage productMessage;
            productMessage.mutable_wirelessstatus( )->set_configured( true );
            productMessage.mutable_wirelessstatus( )->set_frequencykhz( frequencyValue );

            IL::BreakThread( std::bind( m_ProductNotify, productMessage ), m_ProductTask );
        }
        else
        {
            response = "Incorrect Usage: product test_freq [int from 0 to 10M]";

            return -1;
        }
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////
    /// This command tests setting the voice configuration for a Virtual Personal Assistant or VPA
    /// to an off or off state and sending it to the product controller state machine. Its actual
    /// state is not effected.
    ////////////////////////////////////////////////////////////////////////////////////////////////
    else if( command.compare( "product test_voice" ) == 0 )
    {
        if( arguments.size( ) != 1 )
        {
            response = "Incorrect Usage: product test_voice [on | off]";

            return -1;
        }

        const std::string& argumentString = arguments.front( );

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

            return -1;
        }

        IL::BreakThread( std::bind( m_ProductNotify, productMessage ), m_ProductTask );
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////
    /// This command tests sending a key action value to the product controller state machine.
    ////////////////////////////////////////////////////////////////////////////////////////////////
    else if( command.compare( "product key" ) == 0 )
    {
        if( arguments.size( ) != 1 )
        {
            response = "Incorrect Usage: product key [integer from 0 to 254]";

            return -1;
        }

        const std::string& argumentString = arguments.front( );
        const uint32_t     keyActionValue = std::atoi( argumentString.c_str( ) );

        if( 0 <= keyActionValue && keyActionValue <= 254 )
        {
            response  = "The key action value ";
            response +=  keyActionValue;
            response += " will be sent to the product controller state machine.\r\n";

            ProductMessage productMessage;
            productMessage.set_action( static_cast< uint32_t >( keyActionValue ) );

            IL::BreakThread( std::bind( m_ProductNotify, productMessage ), m_ProductTask );
        }
        else
        {
            response = "Incorrect Usage: product key [integer from 0 to 254]";

            return -1;
        }
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////
    /// This command tests sending a power key press to the product controller state machine.
    ////////////////////////////////////////////////////////////////////////////////////////////////
    else if( command.compare( "product test_power" ) == 0 )
    {
        ProductMessage productMessage;
        productMessage.set_action( static_cast< uint32_t >( Action::POWER_TOGGLE ) );

        IL::BreakThread( std::bind( m_ProductNotify, productMessage ), m_ProductTask );
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// This command tests sending a playback request to the product controller state machine.
    ///////////////////////////////////////////////////////////////////////////////////////////////
    else if( command.compare( "product test_nowplaying" ) == 0 )
    {
        if( arguments.size( ) != 1 )
        {
            response = "Incorrect Usage: product test_nowplaying [act | inact]";

            return -1;
        }

        const std::string& argumentString = arguments.front( );

        ProductMessage productMessage;

        if( argumentString == "act" )
        {
            response  = "A now playback active status test will now be made.";

            productMessage.mutable_nowplayingstatus( )->set_state( ProductNowPlayingStatus_ProductNowPlayingState_Active );
        }
        else if( argumentString == "inact" )
        {
            response  = "A now playback inactive status test will now be made.";

            productMessage.mutable_nowplayingstatus( )->set_state( ProductNowPlayingStatus_ProductNowPlayingState_Inactive );
        }
        else
        {
            response = "Incorrect Usage: product test_nowplaying [act | inact]";

            return -1;
        }

        IL::BreakThread( std::bind( m_ProductNotify, productMessage ), m_ProductTask );
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////
    /// This command tests pairing the device with another speaker.
    ////////////////////////////////////////////////////////////////////////////////////////////////
    else if( command.compare( "product test_pairing" ) == 0 )
    {
        ProductMessage productMessage;
        productMessage.set_action( static_cast< uint32_t >( Action::ACTION_START_PAIR_SPEAKERS ) );

        response  = "An attempt to pair with another speaker to this device will be made.";

        IL::BreakThread( std::bind( m_ProductNotify, productMessage ), m_ProductTask );
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////
    /// This command returns the current state name and ID.
    ////////////////////////////////////////////////////////////////////////////////////////////////
    else if( command.compare( "product state" ) == 0 )
    {
        Hsm::STATE  stateId   = m_ProductController.GetHsm( ).GetCurrentState( )->GetId( );
        std::string stateName = m_ProductController.GetHsm( ).GetCurrentState( )->GetName( );

        response  = "The current state name is ";
        response += stateName;
        response += " with ID ";
        response += std::to_string( static_cast< unsigned int >( stateId ) );
        response += ".";
    }

    return 1;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                           End of the Product Application Namespace                           ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
