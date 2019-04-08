////////////////////////////////////////////////////////////////////////////////
/// @file   CustomProductController.h
/// @brief  Product controller class.
///
/// @attention Copyright 2017 Bose Corporation, Framingham, MA
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <locale>
#include <string>
#include <algorithm>
#include <iostream>

#include "ProductController.h"
#include "Clock.h"
#include "NotifyTargetTaskIF.h"
#include "ProtoPersistenceIF.h"
#include "ProductControllerStateTop.h"
#include "CustomProductControllerStateNetworkStandby.h"
#include "CustomProductControllerStateLowPowerResume.h"
#include "CustomProductControllerStateLowPowerStandby.h"
#include "CustomProductControllerStateLowPowerStandbyTransition.h"
#include "ProductControllerStateNetworkStandbyConfigured.h"
#include "ProductControllerStateNetworkStandbyNotConfigured.h"
#include "ProductControllerStateIdleVoiceConfigured.h"
#include "ProductControllerStateIdleVoiceNotConfigured.h"
#include "ProductControllerStatePlayable.h"
#include "ProductControllerStatePlaying.h"
#include "ProductControllerStateBooted.h"
#include "ProductControllerStateBooting.h"
#include "CustomProductControllerStateOn.h"
#include "ProductControllerStateOn.h"
#include "ProductControllerStateIdle.h"
#include "ProductControllerStateCriticalError.h"
#include "ProductControllerStateFactoryDefault.h"
#include "ProductControllerStatePlayingDeselected.h"
#include "ProductControllerStatePlayingSelected.h"
#include "ProductControllerStatePlayingSelectedSilent.h"
#include "ProductControllerStatePlayingSelectedSilentSourceInvalid.h"
#include "ProductControllerStatePlayingSelectedSilentSourceValid.h"
#include "ProductControllerStatePlayingSelectedNotSilent.h"
#include "ProductControllerStatePlayingSelectedSetup.h"
#include "ProductControllerStatePlayingSelectedSetupNetworkConfig.h"
#include "ProductControllerStatePlayingSelectedSetupNetworkConfigWiFiTransition.h"
#include "ProductControllerStatePlayingSelectedSetupNetworkConfigWiFiConnection.h"
#include "ProductControllerStatePlayingSelectedSetupOther.h"
#include "ProductControllerStatePlayingSelectedSetupExiting.h"
#include "ProductControllerStatePlayingSelectedSetupNetworkConfigWiFiExiting.h"
#include "ProductControllerStatePlayingSelectedSetupNetworkConfigWiFiAborting.h"
#include "ProductControllerStatePlayingSelectedStoppingStreams.h"
#include "ProductControllerStatePlayableTransition.h"
#include "ProductControllerStatePlayableTransitionIdle.h"
#include "ProductControllerStatePlayableTransitionInternal.h"
#include "ProductControllerStatePlayableTransitionNetworkStandby.h"
#include "ProductControllerStateSoftwareInstall.h"
#include "ProductControllerStateSoftwareInstallTransition.h"
#include "ProductControllerStatePlayingTransition.h"
#include "ProductControllerStateFirstBootGreeting.h"
#include "ProductControllerStateFirstBootGreetingTransition.h"
#include "ProductControllerStatePlayingTransitionSwitch.h"
#include "ProductControllerStateStoppingStreamsDedicated.h"
#include "ProductControllerStateStoppingStreamsDedicatedForFactoryDefault.h"
#include "ProductControllerStateStoppingStreamsDedicatedForSoftwareUpdate.h"
#include "LightBarController.h"
#include "SoundTouchInterface/AllowSourceSelect.pb.h"
#include "NetManager.pb.h"
#include "SoundTouchInterface/CapsInitializationStatus.pb.h"
#include "SoundTouchInterface/ContentSelectionService.pb.h"
#include "SoundTouchInterface/PlayerService.pb.h"
#include "KeyHandler.h"
#include "IntentHandler.h"
#include "ProductSTSController.h"
#include "DisplayController.h"
#include "MacAddressInfo.h"
#include "BOptional.h"
#include "ProductConfig.pb.h"
#include "ProductIotHandler.h"

