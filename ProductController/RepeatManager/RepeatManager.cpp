///////////////////////////////////////////////////////////////////////////////
/// @file RepeatManager.cpp
///
/// @brief Implementation of RepeatManager
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

#include "RepeatManager.h"
#include "json/json.h"
#include "json/reader.h"
#include "APTimer.h"
#include "APTimerFactory.h"
#include "APTask.h"
#include "AsyncCallback.h"
#include "SoundTouchSdkPaths.h"
#include "SystemUtils.h"
#include "DPrint.h"
#include "RepeatManager.pb.h"
#include "RepeatManagerKeyAction.pb.h"

static DPrint s_logger( "RepeatManager" );

#define CONFIG_FILE_NAME            "KeyConfiguration.json"
#define KEY_CONFIG_FILE              SHELBY_CONFIG_DIR CONFIG_FILE_NAME
#define INVALID_TABLE_INDEX         -1
#define KEY_RELEASE_ACTION_TIMEOUT  -1
#define MAX_CAPSENCE_PIXELS         600
#define MAX_BUTTON_PIXELS           100

KeyRepeatManager :: KeyRepeatManager( NotifyTargetTaskIF* task ) :
    m_keyTableSize(0),
    m_keyTableIndex(INVALID_TABLE_INDEX),
    m_keyComboCounter(INVALID_TABLE_INDEX),
    m_previousTableIndex(INVALID_TABLE_INDEX),
    m_keyTimer(APTimerFactory::CreateTimer (task, "RepeatManagerTimer" ))
{
    for (int i = 0; i < MAXIMUM_KEYS; i++ )
        m_keyTrack[i] = INVALID_KEY_VAL;

    std::map<std::string, int> keyEventMap;

    /*
     * Initially generating map for event action string and the value corresponding.
     */
    for ( int i = (int)KEY_EVENT_ACTION_MIN; i <= KEY_EVENT_ACTION_MAX; i++ )
    {
        if ( KEY_EVENT_ACTION_IsValid(i) )
            keyEventMap.insert(std::pair<std::string, int>(KEY_EVENT_ACTION_Name((KEY_EVENT_ACTION)i), i));
    }

    auto config = SystemUtils::ReadFile( KEY_CONFIG_FILE );
    if( !config )
    {
        BOSE_DEBUG( s_logger, "Failed to load config file");
        return;
    }

    Json::Value root;
    Json::Reader reader;

    if ( !reader.parse( *config, root ) )
    {
        BOSE_DEBUG( s_logger, "Failed to parse configuration ----> %s", reader.getFormattedErrorMessages().c_str());
        return;
    }

    m_keyTableSize = root["keyTable"].size();
    m_keyTableEntry = new _KeyTableEntry[m_keyTableSize];

    BOSE_DEBUG( s_logger, "Key Configuration table size  ------------ > %d", m_keyTableSize);

    for ( int index = 0; index < m_keyTableSize; index++ )
    {
        int keyNumber = root["keyTable"][index]["Keys"].size();
        for ( int j = 0; j < MAXIMUM_KEYS; j++ )
        {
            if ( j < keyNumber )
                m_keyTableEntry[index].Keys[j] = keyMap.find(root["keyTable"][index]["Keys"][j].asString())->second;
            else
                m_keyTableEntry[index].Keys[j] = INVALID_KEY_VAL;
        }
        m_keyTableEntry[index].TimeOut = root["keyTable"][index]["TimeOut"].asInt();
        m_keyTableEntry[index].Event = keyEventMap.find(root["keyTable"][index]["Event"].asString())->second;
        m_keyTableEntry[index].Repeat = root["keyTable"][index]["Repeat"].asBool();
        m_keyTableEntry[index].ActionOnTimeout = root["keyTable"][index]["ActionOnTimeout"].asBool();
    }
}

KeyRepeatManager :: ~KeyRepeatManager()
{
    delete [] m_keyTableEntry;
    m_keyTimer->Shutdown();
}

