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
#include "ProductControllerStateSetup.h"
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
#include "CustomProductControllerStateBooting.h"
#include "CustomProductControllerStateSetup.h"
#include "CustomProductControllerStateOn.h"
#include "ProductControllerStateOn.h"
#include "ProductControllerStateIdle.h"
#include "ProductControllerStateSoftwareUpdating.h"
#include "ProductControllerStateCriticalError.h"
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
#include "DataCollectionClient.h"
#include "MacAddressInfo.h"
#include "BOptional.h"
#include "VoiceServiceClient.h"

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
    std::string GetDefaultProductName() const override
    {
        /// To-Do: fix the default name
        return "Bose " + MacAddressInfo::GetPrimaryMAC();
    }
    std::vector<std::string> GetUniqueLanguages() const override
    {
        return {};
    }

    BLESetupService::ProductId GetProductId() const override
    {
        return BLESetupService::ProductId::EDDIE;
    }
    BLESetupService::VariantId GetVariantId() const override
    {
        return BLESetupService::VariantId::NONE;
    }
    std::string GetProductVersionNumber() const override
    {
        return "0.0.1";
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
    bool IsSoftwareUpdateRequired( ) const override
    {
        /// TO_Do
        return false;
    }

    std::string const& GetProductType() const override;
    std::string const& GetProductVariant() const override;

private:
    /// Disable copies
    EddieProductController( const EddieProductController& ) = delete;
    EddieProductController& operator=( const EddieProductController& ) = delete;

private:
    ///Register with LPM for events notifications

    void RegisterLpmEvents();
    void RegisterKeyHandler();
    void RegisterEndPoints();
    void HandleCliCmd( uint16_t cmdKey,
                       const std::list<std::string> & argList,
                       AsyncCallback<std::string, int32_t> rspAndRspCmplt,
                       int32_t transact_id );
    void RegisterCliClientCmds() override;

    void HandleBtLeModuleReady( bool btLeModuleReady );
    void HandleNetworkCapabilityReady( const std::list<std::string>& points );
    void HandleNetworkCapabilityNotReady( const std::list<std::string>& points );
    void HandleCapsCapabilityReady( const std::list<std::string>& points );
    void HandleCapsCapabilityNotReady( const std::list<std::string>& points );
    void HandleBtLeCapabilityReady( const std::list<std::string>& points );
    void HandleBtLeCapabilityNotReady( const std::list<std::string>& points );

///////////////////////////////////////////////////////////////////////////////
/// @name DataCollectionClient
/// @brief When any key is been released sending the Data to DataCollectionClient
/// @return void
//////////////////////////////////////////////////////////////////////////////
    void SendDataCollection( const IpcKeyInformation_t& keyInformation );
    std::string keyToOriginator( enum KeyOrigin_t e );
    std::string keyToEventName( uint32_t e );
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

    void HandleAllowSourceSelectCliCmd( const std::list<std::string> & argList, std::string& response );

///////////////////////////////////////////////////////////////////////////////
/// @name  HandleSetDisplayAutoMode
/// @brief Function to TDB
/// @return void
////////////////////////////////////////////////////////////////////////////////
    void HandleSetDisplayAutoMode( const std::list<std::string> & argList, std::string& response );

///////////////////////////////////////////////////////////////////////////////
/// @name HandleGetProductControllerStateCliCmd
/// @brief Function to get the state of EddieProductController
///        Usage: getProductState
///////////////////////////////////////////////////////////////////////////////
    void HandleGetProductControllerStateCliCmd( const std::list<std::string> & argList,
                                                std::string& response );

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
/// @name  IsCAPSReady
/// @brief true if CAPS module is ready.
/// @return bool
////////////////////////////////////////////////////////////////////////////////
    bool IsCAPSReady() const;
    bool IsNetworkModuleReady() const;
    bool IsLpmReady() const;

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
    void CallbackError( const FRONT_DOOR_CLIENT_ERRORS errorCode );

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
    IntentHandler& GetIntentHandler()
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

    ProductControllerStateTop               m_ProductControllerStateTop;
    CustomProductControllerStateBooting     m_CustomProductControllerStateBooting;
    CustomProductControllerStateSetup       m_CustomProductControllerStateSetup;
    CustomProductControllerStateOn          m_CustomProductControllerStateOn;
    ProductControllerStateLowPowerStandby   m_ProductControllerStateLowPowerStandby;
    ProductControllerSoftwareUpdating       m_ProductControllerStateSwUpdating;
    ProductControllerStateCriticalError     m_ProductControllerStateCriticalError;
    ProductControllerStateRebooting         m_ProductControllerStateRebooting;

    ProductControllerStatePlaying           m_ProductControllerStatePlaying;
    ProductControllerStatePlayable          m_ProductControllerStatePlayable;
    ProductControllerStateLowPowerStandbyTransition   m_ProductControllerStateLowPowerStandbyTransition;

    ProductControllerStatePlayingActive     m_ProductControllerStatePlayingActive;
    ProductControllerStatePlayingInactive   m_ProductControllerStatePlayingInactive;
    ProductControllerStateIdle              m_ProductControllerStateIdle;
    ProductControllerStateNetworkStandby    m_ProductControllerStateNetworkStandby;

    ProductControllerStateIdleVoiceConfigured   m_ProductControllerStateVoiceConfigured;
    ProductControllerStateIdleVoiceNotConfigured   m_ProductControllerStateVoiceNotConfigured;
    ProductControllerStateNetworkStandbyConfigured   m_ProductControllerStateNetworkConfigured;
    ProductControllerStateNetworkStandbyNotConfigured   m_ProductControllerStateNetworkNotConfigured;
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
    bool                                        m_isLpmReady  = false;
    bool                                        m_isNetworkModuleReady  = false;
    bool                                        m_isBLEModuleReady  = false;

    BOptional<int>                              m_wifiProfilesCount;
    AsyncCallback<FRONT_DOOR_CLIENT_ERRORS>     errorCb;
    //////////////////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief Interfaces to the ProductSTSController, which implements the interactions
    ///       between the Eddie Product Controller and the STS source proxies.
    ///
    //////////////////////////////////////////////////////////////////////////////////////////////
    bool                                        m_isSTSReady = false;
    bool m_IsAudioPathReady = true;
    ProductSTSController                        m_ProductSTSController;
    DataCollectionClient                        m_DataCollectionClient;
    VoiceServiceClient                          m_voiceServiceClient;

    /// Shared Pointer to the LPM Custom Hardware Interface
    std::shared_ptr< CustomProductLpmHardwareInterface > m_LpmInterface;
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
