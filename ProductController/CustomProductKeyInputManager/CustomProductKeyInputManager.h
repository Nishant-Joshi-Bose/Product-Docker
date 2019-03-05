////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file      CustomProductKeyInputManager.h
///
/// @brief     This header file declares a CustomProductKeyInputManager class that inherits the
///            base ProductKeyInputManager class to manage receiving and processing of raw key input
///            from the LPM hardware.
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
#pragma once
#include "ProductKeyInputManager.h"
#include "DeviceControllerClientFactory.h"
#include "SystemPowerMacro.pb.h"
#include "FrontDoorClient.h"
#include "KeyFilter.pb.h"
#include <regex>

////////////////////////////////////////////////////////////////////////////////////////////////////
///                          Start of the Product Application Namespace                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
namespace ProductApp
{

////////////////////////////////////////////////////////////////////////////////////////////////////
///
///            Forward Class Declarations
///
////////////////////////////////////////////////////////////////////////////////////////////////////
class CustomProductController;

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @class CustomProductKeyInputManager
///
////////////////////////////////////////////////////////////////////////////////////////////////////
class CustomProductKeyInputManager : public ProductKeyInputManager
{
public:

    ///
    /// CustomProductKeyInputManager Constructor
    ///
    explicit CustomProductKeyInputManager( CustomProductController& ProductController );

    void ExecutePowerMacro( const ProductPb::PowerMacro& pwrMacro, LpmServiceMessages::KEY_VALUE key );


    ///
    /// CustomProductKeyInputManager Deconstructor
    ///
    ~CustomProductKeyInputManager( ) override
    {

    }

    bool FilterIntent( KeyHandlerUtil::ActionType_t& ) const;
    static const std::string& IntentName( KeyHandlerUtil::ActionType_t intent );

protected:

    ///
    /// CustomProcessKeyEvent Override
    ///
    bool CustomProcessKeyEvent( const IpcKeyInformation_t& keyEvent ) override;

private:

    ///
    /// Product Controller Reference
    ///
    CustomProductController& m_ProductController;

    ///
    /// Custom A4V-DeviceController Client and Initialization
    ///
    DeviceController::DeviceControllerClientIF::DeviceControllerClientPtr       m_deviceControllerPtr;

    ///
    /// Data used by FilterIncompleteChord() to track history
    ///
    int64_t m_TimeOfChordRelease;
    ::google::protobuf::uint32 m_KeyIdOfIncompleteChordRelease;

    bool FilterIncompleteChord( const IpcKeyInformation_t& keyEvent );

    bool IsSourceKey( const LpmServiceMessages::IpcKeyInformation_t& keyEvent );

    void BlastKey( const IpcKeyInformation_t&  keyEvent, const std::string& cicode );

    struct FilterRegex
    {
        FilterRegex( const std::string& source, const std::string& sourceAccount ) :
            m_sourceFilter( std::regex( source ) ),
            m_sourceAccountFilter( std::regex( sourceAccount ) )
        {
        }

        const std::regex    m_sourceFilter;
        const std::regex    m_sourceAccountFilter;
    };

    ///
    /// Filter subset of key table
    ///
    KeyFilter::KeyFilter                                                            m_filterTable;
    std::map< const KeyFilter::FilterEntry*, std::vector<FilterRegex> >             m_filterRegex;
    void InitializeKeyFilter( );
    std::map< unsigned, bool >                                                      m_lastPressStatus;
    bool AccommodateOrphanReleaseEvents( const IpcKeyInformation_t& keyEvent, bool pressRet );
};

////////////////////////////////////////////////////////////////////////////////////////////////////
///                           End of the Product Application Namespace                           ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