void KeyRepeatManager::DeInitializeVariables()
{
    m_keyTimer->Stop();
    m_keyTableIndex = INVALID_TABLE_INDEX;
    m_previousTableIndex = INVALID_TABLE_INDEX;
    m_keyComboCounter = INVALID_TABLE_INDEX;
    for (int i = 0; i < MAXIMUM_KEYS; i++ )
        m_keyTrack[i] = INVALID_KEY_VAL;
}

void KeyRepeatManager::SetRepeatManagerResultsCb( RepeatManagerResultsCb *cb, void *context )
{
    m_keyResultCallBack = cb;
    m_keyCallBackbContext = context;
}

int KeyRepeatManager::GetKeyNumberForPosition( int xPos )
{
    if ( xPos > 0 && xPos <= MAX_CAPSENCE_PIXELS)
    {
        return (KEY_VAL_PRESET_1 + (xPos / MAX_BUTTON_PIXELS + (xPos % MAX_BUTTON_PIXELS > 0)) - 1);
    }

    return INVALID_KEY_VAL;
}

int KeyRepeatManager::GetKeyTableIndexNumber( int keyNo, int index, int keyComboCounter )
{
    BOSE_INFO( s_logger, __func__ );
    BOSE_DEBUG( s_logger, "keyNo  = %d    index = : %d  keyComboCounter = %d", keyNo, index, keyComboCounter);

    int count = index;

    /*
     * For long key press or if Second key is pressed as combo keyNo
     */
    while ( count < m_keyTableSize )
    {
        if ( INVALID_TABLE_INDEX == keyComboCounter )
        {
            if ( keyNo == m_keyTableEntry[count].Keys[0] )
                return count;
        }
        else
        {
            int i = 0;
            /*INVALID_TABLE_INDEX != m_keyTableIndex
             * Checking for the previous keys which are pressed and matches from the config table
             */
            while ( i <= keyComboCounter )
            {
                if ( m_keyTrack[i] != m_keyTableEntry[count].Keys[i] )
                {
                    break;
                }
                i++;
            }
            /*
             * Validating for the last key pressed with the config file and get the index
             */
            if ( i > keyComboCounter )
            {
                if ( 0 == index )
                {
                    if ( m_keyTableEntry[count].Keys[i] == keyNo )
                        return count;
                }
                else
                {
                    if ( INVALID_KEY_VAL != m_keyTrack[i] && m_keyTableEntry[count].Keys[i] == keyNo )
                    {
                        return count;
                    }
                    else if ( INVALID_KEY_VAL == m_keyTrack[i] && INVALID_KEY_VAL == m_keyTableEntry[count].Keys[i] )
                    {
                        return count;
                    }
                }
            }
        }
        count++;
    }

    return INVALID_TABLE_INDEX;
}

