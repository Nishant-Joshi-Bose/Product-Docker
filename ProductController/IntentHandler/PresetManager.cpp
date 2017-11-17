///////////////////////////////////////////////////////////////////////////////
/// @file PresetManager.cpp
///
/// @brief Implementation of Transport Control Manager
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

#include "DPrint.h"
#include "PresetManager.h"
#include "SoundTouchInterface/Presets.pb.h"

static DPrint s_logger( "PresetManager" );

namespace ProductApp
{

PresetManager::PresetManager( NotifyTargetTaskIF& task,
                              const CliClientMT& cliClient,
                              const FrontDoorClientIF_t& frontDoorClient,
                              EddieProductController& controller ):
    IntentManager( task, cliClient, frontDoorClient, controller )
{
    m_frontDoorClientErrorCb = AsyncCallback<FRONT_DOOR_CLIENT_ERRORS>\
                               ( std::bind( &PresetManager::FrontDoorClientErrorCb,
                                            this, std::placeholders::_1 ), &task );
}

///////////////////////////////////////////////////////////////////////////////
/// @name  Handle
/// @brief Function to build and send passport message to select or store preset
//         based on the intent
//         The callBack function is called to give control back to the state
//         machine if HSM has registered a call back.
/// @return true: Successful
//          false: Error
////////////////////////////////////////////////////////////////////////////////

bool PresetManager::Handle( KeyHandlerUtil::ActionType_t& intent )
{
    BOSE_DEBUG( s_logger, "%s", __func__ );

    switch(intent)
    {
    case ( uint16_t ) Action::PRESET_SELECT_1:
    case ( uint16_t ) Action::PRESET_SELECT_2:
    case ( uint16_t ) Action::PRESET_SELECT_3:
    case ( uint16_t ) Action::PRESET_SELECT_4:
    case ( uint16_t ) Action::PRESET_SELECT_5:
    case ( uint16_t ) Action::PRESET_SELECT_6:
    {
        uint8_t presetId = IntentToIdMap(intent);
        if (IsPresetValid(presetId, presets ))
        {
        }
        else
        {
            BOSE_DEBUG( s_logger, "Preset not present for index:%d, intent:%d",
                       presetId, intent);
        }
    }
    break;
    }

    //Fire the cb so the control goes back to the ProductController
    if( GetCallbackObject() != nullptr )
    {
        ( *GetCallbackObject() )( intent );
    }
    return true;
}

void uint8_t PresetManager::IntentToIdMap(KeyHandlerUtil::ActionType_t& intent)
{
    switch( intent)
    {
        case ( uint16_t ) Action::PRESET_SELECT_1:
            return PRESET_2;
        case ( uint16_t ) Action::PRESET_SELECT_2:
            return PRESET_2;
        case ( uint16_t ) Action::PRESET_SELECT_3:
            return PRESET_3;
        case ( uint16_t ) Action::PRESET_SELECT_4:
            return PRESET_4;
        case ( uint16_t ) Action::PRESET_SELECT_5:
            return PRESET_5;
        case ( uint16_t ) Action::PRESET_SELECT_6:
            return PRESET_6;
        default:
            return PRESET_INVALID;
    }
}

void bool IsPresetValid(uint8_t presetId, SoundTouchInterface::preset &presetItem)
{
    if (GetProductController().GetPresets().preset_size())
    {
        TBD
    }
    return false;
}

void PresetManager::FrontDoorClientErrorCb( const FRONT_DOOR_CLIENT_ERRORS errorCode )
{
    // Nothing to do for now, printing this if anyone cares.
    BOSE_ERROR( s_logger, "%s:error code- %d", __func__, errorCode );
    return;
}
}
