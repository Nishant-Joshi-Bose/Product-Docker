////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file      CustomProductKeyInputManager.cpp
///
/// @brief     This source code file implements a CustomProductKeyInputManager class that inherits
///            the base ProductKeyInputManager class to manage receiving and processing of raw key
///            input from the LPM hardware.
///
/// @author    Stuart J. Lumby
///
/// @attention Copyright (C) 2018 Bose Corporation All Rights Reserved
///
////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////
///
///            Included Header Files
///
////////////////////////////////////////////////////////////////////////////////////////////////////
#include "Utilities.h"
#include "CustomProductController.h"
#include "ProductSourceInfo.h"
#include "CustomProductKeyInputManager.h"
#include "MonotonicClock.h"
#include "AutoLpmServiceMessages.pb.h"
#include "SystemSourcesProperties.pb.h"

using namespace ProductSTS;
using namespace SystemSourcesProperties;

////////////////////////////////////////////////////////////////////////////////////////////////////
///                          Start of the Product Application Namespace                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
namespace ProductApp
{

////////////////////////////////////////////////////////////////////////////////////////////////////
///
///            Constant Definitions
///
////////////////////////////////////////////////////////////////////////////////////////////////////
constexpr const char KEY_CONFIGURATION_FILE_NAME[ ] = "/var/run/KeyConfiguration.json";
constexpr const char BLAST_CONFIGURATION_FILE_NAME[ ] = "/opt/Bose/etc/BlastConfiguration.json";

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief CustomProductKeyInputManager::CustomProductKeyInputManager
///
/// @param CustomProductController& ProductController
///
////////////////////////////////////////////////////////////////////////////////////////////////////
CustomProductKeyInputManager::CustomProductKeyInputManager( CustomProductController& ProductController )

    : ProductKeyInputManager( ProductController.GetTask( ),
                              ProductController.GetMessageHandler( ),
                              ProductController.GetLpmHardwareInterface( ),
                              ProductController.GetCommonCliClientMT(),
                              KEY_CONFIGURATION_FILE_NAME ),

