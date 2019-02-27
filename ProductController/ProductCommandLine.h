/*
 * The ProductCommandLine class implements the Product's CLI commands.
 */

#pragma once

#include <string>
#include <list>
#include <unordered_set>

#include "AsyncCallback.h"
#include "CustomProductController.h"

namespace ProductApp
{

class NotifyTargetTaskIF;

class ProductCommandLine
{

public:

    ProductCommandLine( CustomProductController& ProductController );

    void Run( );

private:

    CustomProductController&                                  m_ProductController;
    NotifyTargetTaskIF*                                       m_task;

    void RegisterCliCmds();

    void HandleCliCmd( uint16_t                               cmdKey,
                       const std::list< std::string >&        argList,
                       AsyncCallback< std::string, int32_t >  respCb,
                       int32_t                                transact_id );

    void HandleBootStatus( const std::list<std::string> & argList,
                           std::string& response );

    void HandleMfgdata( const std::list<std::string> & argList,
                        std::string& response );

    void HandleBacklight( const std::list<std::string> & argList,
                          std::string& response );

    void HandleLightsensor( const std::list<std::string> & argList,
                            std::string& response );

    void HandleAmp( const std::list<std::string> & argList,
                    std::string& response );

    void HandleLcd( const std::list<std::string> & argList,
                    std::string& response );

    void HandleBattery( const std::list<std::string>& argList,
                        std::string& response );

};

}
