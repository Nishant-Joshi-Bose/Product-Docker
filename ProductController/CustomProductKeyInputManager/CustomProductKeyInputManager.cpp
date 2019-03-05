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
// #include "UEIKeyNames.pb.h"
#include "SystemUtils.h"

using namespace ProductSTS;
using namespace SystemSourcesProperties;
using namespace LpmServiceMessages;

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
constexpr const char BLAST_CONFIGURATION_FILE_NAME[ ] = BOSE_CONF_DIR "BlastConfiguration.json";
constexpr const char USER_KEY_CONFIGURATION_FILE_NAME[ ] = BOSE_CONF_DIR "UserKeyConfig.json";

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
                              ProductController.GetDataCollectionClient( ),
                              KEY_CONFIGURATION_FILE_NAME ),

      m_ProductController( ProductController ),
      m_deviceControllerPtr( ProductController.GetDeviceControllerClient() ),
      m_TimeOfChordRelease( 0 ),
      m_KeyIdOfIncompleteChordRelease( BOSE_INVALID_KEY )
{
}

void CustomProductKeyInputManager::Run( const Callback<>& cancelAlarmCallback )
{
    ProductKeyInputManager::Run( cancelAlarmCallback );

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
/// @name   ProductKeyInputInterface::InitializeKeyFilter
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductKeyInputManager::InitializeKeyFilter( )
{
    const auto& filter = SystemUtils::ReadFile( USER_KEY_CONFIGURATION_FILE_NAME );
    if( !filter )
    {
        BOSE_DIE( __PRETTY_FUNCTION__ << ": Failed loading key filter" );
        return;
    }

    try
    {
        ProtoToMarkup::FromJson( *filter, &m_filterTable, "KeyFilter" );
    }
    catch( const ProtoToMarkup::MarkupError & e )
    {
        BOSE_DIE( __PRETTY_FUNCTION__ << ": Markup error in key configuration file: " << e.what( ) );
    }

    for( const auto& e : m_filterTable.keytable() )
    {
        const auto& f = e.filter();
        for( const auto& s : f.sources() )
        {
            try
            {
                // A "filter" entry in the filter table contains a "sources" array, which
                // in turn contains a pair of regular expressions that are applied to the source name
                // and source account name
                //
                // "filter": {
                //     "sources": [
                //         { "sourceName": "(PRODUCT|INVALID_SOURCE)", "sourceAccountName": ".*" }
                //     ],
                //
                // m_filterRegex is a map of vectors of FilterRegex structs, where the vector represents
                // the "sources" array and the FilterRegex contains regular expression objects
                // associated with sourceName and sourceAccountName.  The map is indexed by the address
                // of the filter structure from the filter table.
                m_filterRegex[&f].push_back( FilterRegex( s.sourcename(), s.sourceaccountname() ) );
            }
            catch( const std::regex_error& e )
            {
                BOSE_DIE( __PRETTY_FUNCTION__ << ": regex error " << e.what() << ": sourceName = " <<
                          s.sourcename() << ": accountName = " << s.sourceaccountname() );
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   CustomProductKeyInputManager::IsBlastableEvent
///
/// @param  const IpcKeyInformation_t& keyEvent
///
/// @return true if event is blastable, false otherwise
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductKeyInputManager::IsBlastableEvent(
    const IpcKeyInformation_t&  keyEvent
)
{
    auto origin = keyEvent.keyorigin();

    return (
               ( origin == KEY_ORIGIN_RF ) ||
               ( origin == KEY_ORIGIN_NETWORK ) ||
               ( origin == KEY_ORIGIN_TAP )
           );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   CustomProductKeyInputManager::BlastKey
///
/// @param  const IpcKeyInformation_t& keyEvent
///
/// @param  const std::string& cicode
///
/// @return true if key was blasted, false otherwise
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductKeyInputManager::BlastKey(
    const IpcKeyInformation_t&  keyEvent,
    const std::string&          cicode
)
{
    if( !IsBlastableEvent( keyEvent ) )
    {
        return false;
    }


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
    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   CustomProductKeyInputManager::AccommodateOrphanReleaseEvents
///
/// @param  const IpcKeyInformation_t& keyEvent
///
/// @param  bool pressRet
///
/// @return This is a convenience method that
///             1) Determines the correct return value of CustomProcessKeyEvent
///             2) For KEY_PRESSED events, saves the return status of CustomProcessKeyEvent for the given key+origin combination
///
///         The purpose of this function is to ensure that for a given key+origin press event,
///         CustomProductKeyInputManager returns the same value for the corresponding release event.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductKeyInputManager::AccommodateOrphanReleaseEvents( const IpcKeyInformation_t& keyEvent, bool pressRet )
{
    unsigned idx = ( keyEvent.keyorigin( ) * LpmServiceMessages::NUM_BOSE_KEYS ) + keyEvent.keyid( );

    if( keyEvent.keystate( ) == KEY_PRESSED )
    {
        m_lastPressStatus[ idx ] = pressRet;
        return pressRet;
    }
    // we shouldn't ever get here without having a corresponding press entry, but
    // let's be safe and check
    auto ret = m_lastPressStatus.find( idx );
    if( ret == m_lastPressStatus.end( ) )
    {
        BOSE_WARNING( s_logger, "%s: got release without press for origin %s, key %s", __PRETTY_FUNCTION__,
                      KeyOrigin_t_Name( static_cast< KeyOrigin_t >( keyEvent.keyorigin( ) ) ).c_str( ),
                      KEY_VALUE_Name( static_cast< KEY_VALUE >( keyEvent.keyid( ) ) ).c_str( ) );
        return false;
    }

    auto status = ret->second;
    m_lastPressStatus.erase( ret );
    return status;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   CustomProductKeyInputManager::CustomProcessKeyEvent
///
/// @param  const IpcKeyInformation_t& keyEvent
///
/// @return This method returns a true value if the key was consumed so that no further
///         processing of the key event in the base ProductKeyInputManager class takes place;
///         otherwise, it returns false to allow further processing.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductKeyInputManager::CustomProcessKeyEvent( const IpcKeyInformation_t& keyEvent )
{
    if( FilterIncompleteChord( keyEvent ) )
    {
        return AccommodateOrphanReleaseEvents( keyEvent, true );
    }

    auto keyid = keyEvent.keyid( );

    // blast release unconditionally (BlastKey will check origin)
    // if nothing is currently being blasted this will have no effect
    if( keyEvent.keystate( ) == KEY_RELEASED )
    {
        BlastKey( keyEvent, "none" );
    }

    // TV_INPUT is a special case.  It should always be sent to tv source, regardless of what source is selected
    if( keyid == BOSE_TV_INPUT )
    {
        const auto tvSource = m_ProductController.GetSourceInfo( ).FindSource( SHELBY_SOURCE::PRODUCT,  ProductSourceSlot_Name( TV ) );

        if( ( tvSource and tvSource->has_details( ) ) && ( keyEvent.keystate() == KEY_PRESSED ) )
        {
            BlastKey( keyEvent, tvSource->details( ).cicode( ) );
        }

        return AccommodateOrphanReleaseEvents( keyEvent, true );
    }

    // Do some sanity-checks to see if we can proceed
    // We can't determine anything more related to key blasting without knowing what source is selected
    const auto& nowSelection = m_ProductController.GetNowSelection( );
    if( not nowSelection.has_contentitem( ) )
    {
        return AccommodateOrphanReleaseEvents( keyEvent, false );
    }
    auto sourceItem = m_ProductController.GetSourceInfo( ).FindSource( nowSelection.contentitem( ) );
    if( not sourceItem )
    {
        return AccommodateOrphanReleaseEvents( keyEvent, false );
    }

    // CEC key handling is also special.
    // We need to make sure not to pass CEC key presses to the keyhandler if we're not in a product source
    // (key releases are okay to pass to KeyHandler in this case)
    // This limitation is required so that CEC keys that may be sent from a TV during a source switch to a non-product source
    // don't end up affecting the new source (for instance a TV may send a "mute" key when we're switching
    // from TV to Bluetooth; we don't want to process this)
    if(
        ( keyEvent.keyorigin( ) == KEY_ORIGIN_CEC ) and
        ( sourceItem->sourcename().compare( SHELBY_SOURCE::PRODUCT ) != 0 )
    )
    {
        return AccommodateOrphanReleaseEvents( keyEvent, true );
    }

    // The rest of the checks require a valid details field; if it doesn't exist, pass this to the KeyHandler
    // (note that sourceItem->has_details() is effectively a check for product source as well, since a non-product
    // source won't have details)
    if( not sourceItem->has_details( ) )
    {
        // TV source won't have "details" after a factory default (before /system/sources has been written)
        // In this case, we need to consume keys that normally would have been blasted
        if( ( sourceItem->sourceaccountname().compare( ProductSourceSlot_Name( TV ) ) == 0 ) and
            m_deviceControllerPtr->IsBlastedKey( keyid, DEVICE_TYPE__Name( DEVICE_TYPE_TV ) ) )
        {
            BOSE_INFO( s_logger, "%s consuming key for unconfigured TV", __func__ );
            return AccommodateOrphanReleaseEvents( keyEvent, true );
        }

        return AccommodateOrphanReleaseEvents( keyEvent, false );
    }

    // Determine whether this is a blasted key for the current device type; if not, pass it to KeyHandler
    if( not m_deviceControllerPtr->IsBlastedKey( keyid, sourceItem->details( ).devicetype( ) ) )
    {
        return AccommodateOrphanReleaseEvents( keyEvent, false );
    }

    // If the device has been configured, blast the key (if it hasn't been configured but it's a key
    // that normally would have been blasted, we'll consume the key)
    if( m_ProductController.GetSourceInfo().IsSourceAvailable( *sourceItem ) && ( keyEvent.keystate() == KEY_PRESSED ) )
    {
        BlastKey( keyEvent, sourceItem->details( ).cicode( ) );
    }
    else
    {
        BOSE_INFO( s_logger, "%s unconfigured source - consuming key", __func__ );
    }

    return AccommodateOrphanReleaseEvents( keyEvent, true );
}

void CustomProductKeyInputManager::ExecutePowerMacro( const ProductPb::PowerMacro& pwrMacro, LpmServiceMessages::KEY_VALUE key )
{
    // TODO - MONTAUK-360 fix this to be more inline with new implementation
    if( key != LpmServiceMessages::BOSE_ASSERT_ON && key != LpmServiceMessages::BOSE_ASSERT_OFF )
    {
        BOSE_ERROR( s_logger, "Unexpected key value %d", key );
        return;
    }

    if( pwrMacro.enabled() )
    {
        BOSE_INFO( s_logger, "Executing power macro %s : %s", ( key == LpmServiceMessages::BOSE_ASSERT_ON ? "on" : "off" ),
                   pwrMacro.ShortDebugString().c_str() );

        auto srcMacro = [ this, key, pwrMacro]()
        {
            if( pwrMacro.has_powerondevice() )
            {
                const auto macroSrc = m_ProductController.GetSourceInfo( ).FindSource( SHELBY_SOURCE::PRODUCT,  ProductSTS::ProductSourceSlot_Name( pwrMacro.powerondevice() ) );
                if( macroSrc and macroSrc->has_details( ) and macroSrc->details().has_cicode() )
                {
                    DeviceControllerClientMessages::BoseKeyReqMessage_t request;
                    request.set_keyaction( DeviceControllerClientMessages::BoseKeyReqMessage_t::KEY_ACTION_SINGLE_PRESS );
                    request.set_keyval( key );
                    request.set_codeset( macroSrc->details( ).cicode( ) );
                    m_deviceControllerPtr->SendKey( request );
                }
            }
        };

        auto cbFunc = [ this, key, pwrMacro, srcMacro]( DeviceControllerClientMessages::BoseKeyReqMessage_t resp )
        {
            srcMacro( );
        };
        AsyncCallback<DeviceControllerClientMessages::BoseKeyReqMessage_t> respCb( cbFunc, m_ProductController.GetTask() );

        if( pwrMacro.powerontv() )
        {
            const auto tvSource = m_ProductController.GetSourceInfo( ).FindSource( SHELBY_SOURCE::PRODUCT,  ProductSTS::ProductSourceSlot_Name( ProductSTS::TV ) );
            if( tvSource and tvSource->has_details( ) and tvSource->details().has_cicode() )
            {
                DeviceControllerClientMessages::BoseKeyReqMessage_t request;
                request.set_keyaction( DeviceControllerClientMessages::BoseKeyReqMessage_t::KEY_ACTION_SINGLE_PRESS );
                request.set_keyval( key );
                request.set_codeset( tvSource->details( ).cicode( ) );
                // Wait for the callback from DeviceController before sending the next key
                m_deviceControllerPtr->SendKey( request, respCb );
            }
            else
            {
                srcMacro();
            }
        }
        else
        {
            srcMacro( );
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
///
/// @name   CustomProductKeyInputManager::IntentName
///
/// @param  KeyHandlerUtil::ActionType_t intent
///
/// @return This method returns the name of the supplied intent
///
////////////////////////////////////////////////////////////////////////////////////////////////////
const std::string& CustomProductKeyInputManager::IntentName( KeyHandlerUtil::ActionType_t intent )
{
    return ( intent <= Action::ActionCommon_t::ACTION_COMMON_LAST ) ?
           Action::ActionCommon_t::Actions_Name( static_cast<Action::ActionCommon_t::Actions>( intent ) ) :
           Action::ActionCustom_t::Actions_Name( static_cast<Action::ActionCustom_t::Actions>( intent ) );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   CustomProductKeyInputManager::FilterIntent
///
/// @param  KeyHandlerUtil::ActionType_t& intent
///         Note "intent" may be modified if the filter contains a "translate" entry
///
/// @return This method returns a true value if the intent is to be ignored
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductKeyInputManager::FilterIntent( KeyHandlerUtil::ActionType_t& intent ) const
{
    using namespace KeyFilter;
    using namespace LpmServiceMessages;

    ///
    /// First we check the key table to see if there's an entry matching this intent
    ///
    const auto& intentName = IntentName( intent );
    auto matchEntry = [ intentName ]( const KeyEntry & e )
    {
        return intentName == e.action();
    };
    const auto& entries = m_filterTable.keytable( );
    const auto& it = std::find_if( entries.begin(), entries.end(), matchEntry );
    if( it == entries.end() )
    {
        BOSE_WARNING( s_logger, "%s No entry for %s (%d)", __PRETTY_FUNCTION__, intentName.c_str(), intent );
        return false;
    }

    if( !it->has_filter( ) )
    {
        return false;
    }

    const auto& nowSelection = m_ProductController.GetNowSelection( );
    BOSE_DEBUG( s_logger, "%s: nowSelection %s", __PRETTY_FUNCTION__,  ProtoToMarkup::ToJson( nowSelection ).c_str() );
    const auto& source = nowSelection.contentitem( ).source( );
    const auto& sourceAccount = nowSelection.contentitem( ).sourceaccount( );
    auto filterSource = [ source, sourceAccount]( const FilterRegex & f )
    {
        BOSE_DEBUG( s_logger, "%s: check %s %s", __PRETTY_FUNCTION__,  source.c_str(), sourceAccount.c_str() );

        try
        {
            return ( std::regex_match( source, f.m_sourceFilter ) && std::regex_match( sourceAccount, f.m_sourceAccountFilter ) );
        }
        catch( const std::regex_error& e )
        {
            BOSE_ERROR( s_logger, "%s: regex error %s (%d)", __PRETTY_FUNCTION__, e.what(), e.code() );
            return false;
        }
    };
    const auto& filter = it->filter();
    const auto& filterRegex = m_filterRegex.find( &filter );
    if( filterRegex == m_filterRegex.end() )
    {
        BOSE_ERROR( s_logger, "%s: Couldn't find filter regex for intent %s (this should not happen!)", __PRETTY_FUNCTION__, intentName.c_str() );
        return false;
    }
    const auto& itf = std::find_if( filterRegex->second.begin(), filterRegex->second.end(), filterSource );
    if( itf != filterRegex->second.end() )
    {
        // We found a matching source, this intent is to be filtered
        BOSE_INFO( s_logger, "%s: Discard intent %s due to filter", __PRETTY_FUNCTION__, intentName.c_str() );
        return true;
    }

    if( ! filter.has_translate() )
    {
        return false;
    }

    const auto& translate = filter.translate();
    ActionCustom_t::Actions customAction = static_cast<ActionCustom_t::Actions>( intent );
    ActionCommon_t::Actions commonAction = static_cast<ActionCommon_t::Actions>( intent );
    if( ActionCustom_t::Actions_Parse( translate, &customAction ) )
    {
        intent = static_cast<KeyHandlerUtil::ActionType_t>( customAction );
    }
    else if( ActionCommon_t::Actions_Parse( translate, &commonAction ) )
    {
        intent = static_cast<KeyHandlerUtil::ActionType_t>( commonAction );
    }
    BOSE_INFO( s_logger, "%s: translate %s -> %s", __PRETTY_FUNCTION__, intentName.c_str(), translate.c_str() );

    return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                           End of the Product Application Namespace                           ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
