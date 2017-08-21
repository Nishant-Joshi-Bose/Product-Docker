/*
 * The ProductCliClient class implements the Product's CLI commands.
 */

#pragma once

#include "CliClient.h"
#include <string>

class NotifyTargetTaskIF;

class ProductCliClient
{
public:

    ProductCliClient();

    void Initialize( NotifyTargetTaskIF* );

private:

    CLIClient m_cliClient;

    bool HandleCommand( std::string const&, CLIClient::StringListType&,
                        std::string& );

    void WahooOne( CLIClient::StringListType&, std::string& );

};
