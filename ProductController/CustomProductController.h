////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file      CustomProductController.h
///
/// @brief     This header file declares a CustomProductController class that acts as a container
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
#include "APTimer.h"
#include "IntentHandler.h"
#include "ProductCecHelper.h"
#include "ProductDspHelper.h"
#include "ProductController.h"
#include "ProductSTSController.h"
#include "FrontDoorClientIF.h"
#include "ProductMessage.pb.h"
#include "SoundTouchInterface/PlayerService.pb.h"
#include "SoundTouchInterface/AudioService.pb.h"
#include "MacAddressInfo.h"
#include "LightBarController.h"
#include "SystemPowerProduct.pb.h"
#include "DisplayController.pb.h"
#include "SystemPower.pb.h"
#include "SystemPowerMacro.pb.h"
#include "ProductFrontDoorKeyInjectIF.h"
#include "AccessorySoftwareInstallManager.h"
#include "MonotonicClock.h"
#include "DeviceControllerClientIF.h"

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
class CustomProductAudioService;
class ProductCecHelper;
class ProductCommandLine;
class CommonProductCommandLine;
class CustomProductKeyInputManager;
class ProductAdaptIQManager;
class ProductBLERemoteManager;

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @class CustomProductController
///
/// @brief This class acts as a container to handle all the main functionality related to this
///        program, including controlling the product states, as well as to instantiating module
///        classes to manage the device and lower level hardware, and to interface with the user and
///        higher level applications.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
class CustomProductController : public ProductController
{
    friend class ProductCommandLine;
    friend class CommonProductCommandLine;

public:

    ////////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief  Constructor for the CustomProductController Class
    ///
    ////////////////////////////////////////////////////////////////////////////////////////////////
    CustomProductController( );

    ////////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief  The following public methods are used to start and run the Custom product
    ///         controller in a single-threaded task, wait in a separate task until the product task
    ///         ends, and end product controller processing, respectively.
    ///
    ////////////////////////////////////////////////////////////////////////////////////////////////
    void Start( );
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

