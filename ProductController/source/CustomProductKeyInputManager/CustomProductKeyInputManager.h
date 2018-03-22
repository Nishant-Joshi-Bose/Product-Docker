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
class ProfessorProductController;

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
    CustomProductKeyInputManager( ProfessorProductController& ProductController );


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
    ProfessorProductController& m_ProductController;

    ///
    /// Custom A4V Quick Set Service Client and Initialization
    ///
    A4VQuickSetService::A4VQuickSetServiceClientIF::A4VQuickSetServiceClientPtr m_QSSClient;

    void InitializeQuickSetService( );
};

////////////////////////////////////////////////////////////////////////////////////////////////////
///                           End of the Product Application Namespace                           ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
