////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file      ProductCommandLine.h
///
/// @brief     This header file declares a ProductCommandLine class that is used to set up a command
///            line interface.
///
/// @author    Stuart J. Lumby
///
/// @attention Copyright (C) 2017 Bose Corporation All Rights Reserved
///
///            Bose Corporation
///            The Mountain Road,
///            Framingham, MA 01701-9168
///            U.S.A.
///
///            This program may not be reproduced, in whole or in part, in any form by any means
///            whatsoever without the written permission of Bose Corporation.
///
////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// The following compiler directive prevents this header file from being included more than once,
/// which may cause multiple declaration compiler errors.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////
///
///            Included Header Files
///
////////////////////////////////////////////////////////////////////////////////////////////////////
#include "Utilities.h"
#include "ProductMessage.pb.h"
#include "CLICmdsKeys.h"

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
class CustomProductLpmHardwareInterface;
class CustomProductController;

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @class ProductCommandLine
///
/// @brief This class is used to set up a command line interface through a product controller
///        class instance.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
class ProductCommandLine
{

public:

    //////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @name   ProductCommandLine Constructor
    ///
    /// @param  CustomProductController& ProductController
    ///
    //////////////////////////////////////////////////////////////////////////////////////////////
    ProductCommandLine( CustomProductController& ProductController );

    //////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief  The following public methods are used to run and stop instances of the
    ///         ProductCommandLine class, respectively.
    ///
    //////////////////////////////////////////////////////////////////////////////////////////////
    void Run( );
    void Stop( );

private:


    //////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief The following declarations are used to interface with the product controller and
    ///        the lower level LPM hardware.
    ///
    //////////////////////////////////////////////////////////////////////////////////////////////
    CustomProductController&                             m_ProductController;
    NotifyTargetTaskIF*                                  m_ProductTask;
    std::shared_ptr< CustomProductLpmHardwareInterface > m_ProductLpmHardwareInterface;

    //////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief The following declarations and methods are used to create a command line interface,
    ///        as well as assemble product controller related commands into a list for the
    ///        interface.
    ///
    //////////////////////////////////////////////////////////////////////////////////////////////
    void RegisterCliCmds();

    void HandleCliCmd( uint16_t cmdKey,
                       const std::list<std::string> & argList,
                       AsyncCallback<std::string, int32_t> rspAndRspCmplt,
                       int32_t transact_id );

    void HandleAutowake( const std::list<std::string> & argList,
                         std::string& response );

    void HandleIntent( const std::list<std::string> & argList,
                       std::string& response );

    void HandleMute( const std::list<std::string> & argList,
                     std::string& response );

    void HandleSource( const std::list<std::string> & argList,
                       std::string& response );

    void HandleBootStatus( const std::list<std::string> & argList,
                           std::string& response );

    void HandleTestFreq( const std::list<std::string> & argList,
                         std::string& response );

    void HandleTestPairing( const std::list<std::string> & argList,
                            std::string& response );

    void HandleTestPower( const std::list<std::string> & argList,
                          std::string& response );

    void HandleTestVoice( const std::list<std::string> & argList,
                          std::string& response );

    void HandleVolume( const std::list<std::string> & argList,
                       std::string& response );

    void HandleTestAccessoryUpdate( const std::list<std::string> & argList,
                                    std::string& response );

    void HandleTestTransition( const std::list<std::string> & argList,
                               std::string& response );

};

////////////////////////////////////////////////////////////////////////////////////////////////////
///                           End of the Product Application Namespace                           ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
