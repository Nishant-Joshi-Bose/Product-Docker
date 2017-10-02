////////////////////////////////////////////////////////////////////////////////
/// @file   EddieProductController.h
/// @brief  Eddie Product controller class.
///
/// @attention Copyright 2017 Bose Corporation, Framingham, MA
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ProductController.h"
#include "EddieProductControllerHsm.h"
#include "FrontDoorClientIF.h"
#include "NotifyTargetTaskIF.h"
#include "ProtoPersistenceIF.h"
#include "EddieProductControllerStateTop.h"
#include "EddieProductControllerStateBooting.h"
#include "EddieProductControllerStateSetup.h"
#include "EddieProductControllerStateNetworkStandby.h"
#include "DeviceManager.h"
#include "ConfigurationStatus.pb.h"
#include "SoundTouchInterface/AllowSourceSelect.pb.h"
#include "Language.pb.h"
#include "DeviceManager.pb.h"
#include "SoundTouchInterface/CapsInitializationStatus.pb.h"
#include "ProductCliClient.h"
#include "LpmClientIF.h"
#include "KeyHandler.h"

namespace ProductApp
{
class EddieProductController : public ProductController
{
public:
    EddieProductController( std::string const& ProductName = "eddie" );
    virtual ~EddieProductController();

    void Initialize();

private:
    /// Disable copies
    EddieProductController( const EddieProductController& ) = delete;
    EddieProductController& operator=( const EddieProductController& ) = delete;

private:
    // Initialize and Register with LPM for events notifications
    void InitializeLpmClient();
    void RegisterLpmEvents();
    void RegisterKeyHandler();
    void RegisterEndPoints();
    void HandleCliCmd( uint16_t cmdKey,
                       const std::list<std::string> & argList,
                       AsyncCallback<std::string, int32_t> rspAndRspCmplt,
                       int32_t transact_id );
    void RegisterCliClientCmds() override;

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

public:
    // Handle Key Information received from LPM
    void HandleLpmKeyInformation( IpcKeyInformation_t keyInformation );

    static void KeyInformationCallBack( const int result, void *context );

///////////////////////////////////////////////////////////////////////////////
/// @name  SendAllowSourceSelectNotification
/// @brief function to send Send allowSourceSelectUpdate Notification Msg to the subscriber.
/// @return void
////////////////////////////////////////////////////////////////////////////////
    void SendAllowSourceSelectNotification( bool isSourceSelectAllowed );

///////////////////////////////////////////////////////////////////////////////
/// @name  IsAllModuleReady
/// @brief true if all the dependent modules are up and ready.
/// Modules like- LPM, CAPS, SW Update etc.
/// @return bool
////////////////////////////////////////////////////////////////////////////////
    bool IsAllModuleReady();

///////////////////////////////////////////////////////////////////////////////
/// @name  HandleLPMReady
/// @brief Function to call when LPM client is ready to send/receive request.
/// @return void
////////////////////////////////////////////////////////////////////////////////
    void HandleLPMReady();

///////////////////////////////////////////////////////////////////////////////
/// @name  HandleLPMReady
/// @brief Function to call when CAPS is ready to send/receive request.
/// @return void
////////////////////////////////////////////////////////////////////////////////
    void HandleCAPSReady( bool capsReady );

///////////////////////////////////////////////////////////////////////////////
/// @name  IsLanguageSet
/// @brief true if system language is initialized
/// @return bool
////////////////////////////////////////////////////////////////////////////////
    bool IsLanguageSet();

///////////////////////////////////////////////////////////////////////////////
/// @name  IsNetworkSetupDone
/// @brief true if system is conencted to ethernet or number of wifi profiles are nonzero
/// @return bool
////////////////////////////////////////////////////////////////////////////////
    bool IsNetworkSetupDone();

///////////////////////////////////////////////////////////////////////////////
/// @name  GetSystemLanguageCode
/// @brief returns system language code.
/// @return std::string
////////////////////////////////////////////////////////////////////////////////
    std::string GetSystemLanguageCode();

    void SendActivateAccessPointCmd();
    void SendDeActivateAccessPointCmd();
///////////////////////////////////////////////////////////////////////////////
/// @name  HandleGetLanguageRequest
/// @brief Handles GET request for "/system/language" endpoint.
/// @return void
////////////////////////////////////////////////////////////////////////////////
    void HandleGetLanguageRequest( const Callback<ProductPb::Language> &resp );

///////////////////////////////////////////////////////////////////////////////
/// @name  HandleGetLanguageRequest
/// @brief Handles POST request for "/system/language" endpoint.
/// @return void
////////////////////////////////////////////////////////////////////////////////
    void HandlePostLanguageRequest( const ProductPb::Language &lang, const Callback<ProductPb::Language> &resp );

///////////////////////////////////////////////////////////////////////////////
/// @name  HandleConfigurationStatusRequest
/// @brief "/system/configuration/status" endpoint request handler.
/// @return void
////////////////////////////////////////////////////////////////////////////////
    void HandleConfigurationStatusRequest( const Callback<ProductPb::ConfigurationStatus> &resp );

///////////////////////////////////////////////////////////////////////////////
/// @name  HandleDeviceInfoRequest
/// @brief "system/info" endpoint request handler.
/// @return void
////////////////////////////////////////////////////////////////////////////////
    void HandleGetDeviceInfoRequest( const Callback<::DeviceManager::Protobuf::DeviceInfo>& resp );

///////////////////////////////////////////////////////////////////////////////
/// @name  HandleGetDeviceStateRequest
/// @brief "system/state" endpoint request handler.
/// @return void
////////////////////////////////////////////////////////////////////////////////
    void HandleGetDeviceStateRequest( const Callback<::DeviceManager::Protobuf::DeviceState>& resp );

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
    void CapsInitializationStatusCallbackError( const FRONT_DOOR_CLIENT_ERRORS errorCode );
    void HandleAllowSourceSelectRequest( const Callback<SoundTouchInterface::AllowSourceSelect> &resp );

private:

    EddieProductControllerHsm                   m_EddieProductControllerHsm;

    EddieProductControllerStateTop              m_EddieProductControllerStateTop;
    EddieProductControllerStateBooting          m_EddieProductControllerStateBooting;
    EddieProductControllerStateSetup            m_EddieProductControllerStateSetup;
    EddieProductControllerStateNetworkStandby   m_EddieProductControllerStateNetworkStandby;

    // LPM Client handle
    LpmClientIF::LpmClientPtr                   m_LpmClient;

    // Key Handler
    KeyHandlerUtil::KeyHandler                  m_KeyHandler;

    ///Device manager instance
    DeviceManager                               m_deviceManager;

    ProtoPersistenceIF::ProtoPersistencePtr     m_ConfigurationStatusPersistence = nullptr;
    ProtoPersistenceIF::ProtoPersistencePtr     m_LanguagePersistence = nullptr;
    ProductPb::ConfigurationStatus              m_ConfigurationStatus;
    ProductPb::Language                         m_systemLanguage;

    std::shared_ptr<FrontDoorClientIF>          m_FrontDoorClientIF;
    ProductCliClient                            m_productCliClient;

    bool                                        m_isCapsReady = false;
    bool                                        m_isLPMReady  = true;
};
}
// namespace
