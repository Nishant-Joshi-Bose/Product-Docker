////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file      AccessorySoftwareInstallManager.h
///
/// @brief     This header file declares a AccessorySoftwareInstallManager class that is used to manage
///            Software Update for ProductController.
///
/// @attention Copyright (C) 2018 Bose Corporation All Rights Reserved
///
////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////
///
///            Included Header Files
///
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "ClockSyncClient.h"
#include "ProductSoftwareUpdate.pb.h"
#include "BOptional.h"
#include "APTimerFactory.h"
#include "CustomProductLpmHardwareInterface.h"
#include "SoftwareUpdateAPIMessages.pb.h"
#include "SystemUpdateStart.pb.h"
#include "ProductConstants.h"

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
class ProductSoftwareInstallScheduler;
class ProductSoftwareInstallManager;

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @class The AccessorySoftwareInstallManager Class
///
////////////////////////////////////////////////////////////////////////////////////////////////////
class AccessorySoftwareInstallManager
{
    friend class ProductCommandLine;

public:
    AccessorySoftwareInstallManager( NotifyTargetTaskIF*                                     task,
                                     ProductSoftwareInstallManager&                         productSoftwareInstallManager,
                                     ProductSoftwareInstallScheduler&                       productSoftwareInstallScheduler );

    void Initialize( std::shared_ptr< AsyncCallback<void> > callbackForInstall,
                     std::shared_ptr< CustomProductLpmHardwareInterface >&  productLpmHardwareInterface );

    void InitiateSoftwareInstall();

    bool IsSoftwareUpdatePending() const
    {
        return ( m_softwareStatusCache.status() == ACCESSORY_UPDATE_INSTALLATION_REQUIRED ||
                 m_softwareStatusCache.status() == ACCESSORY_UPDATE_INSTALLATION_PENDING );
    }

    bool IsSwUpdateForeground() const
    {
        return m_swUpdateForeground;
    }

    void Dump( std::ostringstream& oss ) const;

private:
    NotifyTargetTaskIF& GetTask() const
    {
        return *m_task;
    }

    const APTimerPtr& InstallSoftwareUpdateTimer() const
    {
        return m_TimerInstallSoftwareUpdate;
    }

    void HandleSpeakerSoftwareStatus( LpmServiceMessages::IpcAccessorySpeakerSoftwareStatusMessage_t softwareStatus );

    void RegisterLPMCallbacks();
    void ProceedWithSoftwareUpdate();

private:
    NotifyTargetTaskIF*                                    m_task;
    std::shared_ptr< CustomProductLpmHardwareInterface >   m_ProductLpmHardwareInterface;
    ProductSoftwareInstallManager&                         m_productSoftwareInstallManager;
    ProductSoftwareInstallScheduler&                       m_ProductSoftwareInstallScheduler;
    APTimerPtr                                             m_TimerInstallSoftwareUpdate;
    LpmServiceMessages::IpcAccessorySpeakerSoftwareStatusMessage_t m_softwareStatusCache;
    std::shared_ptr< AsyncCallback< void > >               m_ProductNotifyCallbackForInstall;
    uint32_t                                               m_reTryMaxCount               = 0;
    bool                                                   m_swUpdateForeground          = false;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
///                           End of the Product Application Namespace                           ///
////////////////////////////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         End of File                                          ///
////////////////////////////////////////////////////////////////////////////////////////////////////