namespace ProductApp
{

class CustomProductAudioService;
class CustomProductKeyInputManager;
class ProductCommandLine;
class CommonProductCommandLine;

class CustomProductController : public ProductController
{
    friend class ProductCommandLine;
    friend class CommonProductCommandLine;

public:
    CustomProductController();
    virtual ~CustomProductController();

    void Initialize();
    void InitializeKeyIdToKeyNameMap() override;

    Callback < ProductMessage > GetMessageHandler() override;

    std::vector<std::string> GetUniqueLanguages() const override
    {
        return {};
    }

    // The following methods are used by the state machine to determine the
    // status of the product controller.
    bool IsBooted() const override;
    bool IsLowPowerExited() const override;

    bool IsAutoWakeEnabled() const override
    {
        return false;
    }

    std::string GetDefaultProductName() const override;
    void RegisterAuxEvents( AsyncCallback<LpmServiceMessages::IpcAuxState_t> &cb );
    void RequestAuxCableState();

private:
    CustomProductController( const CustomProductController& ) = delete;
    CustomProductController& operator=( const CustomProductController& ) = delete;

private:

    void InitializeHsm();
    void InitializeAction();
    void ProductDependentInitialize();
    void LoadProductConfiguration( ProductPb::ProductConfig& );
    int FindThisProductConfig( ProductPb::ProductConfig& );
    void RegisterLpmEvents();
    void HandleBtLeModuleReady( bool btLeModuleReady );
    void HandleBtLeCapabilityReady( const std::list<std::string>& points );
    void HandleBtLeCapabilityNotReady( const std::list<std::string>& points );

    // @brief Output the current boot status
    void HandleGetBootStatus( const std::list<std::string>& argList, std::string& response );

    void HandleNetworkStatus( const NetManager::Protobuf::NetworkStatus& networkStatus );
    void HandleWiFiProfileResponse( const NetManager::Protobuf::WiFiProfiles& profiles );

public:
    // Handle Key Information received from LPM.
    void HandleLpmKeyInformation( IpcKeyInformation_t keyInformation );

    void HandleIntents( KeyHandlerUtil::ActionType_t intent );

    // @brief This is a registered cb in the IntentHandler for Network Busy.
    //        Actions to be taken by the Product Controller and HSM needs to
    //        be implemented here.
    void HandleNetworkStandbyIntentCb( const KeyHandlerUtil::ActionType_t& intent );

    // @brief True if all the dependent modules are up and ready.  Modules
    //        like- LPM, CAPS, SW Update etc.
    bool IsAllModuleReady() const;

    // @brief True if IsBtLeModuleReady modules is up and ready.  Module
    // IsBtLeModuleReady.
    bool IsBtLeModuleReady() const;

    // @brief True if UI (e.g., Monaco) is up and ready.
    bool IsUiConnected() const;

    // @brief True if STS sources initialization is complete.
    bool IsSTSReady() const;

    // @brief Function to call when CAPS is ready to send/receive request.
    void HandleCAPSReady( bool capsReady );

    // @brief Function to call when NetworkService is ready to send/receive request.
    void HandleNetworkModuleReady( bool networkModuleReady );

    // @brief True if system language is initialized.
    bool IsLanguageSet();

    void SendInitialCapsData() override;
    void SendActivateAccessPointCmd();
    void SendDeActivateAccessPointCmd();

    // @brief Function to send initial endpoint request to the front door like
    // "/system/capsInitializationStatus".
    void SendInitialRequests();

    // @brief Handles CapsInitializationUpdate notification.
    void HandleCapsInitializationUpdate( const SoundTouchInterface::CapsInitializationStatus &status );

    void CallbackError( const FrontDoor::Error &error );

    // @brief Handles STS sources initialization complete callback from
    // ProductSTSController.
    void HandleSTSReady();

    // @brief Get a reference to the current IntentHandler
    IntentHandler& GetIntentHandler() override
    {
        return m_IntentHandler;
    }

