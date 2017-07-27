///////////////////////////////////////////////////////////////////////////////
/// @file RepeatManagerController.h
///
/// @brief Defination of RepeatManager Controller
///
/// @attention
///    BOSE CORPORATION.
///    COPYRIGHT 2017 BOSE CORPORATION ALL RIGHTS RESERVED.
///    This program may not be reproduced, in whole or in part in any
///    form or any means whatsoever without the written permission of:
///        BOSE CORPORATION
///        The Mountain
///        Framingham, MA 01701-9168
///
///////////////////////////////////////////////////////////////////////////////
#pragma once

#include "RepeatManagerCliClient.h"
#include "RepeatManager.h"
#include "CliClient.h"
#include <string>
#include <map>
#include <functional>

class RepeatManagerController
{
public:
    RepeatManagerController();
    virtual ~RepeatManagerController();
    void SessionCreated();

    static void RepeatManagerCallback(const int result, void *context);

private:
    NotifyTargetTaskIF* m_pTask;
    // CLI Stuff
    enum CliCmdKey
    {
        CLI_BUTTON_PRESS,
        CLI_BUTTON_RELEASE,
        CLI_BUTTON_MOVE,
        CLI_LOG_LEVEL
    };
    RepeatManagerCliClient m_CliClient;
    bool m_CliStarted;
    KeyRepeatManager m_RepeatMgr;

private:
    void StartCliClient();
    std::string HandleCliCmd( u_int16_t cmdKey, CLIClient::StringListType &args );
};
