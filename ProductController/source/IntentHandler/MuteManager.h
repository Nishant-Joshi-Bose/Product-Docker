////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file      MuteManager.h
///
/// @brief     This header file declares an intent manager class for implementing volume and mute
///            control based on Professor product specific key actions
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
///////////////////////////////////////////////////////////////////////////////////////////////////
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
#include "FrontDoorClientIF.h"
#include "ProductMessage.pb.h"

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
/// @class MuteManager
///
/// @brief This class provides functionality to implement audio volume management.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
class MuteManager : public IntentManager
{
public:

    ////////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief Constructor for the MuteManager Class
    ///
    /// @param NotifyTargetTaskIF&         task
    ///
    /// @param const CliClientMT&          commandLineClient
    ///
    /// @param const FrontDoorClientIF_t&  frontDoorClient
    ///
    /// @param ProfessorProductController& productController
    ///
    ////////////////////////////////////////////////////////////////////////////////////////////////
    MuteManager( NotifyTargetTaskIF&        task,
                 const CliClientMT&         commandLineClient,
                 const FrontDoorClientIF_t& frontDoorClient,
                 ProductController&         productController );

    //////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief  The following methods is used to handle volume and mute actions.
    ///
    //////////////////////////////////////////////////////////////////////////////////////////////
    bool Handle( KeyHandlerUtil::ActionType_t& action ) override;

    //////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief  The following methods is used to stop instances of the MuteManager
    ///         class.
    ///
    //////////////////////////////////////////////////////////////////////////////////////////////
    void Stop( );

    //////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief The following methods allow for manipulation of system mute
    ///
    //////////////////////////////////////////////////////////////////////////////////////////////
    void ToggleMute( );

private:

    ////////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief The following member variable stores the custom Professor product controller instance.
    ///
    ////////////////////////////////////////////////////////////////////////////////////////////////
    ProfessorProductController& m_CustomProductController;

    //////////////////////////////////////////////////////////////////////////////////////////////
    /// These declarations store the main task for processing LPM hardware events and requests. It
    /// is passed by the ProductController instance.
    //////////////////////////////////////////////////////////////////////////////////////////////
    NotifyTargetTaskIF*        m_ProductTask    = nullptr;
    Callback< ProductMessage > m_ProductNotify  = nullptr;

    //////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief The following method provides for one-time initialization after the constructor
    ///        has completed
    ///
    //////////////////////////////////////////////////////////////////////////////////////////////
    void Initialize( );

    //////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief The following methods communicate with the FrontDoor to set and receive volume
    ///
    //////////////////////////////////////////////////////////////////////////////////////////////
    void UpdateFrontDoorVolume( int32_t volume );
    void ReceiveFrontDoorVolume( SoundTouchInterface::volume const& volume );

    std::shared_ptr< FrontDoorClientIF >    m_FrontDoorClient;
    CallbackConnection                      m_NotifierCallback;

    //////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief The following member tracks mute status
    ///
    //////////////////////////////////////////////////////////////////////////////////////////////
    bool m_muted = false;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
///                           End of the Product Application Namespace                           ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
