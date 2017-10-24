////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file      ProductSystemManager.h
///
/// @brief     This header file contains declarations for sending and receiving system information
///            through a Front Door network router process.
///
/// @author    Stuart J. Lumby
///
/// @date      09/22/2017
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
/// The following compiler directive prevents this header file from being included more than
/// once, which will cause multiple declaration compiler errors, in any file that includes it.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////
///
///            Included Header Files
///
////////////////////////////////////////////////////////////////////////////////////////////////////
#include "NotifyTargetTaskIF.h"
#include "ConfigurationStatus.pb.h"
#include "Language.pb.h"
#include "ProductMessage.pb.h"
#include "CapsInitializationStatus.pb.h"
#include "NetworkService.pb.h"
#include "STSNetworkStatus.pb.h"
#include "NetManager.pb.h"
#include "BreakThread.h"
#include "APTaskFactory.h"
#include "FrontDoorClientIF.h"
#include "ProtoPersistenceIF.h"
#include "ProtoPersistenceFactory.h"
#include "ProductMessage.pb.h"
#include "Language.pb.h"
#include "ConfigurationStatus.pb.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
///                             Start of Product Namespace                                       ///
////////////////////////////////////////////////////////////////////////////////////////////////////
namespace ProductApp
{

////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @class ProductSystemManager
///
/// @brief This class provides functionality for sending and receiving information through a
///        Front Door network router process, which handles connections and communications
///        between various Bose processes.
///
////////////////////////////////////////////////////////////////////////////////////////////////
class ProductSystemManager
{
public:

    //////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @name   ProductSystemManager::GetInstance
    ///
    /// @brief  This static method creates the one and only instance of the object
    ///         ProductSystemManager. That only one instance is created in a thread safe
    ///         way is guaranteed by the C++ Version 11 compiler.
    ///
    /// @param  void This method does not take any arguments.
    ///
    /// @return This method returns a pointer to a singleton ProductSystemManager object.
    ///
    //////////////////////////////////////////////////////////////////////////////////////////
    static ProductSystemManager* GetInstance( NotifyTargetTaskIF*        mainTask,
                                              Callback< ProductMessage > ProductNotify );

    //////////////////////////////////////////////////////////////////////////////////////////
    /// This declaration is used to run and stop an instance of the System Manager.
    //////////////////////////////////////////////////////////////////////////////////////////
    bool Run( void );
    void Stop( void );

    //////////////////////////////////////////////////////////////////////////////////////////
    /// These public declarations are primarily to be used by the Product Controller to
    /// determine if the system language has been set and to set the configuration status.
    //////////////////////////////////////////////////////////////////////////////////////////
    bool IsSystemLanguageSet( void );
    void SetConfigurationStatus( bool network, bool account, bool language );

private:

    //////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @name   ProductSystemManager
    ///
    /// @brief  The constructor for this class is set to be private. This definition prevents
    ///         this class from being instantiated directly, so that only the static method
    ///         GetInstance to this class can be used to get the one sole instance of it.
    ///
    /// @param mainTask This argument points to a task to process resource requests and
    ///                 notifications.
    ///
    /// @param ProductNotify This is a callback to send events to the Product Controller.
    ///
    //////////////////////////////////////////////////////////////////////////////////////////
    ProductSystemManager( NotifyTargetTaskIF*        mainTask,
                          Callback< ProductMessage > ProductNotify );

    //////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// The following copy constructor and equality operator for this class are private and
    /// are set to be undefined through the delete keyword. This prevents this class from
    /// being copied directly, so that only the static method GetInstance to this class can be
    /// used to get the one sole instance of it.
    ///
    //////////////////////////////////////////////////////////////////////////////////////////
    ProductSystemManager( ProductSystemManager const& ) = delete;
    ProductSystemManager operator = ( ProductSystemManager const& ) = delete;

    //////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// This declaration stores the main task for processing events and requests. It
    /// is inherited by the ProductSystemManager instance.
    ///
    //////////////////////////////////////////////////////////////////////////////////////////
    NotifyTargetTaskIF*                m_mainTask;
    Callback< ProductMessage >         m_ProductNotify;
    std::shared_ptr<FrontDoorClientIF> m_FrontDoorClient;

    //////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief The following member variables and methods are used to store the language settings
    ///        and the configuration status.
    ///
    //////////////////////////////////////////////////////////////////////////////////////////////
    ProductPb::Language                     m_LanguageSettings;
    ProtoPersistenceIF::ProtoPersistencePtr m_LanguageSettingsPersistentStorage;
    ProductPb::ConfigurationStatus          m_ConfigurationStatus;
    ProtoPersistenceIF::ProtoPersistencePtr m_ConfigurationStatusPersistentStorage;

    void ReadLanguageSettingsFromPersistentStorage( void );
    void ReadConfigurationStatusFromPersistentStorage( void );
    void WriteLanguageSettingsToPersistentStorage( void );
    void WriteConfigurationStatusToPersistentStorage( void );

    //////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// The following methods are used to handle the CAPS Content Audio Playback Service
    /// status.
    ///
    //////////////////////////////////////////////////////////////////////////////////////////
    void GetCapsNotification( const SoundTouchInterface::CapsInitializationStatus& status );
    void GetCapsStatus( const SoundTouchInterface::CapsInitializationStatus& status );
    void GetCapsStatusFailed( const FRONT_DOOR_CLIENT_ERRORS error );

    //////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// The following methods are used to handle system and supported language information.
    ///
    //////////////////////////////////////////////////////////////////////////////////////////
    void HandleGetLanguageRequest( const Callback< ProductPb::Language >&  response );
    void HandlePostLanguageRequest( const ProductPb::Language&              language,
                                    const Callback< ProductPb::Language >&  response );

    //////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// The following method is used to handle the configuration status.
    ///
    //////////////////////////////////////////////////////////////////////////////////////////
    void HandleGetConfigurationStatusRequest( const Callback< ProductPb::ConfigurationStatus >&
                                              response );

    //////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// The following method is used to send information to the Product Controller state
    /// machine.
    ///
    //////////////////////////////////////////////////////////////////////////////////////////
    void SendMessage( ProductMessage& message );
};

////////////////////////////////////////////////////////////////////////////////////////////////////
///                               End of ProductApp Namespace                                    ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