void KeyRepeatManager :: CallBackFunction( int keyState, int keyNumber )
{
    BOSE_INFO( s_logger, __func__ );
    BOSE_DEBUG( s_logger, "keyState = %d,   keyNumber : %d,    keyContinuious = %d", keyState, keyNumber, m_keyTableEntry[m_keyTableIndex].Repeat);

    /*
     * Starting timer again if the repeat is true.
     */
    if ( m_keyTableEntry[m_keyTableIndex].Repeat )
    {
        m_keyTimer->Stop();
        (m_keyTimer)->SetTimeouts(m_keyTableEntry[m_keyTableIndex].TimeOut, 0);
        (m_keyTimer)->Start (std::bind (&KeyRepeatManager::CallBackFunction, this, keyState, keyNumber));
        m_keyResultCallBack( m_keyTableEntry[m_keyTableIndex].Event, m_keyCallBackbContext );
        BOSE_DEBUG( s_logger, "Key is pressed and held for continuois action, send event to perform Event : %d", m_keyTableEntry[m_keyTableIndex].TimeOut);
        return;
    }

    int index = INVALID_TABLE_INDEX;
    int previousTimeOut = m_keyTableEntry[m_keyTableIndex].TimeOut;

    /*
     * Checking for any other row with same key comibination in the config file, starting timer for the longer duration
     */
    index = GetKeyTableIndexNumber(keyNumber, m_keyTableIndex+1, m_keyComboCounter);
    BOSE_DEBUG( s_logger, "New keyIndex = : %d", index);
    if ( INVALID_TABLE_INDEX != index )
    {
        if ( m_keyTableEntry[m_keyTableIndex].ActionOnTimeout )
            m_keyResultCallBack( m_keyTableEntry[m_keyTableIndex].Event, m_keyCallBackbContext );

        m_previousTableIndex = m_keyTableIndex;
        m_keyTableIndex = index;
        m_keyTimer->Stop();
        if ( m_keyTableEntry[m_keyTableIndex].Repeat )
            (m_keyTimer)->SetTimeouts(abs(m_keyTableEntry[m_keyTableIndex].TimeOut), 0);
        else
            (m_keyTimer)->SetTimeouts(abs(m_keyTableEntry[m_keyTableIndex].TimeOut-previousTimeOut), 0);
        (m_keyTimer)->Start (std::bind (&KeyRepeatManager::CallBackFunction, this, keyState, keyNumber));
        BOSE_DEBUG( s_logger, "Long is pressed Remaining timeout = : %d", abs(m_keyTableEntry[m_keyTableIndex].TimeOut-previousTimeOut));
        return;
    }
    /*
     * Send event action for the index from the key config file to process the key/keycombo based on the timeout.
     */
    m_keyResultCallBack( m_keyTableEntry[m_keyTableIndex].Event, m_keyCallBackbContext );
    BOSE_DEBUG( s_logger, "Long key press action Event : %d", m_keyTableEntry[m_keyTableIndex].Event);

    DeInitializeVariables();
}

