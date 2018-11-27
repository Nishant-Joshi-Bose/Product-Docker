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
#include "A4VQuickSetServiceClientFactory.h"
#include "SystemPowerMacro.pb.h"
#include "FrontDoorClient.h"
#include "KeyFilter.pb.h"


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
    explicit CustomProductKeyInputManager( CustomProductController& ProductController,
                                           const A4VQuickSetService::A4VQuickSetServiceClientIF::A4VQuickSetServiceClientPtr& QSSClient );

    void ExecutePowerMacro( const ProductPb::PowerMacro& pwrMacro, LpmServiceMessages::KEY_VALUE key );


    ///
    /// CustomProductKeyInputManager Deconstructor
    ///
    ~CustomProductKeyInputManager( ) override
    {

    }

    bool IsIntentIgnored( KeyHandlerUtil::ActionType_t ) const;

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
    /// Custom A4V Quick Set Service Client and Initialization
    ///
    A4VQuickSetService::A4VQuickSetServiceClientIF::A4VQuickSetServiceClientPtr m_QSSClient;

    ///
    /// Data used by FilterIncompleteChord() to track history
    ///
    int64_t m_TimeOfChordRelease;
    ::google::protobuf::uint32 m_KeyIdOfIncompleteChordRelease;

    void InitializeQuickSetService( );
    bool FilterIncompleteChord( const IpcKeyInformation_t& keyEvent );

    void BlastKey( const IpcKeyInformation_t&  keyEvent, const std::string& cicode );

    ///
    /// Filter subset of key table
    ///
    KeyFilter::KeyFilter        m_filterTable;
    void InitializeKeyFilter( );
    bool KeyIgnoredInCurrentSource( const IpcKeyInformation_t& keyEvent, const SoundTouchInterface::Sources::SourceItem* sourceItem ) const;
    static const std::string& IntentName( KeyHandlerUtil::ActionType_t intent );
};

////////////////////////////////////////////////////////////////////////////////////////////////////
///                           End of the Product Application Namespace                           ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
