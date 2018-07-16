/*
 * The ProductCliClient class implements the Product's CLI commands.
 */

#include <endian.h>
#include <iostream>
#include <fstream>

#include "ProductCliClient.h"
#include "DPrint.h"
#include "DirUtils.h"
#include "SystemUtils.h"
#include "LpmClientFactory.h"

namespace
{

DPrint s_logger{ "ProductCliClient" };

template <typename UINT>
bool ToInteger( std::string const& str, UINT& result )
{
    // Return true if 'str' is a well-formed string representation of an
    // integer whose value will fit in 'result'.
    try
    {
        size_t end = 0;
        auto v = std::stoull( str, &end );
        result = v;
        if( result != v )
            return false;           // truncation/overflow
        return end == str.size();
    }
    catch( std::invalid_argument const& )
    {
        result = 0;
        return false;
    }
}

} // namespace

ProductCliClient::ProductCliClient()
    : m_cliClient{ "product" }
{
    BOSE_INFO( s_logger, __func__ );
}

void ProductCliClient::Initialize( NotifyTargetTaskIF* task )
{
    BOSE_INFO( s_logger, __func__ );

    std::vector<CLIClient::CmdPtr> cmds;

    using Cmd = CLIClient::CLICmdDescriptor;

    m_task = task;

    cmds.emplace_back( std::make_shared<Cmd>
                       ( "mfgdata",
                         "Show the manufacturing data",
                         "mfgdata" ) );

    cmds.emplace_back( std::make_shared<Cmd>
                       ( "backlight",
                         "Adjust the LCD back light level",
                         "backlight [level]" ) );

    cmds.emplace_back( std::make_shared<Cmd>
                       ( "lightsensor",
                         "Get the light sensor LUX value",
                         "light sensor" ) );

    cmds.emplace_back( std::make_shared<Cmd>
                       ( "amp",
                         "Get/set amplifier state",
                         "amp [on|off|mute|unmute]" ) );

    cmds.emplace_back( std::make_shared<Cmd>
                       ( "lcd",
                         "Manage the lcd hardware",
                         "lcd" ) );

    m_cliClient.Initialize( task, cmds,
                            [this]( std::string const & cmd,
                                    CLIClient::StringListType & argList,
                                    std::string & response )
    {
        return HandleCommand( cmd, argList, response );
    } );
    m_lpmClient = LpmClientFactory::Create( "ProductCliLpmClient", task );
    m_lpmClient->Connect( []( bool connected )
    {
        if( connected )
            BOSE_INFO( s_logger, "lpmClient connected" );
        else
            BOSE_WARNING( s_logger, "lpmClient not connected" );
    } );
}

bool ProductCliClient::HandleCommand( std::string const& cmd,
                                      CLIClient::StringListType& argList,
                                      std::string& response )
{
    BOSE_INFO( s_logger, "HandleCommand '%s'", cmd.c_str() );

    if( cmd == "mfgdata" )
    {
        CliCmdMfgData( argList, response );
        return true;
    }

    if( cmd == "backlight" )
    {
        CliCmdBackLight( argList, response );
        return true;
    }

    if( cmd == "lightsensor" )
    {
        CliCmdLightSensor( argList, response );
        return true;
    }

    if( cmd == "amp" )
    {
        CliCmdAmp( argList, response );
        return true;
    }

    if( cmd == "lcd" )
    {
        CliCmdLcd( argList, response );
        return true;
    }

    response = "Internal error: " + cmd;
    return false;
}

void ProductCliClient::CliCmdMfgData( CLIClient::StringListType& argList,
                                      std::string& response )
{
    if( !argList.empty() )
    {
        response = "Wrong usage";
        return;
    }
    auto f = SystemUtils::ReadFile( "/persist/mfg_data.json" );
    if( f )
    {
        response = *f;
    }
    else
    {
        response = "No manufacturing data";
    }
}

