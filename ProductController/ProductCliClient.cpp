/*
 * The ProductCliClient class implements the Product's CLI commands.
 */

#include "ProductCliClient.h"
#include "DPrint.h"

namespace
{

DPrint s_logger{ "ProductCliClient" };

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
                       ( "wahoo one",
                         "Wahoo one",
                         "wahoo one args..." ) );

    cmds.emplace_back( std::make_shared<Cmd>
                       ( "wahoo two",
                         "Wahoo two",
                         "wahoo two args..." ) );

    cmds.emplace_back( std::make_shared<Cmd>
                       ( "wahoo three",
                         "Wahoo three",
                         "wahoo three args..." ) );

    cmds.emplace_back( std::make_shared<Cmd>
                       ( "wahoo four",
                         "Wahoo four",
                         "wahoo four args..." ) );

    m_cliClient.Initialize( task, cmds,
                            [this]( std::string const& cmd,
                                    CLIClient::StringListType& argList,
                                    std::string& response ) {
                                return HandleCommand( cmd, argList, response );
                            } );
}

bool ProductCliClient::HandleCommand( std::string const& cmd,
                                      CLIClient::StringListType& argList,
                                      std::string& response )
{
    BOSE_INFO( s_logger, "HandleCommand '%s'", cmd.c_str() );

    if( cmd == "wahoo one" )
    {
        WahooOne( argList, response );
        return true;
    }

    if( cmd == "wahoo two" )
    {
        //WahooTwo( argList, response );
        return true;
    }

    if( cmd == "wahoo three" )
    {
        //WahooThree( argList, response );
        return true;
    }

    if( cmd == "wahoo four" )
    {
        //WahooFour( argList, response );
        return true;
    }

    response = "Unknown command: " + cmd;
    return false;
}

void ProductCliClient::WahooOne( CLIClient::StringListType& argList,
                                 std::string& response )
{
    std::ostringstream ss;
    ss << "args [" << argList.size() << ']';
    for( auto const& arg : argList )
        ss << " '" << arg << '\'';
    response = ss.str();
}
