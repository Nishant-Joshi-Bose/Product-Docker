////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file      ProductCommandLine.cpp
///
/// @brief     This file contains source code that implements the ProductCommandLine class that is
///            used to set up a command line interface.
///
/// @author    Stuart J. Lumby
///
/// @date      07/15/2017
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
#include "DPrint.h"
#include "CliClient.h"
#include "APTaskFactory.h"
#include "APProductIF.h"
#include "ProductHardwareInterface.h"
#include "ProfessorProductController.h"
#include "ProductCommandLine.h"
#include "AutoLpmServiceMessages.pb.h"

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
typedef CLIClient::CmdPtr                       CommandPointer;
typedef CLIClient::CLICmdDescriptor             CommandDescription;

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// The following declares a DPrint class type object for logging information in this source code
/// file.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
static const DPrint s_logger    { "Product" };

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductCommandLine::GetInstance
///
/// @brief  This static method creates the one and only instance of a ProductCommandLine object.
///         The C++ Version 11 compiler guarantees that only one instance is created in a thread
///         safe way.
///
/// @param mainTask
///
/// @param ProductNotify
///
/// @return This method returns a pointer to a ProductCommandLine object.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
ProductCommandLine* ProductCommandLine::GetInstance( NotifyTargetTaskIF*        mainTask,
                                                     ProductHardwareInterface*  HardwareInterface )
{
    static ProductCommandLine* instance = new ProductCommandLine( mainTask, HardwareInterface );

    BOSE_DEBUG( s_logger, "The instance %8p of the Product Command Line was returned.", instance );

    return instance;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductCommandLine::
///
/// @brief  This method is the ProductCommandLine constructor, which is declared as being private to
///         ensure that only one instance of this class can be created through the class GetInstance
///         method.
///
/// @param  void This method does not take any arguments.
///
/// @return This method does not return anything.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
ProductCommandLine::ProductCommandLine( NotifyTargetTaskIF*        mainTask,
                                        ProductHardwareInterface*  HardwareInterface )
    : m_CommandLineTask( mainTask ),
      m_ProductHardwareInterface( HardwareInterface )

{
    return;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductCommandLine::Run
///
/// @brief  This method starts the main task for the ProductCommandLine instance. The OnEntry method
///         for the ProductCommandLine instance is called just before the main task starts. Also,
///         this main task is used for most of the internal processing for each of the subclass
///         instances.
///
/// @param  void This method does not take any arguments.
///
/// @return This method does not return anything.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductCommandLine::Run( )
{
    m_CommandLineInterface = new CLIClient( "ProductCommandLineInterface" );

    m_CommandLineInterface->Initialize( m_CommandLineTask,
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
/// @brief  This method starts the main task for the ProductCommandLine instance. The OnEntry method
///         for the ProductCommandLine instance is called just before the main task starts. Also,
///         this main task is used for most of the internal processing for each of the subclass
///         instances.
///
/// @param  void This method does not take any arguments.
///
/// @return This method does not return anything.
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
/// @return
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
                                                                             "",
                                                                             "product source [integer | string] "
                                                                             "[This command selects the audio source.] \r\n"
                                                                             "               <hdmi1>                   \r\n"
                                                                             "               <hdmi2>                   \r\n"
                                                                             "               <hdmi3>                   \r\n"
                                                                             "               <hdmi4>                   \r\n"
                                                                             "               <tv>                      \r\n"
                                                                             "               <aux>                     \r\n"
                                                                             "               <optical1>                \r\n"
                                                                             "               <optical2>                \r\n"
                                                                             "               <coax1>                   \r\n"
                                                                             "               <coax2>                   \r\n" ) ) );

    return commands;

    ////////////////////////////////////////////////////////////////////////////////////////////////
    /// The following commands have yet to be created.
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
    /// commands.push_back( static_cast<CommandPointer>( new CommandDescription(ProcessCommand
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
/// @param  command
///
/// @param  arguments
///
/// @param  response
///
/// @return
///
////////////////////////////////////////////////////////////////////////////////////////////////////
int ProductCommandLine::HandleCommand( const std::string&              command,
                                       const std::list< std::string >& arguments,
                                       std::string&                    response ) const
{
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
    else if( command.compare( "product source" ) == 0 )
    {
        if( arguments.size( ) != 1 )
        {
            response  = "Incorrect Usage: product source [integer | string] \r\n";
            response += "                                <hdmi1>            \r\n";
            response += "                                <hdmi2>            \r\n";
            response += "                                <hdmi3>            \r\n";
            response += "                                <hdmi4>            \r\n";
            response += "                                <tv>               \r\n";
            response += "                                <aux>              \r\n";
            response += "                                <optical1>         \r\n";
            response += "                                <optical2>         \r\n";
            response += "                                <coax1>            \r\n";
            response += "                                <coax2>            \r\n";

            return -1;
        }

        IPCSource_t sourceValue;
        std::string sourceString = arguments.front( );

        if( sourceString == "hdmi1" )
        {
            sourceValue.set_source( LPM_IPC_SOURCE_HDMI_1 );
        }
        else if( sourceString == "hdmi2" )
        {
            sourceValue.set_source( LPM_IPC_SOURCE_HDMI_2 );
        }
        else if( sourceString == "hdmi3" )
        {
            sourceValue.set_source( LPM_IPC_SOURCE_HDMI_3 );
        }
        else if( sourceString == "hdmi4" )
        {
            sourceValue.set_source( LPM_IPC_SOURCE_HDMI_4 );
        }
        else if( sourceString == "tv" )
        {
            sourceValue.set_source( LPM_IPC_SOURCE_TV );
        }
        else if( sourceString == "aux" )
        {
            sourceValue.set_source( LPM_IPC_SOURCE_SIDE_AUX );
        }
        else if( sourceString == "optical1" )
        {
            sourceValue.set_source( LPM_IPC_SOURCE_OPTICAL1 );
        }
        else if( sourceString == "optical2" )
        {
            sourceValue.set_source( LPM_IPC_SOURCE_OPTICAL2 );
        }
        else if( sourceString == "coax1" )
        {
            sourceValue.set_source( LPM_IPC_SOURCE_COAX1 );
        }
        else if( sourceString == "coax2" )
        {
            sourceValue.set_source( LPM_IPC_SOURCE_COAX2 );
        }
        else
        {
            sourceValue.set_source( static_cast< LPM_IPC_SOURCE_ID >( std::atoi( sourceString.c_str( ) ) ) );
        }

        if( 0 <= sourceValue.source( ) && sourceValue.source( ) <= LPM_IPC_NUM_SOURCES )
        {
            response  = "The source will be changed to the value ";
            response +=  sourceString.c_str( );
            response += ". \r\n";

            sourceValue.set_open_field( 0 );
            sourceValue.set_status( 0 );

            m_ProductHardwareInterface->SendSourceSelection( sourceValue );
        }
        else
        {
            response  = "Incorrect Usage: product source [integer | string] \r\n";
            response += "                                <hdmi1>            \r\n";
            response += "                                <hdmi2>            \r\n";
            response += "                                <hdmi3>            \r\n";
            response += "                                <hdmi4>            \r\n";
            response += "                                <tv>               \r\n";
            response += "                                <aux>              \r\n";
            response += "                                <optical1>         \r\n";
            response += "                                <optical2>         \r\n";
            response += "                                <coax1>            \r\n";
            response += "                                <coax2>            \r\n";

            return -1;
        }
    }

    return 1;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductCommandLine::ProcessCommand
///
/// @param  arguments
///
/// @param  response
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void ProductCommandLine::ProcessCommand( const std::list< std::string >& arguments,
                                         std::string&                    response ) const
{
    return;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                               End of ProductApp Namespace                                    ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
