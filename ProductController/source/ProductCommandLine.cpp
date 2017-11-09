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
#include "SystemUtils.h"
#include "Utilities.h"
#include "CliClient.h"
#include "APTaskFactory.h"
#include "APProductIF.h"
#include "ProductHardwareInterface.h"
#include "ProfessorProductController.h"
#include "ProductCommandLine.h"
#include "AutoLpmServiceMessages.pb.h"
#include "KeyActions.pb.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
///                          Start of the Product Application Namespace                          ///
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
typedef CLIClient::CmdPtr                       CommandPointer;
typedef CLIClient::CLICmdDescriptor             CommandDescription;

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductCommandLine::GetInstance
///
/// @brief  This static method creates the one and only instance of a ProductCommandLine object.
///         The C++ Version 11 compiler guarantees that only one instance is created in a thread
///         safe way.
///
/// @param NotifyTargetTaskIF* ProductTask
///
/// @param Callback< ProductMessage > ProductNotify
///
/// @param ProductHardwareInterface*  HardwareInterface
///
/// @return This method returns a pointer to a ProductCommandLine object.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
ProductCommandLine* ProductCommandLine::GetInstance( NotifyTargetTaskIF*        ProductTask,
                                                     Callback< ProductMessage > ProductNotify,
                                                     ProductHardwareInterface*  HardwareInterface )
{
    static ProductCommandLine* instance = new ProductCommandLine( ProductTask,
                                                                  ProductNotify,
                                                                  HardwareInterface );

    BOSE_DEBUG( s_logger, "The instance %8p of the Product Command Line was returned.", instance );

    return instance;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductCommandLine::ProductCommandLine
///
/// @param  NotifyTargetTaskIF* ProductTask
///
/// @param  Callback< ProductMessage > ProductNotify
///
/// @param  ProductHardwareInterface*  HardwareInterface
///
/// @return This method returns a pointer to a ProductCommandLine object.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
ProductCommandLine::ProductCommandLine( NotifyTargetTaskIF*        ProductTask,
                                        Callback< ProductMessage > ProductNotify,
                                        ProductHardwareInterface*  HardwareInterface )
    : m_ProductTask( ProductTask ),
      m_ProductNotify( ProductNotify ),
      m_ProductHardwareInterface( HardwareInterface )
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
    m_CommandLineInterface = new CLIClient( "ProductCommandLineInterface" );

    m_CommandLineInterface->Initialize( m_ProductTask,
                                        CommandsList( ),
                                        std::bind( &ProductCommandLine::HandleCommand,
                                                   this,
                                                   std::placeholders::_1,
                                                   std::placeholders::_2,
                                                   std::placeholders::_3 ) );

    BOSE_DEBUG( s_logger, "A command line has been established." );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductCommandLine::Run
///
/// @brief  This method is used to stop the ProductCommandLine from running.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductCommandLine::Stop( )
{
    return;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductCommandLine::CommandsList
///
/// @brief  This method sets up the commands that can be entered by the user through a telnet
///         interface to the device.
///
/// @return This method returns a vector of the available product commands and is sent during
///         initialization of the CLIClient, which is implemented in the Sound Touch library.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
std::vector< CommandPointer > ProductCommandLine::CommandsList( )
{
    std::vector< CommandPointer > commands;

    commands.push_back( static_cast<CommandPointer>( new CommandDescription( "product volume",
                                                                             "This command set the volume to a specified level.",
                                                                             "product volume [integer from 0 to 100]" ) ) );

    commands.push_back( static_cast<CommandPointer>( new CommandDescription( "product mute",
                                                                             "This command mutes or unmutes the volume",
                                                                             "product mute [on | off]" ) ) );

    commands.push_back( static_cast<CommandPointer>( new CommandDescription( "product source",
                                                                             "This command selects the audio source",
                                                                             "product source [tv | adaptiq] " ) ) );

    commands.push_back( static_cast<CommandPointer>( new CommandDescription( "product test_lpm",
                                                                             "This command tests setting the LPM state",
                                                                             "product test_lpm [on | off]" ) ) );

    commands.push_back( static_cast<CommandPointer>( new CommandDescription( "product test_caps",
                                                                             "This command tests setting CAPS to an on or off state",
                                                                             "product test_caps [on | off]" ) ) );

    commands.push_back( static_cast<CommandPointer>( new CommandDescription( "product test_ap",
                                                                             "This command tests setting the audio path to an on or off state",
                                                                             "product test_ap [on | off]" ) ) );

    commands.push_back( static_cast<CommandPointer>( new CommandDescription( "product test_sts",
                                                                             "This command tests setting STS intialization to complete",
                                                                             "product test_sts" ) ) );

    commands.push_back( static_cast<CommandPointer>( new CommandDescription( "product test_bootup",
                                                                             "This command tests setting the device in a boot up state",
                                                                             "product test_bootup" ) ) );

    commands.push_back( static_cast<CommandPointer>( new CommandDescription( "product autowake",
                                                                             "This command tests setting autowake to an on or off state",
                                                                             "product autowake [on | off]" ) ) );

    commands.push_back( static_cast<CommandPointer>( new CommandDescription( "product test_network",
                                                                             "This command tests setting the network to an off, configured, or connected",
                                                                             "product test_network [off | configured | connected]" ) ) );

    commands.push_back( static_cast<CommandPointer>( new CommandDescription( "product test_wifi",
                                                                             "This command tests setting the wifi to an on or off configured state",
                                                                             "product test_wifi [on | off]" ) ) );

    commands.push_back( static_cast<CommandPointer>( new CommandDescription( "product test_freq",
                                                                             "This command tests setting the wifi frequency in kHz",
                                                                             "product test_freq [integer from 0 to 10,000,000 kHz]" ) ) );

    commands.push_back( static_cast<CommandPointer>( new CommandDescription( "product test_voice",
                                                                             "This command tests setting the voice VPA to an on or off configured state",
                                                                             "product test_voice [on | off]" ) ) );

    commands.push_back( static_cast<CommandPointer>( new CommandDescription( "product key",
                                                                             "This command tests sending a key action value",
                                                                             "product key [integer from 0 to 254]" ) ) );

    commands.push_back( static_cast<CommandPointer>( new CommandDescription( "product test_nowplaying",
                                                                             "This command tests sending a now playing active or inactive status",
                                                                             "product test_nowplaying [active | inactive]" ) ) );

    commands.push_back( static_cast<CommandPointer>( new CommandDescription( "product test_power",
                                                                             "This command tests sending a power key action",
                                                                             "product test_power" ) ) );

    commands.push_back( static_cast<CommandPointer>( new CommandDescription( "product test_pairing",
                                                                             "This command tests pairing the device with another speaker",
                                                                             "product test_pairing" ) ) );

    commands.push_back( static_cast<CommandPointer>( new CommandDescription( "product state",
                                                                             "This command returns the current state name and ID.",
                                                                             "product state" ) ) );

    return commands;

    ////////////////////////////////////////////////////////////////////////////////////////////////
    /// The following commands have yet to be created, and were created for Ginger. Some of them
    /// may not be applicable for Professor though.
    ////////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// commands.push_back( static_cast< CommandPointer >( new CommandDescription(
    ///                                                    "product status",
    ///                                                    "This command displays the product server status information.",
    ///                                                    "product status \n" ) ) );
    ///
    /// commands.push_back( static_cast<CommandPointer>( new CommandDescription(
    ///                                                     "product lpm",
    ///                                                     "This command forwards the command line to the LPM.",
    ///                                                     "product lpm ...\n" ) ) );
    ///
    ///commands.push_back( static_cast<CommandPointer>( new CommandDescription( "product setsource",
    ///                                                     "Send Passthrough Source Events to Product Controller",
    ///                                                     "product setsource [shelby | lastwifi | bt | btpair | btclear | local | standby] \n" ) ) );
    ///
    /// commands.push_back( static_cast<CommandPointer>( new CommandDescription(
    ///                                                      "product internalmute",
    ///                                                      "This command internally mutes or unmutes the speaker.",
    ///                                                      "product internalmute [on | off] \n" ) ) );
    ///
    /// commands.push_back( static_cast<CommandPointer>( new CommandDescription(
    ///                                                      "product actsrcipc",
    ///                                                      "Send Parsed IPC Activation Events to A4VServer",
    ///                                                      "product actsrcipc <int>[shelby(6)|lastwifi(7)|bt(8)|local(5)|standby(1)]\n" ) ) );
    ///
    /// commands.push_back( static_cast<CommandPointer>( new CommandDescription(
    ///                                                      "product srcupipc",
    ///                                                      "Send Parsed IPC Source Update to A4VServer",
    ///                                                      "product srcupipc <int>[shelby(6)|lastwifi(7)|bt(8)|local(5)|standby(1)]\n" ) ) );
    ///
    /// commands.push_back( static_cast<CommandPointer>( new CommandDescription(
    ///                                                      "product stp",
    ///                                                      "Forwards the CLI to the STP server task",
    ///                                                      "product stp [request|send|auth|install|reboot]\n" ) ) );
    ///
    /// commands.push_back( static_cast<CommandPointer>( new CommandDescription(
    ///                                                      "product test",
    ///                                                      "Sends different test commands.",
    ///                                                      "product test [apmode | ampfault | ismaster | isnotmaster]\n" ) ) );
    ///
    /// commands.push_back( static_cast<CommandPointer>( new CommandDescription(
    ///                                                      "product radio",
    ///                                                      "Controls the WiFi radio",
    ///                                                      "product radio [wifi|apmode] [on|off|toggle]\n" ) ) );
    ///
    /// commands.push_back( static_cast<CommandPointer>( new CommandDescription(
    ///                                                      "product btdata",
    ///                                                      "Sends the BT device data to LPM.",
    ///                                                      "product btdata <MacAddress> \"<Device Name>\"\n" ) ) );
    ///
    /// commands.push_back( static_cast<CommandPointer>( new CommandDescription( "product updatedisplay",
    ///                                                      "Sends update display data to LPM.",
    ///                                                      "product updatedisplay <state> <progress>\n" ) ) );
    ///
    /// commands.push_back( static_cast<CommandPointer>( new CommandDescription(
    ///                                                      "product pt",
    ///                                                      "Forwards tap commands to other devices.",
    ///                                                      "product pt <dest> \"command\"\n" ) ) );
    ///
    /// commands.push_back( static_cast<CommandPointer>( new CommandDescription(
    ///                                                      "product tc",
    ///                                                      "Sends the IPC Tone Control to LPM (values are int16).",
    ///                                                      "product tc <bass> <treble> <center> <rear>\n" ) ) );
    ///
    /// commands.push_back( static_cast<CommandPointer>( new CommandDescription(
    ///                                                      "product tcr",
    ///                                                      "Sends the IPC Tone Control Response as if it came from LPM (values are int16).",
    ///                                                      "product tcl <bass> <treble> <center> <rear>\n"
    ///                                                      "        <minBass> <minTreble> <minCenter> <minRear>\n"
    ///                                                      "        <maxBass> <maxTreble> <maxCenter> <maxRear>\n"
    ///                                                      "        <stepBass> <stepTreble> <stepCenter> <stepRear>\n" ) ) );
    ///
    /// commands.push_back( static_cast<CommandPointer>( new CommandDescription(
    ///                                                      "product vsync",
    ///                                                      "Sends the IPC AV Sync .",
    ///                                                      "product vsync <sync(uint32)>\n" ) ) );
    ///
    /// commands.push_back( static_cast<CommandPointer>( new CommandDescription(
    ///                                                      "product spklist",
    ///                                                      "Sends the IPC product Speaker List.",
    ///                                                      "product spklist <speakerList(uint32)>\n" ) ) );
    ///
    /// commands.push_back( static_cast<CommandPointer>( new CommandDescription(
    ///                                                      "product latency",
    ///                                                      "      Get or set totalLatency.",
    ///                                                      "product latency [uint32 mSec]\n" ) ) );
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

        std::string volumeLevelString = arguments.front( );
        uint32_t    volumeLevelValue  = std::atoi( volumeLevelString.c_str( ) );

        if( 0 <= volumeLevelValue && volumeLevelValue <= 100 )
        {
            response  = "The volume will be changed to the level ";
            response +=  volumeLevelString;
            response += ". \r\n";

            m_ProductHardwareInterface->SendSetVolume( volumeLevelValue );
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
        if( arguments.size( ) != 1 )
        {
            response = "Incorrect Usage: product mute [on | off] \r\n";

            return -1;
        }

        std::string muteState = arguments.front( );

        if( muteState == "on" )
        {
            response = "The mute will be turned on. \r\n";

            m_ProductHardwareInterface->SendInternalMute( true );
        }
        else if( muteState == "off" )
        {
            response = "The mute will be turned off. \r\n";

            m_ProductHardwareInterface->SendInternalMute( false );
        }
        else
        {
            response = "Incorrect Usage: product mute [on | off] \r\n";

            return -1;
        }
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////
    /// This command tests changing the audio source on the device.
    ////////////////////////////////////////////////////////////////////////////////////////////////
    else if( command.compare( "product source" ) == 0 )
    {
        if( arguments.size( ) != 1 )
        {
            response  = "Incorrect Usage: product source [tv | adaptiq] \r\n";

            return -1;
        }

        LPM_IPC_SOURCE_ID sourceValue;
        std::string       sourceString = arguments.front( );

        if( sourceString == "tv" )
        {
            sourceValue = LPM_IPC_SOURCE_TV;
        }
        else if( sourceString == "adaptiq" )
        {
            sourceValue = LPM_IPC_SOURCE_ADPAPTIQ;
        }
        else
        {
            response  = "Incorrect Usage: product source [tv | adaptiq] \r\n";

            return -1;
        }

        response  = "The source will be changed to the value ";
        response +=  sourceString.c_str( );
        response += ". \r\n";

        m_ProductHardwareInterface->SendSourceSelection( sourceValue );
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

        std::string argumentString = arguments.front( );

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

        std::string argumentString = arguments.front( );

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

        std::string argumentString = arguments.front( );

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
    /// This command tests setting autowake to an on or off state and sending it to the product
    /// controller state machine.
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

        std::string argumentString = arguments.front( );

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
    /// This command tests setting the network to an off, configured, or connected state and
    /// sending it to the product controller state machine. Its actual state is not effected.
    ////////////////////////////////////////////////////////////////////////////////////////////////
    else if( command.compare( "product test_network" ) == 0 )
    {
        if( arguments.size( ) != 1 )
        {
            response = "Incorrect Usage: product test_network [off | configured | connected]";

            return -1;
        }

        std::string argumentString = arguments.front( );

        ProductMessage productMessage;

        if( argumentString == "off" )
        {
            response  = "An network off state test will now be made.";

            productMessage.mutable_networkstatus( )->set_configured( false );
            productMessage.mutable_networkstatus( )->set_connected( false );
            productMessage.mutable_networkstatus( )->set_networktype( ProductNetworkStatus_ProductNetworkType_Unknown );

            IL::BreakThread( std::bind( m_ProductNotify, productMessage ), m_ProductTask );
        }
        else if( argumentString == "configured" )
        {
            response  = "An network configured state test will now be made.";

            productMessage.mutable_networkstatus( )->set_configured( true );
            productMessage.mutable_networkstatus( )->set_connected( false );
            productMessage.mutable_networkstatus( )->set_networktype( ProductNetworkStatus_ProductNetworkType_Wired );

            IL::BreakThread( std::bind( m_ProductNotify, productMessage ), m_ProductTask );
        }
        else if( argumentString == "connected" )
        {
            response  = "An network connected state test will now be made.";

            productMessage.mutable_networkstatus( )->set_configured( true );
            productMessage.mutable_networkstatus( )->set_connected( true );
            productMessage.mutable_networkstatus( )->set_networktype( ProductNetworkStatus_ProductNetworkType_Wired );

            IL::BreakThread( std::bind( m_ProductNotify, productMessage ), m_ProductTask );
        }
        else
        {
            response = "Incorrect Usage: product test_network [off | configured | connected]";

            return -1;
        }

        IL::BreakThread( std::bind( m_ProductNotify, productMessage ), m_ProductTask );
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////
    /// This command tests setting a wifi network to an off or off configured state and sending it
    /// to the product controller state machine. Its actual state is not effected.
    ////////////////////////////////////////////////////////////////////////////////////////////////
    else if( command.compare( "product test_wifi" ) == 0 )
    {
        if( arguments.size( ) != 1 )
        {
            response = "Incorrect Usage: product test_wifi [on | off]";

            return -1;
        }

        std::string argumentString = arguments.front( );

        ProductMessage productMessage;

        if( argumentString == "on" )
        {
            response  = "A wifi network configured state test will now be made.";

            productMessage.mutable_wirelessstatus( )->set_configured( false );
            productMessage.mutable_wirelessstatus( )->set_frequencykhz( 0 );

            IL::BreakThread( std::bind( m_ProductNotify, productMessage ), m_ProductTask );
        }
        else if( argumentString == "off" )
        {
            response  = "A wifi network unconfigured state test will now be made.";

            productMessage.mutable_wirelessstatus( )->set_configured( true );
            productMessage.mutable_wirelessstatus( )->set_frequencykhz( 0 );

            IL::BreakThread( std::bind( m_ProductNotify, productMessage ), m_ProductTask );
        }
        else
        {
            response = "Incorrect Usage: product test_wifi [on | off]";

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
            response = "Incorrect Usage: product test_freq [integer from 0 to 10,000,000] kHz";

            return -1;
        }

        std::string argumentString = arguments.front( );
        uint32_t    frequencyValue = std::atoi( argumentString.c_str( ) );

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
            response = "Incorrect Usage: product test_freq [integer from 0 to 10,000,000] kHz";

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

        std::string argumentString = arguments.front( );

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

        std::string argumentString = arguments.front( );
        uint32_t    keyActionValue = std::atoi( argumentString.c_str( ) );

        if( 0 <= keyActionValue && keyActionValue <= 254 )
        {
            response  = "The key action value ";
            response +=  keyActionValue;
            response += " will be sent to the product controller state machine.\r\n";

            ProductMessage productMessage;
            productMessage.mutable_keydata( )->set_action( static_cast< KeyActionPb::KEY_ACTION >( keyActionValue ) );

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
        productMessage.set_power( true );

        response  = "A power key will be sent as a test to the product controller state machine.";

        IL::BreakThread( std::bind( m_ProductNotify, productMessage ), m_ProductTask );
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// This command tests sending a playback request to the product controller state machine.
    ///////////////////////////////////////////////////////////////////////////////////////////////
    else if( command.compare( "product test_nowplaying" ) == 0 )
    {
        if( arguments.size( ) != 1 )
        {
            response = "Incorrect Usage: product test_nowplaying [active | inactive]";

            return -1;
        }

        std::string argumentString = arguments.front( );

        ProductMessage productMessage;

        if( argumentString == "active" )
        {
            response  = "A now playback active status test will now be made.";

            productMessage.mutable_nowplayingstatus( )->set_state( ProductNowPlayingStatus_ProductNowPlayingState_Active );
        }
        else if( argumentString == "inactive" )
        {
            response  = "A now playback inactive status test will now be made.";

            productMessage.mutable_nowplayingstatus( )->set_state( ProductNowPlayingStatus_ProductNowPlayingState_Inactive );
        }
        else
        {
            response = "Incorrect Usage: product test_nowplaying [active | inactive]";

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
        productMessage.mutable_keydata( )->set_action( KeyActionPb::KEY_ACTION_PAIR_SPEAKERS );

        response  = "An attempt to pair with another speaker to this device will be made.";

        IL::BreakThread( std::bind( m_ProductNotify, productMessage ), m_ProductTask );
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////
    /// This command returns the current state name and ID.
    ////////////////////////////////////////////////////////////////////////////////////////////////
    else if( command.compare( "product state" ) == 0 )
    {
        Hsm::STATE  stateId   = ProfessorProductController::GetInstance( )->GetHsm( ).GetCurrentState( )->GetId( );
        std::string stateName = ProfessorProductController::GetInstance( )->GetHsm( ).GetCurrentState( )->GetName( );

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
