////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file      ProfessorProductController.h
///
/// @brief     This header file declares a ProfessorProductController class that acts as a container
///            to handle all the main functionality related to this program. A single instance of
///            this class is created in the main function for the Product Controller, where the Run
///            method for its instance is called to start and run this program.
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
#include "IntentHandler.h"
#include "ProductCecHelper.h"
#include "ProductDspHelper.h"
#include "ProductController.h"
#include "ProductSTSController.h"
#include "FrontDoorClientIF.h"
#include "ProductMessage.pb.h"
#include "SoundTouchInterface/PlayerService.pb.h"
#include "MacAddressInfo.h"
#include "BoseVersion.h"
#include "LightBarController.h"
#include "SystemPowerProduct.pb.h"

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
class ProductSystemManager;
class CustomProductAudioService;
class ProductCecHelper;
class ProductCommandLine;
class CustomProductKeyInputManager;
class ProductAdaptIQManager;
class ProductSourceInfo;
class ProductBLERemoteManager;

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @class ProfessorProductController
///
/// @brief This class acts as a container to handle all the main functionality related to this
///        program, including controlling the product states, as well as to instantiating subclasses
///        to manage the device and lower level hardware, and to interface with the user and higher
///        level applications.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
class ProfessorProductController : public ProductController
{
public:

    ////////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief  Constructor for the ProfessorProductController Class
    ///
    ////////////////////////////////////////////////////////////////////////////////////////////////
    ProfessorProductController( );

    ////////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief  The following public methods are used to start the ProfessorProductController
    ///         instance task, wait in a separate task until the product task ends, and stop the
    ///         product task, respectively.
    ///ProductLpmHardwareInterface
    ////////////////////////////////////////////////////////////////////////////////////////////////
    void Run( );
    void Wait( );
    void End( );

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief The following method is used to get the product controller callback function to which
    ///         product message events (based on ProductMessage Protocol Buffers) can be sent.
    ///
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    Callback < ProductMessage > GetMessageHandler( ) override;

    ////////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief The following method is used to get a shared pointer to the LPM hardware interface
    ///        instance from the product controller.
    ///
    ////////////////////////////////////////////////////////////////////////////////////////////////
    std::shared_ptr< CustomProductLpmHardwareInterface >& GetLpmHardwareInterface( ) override;

    ////////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief The following method is used to get a shared pointer to the Product AudioService
    ///        instance from the product controller.
    ///
    ////////////////////////////////////////////////////////////////////////////////////////////////
    std::shared_ptr< CustomProductAudioService >& GetProductAudioServiceInstance( ) override;

    ////////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief The following method is used to get a shared pointer to the AdaptIQ instance
    ///        from the product controller.
    ///
    ////////////////////////////////////////////////////////////////////////////////////////////////
    std::shared_ptr< ProductAdaptIQManager >& GetAdaptIQManager( );

    ////////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief The following method is used to get a shared pointer to the SourceInfo instance
    ///        from the product controller.
    ///
    ////////////////////////////////////////////////////////////////////////////////////////////////
    std::shared_ptr< ProductSourceInfo >& GetSourceInfo( );

    ////////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief The following method is used to get a shared pointer to the BLERemoteMmanager instance
    ///        from the product controller.
    ///
    ////////////////////////////////////////////////////////////////////////////////////////////////
    std::shared_ptr< ProductBLERemoteManager>& GetBLERemoteManager( );

    ////////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief The following method is used to get a shared pointer reference to the Edid
    ///        instance from the product controller.
    ///
    //////////////////////////////////////////////////////////////////////////////////////////////
    std::shared_ptr< ProductCecHelper >& GetCecHelper( );

    ////////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief The following method is used to get a shared pointer reference to the DspHelper
    ///        instance from the product controller.
    ///
    //////////////////////////////////////////////////////////////////////////////////////////////
    std::shared_ptr< ProductDspHelper >& GetDspHelper( );

