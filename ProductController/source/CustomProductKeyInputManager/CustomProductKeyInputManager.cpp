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
#include "CustomProductKeyInputManager.h"

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
constexpr const char* KEY_CONFIGURATION_FILE_NAME = "/opt/Bose/etc/KeyConfiguration.json";
constexpr const char* BLAST_CONFIGURATION_FILE_NAME = "/opt/Bose/etc/BlastConfiguration.json";

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

      m_ProductController( ProductController )
{
    InitializeQuickSetService( );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   ProductKeyInputInterface::InitializeQSS
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
/// @return This method returns a false value to allow further processing of the key event in the
///         base ProductKeyInputManager class.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CustomProductKeyInputManager::CustomProcessKeyEvent( const LpmServiceMessages::IpcKeyInformation_t&
                                                          keyEvent )
{
    ///
    /// Decide if this key should be blasted or sent to the key handler
    ///
    bool isBlastedKey = false;
    auto nowSelection = m_ProductController.GetNowSelection( );
    std::string cicode;

    ///
    /// @todo The following check to determine the device type based on the source needs to be
    ///       implemented.
    ///
#if 0

    if( nowSelection.has_contentitem( ) )
    {
        const auto& contentItem = nowSelection.contentitem( );
        auto source = m_ProductController.GetSourceInfo( )->FindSource( contentItem );

        if( source and source->has_details( ) )
        {
            if( source->details( ).has_devicetype( ) )
            {
                isBlastedKey = m_QSSClient->IsBlastedKey( keyEvent.keyid( ), source->details( ).devicetype( ) );
            }

            cicode = source->details( ).cicode( );
        }
    }

#endif

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
///                           End of the Product Application Namespace                           ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
