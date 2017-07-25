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
#include "Config/KeyEventConfiguration.h"

static DPrint s_logger( "RepeatManagerController" );

static std::map<std::string, int> keyMap = {
                                        {"play",            KEY_VAL_PLAY},
                                        {"pause",           KEY_VAL_PAUSE},
                                        {"stop",            KEY_VAL_STOP},
                                        {"prev_track",      KEY_VAL_PREV_TRACK},
                                        {"next_track",      KEY_VAL_NEXT_TRACK},
                                        {"thumbs_up",       KEY_VAL_THUMBS_UP},
                                        {"thumbs_down",     KEY_VAL_THUMBS_DOWN},
                                        {"bookmark",        KEY_VAL_BOOKMARK},
                                        {"power",           KEY_VAL_POWER},
                                        {"mute",            KEY_VAL_MUTE},
                                        {"volume_up",       KEY_VAL_VOLUME_UP},
                                        {"volume_down",     KEY_VAL_VOLUME_DOWN},
                                        {"preset_1",        KEY_VAL_PRESET_1},
                                        {"preset_2",        KEY_VAL_PRESET_2},
                                        {"preset_3",        KEY_VAL_PRESET_3},
                                        {"preset_4",        KEY_VAL_PRESET_4},
                                        {"preset_5",        KEY_VAL_PRESET_5},
                                        {"preset_6",        KEY_VAL_PRESET_6},
                                        {"aux_input",       KEY_VAL_AUX_INPUT},
                                        {"shuffel_off",     KEY_VAL_SHUFFLE_OFF},
                                        {"shuffel_on",      KEY_VAL_SHUFFLE_ON},
                                        {"repeat_off",      KEY_VAL_REPEAT_OFF},
                                        {"repeat_one",      KEY_VAL_REPEAT_ONE},
                                        {"repeat_all",      KEY_VAL_REPEAT_ALL},
                                        {"play_pause",      KEY_VAL_PLAY_PAUSE},
                                        {"add_favorite",    KEY_VAL_ADD_FAVORITE},
                                        {"remove_favorite", KEY_VAL_REMOVE_FAVORITE},
                                        {"bluetooth",       KEY_VAL_BLUETOOTH},
                                        {"invalid",         INVALID_KEY_VAL},
                                        } ;

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
            int keyType = (cmdKey == CLI_BUTTON_PRESS) ? KEY_STATE_PRESSED: KEY_STATE_RELEASED;
            CLIClient::StringListType::const_iterator argit = args.begin();
            int count = args.size();
            RepeatManager::Repeat repeat;
            std::string key_string;

            for ( int i = 0; i < count; ++i )
            {
                key_string = *argit++;
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
    static_cast<RepeatManagerController*>( context )->m_CliClient.AsyncResponse("Button Event :" + std::to_string(result));
}