    // @brief Handles message sent by LPM to ProductController. As per the
    //        message id in productMessage, appropriate methods in state
    //        machine or ProductController are called.
    //
    // @param productMessage - ProductMessage protobuf
    void HandleProductMessage( const ProductMessage& productMessage );

    void UpdateUiConnectedStatus( bool status );

    // @brief Returns reference to LpmInterface
    inline std::shared_ptr< CustomProductLpmHardwareInterface >& GetLpmHardwareInterface() override
    {
        return m_LpmInterface;
    }

    // @brief Returns reference to ProductAudioService
    inline std::shared_ptr< CustomProductAudioService >& GetProductAudioServiceInstance()
    {
        return m_ProductAudioService;
    }

    // @brief Interfaces to the ProductSTSController, which implements the interactions
    //        between the Product Controller and the STS source proxies.
    void SetupProductSTSController() override;
    void HandleSTSInitWasComplete();
    void HandleSelectSourceSlot( ProductSTSAccount::ProductSourceSlot sourceSlot );
    void HandleRawKeyCliCmd( const std::list<std::string>& argList, std::string& response );

    // @brief   Returns an instance to the DisplayController.
    std::shared_ptr<DisplayController>& GetDisplayController()
    {
        return m_displayController;
    }

    // @return NetManager::Protobuf::OperationalMode of the WiFi subsystem
    NetManager::Protobuf::OperationalMode GetWiFiOperationalMode() const;

private:

    ProductControllerStateTop m_ProductControllerStateTop;
    ProductControllerStateBooting m_ProductControllerStateBooting;
    ProductControllerStateBooted m_ProductControllerStateBooted;
    CustomProductControllerStateOn m_CustomProductControllerStateOn;
    CustomProductControllerStateLowPowerResume m_ProductControllerStateLowPowerResume;
    CustomProductControllerStateLowPowerStandby m_CustomProductControllerStateLowPowerStandby;
    ProductControllerStateCriticalError m_ProductControllerStateCriticalError;
    ProductControllerStatePlaying m_ProductControllerStatePlaying;
    ProductControllerStatePlayable m_ProductControllerStatePlayable;
    CustomProductControllerStateLowPowerStandbyTransition m_CustomProductControllerStateLowPowerStandbyTransition;
    ProductControllerStateIdle m_ProductControllerStateIdle;
    CustomProductControllerStateNetworkStandby m_CustomProductControllerStateNetworkStandby;
    ProductControllerStateIdleVoiceConfigured m_ProductControllerStateVoiceConfigured;
    ProductControllerStateIdleVoiceNotConfigured m_ProductControllerStateVoiceNotConfigured;
    ProductControllerStateNetworkStandbyConfigured m_ProductControllerStateNetworkConfigured;
    ProductControllerStateNetworkStandbyNotConfigured m_ProductControllerStateNetworkNotConfigured;
    ProductControllerStateFactoryDefault m_ProductControllerStateFactoryDefault;
    ProductControllerStatePlayingDeselected m_ProductControllerStatePlayingDeselected;
    ProductControllerStatePlayingSelected m_ProductControllerStatePlayingSelected;
    ProductControllerStatePlayingSelectedSilent m_ProductControllerStatePlayingSelectedSilent;
    ProductControllerStatePlayingSelectedSilentSourceInvalid m_ProductControllerStatePlayingSelectedSilentSourceInvalid;
    ProductControllerStatePlayingSelectedSilentSourceValid m_ProductControllerStatePlayingSelectedSilentSourceValid;
    ProductControllerStatePlayingSelectedNotSilent m_ProductControllerStatePlayingSelectedNotSilent;
    ProductControllerStatePlayingSelectedSetup m_ProductControllerStatePlayingSelectedSetup;
    ProductControllerStatePlayingSelectedSetupNetworkConfig m_ProductControllerStatePlayingSelectedSetupNetworkConfig;
    ProductControllerStatePlayingSelectedSetupNetworkConfigWiFiConnection m_ProductControllerStatePlayingSelectedSetupNetworkConfigWiFiConnection;
    ProductControllerStatePlayingSelectedSetupNetworkConfigWiFiTransition m_ProductControllerStatePlayingSelectedSetupNetworkConfigWiFiTransition;
    ProductControllerStatePlayingSelectedSetupOther m_ProductControllerStatePlayingSelectedSetupOther;
    ProductControllerStatePlayingSelectedSetupExiting m_ProductControllerStatePlayingSelectedSetupExiting;
    ProductControllerStatePlayingSelectedSetupNetworkConfigWiFiExiting m_ProductControllerStatePlayingSelectedSetupNetworkConfigWiFiExiting;
    ProductControllerStatePlayingSelectedSetupNetworkConfigWiFiAborting m_ProductControllerStatePlayingSelectedSetupNetworkConfigWiFiAborting;
    ProductControllerStatePlayingSelectedStoppingStreams m_ProductControllerStatePlayingSelectedStoppingStreams;
    ProductControllerStatePlayableTransition m_ProductControllerStatePlayableTransition;
    ProductControllerStatePlayableTransitionInternal m_ProductControllerStatePlayableTransitionInternal;
    ProductControllerStatePlayableTransitionIdle m_ProductControllerStatePlayableTransitionIdle;
    ProductControllerStatePlayableTransitionNetworkStandby m_ProductControllerStatePlayableTransitionNetworkStandby;
    ProductControllerStatePlayingTransition m_ProductControllerStatePlayingTransition;
    ProductControllerStateFirstBootGreeting m_ProductControllerStateFirstBootGreeting;
    ProductControllerStateFirstBootGreetingTransition m_ProductControllerStateFirstBootGreetingTransition;
    ProductControllerStatePlayingTransitionSwitch m_ProductControllerStatePlayingTransitionSwitch;
    ProductControllerStateStoppingStreamsDedicated m_ProductControllerStateStoppingStreamsDedicated;
    ProductControllerStateStoppingStreamsDedicatedForFactoryDefault m_ProductControllerStateStoppingStreamsDedicatedForFactoryDefault;
    ProductControllerStateStoppingStreamsDedicatedForSoftwareUpdate m_ProductControllerStateStoppingStreamsDedicatedForSoftwareUpdate;
    ProductControllerStateSoftwareInstall m_ProductControllerStateSoftwareInstall;
    ProductControllerStateSoftwareInstallTransition m_ProductControllerStateSoftwareInstallTransition;
    ProductControllerStateSoftwareInstall m_ProductControllerStateSoftwareInstallManual;

