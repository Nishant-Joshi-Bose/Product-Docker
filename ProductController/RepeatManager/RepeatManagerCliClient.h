////////////////////////////////////////////////////////////////////////////////
/// @file  RepeatManagerCliClient.h
/// @brief This class is for providing a command line interface for the RepeatManager
/// @author Jon cooper
/// @date   1/23/2015
/// @attention Copyright 2015 Bose Corporation, Framingham, MA
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "CliClient.h"
#include <string>
#include <map>
#include <functional>

class RepeatManagerCliClient
{
public:
    ////////////////////////////////////////////////////////////////////////
    /// @brief constructor
    /// @param task - this is the task that the CLI handler function will be called in
    ////////////////////////////////////////////////////////////////////////
    RepeatManagerCliClient( NotifyTargetTaskIF* task );

    ////////////////////////////////////////////////////////////////////////
    /// @brief Register a CLI command
    /// @param cmdKey - a unique key that should come from a source specific enum of cli commands.
    /// @param cmd - the actual command. This command will be prefixed with net and a space
    /// @param summary - a description of the command
    /// @param usage - specify the format of the expected arguments.
    /// @return bool - whether or not the commmand was successfully registered, true if so. If it
    /// already exists false will be returned.
    ////////////////////////////////////////////////////////////////////////
    bool RegisterCliCmd( u_int16_t cmdKey,
                         const std::string& cmd,
                         const std::string& summary,
                         const std::string& usage );

    ////////////////////////////////////////////////////////////////////////
    /// @brief Every STS Source that wants a command line interface needs to have one function
    /// matching this type where it returns a const string (response), and takes in a
    /// u_int16_t (cmdKey) and a const StringListType reference which is the arguments the user
    /// entered. For example:
    ///
    /// const std::string HandleCliCmd( u_int16_t cmdKey, const CLIClient::StringListType & args );
    ////////////////////////////////////////////////////////////////////////
    typedef std::function<const std::string( u_int16_t, CLIClient::StringListType & )> CliHandlerFunc;

    ////////////////////////////////////////////////////////////////////////
    /// @brief Initialize the command line interface for the STS Source
    /// @param cliHandlerFunc - This is a reference to one of the function objects specified above.
    /// @return none
    ////////////////////////////////////////////////////////////////////////
    void Initialize( const CliHandlerFunc& cliHandlerFunc );

    ////////////////////////////////////////////////////////////////////////
    /// @brief Send an Asyncronous response
    /// @param the response to send
    /// @return none
    ////////////////////////////////////////////////////////////////////////
    void AsyncResponse( const std::string& response );

private:
    /// This is called from the Cli Server
    bool HandleCliCmd( const std::string & cmd,
                       CLIClient::StringListType &argList,
                       std::string & response );

    /// This lookup type is a simple and fast way to find the correct command key without needing
    /// an extra switch. It also provides a consistent unique way to track the registered commands
    typedef std::map<std::string, u_int16_t> CmdKeyLookup;
    typedef CmdKeyLookup::const_iterator CmdKeyLookupIter;

    /// Used internally for retrieving the key for the specified command
    CmdKeyLookupIter GetKey( const std::string& cmd );

private:
    CLIClient m_cliClient;
    NotifyTargetTaskIF* m_task;

    /// This provides quick access to the cooresponding command key
    CmdKeyLookup m_cmdKeyLookup;

    /// This is a temporary data structure to build up the registered commands and it is cleared
    /// after intitialization
    std::vector<CLIClient::CmdPtr> m_cmds;

    /// The handler for the STS source to call when a new cli command is entered
    CliHandlerFunc m_cliHandleFunc;
};
