////////////////////////////////////////////////////////////////////////////////
/// @file   EddieProductController.h
/// @brief  Eddie Product controller class.
///
/// @attention Copyright 2017 Bose Corporation, Framingham, MA
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <locale>
#include <string>
#include <algorithm>
#include <iostream>

#include "ProductController.h"
#include "NotifyTargetTaskIF.h"
#include "ProtoPersistenceIF.h"
#include "ProductControllerStateTop.h"
#include "ProductControllerStateNetworkStandby.h"
#include "ProductControllerStateLowPowerStandby.h"
#include "ProductControllerStateLowPowerStandbyTransition.h"
#include "ProductControllerStateNetworkStandbyConfigured.h"
#include "ProductControllerStateNetworkStandbyNotConfigured.h"
#include "ProductControllerStateIdleVoiceConfigured.h"
#include "ProductControllerStateIdleVoiceNotConfigured.h"
#include "ProductControllerStatePlayable.h"
#include "ProductControllerStatePlaying.h"
#include "ProductControllerStatePlayingActive.h"
#include "ProductControllerStatePlayingInactive.h"
#include "ProductControllerStateRebooting.h"
#include "ProductControllerStateBooting.h"
#include "CustomProductControllerStateOn.h"
#include "ProductControllerStateOn.h"
#include "ProductControllerStateIdle.h"
#include "ProductControllerStateSoftwareInstall.h"
#include "ProductControllerStateCriticalError.h"
#include "ProductControllerStateFactoryDefault.h"
#include "ProductControllerStatePlayingDeselected.h"
#include "ProductControllerStatePlayingSelected.h"
#include "ProductControllerStatePlayingSelectedSilent.h"
#include "ProductControllerStatePlayingSelectedNotSilent.h"
#include "ProductControllerStatePlayingSelectedSetup.h"
#include "ProductControllerStatePlayingSelectedSetupNetworkTransition.h"
#include "ProductControllerStatePlayingSelectedSetupNetwork.h"
#include "ProductControllerStatePlayingSelectedSetupOther.h"
#include "ProductControllerStatePlayingSelectedSetupExiting.h"
#include "ProductControllerStateStoppingStreams.h"
#include "ProductControllerStatePlayableTransition.h"
#include "ProductControllerStatePlayableTransitionIdle.h"
#include "ProductControllerStatePlayableTransitionInternal.h"
#include "ProductControllerStatePlayableTransitionNetworkStandby.h"
#include "ProductControllerStateSoftwareUpdateTransition.h"
#include "ProductControllerStatePlayingTransition.h"
#include "ProductControllerStatePlayingTransitionSwitch.h"
#include "ProductControllerStateStoppingStreamsDedicated.h"
#include "ProductControllerStateStoppingStreamsDedicatedForFactoryDefault.h"
#include "ProductControllerStateStoppingStreamsDedicatedForSoftwareUpdate.h"
#include "LightBarController.h"
#include "ConfigurationStatus.pb.h"
#include "SoundTouchInterface/AllowSourceSelect.pb.h"
#include "NetManager.pb.h"
#include "SoundTouchInterface/CapsInitializationStatus.pb.h"
#include "SoundTouchInterface/ContentSelectionService.pb.h"
#include "SoundTouchInterface/PlayerService.pb.h"
#include "ProductCliClient.h"
#include "KeyHandler.h"
#include "IntentHandler.h"
#include "ProductSTSController.h"
#include "DisplayController.h"
#include "DataCollectionClientIF.h"
#include "DataCollectionClientInterface.h"
#include "MacAddressInfo.h"
#include "BOptional.h"
#include "VoiceServiceClient.h"
#include "BoseVersion.h"

namespace ProductApp
{

class CustomProductAudioService;

class EddieProductController : public ProductController
{
public:
    EddieProductController( std::string const& ProductName = "eddie" );
    virtual ~EddieProductController();

    void Initialize();

    Callback < ProductMessage > GetMessageHandler( );

