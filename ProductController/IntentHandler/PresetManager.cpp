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
    BOSE_DEBUG( s_logger, "%s : Intent: %d", __func__, intent );

    uint8_t presetId = IntentToIdMap( intent );
    SoundTouchInterface::StatusJson status = CurrentStatusJson();
    switch( intent )
    {
    case( uint16_t ) Action::PRESET_SELECT_1:
    case( uint16_t ) Action::PRESET_SELECT_2:
    case( uint16_t ) Action::PRESET_SELECT_3:
    case( uint16_t ) Action::PRESET_SELECT_4:
    case( uint16_t ) Action::PRESET_SELECT_5:
    case( uint16_t ) Action::PRESET_SELECT_6:
    {
        SoundTouchInterface::preset presetsItem;
        if( IsPresetContentPreset( presetId, presetsItem ) )
        {
            // Send PlaybackRequest - TBD
            // Use persisted presetsPb to build playbackRequest
        }
        else
        {
            BOSE_DEBUG( s_logger, "Preset not present for presetId:%d, "
                        "intent:%d", presetId, intent );
        }
    }
    break;

    case( uint16_t ) Action::PRESET_STORE_1:
    case( uint16_t ) Action::PRESET_STORE_2:
    case( uint16_t ) Action::PRESET_STORE_3:
    case( uint16_t ) Action::PRESET_STORE_4:
    case( uint16_t ) Action::PRESET_STORE_5:
    case( uint16_t ) Action::PRESET_STORE_6:
    {
        if( ValidSourceAvailable() &&
            ( status == SoundTouchInterface::StatusJson::play ) &&
            ( status == SoundTouchInterface::StatusJson::buffering ) &&
            ( GetProductController().GetNowPlaying().has_container() ) &&
            ( GetProductController().GetNowPlaying().container().has_contentitem() ) &&
            ( GetProductController().GetNowPlaying().container().contentitem().has_presetable() ) &&
            ( GetProductController().GetNowPlaying().container().contentitem().presetable() ) )
        {
            // Use the content item from nowPlaying to send to Passport
            // for storing preset
            // TBD Later
        }
    }
    break;

    default:
    {
        BOSE_ERROR( s_logger, "%s: unsupported intent:%d", __func__, intent );
        return false;
    }
    }

    //Fire the cb so the control goes back to the ProductController
    if( GetCallbackObject() != nullptr )
    {
        ( *GetCallbackObject() )( intent );
    }
    return true;
}

uint8_t PresetManager::IntentToIdMap( KeyHandlerUtil::ActionType_t& intent )
{
    BOSE_DEBUG( s_logger, "%s", __func__ );
    switch( intent )
    {
    case( uint16_t ) Action::PRESET_SELECT_1:
    case( uint16_t ) Action::PRESET_STORE_1:
        return PRESET_1;
    case( uint16_t ) Action::PRESET_SELECT_2:
    case( uint16_t ) Action::PRESET_STORE_2:
        return PRESET_2;
    case( uint16_t ) Action::PRESET_SELECT_3:
    case( uint16_t ) Action::PRESET_STORE_3:
        return PRESET_3;
    case( uint16_t ) Action::PRESET_SELECT_4:
    case( uint16_t ) Action::PRESET_STORE_4:
        return PRESET_4;
    case( uint16_t ) Action::PRESET_SELECT_5:
    case( uint16_t ) Action::PRESET_STORE_5:
        return PRESET_5;
    case( uint16_t ) Action::PRESET_SELECT_6:
    case( uint16_t ) Action::PRESET_STORE_6:
        return PRESET_6;
    default:
        return PRESET_INVALID;
    }
}

bool PresetManager::IsPresetContentPreset( uint8_t presetId,
                                           SoundTouchInterface::preset &presetItem )
{
    BOSE_DEBUG( s_logger, "%s", __func__ );
    uint8_t index = 0;
    while( GetProductController().GetPresets().preset_size() > index )
    {
        if( ( GetProductController().GetPresets().preset( index ).has_id() ) &&
            ( GetProductController().GetPresets().preset( index ).id() == \
              presetId ) &&
            ( GetProductController().GetPresets().preset( index ).has_contentitem() ) )
        {
            presetItem = GetProductController().GetPresets().preset( index );
            return true;
        }
        index++;
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
