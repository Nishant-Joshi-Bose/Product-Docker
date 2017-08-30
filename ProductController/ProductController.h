////////////////////////////////////////////////////////////////////////////////
/// @file   ProductController.h
/// @brief  Generic Product controller class for Riviera based product
///
/// @attention Copyright 2017 Bose Corporation, Framingham, MA
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "FrontDoorClientIF.h"
#include "NotifyTargetTaskIF.h"
#include "ProtoPersistenceIF.h"
#include "ProductAppStateTop.h"
#include "ProductAppStateBooting.h"
#include "ProductAppStateStdOp.h"
#include "ProductAppStateSetup.h"
#include "ProductAppStateStandby.h"
#include "DeviceManager.h"
#include "ConfigurationStatus.pb.h"
#include "Language.pb.h"
#include "DeviceManager.pb.h"
#include "SoundTouchInterface/SystemService.pb.h"
#include "ProductCliClient.h"

namespace ProductApp
{
class ProductController
{
public:
    ProductController();
    virtual ~ProductController();
    inline NotifyTargetTaskIF* GetTask() const
    {
        return m_ProductControllerTask;
    }

    void Initialize();

private:
    /// Disable copies
    ProductController( const ProductController& ) = delete;
    ProductController& operator=( const ProductController& ) = delete;

private:
    void RegisterEndPoints();
///////////////////////////////////////////////////////////////////////////////
/// @name  ReadSystemLanguageFromPersistence
/// @brief Function to read persisted language code from /mnt/nv/product-persistence.
/// @return void
////////////////////////////////////////////////////////////////////////////////
    void ReadSystemLanguageFromPersistence();

///////////////////////////////////////////////////////////////////////////////
/// @name  PersistSystemLanguageCode
/// @brief Function to persist language code in /mnt/nv/product-persistence.
/// @return void
////////////////////////////////////////////////////////////////////////////////
    void PersistSystemLanguageCode();

public:
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
/// @name   HandleCapsInitializationUpdate
/// @brief- Handles CapsInitializationUpdate notification
/// @return void
///////////////////////////////////////////////////////////////////////////////
    void HandleCapsInitializationUpdate( const SoundTouchInterface::CapsInitializationUpdate &resp );
private:
    NotifyTargetTaskIF* m_ProductControllerTask;
    ProductAppHsm m_ProductAppHsm;
    ProtoPersistenceIF::ProtoPersistencePtr m_ConfigurationStatusPersistence = nullptr;
    ProtoPersistenceIF::ProtoPersistencePtr m_LanguagePersistence = nullptr;
    ProductPb::ConfigurationStatus m_ConfigurationStatus;
    ProductPb::Language m_systemLanguage;
    std::shared_ptr<FrontDoorClientIF> m_FrontDoorClientIF;
    ProductCliClient m_productCliClient;

    ProductAppStateTop m_ProductAppStateTop;
    ProductAppStateBooting m_ProductAppStateBooting;
    ProductAppStateStdOp m_ProductAppStateStdOp;
    ProductAppStateSetup m_ProductAppStateSetup;
    ProductAppStateStandby m_ProductAppStateStandby;

    ///Device manager instance
    DeviceManager               m_deviceManager;

    bool m_isCapsReady = false;
    bool m_isLPMReady  = true;
};
} // namespace
