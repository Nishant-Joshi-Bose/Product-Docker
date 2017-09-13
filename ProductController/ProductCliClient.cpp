/*
 * The ProductCliClient class implements the Product's CLI commands.
 */
#include <iostream>
#include <fstream>


#include "ProductCliClient.h"
#include "DPrint.h"
#include "SystemUtils.h"
#include "LpmClientFactory.h"
#include "RivieraLPM_IpcProtocol.h"

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

    cmds.emplace_back( std::make_shared<Cmd>
                       ( "lpm echo",
                         "Send an echo request to the LPM",
                         "lpm echo [count]" ) );

    cmds.emplace_back( std::make_shared<Cmd>
                       ( "mfgdata",
                         "Show the manufacturing data",
                         "mfgdata" ) );

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

    if( cmd == "lpm echo" )
    {
        CliCmdLpmEcho( argList, response );
        return true;
    }

    if( cmd == "mfgdata" )
    {
        CliCmdMfgData( argList, response );
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

void ProductCliClient::CliCmdLpmEcho( CLIClient::StringListType& argList,
                                      std::string& response )
{
    decltype( IpcProtocol::IpcEcho_t::count ) count {};
    if( argList.size() == 1 )
    {
        auto& arg = argList.front();
        if( !ToInteger( arg, count ) )
        {
            response = "Malformed integer: " + arg;
            return;
        }
    }
    else if( !argList.empty() )
    {
        response = "Wrong usage";
        return;
    }
    BOSE_LOG( INFO, "Send lpm echo request count=" << count );
    IpcEcho_t param;
    param.set_count( count );
    m_lpmClient->RequestEcho( param, [this]( IpcEcho_t const & rsp )
    {
        std::ostringstream ss;
        ss << "Got lpm echo response count=" << rsp.count();
        BOSE_LOG( INFO, ss.str() );
        m_cliClient.SendAsyncResponse( ss.str() );
    } );
    response = "echo sent";
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

void ProductCliClient::CliCmdLcd( CLIClient::StringListType& argList,
                                  std::string& response )
{
    const std::string str_usage                    = "usage";
    const std::string display_controller_file_name = "/sys/devices/soc/7af6000.spi/spi_master/spi6/spi6.1/graphics/fb1/send_command";
    auto& arg                                      = ( argList.size() == 1 ) ? argList.front() : str_usage;
    std::ifstream     display_controller_exist( display_controller_file_name );

    if( std::ifstream( display_controller_file_name ).good() == false )
    {
        response = "error: can't find file: " + display_controller_file_name + " - " + strerror( errno );
        BOSE_LOG( ERROR, response );
        return;
    }

    std::ofstream display_controller_stream( display_controller_file_name );
    if( ! display_controller_stream )
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