void ProductCliClient::CliCmdBackLight( CLIClient::StringListType& argList,
                                        std::string& response )
{
    if( argList.size() == 0 )
    {
        m_lpmClient->GetBackLight( [this]( IpcBackLight_t const & rsp )
        {
            std::ostringstream ss;
            ss << rsp.value() << "%";
            BOSE_LOG( INFO, ss.str() );
            m_cliClient.SendAsyncResponse( ss.str() );

        } );

        return;
    }// If there is no argument

    if( argList.size() == 1 )
    {
        uint8_t intensity = 0;
        auto&   arg       = argList.front();

        if( !ToInteger( arg, intensity ) )
        {
            response = "Malformed integer: " + arg;
            return;
        }

        if( intensity > 100 )
        {
            response = "back light intensity must between 0..100";
            return;
        }

        IpcBackLight_t backlight;
        backlight.set_value( intensity );
        m_lpmClient->SetBackLight( backlight );

        return;
    }// If there is 1 argument

    response = "usage: backlight [0..100]";
    return;
}// ProductCliClient::CliBackLight

void ProductCliClient::CliCmdLightSensor( CLIClient::StringListType& argList,
                                          std::string& response )
{
    if( argList.size() == 0 )
    {
        m_lpmClient->GetLightSensor( [this]( IpcLightSensor_t const & rsp )
        {
            std::ostringstream ss;
            int                lux_decimal    = ( int )( be16toh( rsp.lux_decimal_value() ) );
            int                lux_fractional = ( int )( be16toh( rsp.lux_fractional_value() ) );

            ss << lux_decimal << "." << lux_fractional << " LUX";
            BOSE_LOG( INFO, ss.str() );
            m_cliClient.SendAsyncResponse( ss.str() );

        } );

        return;
    }// If there is no argument

    response = "usage: lightsensor";
    return;
}// ProductCliClient::CliLightSensor

void ProductCliClient::CliCmdAmp( CLIClient::StringListType& argList,
                                  std::string& response )
{
    if( argList.empty() )
    {
        BOSE_LOG( INFO, "Get amplifier status..." );
        m_lpmClient->GetAmp( [this]( IpcAmp_t const & rsp )
        {
            std::ostringstream ss;
            ss << "amplifier on=" << rsp.on()
               << " mute=" << rsp.mute();

            auto const& msg = ss.str();

            BOSE_LOG( INFO, msg );
            m_cliClient.SendAsyncResponse( msg );
        } );
        response = "Sent request for amplifier status";
    }
    else
    {
        IpcAmp_t req;
        for( auto& arg : argList )
        {
            if( arg == "on" )
            {
                req.set_on( true );
                continue;
            }
            if( arg == "off" )
            {
                req.set_on( false );
                continue;
            }
            if( arg == "mute" )
            {
                req.set_mute( true );
                continue;
            }
            if( arg == "unmute" )
            {
                req.set_mute( false );
                continue;
            }
            response = "Expected on|off|mute|unmute got '" + arg + '\'';
            return;
        }
        m_lpmClient->SetAmp( req );
        response = "Sent request to set amplifier state";
    }
}

void ProductCliClient::CliCmdLcd( CLIClient::StringListType& argList,
                                  std::string& response )
{
    const std::string str_usage                    = "usage";
    const std::string display_controller_file_name = "/sys/devices/soc/7af6000.spi/spi_master/spi6/spi6.1/graphics/fb1/send_command";
    auto& arg                                      = ( argList.size() == 1 ) ? argList.front() : str_usage;

    if( DirUtils::DoesFileExist( display_controller_file_name ) == false )
    {
        response = "error: can't find file: " + display_controller_file_name + " - " + strerror( errno );
        BOSE_LOG( ERROR, response );
        return;
    }

    std::ofstream display_controller_stream( display_controller_file_name );

    if( display_controller_stream.is_open() == false )
    {
        response = "error: failed to open file: " + display_controller_file_name + " - " + strerror( errno );
        BOSE_LOG( ERROR, response );
        return;
    }

    if( arg == "off" )
    {
        display_controller_stream << "28"; // see ST7789VI_SPEC_V1.4.pdf
        response = "lcd turned on";
    }
    else if( arg == "on" )
    {
        display_controller_stream << "29"; // see ST7789VI_SPEC_V1.4.pdf
        response = "lcd turned on";
    }
    else
    {
        response = "usage: lcd [on | off]";
    }

    return;
}// ProductCliClient::CliCmdLcd