    const DeviceController::DeviceControllerClientIF::DeviceControllerClientPtr& GetDeviceControllerClient() const
    {
        return m_deviceControllerPtr;
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
    NetManager::Protobuf::OperationalMode GetWiFiOperationalMode( ) const override;

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

    std::vector<std::string> GetUniqueLanguages() const override
    {
        return {};
    }

    bool IsSystemLanguageSet( ) const;

    void SendSystemSourcesPropertiesToCAPS();
    void SendInitialCapsData() override;

    std::unique_ptr<LightBar::LightBarController> m_lightbarController;

    APTimerPtr m_uiAliveTimer;

    PassportPB::contentItem GetOOBDefaultLastContentItem() const override;

    void PossiblyPairBLERemote( );

    void PairBLERemote( uint32_t timeout );

    void StopPairingBLERemote( );

    bool IsBLERemoteConnected( ) const;

    bool AreAccessoriesKnown( ) const
    {
        return m_AccessoriesAreKnown;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief This method is used to bring up and down the ethernet interface for network standby
    ///
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    void SetEthernetEnabled( bool enabled );

    ////////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief The following method is called to fetch the desired volume level for audio playback
    ///        when entering the PLAYING state
    ///
    ////////////////////////////////////////////////////////////////////////////////////////////////
    std::pair<bool, int32_t> GetDesiredPlayingVolume( ) const;

    ///////////////////////////////////////////////////////////////////////////////
    /// @name  AttemptToStartPlayback
    /// @brief This function attempts to start playback previously played content item
    ///        if not able to, go to SETUP
    ///////////////////////////////////////////////////////////////////////////////
    void AttemptToStartPlayback( ) override;

    //////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief Interfaces to the software updates components. Implement virtual functions
    ///        to facilitate accessory update
    ///
    //////////////////////////////////////////////////////////////////////////////////////////////
    virtual void InitiateSoftwareInstall( ) override;

    virtual bool IsSwUpdateForeground( ) const override;

    void SetSpeakerPairingIsFromLAN( bool isFromLan )
    {
        m_speakerPairingIsFromLAN = isFromLan;
    }

    bool GetSpeakerPairingIsFromLAN( ) const
    {
        return m_speakerPairingIsFromLAN;
    }

    void PowerMacroOff();

    bool GetHaltInPlayableTransitionNetworkStandby( ) const
    {
        return m_haltInPlayableTransitionNetworkStandby;
    }

    void SetBootCompleteTime( )
    {
        m_bootCompleteTime = MonotonicClock::NowMs( );
    }

    int64_t GetBootCompleteTime( ) const
    {
        return m_bootCompleteTime;
    }

private:

    ////////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief  The following private method is used to run the Custom product controller. It is
    ///         dispatched from the public product controller Start method, which ensures that it
    ///         runs in the single-threaded product task.
    ///
    ////////////////////////////////////////////////////////////////////////////////////////////////
    void Run( );

    ////////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief The following subclasses declarations are used to manage the lower level hardware and
    ///        the device, as well as to interface with the user and higher level system
    ///        applications and command line.
    ///
    ////////////////////////////////////////////////////////////////////////////////////////////////
    std::shared_ptr< CustomProductLpmHardwareInterface > m_ProductLpmHardwareInterface;
    std::shared_ptr< ProductCommandLine                > m_ProductCommandLine;
    std::shared_ptr< CommonProductCommandLine          > m_CommonProductCommandLine;
    std::shared_ptr< CustomProductKeyInputManager      > m_ProductKeyInputManager;
    std::shared_ptr< ProductFrontDoorKeyInjectIF       > m_ProductFrontDoorKeyInjectIF;
    std::shared_ptr< ProductCecHelper                  > m_ProductCecHelper;
    std::shared_ptr< ProductDspHelper                  > m_ProductDspHelper;
    std::shared_ptr< ProductAdaptIQManager             > m_ProductAdaptIQManager;
    std::shared_ptr< CustomProductAudioService         > m_ProductAudioService;
    std::shared_ptr< ProductBLERemoteManager           > m_ProductBLERemoteManager;

    ////////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief The following subclasses declarations are used to manage the lower level hardware and
    ///        the device, as well as to interface with the user and higher level system
    ///        applications and command line.
    ///
    ////////////////////////////////////////////////////////////////////////////////////////////////
    DeviceController::DeviceControllerClientIF::DeviceControllerClientPtr       m_deviceControllerPtr;

    ////////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief The following member variables are used to determined whether certain required
    ///        processes or connections are ready for setting the various Product Controller state
    ///        machine states.
    ///
    ////////////////////////////////////////////////////////////////////////////////////////////////
    bool m_IsAutoWakeEnabled;
    bool m_Running;

    NetManager::Protobuf::OperationalMode m_networkOperationalMode;

    bool m_isNetworkWired;

    ////////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief The following member variable is used to cache the radio status as to not spam
    ///        the LPM
    ///
    ////////////////////////////////////////////////////////////////////////////////////////////////
    IpcRadioStatus_t m_radioStatus;

    ////////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief The following member variable is used to cache whether ethernet is enabled or disabled
    ///
    ////////////////////////////////////////////////////////////////////////////////////////////////
    bool             m_ethernetEnabled;

    ////////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief The following member variable is used to track whether the accessory state is known
    ///
    ////////////////////////////////////////////////////////////////////////////////////////////////
    bool m_AccessoriesAreKnown = false;

    ////////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief The following declarations are used as interfaces to the ProductSTSController,
    ///        which implements the interactions between the Custom Product Controller and the
    ///        STS source proxies.
    ///
    ////////////////////////////////////////////////////////////////////////////////////////////////
    ProductSTSController m_ProductSTSController;

    void SetupProductSTSController( ) override;
    void HandleSelectSourceSlot( ProductSTSAccount::ProductSourceSlot sourceSlot );

    ////////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief The following method is used to register for end-points used in the inherited common
    ///        ProductController class, as well as for end-points registered for in this class.
    ///
    ////////////////////////////////////////////////////////////////////////////////////////////////
    void RegisterFrontDoorEndPoints( );



    void HandleGetTimeouts( Callback<SystemPowerPb::SystemPowerTimeouts> respCb,
                            Callback<FrontDoor::Error> errorCb );
    void HandlePutTimeouts( SystemPowerPb::SystemPowerTimeouts req,
                            Callback<SystemPowerPb::SystemPowerTimeouts> respCb,
                            Callback<FrontDoor::Error> errorCb );

    ////////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief The following declaration is for handling the /ui/alive frontdoor endpoint
    ///
    ////////////////////////////////////////////////////////////////////////////////////////////////
    void HandleUiHeartBeat(
        const DisplayControllerPb::UiHeartBeat & req,
        const Callback<DisplayControllerPb::UiHeartBeat> & respCb,
        const Callback<FrontDoor::Error> & errorCb );

    ////////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief The following declaration is for killing the Brussels process if no /ui/alive messages
    ///        received by ProductController.
    ///
    ////////////////////////////////////////////////////////////////////////////////////////////////
    void KillUiProcess();

    ////////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief The following declaration is for initializing the UI recovery timer
    ///
    ////////////////////////////////////////////////////////////////////////////////////////////////
    void StartUiTimer();

    ////////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief The following declaration is for handling the /audio/volume frontdoor endpoint
    ///
    ////////////////////////////////////////////////////////////////////////////////////////////////
    void HandleAudioVolumeNotification( SoundTouchInterface::volume volume );

    ////////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief The following declaration is used for now playing information handling
    ///
    ////////////////////////////////////////////////////////////////////////////////////////////////
    void  HandleCapsNowPlaying( SoundTouchInterface::NowPlaying nowPlayingStatus ) override;

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
    void HandlePutOpticalAutoWake( const SystemPowerProductPb::SystemPowerModeOpticalAutoWake & req,
                                   const Callback<SystemPowerProductPb::SystemPowerModeOpticalAutoWake> & respCb,
                                   const Callback<FrontDoor::Error> & errorCb );
    void ApplyOpticalAutoWakeSettingFromPersistence( );
    void NotifyFrontdoorAndStoreOpticalAutoWakeSetting( );
    void RegisterOpticalAutowakeForLpmConnection( );

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
    /// @brief The following declarations are used to handle product-specific chimes associated with accessory pairing.
    ///
    ////////////////////////////////////////////////////////////////////////////////////////////////
    void HandleChimeResponse( ChimesControllerPb::ChimesStatus status ) override;
    bool HandleAccessoriesPlayTonesResponse( ChimesControllerPb::ChimesStatus status );
    void AccessoriesPlayTonesPutHandler( ProductPb::AccessoriesPlayTonesRequest req,
                                         Callback<ProductPb::AccessoriesPlayTonesRequest> resp,
                                         Callback<FrontDoor::Error> error );
    void AccessoriesPlayTones( bool subs, bool rears );
    bool m_queueRearAccessoryTone = false;
    bool m_speakerPairingIsFromLAN = false;

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

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief The following member manages the installation of accessory software
    ///
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    AccessorySoftwareInstallManager m_AccessorySoftwareInstallManager;
    void InitializeAccessorySoftwareInstallManager( );

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief The following member determines whether the given key origin belongs to a product
    ///        control surface.
    ///
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    bool IsProductControlSurface( LpmServiceMessages::KeyOrigin_t keyOrigin ) const override;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief The following member is used to store the /audio/volume data sent from CAPS
    ///
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    SoundTouchInterface::volume m_cachedVolume;


    ////////////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief The following member is used to store the /system/power/macro settings
    ///
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    ProductPb::PowerMacro m_powerMacro;
    void HandlePutPowerMacro( const ProductPb::PowerMacro & req,
                              const Callback<ProductPb::PowerMacro> & respCb,
                              const Callback<FrontDoor::Error> & errorCb );
    void HandleGetPowerMacro( const Callback<ProductPb::PowerMacro> & respCb,
                              const Callback<FrontDoor::Error> & errorCb ) const;
    void UpdatePowerMacro( );
    void PersistPowerMacro( );
    void SendPowerMacroToDataCollection( );
    void ReconcileCurrentProductSource( );
    void LoadPowerMacroFromPersistance( );

    void HandleVoiceStatus( VoiceServicePB::VoiceStatus voiceStatus ) override;

    bool IsAllModuleReady() const;

    bool m_haltInPlayableTransitionNetworkStandby = false;

    int64_t m_bootCompleteTime              = 0;

    VoiceServicePB::VoiceStatus m_voiceStatus = VoiceServicePB::VoiceStatus::UNKNOWN;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
///                           End of the Product Application Namespace                           ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
