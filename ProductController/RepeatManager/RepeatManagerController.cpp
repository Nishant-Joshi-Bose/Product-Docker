///////////////////////////////////////////////////////////////////////////////
/// @file RepeatManagerController.cpp
///
/// @brief Implementation of RepeatManager Controller
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

#include "RepeatManagerController.h"
#include "RepeatManager.h"
#include "APTaskFactory.h"
#include "APTask.h"
#include "AsyncCallback.h"
#include "DPrint.h"
#include "RepeatManager.pb.h"

static DPrint s_logger( "RepeatManagerController" );

RepeatManagerController::RepeatManagerController() :
    m_pTask( IL::CreateTask( "RepeatManagerIF" ) ),
    m_CliClient(m_pTask),
    m_CliStarted( false ),
    m_RepeatMgr(m_pTask)
{
}

RepeatManagerController::~RepeatManagerController()
{
}

void RepeatManagerController::StartCliClient()
{
    if( !m_CliStarted )
    {
        m_CliClient.RegisterCliCmd( CLI_BUTTON_PRESS, "press", "simulate the key press ", "press [first_key_value second_key_value (volume_up, volume_down, preset_1, ..., preset_6)]" );
        m_CliClient.RegisterCliCmd( CLI_BUTTON_RELEASE, "release", "simulate the key release ", "release [first_key_value second_key_value (volume_up, volume_down, preset_1, ..., preset_6)]" );
        m_CliClient.RegisterCliCmd( CLI_BUTTON_MOVE, "move", "simulate the move ", "move [x-coordinate(1 - 600)]" );

        RepeatManagerCliClient::CliHandlerFunc func = std::bind( &RepeatManagerController::HandleCliCmd, this,
                                                              std::placeholders::_1,
                                                              std::placeholders::_2 );
        m_RepeatMgr.SetRepeatManagerResultsCb( RepeatManagerController::RepeatManagerCallback, this );


        m_CliClient.Initialize( func );

        m_CliStarted = true;
    }
}

std::string RepeatManagerController::HandleCliCmd( u_int16_t cmdKey, CLIClient::StringListType &args )
{
    BOSE_VERBOSE( s_logger, "RepeatManagerController::HandleCliCmd cmdKey=%d", cmdKey );
    std::string response( "success" );

    switch( cmdKey )
    {
    case CLI_BUTTON_PRESS:
    case CLI_BUTTON_RELEASE:
        {
            int keyType;
            CLIClient::StringListType::const_iterator argit = args.begin();
            int count = args.size();
            RepeatManager::Repeat repeat;
            std::string key_string;

            switch ( cmdKey )
            {
            case CLI_BUTTON_PRESS:
                keyType = KEY_STATE_PRESSED;
                break;
            case CLI_BUTTON_MOVE:
                keyType = KEY_STATE_MOVED;
                break;
            case CLI_BUTTON_RELEASE:
                keyType = KEY_STATE_RELEASED;
                break;
            }

            for ( int i = 0; i < count; ++i )
            {
                key_string = *argit++;

                if ( keyType == KEY_STATE_MOVED )
                {
                    repeat.set_xposition(atoi(key_string.c_str()));
                    m_RepeatMgr.HandleKeys(repeat);
                    break;
                }
                else
                {
                    auto keyEntry = keyMap.find(key_string);
                    if ( keyEntry == keyMap.end() )
                        return "Invalid Key entered";
                    if ( INVALID_KEY_VAL == keyEntry->second )
                        return "Invalid Key entered";
                    repeat.set_keynumber(keyEntry->second);
                    repeat.set_keystate(keyType);
                    m_RepeatMgr.HandleKeys(repeat);
                }
            }
        }
        break;

    case CLI_LOG_LEVEL:
        if( args.size() == 1 )
        {
            CLIClient::StringListType::const_iterator argit = args.begin();
            auto logLevel = *( argit );
            try
            {
                auto dPlogLevel = DPrint::NameToLevel( logLevel );
                BOSE_DEBUG( s_logger, "%s setting logging level to %s", __func__, logLevel.c_str() );
                DPrint::SetGlobalLogLevel( dPlogLevel );
            }
            catch( const std::out_of_range &e )
            {
                BOSE_DEBUG( s_logger, "%s logging level not set, unknown arg %s", __func__, logLevel.c_str() );
                return "Bad log level";
            }
        }
        else
        {
            BOSE_DEBUG( s_logger, "%s logging level not set, missing arg", __func__ );
            return "Missing log level or too many arguments";
        }
        break;

    default:
        response = "Command not found";
    }

    return response;
}

void RepeatManagerController::SessionCreated()
{
    BOSE_DEBUG( s_logger, __func__ );

    StartCliClient();
}

void RepeatManagerController::RepeatManagerCallback(const int result, void *context)
{
    static_cast<RepeatManagerController*>( context )->m_CliClient.AsyncResponse("Key action event number --> " + std::to_string(result));
}
