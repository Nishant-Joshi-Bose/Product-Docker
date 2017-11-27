///////////////////////////////////////////////////////////////////////////////
/// @file PresetManager.cpp
///
/// @brief Implementation of PresetManager
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
    IntentManager( task, cliClient, frontDoorClient, controller ),
    m_NowPlayingRsp( nullptr, &task )
{
    m_frontDoorClientErrorCb = AsyncCallback<FRONT_DOOR_CLIENT_ERRORS>\
                               ( std::bind( &PresetManager::FrontDoorClientErrorCb,
                                            this, std::placeholders::_1 ), &task );
    m_NowPlayingRsp = AsyncCallback<SoundTouchInterface::NowPlayingJson>\
                      ( std::bind( &PresetManager::PutTransportControlCbRsp,
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

    SoundTouchInterface::PresetID presetId = IntentToPresetIdMap( intent );
    BOSE_DEBUG( s_logger, "%d", ( uint32_t ) presetId );
    SoundTouchInterface::StatusJson status = CurrentNowPlayingStatusJson();
    switch( intent )
    {
    case( uint16_t ) Action::PRESET_SELECT_1:
    case( uint16_t ) Action::PRESET_SELECT_2:
    case( uint16_t ) Action::PRESET_SELECT_3:
    case( uint16_t ) Action::PRESET_SELECT_4:
    case( uint16_t ) Action::PRESET_SELECT_5:
    case( uint16_t ) Action::PRESET_SELECT_6:
    {
        SoundTouchInterface::preset *presetItem =
            IsPresetContentPresent( presetId, m_presets );
        // Look for the preset if present
        if( presetItem != nullptr )
        {
            // We have to use the presetPb from PC, for now use local copy
            SoundTouchInterface::playbackRequestJson pbReqJson;
            BuildPlaybackRequestFromPresetCI( pbReqJson,
                                              presetItem->contentitem() );
            GetFrontDoorClient()->SendPost<SoundTouchInterface::\
            NowPlayingJson>( "/content/playbackRequest", pbReqJson,
                             m_NowPlayingRsp, m_frontDoorClientErrorCb );

        }
        else
        {
            BOSE_DEBUG( s_logger, "Preset not present for presetId:%d, "
                        "intent:%d", ( uint8_t ) presetId, intent );
            return false;
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
            ( ( status == SoundTouchInterface::StatusJson::play ) ||
              ( status == SoundTouchInterface::StatusJson::buffering ) ) &&
            ( GetProductController().GetNowPlaying().has_container() ) &&
            ( GetProductController().GetNowPlaying().container().\
              has_contentitem() ) &&
            ( GetProductController().GetNowPlaying().container().contentitem().\
              presetable() ) )
        {
            // Use the content item from nowPlaying to send to Passport
            // for storing preset

#if 0 // Later when we have passport and persistence support
            // Make a local copy to edit and send @todo
            m_presets.CopyFrom( GetProductController().GetPresets() );
#endif

            // Look for the preset if present
            SoundTouchInterface::preset *presetItem =
                IsPresetContentPresent( presetId, m_presets );
            if( presetItem == nullptr )
            {
                // if not found, add it
                BOSE_DEBUG( s_logger, "Creating a new preset item in presets"
                            " for presetId:%d", ( uint8_t ) presetId );
                presetItem = m_presets.add_preset();
            }
            presetItem->set_id( presetId );
            BuildPresetContentItemFromNp( presetItem->mutable_contentitem(),
                                          GetProductController().GetNowPlaying().container().contentitem() );

            // HACK: For now we have changed the local copy m_presets, but
            // this will change when we integrate with the passport(cloud).
            // Send m_presets to passport using PassportClient IPC message, TBD
            // @todo
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

SoundTouchInterface::PresetID PresetManager::IntentToPresetIdMap( KeyHandlerUtil::ActionType_t& intent )
{
    BOSE_DEBUG( s_logger, "%s", __func__ );
    switch( intent )
    {
    case( uint16_t ) Action::PRESET_SELECT_1:
    case( uint16_t ) Action::PRESET_STORE_1:
        return SoundTouchInterface::PresetID::P1;
    case( uint16_t ) Action::PRESET_SELECT_2:
    case( uint16_t ) Action::PRESET_STORE_2:
        return SoundTouchInterface::PresetID::P2;
    case( uint16_t ) Action::PRESET_SELECT_3:
    case( uint16_t ) Action::PRESET_STORE_3:
        return SoundTouchInterface::PresetID::P3;
    case( uint16_t ) Action::PRESET_SELECT_4:
    case( uint16_t ) Action::PRESET_STORE_4:
        return SoundTouchInterface::PresetID::P4;
    case( uint16_t ) Action::PRESET_SELECT_5:
    case( uint16_t ) Action::PRESET_STORE_5:
        return SoundTouchInterface::PresetID::P5;
    case( uint16_t ) Action::PRESET_SELECT_6:
    case( uint16_t ) Action::PRESET_STORE_6:
        return SoundTouchInterface::PresetID::P6;
    default:
        return SoundTouchInterface::PresetID::UNDEFINED_PRESET;
    }
}

SoundTouchInterface::preset* \
PresetManager::IsPresetContentPresent( SoundTouchInterface::PresetID presetId,
                                       SoundTouchInterface::presets &presetItem ) const
{
    BOSE_DEBUG( s_logger, "%s: Number of preset stored: %d", __func__,
                presetItem.preset_size() );
    uint8_t index = 0;
    while( presetItem.preset_size() > index )
    {
        if( ( presetItem.preset( index ).has_id() ) &&
            ( presetItem.preset( index ).id() == presetId ) &&
            ( presetItem.preset( index ).has_contentitem() ) )
        {
            BOSE_DEBUG( s_logger, "%s: index:%d, Preset Id:%d has contentitem",
                        __func__, index, ( uint8_t ) presetId );
            return( presetItem.mutable_preset( index ) );
        }
        index++;
    }
    return nullptr;
}

void PresetManager::BuildPlaybackRequestFromPresetCI( SoundTouchInterface::playbackRequestJson &pr, const SoundTouchInterface::ContentItem &PresetCi ) const
{
    BOSE_DEBUG( s_logger, "%s:", __func__ );

    if( PresetCi.has_source() )
    {
        pr.set_source( PresetCi.source() );
    }
    if( PresetCi.has_sourceaccount() )
    {
        pr.set_sourceaccount( PresetCi.sourceaccount() );
    }
    SoundTouchInterface::playbackRequestJson::playbackRequestItem *pri =
        pr.mutable_preset();
    if( pri != nullptr )
    {
        if( PresetCi.has_type() )
        {
            pri->set_type( PresetCi.type() );
        }
        if( PresetCi.has_location() )
        {
            pri->set_location( PresetCi.location() );
        }
#if 0 // TBD, does not match @todo
        if( PresetCi.has_itemname() )
        {
            pri->set_name( PresetCi.itemname() );
        }
#endif
        if( PresetCi.ispresetable() )
        {
            pri->set_presetable( PresetCi.ispresetable() );
        }
#if 0 // TBD, does not match @todo
        if( PresetCi.has_containerart() )
        {
            pri->set_containerart( PresetCi.containerart() );
        }
#endif
    }
    return;
}

void PresetManager::BuildPresetContentItemFromNp( SoundTouchInterface::ContentItem *destinationCI, const SoundTouchInterface::NowPlayingJson_ContentItemJson &sourceCI ) const
{
    BOSE_DEBUG( s_logger, "%s:", __func__ );
    if( destinationCI != nullptr )
    {
        if( sourceCI.has_type() )
        {
            destinationCI->set_type( sourceCI.type() );
        }
        if( sourceCI.has_location() )
        {
            destinationCI->set_location( sourceCI.location() );
        }
#if 0 // TBD, they don't match @todo
        if( sourceCI.has_name() )
        {
            destinationCI->set_itemname( sourceCI.name() );
        }
#endif
        if( sourceCI.has_source() )
        {
            destinationCI->set_source( sourceCI.source() );
        }
        if( sourceCI.has_subsource() )
        {
            destinationCI->set_subsource( sourceCI.subsource() );
        }
        if( sourceCI.has_sourceaccount() )
        {
            destinationCI->set_sourceaccount( sourceCI.sourceaccount() );
        }
        if( sourceCI.has_presetable() )
        {
            destinationCI->set_ispresetable( sourceCI.presetable() );
        }
#if 0 // TBD, they don't match @todo
        if( sourceCI.has_containerart() )
        {
            destinationCI->set_containerart( sourceCI.containerart() );
        }
#endif
    }
    else
    {
        BOSE_ERROR( s_logger, "Error: nullptr for destinationCI" );
    }
    return;
}
void PresetManager::PutTransportControlCbRsp( const SoundTouchInterface::NowPlayingJson& resp )
{
    // No Need to handle this as Product Controller will get a nowPlaying that
    // will update update the information.
    BOSE_DEBUG( s_logger, "%s", __func__ );
    return;
}
void PresetManager::FrontDoorClientErrorCb( const FRONT_DOOR_CLIENT_ERRORS errorCode )
{
    // Nothing to do for now, printing this if anyone cares.
    BOSE_ERROR( s_logger, "%s:error code- %d", __func__, errorCode );
    return;
}
}
