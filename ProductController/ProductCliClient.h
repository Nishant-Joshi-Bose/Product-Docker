/*
 * The ProductCliClient class implements the Product's CLI commands.
 */

#pragma once

#include <string>
#include "CliClient.h"
#include "LpmClientIF.h"

class NotifyTargetTaskIF;

class ProductCliClient
{
public:

    ProductCliClient();

    void Initialize( NotifyTargetTaskIF* );

private:

    CLIClient m_cliClient;


    LpmClientIF::LpmClientPtr m_lpmClient;

    bool HandleCommand( std::string const&, CLIClient::StringListType&,
                        std::string& );

    void CliCmdLpmEcho( CLIClient::StringListType&, std::string& );
    void CliCmdMfgData( CLIClient::StringListType&, std::string& );
    void CliCmdBackLight( CLIClient::StringListType&, std::string& );
    void CliCmdLightSensor( CLIClient::StringListType&, std::string& );
    void CliCmdAmp( CLIClient::StringListType&, std::string& );
};
