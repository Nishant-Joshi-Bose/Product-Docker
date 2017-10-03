/*
 * The ProductCliClient class implements the Product's CLI commands.
 */

#include <endian.h>

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
                       ( "backlight",
                         "Adjust the LCD back light level",
                         "backlight [level]" ) );

    cmds.emplace_back( std::make_shared<Cmd>
                       ( "lightsensor",
                         "Get the light sensor LUX value",
                         "light sensor" ) );

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