    NetManager::Protobuf::NetworkStatus const& GetNetworkStatus() const
    {
        return m_cachedStatus.get();
    }
    std::vector<std::string> GetUniqueLanguages() const override
    {
        return {};
    }

    BLESetupService::ProductId GetProductId() const override
    {
        return BLESetupService::ProductId::EDDIE;
    }
    std::string GetProductVersionNumber() const override
    {
        return ( VERSION_STRING_SHORT + std::string( "-" ) + VERSION_BUILD_ABBREV_COMMIT );
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////
    /// @name  IsBooted
    /// @brief The following methods are used by the state machine to determine the status of the
    ///        product controller.
    /// @return bool
    ////////////////////////////////////////////////////////////////////////////////////////////////
    bool IsBooted( ) const override;

    ///////////////////////////////////////////////////////////////////////////////
    /// @name  IsNetworkConfigured
    /// @brief true if system is conencted to ethernet or number of wifi profiles are nonzero
    /// @return bool
    ////////////////////////////////////////////////////////////////////////////////
    bool IsNetworkConfigured() const override;
    bool IsNetworkConnected( ) const override;
    uint32_t GetWifiProfileCount() const override;
    bool IsAutoWakeEnabled( )  const override
    {
        /// TO_Do
        return false;
    }
    bool IsVoiceConfigured( )  const override
    {
        /// TO_Do
        return false;
    }


    std::string const& GetProductType() const override;
    std::string const& GetProductModel() const override;
    std::string GetProductColor() const override;
    std::string const& GetProductVariant() const override;
    std::string const& GetProductDescription() const override;
    std::string const& GetDefaultProductName() const override;
    BLESetupService::VariantId GetVariantId() const override;

    void ClearWifiProfileCount() override
    {
        m_wifiProfilesCount = 0;
    }

    void PerformRequestforWiFiProfiles() override;

private:
    /// Disable copies
    EddieProductController( const EddieProductController& ) = delete;
    EddieProductController& operator=( const EddieProductController& ) = delete;

private:
    ///Register with LPM for events notifications

    void InitializeAction( );
    void RegisterLpmEvents();
    void RegisterKeyHandler();
    void RegisterEndPoints();
    void HandleCliCmd( uint16_t cmdKey,
                       const std::list<std::string> & argList,
                       AsyncCallback<std::string, int32_t> rspAndRspCmplt,
                       int32_t transact_id );
    void RegisterCliClientCmds();

    void HandleBtLeModuleReady( bool btLeModuleReady );
    void HandleNetworkCapabilityReady( const std::list<std::string>& points );
    void HandleNetworkCapabilityNotReady( const std::list<std::string>& points );
    void HandleCapsCapabilityReady( const std::list<std::string>& points );
    void HandleCapsCapabilityNotReady( const std::list<std::string>& points );
    void HandleBtLeCapabilityReady( const std::list<std::string>& points );
    void HandleBtLeCapabilityNotReady( const std::list<std::string>& points );

///////////////////////////////////////////////////////////////////////////////
/// @name DataCollectionClient
/// @brief invokes DataCollectionClient When any key is released.
/// @return void
//////////////////////////////////////////////////////////////////////////////
    void SendDataCollection( const IpcKeyInformation_t& keyInformation );

///////////////////////////////////////////////////////////////////////////////
/// @name  ReadSystemLanguageFromPersistence
/// @brief Function to read persisted language code from /mnt/nv/product-persistence.
/// @return void
////////////////////////////////////////////////////////////////////////////////
    void ReadSystemLanguageFromPersistence();
    void ReadConfigurationStatusFromPersistence();

///////////////////////////////////////////////////////////////////////////////
/// @name  PersistSystemLanguageCode
/// @brief Function to persist language code in /mnt/nv/product-persistence.
/// @return void
////////////////////////////////////////////////////////////////////////////////
    void PersistSystemLanguageCode();
    void PersistSystemConfigurationStatus();

///////////////////////////////////////////////////////////////////////////////
/// @name  HandleSetDisplayAutoMode
/// @brief Function to TDB
/// @return void
////////////////////////////////////////////////////////////////////////////////
    void HandleSetDisplayAutoMode( const std::list<std::string> & argList, std::string& response );
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
/// @name  IsCAPSReady
/// @brief true if CAPS module is ready.
/// @return bool
////////////////////////////////////////////////////////////////////////////////
    bool IsCAPSReady() const;
    bool IsNetworkModuleReady() const;

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
    void SendActivateAccessPointCmd();
    void SendDeActivateAccessPointCmd();

///////////////////////////////////////////////////////////////////////////////
/// @name  HandleConfigurationStatusRequest
/// @brief "/system/configuration/status" endpoint request handler.
/// @return void
////////////////////////////////////////////////////////////////////////////////
    void HandleConfigurationStatusRequest( const Callback<ProductPb::ConfigurationStatus> &resp );

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
    void CallbackError( const EndPointsError::Error &error );

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
///       between the Eddie Product Controller and the STS source proxies.
///
//////////////////////////////////////////////////////////////////////////////////////////////
    void SetupProductSTSController( void );
    void HandleSTSInitWasComplete( void );
    void HandleSelectSourceSlot( ProductSTSAccount::ProductSourceSlot sourceSlot );
    void HandleRawKeyCliCmd( const std::list<std::string>& argList, std::string& response );

//////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief set the display controllee automatic mode  to true or false (manual)
///
//////////////////////////////////////////////////////////////////////////////////////////////
    void SetDisplayAutoMode( bool autoMode ) const
    {
        m_displayController->SetAutoMode( autoMode );
    }


///////////////////////////////////////////////////////////////////////////////
/// @name   GetVoiceServiceClient
/// @brief  Returns reference to VoiceServiceClient
/// @return VoiceServiceClient&
///////////////////////////////////////////////////////////////////////////////
    inline VoiceServiceClient& GetVoiceServiceClient()
    {
        return m_voiceServiceClient;
    }

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @name   GetWiFiOperationalMode
///
/// @return NetManager::Protobuf::OperationalMode of the WiFi subsystem
///
////////////////////////////////////////////////////////////////////////////////////////////////////
    NetManager::Protobuf::OperationalMode GetWiFiOperationalMode( );

private:

    ProductControllerStateTop                                       m_ProductControllerStateTop;
    ProductControllerStateBooting                                   m_ProductControllerStateBooting;
    CustomProductControllerStateOn                                  m_CustomProductControllerStateOn;
    ProductControllerStateLowPowerStandby                           m_ProductControllerStateLowPowerStandby;
    ProductControllerStateSoftwareInstall                           m_ProductControllerStateSwInstall;
    ProductControllerStateCriticalError                             m_ProductControllerStateCriticalError;
    ProductControllerStateRebooting                                 m_ProductControllerStateRebooting;
    ProductControllerStatePlaying                                   m_ProductControllerStatePlaying;
    ProductControllerStatePlayable                                  m_ProductControllerStatePlayable;
    ProductControllerStateLowPowerStandbyTransition                 m_ProductControllerStateLowPowerStandbyTransition;
    ProductControllerStatePlayingActive                             m_ProductControllerStatePlayingActive;
    ProductControllerStatePlayingInactive                           m_ProductControllerStatePlayingInactive;
    ProductControllerStateIdle                                      m_ProductControllerStateIdle;
    ProductControllerStateNetworkStandby                            m_ProductControllerStateNetworkStandby;
    ProductControllerStateIdleVoiceConfigured                       m_ProductControllerStateVoiceConfigured;
    ProductControllerStateIdleVoiceNotConfigured                    m_ProductControllerStateVoiceNotConfigured;
    ProductControllerStateNetworkStandbyConfigured                  m_ProductControllerStateNetworkConfigured;
    ProductControllerStateNetworkStandbyNotConfigured               m_ProductControllerStateNetworkNotConfigured;
    ProductControllerStateFactoryDefault                            m_ProductControllerStateFactoryDefault;
    ProductControllerStatePlayingDeselected                         m_ProductControllerStatePlayingDeselected;
    ProductControllerStatePlayingSelected                           m_ProductControllerStatePlayingSelected;
    ProductControllerStatePlayingSelectedSilent                     m_ProductControllerStatePlayingSelectedSilent;
    ProductControllerStatePlayingSelectedNotSilent                  m_ProductControllerStatePlayingSelectedNotSilent;
    ProductControllerStatePlayingSelectedSetup                      m_ProductControllerStatePlayingSelectedSetup;
    ProductControllerStatePlayingSelectedSetupNetwork               m_ProductControllerStatePlayingSelectedSetupNetwork;
    ProductControllerStatePlayingSelectedSetupNetworkTransition     m_ProductControllerStatePlayingSelectedSetupNetworkTransition;
    ProductControllerStatePlayingSelectedSetupOther                 m_ProductControllerStatePlayingSelectedSetupOther;
    ProductControllerStatePlayingSelectedSetupExiting               m_ProductControllerStatePlayingSelectedSetupExiting;
    ProductControllerStateStoppingStreams                           m_ProductControllerStateStoppingStreams;
    ProductControllerStatePlayableTransition                        m_ProductControllerStatePlayableTransition;
    ProductControllerStatePlayableTransitionInternal                m_ProductControllerStatePlayableTransitionInternal;
    ProductControllerStatePlayableTransitionIdle                    m_ProductControllerStatePlayableTransitionIdle;
    ProductControllerStatePlayableTransitionNetworkStandby          m_ProductControllerStatePlayableTransitionNetworkStandby;
    ProductControllerStateSoftwareUpdateTransition                  m_ProductControllerStateSoftwareUpdateTransition;
    ProductControllerStatePlayingTransition                         m_ProductControllerStatePlayingTransition;
    ProductControllerStatePlayingTransitionSwitch                   m_ProductControllerStatePlayingTransitionSwitch;
    ProductControllerStateStoppingStreamsDedicated                  m_ProductControllerStateStoppingStreamsDedicated;
    ProductControllerStateStoppingStreamsDedicatedForFactoryDefault m_ProductControllerStateStoppingStreamsDedicatedForFactoryDefault;
    ProductControllerStateStoppingStreamsDedicatedForSoftwareUpdate m_ProductControllerStateStoppingStreamsDedicatedForSoftwareUpdate;

    /// Key Handler
    KeyHandlerUtil::KeyHandler                  m_KeyHandler;
    ProtoPersistenceIF::ProtoPersistencePtr     m_ConfigurationStatusPersistence = nullptr;
    ProductPb::ConfigurationStatus              m_ConfigurationStatus;
    BOptional<NetManager::Protobuf::NetworkStatus> m_cachedStatus;

    /// ProductAudioService
    std::shared_ptr< CustomProductAudioService> m_ProductAudioService;

    ProductCliClient                            m_productCliClient;

    std::unique_ptr<LightBar::LightBarController>         m_lightbarController;
    std::unique_ptr<DisplayController>          m_displayController;
    IntentHandler                               m_IntentHandler;
    bool                                        m_isCapsReady = false;
    bool                                        m_isNetworkModuleReady  = false;
    bool                                        m_isBLEModuleReady  = false;
    bool                                        m_isUiConnected = false;

    BOptional<int>                              m_wifiProfilesCount;
    AsyncCallback<EndPointsError::Error>            m_fdErrorCb;
    //////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief Interfaces to the ProductSTSController, which implements the interactions
    ///       between the Eddie Product Controller and the STS source proxies.
    ///
    //////////////////////////////////////////////////////////////////////////////////////////////
    bool                                        m_isSTSReady = false;
    bool m_IsAudioPathReady = true;
    ProductSTSController                        m_ProductSTSController;
    std::shared_ptr<DataCollectionClientIF>     m_DataCollectionClient;
    VoiceServiceClient                          m_voiceServiceClient;

    /// Shared Pointer to the LPM Custom Hardware Interface
    std::shared_ptr< CustomProductLpmHardwareInterface > m_LpmInterface;

    //DataCollectionClientInterface
    DataCollectionClientInterface                m_dataCollectionClientInterface;
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
