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
#include "RepeatManagerKeyAction.h"
#include "DPrint.h"


static DPrint s_logger( "RepeatManager" );

#define CONFIG_FILE_NAME        "KeyConfiguration.json"
#define KEY_CONFIG_FILE         SHELBY_CONFIG_DIR CONFIG_FILE_NAME
#define INVALID_TABLE_INDEX     -1
#define KEY_RELEASE_ACTION_TIMEOUT     -1

CRepeatManager :: CRepeatManager( NotifyTargetTaskIF* task ) :
    m_keyTableSize(0),
    m_keyTableIndex(INVALID_TABLE_INDEX),
    m_keyComboCounter(INVALID_TABLE_INDEX),
    m_previousTableIndex(INVALID_TABLE_INDEX),
    m_keyTimer(APTimerFactory::CreateTimer (task, "RepeatManagerTimer" ))
{
    for (int i = 0; i < MAXIMUM_KEYS; i++ )
        m_keyTrack[i] = INVALID_KEY_VAL;

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
        m_keyTableEntry[index].KeyTimeout = root["keyTable"][index]["KeyTimeout"].asInt();
        m_keyTableEntry[index].EventAction = keyEventActionMap.find(root["keyTable"][index]["EventAction"].asString())->second;
        m_keyTableEntry[index].Repeat = root["keyTable"][index]["Repeat"].asBool();
    }
}

CRepeatManager :: ~CRepeatManager()
{
    delete [] m_keyTableEntry;
    m_keyTimer->Shutdown();
}

void CRepeatManager::DeInitializeVariables()
{
    m_keyTimer->Stop();
    m_keyTableIndex = INVALID_TABLE_INDEX;
    m_previousTableIndex = INVALID_TABLE_INDEX;
    m_keyComboCounter = INVALID_TABLE_INDEX;
    for (int i = 0; i < MAXIMUM_KEYS; i++ )
        m_keyTrack[i] = INVALID_KEY_VAL;
}

void CRepeatManager::SetRepeatManagerResultsCb( RepeatManagerResultsCb *cb, void *context)
{
    ResultCb = cb;
    ResultCbContext = context;
}

int CRepeatManager::GetKeyTableIndexNumber( int keyNo, int index, int keyComboCounter )
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

void CRepeatManager :: CallBackFunction( int keyState, int keyNumber )
{
    BOSE_INFO( s_logger, __func__ );
    BOSE_DEBUG( s_logger, "keyState = %d,   keyNumber : %d,    keyContinuious = %d", keyState, keyNumber, m_keyTableEntry[m_keyTableIndex].Repeat);

    if ( KEY_STATE_PRESSED == keyState )
    {
        BOSE_DEBUG( s_logger, "Keys is  pressed and held : %d", keyNumber);
        /*
         * Starting timer again based on the repeat field in the config table.
         */
        if ( m_keyTableEntry[m_keyTableIndex].Repeat )
        {
            (m_keyTimer)->SetTimeouts(m_keyTableEntry[m_keyTableIndex].KeyTimeout, 0);
            (m_keyTimer)->Start (std::bind (&CRepeatManager::CallBackFunction, this, keyState, keyNumber));
            ResultCb( m_keyTableEntry[m_keyTableIndex].EventAction, ResultCbContext );
            BOSE_DEBUG( s_logger, "Key is pressed and held for continuois action, send event to perform EventAction : %d", m_keyTableEntry[m_keyTableIndex].EventAction);
            return;
        }
        else
        {
            int index = INVALID_TABLE_INDEX;
            int previousTimeOut = m_keyTableEntry[m_keyTableIndex].KeyTimeout;

            /*
             * Checking for any other row with the key comibination in the config file greater than the previous key and start the timer for the longer duration
             */
            index = GetKeyTableIndexNumber(keyNumber, m_keyTableIndex+1, m_keyComboCounter);
            BOSE_DEBUG( s_logger, "New keyIndex = : %d", index);
            if ( INVALID_TABLE_INDEX != index )
            {
                m_previousTableIndex = m_keyTableIndex;
                m_keyTableIndex = index;
                m_keyTimer->Stop();
                (m_keyTimer)->SetTimeouts(abs(m_keyTableEntry[m_keyTableIndex].KeyTimeout-previousTimeOut), 0);
                (m_keyTimer)->Start (std::bind (&CRepeatManager::CallBackFunction, this, keyState, keyNumber));
                BOSE_DEBUG( s_logger, "Long is pressed Remaining timeout = : %d", abs(m_keyTableEntry[m_keyTableIndex].KeyTimeout-previousTimeOut));
                return;
            }
            /*
             * Send event for the the index from the key config file to process the key/keycombo based on the timeout.
             */
            ResultCb( m_keyTableEntry[m_keyTableIndex].EventAction, ResultCbContext );
            BOSE_DEBUG( s_logger, "Long key press action EventAction : %d", m_keyTableEntry[m_keyTableIndex].EventAction);
        }
    }
    DeInitializeVariables();
}