      m_ProductController( ProductController ),
      m_TimeOfChordRelease( 0 ),
      m_KeyIdOfIncompleteChordRelease( BOSE_INVALID_KEY )
{
    InitializeDeviceController( );

    auto sourceInfoCb = [ this ]( const SoundTouchInterface::Sources & sources )
    {
        DeviceControllerClientMessages::SrcCiCodeMessage_t          codes;
        static DeviceControllerClientMessages::SrcCiCodeMessage_t   lastCodes;

        for( auto i = 0 ; i < sources.sources_size(); i++ )
        {
            const auto& source = sources.sources( i );

            if( source.has_details() and source.details().has_cicode() and m_ProductController.GetSourceInfo().IsSourceAvailable( source ) )
            {
                codes.add_cicode( source.details().cicode() );
            }
        }
        if( ( codes.SerializeAsString() != lastCodes.SerializeAsString() ) )
        {
            BOSE_INFO( s_logger, "notify cicodes : %s", ProtoToMarkup::ToJson( codes ).c_str() );
            m_deviceControllerPtr->NotifySourceCiCodes( codes );
            lastCodes = codes;
        }
    };
    m_ProductController.GetSourceInfo().RegisterSourceListener( sourceInfoCb );

}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   CustomProductKeyInputManager::InitializeDeviceController
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductKeyInputManager::InitializeDeviceController( )
{
    m_deviceControllerPtr = DeviceControllerClientFactory::Create( "CustomProductKeyInputManager",
                                                                   m_ProductController.GetTask( ) );

    if( !m_deviceControllerPtr )
    {
        BOSE_DIE( "Failed loading key blaster configuration file." );
    }

    m_deviceControllerPtr->LoadFilter( BLAST_CONFIGURATION_FILE_NAME );
    m_deviceControllerPtr->Connect( []( bool connected ) {} );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   CustomProductKeyInputManager::IsSourceKey
///
/// @param  const IpcKeyInformat_t& keyEvent
///
/// @return bool - true if it is a source key
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductKeyInputManager::IsSourceKey( const LpmServiceMessages::IpcKeyInformation_t& keyEvent )
{

    // ACTIVATION_KEY_GAME
    // ACTIVATION_KEY_BD_DVD
    // ACTIVATION_KEY_CBL_SAT

    switch( keyEvent.keyid() )
    {
    case BOSE_GAME_SOURCE:
    case BOSE_BD_DVD_SOURCE:
    case BOSE_CBL_SAT_SOURCE:
    case BOSE_TV_SOURCE:
    case BOSE_321_AUX_SOURCE:
    case BOSE_AUX_SOURCE:
        return true;
    default:
        return false;
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   CustomProductKeyInputManager::BlastKey
///
/// @param  const IpcKeyInformat_t& keyEvent
///
/// @param  const std::string& cicode
///
/// @return None
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductKeyInputManager::BlastKey(
    const IpcKeyInformation_t&  keyEvent,
    const std::string&          cicode
)
{
    DeviceControllerClientMessages::BoseKeyReqMessage_t request;

    request.set_keyval( keyEvent.keyid( ) );
    request.set_codeset( cicode );

    if( keyEvent.keystate( ) ==  KEY_PRESSED )
    {
        request.set_keyaction( DeviceControllerClientMessages::BoseKeyReqMessage_t::KEY_ACTION_CONTINUOUS_PRESS );
    }
    else
    {
        request.set_keyaction( DeviceControllerClientMessages::BoseKeyReqMessage_t::KEY_ACTION_END_PRESS );
    }

    BOSE_INFO( s_logger, "Blasting 0x%08x/%s (%s)", request.keyval( ), request.codeset( ).c_str( ),
               ( keyEvent.keystate( ) ==  KEY_PRESSED ) ? "PRESSED" : "RELEASED" );

    m_deviceControllerPtr->SendKey( request );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   CustomProductKeyInputManager::CustomProcessKeyEvent
///
/// @param  const IpcKeyInformation_t& keyEvent
///
/// @return This method returns a true value if the key is to be blasted so that no further
///         processing of the key event in the base ProductKeyInputManager class takes place;
///         otherwise, it returns false to allow further processing.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductKeyInputManager::CustomProcessKeyEvent( const IpcKeyInformation_t&
                                                          keyEvent )
{

    if( FilterIncompleteChord( keyEvent ) )
    {
        return true;
    }


    if( IsSourceKey( keyEvent ) )
    {

        BlastKey( keyEvent, "SourceKeysAreABlast" );

        // Still want this to be handled by ProductController key handler
        return false;
    }

    // TV_INPUT is a special case.  It should always be sent to tv source, regardless of what source is selected
    if( keyEvent.keyid( ) == BOSE_TV_INPUT )
    {
        const auto tvSource = m_ProductController.GetSourceInfo( ).FindSource( SHELBY_SOURCE::PRODUCT,  ProductSourceSlot_Name( TV ) );

        if( tvSource and tvSource->has_details( ) )
        {
            BlastKey( keyEvent, tvSource->details( ).cicode( ) );
        }

        return true;
    }

    // Do some sanity-checks to see if we can proceed
    // We can't determine anything more related to key blasting without knowing what source is selected
    const auto& nowSelection = m_ProductController.GetNowSelection( );
    if( not nowSelection.has_contentitem( ) )
    {
        return false;
    }
    auto sourceItem = m_ProductController.GetSourceInfo( ).FindSource( nowSelection.contentitem( ) );
    if( not sourceItem )
    {
        return false;
    }

    // CEC key handling is also special.
    // We need to make sure not to pass CEC key presses to the keyhandler if we're not in a product source
    // (key releases are okay to pass to KeyHandler in this case)
    // This limitation is required so that CEC keys that may be sent from a TV during a source switch to a non-product source
    // don't end up affecting the new source (for instance a TV may send a "mute" key when we're switching
    // from TV to Bluetooth; we don't want to process this)
    if(
        ( keyEvent.keyorigin( ) == KEY_ORIGIN_CEC ) and
        ( keyEvent.keystate( ) == KEY_PRESSED ) and
        ( sourceItem->sourcename().compare( SHELBY_SOURCE::PRODUCT ) != 0 )
    )
    {
        return true;
    }

    // The rest of the checks require a valid details field; if it doesn't exist, pass this to the KeyHandler
    // (note that sourceItem->has_details() is effectively a check for product source as well, since a non-product
    // source won't have details)
    if( not sourceItem->has_details( ) )
    {
        // TV source won't have "details" after a factory default (before /system/sources has been written)
        // In this case, we need to consume keys that normally would have been blasted
        if(
            ( sourceItem->sourceaccountname().compare( ProductSourceSlot_Name( TV ) ) == 0 ) and
            m_deviceControllerPtr->IsBlastedKey( keyEvent.keyid( ), DEVICE_TYPE__Name( DEVICE_TYPE_TV ) ) )
        {
            BOSE_INFO( s_logger, "%s consuming key for unconfigured TV", __func__ );
            return true;
        }

        return false;
    }

    // Determine whether this is a blasted key for the current device type; if not, pass it to KeyHandler
    if( not m_deviceControllerPtr->IsBlastedKey( keyEvent.keyid( ), sourceItem->details( ).devicetype( ) ) )
    {
        return false;
    }

    // If the device has been configured, blast the key (if it hasn't been configured but it's a key
    // that normally would have been blasted, we'll consume the key)
    if( m_ProductController.GetSourceInfo().IsSourceAvailable( *sourceItem ) )
    {
        BlastKey( keyEvent, sourceItem->details( ).cicode( ) );
    }
    else
    {
        BOSE_INFO( s_logger, "%s unconfigured source - consuming key", __func__ );
    }

    return true;
}

void CustomProductKeyInputManager::ExecutePowerMacro( const ProductPb::PowerMacro& pwrMacro )
{
    if( pwrMacro.enabled() )
    {
        BOSE_INFO( s_logger, "Executing power macro : %s", pwrMacro.ShortDebugString().c_str() );
        if( pwrMacro.powerontv() )
        {
            const auto tvSource = m_ProductController.GetSourceInfo( ).FindSource( SHELBY_SOURCE::PRODUCT,  ProductSTS::ProductSourceSlot_Name( ProductSTS::TV ) );
            if( tvSource and tvSource->has_details( ) and tvSource->details().has_cicode() )
            {
                DeviceControllerClientMessages::BoseKeyReqMessage_t request;
                request.set_keyaction( DeviceControllerClientMessages::BoseKeyReqMessage_t::KEY_ACTION_SINGLE_PRESS );
                request.set_keyval( BOSE_ASSERT_ON );
                request.set_codeset( tvSource->details( ).cicode( ) );
            }
        }
        if( pwrMacro.has_powerondevice() )
        {
            const auto macroSrc = m_ProductController.GetSourceInfo( ).FindSource( SHELBY_SOURCE::PRODUCT,  ProductSTS::ProductSourceSlot_Name( pwrMacro.powerondevice() ) );
            if( macroSrc and macroSrc->has_details( ) and macroSrc->details().has_cicode() )
            {
                DeviceControllerClientMessages::BoseKeyReqMessage_t request;
                request.set_keyaction( DeviceControllerClientMessages::BoseKeyReqMessage_t::KEY_ACTION_SINGLE_PRESS );
                request.set_keyval( BOSE_ASSERT_ON );
                request.set_codeset( macroSrc->details( ).cicode( ) );
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   CustomProductKeyInputManager::FilterIncompleteChord
///
/// @param  const IpcKeyInformation_t& keyEvent
///
/// @return This method returns a true value if the key is to be ignored because of incomplete
///         chord processing. That would be when keys A and B are PnH so they produce a special
///         value S, then A is released while B is still held. In this case, both the press and the
///         release for B needs to be ignored. This condition is detected by the arrival of
///         the release for S followed by the press of B within a short amount of time.
///         otherwise, it returns false to allow further processing.
///
///         This is driven by the remote defects described in PGC-1427.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductKeyInputManager::FilterIncompleteChord( const IpcKeyInformation_t&
                                                          keyEvent )
{
    constexpr int64_t CHORD_RECOVERY_TIME_MS = 20;
    const int64_t timeNow = MonotonicClock::NowMs( );
    bool retVal = false;

    if( keyEvent.keystate( ) == KEY_PRESSED )
    {
        if( ( timeNow - m_TimeOfChordRelease ) > CHORD_RECOVERY_TIME_MS )
        {
            // This key was pressed long enough after the chord key release, so we are out of danger
            m_KeyIdOfIncompleteChordRelease = BOSE_INVALID_KEY;
        }
        else
        {
            // This key was pressed within the danger zone, record it so we can wait for its release
            m_KeyIdOfIncompleteChordRelease = keyEvent.keyid( );
            retVal = true;
        }
    }
    else
    {
        if( keyEvent.keyid( ) >= BOSE_FD && keyEvent.keyid( ) <= BOSE_USB_NET_SVR )
        {
            // This is a chord key release event - record the time
            m_TimeOfChordRelease = timeNow;
        }
        else
        {
            // This is a release of a non-chord key
            if( keyEvent.keyid( ) == m_KeyIdOfIncompleteChordRelease )
            {
                // This is the key that was pressed within the danger zone
                m_KeyIdOfIncompleteChordRelease = BOSE_INVALID_KEY;
                retVal = true;
            }
        }
    }

    BOSE_VERBOSE( s_logger, "%s( %s ) @ %lld returning %s", __func__, keyEvent.ShortDebugString().c_str( ), timeNow, retVal ? "true" : "false" );

    return retVal;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                           End of the Product Application Namespace                           ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