    std::shared_ptr<CustomProductAudioService> m_ProductAudioService;

    std::shared_ptr<CustomProductKeyInputManager> m_ProductKeyInputManager;

    std::shared_ptr<ProductCommandLine> m_ProductCommandLine;
    std::shared_ptr<CommonProductCommandLine> m_CommonProductCommandLine;

    std::unique_ptr<LightBar::LightBarController> m_lightbarController;
    std::shared_ptr<DisplayController> m_displayController;
    IntentHandler m_IntentHandler;
    std::shared_ptr<Clock> m_clock;

    bool m_isBLEModuleReady  = false;
    bool m_isUiConnected = false;

    // @brief Interfaces to the ProductSTSController, which implements the
    //        interactions between the Product Controller and the STS source
    //        proxies.
    bool m_isSTSReady = false;
    bool m_IsAudioPathReady = true;

    // Shared Pointer to the LPM Custom Hardware Interface.
    std::shared_ptr<CustomProductLpmHardwareInterface> m_LpmInterface;

    ProductSTSController m_ProductSTSController;
    ProductIotHandler m_ProductIotHandler;
    BOptional<AsyncCallback<LpmServiceMessages::IpcAuxState_t>> m_AuxCableStateCb;
};

static const char* const KEY_NAMES[] __attribute__( ( unused ) ) =
{
    "Bluetooth",
    "Aux",
    "VolumePlus",
    "MultiFunction",
    "Volumeminus",
    "Alexa",
    "InvalidKey"
};

constexpr auto NUM_KEY_NAMES __attribute__( ( unused ) ) =
    sizeof( KEY_NAMES ) / sizeof( KEY_NAMES[0] );

} // namespace