void CRepeatManager::HandleKeys( const RepeatManager::Repeat & keyEvent )
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
             * This is to process for the last key pressed and ignoring the earlier keys in case of combo keys
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
                    break;
            }
            /*
             * Processing key which require immediate action as soon as the key is pressed, this is based on the timeout value as 0 in config table
             */
            if ( !m_keyTableEntry[m_keyTableIndex].KeyTimeout )
            {
                BOSE_DEBUG( s_logger, "Perform immediate action, m_keyTableIndex = %d, KeyTimeout = %d, EventAction = %d ", m_keyTableIndex, m_keyTableEntry[m_keyTableIndex].KeyTimeout, m_keyTableEntry[m_keyTableIndex].EventAction);
                ResultCb( m_keyTableEntry[m_keyTableIndex].EventAction, ResultCbContext );
                DeInitializeVariables();
                break;
            }

            m_keyComboCounter++;
            m_keyTrack[m_keyComboCounter] = keyNumber;
            m_keyTimer->Stop();

            /*
             * Starting timer for those which require timeout processing and has a valid number in the timeout field of key configuration file
             * If the timeout value is KEY_RELEASE_ACTION_TIMEOUT(-1) then processing should happen only on the key release event.
             */
            if ( KEY_RELEASE_ACTION_TIMEOUT != m_keyTableEntry[m_keyTableIndex].KeyTimeout )
            {
                m_keyTimer->SetTimeouts(m_keyTableEntry[m_keyTableIndex].KeyTimeout, 0);
                m_keyTimer->Start (std::bind (&CRepeatManager::CallBackFunction, this, keyState, keyNumber));
            }
            BOSE_DEBUG( s_logger, "m_keyTableIndex = %d, KeyTimeout = %d", m_keyTableIndex, m_keyTableEntry[m_keyTableIndex].KeyTimeout);
        }
        break;

    case KEY_STATE_RELEASED:
        {
            m_keyTimer->Stop();
            if ( INVALID_TABLE_INDEX != m_keyTableIndex )
            {
                if ( KEY_RELEASE_ACTION_TIMEOUT == m_keyTableEntry[m_keyTableIndex].KeyTimeout )
                {
                    BOSE_DEBUG( s_logger, "Key is released perform action only for the key release event item EventAction : %d", m_keyTableEntry[m_keyTableIndex].EventAction);
                    ResultCb( m_keyTableEntry[m_keyTableIndex].EventAction, ResultCbContext );
                }
                else if ( m_keyTableEntry[m_keyTableIndex].KeyTimeout > 0 )
                {
                    if ( !m_keyTableEntry[m_keyTableIndex].Repeat && INVALID_TABLE_INDEX != m_previousTableIndex )
                    {
                        BOSE_DEBUG( s_logger, "Key is released before timeout and send event to perform EventAction : %d", m_keyTableEntry[m_previousTableIndex].EventAction);
                        ResultCb( m_keyTableEntry[m_previousTableIndex].EventAction, ResultCbContext );
                    }
                }
                else
                {
                    BOSE_DEBUG( s_logger, "Key is released after all key events nothing to perform it is invalid action");
                }
            }
            else
            {
                BOSE_DEBUG( s_logger, "Invalid operation, nothing to perform");
            }
            DeInitializeVariables();
        }
        break;
    default:
        break;
    }
}
