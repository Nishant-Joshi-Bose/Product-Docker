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
#include "ProfessorProductController.h"
#include "ProductSourceInfo.h"
#include "CustomProductKeyInputManager.h"
#include "MonotonicClock.h"
#include "AutoLpmServiceMessages.pb.h"

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
/// @param ProfessorProductController& ProductController
///
////////////////////////////////////////////////////////////////////////////////////////////////////
CustomProductKeyInputManager::CustomProductKeyInputManager( ProfessorProductController& ProductController )

    : ProductKeyInputManager( ProductController.GetTask( ),
                              ProductController.GetMessageHandler( ),
                              ProductController.GetLpmHardwareInterface( ),
                              ProductController.GetCommandLineInterface( ),
                              KEY_CONFIGURATION_FILE_NAME ),

      m_ProductController( ProductController ),
      m_TimeOfChordRelease( 0 ),
      m_KeyIdOfIncompleteChordRelease( BOSE_INVALID_KEY )
{
    InitializeQuickSetService( );

    auto sourceInfoCb = [ this ]( const SoundTouchInterface::Sources & sources )
    {
        QSSMSG::SrcCiCodeMessage_t          codes;
        static QSSMSG::SrcCiCodeMessage_t   lastCodes;

        for( auto i = 0 ; i < sources.sources_size(); i++ )
        {
            const auto& source = sources.sources( i );

            // TODO - we need to check "visible" here as well, but it's not yet supported
            if( source.has_details() and source.details().has_cicode() )
            {
                codes.add_cicode( source.details().cicode() );
            }
        }
        if( codes.cicode_size() and ( codes.SerializeAsString() != lastCodes.SerializeAsString() ) )
        {
            BOSE_INFO( s_logger, "notify cicodes : %s", ProtoToMarkup::ToJson( codes ).c_str() );
            m_QSSClient->NotifySourceCiCodes( codes );
            lastCodes = codes;
        }
    };
    m_ProductController.GetSourceInfo()->RegisterSourceListener( sourceInfoCb );

}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductKeyInputInterface::InitializeQuickSetService
///
////////////////////////////////////////////////////////////////////////////////////////////////////
void CustomProductKeyInputManager::InitializeQuickSetService( )
{
    m_QSSClient = A4VQuickSetServiceClientFactory::Create( "CustomProductKeyInputManager",
                                                           m_ProductController.GetTask( ) );

    if( not m_QSSClient )
    {
        BOSE_DIE( "Failed loading key blaster configuration file." );
    }

    m_QSSClient->LoadFilter( BLAST_CONFIGURATION_FILE_NAME );
    m_QSSClient->Connect( [ ]( bool connected ) { } );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   CustomProductKeyInputManager::CustomProcessKeyEvent
///
/// @param  const LpmServiceMessages::IpcKeyInformation_t& keyEvent
///
/// @return This method returns a true value if the key is to be blasted so that no further
///         processing of the key event in the base ProductKeyInputManager class takes place;
///         otherwise, it returns false to allow further processing.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductKeyInputManager::CustomProcessKeyEvent( const LpmServiceMessages::IpcKeyInformation_t&
                                                          keyEvent )
{
    if( FilterIncompleteChord( keyEvent ) )
    {
        return true;
    }
    ///
    /// Decide if this key should be blasted or sent to the key handler
    ///
    bool isBlastedKey = false;
    const auto& nowSelection = m_ProductController.GetNowSelection( );
    std::string cicode;

    if( nowSelection.has_contentitem( ) )
    {
        const auto& contentItem = nowSelection.contentitem( );
        const auto source = m_ProductController.GetSourceInfo( )->FindSource( contentItem );

        if( source and source->has_details( ) )
        {
            if( source->details( ).has_devicetype( ) )
            {
                isBlastedKey = m_QSSClient->IsBlastedKey( keyEvent.keyid( ), source->details( ).devicetype( ) );
            }

            cicode = source->details( ).cicode( );
        }
    }

    if( isBlastedKey )
    {
        ///
        /// This key should be blasted.
        ///
        QSSMSG::BoseKeyReqMessage_t request;

        request.set_keyval( keyEvent.keyid( ) );
        request.set_codeset( cicode );

        if( keyEvent.keystate( ) ==  LpmServiceMessages::KEY_PRESSED )
        {
            request.set_keyaction( QSSMSG::BoseKeyReqMessage_t::KEY_ACTION_CONTINUOUS_PRESS );
        }
        else
        {
            request.set_keyaction( QSSMSG::BoseKeyReqMessage_t::KEY_ACTION_END_PRESS );
        }

        BOSE_INFO( s_logger, "Blasting 0x%08x/%s (%s)", request.keyval( ), request.codeset( ).c_str( ),
                   ( keyEvent.keystate( ) ==  LpmServiceMessages::KEY_PRESSED ) ? "PRESSED" : "RELEASED" );

        m_QSSClient->SendKey( request );
    }

    return( isBlastedKey );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   CustomProductKeyInputManager::FilterIncompleteChord
///
/// @param  const LpmServiceMessages::IpcKeyInformation_t& keyEvent
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
bool CustomProductKeyInputManager::FilterIncompleteChord( const LpmServiceMessages::IpcKeyInformation_t&
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
