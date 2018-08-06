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
                                           const FrontDoorClientIF_t& frontDoorClient );

    void ExecutePowerMacro( const ProductPb::PowerMacro& pwrMacro );

    explicit CustomProductKeyInputManager( CustomProductController& ProductController );


    ///
    /// CustomProductKeyInputManager Deconstructor
    ///
    ~CustomProductKeyInputManager( ) override
    {

    }

protected:

    ///
    /// CustomProcessKeyEvent Override
    ///
    bool CustomProcessKeyEvent( const LpmServiceMessages::IpcKeyInformation_t& keyEvent ) override;

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
    bool FilterIncompleteChord( const LpmServiceMessages::IpcKeyInformation_t& keyEvent );

    void BlastKey( const IpcKeyInformation_t&  keyEvent, const std::string& cicode );
};

////////////////////////////////////////////////////////////////////////////////////////////////////
///                           End of the Product Application Namespace                           ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