void KeyRepeatManager::HandleKeys( const RepeatManager::Repeat & keyEvent )
{
    BOSE_INFO( s_logger, __func__ );

    int keyState = keyEvent.keystate();
    int keyNumber = keyEvent.keynumber();

    BOSE_DEBUG( s_logger, "keyState = %d  keyNumber = %d", keyState, keyNumber);

    switch( keyState )
    {
    case KEY_STATE_PRESSED:
        {
            m_keyTableIndex = GetKeyTableIndexNumber(keyNumber, 0, m_keyComboCounter);
            /*
             * To process for the last key pressed and ignoring the earlier keys in case of combo keys
             */
            if ( INVALID_TABLE_INDEX == m_keyTableIndex )
            {
                BOSE_DEBUG( s_logger, "Invalid m_keyTableIndex = %d ignore previous keys", m_keyTableIndex);
                /*
                 * Deinitialize the variables
                 */
                DeInitializeVariables();
                m_keyTableIndex = GetKeyTableIndexNumber(keyNumber, 0, m_keyComboCounter);
                if ( INVALID_TABLE_INDEX == m_keyTableIndex )
                    return;
            }

            m_keyComboCounter++;
            m_keyTrack[m_keyComboCounter] = keyNumber;

            /*
             * Processing key which require immediate action as soon as the key is pressed, this is based on the timeout value as 0 in config table
             */
            if ( !m_keyTableEntry[m_keyTableIndex].TimeOut )
            {
                BOSE_DEBUG( s_logger, "Perform immediate action, m_keyTableIndex = %d, TimeOut = %d, Event = %d ", m_keyTableIndex, m_keyTableEntry[m_keyTableIndex].TimeOut, m_keyTableEntry[m_keyTableIndex].Event);
                m_keyResultCallBack( m_keyTableEntry[m_keyTableIndex].Event, m_keyCallBackbContext );
                /*
                 * Checking for any other action to be performed on the same key with timeout of for release
                 */
                m_keyTableIndex = GetKeyTableIndexNumber(keyNumber, m_keyTableIndex+1, m_keyComboCounter);
                if ( INVALID_TABLE_INDEX == m_keyTableIndex )
                {
                    DeInitializeVariables();
                    return;
                }
            }


            m_keyTimer->Stop();

            /*
             * Starting timer for those which require timeout processing and has a valid number in the timeout field of key configuration file
             * If the timeout value is KEY_RELEASE_ACTION_TIMEOUT(-1) then processing should happen only on the key release event.
             */
            if ( KEY_RELEASE_ACTION_TIMEOUT != m_keyTableEntry[m_keyTableIndex].TimeOut )
            {
                m_keyTimer->SetTimeouts(m_keyTableEntry[m_keyTableIndex].TimeOut, 0);
                m_keyTimer->Start (std::bind (&KeyRepeatManager::CallBackFunction, this, keyState, keyNumber));
            }
            BOSE_DEBUG( s_logger, "m_keyTableIndex = %d, TimeOut = %d", m_keyTableIndex, m_keyTableEntry[m_keyTableIndex].TimeOut);
        }
        break;

    case KEY_STATE_RELEASED:
        {
            m_keyTimer->Stop();
            if ( INVALID_TABLE_INDEX == m_keyTableIndex )
            {
                BOSE_DEBUG( s_logger, "Invalid operation, nothing to perform");
                return;
            }

            if ( KEY_RELEASE_ACTION_TIMEOUT == m_keyTableEntry[m_keyTableIndex].TimeOut )
            {
                BOSE_DEBUG( s_logger, "Key is released perform action only for the key release event item Event : %d", m_keyTableEntry[m_keyTableIndex].Event);
                m_keyResultCallBack( m_keyTableEntry[m_keyTableIndex].Event, m_keyCallBackbContext );
            }
            else if ( m_keyTableEntry[m_keyTableIndex].TimeOut > 0 )
            {
                if ( !m_keyTableEntry[m_keyTableIndex].Repeat && !m_keyTableEntry[m_keyTableIndex].ActionOnTimeout && INVALID_TABLE_INDEX != m_previousTableIndex )
                {
                    BOSE_DEBUG( s_logger, "Key is released before timeout and send event to perform Event : %d", m_keyTableEntry[m_previousTableIndex].Event);
                    m_keyResultCallBack( m_keyTableEntry[m_previousTableIndex].Event, m_keyCallBackbContext );
                }
            }
            else
            {
                BOSE_DEBUG( s_logger, "Key is released after all key events nothing to perform it is invalid action");
            }
            DeInitializeVariables();
        }
        break;

    case KEY_STATE_MOVED:
        {
            int keyPos = keyEvent.xposition();

            keyNumber = GetKeyNumberForPosition(keyPos);

            if ( INVALID_KEY_VAL == keyNumber )
                return;

            RepeatManager::Repeat repeatKey;

            repeatKey.set_keynumber(keyNumber);
            repeatKey.set_keystate(KEY_STATE_PRESSED);

            if ( INVALID_TABLE_INDEX == m_keyComboCounter )
            {
                HandleKeys(repeatKey);
                return;
            }

            /*
             * Check for the existance of the key in the keytrack, if it exist then perform no action
             */
            for ( int i = m_keyComboCounter; i >= 0; i-- )
            {
                if ( keyNumber == m_keyTrack[i] )
                    return;
            }

            /*
             * if key does not exist and contain other capsese key then remove the last key of capsense from keytrack
             *  and process key press with new key, only one should exist at a time with capsense
             */
            for ( int i = m_keyComboCounter; i >= 0; i-- )
            {
                if (m_keyTrack[i] >= KEY_VAL_PRESET_1 && m_keyTrack[i] <= KEY_VAL_PRESET_6 )
                {
                    for ( int j = i; j < m_keyComboCounter; j++ )
                    {
                        m_keyTrack[j] = m_keyTrack[j+1];
                    }
                    m_keyTrack[m_keyComboCounter] = INVALID_KEY_VAL;
                    m_keyComboCounter--;
                    HandleKeys(repeatKey);
                    return;
                }
            }
        }
        break;

    default:
        break;
    }
}
