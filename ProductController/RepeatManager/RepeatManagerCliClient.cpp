////////////////////////////////////////////////////////////////////////////////
/// @file  RepeatManagerCliClient.cpp
/// @brief This class is for providing a command line interface in an STS source
/// @attention Copyright 2015 Bose Corporation, Framingham, MA
////////////////////////////////////////////////////////////////////////////////

#include "RepeatManagerCliClient.h"
#include "StringUtils.h"
#include "DPrint.h"

static DPrint s_logger( "RepeatManagerCliClient" );
#define COMMAND_PREFIX "button "

RepeatManagerCliClient::RepeatManagerCliClient( NotifyTargetTaskIF* task ) :
    m_cliClient( "button" ),
    m_task( task )
{
}

bool RepeatManagerCliClient::RegisterCliCmd( u_int16_t cmdKey,
                                          const std::string& cmd,
                                          const std::string& summary,
                                          const std::string& usage )
{
    BOSE_VERBOSE( s_logger, "RegisterCliCmd cmdKey:%d cmd:\"%s\"", cmdKey, cmd.c_str() );

    if( GetKey( cmd ) == m_cmdKeyLookup.end() )
    {
        // prefix the command with the source name so the sources don't have to redundantly do it
        CLIClient::CLICmdDescriptor* pCmd = new CLIClient::CLICmdDescriptor( COMMAND_PREFIX + cmd,
                                                                             summary,
                                                                             COMMAND_PREFIX + usage );
        m_cmds.push_back( ( CLIClient::CmdPtr ) pCmd );

        // To make sure we always match the command no matter how many spaces or what case they use
        // remove all the whitespace and force it to lowercase. We do the same conversion when
        // checking for it later.
        m_cmdKeyLookup[ StringUtils::RemoveWhitespace(
                            StringUtils::ToLower( COMMAND_PREFIX + cmd ) ) ] = cmdKey;
        return true;
    }
    else
    {
        s_logger.LogError( "Command \"%s\" already exists!", cmd.c_str() );
        return false;
    }
}

////////////////////////////////////////////////////////////////////////
void RepeatManagerCliClient::Initialize( const CliHandlerFunc& cliHandlerFunc )
{
    BOSE_VERBOSE( s_logger, "Initialize" );

    m_cliHandleFunc = cliHandlerFunc;

    CLIClient::CLICmdHandlerFunc func = std::bind( &RepeatManagerCliClient::HandleCliCmd, this,
                                                   std::placeholders::_1,
                                                   std::placeholders::_2,
                                                   std::placeholders::_3 );

    m_cliClient.Initialize( m_task, m_cmds, func );

    // Once we are initialized we don't need this anymore so clean up
    m_cmds.clear();
}

////////////////////////////////////////////////////////////////////////
bool RepeatManagerCliClient::HandleCliCmd( const std::string & cmd,
                                        CLIClient::StringListType & argList,
                                        std::string & response )
{
    BOSE_VERBOSE( s_logger, "HandleCliCmd cmd:\"%s\"", cmd.c_str() );

    CmdKeyLookupIter cmdIter = GetKey( cmd );

    if( cmdIter != m_cmdKeyLookup.end() )
    {
        // This callback should never happen if the cliHandlerFunc isn't set but check just in case
        if( m_cliHandleFunc )
        {
            response = m_cliHandleFunc( cmdIter->second, argList );
            return true;
        }
        else
        {
            s_logger.LogError( "Missing cli handler for net %s", cmd.c_str() );
            return false;
        }
    }
    else
    {
        // Realistitcally unless there is an internal error this should never print out because the
        // cli server would have to get out of sync with us
        s_logger.LogError( "Unsupport command: \"%s\"", cmd.c_str() );
        return false;
    }
}

////////////////////////////////////////////////////////////////////////
RepeatManagerCliClient::CmdKeyLookupIter RepeatManagerCliClient::GetKey( const std::string& cmd )
{
    // To make sure we always match the command no matter how many spaces or what case they use
    // remove all the whitespace and force it to lowercase.
    return m_cmdKeyLookup.find( StringUtils::RemoveWhitespace( StringUtils::ToLower( cmd ) ) );
}

////////////////////////////////////////////////////////////////////////
void RepeatManagerCliClient::AsyncResponse( const std::string &response )
{
    m_cliClient.SendAsyncResponse( response );
}