    ////////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief The following method is used to get a reference to the intent handler instance for
    ///        processing actions from the product controller.
    ///
    //////////////////////////////////////////////////////////////////////////////////////////////
    IntentHandler& GetIntentHandler( ) override
    {
        return m_IntentHandler;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief The following methods are used by the state machine to determine the status of the
    ///        product controller.
    ///
    ////////////////////////////////////////////////////////////////////////////////////////////////
    bool  IsBooted( )          const override;
    bool  IsLowPowerExited()   const override;
    bool  IsAutoWakeEnabled( ) const override;

    ////////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief The following declaration is used to get the currently selected source.
    ///
    ////////////////////////////////////////////////////////////////////////////////////////////////
    PlaybackSource_t GetCurrentSource( );

    ////////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief The following declaration is used to get the the last Sound Touch playback. It is
    ///        currently is a kludge, until the common code supports persistent storage of this data.
    ///
    ////////////////////////////////////////////////////////////////////////////////////////////////
    SoundTouchInterface::PlaybackRequest& GetLastSoundTouchPlayback( );

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @name   GetWiFiOperationalMode
    ///
    /// @return NetManager::Protobuf::OperationalMode of the WiFi subsystem
    ///
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    NetManager::Protobuf::OperationalMode GetWiFiOperationalMode( ) override;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @name   InitializeKeyIdToKeyNameMap
    //  @brief  This function is needed to create a mapping of keys to keyNames
    ///
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    void InitializeKeyIdToKeyNameMap() override;
    ////////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief The following method is called to handle product controller messages, which are
    ///        sent from the more product specific class instances, and is used to process the
    ///        state machine for the product.
    ///
    ////////////////////////////////////////////////////////////////////////////////////////////////
    void HandleMessage( const ProductMessage& message );

    std::string GetDefaultProductName() const override;

    std::string GetProductVersionNumber() const override
    {
        return ( VERSION_STRING_SHORT + std::string( "-" ) + VERSION_BUILD_ABBREV_COMMIT );
    }

    std::vector<std::string> GetUniqueLanguages() const override
    {
        return {};
    }

    bool IsSystemLanguageSet( ) const;

    void SendInitialCapsData() override;

    std::unique_ptr<LightBar::LightBarController> m_lightbarController;

    PassportPB::ContentItem GetOOBDefaultLastContentItem() const override;

    bool CanPersistAsLastContentItem( const SoundTouchInterface::ContentItem &ci ) const override;

    void PossiblyPairBLERemote( );

    void PairBLERemote( uint32_t timeout );

    void StopPairingBLERemote( );

private:

    ////////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief The following subclasses declarations are used to manage the lower level hardware and
    ///        the device, as well as to interface with the user and higher level system
    ///        applications and command line.
    ///
    ////////////////////////////////////////////////////////////////////////////////////////////////
    std::shared_ptr< CustomProductLpmHardwareInterface > m_ProductLpmHardwareInterface;
    std::shared_ptr< ProductSystemManager              > m_ProductSystemManager;
    std::shared_ptr< ProductCommandLine                > m_ProductCommandLine;
    std::shared_ptr< ProductSourceInfo                 > m_ProductSourceInfo;
    std::shared_ptr< CustomProductKeyInputManager      > m_ProductKeyInputManager;
    std::shared_ptr< ProductCecHelper                  > m_ProductCecHelper;
    std::shared_ptr< ProductDspHelper                  > m_ProductDspHelper;
    std::shared_ptr< ProductAdaptIQManager             > m_ProductAdaptIQManager;
    std::shared_ptr< CustomProductAudioService         > m_ProductAudioService;
    std::shared_ptr< ProductBLERemoteManager           > m_ProductBLERemoteManager;

    ////////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief The following member variables are used to determined whether certain required
    ///        processes or connections are ready for setting the various Product Controller state
    ///        machine states.
    ///
    ////////////////////////////////////////////////////////////////////////////////////////////////
    bool m_IsAudioPathReady;
    bool m_IsAutoWakeEnabled;
    bool m_Running;

    ////////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief The following member variable is used to cache the radio status as to not spam
    ///        the LPM
    ///
    ////////////////////////////////////////////////////////////////////////////////////////////////
    IpcRadioStatus_t m_radioStatus;

    ////////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief The following declarations are used as interfaces to the ProductSTSController,
    ///        which implements the interactions between the Professor Product Controller and the
    ///        STS source proxies.
    ///
    ////////////////////////////////////////////////////////////////////////////////////////////////
    ProductSTSController m_ProductSTSController;

    void SetupProductSTSConntroller( );
    void HandleSelectSourceSlot( ProductSTSAccount::ProductSourceSlot sourceSlot );

    ////////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief The following method is used to register for end-points used in the inherited common
    ///        ProductController class, as well as for end-points registered for in this class.
    ///
    ////////////////////////////////////////////////////////////////////////////////////////////////
    void RegisterFrontDoorEndPoints( );

    ////////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief The following declarations is used for internal source selection and playback.
    ///
    ////////////////////////////////////////////////////////////////////////////////////////////////
    void  RegisterNowPlayingEndPoint( );
    void  HandleNowPlaying( const SoundTouchInterface::NowPlaying& nowPlayingStatus );

    ////////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief The following declarations are for handling the /system/power/mode/opticalAutoWake
    ///        frontdoor endpoint.
    ///
    ////////////////////////////////////////////////////////////////////////////////////////////////
    void HandleGetOpticalAutoWake( const Callback<SystemPowerProductPb::SystemPowerModeOpticalAutoWake> & respCb,
                                   const Callback<FrontDoor::Error> & errorCb ) const;
    void HandlePutOpticalAutoWake(
        const SystemPowerProductPb::SystemPowerModeOpticalAutoWake & req,
        const Callback<SystemPowerProductPb::SystemPowerModeOpticalAutoWake> & respCb,
        const Callback<FrontDoor::Error> & errorCb );
    void ApplyOpticalAutoWakeSettingFromPersistence( );
    void NotifyFrontdoorAndStoreOpticalAutoWakeSetting( );

    ////////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief The following declarations are for overriding the chime file locations so we can support
    ///        Professor and Ginger-Cheevers out of a single file system.
    ///
    ////////////////////////////////////////////////////////////////////////////////////////////////
    std::string GetChimesConfigurationLocation( ) const override;
    std::string GetChimesFilesLocation( ) const override;

    ////////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief The following declaration is used for intent management based on actions.
    ///
    ////////////////////////////////////////////////////////////////////////////////////////////////
    IntentHandler m_IntentHandler;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief The following member is used to store the product controller callback function to which
    ///         product message events (based on ProductMessage Protocol Buffers) can be sent.
    ///
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    Callback < ProductMessage > m_ProductMessageHandler;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
///                           End of the Product Application Namespace                           ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
