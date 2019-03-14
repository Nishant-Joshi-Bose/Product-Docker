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
#include "ProductControllerStateLowPowerResume.h"
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

    Callback < ProductMessage > GetMessageHandler( ) override;

    std::vector<std::string> GetUniqueLanguages() const override
    {
        return {};
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    /// @name  IsBooted
    /// @brief The following methods are used by the state machine to determine the status of the
    ///        product controller.
    /// @return bool
    ////////////////////////////////////////////////////////////////////////////////////////////////
    bool IsBooted( )        const override;
    bool IsLowPowerExited() const override;

    bool IsAutoWakeEnabled( )  const override
    {
        /// TO_Do
        return false;
    }

    std::string GetDefaultProductName() const override;
    void RegisterAuxEvents( AsyncCallback<LpmServiceMessages::IpcAuxState_t> &cb );

private:
    /// Disable copies
    CustomProductController( const CustomProductController& ) = delete;
    CustomProductController& operator=( const CustomProductController& ) = delete;

private:
    ///Register with LPM for events notifications

    void InitializeHsm( );
    void InitializeAction( );
    void ProductDependentInitialize();
    void LoadProductConfiguration();
    void RegisterLpmEvents();
    void HandleBtLeModuleReady( bool btLeModuleReady );
    void HandleBtLeCapabilityReady( const std::list<std::string>& points );
    void HandleBtLeCapabilityNotReady( const std::list<std::string>& points );

///////////////////////////////////////////////////////////////////////////////
/// @name  HandleGetBootStatus
/// @brief Function to output the current boot status
/// @return void
////////////////////////////////////////////////////////////////////////////////
    void HandleGetBootStatus( const std::list<std::string>& argList, std::string& response );

    void HandleNetworkStatus( const NetManager::Protobuf::NetworkStatus& networkStatus );
    void HandleWiFiProfileResponse( const NetManager::Protobuf::WiFiProfiles& profiles );

public:
    /// Handle Key Information received from LPM
    void HandleLpmKeyInformation( IpcKeyInformation_t keyInformation );

    void HandleIntents( KeyHandlerUtil::ActionType_t intent );

///////////////////////////////////////////////////////////////////////////////
/// @name  HandleNetworkStandbyIntentCb
/// @brief This is a registered cb in the IntentHandler for Network Busy
/// Actions to be taken by the Product Controller and HSM needs to be implemented
//  here.
/// @return void
////////////////////////////////////////////////////////////////////////////////
    void HandleNetworkStandbyIntentCb( const KeyHandlerUtil::ActionType_t& intent );

///////////////////////////////////////////////////////////////////////////////
/// @name  IsAllModuleReady
/// @brief true if all the dependent modules are up and ready.
/// Modules like- LPM, CAPS, SW Update etc.
/// @return bool
////////////////////////////////////////////////////////////////////////////////
    bool IsAllModuleReady() const;

///////////////////////////////////////////////////////////////////////////////
/// @name  IsBtLeModuleReady
/// @brief true if IsBtLeModuleReady modules is up and ready.
/// Module IsBtLeModuleReady.
/// @return bool
////////////////////////////////////////////////////////////////////////////////
    bool IsBtLeModuleReady() const;

///////////////////////////////////////////////////////////////////////////////
/// @name  IsUiConnected
/// @brief true if UI(monaco) is up and ready.
/// @return bool
////////////////////////////////////////////////////////////////////////////////
    bool IsUiConnected() const;

///////////////////////////////////////////////////////////////////////////////
/// @name  IsSTSReady
/// @brief true if STS sources initialization is complete.
/// @return bool
////////////////////////////////////////////////////////////////////////////////
    bool IsSTSReady() const;

///////////////////////////////////////////////////////////////////////////////
/// @name  HandleCAPSReady
/// @brief Function to call when CAPS is ready to send/receive request.
/// @return void
////////////////////////////////////////////////////////////////////////////////
    void HandleCAPSReady( bool capsReady );

///////////////////////////////////////////////////////////////////////////////
/// @name  HandleNetworkModuleReady
/// @brief Function to call when NetworkService is ready to send/receive request.
/// @return void
////////////////////////////////////////////////////////////////////////////////
    void HandleNetworkModuleReady( bool networkModuleReady );

///////////////////////////////////////////////////////////////////////////////
/// @name  IsLanguageSet
/// @brief true if system language is initialized
/// @return bool
////////////////////////////////////////////////////////////////////////////////
    bool IsLanguageSet();
    void SendInitialCapsData() override;
    void SendActivateAccessPointCmd();
    void SendDeActivateAccessPointCmd();

///////////////////////////////////////////////////////////////////////////////
/// @name  SendInitialRequests
/// @brief Function to send initial endpoint request to the front door like "/system/capsInitializationStatus".
/// @return void
////////////////////////////////////////////////////////////////////////////////
    void SendInitialRequests();
///////////////////////////////////////////////////////////////////////////////
/// @name   HandleCapsInitializationUpdate
/// @brief- Handles CapsInitializationUpdate notification
/// @return void
///////////////////////////////////////////////////////////////////////////////
    void HandleCapsInitializationUpdate( const SoundTouchInterface::CapsInitializationStatus &status );
    void CallbackError( const FrontDoor::Error &error );

///////////////////////////////////////////////////////////////////////////////
/// @name   HandleSTSReady
/// @brief- Handles STS sources initialization complete callback from
/// ProductSTSController
/// @return void
///////////////////////////////////////////////////////////////////////////////
    void HandleSTSReady( void );

///////////////////////////////////////////////////////////////////////////////
/// @name   GetIntentHandler
/// @brief  Returns reference to IntentHandler
/// @return IntentHandler&
///////////////////////////////////////////////////////////////////////////////
    IntentHandler& GetIntentHandler( ) override
    {
        return m_IntentHandler;
    }
///////////////////////////////////////////////////////////////////////////////
/// @name   HandleProductMessage
/// @brief  Handles message sent by LPM to ProductController. As per the
///         message id in productMessage, appropriate methods in state machine
///         or ProductController are called
/// @param  ProductMessage - ProductMessage protobuf
/// @return void
///////////////////////////////////////////////////////////////////////////////
    void HandleProductMessage( const ProductMessage& productMessage );

    void UpdateUiConnectedStatus( bool status );

///////////////////////////////////////////////////////////////////////////////
/// @name   GetLpmHardwareInterface
/// @brief  Returns reference to LpmInterface
/// @return LpmInterface&
///////////////////////////////////////////////////////////////////////////////
    inline std::shared_ptr< CustomProductLpmHardwareInterface >& GetLpmHardwareInterface( ) override
    {
        return m_LpmInterface;
    }

///////////////////////////////////////////////////////////////////////////////
/// @name   GetProductAudioServiceInstance
/// @brief  Returns reference to ProductAudioService
/// @return CustomProductLpmHardwareInterface&
///////////////////////////////////////////////////////////////////////////////
    inline std::shared_ptr< CustomProductAudioService >& GetProductAudioServiceInstance( )
    {
        return m_ProductAudioService;
    }

//////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief Interfaces to the ProductSTSController, which implements the interactions
///       between the Product Controller and the STS source proxies.
///
//////////////////////////////////////////////////////////////////////////////////////////////
    void SetupProductSTSController( void ) override;
    void HandleSTSInitWasComplete( void );
    void HandleSelectSourceSlot( ProductSTSAccount::ProductSourceSlot sourceSlot );
    void HandleRawKeyCliCmd( const std::list<std::string>& argList, std::string& response );

//////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief Returns an instance to the DisplayController.
///
//////////////////////////////////////////////////////////////////////////////////////////////
    std::shared_ptr< DisplayController >& GetDisplayController( )
    {
        return m_displayController;
    }


////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   GetWiFiOperationalMode
///
/// @return NetManager::Protobuf::OperationalMode of the WiFi subsystem
///
////////////////////////////////////////////////////////////////////////////////////////////////////
    NetManager::Protobuf::OperationalMode GetWiFiOperationalMode( ) const;

private:

    ProductControllerStateTop                                       m_ProductControllerStateTop;
    ProductControllerStateBooting                                   m_ProductControllerStateBooting;
    ProductControllerStateBooted                                    m_ProductControllerStateBooted;
    CustomProductControllerStateOn                                  m_CustomProductControllerStateOn;
    ProductControllerStateLowPowerResume                            m_ProductControllerStateLowPowerResume;
    CustomProductControllerStateLowPowerStandby                     m_CustomProductControllerStateLowPowerStandby;
    ProductControllerStateCriticalError                             m_ProductControllerStateCriticalError;
    ProductControllerStatePlaying                                   m_ProductControllerStatePlaying;
    ProductControllerStatePlayable                                  m_ProductControllerStatePlayable;
    CustomProductControllerStateLowPowerStandbyTransition           m_CustomProductControllerStateLowPowerStandbyTransition;
    ProductControllerStateIdle                                      m_ProductControllerStateIdle;
    CustomProductControllerStateNetworkStandby                      m_CustomProductControllerStateNetworkStandby;
    ProductControllerStateIdleVoiceConfigured                       m_ProductControllerStateVoiceConfigured;
    ProductControllerStateIdleVoiceNotConfigured                    m_ProductControllerStateVoiceNotConfigured;
    ProductControllerStateNetworkStandbyConfigured                  m_ProductControllerStateNetworkConfigured;
    ProductControllerStateNetworkStandbyNotConfigured               m_ProductControllerStateNetworkNotConfigured;
    ProductControllerStateFactoryDefault                            m_ProductControllerStateFactoryDefault;
    ProductControllerStatePlayingDeselected                         m_ProductControllerStatePlayingDeselected;
    ProductControllerStatePlayingSelected                           m_ProductControllerStatePlayingSelected;
    ProductControllerStatePlayingSelectedSilent                     m_ProductControllerStatePlayingSelectedSilent;
    ProductControllerStatePlayingSelectedSilentSourceInvalid        m_ProductControllerStatePlayingSelectedSilentSourceInvalid;
    ProductControllerStatePlayingSelectedSilentSourceValid          m_ProductControllerStatePlayingSelectedSilentSourceValid;
    ProductControllerStatePlayingSelectedNotSilent                  m_ProductControllerStatePlayingSelectedNotSilent;
    ProductControllerStatePlayingSelectedSetup                      m_ProductControllerStatePlayingSelectedSetup;
    ProductControllerStatePlayingSelectedSetupNetworkConfig         m_ProductControllerStatePlayingSelectedSetupNetworkConfig;
    ProductControllerStatePlayingSelectedSetupNetworkConfigWiFiConnection     m_ProductControllerStatePlayingSelectedSetupNetworkConfigWiFiConnection;
    ProductControllerStatePlayingSelectedSetupNetworkConfigWiFiTransition     m_ProductControllerStatePlayingSelectedSetupNetworkConfigWiFiTransition;
    ProductControllerStatePlayingSelectedSetupOther                 m_ProductControllerStatePlayingSelectedSetupOther;
    ProductControllerStatePlayingSelectedSetupExiting               m_ProductControllerStatePlayingSelectedSetupExiting;
    ProductControllerStatePlayingSelectedSetupNetworkConfigWiFiExiting        m_ProductControllerStatePlayingSelectedSetupNetworkConfigWiFiExiting;
    ProductControllerStatePlayingSelectedSetupNetworkConfigWiFiAborting       m_ProductControllerStatePlayingSelectedSetupNetworkConfigWiFiAborting;
    ProductControllerStatePlayingSelectedStoppingStreams            m_ProductControllerStatePlayingSelectedStoppingStreams;
    ProductControllerStatePlayableTransition                        m_ProductControllerStatePlayableTransition;
    ProductControllerStatePlayableTransitionInternal                m_ProductControllerStatePlayableTransitionInternal;
    ProductControllerStatePlayableTransitionIdle                    m_ProductControllerStatePlayableTransitionIdle;
    ProductControllerStatePlayableTransitionNetworkStandby          m_ProductControllerStatePlayableTransitionNetworkStandby;
    ProductControllerStatePlayingTransition                         m_ProductControllerStatePlayingTransition;
    ProductControllerStateFirstBootGreeting                         m_ProductControllerStateFirstBootGreeting;
    ProductControllerStateFirstBootGreetingTransition               m_ProductControllerStateFirstBootGreetingTransition;
    ProductControllerStatePlayingTransitionSwitch                   m_ProductControllerStatePlayingTransitionSwitch;
    ProductControllerStateStoppingStreamsDedicated                  m_ProductControllerStateStoppingStreamsDedicated;
    ProductControllerStateStoppingStreamsDedicatedForFactoryDefault m_ProductControllerStateStoppingStreamsDedicatedForFactoryDefault;
    ProductControllerStateStoppingStreamsDedicatedForSoftwareUpdate m_ProductControllerStateStoppingStreamsDedicatedForSoftwareUpdate;
    ProductControllerStateSoftwareInstall                           m_ProductControllerStateSoftwareInstall;
    ProductControllerStateSoftwareInstallTransition                 m_ProductControllerStateSoftwareInstallTransition;
    ProductControllerStateSoftwareInstall                           m_ProductControllerStateSoftwareInstallManual;


    /// ProductAudioService
    std::shared_ptr<CustomProductAudioService>                      m_ProductAudioService;

    /// ProductKeyInputManager
    std::shared_ptr<CustomProductKeyInputManager>                   m_ProductKeyInputManager;

    std::shared_ptr<ProductCommandLine>                             m_ProductCommandLine;
    std::shared_ptr<CommonProductCommandLine>                       m_CommonProductCommandLine;

    std::unique_ptr<LightBar::LightBarController>                   m_lightbarController;
    std::shared_ptr<DisplayController>                              m_displayController;
    IntentHandler                                                   m_IntentHandler;
    std::shared_ptr<Clock>                                          m_clock;
    std::string                                                     m_productName = "NOT SET";

    bool                                                            m_isBLEModuleReady  = false;
    bool                                                            m_isUiConnected = false;

    ProductPb::ProductConfig                                        m_productConfig;

    //////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief Interfaces to the ProductSTSController, which implements the interactions
    ///       between the Product Controller and the STS source proxies.
    ///
    //////////////////////////////////////////////////////////////////////////////////////////////
    bool                                                            m_isSTSReady = false;
    bool m_IsAudioPathReady = true;

    /// Shared Pointer to the LPM Custom Hardware Interface
    std::shared_ptr< CustomProductLpmHardwareInterface >            m_LpmInterface;

    ProductSTSController                                            m_ProductSTSController;
    ProductIotHandler                                               m_ProductIotHandler;
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
}
// namespace
